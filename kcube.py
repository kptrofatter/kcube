#==============================================================================#
#                                                                              #
#                                                                              #
#                                                                              #
#==============================================================================#
import os
import sys
import ctypes
from ctypes import *

# limits
# kcube server
KCUBE_SERVER_MAX_DEVICES = 16
# kcube message
KCUBE_MESSAGE_HEADER_SIZE = 6
KCUBE_MESSAGE_PACKET_MAX_SIZE = 256
KCUBE_MESSAGE_MAX_SIZE = KCUBE_MESSAGE_HEADER_SIZE + KCUBE_MESSAGE_PACKET_MAX_SIZE

# enums
#kcube_server_log_level.off = 0
#kcube_server_log_level.info = 1
#kcube_server_log_level.debug = 2

# message addresses
KCUBE_HOST = 0x01
KCUBE_MOTHERBOARD = 0x11
KCUBE_USB_DEVICE = 0x50


def import_library(kcube_dir):
	
	def prepend_to_os_path(path):
		print("testing os path for '" + path + "'... ", end="")
		os_path = os.environ["PATH"]
		if os_path.find(path) == -1:
			print("fail")
			print("prepending to os path")
			os.environ["PATH"] = path + os.pathsep + os_path
		else:
			print("pass")
	
	# banner
	print("==============================")
	print("libkcube Python interface v1.0")
	print("==============================")
	
	# get system path information
	platform = sys.platform
	print("sys.platform == " + platform)
	if platform == 'linux':
		sys_dir = "/"
		bin_dir = sys_dir + "bin"
		kcube_lib_filename = "libkcube.so"
		
	elif platform == 'cygwin':
		sys_dir = "C:\\cygwin64\\"
		bin_dir = sys_dir + "bin"
		kcube_lib_filename = "cygkcube.dll"
		
	elif platform == 'win32':
		sys_dir = "C:\\cygwin64\\usr\\x86_64-w64-mingw32\\sys-root\\mingw\\"
		bin_dir = sys_dir + "bin\\" + ";C:\\cygwin64\\home\\Neurophos\\.x86_64-w64-mingw32\\bin"
		kcube_lib_filename = "libkcube.dll"
		
	else:
		print("error: unsupported platform")
		return -1
		
	# set os path (ensure 	kcube dependencies are found by system)
	prepend_to_os_path(bin_dir)
	
	# import 	kcube dynamic library
	kcube_lib_pathname = kcube_dir + kcube_lib_filename
	print("importing '" + kcube_lib_pathname + "'")
	kcube = ctypes.CDLL(kcube_lib_pathname)
	
	# interface function prototypes
	c_char_pp = ctypes.POINTER(c_char_p)
	c_uint8_p = ctypes.POINTER(c_uint8)
	c_uint8_pp = ctypes.POINTER(c_uint8_p)
	c_uint16_p = ctypes.POINTER(c_ushort)
	c_uint32_p = ctypes.POINTER(c_uint)
	c_int16_p = ctypes.POINTER(c_short)
	c_int32_p = ctypes.POINTER(c_int)
	
	# kcube_server.h ======================================================#
	kcube.kcube_server_set_log_level.argtypes = [c_int]
	kcube.kcube_server_set_log_level.restype = None
	kcube.kcube_server_error.argtypes = None
	kcube.kcube_server_error.restype = c_bool
	kcube.kcube_server_start.argtypes = None
	kcube.kcube_server_start.restype = c_int
	kcube.kcube_server_stop.argtypes = None
	kcube.kcube_server_stop.restype = c_int
	kcube.kcube_server_open.argtypes = [c_char_p]
	kcube.kcube_server_open.restype = c_int
	kcube.kcube_server_close.argtypes = [c_int]
	kcube.kcube_server_close.restype = None
	kcube.kcube_server_set.argtypes = [c_int, c_uint8_p]
	kcube.kcube_server_set.restype = None
	kcube.kcube_server_request.argtypes = [c_int, c_uint8_p]
	kcube.kcube_server_request.restype = c_uint8_p
	kcube.kcube_server_status.argtypes = [c_int]
	kcube.kcube_server_status.restype = c_uint8_p
	
	# kcube_message.h =====================================================#
	# decode header
	kcube.kcube_message_decode_id.argtypes = [c_uint8_p]
	kcube.kcube_message_decode_id.restype = c_uint16
	kcube.kcube_message_decode_param1.argtypes = [c_uint8_p]
	kcube.kcube_message_decode_param1.restype = c_uint8
	kcube.kcube_message_decode_param2.argtypes = [c_uint8_p]
	kcube.kcube_message_decode_param2.restype = c_uint8
	kcube.kcube_message_decode_packet_bit.argtypes = [c_uint8_p]
	kcube.kcube_message_decode_packet_bit.restype = c_bool
	kcube.kcube_message_decode_dest.argtypes = [c_uint8_p]
	kcube.kcube_message_decode_dest.restype = c_uint8
	kcube.kcube_message_decode_source.argtypes = [c_uint8_p]
	kcube.kcube_message_decode_source.restype = c_uint8
	kcube.kcube_message_decode_packet_size.argtypes = [c_uint8_p]
	kcube.kcube_message_decode_packet_size.restype = c_uint16
	
	# test
	kcube.kcube_message_is_set.argtypes = [c_uint16]
	kcube.kcube_message_is_set.restype = c_bool
	kcube.kcube_message_is_request.argtypes = [c_uint16]
	kcube.kcube_message_is_request.restype = c_bool
	kcube.kcube_message_is_get.argtypes = [c_uint16]
	kcube.kcube_message_is_get.restype = c_bool
	kcube.kcube_message_is_status.argtypes = [c_uint16]
	kcube.kcube_message_is_status.restype = c_bool
	kcube.kcube_message_is_spontaneous.argtypes = [c_uint16]
	kcube.kcube_message_is_spontaneous.restype = c_bool
	kcube.kcube_message_is_generic.argtypes = [c_uint16]
	kcube.kcube_message_is_generic.restype = c_bool
	kcube.kcube_message_is_kpz101.argtypes = [c_uint16]
	kcube.kcube_message_is_kpz101.restype = c_bool
	kcube.kcube_message_is_ksg101.argtypes = [c_uint16]
	kcube.kcube_message_is_ksg101.restype = c_bool
	
	# generic messages
	kcube.mgmsg_mod_identify.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_mod_identify.restype = None
	kcube.mgmsg_mod_set_chanenablestate.argtypes = [c_uint8_p, c_uint8, c_uint8, c_uint8]
	kcube.mgmsg_mod_set_chanenablestate.restype = None
	kcube.mgmsg_mod_req_chanenablestate.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_mod_req_chanenablestate.restype = None
	kcube.mgmsg_mod_get_chanenablestate.argtypes = [c_uint8_p, c_uint8_p, c_uint8_p]
	kcube.mgmsg_mod_get_chanenablestate.restype = None
	kcube.mgmsg_hw_disconnect.argtypes = [c_uint8_p, c_uint8]
	kcube.mgmsg_hw_disconnect.restype = None
	kcube.mgmsg_hw_response.argtypes = [c_uint8_p]
	kcube.mgmsg_hw_response.restype = None
	kcube.mgmsg_hw_richresponse.argtypes = [c_uint8_p, c_uint16_p, c_uint16_p, c_char_pp]
	kcube.mgmsg_hw_richresponse.restype = None
	kcube.mgmsg_hw_start_updatemsgs.argtypes = [c_uint8_p, c_uint8]
	kcube.mgmsg_hw_start_updatemsgs.restype = None
	kcube.mgmsg_hw_stop_updatemsgs.argtypes = [c_uint8_p, c_uint8]
	kcube.mgmsg_hw_stop_updatemsgs.restype = None
	kcube.mgmsg_hw_req_info.argtypes = [c_uint8_p, c_uint8]
	kcube.mgmsg_hw_req_info.restype = None
	kcube.mgmsg_hw_get_info.argtypes = [c_uint8_p, c_int32_p, c_char_pp, c_uint16_p, c_uint8_pp, c_uint16_p, c_uint16_p, c_uint16_p]
	kcube.mgmsg_hw_get_info.restype = None
	
	# kpz101 messages
	kcube.mgmsg_pz_set_poscontrolmode.argtypes = [c_uint8_p, c_uint8, c_uint8, c_uint8]
	kcube.mgmsg_pz_set_poscontrolmode.restype = None
	kcube.mgmsg_pz_req_poscontrolmode.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_pz_req_poscontrolmode.restype = None
	kcube.mgmsg_pz_get_poscontrolmode.argtypes = [c_uint8_p, c_uint8_p, c_uint8_p]
	kcube.mgmsg_pz_get_poscontrolmode.restype = None
	kcube.mgmsg_pz_set_outputvolts.argtypes = [c_uint8_p, c_uint8, c_uint16, c_int16]
	kcube.mgmsg_pz_set_outputvolts.restype = None
	kcube.mgmsg_pz_req_outputvolts.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_pz_req_outputvolts.restype = None
	kcube.mgmsg_pz_get_outputvolts.argtypes = [c_uint8_p, c_uint16_p, c_int16_p]
	kcube.mgmsg_pz_get_outputvolts.restype = None
	kcube.mgmsg_pz_set_inputvoltssrc.argtypes = [c_uint8_p, c_uint8, c_uint16, c_uint16]
	kcube.mgmsg_pz_set_inputvoltssrc.restype = None
	kcube.mgmsg_pz_req_inputvoltssrc.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_pz_req_inputvoltssrc.restype = None
	kcube.mgmsg_pz_get_inputvoltssrc.argtypes = [c_uint8_p, c_uint16_p, c_uint16_p]
	kcube.mgmsg_pz_get_inputvoltssrc.restype = None
	kcube.mgmsg_pz_set_piconsts.argtypes = [c_uint8_p, c_uint8, c_uint16, c_uint16, c_uint16]
	kcube.mgmsg_pz_set_piconsts.restype = None
	kcube.mgmsg_pz_req_piconsts.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_pz_req_piconsts.restype = None
	kcube.mgmsg_pz_get_piconsts.argtypes = [c_uint8_p, c_uint16_p, c_uint16_p, c_uint16_p]
	kcube.mgmsg_pz_get_piconsts.restype = None
	kcube.mgmsg_pz_get_pzstatusupdate.argtypes = [c_uint8_p, c_uint16_p, c_int16_p, c_uint16_p, c_uint32_p]
	kcube.mgmsg_pz_get_pzstatusupdate.restype = None
	kcube.mgmsg_pz_set_outputlut.argtypes = [c_uint8_p, c_uint8, c_uint16, c_uint16, c_int16]
	kcube.mgmsg_pz_set_outputlut.restype = None
	kcube.mgmsg_pz_set_outputlutparams.argtypes = [c_uint8_p, c_uint8, c_uint16, c_uint16, c_uint16, c_int32, c_int32, c_int32, c_int32, c_uint16, c_int32, c_uint16]
	kcube.mgmsg_pz_set_outputlutparams.restype = None
	kcube.mgmsg_pz_req_outputlutparams.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_pz_req_outputlutparams.restype = None
	kcube.mgmsg_pz_get_outputlutparams.argtypes = [c_uint8_p, c_uint16_p, c_uint16_p, c_uint16_p, c_int32_p, c_int32_p, c_int32_p, c_int32_p, c_uint16_p, c_int32_p, c_uint16_p]
	kcube.mgmsg_pz_get_outputlutparams.restype = None
	kcube.mgmsg_pz_start_lutoutput.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_pz_start_lutoutput.restype = None
	kcube.mgmsg_pz_stop_lutoutput.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_pz_stop_lutoutput.restype = None
	kcube.mgmsg_pz_set_eepromparams.argtypes = [c_uint8_p, c_uint8, c_uint16, c_uint16]
	kcube.mgmsg_pz_set_eepromparams.restype = None
	kcube.mgmsg_pz_set_tpz_dispsettings.argtypes = [c_uint8_p, c_uint8, c_uint16]
	kcube.mgmsg_pz_set_tpz_dispsettings.restype = None
	kcube.mgmsg_pz_req_tpz_dispsettings.argtypes = [c_uint8_p, c_uint8]
	kcube.mgmsg_pz_req_tpz_dispsettings.restype = None
	kcube.mgmsg_pz_get_tpz_dispsettings.argtypes = [c_uint8_p, c_uint16_p]
	kcube.mgmsg_pz_get_tpz_dispsettings.restype = None
	kcube.mgmsg_pz_set_tpz_iosettings.argtypes = [c_uint8_p, c_uint8, c_uint16, c_uint16]
	kcube.mgmsg_pz_set_tpz_iosettings.restype = None
	kcube.mgmsg_pz_req_tpz_iosettings.argtypes = [c_uint8_p, c_uint8]
	kcube.mgmsg_pz_req_tpz_iosettings.restype = None
	kcube.mgmsg_pz_get_tpz_iosettings.argtypes = [c_uint8_p, c_uint16_p, c_uint16_p]
	kcube.mgmsg_pz_get_tpz_iosettings.restype = None
	kcube.mgmsg_kpz_set_kcubemmiparams.argtypes = [c_uint8_p, c_uint8, c_uint16, c_uint16, c_uint16, c_int32, c_uint16, c_int32, c_int32, c_uint16, c_uint16, c_uint16]
	kcube.mgmsg_kpz_req_kcubemmiparams.restype = None
	kcube.mgmsg_kpz_req_kcubemmiparams.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_kpz_get_kcubemmiparams.restype = None
	kcube.mgmsg_kpz_get_kcubemmiparams.argtypes = [c_uint8_p, c_uint16_p, c_uint16_p, c_uint16_p, c_int32_p, c_int16_p, c_int32_p, c_int32_p, c_uint16_p, c_uint16_p, c_uint16_p]
	kcube.mgmsg_kpz_set_kcubemmiparams.restype = None
	kcube.mgmsg_kpz_set_kcubetrigioconfig.argtypes = [c_uint8_p, c_uint8, c_uint16, c_uint16, c_uint16, c_uint16, c_uint16]
	kcube.mgmsg_kpz_set_kcubetrigioconfig.restype = None
	kcube.mgmsg_kpz_req_kcubetrigioconfig.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_kpz_req_kcubetrigioconfig.restype = None
	kcube.mgmsg_kpz_get_kcubetrigioconfig.argtypes = [c_uint8_p, c_uint16_p, c_uint16_p, c_uint16_p, c_uint16_p]
	kcube.mgmsg_kpz_get_kcubetrigioconfig.restype = None
	
	# ksg101 messages
	kcube.mgmsg_hub_req_bayused.argtypes = [c_uint8_p, c_uint8]
	kcube.mgmsg_hub_req_bayused.restype = None
	kcube.mgmsg_hub_get_bayused.argtypes = [c_uint8_p, c_uint8_p]
	kcube.mgmsg_hub_get_bayused.restype = None
	kcube.mgmsg_pz_ack_pzstatusupdate.argtypes = [c_uint8_p, c_uint8]
	kcube.mgmsg_pz_ack_pzstatusupdate.restype = None
	kcube.mgmsg_pz_set_zero.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_pz_set_zero.restype = None
	kcube.mgmsg_pz_req_maxtravel.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_pz_req_maxtravel.restype = None
	kcube.mgmsg_pz_get_maxtravel.argtypes = [c_uint8_p, c_uint16_p, c_uint16_p]
	kcube.mgmsg_pz_get_maxtravel.restype = None
	kcube.mgmsg_pz_set_tsg_iosettings.argtypes = [c_uint8_p, c_uint8, c_uint16, c_uint16, c_uint16, c_int32]
	kcube.mgmsg_pz_set_tsg_iosettings.restype = None
	kcube.mgmsg_pz_req_tsg_iosettings.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_pz_req_tsg_iosettings.restype = None
	kcube.mgmsg_pz_get_tsg_iosettings.argtypes = [c_uint8_p, c_uint16_p, c_uint16_p, c_uint16_p, c_int32_p]
	kcube.mgmsg_pz_get_tsg_iosettings.restype = None
	kcube.mgmsg_pz_req_tsg_reading.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_pz_req_tsg_reading.restype = None
	kcube.mgmsg_pz_get_tsg_reading.argtypes = [c_uint8_p, c_uint16_p, c_int16_p, c_uint16_p]
	kcube.mgmsg_pz_get_tsg_reading.restype = None
	kcube.mgmsg_ksg_set_kcubemmiparams.argtypes = [c_uint8_p, c_uint8, c_uint16, c_uint16, c_uint16, c_uint16]
	kcube.mgmsg_ksg_set_kcubemmiparams.restype = None
	kcube.mgmsg_ksg_req_kcubemmiparams.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_ksg_req_kcubemmiparams.restype = None
	kcube.mgmsg_ksg_get_kcubemmiparams.argtypes = [c_uint8_p, c_uint16_p, c_uint16_p, c_uint16_p, c_uint16_p]
	kcube.mgmsg_ksg_get_kcubemmiparams.restype = None
	kcube.mgmsg_ksg_set_kcubetrigioconfig.argtypes = [c_uint8_p, c_uint8, c_uint16, c_uint16, c_uint16, c_uint16, c_uint16, c_int32, c_int32, c_uint16]
	kcube.mgmsg_ksg_set_kcubetrigioconfig.restype = None
	kcube.mgmsg_ksg_req_kcubetrigioconfig.argtypes = [c_uint8_p, c_uint8, c_uint8]
	kcube.mgmsg_ksg_req_kcubetrigioconfig.restype = None
	kcube.mgmsg_ksg_get_kcubetrigioconfig.argtypes = [c_uint8_p, c_uint16_p, c_uint16_p, c_uint16_p, c_uint16_p, c_uint16_p, c_int32_p, c_int32_p, c_uint16_p]
	kcube.mgmsg_ksg_get_kcubetrigioconfig.restype = None
	
	# print
	kcube.kcube_message_dump.argtypes = [c_uint8_p]
	kcube.kcube_message_dump.restype = None
	kcube.kcube_message_id_string.argtypes = [c_uint16]
	kcube.kcube_message_id_string.restype = c_char_p
	kcube.kcube_message_print_header.argtypes = [c_uint8_p]
	kcube.kcube_message_print_header.restype = None
	kcube.kcube_message_print_kpz101_pzstatus.argtypes = [c_uint8_p]
	kcube.kcube_message_print_kpz101_pzstatus.restype = None
	kcube.kcube_message_print_ksg101_pzstatus.argtypes = [c_uint8_p]
	kcube.kcube_message_print_ksg101_pzstatus.restype = None
	
	return kcube

#==============================================================================#
#                                                                              #
#                                                                              #
#                                                                              #
#==============================================================================#

