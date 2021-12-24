#include <stddef.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "ll_api.h"

// GATT characteristic handles
#include "../data/gatt.const"

const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const uint8_t cte_adv_data[] = {
    #include "../data/advertising_cte.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

extern void set_led_color(uint8_t r, uint8_t g, uint8_t b);
extern void start_led_breathing(void);

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
    case HANDLE_RGB_LIGHTING_CONTROL:
        set_led_color(buffer[0], buffer[1], buffer[2]);
        return 0;
    
    case HANDLE_CONSTANT_TONE_EXTENSION_ENABLE:
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

static bd_addr_t rand_addr = { 0xFD, 0xAB, 0x79, 0x08, 0x91, 0xBE };

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{    
    static const ext_adv_set_en_t adv_sets_en[] = {
        {.handle = 0, .duration = 0, .max_events = 0},
#ifndef DISABLE_CTE
        {.handle = 1, .duration = 0, .max_events = 0},
#endif
    };
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
#ifndef DISABLE_CTE
        rand_addr[4] = platform_rand() & 0xff;
        rand_addr[5] = platform_rand() & 0xff;
#endif
        gap_set_adv_set_random_addr(0, rand_addr);
        gap_set_ext_adv_para(0, 
                                CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                                0x00a1, 0x00a1,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
                                PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                                BD_ADDR_TYPE_LE_RANDOM,    // Own_Address_Type
                                BD_ADDR_TYPE_LE_PUBLIC,    // Peer_Address_Type (ignore)
                                NULL,                      // Peer_Address      (ignore)
                                ADV_FILTER_ALLOW_ALL,      // Advertising_Filter_Policy
                                100,                       // Advertising_Tx_Power
                                PHY_1M,                    // Primary_Advertising_PHY
                                0,                         // Secondary_Advertising_Max_Skip
                                PHY_1M,                    // Secondary_Advertising_PHY
                                0x00,                      // Advertising_SID
                                0x00);                     // Scan_Request_Notification_Enable
        gap_set_ext_adv_data(0, sizeof(adv_data), (uint8_t*)adv_data);
        gap_set_ext_scan_response_data(0, sizeof(scan_data), (uint8_t*)scan_data);
        
#ifndef DISABLE_CTE
        gap_set_adv_set_random_addr(1, rand_addr);
        gap_set_ext_adv_para(1, 
                            0,
                            0x0050, 0x0050,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
                            PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                            BD_ADDR_TYPE_LE_RANDOM,    // Own_Address_Type
                            BD_ADDR_TYPE_LE_PUBLIC,    // Peer_Address_Type (ignore)
                            NULL,                      // Peer_Address      (ignore)
                            ADV_FILTER_ALLOW_ALL,      // Advertising_Filter_Policy
                            100,                       // Advertising_Tx_Power
                            PHY_1M,                    // Primary_Advertising_PHY
                            0,                         // Secondary_Advertising_Max_Skip
                            PHY_1M,                    // Secondary_Advertising_PHY
                            0x00,                      // Advertising_SID
                            0x00);                     // Scan_Request_Notification_Enable
        gap_set_ext_adv_data(1, sizeof(cte_adv_data), cte_adv_data);
#endif

        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            {
                const le_meta_event_enh_create_conn_complete_t *conn_complete
                    = decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                att_set_db(conn_complete->handle, profile_data);
#ifndef DISABLE_CTE
                gap_set_connection_cte_tx_param(conn_complete->handle,
                                                (1 << CTE_AOA),
                                                2, NULL);
                gap_set_connection_cte_response_enable(conn_complete->handle, 1);
#endif
                set_led_color(0, 0, 0);
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        start_led_breathing();
        break;
    
    case HCI_EVENT_COMMAND_COMPLETE:
#ifndef DISABLE_CTE
        if (hci_event_command_complete_get_command_opcode(packet) == ((0x08 << 10) | 0x39))
            ll_attach_cte_to_adv_set(1, 0, 20, 0, NULL);
#endif
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

uint32_t setup_profile(void *data, void *user_data)
{
    platform_printf("setup profile\n");
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

