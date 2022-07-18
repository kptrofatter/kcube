//============================================================================//
//                                                                            //
//                                                                            //
//                                                                            //
//============================================================================//
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <ftdi.h>
#include "kcube_server.h"
#include "kcube_message.h"

// ftdi usb ids
#define FTDI_VID 0x0403
#define FTDI_PID 0xfaf0

// data structures ===========================================================//
enum kcube_device_cmd {
	KCUBE_DEVICE_CMD_idle,
	KCUBE_DEVICE_CMD_set,
	KCUBE_DEVICE_CMD_request,
	KCUBE_DEVICE_CMD_get,
};

struct kcube_device {
	const char *serial_number;
	struct ftdi_context *ftdi;
	volatile enum kcube_device_cmd cmd;
	uint8_t rx_buffer[KCUBE_MESSAGE_MAX_SIZE];
	uint8_t status_buffer[KCUBE_MESSAGE_MAX_SIZE];
};

// kcube server
enum kcube_server_cmd {
	KCUBE_SERVER_CMD_idle,
	KCUBE_SERVER_CMD_open,
	KCUBE_SERVER_CMD_close,
	KCUBE_SERVER_CMD_status,
};

struct kcube_server {
	volatile bool is_running;
	volatile bool error_flag;
	volatile enum kcube_server_log_level log_level;
	volatile enum kcube_server_cmd cmd;
	const char *serial_number;
	volatile int device_index;
	uint8_t *tx_buffer;
	uint8_t rx_buffer[KCUBE_MESSAGE_MAX_SIZE];
	size_t n_devices;
	struct kcube_device *devices[KCUBE_SERVER_MAX_DEVICES];
};

// file variables ============================================================//
// kcube server
static struct kcube_server kcube_server;

// threading
static pthread_t kcube_server_thread;
static pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t sync_cond = PTHREAD_COND_INITIALIZER;

// timing
static const struct timespec pre_purge_dwell_time = {0, 50000};
static const struct timespec post_purge_dwell_time = {0, 50000};
static struct timespec sync_timeout;

// logging ===================================================================//
static bool should_log(enum kcube_server_log_level log_level)
{
	return kcube_server.log_level >= log_level;
}

// host-server synchronization ===============================================//
static void kcube_server_sync(void)
{
	pthread_mutex_lock(&sync_mutex);
	if (should_log(KCUBE_SERVER_LOG_LEVEL_debug)) fprintf(stderr, "debug: signal\n");
	pthread_cond_signal(&sync_cond);
	pthread_mutex_unlock(&sync_mutex);
}

// kcube server process commands =============================================//
static int kcube_server_process_open(const char *serial_number)
{
	// check for room in device pointer table
	if (kcube_server.n_devices >= KCUBE_SERVER_MAX_DEVICES) {
		fprintf(stderr, "error: max number of devices opened\n");
		kcube_server.error_flag = true;
		return -1;
	}
	
	// find free device structure pointer
	int i = 0;
	while (kcube_server.devices[i]) i++;
	
	// allocate device structure
	struct kcube_device *device = (struct kcube_device *) malloc(sizeof(struct kcube_device));
	if (device == NULL) {
		fprintf(stderr, "error: malloc() failed\n");
		kcube_server.error_flag = true;
		return -1;
	}
	
	// initalize members
	device->serial_number = serial_number;
	device->cmd = KCUBE_DEVICE_CMD_idle;
	memset(device->rx_buffer, 0, KCUBE_MESSAGE_MAX_SIZE);
	memset(device->status_buffer, 0, KCUBE_MESSAGE_MAX_SIZE);
	
	// initalize ftdi context
	struct ftdi_context *ftdi = ftdi_new();
	device->ftdi = ftdi;
	if (ftdi == NULL) {
		fprintf(stderr, "error: ftdi_new() failed\n");
		kcube_server.error_flag = true;
		free(device);
		return -1;
	}
	
	// open ftdi
	int status = ftdi_usb_open_desc(ftdi, FTDI_VID, FTDI_PID, NULL, serial_number);
	if (status) {
		fprintf(stderr, "error: ftdi_open() failed: %d (%s)\n", status, ftdi_get_error_string(ftdi));
		kcube_server.error_flag = true;
		ftdi_free(ftdi);
		free(device);
		return -1;
	}
	
	// configure ftdi for kcube
	// see www.thorlabs.com/Software/Motion%20Control/APT_Communications_Protocol.pdf pg32-33
	status |= ftdi_set_baudrate(ftdi, 115200);
	status |= ftdi_set_line_property(ftdi, BITS_8, STOP_BIT_1, NONE);
	nanosleep(&pre_purge_dwell_time, NULL);
	status |= ftdi_usb_purge_buffers(ftdi);
	nanosleep(&post_purge_dwell_time, NULL);
	status |= ftdi_usb_reset(ftdi);
	status |= ftdi_setflowctrl(ftdi, SIO_RTS_CTS_HS);
	status |= ftdi_setrts(ftdi, 1);
	if (status) {
		fprintf(stderr, "error: failed to configure ftdi-kcube communication\n");
		kcube_server.error_flag = true;
		ftdi_free(ftdi);
		free(device);
		return -1;
	}
	
	// add device pointer table entry
	kcube_server.devices[i] = device;
	kcube_server.n_devices++;
	return i;
}

static void kcube_server_process_close(int i)
{
	// get device
	struct kcube_device *device = kcube_server.devices[i];
	
	// close ftdi
	int status = ftdi_usb_close(device->ftdi);
	if (status) {
		fprintf(stderr, "error: ftdi_usb_close() failed\n");
		kcube_server.error_flag = true;
	}
	
	// free memory
	ftdi_free(device->ftdi);
	free(device);
	
	// nullify device pointer table entry
	kcube_server.devices[i] = NULL;
	kcube_server.n_devices--;
}

static void kcube_server_process_status(int i)
{
	// get device
	struct kcube_device *device = kcube_server.devices[i];
	
	// copy device status buffer to server rx buffer
	uint16_t n = kcube_message_decode_packet_size(device->status_buffer);
	memcpy(kcube_server.rx_buffer, device->status_buffer, KCUBE_MESSAGE_HEADER_SIZE + n);
}

static void kcube_server_process_commands(void)
{
	// open device
	if (kcube_server.cmd == KCUBE_SERVER_CMD_open) {
		if (should_log(KCUBE_SERVER_LOG_LEVEL_info)) fprintf(stderr, "info: kcube server opening device %s\n", kcube_server.serial_number);
		kcube_server.device_index = kcube_server_process_open(kcube_server.serial_number);
		kcube_server.cmd = KCUBE_SERVER_CMD_idle;
		kcube_server_sync();
	}
	
	// close device
	if (kcube_server.cmd == KCUBE_SERVER_CMD_close) {
		if (should_log(KCUBE_SERVER_LOG_LEVEL_info)) fprintf(stderr, "info: kcube server (%i) closing device\n", kcube_server.device_index);
		kcube_server_process_close(kcube_server.device_index);
		kcube_server.cmd = KCUBE_SERVER_CMD_idle;
		kcube_server_sync();
	}
	
	// device status
	if (kcube_server.cmd == KCUBE_SERVER_CMD_status) {
		if (should_log(KCUBE_SERVER_LOG_LEVEL_info)) fprintf(stderr, "info: kcube server (%i) getting device status\n", kcube_server.device_index);
		kcube_server_process_status(kcube_server.device_index);
		kcube_server.cmd = KCUBE_SERVER_CMD_idle;
		kcube_server_sync();
	}
}

// kcube server process device ===============================================//
static void kcube_server_tx(struct kcube_device *device)
{
	// debug
	if (should_log(KCUBE_SERVER_LOG_LEVEL_debug)) {
		fprintf(stderr, "debug: tx message (%i)\n", kcube_server.device_index);
		kcube_message_dump(kcube_server.tx_buffer);
	}
	// tx
	uint16_t packet_size = kcube_message_decode_packet_size(kcube_server.tx_buffer);
	int message_size = KCUBE_MESSAGE_HEADER_SIZE + packet_size;
	int result = ftdi_write_data(device->ftdi, kcube_server.tx_buffer, message_size);
	if (result != message_size) {
		fprintf(stderr, "error: tx failed\n");
		kcube_server.error_flag = true;
	}
}

static void kcube_server_rx_packet(struct kcube_device *device)
{
	uint16_t packet_size = kcube_message_decode_packet_size(device->rx_buffer);
	int result = ftdi_read_data(device->ftdi, device->rx_buffer + KCUBE_MESSAGE_HEADER_SIZE, packet_size);
	if (result < 0) {
		fprintf(stderr, "error: rx failed\n");
		kcube_server.error_flag = true;
	} else if (result != packet_size) {
		fprintf(stderr, "error: expected packet size 0x%04x, received %04x\n", packet_size, result);
		kcube_server.error_flag = true;
	}
}

static void kcube_server_process_device(int i)
{
	// skip inactive devices
	struct kcube_device *device = kcube_server.devices[i];
	if (device == NULL) return;
	
	// debug
	if (should_log(KCUBE_SERVER_LOG_LEVEL_debug)) fprintf(stderr, "debug: processing device[%i]\n", i);
	
	// process set messages
	if (device->cmd == KCUBE_DEVICE_CMD_set) {
		if (should_log(KCUBE_SERVER_LOG_LEVEL_info)) {
			uint16_t id = kcube_message_decode_id(kcube_server.tx_buffer);
			fprintf(stderr, "info: kcube server (%i) set message 0x%04x %s\n", i, id, kcube_message_id_string(id));
		}
		kcube_server_tx(device);
		device->cmd = KCUBE_DEVICE_CMD_idle;
		kcube_server_sync();
	}
	
	// process request messages
	if (device->cmd == KCUBE_DEVICE_CMD_request) {
		if (should_log(KCUBE_SERVER_LOG_LEVEL_info)) {
			uint16_t id = kcube_message_decode_id(kcube_server.tx_buffer);
			fprintf(stderr, "info: kcube server (%i) req message 0x%04x %s\n", i, id, kcube_message_id_string(id));
		}
		kcube_server_tx(device);
		device->cmd = KCUBE_DEVICE_CMD_get;
	}
	
	// process rx
	int result = ftdi_read_data(device->ftdi, device->rx_buffer, KCUBE_MESSAGE_HEADER_SIZE);
	if (result == KCUBE_MESSAGE_HEADER_SIZE) {
		
		// read incoming packet
		kcube_server_rx_packet(device);
		
		// debug
		if (should_log(KCUBE_SERVER_LOG_LEVEL_debug)) {
			fprintf(stderr, "debug: rx message (%i)\n", kcube_server.device_index);
			kcube_message_dump(device->rx_buffer);
		}

		// identify message type
		uint16_t rx_msg_id = kcube_message_decode_id(device->rx_buffer);
		if (device->cmd == KCUBE_DEVICE_CMD_get && rx_msg_id == 1 + kcube_message_decode_id(kcube_server.tx_buffer)) {
			// process get messages
			if (should_log(KCUBE_SERVER_LOG_LEVEL_info)) {
				fprintf(stderr, "info: kcube server (%i) get message 0x%04x %s\n", i, rx_msg_id, kcube_message_id_string(rx_msg_id));
			}
			memcpy(kcube_server.rx_buffer, device->rx_buffer, KCUBE_MESSAGE_HEADER_SIZE + kcube_message_decode_packet_size(device->rx_buffer));
			device->cmd = KCUBE_DEVICE_CMD_idle;
			kcube_server_sync();
			
		} else if(kcube_message_is_status(rx_msg_id)) {
			// process status messages
			if (should_log(KCUBE_SERVER_LOG_LEVEL_info)) {
				fprintf(stderr, "info: kcube server (%i) status message 0x%04x %s\n", i, rx_msg_id, kcube_message_id_string(rx_msg_id));
			}
			uint16_t n = kcube_message_decode_packet_size(device->rx_buffer);
			memcpy(device->status_buffer, device->rx_buffer, KCUBE_MESSAGE_HEADER_SIZE + n);
			
		} else if (kcube_message_is_spontaneous(rx_msg_id)) {
			// process spontaneous messages
			if (should_log(KCUBE_SERVER_LOG_LEVEL_info)) {
				fprintf(stderr, "info: kcube server (%i) spontaneous message 0x%04x %s\n", i, rx_msg_id, kcube_message_id_string(rx_msg_id));
			}
			kcube_message_dump(device->rx_buffer);
			
		} else {
			// unexpected message
			fprintf(stderr, "warning: kcube server (%i) unexpected rx message\n", i);
			kcube_message_dump(device->rx_buffer);
		}
		
	} else if (result < 0) {
		// read error (likely disconnected usb device)
		if (!kcube_server.error_flag) {
			fprintf(stderr, "error: rx failed\n");
			kcube_server.error_flag = true;
		}
		
	} else if (result != 0) {
		// unexpected header data
		fprintf(stderr, "warning: kcube server (%i) unexpected rx data\n", i);
		print_hex(device->rx_buffer, result);
	}
}

// kcube server ==============================================================//
static void *kcube_server_loop(void *arg)
{
	// start server
	if (should_log(KCUBE_SERVER_LOG_LEVEL_info)) fprintf(stderr, "info: kcube server starting\n");
	kcube_server.is_running = true;
	
	// loop
	while (kcube_server.is_running) {
		// process server commands
		kcube_server_process_commands();
		
		// process devices
		for (int i = 0; i < KCUBE_SERVER_MAX_DEVICES; i++) {
			kcube_server_process_device(i);
		}
	}
	
	// stop server
	if (should_log(KCUBE_SERVER_LOG_LEVEL_info)) fprintf(stderr, "info: kcube server stopping\n"); 
	
	// close any open devices
	for (int i = 0; i < KCUBE_SERVER_MAX_DEVICES; i++) {
		if (kcube_server.devices[i]) {
			kcube_server_process_close(i);
		}
	}
	
	// thread returns no data
	return NULL;
}

// host-server synchronization ===============================================//
static int wait_on_kcube_server(void)
{
	// lock condition mutex
	pthread_mutex_lock(&sync_mutex);
	
	// log wait
	if (should_log(KCUBE_SERVER_LOG_LEVEL_debug)) fprintf(stderr, "debug: wait\n");
	
	// compute timeout time
	clock_gettime(CLOCK_REALTIME, &sync_timeout);
	sync_timeout.tv_sec += 1;
	
	// wait for server signal
	int result = pthread_cond_timedwait(&sync_cond, &sync_mutex, &sync_timeout);
	
	// unlock condition mutex
	pthread_mutex_unlock(&sync_mutex);
	
	// timed out
	if (result == ETIMEDOUT) {
		fprintf(stderr, "error: timed out waiting for server\n");
		kcube_server.error_flag = true;
	}
	
	// return
	return result;
}

// sanitation ================================================================//
static bool kcube_server_is_device_index(int i)
{
	// check index
	if (i < 0 || i >= KCUBE_SERVER_MAX_DEVICES) {
		fprintf(stderr, "error: device index out of range\n");
		kcube_server.error_flag = true;
		return false;
	}
	
	// check device pointer
	if (kcube_server.devices[i] == NULL) {
		fprintf(stderr, "error: device index invalid\n");
		kcube_server.error_flag = true;
		return false;
	}
	
	// valid device index
	return true;
}

// interface =================================================================//
void kcube_server_set_log_level(enum kcube_server_log_level log_level)
{
	kcube_server.log_level = log_level;
}

bool kcube_server_error(void)
{
	bool e = kcube_server.error_flag;
	kcube_server.error_flag = false;
	return e;
}

int kcube_server_start(void)
{
	if (kcube_server.is_running) {
		fprintf(stderr, "warning: kcube server wasn't stopped\n");
		return 0;
	} else {
		return pthread_create(&kcube_server_thread, NULL, &kcube_server_loop, NULL);
	}
}

int kcube_server_stop(void)
{
	if (kcube_server.is_running) {
		kcube_server.is_running = false;
		return pthread_join(kcube_server_thread, NULL);
	} else {
		fprintf(stderr, "warning: kcube server wasn't started\n");
		return 0;
	}
}

int kcube_server_open(const char *serial_number)
{
	kcube_server.serial_number = serial_number;
	kcube_server.cmd = KCUBE_SERVER_CMD_open;
	if (wait_on_kcube_server() == ETIMEDOUT) {
		return -1;
	} else {
		return kcube_server.device_index;
	}
}

void kcube_server_close(int device_index)
{
	if (!kcube_server_is_device_index(device_index)) return;
	kcube_server.device_index = device_index;
	kcube_server.cmd = KCUBE_SERVER_CMD_close;
	wait_on_kcube_server();
}

uint8_t *kcube_server_status(int device_index)
{
	if (!kcube_server_is_device_index(device_index)) return NULL;
	kcube_server.device_index = device_index;
	kcube_server.cmd = KCUBE_SERVER_CMD_status;
	wait_on_kcube_server();
	return kcube_server.rx_buffer;
}

void kcube_server_set(int device_index, uint8_t *message_buffer)
{
	if (!kcube_server_is_device_index(device_index)) return;
	uint16_t id = kcube_message_decode_id(message_buffer);
	if (!kcube_message_is_set(id)) {
		fprintf(stderr, "error: not a set message\n");
		kcube_server.error_flag = true;
		return;
	} 
	kcube_server.device_index = device_index;
	kcube_server.tx_buffer = message_buffer;
	kcube_server.devices[device_index]->cmd = KCUBE_DEVICE_CMD_set;
	wait_on_kcube_server();
}

uint8_t *kcube_server_request(int device_index, uint8_t *message_buffer)
{
	if (!kcube_server_is_device_index(device_index)) return NULL;
	uint16_t id = kcube_message_decode_id(message_buffer);
	if (!kcube_message_is_request(id)) {
		fprintf(stderr, "error: not a request message\n");
		kcube_server.error_flag = true;
		return NULL;
	}
	kcube_server.device_index = device_index;
	kcube_server.tx_buffer = message_buffer;
	kcube_server.devices[device_index]->cmd = KCUBE_DEVICE_CMD_request;
	if (wait_on_kcube_server() == ETIMEDOUT) {
		kcube_server.devices[device_index]->cmd = KCUBE_DEVICE_CMD_idle;
		return NULL;
	} else {
		return kcube_server.rx_buffer;
	}
}

//============================================================================//
//                                                                            //
//                                                                            //
//                                                                            //
//============================================================================//

