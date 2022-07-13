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

import libkcube

kcube_dir = "C:\\cygwin64\\home\\Neurophos_Design\\kcube\\"
kcube = libkcube.import_library(kcube_dir)




# hardcoded values
id_usb_device = 0x50
kcube_message_max_length = 6 + 256
dest = id_usb_device
channel = 1;

# print kcube server version
print("info: kcube server v0.0");

# set logging mode
kcube.kcube_server_set_log_level(1);

# start kcube server
if kcube.kcube_server_start():
	print("error: failed to create kcube server thread");
	quit();

# open device connections
kpz101 = kcube.kcube_server_open(b"29251670");
ksg101 = kcube.kcube_server_open(b"59001225");
if kpz101 == -1 or ksg101 == -1:
	print("error: failed to open usb");
	quit();

# identify devices
msg = bytes(kcube_message_max_length);
kcube.mgmsg_mod_identify(msg, dest, channel);
kcube.kcube_server_set(kpz101, msg);
kcube.kcube_server_set(ksg101, msg);

# initalize kpz101
print("initalizing kpz101")
# mod_set_chanenablestate
kcube.mgmsg_mod_set_chanenablestate(msg, dest, channel, 0x02) # disable hv output
kcube.kcube_server_set(kpz101, msg)
# hw_stop_updatemsgs
kcube.mgmsg_hw_stop_updatemsgs(msg, dest, channel)
kcube.kcube_server_set(kpz101, msg)
# pz_set_poscontrolmode
kcube.mgmsg_pz_set_poscontrolmode(msg, dest, channel, 0x03) # open loop (smooth)
kcube.kcube_server_set(kpz101, msg)
# pz_set_outputvolts
kcube.mgmsg_pz_set_outputvolts(msg, dest, channel, 0x0000) # 0 V
kcube.kcube_server_set(kpz101, msg)
# pz_set_outputpos
kcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0x0000) # 0 um
kcube.kcube_server_set(kpz101, msg)
# pz_set_inputvoltssrc
kcube.mgmsg_pz_set_inputvoltssrc(msg, dest, channel, 0x00) # software on, ext off, pot off
kcube.kcube_server_set(kpz101, msg)
# pz_set_piconsts
kcube.mgmsg_pz_set_piconsts(msg, dest, channel, 100, 20) # prop, int control loop constants
kcube.kcube_server_set(kpz101, msg)
# pz_set_outputlut
kcube.mgmsg_pz_set_outputlut(msg, dest, channel, 0, 0)
kcube.kcube_server_set(kpz101, msg)
# pz_set_outputlutparams
kcube.mgmsg_pz_set_outputlutparams(msg, dest, channel, 0x0000, 0, 1, 0, 0, 0, 1, 1, 0) # output lut off
kcube.kcube_server_set(kpz101, msg)
# pz_stop_lutoutput
kcube.mgmsg_pz_stop_lutoutput(msg, dest, channel)
kcube.kcube_server_set(kpz101, msg)
# pz_set_eepromparams
#kcube.mgmsg_pz_set_eepromparams(msg, dest, channel, 0x0640)
#kcube.kcube_server_set(kpz101, msg)
# pz_set_tpz_dispsettings
kcube.mgmsg_pz_set_tpz_dispsettings(msg, dest, 26) # display intensity (? investigate more)
kcube.kcube_server_set(kpz101, msg)
# pz_set_tpz_iosettings
kcube.mgmsg_pz_set_tpz_iosettings(msg, dest, 0x01, 0x03) # 75 V limit, ext sma feedback
kcube.kcube_server_set(kpz101, msg)
# kpz_set_kcubemmiparams
kcube.mgmsg_kpz_set_kcubemmiparams(msg, dest, channel, 0x01, 0x01, 256, 0x00, 0x0000, 0x7FFF, 40, 1, 10)
kcube.kcube_server_set(kpz101, msg)
# kpz_set_kcubetrigioconfig
kcube.mgmsg_kpz_set_kcubetrigioconfig(msg, dest, channel, 0x00, 0x01, 0x00, 0x01) # disable triggers
kcube.kcube_server_set(kpz101, msg)

# initalize ksg101
print("initalizing ksg101")
# mod_set_chanenablestate
kcube.mgmsg_mod_set_chanenablestate(msg, dest, channel, 0x01)
kcube.kcube_server_set(ksg101, msg)
# hw_stop_updatemsgs
kcube.mgmsg_hw_stop_updatemsgs(msg, dest, channel)
kcube.kcube_server_set(ksg101, msg)
# pz_ack_pzstatusupdate
#kcube.mgmsg_pz_ack_pzstatusupdate(msg, dest)
#kcube.kcube_server_set(ksg101, msg)
# pz_set_eepromparams
#kcube.mgmsg_pz_set_eepromparams(msg, dest, channel, 0x07f9)
#kcube.kcube_server_set(ksg101, msg)
# pz_set_tpz_dispsettings
kcube.mgmsg_pz_set_tpz_dispsettings(msg, dest, channel, 26)
kcube.kcube_server_set(ksg101, msg)
# pz_set_tsg_iosettings
kcube.mgmsg_pz_set_tsg_iosettings(msg, dest, channel, 1, 1, 30000)
kcube.kcube_server_set(ksg101, msg)
# ksg_set_kcubemmiparams
kcube.mgmsg_ksg_set_kcubemmiparams(msg, dest, channel, 20, 5, 10)
kcube.kcube_server_set(ksg101, msg)
# ksg_set_kcubetrigioconfig
kcube.mgmsg_ksg_set_kcubetrigioconfig(msg, dest, channel, 0, 1, 0, 1, 0, 100, 100)
kcube.kcube_server_set(ksg101, msg)

# dump kpz101
print("dumping kpz101")
# mod_req_chanenablestate
kcube.mgmsg_mod_req_chanenablestate(msg, dest, channel)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)
# hw_req_info
kcube.mgmsg_hw_req_info(msg, dest, channel)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)
# pz_req_poscontrolmode
kcube.mgmsg_pz_req_poscontrolmode(msg, dest, channel)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)
# pz_req_outputvolts
kcube.mgmsg_pz_req_outputvolts(msg, dest, channel)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)
# pz_req_outputpos
kcube.mgmsg_pz_req_outputpos(msg, dest, channel)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)
# pz_req_inputvoltssrc
kcube.mgmsg_pz_req_inputvoltssrc(msg, dest, channel)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)
# pz_req_piconsts
kcube.mgmsg_pz_req_piconsts(msg, dest, channel)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)
# pz_req_outputlut_params
kcube.mgmsg_pz_req_outputlutparams(msg, dest, channel)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)
# pz_req_tpz_dispsettings
kcube.mgmsg_pz_req_tpz_dispsettings(msg, dest)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)
# pz_req_tpz_iosettings
kcube.mgmsg_pz_req_tpz_iosettings(msg, dest)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)
# kpz_req_kcubemmiparams
kcube.mgmsg_kpz_req_kcubemmiparams(msg, dest, channel)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)
# kpz_req_kcubetrigioconfig
kcube.mgmsg_kpz_req_kcubetrigioconfig(msg, dest, channel)
get = kcube.kcube_server_request(kpz101, msg)
kcube.kcube_message_dump(get)

# dump ksg101
print("dumping ksg101")
# mod_req_chanenablestate # HANGS!!
#kcube.mgmsg_mod_req_chanenablestate(msg, dest, channel)
#get = kcube.kcube_server_request(ksg101, msg)
#kcube.kcube_message_dump(get)
# hw_req_info
kcube.mgmsg_hw_req_info(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)
# hub_req_bayysed # HANGS !!
#kcube.mgmsg_hub_req_bayused(msg, dest)
#get = kcube.kcube_server_request(ksg101, msg)
#kcube.kcube_message_dump(get)
# pz_req_tpz_dispsettings
kcube.mgmsg_pz_req_tpz_dispsettings(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)
# pz_req_maxtravel
kcube.mgmsg_pz_req_maxtravel(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)
# pz_req_tsg_iosettings
kcube.mgmsg_pz_req_tsg_iosettings(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)
# pz_req_tsg_reading
#kcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
#get = kcube.kcube_server_request(ksg101, msg)
#kcube.kcube_message_dump(get)
# ksg_req_kcubemmiparams
kcube.mgmsg_ksg_req_kcubemmiparams(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)
# ksg_req_kcubetrigioconfig
kcube.mgmsg_ksg_req_kcubetrigioconfig(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)

# start updates
print("start pz updates")
kcube.mgmsg_hw_start_updatemsgs(msg, dest);
kcube.kcube_server_set(kpz101, msg);
kcube.kcube_server_set(ksg101, msg);
sleep(1)

# zero
print("zeroing pz stage")
# set kpz101 hv out to 0 volts
# pz_set_outputvolts
kcube.mgmsg_pz_set_outputvolts(msg, dest, channel, 0x0000) # 0 V
kcube.kcube_server_set(kpz101, msg)
# pz_set_poscontrolmode
kcube.mgmsg_pz_set_poscontrolmode(msg, dest, channel, 0x03) # open loop (smooth)
kcube.kcube_server_set(kpz101, msg)
# mod_set_channel_enablestate
kcube.mgmsg_mod_set_chanenablestate(msg, dest, channel, 0x01) # enable hv output
kcube.kcube_server_set(kpz101, msg)
# zero ksg101
# pz_set_zero
kcube.mgmsg_pz_set_zero(msg, dest, channel)
kcube.kcube_server_set(ksg101, msg)
# wait until zeroed
sleep(20)

# configure
print("configuring pz stage")
# set closed loop
# pz_set_outputpos
kcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0x0000) # 0 um
kcube.kcube_server_set(kpz101, msg)
# pz_set_poscontrolmode
kcube.mgmsg_pz_set_poscontrolmode(msg, dest, channel, 0x04) # closed loop (smooth)
kcube.kcube_server_set(kpz101, msg)
# mod_set_channel_enablestate
kcube.mgmsg_mod_set_chanenablestate(msg, dest, channel, 0x01) # enable hv output
kcube.kcube_server_set(kpz101, msg)

# move
print("move sequence")
kcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0x0000);
kcube.kcube_server_set(kpz101, msg);

kcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)

sleep(2);
kcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0x4000);
kcube.kcube_server_set(kpz101, msg);

kcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)

sleep(2);
kcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0x8000);
kcube.kcube_server_set(kpz101, msg);

kcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)

sleep(2);
kcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0xC000);
kcube.kcube_server_set(kpz101, msg);

kcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)

sleep(2);
kcube.mgmsg_pz_set_outputpos(msg, dest, channel, 0xFFFF);
kcube.kcube_server_set(kpz101, msg);

kcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)

sleep(2);

kcube.mgmsg_pz_req_tsg_reading(msg, dest, channel)
get = kcube.kcube_server_request(ksg101, msg)
kcube.kcube_message_dump(get)


# stop updates
print("stop pz updates")
kcube.mgmsg_hw_stop_updatemsgs(msg, dest);
kcube.kcube_server_set(kpz101, msg);
kcube.kcube_server_set(ksg101, msg);

# close device connections
kcube.kcube_server_close(kpz101);
kcube.kcube_server_close(ksg101);

# stop server
if kcube.kcube_server_stop():
	print("error: failed to stop kcube server");
	quit();

#==============================================================================#
#                                                                              #
#                                                                              #
#                                                                              #
#==============================================================================#

