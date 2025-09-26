#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "platform_api.h"
#include "ll_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"
#include "gatt_client.h"

#define CONST_MASTER    0
#define CONST_SLAVE     1
#ifndef APP_ROLE
#define APP_ROLE        CONST_MASTER
// #define APP_ROLE        CONST_SLAVE
#endif

#define TEST_SERVICE_UUID      0xFF10
#define TEST_CHARACTERISTIC_UUID 0xFF11

// 测试数据缓冲区
static uint8_t test_data[200] = "EATT Server Test Data";
static uint16_t test_data_len = 21;
static uint16_t test_value_handle;
// GATT characteristic handles





static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {

    default:
        return 0;
    }
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    if (att_handle == test_value_handle) {
        printf("Server收到数据: %.*s\n", buffer_size, buffer);
        // 收到后通过通知返回测试数据
        att_server_notify(connection_handle, test_value_handle, test_data, test_data_len);
    }
    return 0;
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
        // add your code
    //case MY_MESSAGE_ID:
    //    break;
    default:
        ;
    }
}
static scan_phy_config_t scan_configs[] =
{
    {
        .phy = PHY_1M,
        .type = SCAN_ACTIVE,
        .interval = 200,
        .window = 90
    },
    {
        .phy = PHY_CODED,
        .type = SCAN_ACTIVE,
        .interval = 200,
        .window = 90
    }
};
const static ext_adv_set_en_t adv_sets_en[] = { {.handle = 0, .duration = 0, .max_events = 0} };
#define CONN_PARAM  {                   \
            .scan_int = 200,            \
            .scan_win = 100,            \
            .interval_min = 50,         \
            .interval_max = 50,         \
            .latency = 0,               \
            .supervision_timeout = 600, \
            .min_ce_len = 90,           \
            .max_ce_len = 90            \
    }

static initiating_phy_config_t phy_configs[] =
{
    {
        .phy = PHY_1M,
        .conn_param = CONN_PARAM
    },
    {
        .phy = PHY_2M,
        .conn_param = CONN_PARAM
    },
    {
        .phy = PHY_CODED,
        .conn_param = CONN_PARAM
    }
};

#if (APP_ROLE == CONST_MASTER)
static const bd_addr_t rand_addr = { 0xCC, 0x11, 0xD8, 0x29, 0x81, 0x33 };
#else
static const bd_addr_t rand_addr = { 0xCC, 0x11, 0xD8, 0x29, 0x81, 0x34 };
#endif
static const bd_addr_t peer_addr = { 0xCC, 0x11, 0xD8, 0x29, 0x81, 0x34 };

void create_connection(void)
{
    platform_printf("Create Connection to %02X:%02X:%02X:%02X:%02X:%02X\n",
                    peer_addr[5], peer_addr[4], peer_addr[3],
                    peer_addr[2], peer_addr[1], peer_addr[0]);
    gap_ext_create_connection(INITIATING_ADVERTISER_FROM_PARAM,
                    BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                    BD_ADDR_TYPE_LE_RANDOM,           // Peer_Address_Type,
                    peer_addr,                        // Peer_Address,
                    sizeof(phy_configs) / sizeof(phy_configs[0]),
                    phy_configs);
}
#if (APP_ROLE != CONST_MASTER)
const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};
static void setup_adv(void)
{
    gap_set_ext_adv_para(0,
                            CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                            0x00a1, 0x00a1,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
                            PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                            BD_ADDR_TYPE_LE_RANDOM,    // Own_Address_Type
                            BD_ADDR_TYPE_LE_PUBLIC,    // Peer_Address_Type (ignore)
                            NULL,                      // Peer_Address      (ignore)
                            ADV_FILTER_ALLOW_ALL,      // Advertising_Filter_Policy
                            0x00,                      // Advertising_Tx_Power
                            PHY_1M,                    // Primary_Advertising_PHY
                            0,                         // Secondary_Advertising_Max_Skip
                            PHY_1M,                    // Secondary_Advertising_PHY
                            0x00,                      // Advertising_SID
                            0x00);                     // Scan_Request_Notification_Enable
    gap_set_ext_adv_data(0, sizeof(adv_data), (uint8_t*)adv_data);
    gap_set_ext_scan_response_data(0, sizeof(scan_data), (uint8_t*)scan_data);
    gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
}
#endif
#define STORAGE_SIZE 1024*4
uint8_t num_eatt_bearers = 5;
uint8_t test_storage_buffer[STORAGE_SIZE] = {0};
uint8_t gatt_client_storage[STORAGE_SIZE] = {0};
void my_gatt_callback(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size){
    platform_printf("my_gatt_callback: packet_type=%d, channel=0x%04X, size=%d\n", packet_type, channel, size);
    }
void start_enhanced_gatt(hci_con_handle_t con_handle){
    uint8_t status = gatt_client_le_enhanced_connect(
        my_gatt_callback,
        con_handle,
        num_eatt_bearers,
        gatt_client_storage,
        sizeof(gatt_client_storage)
    );

    if (status != 0){
        printf("Enhanced GATT connect failed: %d\n", status);
    } else {
        printf("Enhanced GATT connect started\n");
    }
}
static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{

    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        gap_set_adv_set_random_addr(0, rand_addr);
#if (APP_ROLE == CONST_MASTER)
        gap_set_adv_set_random_addr(0, rand_addr);
        gap_set_random_device_address(rand_addr);
        gap_add_whitelist(peer_addr, BD_ADDR_TYPE_LE_RANDOM);
        gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_WLIST_EXCEPT_NOT_DIRECTED,
                              sizeof(scan_configs) / sizeof(scan_configs[0]),
                              scan_configs);
        gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
#else
        setup_adv();
#endif
        
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        switch (hci_event_command_complete_get_command_opcode(packet))
        {
        // add your code to check command complete response
        // case :
        //     break;
        default:
            break;
        }
        break;

    case HCI_EVENT_LE_META:
    
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
					case HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:
					{ 
							// add your code to process advertising report

							const le_ext_adv_report_t *report = decode_hci_le_meta_event(packet, le_meta_event_ext_adv_report_t)->reports;
							if(report)
							{
									gap_set_ext_scan_enable(0, 0, 0, 0);
									platform_printf("create connection...\n");
									create_connection();
							}
							break;
					}

					case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
                    case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
                    start_enhanced_gatt(decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t)->handle);
							// att_set_db(decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t)->handle,
							// 					 profile_data);
							break;
					default:
							break;
					}

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        // add your code
        break;

    case BTSTACK_EVENT_USER_MSG:
        p_user_msg = hci_event_packet_get_user_msg(packet);
        user_msg_handler(p_user_msg->msg_id, p_user_msg->data, p_user_msg->len);
        break;

    default:
        break;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

uint32_t setup_profile(void *data, void *user_data)
{
    platform_printf("setup profile\n");
    platform_32k_rc_auto_tune();
    // Note: security has not been enabled.
    att_server_init(att_read_callback, att_write_callback);
    #if (APP_ROLE != CONST_MASTER)
    int ret = att_server_eatt_init(num_eatt_bearers, test_storage_buffer, STORAGE_SIZE);
    platform_printf("att_server_eatt_init: %d\n", ret);
    #endif
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

