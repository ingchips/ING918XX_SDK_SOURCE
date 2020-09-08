#include <stddef.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"

#include "FreeRTOS.h"
#include "timers.h"

// GATT characteristic handles


static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

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

    default:
        return 0;
    }
}

#define USER_MSG_ID_UPDATE_TEMP         1

static uint8_t * const temperature_value = adv_data + 5;

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    static int v = 0;
    switch (msg_id)
    {
    case USER_MSG_ID_UPDATE_TEMP:
        temperature_value[0] = ++v;
        if (v > 20) v = 0;
        gap_set_periodic_adv_data(0, sizeof(adv_data), (uint8_t*)adv_data);
        break;
    default:
        ;
    }
}

static void app_timer_callback(TimerHandle_t _)
{
    btstack_push_user_msg(USER_MSG_ID_UPDATE_TEMP, NULL, 0);
}

const static ext_adv_set_en_t adv_sets_en[] = { {.handle = 0, .duration = 0, .max_events = 0} };

static void setup_adv(void)
{
    gap_set_ext_adv_para(0, 
                            0,
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
    gap_set_periodic_adv_para(0, 500, 500, PERIODIC_ADV_BIT_INC_TX);
    gap_set_periodic_adv_data(0, sizeof(adv_data), (uint8_t*)adv_data);
    gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
    gap_set_periodic_adv_enable(1, 0);
#ifdef CTE
    gap_set_connectionless_cte_tx_param(0, 5,
                                        CTE_AOA,
                                        1,
                                        2, NULL);
    gap_set_connectionless_cte_tx_enable(0, 1);
#endif
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    static const bd_addr_t rand_addr = {0xC0, 0xFD, 0xDB, 0x54, 0x2A, 0xD2};
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        gap_set_adv_set_random_addr(0, rand_addr);
        setup_adv();
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        default:
            break;
        }

        break;

    case HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED:
        platform_printf("terminated\n");
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

static TimerHandle_t app_timer = 0;

uint32_t setup_profile(void *data, void *user_data)
{
    platform_printf("setup profile\n");
    // Note: security has not been enabled.
    app_timer = xTimerCreate("t1",
                            pdMS_TO_TICKS(1000),
                            pdTRUE,
                            NULL,
                            app_timer_callback);
    xTimerStart(app_timer, portMAX_DELAY);

    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

