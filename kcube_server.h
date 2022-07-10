#ifndef KCUBE_SERVER_H
#define KCUBE_SERVER_H
//============================================================================//
//                                                                            //
//                                                                            //
//                                                                            //
//============================================================================//
#include <stdint.h>
#include <stdbool.h>

// server limits
#define KCUBE_SERVER_MAX_DEVICES 16

// enum ======================================================================//
enum kcube_server_log_level {
	KCUBE_SERVER_LOG_LEVEL_off,
	KCUBE_SERVER_LOG_LEVEL_info,
	KCUBE_SERVER_LOG_LEVEL_debug,
};

// interface =================================================================//
// log
void kcube_server_set_log_level(enum kcube_server_log_level log_level);
// error
bool kcube_server_error(void);
// server start/stop
int kcube_server_start(void);
int kcube_server_stop(void);
// device open/close
int kcube_server_open(const char *serial_number);
void kcube_server_close(int device_index);
// device io
void kcube_server_set(int device_index, uint8_t *message_buffer);
uint8_t *kcube_server_request(int device_index, uint8_t *message_buffer);
// device update status
uint8_t *kcube_server_status(int device_index);

//============================================================================//
//                                                                            //
//                                                                            //
//                                                                            //
//============================================================================//
#endif

