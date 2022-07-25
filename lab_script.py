#==============================================================================#
#                                                                              #
#                                                                              #
#                                                                              #
#==============================================================================#
import os
import sys
import ctypes
from ctypes import *
from time import sleep

import kcube

#kcube_dir = "C:\\cygwin64\\home\\Neurophos_Design\\kcube\\"
kcube_dir = "C:\\cygwin64\\home\\Neurophos\\kcube\\"
#kcube_dir = "/home/demob/projects/devices/kcube/"
libkcube = kcube.import_library(kcube_dir)




# hardcoded values
id_usb_device = 0x50
kcube_message_max_length = 6 + 256
dest = id_usb_device
channel = 1;

# print kcube server version
print("info: kcube server v0.0");

# set logging mode
libkcube.kcube_server_set_log_level(1);

# start kcube server
if libkcube.kcube_server_start():
	print("error: failed to create kcube server thread");
	quit();

# open device connections
kpz101 = libkcube.kcube_server_open(b"29251670");
ksg101 = libkcube.kcube_server_open(b"59001225");
if kpz101 == -1 or ksg101 == -1:
	print("error: failed to open usb");
	quit();

# identify devices
msg = cast(bytes(kcube_message_max_length), ctypes.POINTER(ctypes.c_uint8));
libkcube.mgmsg_mod_identify(msg, dest, channel);
libkcube.kcube_server_set(kpz101, msg);
libkcube.kcube_server_set(ksg101, msg);

# init kcubes
kcube.init_kpz101(libkcube, kpz101, id_usb_device, channel)
kcube.init_ksg101(libkcube, ksg101, id_usb_device, channel)

# dump kpz101
print("dumping kpz101")
# mod_req_chanenablestate
libkcube.mgmsg_mod_req_chanenablestate(msg, dest, channel)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
# hw_req_info
libkcube.mgmsg_hw_req_info(msg, dest, channel)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
# pz_req_poscontrolmode
libkcube.mgmsg_pz_req_poscontrolmode(msg, dest, channel)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
# pz_req_outputvolts
libkcube.mgmsg_pz_req_outputvolts(msg, dest, channel)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
# pz_req_outputpos
libkcube.mgmsg_pz_req_outputpos(msg, dest, channel)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
# pz_req_inputvoltssrc
libkcube.mgmsg_pz_req_inputvoltssrc(msg, dest, channel)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
# pz_req_piconsts
libkcube.mgmsg_pz_req_piconsts(msg, dest, channel)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
# pz_req_outputlut_params
libkcube.mgmsg_pz_req_outputlutparams(msg, dest, channel)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
# pz_req_tpz_dispsettings
libkcube.mgmsg_pz_req_tpz_dispsettings(msg, dest)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
# pz_req_tpz_iosettings
libkcube.mgmsg_pz_req_tpz_iosettings(msg, dest)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
# kpz_req_kcubemmiparams
libkcube.mgmsg_kpz_req_kcubemmiparams(msg, dest, channel)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)
# kpz_req_kcubetrigioconfig
libkcube.mgmsg_kpz_req_kcubetrigioconfig(msg, dest, channel)
get = libkcube.kcube_server_request(kpz101, msg)
libkcube.kcube_message_dump(get)

# dump ksg101
print("dumping ksg101")
# mod_req_chanenablestate # HANGS!!
#libkcube.mgmsg_mod_req_chanenablestate(msg, dest, channel)
#get = libkcube.kcube_server_request(ksg101, msg)
#libkcube.kcube_message_dump(get)
# hw_req_info
libkcube.mgmsg_hw_req_info(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)
# hub_req_bayysed # HANGS !!
#libkcube.mgmsg_hub_req_bayused(msg, dest)
#get = libkcube.kcube_server_request(ksg101, msg)
#libkcube.kcube_message_dump(get)
# pz_req_tpz_dispsettings
libkcube.mgmsg_pz_req_tpz_dispsettings(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)
# pz_req_maxtravel
libkcube.mgmsg_pz_req_maxtravel(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)
# pz_req_tsg_iosettings
libkcube.mgmsg_pz_req_tsg_iosettings(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)
# pz_req_tsg_reading
#libkcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
#get = libkcube.kcube_server_request(ksg101, msg)
#libkcube.kcube_message_dump(get)
# ksg_req_kcubemmiparams
libkcube.mgmsg_ksg_req_kcubemmiparams(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)
# ksg_req_kcubetrigioconfig
libkcube.mgmsg_ksg_req_kcubetrigioconfig(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)

# start updates
print("start pz updates")
libkcube.mgmsg_hw_start_updatemsgs(msg, dest);
libkcube.kcube_server_set(kpz101, msg);
libkcube.kcube_server_set(ksg101, msg);
sleep(1)

# zero
kcube.zero_pz_stage(libkcube, kpz101, ksg101, dest, channel, 20)

# configure
print("configuring pz stage")
# set closed loop
# pz_set_outputpos
libkcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0x0000) # 0 um
libkcube.kcube_server_set(kpz101, msg)
# pz_set_poscontrolmode
libkcube.mgmsg_pz_set_poscontrolmode(msg, dest, channel, 0x04) # closed loop (smooth)
libkcube.kcube_server_set(kpz101, msg)
# mod_set_channel_enablestate
libkcube.mgmsg_mod_set_chanenablestate(msg, dest, channel, 0x01) # enable hv output
libkcube.kcube_server_set(kpz101, msg)

# move
print("move sequence")
libkcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0x0000);
libkcube.kcube_server_set(kpz101, msg);

libkcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)

sleep(2);
libkcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0x4000);
libkcube.kcube_server_set(kpz101, msg);

libkcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)

sleep(2);
libkcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0x8000);
libkcube.kcube_server_set(kpz101, msg);

libkcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)

sleep(2);
libkcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0xC000);
libkcube.kcube_server_set(kpz101, msg);

libkcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)

sleep(2);
libkcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0xFFFF);
libkcube.kcube_server_set(kpz101, msg);

libkcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)

sleep(2);

libkcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = libkcube.kcube_server_request(ksg101, msg)
libkcube.kcube_message_dump(get)


# stop updates
print("stop pz updates")
libkcube.mgmsg_hw_stop_updatemsgs(msg, dest);
libkcube.kcube_server_set(kpz101, msg);
libkcube.kcube_server_set(ksg101, msg);

# close device connections
libkcube.kcube_server_close(kpz101);
libkcube.kcube_server_close(ksg101);

# stop server
if libkcube.kcube_server_stop():
	print("error: failed to stop kcube server");
	quit();

#==============================================================================#
#                                                                              #
#                                                                              #
#                                                                              #
#==============================================================================#

