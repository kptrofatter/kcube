//============================================================================//
//                                                                            //
//                                                                            //
//                                                                            //
//============================================================================//
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "kcube.h"

// driver ====================================================================//
int main(void)
{
	// print kcube server version
	fprintf(stderr, "info: kcube server v0.0\n");
	
	// set log level
	kcube_server_set_log_level(KCUBE_SERVER_LOG_LEVEL_info);
	
	// start kcube server
	if (kcube_server_start()) {
		fprintf(stderr, "error: failed to start kcube server\n");
		return EXIT_FAILURE;
	}
	
	// open device connections
	int kpz101 = kcube_server_open("29251670");
	int ksg101 = kcube_server_open("59001225");
	if (kpz101 == -1 || ksg101 == -1) {
		fprintf(stderr, "error: failed to open devices\n");
		return EXIT_FAILURE;
	}
	
	// test set message
	uint8_t msg[KCUBE_MESSAGE_MAX_SIZE];
	mgmsg_mod_identify(msg, KCUBE_USB_DEVICE, 1);
	kcube_server_set(kpz101, msg);
	kcube_server_set(ksg101, msg);
	
	// test request message
	uint8_t *get;
	get = (unsigned char*)"xxxxxx\0";
	mgmsg_hw_req_info(msg, KCUBE_USB_DEVICE);
	get = kcube_server_request(kpz101, msg);
	//kcube_message_dump(get);
	get = kcube_server_request(ksg101, msg);
	//kcube_message_dump(get);
	printf("%p\n", (void*)get);
	
	// start update messages
	fprintf(stderr, "start updates\n");
	mgmsg_hw_start_updatemsgs(msg, KCUBE_USB_DEVICE);
	kcube_server_set(kpz101, msg);
	sleep(2);
	
	// stop update messages
	fprintf(stderr, "stop updates\n");
	mgmsg_hw_stop_updatemsgs(msg, KCUBE_USB_DEVICE);
	kcube_server_set(kpz101, msg);
	sleep(2);
	
	// dump status
	fprintf(stderr, "final status\n");
	uint8_t *kpz101_status = kcube_server_status(kpz101);
	kcube_message_dump(kpz101_status);
	
	// test spontaneous messages
	// halt_and_catch_fire();
	
	// close device connections
	kcube_server_close(kpz101);
	kcube_server_close(ksg101);
	
	// stop server
	if (kcube_server_stop()) {
		fprintf(stderr, "error: failed to stop kcube server\n");
		return EXIT_FAILURE;
	}
	
	// done
	return EXIT_SUCCESS;
}

//============================================================================//
//                                                                            //
//                                                                            //
//                                                                            //
//============================================================================//

