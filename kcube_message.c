//============================================================================//
//                                                                            //
//                                                                            //
//                                                                            //
//============================================================================//
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "kcube_message.h"

// generic message ids
#define MGMSG_MOD_IDENTIFY 0x0223
#define MGMSG_MOD_SET_CHANENABLESTATE 0x0210
#define MGMSG_MOD_REQ_CHANENABLESTATE 0x0211
#define MGMSG_MOD_GET_CHANENABLESTATE 0x0212
#define MGMSG_HW_DISCONNECT 0x0002
#define MGMSG_HW_RESPONSE 0x0080
#define MGMSG_HW_RICHRESPONSE 0x0081
#define MGMSG_HW_START_UPDATEMSGS 0x0011
#define MGMSG_HW_STOP_UPDATEMSGS 0x0012
#define MGMSG_HW_REQ_INFO 0x0005
#define MGMSG_HW_GET_INFO 0x0006

// kpz101 message ids
#define MGMSG_PZ_SET_POSCONTROLMODE 0x0640
#define MGMSG_PZ_REQ_POSCONTROLMODE 0x0641
#define MGMSG_PZ_GET_POSCONTROLMODE 0x0642
#define MGMSG_PZ_SET_OUTPUTVOLTS 0x0643
#define MGMSG_PZ_REQ_OUTPUTVOLTS 0x0644
#define MGMSG_PZ_GET_OUTPUTVOLTS 0x0645
#define MGMSG_PZ_SET_OUTPUTPOS 0x0646
#define MGMSG_PZ_REQ_OUTPUTPOS 0x0647
#define MGMSG_PZ_GET_OUTPUTPOS 0x0648
#define MGMSG_PZ_SET_INPUTVOLTSSRC 0x0652
#define MGMSG_PZ_REQ_INPUTVOLTSSRC 0x0653
#define MGMSG_PZ_GET_INPUTVOLTSSRC 0x0654
#define MGMSG_PZ_SET_PICONSTS 0x0655
#define MGMSG_PZ_REQ_PICONSTS 0x0656
#define MGMSG_PZ_GET_PICONSTS 0x0657
#define MGMSG_PZ_GET_PZSTATUSUPDATE 0x0661
#define MGMSG_PZ_SET_OUTPUTLUT 0x0700
#define MGMSG_PZ_SET_OUTPUTLUTPARAMS 0x0703
#define MGMSG_PZ_REQ_OUTPUTLUTPARAMS 0x0704
#define MGMSG_PZ_GET_OUTPUTLUTPARAMS 0x0705
#define MGMSG_PZ_START_LUTOUTPUT 0x0706
#define MGMSG_PZ_STOP_LUTOUTPUT 0x0707
#define MGMSG_PZ_SET_EEPROMPARAMS 0x07D0
#define MGMSG_PZ_SET_TPZ_DISPSETTINGS 0x07D1
#define MGMSG_PZ_REQ_TPZ_DISPSETTINGS 0x07D2
#define MGMSG_PZ_GET_TPZ_DISPSETTINGS 0x07D3
#define MGMSG_PZ_SET_TPZ_IOSETTINGS 0x07D4
#define MGMSG_PZ_REQ_TPZ_IOSETTINGS 0x07D5
#define MGMSG_PZ_GET_TPZ_IOSETTINGS 0x07D6
#define MGMSG_KPZ_SET_KCUBEMMIPARAMS 0x07F0
#define MGMSG_KPZ_REQ_KCUBEMMIPARAMS 0x07F1
#define MGMSG_KPZ_GET_KCUBEMMIPARAMS 0x07F2
#define MGMSG_KPZ_SET_KCUBETRIGIOCONFIG 0x07F3
#define MGMSG_KPZ_REQ_KCUBETRIGIOCONFIG 0x07F4
#define MGMSG_KPZ_GET_KCUBETRIGIOCONFIG 0x07F5

// ksg101 message ids
#define MGMSG_HUB_REQ_BAYUSED 0x0065
#define MGMSG_HUB_GET_BAYUSED 0x0066
// #define MGMSG_PZ_GET_PZSTATUSUPDATE 0x0661 // shared
#define MGMSG_PZ_ACK_PZSTATUSUPDATE 0x0662
// #define MGMSG_PZ_SET_EEPROMPARAMS 0x07D0 // shared
// #define MGMSG_PZ_SET_TPZ_DISPSETTINGS 0x07D1 // shared
// #define MGMSG_PZ_REQ_TPZ_DISPSETTINGS 0x07D2 // shared
// #define MGMSG_PZ_GET_TPZ_DISPSETTINGS 0x07D3 // shared
#define MGMSG_PZ_SET_ZERO 0x0658
#define MGMSG_PZ_REQ_MAXTRAVEL 0x0650
#define MGMSG_PZ_GET_MAXTRAVEL 0x0651
#define MGMSG_PZ_SET_TSG_IOSETTINGS 0x07DA
#define MGMSG_PZ_REQ_TSG_IOSETTINGS 0x07DB
#define MGMSG_PZ_GET_TSG_IOSETTINGS 0x07DC
#define MGMSG_PZ_REQ_TSG_READING 0x07DD
#define MGMSG_PZ_GET_TSG_READING 0x07DE
#define MGMSG_KSG_SET_KCUBEMMIPARAMS 0x07F6
#define MGMSG_KSG_REQ_KCUBEMMIPARAMS 0x07F7
#define MGMSG_KSG_GET_KCUBEMMIPARAMS 0x07F8
#define MGMSG_KSG_SET_KCUBETRIGIOCONFIG 0x07F9
#define MGMSG_KSG_REQ_KCUBETRIGIOCONFIG 0x07FA
#define MGMSG_KSG_GET_KCUBETRIGIOCONFIG 0x07FB

// message macros
#define BYTE(data, byte) ((uint8_t) ((data) >> (8 * (byte))))
#define WRITE_UINT8(data) buffer[i++] = (uint8_t) (data);
#define WRITE_UINT16(data) for(int j = 0; j < 2; j++) buffer[i++] = BYTE((data), j);
#define WRITE_UINT32(data) for(int j = 0; j < 4; j++) buffer[i++] = BYTE((data), j);

#define READ_UINT16() decode_uint16(buffer + i); i += 2;
#define READ_UINT32() decode_uint32(buffer + i); i += 4;
#define READ_INT16() decode_int16(buffer + i); i += 2;
#define READ_INT32() decode_int32(buffer + i); i += 4;

// decode data ===============================================================//
static uint16_t decode_uint16(uint8_t *buffer)
{
	return (uint16_t) buffer[0] + ((uint16_t) buffer[1] << 8);
}

static uint32_t decode_uint32(uint8_t *buffer)
{
	return (uint32_t) buffer[0] + ((uint32_t) buffer[1] << 8) + ((uint32_t) buffer[2] << 16) + ((uint32_t) buffer[3] << 24);
}

static int16_t decode_int16(uint8_t *buffer)
{
	return (int16_t) decode_uint16(buffer);
}

static int32_t decode_int32(uint8_t *buffer)
{
	return (int32_t) decode_uint32(buffer);
}

// decode header =============================================================//
uint16_t kcube_message_decode_id(uint8_t *buffer)
{
	return decode_uint16(buffer + 0);
}

uint8_t kcube_message_decode_param1(uint8_t *buffer)
{
	return buffer[2];
}

uint8_t kcube_message_decode_param2(uint8_t *buffer)
{
	return buffer[3];
}

bool kcube_message_decode_packet_bit(uint8_t *buffer)
{
	return buffer[4] & 0x80;
}

uint8_t kcube_message_decode_dest(uint8_t *buffer)
{
	return buffer[4] & 0x7F;
}

uint8_t kcube_message_decode_source(uint8_t *buffer)
{
	return buffer[5];
}

uint16_t kcube_message_decode_packet_size(uint8_t *buffer)
{
	return kcube_message_decode_packet_bit(buffer)? decode_uint16(buffer + 2) & 0x00FF: 0; // no packet is over 256 bytes
}

// encode header =============================================================//
static void simple_header(uint8_t *buffer, int16_t cmd, uint8_t arg1, uint8_t arg2, uint8_t dest, uint8_t source)
{
	int i = 0;
	WRITE_UINT16(cmd);
	WRITE_UINT8(arg1);
	WRITE_UINT8(arg2);
	WRITE_UINT8(dest);
	WRITE_UINT8(source);
}

static void packet_header(uint8_t *buffer, int16_t cmd, uint16_t n_bytes, uint8_t dest, uint8_t source)
{
	int i = 0;
	WRITE_UINT16(cmd);
	WRITE_UINT16(n_bytes & 0x00FF); // no packet exceeds 256 bytes, so this is for safety
	WRITE_UINT8(dest | 0x80);
	WRITE_UINT8(source);
}

// test ======================================================================//
bool kcube_message_is_set(uint16_t id)
{
	return id == MGMSG_HW_DISCONNECT || id == MGMSG_HW_START_UPDATEMSGS
	|| id == MGMSG_HW_STOP_UPDATEMSGS || id == MGMSG_MOD_SET_CHANENABLESTATE
	|| id == MGMSG_MOD_IDENTIFY || id == MGMSG_PZ_SET_POSCONTROLMODE
	|| id == MGMSG_PZ_SET_OUTPUTVOLTS || id == MGMSG_PZ_SET_OUTPUTPOS
	|| id == MGMSG_PZ_SET_INPUTVOLTSSRC || id == MGMSG_PZ_SET_PICONSTS
	|| id == MGMSG_PZ_ACK_PZSTATUSUPDATE || id == MGMSG_PZ_SET_ZERO
	|| id == MGMSG_PZ_SET_OUTPUTLUT || id == MGMSG_PZ_SET_OUTPUTLUTPARAMS
	|| id == MGMSG_PZ_START_LUTOUTPUT || id == MGMSG_PZ_STOP_LUTOUTPUT
	|| id == MGMSG_PZ_SET_EEPROMPARAMS || id == MGMSG_PZ_SET_TPZ_DISPSETTINGS
	|| id == MGMSG_PZ_SET_TPZ_IOSETTINGS || id == MGMSG_PZ_SET_TSG_IOSETTINGS
	|| id == MGMSG_KPZ_SET_KCUBEMMIPARAMS || id == MGMSG_KPZ_SET_KCUBETRIGIOCONFIG
	|| id == MGMSG_KSG_SET_KCUBEMMIPARAMS || id == MGMSG_KSG_SET_KCUBETRIGIOCONFIG;
}

bool kcube_message_is_request(uint16_t id)
{
	return id == MGMSG_HW_REQ_INFO || id == MGMSG_HUB_REQ_BAYUSED
	|| id == MGMSG_MOD_REQ_CHANENABLESTATE || id == MGMSG_PZ_REQ_POSCONTROLMODE
	|| id == MGMSG_PZ_REQ_OUTPUTVOLTS || id == MGMSG_PZ_REQ_OUTPUTPOS
	|| id == MGMSG_PZ_REQ_MAXTRAVEL || id == MGMSG_PZ_REQ_INPUTVOLTSSRC
	|| id == MGMSG_PZ_REQ_PICONSTS || id == MGMSG_PZ_REQ_OUTPUTLUTPARAMS
	|| id == MGMSG_PZ_REQ_TPZ_DISPSETTINGS || id == MGMSG_PZ_REQ_TPZ_IOSETTINGS
	|| id == MGMSG_PZ_REQ_TSG_IOSETTINGS || id == MGMSG_PZ_REQ_TSG_READING
	|| id == MGMSG_KPZ_REQ_KCUBEMMIPARAMS || id ==MGMSG_KPZ_REQ_KCUBETRIGIOCONFIG
	|| id == MGMSG_KSG_REQ_KCUBEMMIPARAMS || id == MGMSG_KSG_REQ_KCUBETRIGIOCONFIG;
}

bool kcube_message_is_get(uint16_t id)
{
	return id == MGMSG_HW_DISCONNECT || id == MGMSG_HW_GET_INFO
	|| id == MGMSG_HW_RESPONSE || id == MGMSG_HW_RICHRESPONSE
	|| id == MGMSG_HUB_GET_BAYUSED || id == MGMSG_MOD_GET_CHANENABLESTATE
	|| id == MGMSG_PZ_GET_POSCONTROLMODE || id == MGMSG_PZ_GET_OUTPUTVOLTS
	|| id == MGMSG_PZ_GET_OUTPUTPOS || id == MGMSG_PZ_GET_MAXTRAVEL
	|| id == MGMSG_PZ_GET_INPUTVOLTSSRC || id == MGMSG_PZ_GET_PICONSTS
	|| id == MGMSG_PZ_GET_PZSTATUSUPDATE || id == MGMSG_PZ_GET_OUTPUTLUTPARAMS
	|| id == MGMSG_PZ_GET_TPZ_DISPSETTINGS || id == MGMSG_PZ_GET_TPZ_IOSETTINGS
	|| id == MGMSG_PZ_GET_TSG_IOSETTINGS || id == MGMSG_PZ_GET_TSG_READING
	|| id == MGMSG_KPZ_GET_KCUBEMMIPARAMS || id == MGMSG_KPZ_GET_KCUBETRIGIOCONFIG
	|| id == MGMSG_KSG_GET_KCUBEMMIPARAMS || id == MGMSG_KSG_GET_KCUBETRIGIOCONFIG;
}

bool kcube_message_is_status(uint16_t id)
{
	return id == MGMSG_PZ_GET_PZSTATUSUPDATE;
}

bool kcube_message_is_spontaneous(uint16_t id)
{
	return id == MGMSG_HW_DISCONNECT || id == MGMSG_HW_RESPONSE || id == MGMSG_HW_RICHRESPONSE;
}

bool kcube_message_is_generic(uint16_t id)
{
	return id == MGMSG_MOD_IDENTIFY || id == MGMSG_MOD_SET_CHANENABLESTATE
	|| id == MGMSG_MOD_REQ_CHANENABLESTATE || id == MGMSG_MOD_GET_CHANENABLESTATE
	|| id == MGMSG_HW_DISCONNECT || MGMSG_HW_RESPONSE
	|| id == MGMSG_HW_RICHRESPONSE || MGMSG_HW_START_UPDATEMSGS
	|| id == MGMSG_HW_STOP_UPDATEMSGS || MGMSG_HW_REQ_INFO
	|| id == MGMSG_HW_GET_INFO;
}

bool kcube_message_is_kpz101(uint16_t id)
{
	return kcube_message_is_generic(id) || id == MGMSG_PZ_SET_POSCONTROLMODE
	|| id == MGMSG_PZ_REQ_POSCONTROLMODE || id == MGMSG_PZ_GET_POSCONTROLMODE
	|| id == MGMSG_PZ_SET_OUTPUTVOLTS || id == MGMSG_PZ_REQ_OUTPUTVOLTS
	|| id == MGMSG_PZ_GET_OUTPUTVOLTS || id == MGMSG_PZ_SET_OUTPUTPOS
	|| id == MGMSG_PZ_REQ_OUTPUTPOS || id == MGMSG_PZ_GET_OUTPUTPOS
	|| id == MGMSG_PZ_SET_INPUTVOLTSSRC || id == MGMSG_PZ_REQ_INPUTVOLTSSRC
	|| id == MGMSG_PZ_GET_INPUTVOLTSSRC || id == MGMSG_PZ_SET_PICONSTS
	|| id == MGMSG_PZ_REQ_PICONSTS || id == MGMSG_PZ_GET_PICONSTS
	|| id == MGMSG_PZ_GET_PZSTATUSUPDATE || id == MGMSG_PZ_SET_OUTPUTLUT
	|| id == MGMSG_PZ_SET_OUTPUTLUTPARAMS || id == MGMSG_PZ_REQ_OUTPUTLUTPARAMS
	|| id == MGMSG_PZ_GET_OUTPUTLUTPARAMS || id == MGMSG_PZ_START_LUTOUTPUT
	|| id == MGMSG_PZ_STOP_LUTOUTPUT || id == MGMSG_PZ_SET_EEPROMPARAMS
	|| id == MGMSG_PZ_SET_TPZ_DISPSETTINGS || id == MGMSG_PZ_REQ_TPZ_DISPSETTINGS
	|| id == MGMSG_PZ_GET_TPZ_DISPSETTINGS || id == MGMSG_PZ_SET_TPZ_IOSETTINGS
	|| id == MGMSG_PZ_REQ_TPZ_IOSETTINGS || id == MGMSG_PZ_GET_TPZ_IOSETTINGS
	|| id == MGMSG_KPZ_SET_KCUBEMMIPARAMS || id == MGMSG_KPZ_REQ_KCUBEMMIPARAMS
	|| id == MGMSG_KPZ_GET_KCUBEMMIPARAMS || id == MGMSG_KPZ_SET_KCUBETRIGIOCONFIG
	|| id == MGMSG_KPZ_REQ_KCUBETRIGIOCONFIG || id == MGMSG_KPZ_GET_KCUBETRIGIOCONFIG;
}

bool kcube_message_is_ksg101(uint16_t id)
{
	return kcube_message_is_generic(id) || id == MGMSG_HUB_REQ_BAYUSED
	|| id == MGMSG_HUB_GET_BAYUSED || id == MGMSG_PZ_GET_PZSTATUSUPDATE
	|| id == MGMSG_PZ_ACK_PZSTATUSUPDATE || id == MGMSG_PZ_SET_EEPROMPARAMS
	|| id == MGMSG_PZ_SET_TPZ_DISPSETTINGS || id == MGMSG_PZ_REQ_TPZ_DISPSETTINGS
	|| id == MGMSG_PZ_GET_TPZ_DISPSETTINGS || id == MGMSG_PZ_SET_ZERO
	|| id == MGMSG_PZ_REQ_MAXTRAVEL || id == MGMSG_PZ_GET_MAXTRAVEL
	|| id == MGMSG_PZ_SET_TSG_IOSETTINGS || id == MGMSG_PZ_REQ_TSG_IOSETTINGS
	|| id == MGMSG_PZ_GET_TSG_IOSETTINGS || id == MGMSG_PZ_REQ_TSG_READING
	|| id == MGMSG_PZ_GET_TSG_READING || id == MGMSG_KSG_SET_KCUBEMMIPARAMS
	|| id == MGMSG_KSG_REQ_KCUBEMMIPARAMS || id == MGMSG_KSG_GET_KCUBEMMIPARAMS
	|| id == MGMSG_KSG_SET_KCUBETRIGIOCONFIG || id == MGMSG_KSG_REQ_KCUBETRIGIOCONFIG
	|| id == MGMSG_KSG_GET_KCUBETRIGIOCONFIG;
}

// encode generic messages ===================================================//
void mgmsg_mod_identify(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_MOD_IDENTIFY, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_mod_set_chanenablestate(uint8_t *buffer, uint8_t dest, uint8_t channel, uint8_t state)
{
	simple_header(buffer, MGMSG_MOD_SET_CHANENABLESTATE, channel, state, dest, KCUBE_HOST);
}

void mgmsg_mod_req_chanenablestate(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_MOD_REQ_CHANENABLESTATE, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_mod_get_chanenablestate(uint8_t *buffer, uint8_t *channel, uint8_t *state)
{
	*channel = kcube_message_decode_param1(buffer);
	*state = kcube_message_decode_param2(buffer);
}

void mgmsg_hw_disconnect(uint8_t *buffer, uint8_t dest)
{
	simple_header(buffer, MGMSG_HW_DISCONNECT, 0x00, 0x00, dest, KCUBE_HOST);
}

void mgmsg_hw_response(uint8_t *buffer)
{
	return; // documentation mentions device specfic error code. look for ThorlabsServer helpfile
}

void mgmsg_hw_richresponse(uint8_t *buffer, uint16_t *id, uint16_t *code, char **notes)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*id = READ_UINT16();
	*code = READ_UINT16();
	*notes = (char *) (buffer + i);
}

void mgmsg_hw_start_updatemsgs(uint8_t *buffer, uint8_t dest)
{
	simple_header(buffer, MGMSG_HW_START_UPDATEMSGS, 0x00, 0x00, dest, KCUBE_HOST);
}

void mgmsg_hw_stop_updatemsgs(uint8_t *buffer, uint8_t dest)
{
	simple_header(buffer, MGMSG_HW_STOP_UPDATEMSGS, 0x00, 0x00, dest, KCUBE_HOST);
}

void mgmsg_hw_req_info(uint8_t *buffer, uint8_t dest)
{
	simple_header(buffer, MGMSG_HW_REQ_INFO, 0x00, 0x00, dest, KCUBE_HOST);
}

void mgmsg_hw_get_info(uint8_t *buffer, int32_t *serial_number, char **model_number, uint16_t *type,
	uint8_t **fw_version, uint16_t *hw_version, uint16_t *mod_state, uint16_t *n_channels)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*serial_number = READ_INT32();
	*model_number = (char *) (buffer + i); i += 8; // 8 char c string
	*type = READ_UINT16();
	*fw_version = buffer + i; i += 4; // 4 byte array
	*hw_version = READ_UINT16();
	*mod_state = READ_UINT16();
	*n_channels = READ_UINT16();
}

// encode kpz101 messages ====================================================//
void mgmsg_pz_set_poscontrolmode(uint8_t *buffer, uint8_t dest, uint8_t channel, uint8_t mode)
{
	simple_header(buffer, MGMSG_PZ_SET_POSCONTROLMODE, channel, mode, dest, KCUBE_HOST);
}

void mgmsg_pz_req_poscontrolmode(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_REQ_POSCONTROLMODE, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_get_poscontrolmode(uint8_t *buffer, uint8_t *channel, uint8_t *mode)
{
	*channel = kcube_message_decode_param1(buffer);
	*mode = kcube_message_decode_param2(buffer);
}

void mgmsg_pz_set_outputvolts(uint8_t *buffer, uint8_t dest, uint16_t channel, int16_t voltage)
{
	packet_header(buffer, MGMSG_PZ_SET_OUTPUTVOLTS, 0x0004, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(voltage);
}

void mgmsg_pz_req_outputvolts(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_REQ_OUTPUTVOLTS, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_get_outputvolts(uint8_t *buffer, uint16_t *channel, int16_t *voltage)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*voltage = READ_INT16();
}

void mgmsg_pz_set_outputpos(uint8_t *buffer, uint8_t dest, uint16_t channel, uint16_t position)
{
	packet_header(buffer, MGMSG_PZ_SET_OUTPUTPOS, 0x0004, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(position);
}

void mgmsg_pz_req_outputpos(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_REQ_OUTPUTPOS, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_get_outputpos(uint8_t *buffer, uint16_t *channel, uint16_t *position)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*position = READ_UINT16();
}

void mgmsg_pz_set_inputvoltssrc(uint8_t *buffer, uint8_t dest, uint16_t channel, uint16_t voltsrc)
{
	packet_header(buffer, MGMSG_PZ_SET_INPUTVOLTSSRC, 0x0004, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(voltsrc);
}

void mgmsg_pz_req_inputvoltssrc(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_REQ_INPUTVOLTSSRC, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_get_inputvoltssrc(uint8_t *buffer, uint16_t *channel, uint16_t *voltsrc)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*voltsrc = READ_UINT16();
}

void mgmsg_pz_set_piconsts(uint8_t *buffer, uint8_t dest, uint16_t channel, uint16_t propconst, uint16_t intconst)
{
	packet_header(buffer, MGMSG_PZ_SET_PICONSTS, 0x0006, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(propconst);
	WRITE_UINT16(intconst);
}

void mgmsg_pz_req_piconsts(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_REQ_PICONSTS, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_get_piconsts(uint8_t *buffer, uint16_t *channel, uint16_t *propconst, uint16_t *intconst)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*propconst = READ_UINT16();
	*intconst = READ_UINT16();
}

void mgmsg_pz_get_pzstatusupdate(uint8_t *buffer, uint16_t *channel, int16_t *opvoltage, uint16_t *position, uint32_t *status)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*opvoltage = READ_INT16();
	*position = READ_UINT16(); // documentation says this is a int16_t, but really it's uint16_t
	*status = READ_UINT32();
}

void mgmsg_pz_set_outputlut(uint8_t *buffer, uint8_t dest, uint16_t channel, uint16_t index, int16_t output)
{
	packet_header(buffer, MGMSG_PZ_SET_OUTPUTLUT, 0x0006, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(index);
	WRITE_UINT16(output);
}

void mgmsg_pz_set_outputlutparams(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t mode, uint16_t cycle_length, int32_t num_cycles, int32_t delay_time,
	int32_t pre_cycle_rest, int32_t post_cycle_rest, uint16_t op_trig_start, int32_t op_trig_width, uint16_t trig_rep_cycle)
{
	packet_header(buffer, MGMSG_PZ_SET_OUTPUTLUTPARAMS, 0x001E, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(mode);
	WRITE_UINT16(cycle_length);
	WRITE_UINT32(num_cycles);
	WRITE_UINT32(delay_time);
	WRITE_UINT32(pre_cycle_rest);
	WRITE_UINT32(post_cycle_rest);
	WRITE_UINT16(op_trig_start);
	WRITE_UINT32(op_trig_width);
	WRITE_UINT16(trig_rep_cycle);
}

void mgmsg_pz_req_outputlutparams(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_REQ_OUTPUTLUTPARAMS, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_get_outputlutparams(uint8_t *buffer, uint16_t *channel,
	uint16_t *mode, uint16_t *cycle_length, int32_t *num_cycles, int32_t *delay_time,
	int32_t *pre_cycle_rest, int32_t *post_cycle_rest, uint16_t *op_trig_start, int32_t *op_trig_width, uint16_t *trig_rep_cycle)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*mode = READ_UINT16();
	*cycle_length = READ_UINT16();
	*num_cycles = READ_INT32();
	*delay_time = READ_INT32();
	*pre_cycle_rest = READ_INT32();
	*post_cycle_rest = READ_INT32();
	*op_trig_start = READ_UINT16();
	*op_trig_width = READ_INT32();
	*trig_rep_cycle = READ_UINT16();
}

void mgmsg_pz_start_lutoutput(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_START_LUTOUTPUT, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_stop_lutoutput(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_STOP_LUTOUTPUT, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_set_eepromparams(uint8_t *buffer, uint8_t dest, uint16_t channel, uint16_t id)
{
	packet_header(buffer, MGMSG_PZ_SET_EEPROMPARAMS, 0x0004, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(id);
}

void mgmsg_pz_set_tpz_dispsettings(uint8_t *buffer, uint8_t dest, uint16_t intensity)
{
	packet_header(buffer, MGMSG_PZ_SET_TPZ_DISPSETTINGS, 0x0002, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(intensity);
}

void mgmsg_pz_req_tpz_dispsettings(uint8_t *buffer, uint8_t dest)
{
	simple_header(buffer, MGMSG_PZ_REQ_TPZ_DISPSETTINGS, 0x01, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_get_tpz_dispsettings(uint8_t *buffer, uint16_t *intensity)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*intensity = READ_UINT16();
}

void mgmsg_pz_set_tpz_iosettings(uint8_t *buffer, uint8_t dest, uint16_t limit, uint16_t input)
{
	packet_header(buffer, MGMSG_PZ_SET_TPZ_IOSETTINGS, 0x000A, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(1);
	WRITE_UINT16(limit);
	WRITE_UINT16(input);
	WRITE_UINT16(0);
	WRITE_UINT16(0);
}

void mgmsg_pz_req_tpz_iosettings(uint8_t *buffer, uint8_t dest)
{
	simple_header(buffer, MGMSG_PZ_REQ_TPZ_IOSETTINGS, 0x01, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_get_tpz_iosettings(uint8_t *buffer, uint16_t *limit, uint16_t *input)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE + 2;
	*limit = READ_UINT16();
	*input = READ_UINT16();
}

void mgmsg_kpz_set_kcubemmiparams(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t mode, uint16_t gearbox, int32_t step, uint16_t sense,
	int32_t preset_volt_1, int32_t preset_volt_2, uint16_t intensity, uint16_t timeout, uint16_t dimness)
{
	packet_header(buffer, MGMSG_KPZ_SET_KCUBEMMIPARAMS, 0x0022, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(mode);
	WRITE_UINT16(gearbox);
	WRITE_UINT32(step);
	WRITE_UINT16(sense);
	WRITE_UINT32(preset_volt_1);
	WRITE_UINT32(preset_volt_2);
	WRITE_UINT16(intensity);
	WRITE_UINT16(timeout);
	WRITE_UINT16(dimness);
	WRITE_UINT16(0);
	WRITE_UINT16(0);
	WRITE_UINT16(0);
	WRITE_UINT16(0);
}

void mgmsg_kpz_req_kcubemmiparams(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_KPZ_REQ_KCUBEMMIPARAMS, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_kpz_get_kcubemmiparams(uint8_t *buffer, uint16_t *channel,
	uint16_t *mode, uint16_t *gearbox, int32_t *step, uint16_t *sense,
	int32_t *preset_volt_1, int32_t *preset_volt_2, uint16_t *intensity, uint16_t *timeout, uint16_t *dimness)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*mode = READ_UINT16();
	*gearbox = READ_UINT16();
	*step = READ_INT32();
	*sense = READ_UINT16();
	*preset_volt_1 = READ_INT32();
	*preset_volt_2 = READ_INT32();
	*intensity = READ_UINT16();
	*timeout = READ_UINT16();
	*dimness = READ_UINT16();
}

void mgmsg_kpz_set_kcubetrigioconfig(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t mode_1, uint16_t polarity_1, uint16_t mode_2, uint16_t polarity_2)
{
	packet_header(buffer, MGMSG_KPZ_SET_KCUBETRIGIOCONFIG, 0x0016, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(mode_1);
	WRITE_UINT16(polarity_1);
	WRITE_UINT16(mode_2);
	WRITE_UINT16(polarity_2);
	WRITE_UINT16(0);
	WRITE_UINT16(0);
	WRITE_UINT16(0);
	WRITE_UINT16(0);
	WRITE_UINT16(0);
	WRITE_UINT16(0);
}

void mgmsg_kpz_req_kcubetrigioconfig(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_KPZ_REQ_KCUBETRIGIOCONFIG, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_kpz_get_kcubetrigioconfig(uint8_t *buffer, uint16_t *channel,
	uint16_t *mode_1, uint16_t *polarity_1, uint16_t *mode_2, uint16_t *polarity_2)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*mode_1 = READ_UINT16();
	*polarity_1 = READ_UINT16();
	*mode_2 = READ_UINT16();
	*polarity_2 = READ_UINT16();
}

// encode ksg101 messages ====================================================//
void mgmsg_hub_req_bayused(uint8_t *buffer, uint8_t dest)
{
	simple_header(buffer, MGMSG_HUB_REQ_BAYUSED, 0x00, 0x00, dest, KCUBE_HOST);
}

void mgmsg_hub_get_bayused(uint8_t *buffer, uint8_t *bay)
{
	*bay = kcube_message_decode_param1(buffer);
}

void mgmsg_pz_ack_pzstatusupdate(uint8_t *buffer, uint8_t dest)
{
	simple_header(buffer, MGMSG_PZ_ACK_PZSTATUSUPDATE, 0x00, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_set_zero(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_SET_ZERO, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_req_maxtravel(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_REQ_MAXTRAVEL, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_get_maxtravel(uint8_t *buffer, uint16_t *channel, uint16_t *travel)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*travel = READ_UINT16();
}

void mgmsg_pz_set_tsg_iosettings(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t hub_analog_op, uint16_t display_mode, int32_t force_calib)
{
	packet_header(buffer, MGMSG_PZ_SET_TSG_IOSETTINGS, 0x000E, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(hub_analog_op);
	WRITE_UINT16(display_mode);
	WRITE_UINT32(force_calib);
	WRITE_UINT16(0);
}

void mgmsg_pz_req_tsg_iosettings(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_REQ_TSG_IOSETTINGS, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_get_tsg_iosettings(uint8_t *buffer, uint16_t *channel,
	uint16_t *hub_analog_op, uint16_t *display_mode, int32_t *force_calib)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*hub_analog_op = READ_UINT16();
	*display_mode = READ_UINT16();
	*force_calib = READ_INT32();
}

void mgmsg_pz_req_tsg_reading(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_PZ_REQ_TSG_READING, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_pz_get_tsg_reading(uint8_t *buffer, uint16_t *channel, int16_t *reading, uint16_t *smoothed)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*reading = READ_INT16();
	*smoothed = READ_UINT16();
}

void mgmsg_ksg_set_kcubemmiparams(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t intensity, uint16_t timeout, uint16_t dimness)
{
	packet_header(buffer, MGMSG_KSG_SET_KCUBEMMIPARAMS, 0x0008, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(intensity);
	WRITE_UINT16(timeout);
	WRITE_UINT16(dimness);
}

void mgmsg_ksg_req_kcubemmiparams(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_KSG_REQ_KCUBEMMIPARAMS, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_ksg_get_kcubemmiparams(uint8_t *buffer, uint16_t *channel,
	uint16_t *intensity, uint16_t *timeout, uint16_t *dimness)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*intensity = READ_UINT16();
	*timeout = READ_UINT16();
	*dimness = READ_UINT16();
}

void mgmsg_ksg_set_kcubetrigioconfig(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t mode_1, uint16_t polarity_1, uint16_t mode_2, uint16_t polarity_2,
	int32_t lower_lim, int32_t upper_lim, uint16_t smoothing_samples)
{
	packet_header(buffer, MGMSG_KSG_SET_KCUBETRIGIOCONFIG, 0x0016, dest, KCUBE_HOST);
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	WRITE_UINT16(channel);
	WRITE_UINT16(mode_1);
	WRITE_UINT16(polarity_1);
	WRITE_UINT16(mode_2);
	WRITE_UINT16(polarity_2);
	WRITE_UINT32(lower_lim);
	WRITE_UINT32(upper_lim);
	WRITE_UINT16(smoothing_samples);
	WRITE_UINT16(0);
}

void mgmsg_ksg_req_kcubetrigioconfig(uint8_t *buffer, uint8_t dest, uint8_t channel)
{
	simple_header(buffer, MGMSG_KSG_REQ_KCUBETRIGIOCONFIG, channel, 0x00, dest, KCUBE_HOST);
}

void mgmsg_ksg_get_kcubetrigioconfig(uint8_t *buffer, uint16_t *channel,
	uint16_t *mode_1, uint16_t *polarity_1, uint16_t *mode_2, uint16_t *polarity_2,
	int32_t *lower_lim, int32_t *upper_lim, uint16_t *smoothing_samples)
{
	int i = KCUBE_MESSAGE_HEADER_SIZE;
	*channel = READ_UINT16();
	*mode_1 = READ_UINT16();
	*polarity_1 = READ_UINT16();
	*mode_2 = READ_UINT16();
	*polarity_2 = READ_UINT16();
	*lower_lim = READ_INT32();
	*upper_lim = READ_INT32();
	*smoothing_samples = READ_UINT16();
}

// test message ==============================================================//
bool kcube_message_is_pz_zeroing(uint32_t status)
{
	return status & 0x00000020;
}

// print =====================================================================//
void print_hex(uint8_t *buffer, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		fprintf(stderr, " %02x", buffer[i]);
		if (i % 16 == 15 || i == size - 1) fprintf(stderr, "\n");
	}
}

void kcube_message_dump(uint8_t *buffer)
{
	// dump header
	print_hex(buffer, KCUBE_MESSAGE_HEADER_SIZE);
	print_hex(buffer + KCUBE_MESSAGE_HEADER_SIZE, kcube_message_decode_packet_size(buffer));
}

const char *kcube_message_id_string(uint16_t id)
{
	const char *str = "*unknown*";
	switch (id) {
	// generic message ids
	case MGMSG_MOD_IDENTIFY: str = "mgmsg_mod_identify"; break;
	case MGMSG_MOD_SET_CHANENABLESTATE: str = "mgmsg_mod_set_chanenablestate"; break;
	case MGMSG_MOD_REQ_CHANENABLESTATE: str = "mgmsg_mod_req_chanenablestate"; break;
	case MGMSG_MOD_GET_CHANENABLESTATE: str = "mgmsg_mod_get_chanenablestate"; break;
	case MGMSG_HW_DISCONNECT: str = "mgmsg_hw_disconnect"; break;
	case MGMSG_HW_RESPONSE: str = "mgmsg_hw_response"; break;
	case MGMSG_HW_RICHRESPONSE: str = "mgmsg_hw_richresponse"; break;
	case MGMSG_HW_START_UPDATEMSGS: str = "mgmsg_hw_start_updatemsgs"; break;
	case MGMSG_HW_STOP_UPDATEMSGS: str = "mgmsg_hw_stop_updatemsgs"; break;
	case MGMSG_HW_REQ_INFO: str = "mgmsg_hw_req_info"; break;
	case MGMSG_HW_GET_INFO: str = "mgmsg_hw_get_info"; break;
	// kpz101 message ids
	case MGMSG_PZ_SET_POSCONTROLMODE: str = "mgmsg_pz_set_poscontrolmode"; break;
	case MGMSG_PZ_REQ_POSCONTROLMODE: str = "mgmsg_pz_req_poscontrolmode"; break;
	case MGMSG_PZ_GET_POSCONTROLMODE: str = "mgmsg_pz_get_poscontrolmode"; break;
	case MGMSG_PZ_SET_OUTPUTVOLTS: str = "mgmsg_pz_set_outputvolts"; break;
	case MGMSG_PZ_REQ_OUTPUTVOLTS: str = "mgmsg_pz_req_outputvolts"; break;
	case MGMSG_PZ_GET_OUTPUTVOLTS: str = "mgmsg_pz_get_outputvolts"; break;
	case MGMSG_PZ_SET_OUTPUTPOS: str = "mgmsg_pz_set_outputpos"; break;
	case MGMSG_PZ_REQ_OUTPUTPOS: str = "mgmsg_pz_req_outputpos"; break;
	case MGMSG_PZ_GET_OUTPUTPOS: str = "mgmsg_pz_get_outputpos"; break;
	case MGMSG_PZ_SET_INPUTVOLTSSRC: str = "mgmsg_pz_set_inputvoltssrc"; break;
	case MGMSG_PZ_REQ_INPUTVOLTSSRC: str = "mgmsg_pz_req_inputvoltssrc"; break;
	case MGMSG_PZ_GET_INPUTVOLTSSRC: str = "mgmsg_pz_get_inputvoltssrc"; break;
	case MGMSG_PZ_SET_PICONSTS: str = "mgmsg_pz_set_piconsts"; break;
	case MGMSG_PZ_REQ_PICONSTS: str = "mgmsg_pz_req_piconsts"; break;
	case MGMSG_PZ_GET_PICONSTS: str = "mgmsg_pz_get_piconsts"; break;
	case MGMSG_PZ_GET_PZSTATUSUPDATE: str = "mgmsg_pz_get_pzstatusupdate"; break;
	case MGMSG_PZ_SET_OUTPUTLUT: str = "mgmsg_pz_set_outputlut"; break;
	case MGMSG_PZ_SET_OUTPUTLUTPARAMS: str = "mgmsg_pz_set_outputlutparams"; break;
	case MGMSG_PZ_REQ_OUTPUTLUTPARAMS: str = "mgmsg_pz_req_outputlutparams"; break;
	case MGMSG_PZ_GET_OUTPUTLUTPARAMS: str = "mgmsg_pz_get_outputlutparams"; break;
	case MGMSG_PZ_START_LUTOUTPUT: str = "mgmsg_pz_start_lutoutput"; break;
	case MGMSG_PZ_STOP_LUTOUTPUT: str = "mgmsg_pz_stop_lutoutput"; break;
	case MGMSG_PZ_SET_EEPROMPARAMS: str = "mgmsg_pz_set_eepromparams"; break;
	case MGMSG_PZ_SET_TPZ_DISPSETTINGS: str = "mgmsg_pz_set_tpz_dispsettings"; break;
	case MGMSG_PZ_REQ_TPZ_DISPSETTINGS: str = "mgmsg_pz_req_tpz_dispsettings"; break;
	case MGMSG_PZ_GET_TPZ_DISPSETTINGS: str = "mgmsg_pz_get_tpz_dispsettings"; break;
	case MGMSG_PZ_SET_TPZ_IOSETTINGS: str = "mgmsg_pz_set_tpz_iosettings"; break;
	case MGMSG_PZ_REQ_TPZ_IOSETTINGS: str = "mgmsg_pz_req_tpz_iosettings"; break;
	case MGMSG_PZ_GET_TPZ_IOSETTINGS: str = "mgmsg_pz_get_tpz_iosettings"; break;
	case MGMSG_KPZ_SET_KCUBEMMIPARAMS: str = "mgmsg_kpz_set_kcubemmiparams"; break;
	case MGMSG_KPZ_REQ_KCUBEMMIPARAMS: str = "mgmsg_kpz_req_kcubemmiparams"; break;
	case MGMSG_KPZ_GET_KCUBEMMIPARAMS: str = "mgmsg_kpz_get_kcubemmiparams"; break;
	case MGMSG_KPZ_SET_KCUBETRIGIOCONFIG: str = "mgmsg_kpz_set_kcubetrigioconfig"; break;
	case MGMSG_KPZ_REQ_KCUBETRIGIOCONFIG: str = "mgmsg_kpz_req_kcubetrigioconfig"; break;
	case MGMSG_KPZ_GET_KCUBETRIGIOCONFIG: str = "mgmsg_kpz_get_kcubetrigioconfig"; break;
	// ksg101 message ids
	case MGMSG_HUB_REQ_BAYUSED: str = "mgmsg_hub_req_bayused"; break;
	case MGMSG_HUB_GET_BAYUSED: str = "mgmsg_hub_get_bayused"; break;
// 	case MGMSG_PZ_GET_PZSTATUSUPDATE: str = "mgmsg_pz_get_pzstatusupdate"; break;
	case MGMSG_PZ_ACK_PZSTATUSUPDATE: str = "mgmsg_pz_ack_pzstatusupdate"; break;
// 	case MGMSG_PZ_SET_EEPROMPARAMS: str = "mgmsg_eepromparams"; break;
// 	case MGMSG_PZ_SET_TPZ_DISPSETTINGS: str = "mgmsg_set_tpz_dispsettings"; break;
// 	case MGMSG_PZ_REQ_TPZ_DISPSETTINGS: str = "mgmsg_req_tpz_dispsettings"; break;
// 	case MGMSG_PZ_GET_TPZ_DISPSETTINGS: str = "mgmsg_get_tpz_dispsettings"; break;
	case MGMSG_PZ_SET_ZERO: str = "mgmsg_pz_set_zero"; break;
	case MGMSG_PZ_REQ_MAXTRAVEL: str = "mgmsg_pz_req_maxtravel"; break;
	case MGMSG_PZ_GET_MAXTRAVEL: str = "mgmsg_pz_get_maxtravel"; break;
	case MGMSG_PZ_SET_TSG_IOSETTINGS: str = "mgmsg_pz_set_tsg_iosettings"; break;
	case MGMSG_PZ_REQ_TSG_IOSETTINGS: str = "mgmsg_pz_req_tsg_iosettings"; break;
	case MGMSG_PZ_GET_TSG_IOSETTINGS: str = "mgmsg_pz_get_tsg_iosettings"; break;
	case MGMSG_PZ_REQ_TSG_READING: str = "mgmsg_pz_req_tsg_reading"; break;
	case MGMSG_PZ_GET_TSG_READING: str = "mgmsg_pz_get_tsg_reading"; break;
	case MGMSG_KSG_SET_KCUBEMMIPARAMS: str = "mgmsg_ksg_set_kcubemmiparams"; break;
	case MGMSG_KSG_REQ_KCUBEMMIPARAMS: str = "mgmsg_ksg_req_kcubemmiparams"; break;
	case MGMSG_KSG_GET_KCUBEMMIPARAMS: str = "mgmsg_ksg_get_kcubemmiparams"; break;
	case MGMSG_KSG_SET_KCUBETRIGIOCONFIG: str = "mgmsg_ksg_set_kcubetrigioconfig"; break;
	case MGMSG_KSG_REQ_KCUBETRIGIOCONFIG: str = "mgmsg_ksg_req_kcubetrigioconfig"; break;
	case MGMSG_KSG_GET_KCUBETRIGIOCONFIG: str = "mgmsg_ksg_get_kcubetrigioconfig"; break;
	}
	return str;
}

void kcube_message_print_header(uint8_t *buffer)
{
	uint16_t id = kcube_message_decode_id(buffer);
	fprintf(stderr, "0x%04x id:%-32s ", id, kcube_message_id_string(id));
	if (kcube_message_decode_packet_bit(buffer)) {
		// packet header
		fprintf(stderr, "size:0x%04x ", kcube_message_decode_packet_size(buffer));
	} else {
		// simple header
		fprintf(stderr, "param1:0x%02x ", kcube_message_decode_param1(buffer));
		fprintf(stderr, "param2:0x%02x ", kcube_message_decode_param2(buffer));
	}
	fprintf(stderr, "dest:0x%02x ", kcube_message_decode_dest(buffer));
	fprintf(stderr, "source:0x%02x\n", kcube_message_decode_source(buffer));
}

void kcube_message_print_kpz101_pzstatus(uint8_t *buffer)
{
	if (kcube_message_decode_id(buffer) != MGMSG_PZ_GET_PZSTATUSUPDATE) {
		fprintf(stderr, "warning: expected a mgmsg_pz_get_pzstatusupdate\n");
		return;
	}
	uint16_t channel;
	int16_t voltage;
	uint16_t position;
	uint32_t status;
	mgmsg_pz_get_pzstatusupdate(buffer, &channel, &voltage, &position, &status);
	double volt_ratio = voltage / 327.68;
	double position_ratio = position / 655.36;
	char f = status & 0x00000400 ? 'F' : 'f'; // is feedback enabled (closed loop)
	char a = status & 0x20000000 ? 'A' : 'a'; // is unit active
	char c = status & 0x80000000 ? 'C' : 'c'; // is channel active
	fprintf(stderr, "ch:%i | volt:%7.2f%% |  pos:%7.2f%% | flags:%c%c%c 0x%08x", channel, volt_ratio, position_ratio, c, a, f, status);
}

void kcube_message_print_ksg101_pzstatus(uint8_t *buffer)
{
	if (kcube_message_decode_id(buffer) != MGMSG_PZ_GET_PZSTATUSUPDATE) {
		fprintf(stderr, "warning: expected a mgmsg_pz_get_pzstatusupdate\n");
		return;
	}
	uint16_t channel;
	int16_t reading;
	uint16_t dummy;
	uint32_t status;
	mgmsg_pz_get_pzstatusupdate(buffer, &channel, &reading, &dummy, &status);
	double ratio = reading / 327.68;
	char p = status & 0x00000001 ? 'P' : 'p'; // is piezo connected
	char z = status & 0x00000010 ? 'Z' : 'z'; // is zeroed
	char x = status & 0x00000020 ? 'X' : 'x'; // is zeroing
	char s = status & 0x00000100 ? 'S' : 's'; // is strain gauge connected
	char a = status & 0x20000000 ? 'A' : 'a'; // is unit active
	fprintf(stderr, "ch:%i | read:%7.2f%% %c%c%c%c%c", channel, ratio, a, s, x, z, p);
}

//============================================================================//
//                                                                            //
//                                                                            //
//                                                                            //
//============================================================================//

