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
// GATT characteristic handles
#include "../data/gatt.const"
#define CONST_MASTER    0
#define CONST_SLAVE     1
#ifndef APP_ROLE
#define APP_ROLE        CONST_MASTER
//#define APP_ROLE        CONST_SLAVE
#endif

#define TEST_SERVICE_UUID      0xFF10
#define TEST_CHARACTERISTIC_UUID 0xFF11

// 测试数据缓冲区
static uint16_t test_value_handle;
// GATT characteristic handles

/* --- Configuration --- */
#define NUM_EATT_BEARERS 2
#define EATT_STORAGE_BYTES_PER_BEARER sizeof(att_server_eatt_bearer_t)  // from btstack internals
#define EATT_STORAGE_SIZE (NUM_EATT_BEARERS * EATT_STORAGE_BYTES_PER_BEARER)

static uint16_t handle_char_read = 0;
static uint16_t handle_char_write = 0;
static uint16_t handle_char_long = 0;

/* Runtime values */
#define LONG_CHAR_MAX_LEN 512
static uint8_t long_char_value[LONG_CHAR_MAX_LEN];
static uint16_t long_char_len = 0;
#define PROFILE_NUM     4
#define INVALID_SET_ID  0xff
#define BUF_SIZE    200

typedef struct
{
    uint8_t size;
    char buf[BUF_SIZE];
} str_buf_t;

typedef struct profile_cfg
{
    uint8_t adv_handle;
    uint8_t notify_enable;
    str_buf_t to_send;
    str_buf_t spoken;
} profile_cfg_t;

profile_cfg_t profile_cfgs[PROFILE_NUM] = {{.adv_handle = INVALID_SET_ID}, {.adv_handle = INVALID_SET_ID}, {.adv_handle = INVALID_SET_ID}, {.adv_handle = INVALID_SET_ID}};

static void send(uint8_t i)
{
    profile_cfg_t *prof = profile_cfgs + i;
    if (INVALID_SET_ID == prof->adv_handle) return;
    if (0 == (prof->notify_enable | prof->to_send.size)) return;

    if (att_server_notify(i, HANDLE_GENERIC_OUTPUT, (uint8_t *)prof->to_send.buf, prof->to_send.size) != BTSTACK_ACL_BUFFERS_FULL)
        prof->to_send.size = 0;
    else
        att_server_request_can_send_now_event(i);
}

static void send_all()
{
    uint8_t i;
    for (i = 0; i < PROFILE_NUM; i++)
        send(i);
}

static void append_data(str_buf_t *buf, const char *d, const uint8_t len)
{
    if (buf->size + len > sizeof(buf->buf))
        buf->size = 0;

    if (buf->size + len <= sizeof(buf->buf))
    {
        memcpy(buf->buf + buf->size, d, len);
        buf->size += len;
    }
}

static void send_to_client(uint8_t conn_handle, const char *data, uint8_t len)
{
    profile_cfg_t *prof = profile_cfgs + conn_handle;
    if (INVALID_SET_ID == prof->adv_handle) return;
    if (0 == prof->notify_enable) return;

    append_data(&prof->to_send, data, len);

    send(conn_handle);
}

static void send_to_except(uint8_t conn_handle, const char *data, uint8_t len)
{
    int i;
    for (i = 0; i < PROFILE_NUM; i++)
    {
        if (i == conn_handle) continue;
        send_to_client(i, data, len);
    }
}

static void handle_client_speak(uint8_t conn_handle, const char *data, uint8_t len)
{
    profile_cfg_t *prof = profile_cfgs + conn_handle;
    if (0 == prof->spoken.size)
    {
        sprintf(prof->spoken.buf, "[%d]: ", conn_handle);
        prof->spoken.size = strlen(prof->spoken.buf);
    }
    append_data(&prof->spoken, data, len);

    if (prof->spoken.size && (prof->spoken.buf[prof->spoken.size - 1] == '\0'))
    {
        send_to_except(conn_handle, prof->spoken.buf, prof->spoken.size);
        prof->spoken.size = 0;
    }
}

static void send_greetings(uint8_t conn_handle)
{
    static char temp[255];
    uint8_t len;

    sprintf(temp, "Welcome [%d] to join BLE Relay Chat.", conn_handle);
    len = strlen(temp) + 1;
    send_to_except(0xff, temp, len);
}


static uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
        (void) con_handle;
    if (att_handle == handle_char_read){
        const char *msg = "Hello from EATT Server";
        uint16_t len = (uint16_t)strlen(msg);
        if (offset >= len) return 0;
        uint16_t to_copy = btstack_min(buffer_size, len - offset);
        memcpy(buffer, (const uint8_t*)msg + offset, to_copy);
        return to_copy;
    }
    if (att_handle == handle_char_long){
        if (offset >= long_char_len) return 0;
        uint16_t to_copy = btstack_min(buffer_size, long_char_len - offset);
        memcpy(buffer, long_char_value + offset, to_copy);
        return to_copy;
    }
    return 0;
}

static int att_write_callback(hci_con_handle_t conn_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    (void) conn_handle;
    (void) transaction_mode; // if supporting queued writes, transaction_mode indicates execute or cancel
    if (att_handle == handle_char_write){
        // trivial: print and store
        printf("Write to simple char, len=%u: ", buffer_size);
        uint16_t i;
        for (i=0;i<buffer_size;i++) printf("%02x ", buffer[i]);
        printf("\n");
        return 0; // success
    }
    if (att_handle == handle_char_long){
        // support prepare writes: we get offset > 0 for prepare
        if (offset + buffer_size > LONG_CHAR_MAX_LEN) return ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH;
        memcpy(long_char_value + offset, buffer, buffer_size);
        long_char_len = btstack_max(long_char_len, offset + buffer_size);
        return 0;
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
#if (APP_ROLE == CONST_SLAVE)
const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
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
#else
const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

#endif
#define STORAGE_SIZE 1024*4
uint8_t num_eatt_bearers = 5;
uint8_t test_storage_buffer[STORAGE_SIZE] = {0};
uint8_t gatt_client_storage[STORAGE_SIZE] = {0};
void gatt_event_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size) {
    platform_printf("my_gatt_callback: packet_type=%d, channel=0x%04X, size=%d\n", packet_type, channel, size);
    if (packet_type != HCI_EVENT_PACKET) return;
    uint8_t event = hci_event_packet_get_type(packet);
    switch (event) {
        case GATT_EVENT_MTU:
            printf("[EATT] MTU exchanged: %u\n", little_endian_read_16(packet, 4));
            break;
        case GATT_EVENT_SERVICE_QUERY_RESULT:
            printf("[EATT] Service found\n");
            break;
        case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
            test_value_handle = gatt_event_characteristic_query_result_get_handle(packet);
            printf("test_value_handle=0x%04x\n", test_value_handle);
            break;
        case GATT_EVENT_NOTIFICATION:
            {
                uint16_t len = size - 3;
                const uint8_t *value = packet + 3;
                printf("received notification: ");
                uint16_t i;
                for (i = 0; i < len; i++) {
                    printf("%02x ", value[i]);
                }
                printf("\n");
            }
            break;
        case GATT_EVENT_QUERY_COMPLETE:
            printf("[EATT] Query complete, status: 0x%02x\n", gatt_event_query_complete_get_att_status(packet));
            break;
        default:
            break;
    }
    }
void start_enhanced_gatt(hci_con_handle_t con_handle){
    uint8_t status = gatt_client_le_enhanced_connect(
        gatt_event_handler,
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
#if (APP_ROLE == CONST_MASTER)
                        start_enhanced_gatt(decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t)->handle);
#else
                        att_set_db(decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t)->handle,
                                               profile_data);
#endif
							break;
					default:
							break;
					}

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        platform_printf("I can send now\r\n");
        send_all();
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

