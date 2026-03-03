#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "gap.h"
#include "bt_types.h"
#include "btstack_event.h"
#include "errno.h"
#include "stdbool.h"
#include "platform_api.h"
#include "att_db.h"
#include "mesh_profile.h"
#include "app_config.h"
#include "profile.h"
#include "mesh.h"
#include "mesh_port_stack.h"
#include "mesh_storage_app.h"
#include "app_debug.h"
#include "board.h"
#include "ble_status.h"

#define BLUETOOTH_DATA_TYPE_PB_ADV                                             0x29 // PB-ADV
#define BLUETOOTH_DATA_TYPE_MESH_MESSAGE                                       0x2A // Mesh Message
#define BLUETOOTH_DATA_TYPE_MESH_BEACON                                        0x2B // Mesh Beacon

#define CON_HANDLE_INVALID 0xFFFF

// mesh adv handle
#define MESH_PROXY_ADV_HANDLE        0x00
#define MESH_PB_ADV_HANDLE           0x01

// conn update.
#define CPI_VAL_TO_MS(x)    ((uint16_t)(x * 5 / 4))
#define CPI_MS_TO_VAL(x)    ((uint16_t)(x * 4 / 5))
#define CPSTT_VAL_TO_MS(x)  ((uint16_t)(x * 10))
#define CPSTT_MS_TO_VAL(x)  ((uint16_t)(x / 10))

// scan
#define SCAN_SET_TIME_MS(x)  (uint16_t)(x * 1000 / 625)

// var.
static hci_con_handle_t my_conn_handle = CON_HANDLE_INVALID;

static bd_addr_t m_gatt_adv_addr;
static bd_addr_t m_beacon_adv_addr;

const static ext_adv_set_en_t adv_sets_en[2] = {{.handle = MESH_PROXY_ADV_HANDLE, .duration = 0, .max_events = 0},
                                                {.handle = MESH_PB_ADV_HANDLE,    .duration = 0, .max_events = 1}};

static bd_addr_t m_rnd_addr_scan = {0xFD, 0x51, 0x52, 0x53, 0x53, 0x02};

const uint8_t gatt_data_pb[] =
{
    #include "../data/gatt_pb.profile"
};

const uint8_t gatt_data_proxy[] =
{
    #include "../data/gatt_proxy.profile"
};

#include "../data/gatt_pb.const"
#include "../data/gatt_proxy.const"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void mesh_adv_set_params(uint8_t   adv_handle,
                                uint16_t  adv_int_min,
                                uint16_t  adv_int_max,
                                uint8_t   adv_type,
                                uint8_t   direct_address_typ,
                                bd_addr_t direct_address,
                                uint8_t   channel_map,
                                uint8_t   filter_policy){

    gap_set_ext_adv_para(   adv_handle,
                            adv_type,
                            adv_int_min,
                            adv_int_max,
                            channel_map,
                            BD_ADDR_TYPE_LE_RANDOM,
                            (bd_addr_type_t)direct_address_typ,
                            direct_address,
                            (adv_filter_policy_t)filter_policy,
                            100,   //tx power
                            PHY_1M,
                            0,     //max skip
                            PHY_1M,
                            0x00,  //sig
                            0x00 ); //notify
}


static void mesh_advertisements_enable(uint8_t adv_handle, int enabled){
    uint8_t i;
    uint8_t set_number = sizeof(adv_sets_en) / sizeof(adv_sets_en[0]);
    for(i=0; i<set_number; i++){
        if(adv_handle == adv_sets_en[i].handle){
            gap_set_ext_adv_enable(enabled, 1, &adv_sets_en[i]);
        }
    }
}

/* API START */
void mesh_profile_adv_proxy_set_params( uint16_t  adv_int_min,
                                        uint16_t  adv_int_max,
                                        uint8_t   adv_type,
                                        uint8_t   direct_address_typ,
                                        bd_addr_t direct_address,
                                        uint8_t   channel_map,
                                        uint8_t   filter_policy){
    mesh_adv_set_params(MESH_PROXY_ADV_HANDLE, adv_int_min, adv_int_max, adv_type, direct_address_typ, direct_address, channel_map, filter_policy);
}

void mesh_profile_adv_pb_set_params(uint16_t  adv_int_min,
                                    uint16_t  adv_int_max,
                                    uint8_t   adv_type,
                                    uint8_t   direct_address_typ,
                                    bd_addr_t direct_address,
                                    uint8_t   channel_map,
                                    uint8_t   filter_policy){
    mesh_adv_set_params(MESH_PB_ADV_HANDLE, adv_int_min, adv_int_max, adv_type, direct_address_typ, direct_address, channel_map, filter_policy);
}


void mesh_profile_adv_proxy_set_data(uint8_t advertising_data_length, uint8_t * advertising_data){
    gap_set_ext_adv_data(MESH_PROXY_ADV_HANDLE, advertising_data_length, advertising_data);
}

void mesh_profile_adv_pb_set_data(uint8_t advertising_data_length, uint8_t * advertising_data){
    gap_set_ext_adv_data(MESH_PB_ADV_HANDLE, advertising_data_length, advertising_data);
}

void mesh_profile_adv_proxy_set_scan_rsp_data(const uint16_t length, const uint8_t *data){
    gap_set_ext_scan_response_data(MESH_PROXY_ADV_HANDLE, length, data);
}

void mesh_profile_adv_proxy_enable(int enabled){
    mesh_advertisements_enable(MESH_PROXY_ADV_HANDLE, enabled);
}

void mesh_profile_adv_pb_enable(int enabled){
    mesh_advertisements_enable(MESH_PB_ADV_HANDLE, enabled);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* API START */

// adv bearer send begin callback.
static void mesh_profile_adv_bearer_event_send_msg_begin(mesh_adv_bearer_send_msg_begin_t *msg){
}

// adv bearer send end callback.
static void mesh_profile_adv_bearer_event_send_msg_end(mesh_adv_bearer_send_msg_end_evt_t *evt){
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* API START */

// att can send now.
static void mesh_profile_att_server_request_can_send_now_event(hci_con_handle_t con_handle)
{
    att_server_request_can_send_now_event(con_handle);
}

// att server noitfy.
static int mesh_profile_att_server_notify(hci_con_handle_t con_handle, uint16_t attribute_handle, uint8_t *value, uint16_t value_len)
{
    return att_server_notify(con_handle, attribute_handle, value, value_len);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// scan enable or disable.
static void mesh_scan_run_set(uint8_t en, uint16_t scan_timeout_ms){
    // Scan disable.
    if (0 != gap_set_ext_scan_enable(en, 0, scan_timeout_ms/10, 0)){
        app_log_error("=============>ERR - %s, en = %d\n", __func__, en);
        return;
    }

    ble_status_scan_state_set( (en>0)? 1:0 );
}

/* API START */

// set scan addr.
static void mesh_profile_scan_addr_set(void){
    gap_set_random_device_address(m_rnd_addr_scan);
}

// set scan parameters.
void mesh_profile_scan_param_set(uint16_t interval_ms, uint16_t window_ms){
    // Setup scan parameters.
    bd_addr_type_t          own_addr_type = BD_ADDR_TYPE_LE_RANDOM;
    scan_filter_policy_t    filter_policy = SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED;
    uint8_t                 config_num = 1;
    scan_phy_config_t       scan_config[1] = {{ .phy = PHY_1M, .type = SCAN_PASSIVE,
                                                .interval = SCAN_SET_TIME_MS(interval_ms),
                                                .window = SCAN_SET_TIME_MS(window_ms)}};
    if (0 != gap_set_ext_scan_para(own_addr_type, filter_policy, config_num, scan_config)){
        app_log_error("=============>ERR - %s\n", __func__);
    }
}

void mesh_profile_scan_duty_start(void){
    mesh_scan_run_set(1, 0);
}

void mesh_profile_scan_single_start(uint16_t scan_timeout_ms){
    mesh_scan_run_set(1, scan_timeout_ms);
}

void mesh_profile_scan_stop(void){
    mesh_scan_run_set(0, 0);
}

void mesh_setup_scan(void){
    mesh_profile_scan_addr_set();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* API START */
void mesh_profile_conn_interval_update_ms(uint16_t interval_ms){
    gap_update_connection_parameters(   my_conn_handle, \
                            CPI_MS_TO_VAL(interval_ms), CPI_MS_TO_VAL(interval_ms), \
                            0, CPSTT_MS_TO_VAL(5000), NULL, NULL);//slave can not change ce_len, use [ll_hint_on_ce_len] to set slave local ce_len
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mesh_gatt_adv_addr_set(bd_addr_t addr){
    memcpy(m_gatt_adv_addr, addr, sizeof(bd_addr_t));
}

void mesh_gatt_adv_addr_get(bd_addr_t addr){
    memcpy(addr, m_gatt_adv_addr, sizeof(bd_addr_t));
}

void mesh_beacon_adv_addr_set(bd_addr_t addr){
    memcpy(m_beacon_adv_addr, addr, sizeof(bd_addr_t));
}

void mesh_beacon_adv_addr_get(bd_addr_t addr){
    memcpy(addr, m_beacon_adv_addr, sizeof(bd_addr_t));
}

static void mesh_proxy_adv_setup(void){
    // set gatt addr.
    gap_set_adv_set_random_addr(MESH_PROXY_ADV_HANDLE, m_gatt_adv_addr);
}

static void mesh_pb_adv_setup(void){
    // set beacon addr.
    gap_set_adv_set_random_addr(MESH_PB_ADV_HANDLE, m_beacon_adv_addr);
}

/* API START */
void mesh_setup_adv(void)
{
    mesh_profile_adv_proxy_enable(0);
    mesh_profile_adv_pb_enable(0);

    mesh_proxy_adv_setup();
    mesh_pb_adv_setup();
}

void mesh_server_restart(void)
{
    app_log_debug("reset disconnect %d\n", my_conn_handle);
    if (my_conn_handle != CON_HANDLE_INVALID){
        gap_disconnect(my_conn_handle);
    } else {
        mesh_disconnected(my_conn_handle, NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void user_msg_handler(btstack_user_msg_t * usrmsg)
{
#ifdef ENABLE_BUTTON_TEST
    #include "BUTTON_TEST.h"
    button_msg_handler(usrmsg);
#endif
}

// read
static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    return mesh_att_read_callback(connection_handle, att_handle, buffer, buffer_size);
}

// write
static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    return mesh_att_write_callback(connection_handle, att_handle, buffer, buffer_size);
}

// handler
static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        app_log_debug("bt init ok.\n");
        mesh_stack_ready();
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
            case HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:{
                    const le_ext_adv_report_t *report = decode_hci_le_meta_event(packet, le_meta_event_ext_adv_report_t)->reports;

                    // only non-connectable ind
                    if (report->evt_type != 0x10) break;

                    switch(report->data[1]){
                        case BLUETOOTH_DATA_TYPE_MESH_MESSAGE:
                            // printf("Message adv[%d]:", report->data_len);
                            // printf_hexdump(report->data, report->data_len);
                            break;
                        case BLUETOOTH_DATA_TYPE_MESH_BEACON:
                            // printf("Beacon adv[%d]:", report->data_len);
                            // printf_hexdump(report->data, report->data_len);
                            break;
                        case BLUETOOTH_DATA_TYPE_PB_ADV:
                            // printf("pb adv[%d]:", report->data_len);
                            // printf_hexdump(report->data, report->data_len);
                            break;
                        default:
                            break;
                    }
                }
                break;


            case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:{
                    const le_meta_event_enh_create_conn_complete_t *create_conn =
                                        decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                    my_conn_handle = create_conn->handle;
                    app_log_debug("connect.\n");
                    att_set_db(my_conn_handle, ( mesh_is_provisioned()?  gatt_data_proxy : gatt_data_pb ));
                    mesh_connected(my_conn_handle);
                }
                break;
            case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:{
                    const le_meta_event_conn_update_complete_t *conn_update =
                                        decode_hci_le_meta_event(packet, le_meta_event_conn_update_complete_t);
                    app_log_debug("\nconn update complete:%d\n", conn_update->status);
                    if(conn_update->status == ERROR_CODE_SUCCESS){
                        app_log_debug("status:%d\n", conn_update->status);
                        app_log_debug("handle:%d\n", conn_update->handle);
                        app_log_debug("interval:%dms\n", CPI_VAL_TO_MS(conn_update->interval));
                        app_log_debug("sup_timeout:%dms\n", CPSTT_VAL_TO_MS(conn_update->sup_timeout));
                    }
                }
                break;
            case HCI_SUBEVENT_LE_SCAN_TIMEOUT:{
                }
                break;
            case HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED:{
                    const le_meta_event_adv_set_terminate_t *adv_term =
                                        decode_hci_le_meta_event(packet, le_meta_event_adv_set_terminate_t);
                    if(MESH_PB_ADV_HANDLE == adv_term->adv_handle){
                    }
                }
                break;
            default:
                break;
        }
        break;
    case HCI_EVENT_COMMAND_COMPLETE:{
            uint8_t cmd_packs = hci_event_command_complete_get_num_hci_command_packets(packet);
            uint16_t cmd_opcode = hci_event_command_complete_get_command_opcode(packet);
            const uint8_t *pCmd_param = hci_event_command_complete_get_return_parameters(packet);
            switch(cmd_opcode){
            }
        }
        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        app_log_debug("disconnect.\n");
        app_log_debug("------------------------------------------------------------------------------------------------\r\n");
        mesh_disconnected(my_conn_handle, NULL);
        my_conn_handle = CON_HANDLE_INVALID;
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        mesh_gatt_can_send_now(my_conn_handle);
        break;

    case BTSTACK_EVENT_USER_MSG:
        {
            btstack_user_msg_t *p_user_msg;
            p_user_msg = (btstack_user_msg_t *)hci_event_packet_get_user_msg(packet);
            user_msg_handler(p_user_msg);
        }
        break;

    default:
        break;
    }
}


// adv
static const mesh_profile_api_adv_t mesh_profile_adv_impl = {
    /* void (*proxy_set_params)(..)         */ &mesh_profile_adv_proxy_set_params,
    /* void (*pb_set_params)(..)            */ &mesh_profile_adv_pb_set_params,
    /* void (*proxy_set_data)(..);          */ &mesh_profile_adv_proxy_set_data,
    /* void (*pb_set_data)(..);             */ &mesh_profile_adv_pb_set_data,
    /* void (*proxy_set_scan_rsp_data)(..); */ &mesh_profile_adv_proxy_set_scan_rsp_data,
    /* void (*proxy_enable)(..);            */ &mesh_profile_adv_proxy_enable,
    /* void (*pb_enable)(..);               */ &mesh_profile_adv_pb_enable,
};

// scan
static const mesh_profile_api_scan_t mesh_profile_scan_impl = {
    /* void (*addr_set)(..);                */ &mesh_profile_scan_addr_set,
    /* void (*param_set)(..);               */ &mesh_profile_scan_param_set,
    /* void (*duty_start)(..);              */ &mesh_profile_scan_duty_start,
    /* void (*single_start)(..);            */ &mesh_profile_scan_single_start,
    /* void (*stop)(..);                    */ &mesh_profile_scan_stop,
};

// adv bearer event.
static const mesh_profile_api_adv_bearer_event_t mesh_profile_adv_bearer_event_impl = {
    /* void (*send_msg_begin)(..);           */ &mesh_profile_adv_bearer_event_send_msg_begin,
    /* void (*send_msg_end)(..);             */ &mesh_profile_adv_bearer_event_send_msg_end,
};

// att
static const mesh_profile_api_att_t mesh_profile_att_impl = {
    /* void (*att_server_notify)(..);            */ &mesh_profile_att_server_notify,
    /* void (*att_req_can_send_now_evt)(..);     */ &mesh_profile_att_server_request_can_send_now_event,
};

// conn
static const mesh_profile_api_conn_t mesh_profile_conn_impl = {
    /* void (*conn_interval_update)(..);            */ &mesh_profile_conn_interval_update_ms,
};

// all.
static const mesh_profile_api_t mesh_profile_api_impl = {
    /* const mesh_profile_api_adv_t *                   */ &mesh_profile_adv_impl,
    /* const mesh_profile_api_scan_t *                  */ &mesh_profile_scan_impl,
    /* const mesh_profile_api_adv_bearer_event_t *      */ &mesh_profile_adv_bearer_event_impl,
    /* const mesh_profile_api_att_t *                   */ &mesh_profile_att_impl,
    /* const mesh_profile_api_conn_t *                  */ &mesh_profile_conn_impl,
};


/* API START */
uint32_t setup_profile(void *data, void *user_data)
{
    // mesh func register first.
    mesh_profile_register(&mesh_profile_api_impl);

    // mesh init.
    mesh_init();

    // ble init.
    static btstack_packet_callback_registration_t mesh_hci_event_callback_registration;
    att_server_init(att_read_callback, att_write_callback);
    mesh_hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&mesh_hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);

    // ble status init.
    ble_status_init();

    return 0;
}
