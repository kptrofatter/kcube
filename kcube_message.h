#ifndef KCUBE_MESSAGE_H
#define KCUBE_MESSAGE_H
//============================================================================//
//                                                                            //
//                                                                            //
//                                                                            //
//============================================================================//
#include <stdbool.h>
#include <stdint.h>

// message limits
#define KCUBE_MESSAGE_HEADER_SIZE 6
#define KCUBE_MESSAGE_PACKET_MAX_SIZE 256
#define KCUBE_MESSAGE_MAX_SIZE (KCUBE_MESSAGE_HEADER_SIZE + KCUBE_MESSAGE_PACKET_MAX_SIZE)

// kcube addresses
#define KCUBE_HOST 0x01
#define KCUBE_MOTHERBOARD 0x11
#define KCUBE_USB_DEVICE 0x50

// decode header =============================================================//
uint16_t kcube_message_decode_id(uint8_t *buffer);
uint8_t kcube_message_decode_param1(uint8_t *buffer);
uint8_t kcube_message_decode_param2(uint8_t *buffer);
bool kcube_message_decode_packet_bit(uint8_t *buffer);
uint8_t kcube_message_decode_dest(uint8_t *buffer);
uint8_t kcube_message_decode_source(uint8_t *buffer);
uint16_t kcube_message_decode_packet_size(uint8_t *buffer);

// test ======================================================================//
bool kcube_message_is_set(uint16_t id);
bool kcube_message_is_request(uint16_t id);
bool kcube_message_is_get(uint16_t id);
bool kcube_message_is_status(uint16_t id);
bool kcube_message_is_spontaneous(uint16_t id);
bool kcube_message_is_generic(uint16_t id);
bool kcube_message_is_kpz101(uint16_t id);
bool kcube_message_is_ksg101(uint16_t id);

// generic messages ==========================================================//
void mgmsg_mod_identify(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_mod_set_chanenablestate(uint8_t *buffer, uint8_t dest, uint8_t channel, uint8_t state);
void mgmsg_mod_req_chanenablestate(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_mod_get_chanenablestate(uint8_t *buffer, uint8_t *channel, uint8_t *state);
void mgmsg_hw_disconnect(uint8_t *buffer, uint8_t dest);
void mgmsg_hw_response(uint8_t *buffer);
void mgmsg_hw_richresponse(uint8_t *buffer, uint16_t *id, uint16_t *code, char **notes);
void mgmsg_hw_start_updatemsgs(uint8_t *buffer, uint8_t dest);
void mgmsg_hw_stop_updatemsgs(uint8_t *buffer, uint8_t dest);
void mgmsg_hw_req_info(uint8_t *buffer, uint8_t dest);
void mgmsg_hw_get_info(uint8_t *buffer, int32_t *serial_number, char **model_number, uint16_t *type,
	uint8_t **fw_version, uint16_t *hw_version, uint16_t *mod_state, uint16_t *n_channels);

// kpz101 messages ===========================================================//
void mgmsg_pz_set_poscontrolmode(uint8_t *buffer, uint8_t dest, uint8_t channel, uint8_t mode);
void mgmsg_pz_req_poscontrolmode(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_get_poscontrolmode(uint8_t *buffer, uint8_t *channel, uint8_t *mode);
void mgmsg_pz_set_outputvolts(uint8_t *buffer, uint8_t dest, uint16_t channel, int16_t voltage);
void mgmsg_pz_req_outputvolts(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_get_outputvolts(uint8_t *buffer, uint16_t *channel, int16_t *voltage);
void mgmsg_pz_set_outputpos(uint8_t *buffer, uint8_t dest, uint16_t channel, uint16_t position);
void mgmsg_pz_req_outputpos(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_get_outputpos(uint8_t *buffer, uint16_t *channel, uint16_t *position);
void mgmsg_pz_set_inputvoltssrc(uint8_t *buffer, uint8_t dest, uint16_t channel, int16_t voltsrc);
void mgmsg_pz_req_inputvoltssrc(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_get_inputvoltssrc(uint8_t *buffer, uint16_t *channel, uint16_t *voltsrc);
void mgmsg_pz_set_piconsts(uint8_t *buffer, uint8_t dest, uint16_t channel, uint16_t propconst, uint16_t intconst);
void mgmsg_pz_req_piconsts(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_get_piconsts(uint8_t *buffer, uint16_t *channel, uint16_t *propconst, uint16_t *intconst);
void mgmsg_pz_get_pzstatusupdate(uint8_t *buffer, uint16_t *channel, int16_t *voltage, uint16_t *position, uint32_t *status);
void mgmsg_pz_set_outputlut(uint8_t *buffer, uint8_t dest, uint16_t channel, uint16_t index, int16_t output);
void mgmsg_pz_set_outputlutparams(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t mode, uint16_t cycle_length, int32_t num_cycles, int32_t delay_time,
	int32_t pre_cycle_rest, int32_t post_cycle_rest, uint16_t op_trig_start, int32_t op_trig_width, uint16_t trig_rep_cycle);
void mgmsg_pz_req_outputlutparams(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_get_outputlutparams(uint8_t *buffer, uint16_t *channel,
	uint16_t *mode, uint16_t *cycle_length, int32_t *num_cycles, int32_t *delay_time,
	int32_t *pre_cycle_rest, int32_t *post_cycle_rest, uint16_t *op_trig_start, int32_t *op_trig_width, uint16_t *trig_rep_cycle);
void mgmsg_pz_start_lutoutput(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_stop_lutoutput(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_set_eepromparams(uint8_t *buffer, uint8_t dest, uint16_t channel, uint16_t id);
void mgmsg_pz_set_tpz_dispsettings(uint8_t *buffer, uint8_t dest, uint16_t intensity);
void mgmsg_pz_req_tpz_dispsettings(uint8_t *buffer, uint8_t dest);
void mgmsg_pz_get_tpz_dispsettings(uint8_t *buffer, uint16_t *intensity);
void mgmsg_pz_set_tpz_iosettings(uint8_t *buffer, uint8_t dest, uint16_t limit, uint16_t input);
void mgmsg_pz_req_tpz_iosettings(uint8_t *buffer, uint8_t dest);
void mgmsg_pz_get_tpz_iosettings(uint8_t *buffer, uint16_t *limit, uint16_t *input);
void mgmsg_kpz_set_kcubemmiparams(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t mode, uint16_t gearbox, int32_t step, uint16_t sense, int32_t preset_volt_1, int32_t preset_volt_2,
	uint16_t intensity, uint16_t timeout, uint16_t dimness);
void mgmsg_kpz_req_kcubemmiparams(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_kpz_get_kcubemmiparams(uint8_t *buffer, uint16_t *channel,
	uint16_t *mode, uint16_t *gearbox, int32_t *step, uint16_t *sense, int32_t *preset_volt_1, int32_t *preset_volt_2,
	uint16_t *intensity, uint16_t *timeout, uint16_t *dimness);
void mgmsg_kpz_set_kcubetrigioconfig(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t mode_1, uint16_t polarity_1, uint16_t mode_2, uint16_t polarity_2);
void mgmsg_kpz_req_kcubetrigioconfig(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_kpz_get_kcubetrigioconfig(uint8_t *buffer, uint16_t *channel,
	uint16_t *mode_1, uint16_t *polarity_1, uint16_t *mode_2, uint16_t *polarity_2);

// ksg101 messages ===========================================================//
void mgmsg_hub_req_bayused(uint8_t *buffer, uint8_t dest);
void mgmsg_hub_get_bayused(uint8_t *buffer, uint8_t *bay);
// mgmsg_pz_get_pzstatusupdate
void mgmsg_pz_ack_pzstatusupdate(uint8_t *buffer, uint8_t dest);
// mgmsg_pz_set_eepromparams
// mgmsg_pz_set_tpz_dispsettings
// mgmsg_pz_req_tpz_dispsettings
// mgmsg_pz_get_tpz_dispsettings
void mgmsg_pz_set_zero(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_req_maxtravel(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_get_maxtravel(uint8_t *buffer, uint16_t *channel, uint16_t *travel);
void mgmsg_pz_set_tsg_iosettings(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t hub_analog_op, uint16_t display_mode, int32_t force_calib);
void mgmsg_pz_req_tsg_iosettings(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_get_tsg_iosettings(uint8_t *buffer, uint16_t *channel,
	uint16_t *hub_analog_op, uint16_t *display_mode, int32_t *force_calib);
void mgmsg_pz_req_tsg_reading(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_pz_get_tsg_reading(uint8_t *buffer, uint16_t *channel, int16_t *reading, uint16_t *smoothed);
void mgmsg_ksg_set_kcubemmiparams(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t intensity, uint16_t timeout, uint16_t dimness);
void mgmsg_ksg_req_kcubemmiparams(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_ksg_get_kcubemmiparams(uint8_t *buffer, uint16_t *channel,
	uint16_t *intensity, uint16_t *timeout, uint16_t *dimness);
void mgmsg_ksg_set_kcubetrigioconfig(uint8_t *buffer, uint8_t dest, uint16_t channel,
	uint16_t mode_1, uint16_t polarity_1, uint16_t mode_2, uint16_t polarity_2,
	int32_t lower_lim, int32_t upper_lim, uint16_t smoothing_samples);
void mgmsg_ksg_req_kcubetrigioconfig(uint8_t *buffer, uint8_t dest, uint8_t channel);
void mgmsg_ksg_get_kcubetrigioconfig(uint8_t *buffer, uint16_t *channel,
	uint16_t *mode_1, uint16_t *polarity_1, uint16_t *mode_2, uint16_t *polarity_2,
	int32_t *lower_lim, int32_t *upper_lim, uint16_t *smoothing_samples);

// print =====================================================================//
void print_hex(uint8_t *buffer, size_t size); // try to remove this from header
void kcube_message_dump(uint8_t *buffer);
const char *kcube_message_id_string(uint16_t id);
void kcube_message_print_header(uint8_t *buffer);
void kcube_message_print_kpz101_pzstatus(uint8_t *buffer);
void kcube_message_print_kgz101_pzstatus(uint8_t *buffer);

//============================================================================//
//                                                                            //
//                                                                            //
//                                                                            //
//============================================================================//
#endif

