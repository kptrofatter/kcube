#==============================================================================#
#                                                                              #
#                                                                              #
#                                                                              #
#==============================================================================#
import ctypes
from ctypes import *
from time import sleep

# load shared library
libkcube = CDLL("/home/demob/projects/devices/kcube/libkcube.so");
# fix ctypes function signatures
libkcube.kcube_server_request.restype = ctypes.POINTER(ctypes.c_char);
libkcube.kcube_server_status.restype = ctypes.POINTER(ctypes.c_char);

# hardcoded values
id_usb_device = 0x50;
kcube_message_max_length = 6 + 256;
dest = id_usb_device;
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
msg = bytes(kcube_message_max_length);
libkcube.mgmsg_mod_identify(msg, dest, channel);
libkcube.kcube_server_set(kpz101, msg);
libkcube.kcube_server_set(ksg101, msg);

# initalize kpz101
print("initalizing kpz101")
# mod_set_chanenablestate
libkcube.mgmsg_mod_set_chanenablestate(msg, dest, channel, 0x02) # disable hv output
libkcube.kcube_server_set(kpz101, msg)
# hw_stop_updatemsgs
libkcube.mgmsg_hw_stop_updatemsgs(msg, dest, channel)
libkcube.kcube_server_set(kpz101, msg)
# pz_set_poscontrolmode
libkcube.mgmsg_pz_set_poscontrolmode(msg, dest, channel, 0x03) # open loop (smooth)
libkcube.kcube_server_set(kpz101, msg)
# pz_set_outputvolts
libkcube.mgmsg_pz_set_outputvolts(msg, dest, channel, 0x0000) # 0 V
libkcube.kcube_server_set(kpz101, msg)
# pz_set_outputpos
libkcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0x0000) # 0 um
libkcube.kcube_server_set(kpz101, msg)
# pz_set_inputvoltssrc
libkcube.mgmsg_pz_set_inputvoltssrc(msg, dest, channel, 0x00) # software on, ext off, pot off
libkcube.kcube_server_set(kpz101, msg)
# pz_set_piconsts
libkcube.mgmsg_pz_set_piconsts(msg, dest, channel, 100, 20) # prop, int control loop constants
libkcube.kcube_server_set(kpz101, msg)
# pz_set_outputlut
libkcube.mgmsg_pz_set_outputlut(msg, dest, channel, 0, 0)
libkcube.kcube_server_set(kpz101, msg)
# pz_set_outputlutparams
libkcube.mgmsg_pz_set_outputlutparams(msg, dest, channel, 0x0000, 0, 1, 0, 0, 0, 1, 1, 0) # output lut off
libkcube.kcube_server_set(kpz101, msg)
# pz_stop_lutoutput
libkcube.mgmsg_pz_stop_lutoutput(msg, dest, channel)
libkcube.kcube_server_set(kpz101, msg)
# pz_set_eepromparams
#libkcube.mgmsg_pz_set_eepromparams(msg, dest, channel, 0x0640)
#libkcube.kcube_server_set(kpz101, msg)
# pz_set_tpz_dispsettings
libkcube.mgmsg_pz_set_tpz_dispsettings(msg, dest, 26) # display intensity (? investigate more)
libkcube.kcube_server_set(kpz101, msg)
# pz_set_tpz_iosettings
libkcube.mgmsg_pz_set_tpz_iosettings(msg, dest, 0x01, 0x03) # 75 V limit, ext sma feedback
libkcube.kcube_server_set(kpz101, msg)
# kpz_set_kcubemmiparams
libkcube.mgmsg_kpz_set_kcubemmiparams(msg, dest, channel, 0x01, 0x01, 256, 0x00, 0x0000, 0x7FFF, 40, 1, 10)
libkcube.kcube_server_set(kpz101, msg)
# kpz_set_kcubetrigioconfig
libkcube.mgmsg_kpz_set_kcubetrigioconfig(msg, dest, channel, 0x00, 0x01, 0x00, 0x01) # disable triggers
libkcube.kcube_server_set(kpz101, msg)

# initalize ksg101
print("initalizing ksg101")
# mod_set_chanenablestate
libkcube.mgmsg_mod_set_chanenablestate(msg, dest, channel, 0x01)
libkcube.kcube_server_set(ksg101, msg)
# hw_stop_updatemsgs
libkcube.mgmsg_hw_stop_updatemsgs(msg, dest, channel)
libkcube.kcube_server_set(ksg101, msg)
# pz_ack_pzstatusupdate
#libkcube.mgmsg_pz_ack_pzstatusupdate(msg, dest)
#libkcube.kcube_server_set(ksg101, msg)
# pz_set_eepromparams
#libkcube.mgmsg_pz_set_eepromparams(msg, dest, channel, 0x07f9)
#libkcube.kcube_server_set(ksg101, msg)
# pz_set_tpz_dispsettings
libkcube.mgmsg_pz_set_tpz_dispsettings(msg, dest, channel, 26)
libkcube.kcube_server_set(ksg101, msg)
# pz_set_tsg_iosettings
libkcube.mgmsg_pz_set_tsg_iosettings(msg, dest, channel, 1, 1, 30000)
libkcube.kcube_server_set(ksg101, msg)
# ksg_set_kcubemmiparams
libkcube.mgmsg_ksg_set_kcubemmiparams(msg, dest, channel, 20, 5, 10)
libkcube.kcube_server_set(ksg101, msg)
# ksg_set_kcubetrigioconfig
libkcube.mgmsg_ksg_set_kcubetrigioconfig(msg, dest, channel, 0, 1, 0, 1, 0, 100, 100)
libkcube.kcube_server_set(ksg101, msg)

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
print("zeroing pz stage")
# set kpz101 hv out to 0 volts
# pz_set_outputvolts
libkcube.mgmsg_pz_set_outputvolts(msg, dest, channel, 0x0000) # 0 V
libkcube.kcube_server_set(kpz101, msg)
# pz_set_poscontrolmode
libkcube.mgmsg_pz_set_poscontrolmode(msg, dest, channel, 0x03) # open loop (smooth)
libkcube.kcube_server_set(kpz101, msg)
# mod_set_channel_enablestate
libkcube.mgmsg_mod_set_chanenablestate(msg, dest, channel, 0x01) # enable hv output
libkcube.kcube_server_set(kpz101, msg)
# zero ksg101
# pz_set_zero
libkcube.mgmsg_pz_set_zero(msg, dest, channel)
libkcube.kcube_server_set(ksg101, msg)
# wait until zeroed
sleep(20)

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

