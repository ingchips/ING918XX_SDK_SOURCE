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
#include "mesh_manage_conn_and_scan.h"
#include "app_debug.h"

#define CON_HANDLE_INVALID 0xFFFF

// mesh adv handle
#define MESH_PROXY_ADV_HANDLE        0x00
#define MESH_PB_ADV_HANDLE           0x01

// conn update.
#define CPI_VAL_TO_MS(x)    ((uint16_t)(x * 5 / 4))
#define CPI_MS_TO_VAL(x)    ((uint16_t)(x * 4 / 5))
#define CPSTT_VAL_TO_MS(x)  ((uint16_t)(x * 10))
#define CPSTT_MS_TO_VAL(x)  ((uint16_t)(x / 10))

// If the interval and window are equal, the duty cycle is 100 percent.
// scan_duty_cycle = scan_window/scan_interval.
// The scan_interval can not be smaller than the scan_window.
#define SCAN_CONFIG_SCAN_INTERVAl_MS  30 //set scan interval.
#define SCAN_CONFIG_SCAN_WINDOW_MS    15 //set scan window.
#define SCAN_SET_TIME_MS(x)  (uint16_t)(x * 1000 / 625)

// var.
static hci_con_handle_t my_conn_handle = CON_HANDLE_INVALID;
static uint16_t   my_conn_interval_ms = 0;

static bd_addr_t m_gatt_adv_addr;
static bd_addr_t m_beacon_adv_addr;

const static ext_adv_set_en_t adv_sets_en[2] = {{.handle = MESH_PROXY_ADV_HANDLE, .duration = 0, .max_events = 0},
                                                {.handle = MESH_PB_ADV_HANDLE,    .duration = 0, .max_events = 0}};

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


const static uint8_t prov_adv_data[] = {
    // 0x01 - Flags»
    2, 0x01,
    0x06,

    // 0x03 - Complete List of 16-bit Service Class UUIDs»
    3, 0x03,
    0x27, 0x18,

    // 0x16 - Service Data - 16-bit UUID»
    21, 0x16,
    0x27, 0x18, 0x00, 0x01, 0x61, 0x00, 0x04, 0x20,
    0x30, 0x75, 0x79, 0xBC, 0xDE, 0xEF, 0xB6, 0xDB,
    0x6D, 0xB6, 0x00, 0x00
};

const static uint8_t proxy_adv_data[] = {
    // 0x01 - Flags»
    2, 0x01,
    0x06,

    // 0x03 - Complete List of 16-bit Service Class UUIDs»
    3, 0x03,
    0x27, 0x18,

    // 0x16 - Service Data - 16-bit UUID»
    21, 0x16,
    0x27, 0x18, 0x00, 0x01, 0x61, 0x00, 0x04, 0x20,
    0x30, 0x75, 0x79, 0xBC, 0xDE, 0xEF, 0xB6, 0xDB,
    0x6D, 0xB6, 0x00, 0x00
};

// hello-mesh
const static uint8_t scan_data[] = {
    // 0x09 - Complete Local Name»
    0x0B, 0x09,
    0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x2D, 0x6D, 0x65, 0x73, 0x68
};





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
void mesh_proxy_adv_set_params( uint16_t  adv_int_min, 
                                uint16_t  adv_int_max, 
                                uint8_t   adv_type,
                                uint8_t   direct_address_typ, 
                                bd_addr_t direct_address, 
                                uint8_t   channel_map, 
                                uint8_t   filter_policy){
    mesh_adv_set_params(MESH_PROXY_ADV_HANDLE, adv_int_min, adv_int_max, adv_type, direct_address_typ, direct_address, channel_map, filter_policy);
}

void mesh_pb_adv_set_params(uint16_t  adv_int_min, 
                            uint16_t  adv_int_max, 
                            uint8_t   adv_type,
                            uint8_t   direct_address_typ, 
                            bd_addr_t direct_address, 
                            uint8_t   channel_map, 
                            uint8_t   filter_policy){
    mesh_adv_set_params(MESH_PB_ADV_HANDLE, adv_int_min, adv_int_max, adv_type, direct_address_typ, direct_address, channel_map, filter_policy);
}


void mesh_proxy_adv_set_data(uint8_t advertising_data_length, uint8_t * advertising_data){
    gap_set_ext_adv_data(MESH_PROXY_ADV_HANDLE, advertising_data_length, advertising_data);
}

void mesh_pb_adv_set_data(uint8_t advertising_data_length, uint8_t * advertising_data){
    gap_set_ext_adv_data(MESH_PB_ADV_HANDLE, advertising_data_length, advertising_data);
}

void mesh_proxy_scan_rsp_set_data(const uint16_t length, const uint8_t *data){
    gap_set_ext_scan_response_data(MESH_PROXY_ADV_HANDLE, length, data);
}

void mesh_proxy_adv_enable(int enabled){
    mesh_advertisements_enable(MESH_PROXY_ADV_HANDLE, enabled);
}

void mesh_pb_adv_enable(int enabled){
    mesh_advertisements_enable(MESH_PB_ADV_HANDLE, enabled);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// set scan addr.
static void mesh_scan_addr_set(void){
    gap_set_random_device_address(m_rnd_addr_scan);
}

/* API START */
// set scan parameters.
void mesh_scan_param_set(uint16_t interval_ms, uint8_t window_ms){
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

// scan enable or disable.
void mesh_scan_run_set(uint8_t en){
    app_log_debug("%s\n", __func__);
    // Scan disable.
    if (0 != gap_set_ext_scan_enable(en, 0, 0, 0)){
        app_log_error("=============>ERR - %s, en = %d\n", __func__, en);
    }
}

void mesh_scan_start(void){
    app_log_debug("%s\n", __func__);
    mesh_scan_run_set(1);
}

void mesh_scan_stop(void){
    app_log_debug("%s\n", __func__);
    mesh_scan_run_set(0);
}

void mesh_setup_scan(void){

    app_log_debug("%s\n", __func__);

    // Setup address of scanning other devices and creating connection with other devices.
    mesh_scan_addr_set();

    // Setup scan parameters.
    mesh_scan_param_set(SCAN_CONFIG_SCAN_INTERVAl_MS, SCAN_CONFIG_SCAN_WINDOW_MS);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* API START */
void ble_set_conn_interval_ms(uint16_t interval_ms){
    gap_update_connection_parameters(   ble_get_curr_conn_handle(), CPI_MS_TO_VAL(interval_ms), CPI_MS_TO_VAL(interval_ms), 
                                        0, CPSTT_MS_TO_VAL(5000), 0, 8);//ce_len = unit 0.625ms. 8*0.625=5ms

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mesh_gatt_adv_addr_set(bd_addr_t addr){
    memcpy(m_gatt_adv_addr, addr, sizeof(bd_addr_t));
}

void mesh_beacon_adv_addr_set(bd_addr_t addr){
    memcpy(m_beacon_adv_addr, addr, sizeof(bd_addr_t));
}

static void mesh_proxy_adv_setup(void){
    // set gatt addr.
    gap_set_adv_set_random_addr(MESH_PROXY_ADV_HANDLE, m_gatt_adv_addr);

    // set adv params.
    bd_addr_t peer_addr;
    memset(peer_addr, 0, sizeof(bd_addr_t));
    mesh_proxy_adv_set_params(80, 80, CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT, 
                                    BD_ADDR_TYPE_LE_PUBLIC, peer_addr, PRIMARY_ADV_ALL_CHANNELS, ADV_FILTER_ALLOW_ALL);

    // set adv data.
    gap_set_ext_adv_data(MESH_PROXY_ADV_HANDLE, sizeof(proxy_adv_data), (uint8_t *)proxy_adv_data);

    // set scan rsp data.
    gap_set_ext_scan_response_data(MESH_PROXY_ADV_HANDLE, sizeof(scan_data), scan_data);
}

static void mesh_pb_adv_setup(void){
    
    // set beacon addr.
    gap_set_adv_set_random_addr(MESH_PB_ADV_HANDLE, m_beacon_adv_addr);
        
    // set adv params.
    bd_addr_t peer_addr;
    memset(peer_addr, 0, sizeof(bd_addr_t));
    mesh_pb_adv_set_params(80, 80, LEGACY_PDU_BIT, BD_ADDR_TYPE_LE_PUBLIC, peer_addr, PRIMARY_ADV_ALL_CHANNELS, ADV_FILTER_ALLOW_ALL);

    // set adv data.
    gap_set_ext_adv_data(MESH_PB_ADV_HANDLE, sizeof(prov_adv_data), (uint8_t *)prov_adv_data);
}

/* API START */
void mesh_setup_adv(void)
{
    mesh_proxy_adv_enable(0);
    mesh_pb_adv_enable(0);
    
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
// get current connect interval.
/* API START */
uint16_t ble_get_curr_conn_interval_ms(void){
    return my_conn_interval_ms;
}

uint16_t ble_get_curr_conn_handle(void){
    return my_conn_handle;
}

bool Is_ble_curr_conn_valid(void){
    return (my_conn_handle == CON_HANDLE_INVALID)? false:true;
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
#ifdef ENABLE_RF_TX_RX_TEST
        #include "RF_TEST.H"
        IngRfTest_btstack_ready();
#endif
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
            case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:{
                    const le_meta_event_enh_create_conn_complete_t *create_conn =
                                        decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                    my_conn_handle = create_conn->handle;
                    my_conn_interval_ms = CPI_VAL_TO_MS(create_conn->interval);
                    app_log_debug("connect.\n");
                    att_set_db(my_conn_handle, ( ble_mesh_is_provisioned()?  gatt_data_proxy : gatt_data_pb ));
                    mesh_connected(my_conn_handle);
                    platform_calibrate_32k();
                }
                break;
            case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:{
                    const le_meta_event_conn_update_complete_t *conn_update = 
                                        decode_hci_le_meta_event(packet, le_meta_event_conn_update_complete_t);
                    app_log_debug("\nconn update complete:%d\n", conn_update->status);
                    if(conn_update->status == ERROR_CODE_SUCCESS){
                        my_conn_interval_ms = CPI_VAL_TO_MS(conn_update->interval);
                        
                        app_log_debug("status:%d\n", conn_update->status);
                        app_log_debug("handle:%d\n", conn_update->handle);
                        app_log_debug("interval:%dms\n", CPI_VAL_TO_MS(conn_update->interval));
                        app_log_debug("sup_timeout:%dms\n", CPSTT_VAL_TO_MS(conn_update->sup_timeout));
                    }
                    mesh_mcas_conn_params_update_complete_callback(conn_update->status, conn_update->handle, conn_update->interval, conn_update->sup_timeout);
                }
			    break;
            default:
                break;
        }
        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        app_log_debug("disconnect.\n");
        app_log_debug("------------------------------------------------------------------------------------------------\r\n");
        mesh_disconnected(my_conn_handle, NULL);
        my_conn_handle = CON_HANDLE_INVALID;
        my_conn_interval_ms = 0;
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

/* API START */
uint32_t setup_profile(void *data, void *user_data)
{
    // mesh init.
    mesh_init();

    // ble init.
    static btstack_packet_callback_registration_t mesh_hci_event_callback_registration;
    att_server_init(att_read_callback, att_write_callback);
    mesh_hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&mesh_hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    
    return 0;
}
