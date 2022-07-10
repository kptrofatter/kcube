#==============================================================================#
#                                                                              #
#                                                                              #
#                                                                              #
#==============================================================================#
import ctypes
from ctypes import *
from time import sleep

# load shared library
libkcube = CDLL("/home/demob/projects/devices/kcube/libkcube.so")
# fix ctypes function signatures
libkcube.kcube_server_request.restype = ctypes.POINTER(ctypes.c_char)
libkcube.kcube_server_status.restype = ctypes.POINTER(ctypes.c_char)

# hardcoded values
id_usb_device = 0x50
kcube_message_max_length = 6 + 256

# print kcube server version
print("info: kcube server v0.0")

# set log level
libkcube.kcube_server_set_log_level(1)

# start kcube server
if libkcube.kcube_server_start():
	print("error: failed to start kcube server")
	quit()

# open device connections
kpz101 = libkcube.kcube_server_open(b"29251670")
ksg101 = libkcube.kcube_server_open(b"59001225")
if kpz101 == -1 or ksg101 == -1:
	print("error: failed to open devices")
	quit()

# test set message
msg = bytes(kcube_message_max_length)
libkcube.mgmsg_mod_identify(msg, id_usb_device, 1)
libkcube.kcube_server_set(kpz101, msg)
libkcube.kcube_server_set(ksg101, msg)

# test request message
libkcube.mgmsg_hw_req_info(msg, id_usb_device)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)

# start update messages
print("start updates")
libkcube.mgmsg_hw_start_updatemsgs(msg, id_usb_device)
libkcube.kcube_server_set(kpz101, msg)
sleep(2)

# stop update messages
print("stop updates")
libkcube.mgmsg_hw_stop_updatemsgs(msg, id_usb_device)
libkcube.kcube_server_set(kpz101, msg)
sleep(2)

# dump status
print("final status")
get = libkcube.kcube_server_status(kpz101)
libkcube.kcube_message_dump(get)

# test spontaneous messages
# halt_and_catch_fire();

# close device connections
libkcube.kcube_server_close(kpz101)
libkcube.kcube_server_close(ksg101)

# stop server
if libkcube.kcube_server_stop():
	print("error: failed to stop kcube server")
	quit()

#==============================================================================#
#                                                                              #
#                                                                              #
#                                                                              #
#==============================================================================#

