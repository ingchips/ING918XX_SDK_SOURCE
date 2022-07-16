#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"

#include "service_console.h"
#include "key_detector.h"

// GATT characteristic handles
#include "../data/gatt.const"

const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

hci_con_handle_t handle_send = 0;
static uint8_t notify_enable = 0;

extern const char welcome_msg[];

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {

    default:
        return 0;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {
    case HANDLE_GENERIC_INPUT:
        console_rx_data((const char *)buffer, buffer_size);
        return 0;
    case HANDLE_GENERIC_OUTPUT + 1:
        handle_send = connection_handle;
        if(*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
        {
            notify_enable = 1;
            att_server_notify(handle_send, HANDLE_GENERIC_OUTPUT, (uint8_t *)welcome_msg, strlen(welcome_msg) + 1);
        }
        else
            notify_enable = 0;
        return 0;
    default:
        return 0;
    }
}

#define USER_MSG_ID_REQUEST_SEND            1
#define USER_MSG_ID_KEY_EVENT               2
#define USER_MSG_ID_FIRST_WAKE_UP           3

void stack_notify_tx_data(void)
{
    if (notify_enable)
        btstack_push_user_msg(USER_MSG_ID_REQUEST_SEND, NULL, 0);
}

void stack_on_first_wake_up(void)
{
    btstack_push_user_msg(USER_MSG_ID_FIRST_WAKE_UP, NULL, 0);
}

void on_key_event(key_press_event_t evt)
{
    btstack_push_user_msg(USER_MSG_ID_KEY_EVENT, NULL, evt);
}

void stack_set_latency(int latency)
{
    ll_set_conn_latency(handle_send, latency);
}

const static ext_adv_set_en_t adv_sets_en[1] = {{.handle = 0, .duration = 0, .max_events = 0}};

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_ID_REQUEST_SEND:
        if (notify_enable)
        {
            uint16_t len;
            uint8_t *data = console_get_clear_tx_data(&len);
            uint16_t mtu = att_server_get_mtu(handle_send) - 3;
            while (len > 0)
            {
                uint16_t block = len > mtu ? mtu : len;
                att_server_notify(handle_send, HANDLE_GENERIC_OUTPUT, data, block);
                data += block;
                len -= block;
            }
        }
        break;
    case USER_MSG_ID_KEY_EVENT:
        {
            static char str[50];
            key_press_event_t evt = (key_press_event_t)size;
            if (evt == KEY_LONG_PRESSED)
                strcpy(str, "key long pressed\n");
            else
                sprintf(str, "key pressed %d times\n", evt);

            if (notify_enable)
                att_server_notify(handle_send, HANDLE_GENERIC_OUTPUT, (uint8_t *)str, strlen(str) + 1);
        }
        break;
    case USER_MSG_ID_FIRST_WAKE_UP:
        platform_32k_rc_auto_tune();
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;
    default:
        ;
    }
}

extern int adv_tx_power;

static void setup_adv()
{
    const static bd_addr_t rand_addr = {0xFB, 0x89, 0x67, 0x45, 0x23, 0x01};
    gap_set_adv_set_random_addr(0, rand_addr);
    gap_set_ext_adv_para(0,
                            CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                            800, 800,                  // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
                            PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                            BD_ADDR_TYPE_LE_RANDOM,    // Own_Address_Type
                            BD_ADDR_TYPE_LE_PUBLIC,    // Peer_Address_Type (ignore)
                            NULL,                      // Peer_Address      (ignore)
                            ADV_FILTER_ALLOW_ALL,      // Advertising_Filter_Policy
                            adv_tx_power,              // Advertising_Tx_Power
                            PHY_1M,                    // Primary_Advertising_PHY
                            0,                         // Secondary_Advertising_Max_Skip
                            PHY_1M,                    // Secondary_Advertising_PHY
                            0x00,                      // Advertising_SID
                            0x00);                     // Scan_Request_Notification_Enable
    gap_set_ext_adv_data(0, sizeof(adv_data), (uint8_t*)adv_data);
    gap_set_ext_scan_response_data(0, sizeof(scan_data), (uint8_t*)scan_data);
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
        platform_config(PLATFORM_CFG_LL_LEGACY_ADV_INTERVAL, (1250 << 16) | 750);
        setup_adv();
        platform_set_timer(stack_on_first_wake_up, 200);
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle,
                       profile_data);
            platform_calibrate_32k();
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        setup_adv();
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        break;

    case BTSTACK_EVENT_USER_MSG:
        p_user_msg = hci_event_packet_get_user_msg(packet);
        user_msg_handler(p_user_msg->msg_id, p_user_msg->data, p_user_msg->len);
        break;

    default:
        break;
    }
}

uint32_t setup_profile(void *data, void *user_data)
{
    platform_printf("setup profile\n");
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

