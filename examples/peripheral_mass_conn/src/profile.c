#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"

// GATT characteristic handles
#include "../data/gatt.const"

#ifndef MAX_CONN_NUMBER
    #if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
        #define MAX_CONN_NUMBER     26
        #define MAX_CONN_CENTRAL    24
    #elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
        #define MAX_CONN_NUMBER     10
        #define MAX_CONN_CENTRAL    MAX_CONN_NUMBER
    #elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
        #define MAX_CONN_NUMBER     5
        #define MAX_CONN_CENTRAL    MAX_CONN_NUMBER
    #else
        #error unknown INGCHIPS_FAMILY
    #endif
#endif

#define INVALID_HANDLE  0xffff

hci_con_handle_t conn_handles[MAX_CONN_NUMBER];

const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

void show_heap(void)
{
    static char buffer[200];
    platform_heap_status_t status;
    platform_get_heap_status(&status);
    sprintf(buffer, "heap status:\n"
                    "    free: %d B\n"
                    "min free: %d B\n", status.bytes_free, status.bytes_minimum_ever_free);
    platform_printf(buffer, strlen(buffer) + 1);
}

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
    switch (att_handle)
    {
    case HANDLE_DATE_RECEPTION:
        att_server_notify(connection_handle, HANDLE_DATE_NOTIFICATION, (uint8_t *)buffer, buffer_size);
        return 0;
    default:
        return 0;
    }
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

static bd_addr_t rand_addr = { 0xC3, 0xD4, 0x17, 0x9A, 0xED, 0x00 };
int adv_started;

const static ext_adv_set_en_t adv_sets_en[] = { {.handle = 0, .duration = 0, .max_events = 0} };

static void setup_adv(void)
{
    int i;
    if (adv_started)
    {
#if (MAX_CONN_CENTRAL < MAX_CONN_NUMBER)
        if (rand_addr[BD_ADDR_LEN - 1] < MAX_CONN_CENTRAL)
            return;

        for (i = 0; i < MAX_CONN_CENTRAL; i++)
        {
            if (conn_handles[i] == INVALID_HANDLE)
                break;
        }

        if (i >= MAX_CONN_CENTRAL)
            return;

        adv_started = 0;
        gap_set_ext_adv_enable(0, 0, NULL);
#else
        return;
#endif
    }

    for (i = 0; i < MAX_CONN_NUMBER; i++)
    {
        if (conn_handles[i] == INVALID_HANDLE)
            break;
    }
    if (i >= MAX_CONN_NUMBER)
    {
        platform_printf("ALL Connected\n");
        return;
    }

    platform_printf("ADDR: %d\n", i);
    rand_addr[BD_ADDR_LEN - 1] = i;
    gap_set_adv_set_random_addr(0, rand_addr);
    gap_set_ext_adv_para(0,
                            CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                            0x0a0, 0x0a0,              // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
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
    adv_started = 1;
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
        setup_adv();
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
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            {
                hci_con_handle_t handle = decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t)->handle;
                att_set_db(handle, profile_data);
                ll_hint_on_ce_len(handle, 7, 7);
                show_heap();
            }
            break;
        case HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED:
            {
                const le_meta_adv_set_terminated_t *terminated =
                    decode_hci_le_meta_event(packet, le_meta_adv_set_terminated_t);
                adv_started = 0;
                if (terminated->status == 0)
                {
                    conn_handles[rand_addr[BD_ADDR_LEN - 1]] = terminated->conn_handle;
                }
                setup_adv();
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        {
            const event_disconn_complete_t *disconn = decode_hci_event_disconn_complete(packet);
            int i;
            for (i = 0; i < MAX_CONN_NUMBER; i++)
                if (conn_handles[i] == disconn->conn_handle)
                    conn_handles[i] = INVALID_HANDLE;
            setup_adv();
            show_heap();
        }
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
    show_heap();
    memset(conn_handles, 0xff, sizeof(conn_handles));
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

