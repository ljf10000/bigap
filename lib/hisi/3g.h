#ifndef __3G_H_e8a1bc902ff14781a9446f382cf15382__
#define __3G_H_e8a1bc902ff14781a9446f382cf15382__
/******************************************************************************/
#ifdef __APP__
/******************************************************************************/
#if IS_PRODUCT_LTEFI_MD
#include "hi_3g_intf.h"
#endif

#define hisi_3g_init(card_pointer) \
    hisi_module_call_x(3G, InitCard, card_pointer)
/*
* SB interface, not used it
*/
#define hisi_3g_fini(card_pointer) \
    hisi_module_call_x(3G, DeInitCard, card_pointer)

#define hisi_3g_connect(card_pointer, username, password, telenumber, argc, argv) \
    hisi_module_call_x(3G, ConnectCard, card_pointer, username, password, telenumber, argc, argv)
#define hisi_3g_disconnect(card_pointer) \
    hisi_module_call_x(3G, DisConnectCard, card_pointer)

#define hisi_3g_get_apn(card_pointer, pdp_pointer) \
    hisi_module_call_x(3G, GetApn, card_pointer, pdp_pointer)
#define hisi_3g_set_apn(card_pointer, apn_string) \
    hisi_module_call_x(3G, SetApn, card_pointer, apn_string)

#define hisi_3g_get_all_operators(card_pointer, operator_pointer, max_op_number, op_number_pointer) \
    hisi_module_call_x(3G, GetAllOperators, card_pointer, operator_pointer, max_op_number, op_number_pointer)
#define hisi_3g_get_current_operator(card_pointer, operator_pointer) \
    hisi_module_call_x(3G, GetCurrentOperator, card_pointer, operator_pointer)

#define hisi_3g_get_status(card_pointer, status_pointer) \
    hisi_module_call_x(3G, GetCardStatus, card_pointer, status_pointer)
#define hisi_3g_get_dat_flow(card_pointer, flow_pointer) \
    hisi_module_call_x(3G, GetDataFlow, card_pointer, flow_pointer)
#define hisi_3g_get_quality(card_pointer, rssi_pointer, ber_pointer) \
    hisi_module_call_x(3G, GetQuality, card_pointer, rssi_pointer, ber_pointer)

#define hisi_3g_register_operator(card_pointer, mode, format, operator) \
    hisi_module_call_x(3G, RegisterOperator, card_pointer, mode, format, operator)
#define hisi_3g_search_operator_mode_acqorder(card_pointer, mode, format) \
    hisi_module_call_x(3G, SearchOperatorModeAcqorder, card_pointer, mode, format)

#define hisi_3g_scan(card_pointer, max_card_number, card_number_pointer) \
    hisi_module_call_x(3G, ScanCard, card_pointer, max_card_number, card_number_pointer)
#define hisi_3g_send_cmd(card_pointer, cmd_string, buf_string, buf_len) \
    hisi_module_call_x(3G, SendCmd, card_pointer, cmd_string, buf_string, buf_len)

#define hisi_3g_status_list {   \
    "unavailable",              \
    "pin",                      \
    "puk",                      \
    "pin1",                     \
    "puk2",                     \
    "disconnected",             \
    "connected"                 \
}   /* end */
#define is_good_hisi_3g_status(_status)     is_good_enum(0, HI_3G_CARD_STATUS_BOTT)

/******************************************************************************/
#endif /* __APP__ */
#endif /* __3G_H_e8a1bc902ff14781a9446f382cf15382__ */
