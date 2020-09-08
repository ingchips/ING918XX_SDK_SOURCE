#include <stdio.h>
#include <stdlib.h>
#include "platform_api.h"
#include "gap.h"
#include "att_db.h"
#include "btstack_event.h"

#include "sig_services.h"

#include "disp_task.h"
#include "step_calc.h"

#include "FreeRTOS.h"
#include "timers.h"

// GATT characteristic handles
#define HANDLE_DEVICE_NAME                                   3
#define HANDLE_HEART_RATE_MEASUREMENT                        6
#define HANDLE_RSC_MEASUREMENT                               10
#define HANDLE_RSC_FEATURE                                   13


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
static uint8_t rsc_notify_enable = 0;
static TimerHandle_t app_timer = 0;

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
    case HANDLE_HEART_RATE_MEASUREMENT + 1:
        handle_send = connection_handle;
        if (*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION)
            heart_meas_ctrl(INDICATION_ENABLED);
        else if (*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
            heart_meas_ctrl(NOTIFICATION_ENABLED);
        else
            heart_meas_ctrl(0);
        return 0;
    case HANDLE_RSC_MEASUREMENT + 1:
        handle_send = connection_handle;
        if (*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
            rsc_notify_enable = 1;
        else
            rsc_notify_enable = 0;
        return 0;
    default:
        return 0;
    }
}

#define USER_MSG_ID_REQUEST_SEND_HR             1
#define USER_MSG_ID_REQUEST_SEND_RSC            2

void req_send_hr_meas(void)
{
    if (heart_meas_get_enable())
        btstack_push_user_msg(USER_MSG_ID_REQUEST_SEND_HR, NULL, 0);
}

void send_hr_meas(void)
{
    if (heart_meas_get_enable() & NOTIFICATION_ENABLED)
        att_server_notify(handle_send, HANDLE_HEART_RATE_MEASUREMENT, (uint8_t*)&heart_rate_meas, sizeof(heart_rate_meas));

    if (heart_meas_get_enable() & INDICATION_ENABLED)
        att_server_indicate(handle_send, HANDLE_HEART_RATE_MEASUREMENT, (uint8_t*)&heart_rate_meas, sizeof(heart_rate_meas));
}

void send_rsc_meas(void)
{
    if (rsc_notify_enable)
        att_server_notify(handle_send, HANDLE_RSC_MEASUREMENT, (uint8_t*)&rsc_meas, sizeof(rsc_meas));
}

static void app_timer_callback(TimerHandle_t xTimer)
{
    if (rsc_notify_enable)
    {        
        rsc_meas.flags = (rand() & 0x3) > 0 ? 7 : 3;
        rsc_meas.speed = (rand() & 0xf) * 20;
        rsc_meas.cadence = rand() & 0xf;
        rsc_meas.stride_length = (rand() & 0xf) + 70;
        rsc_meas.total_distance += (rand() & 0xf) << 1;
        btstack_push_user_msg(USER_MSG_ID_REQUEST_SEND_RSC, NULL, 0);
    }
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_ID_REQUEST_SEND_HR:
        send_hr_meas();
        break;
    case USER_MSG_ID_REQUEST_SEND_RSC:
        send_rsc_meas();
        break;
    default:
        ;
    }
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};
    bd_addr_t rand_addr = {1,2,3,4,5,6};    // TODO: random address generation
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        gap_set_adv_set_random_addr(0, rand_addr);
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
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle,
                       profile_data);
            disp_item(DISP_CONNECTED);
            xTimerStart(app_timer, portMAX_DELAY);
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        heart_meas_ctrl(0);
        xTimerStop(app_timer, portMAX_DELAY);
        disp_item(DISP_DISCONNECTED);
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
    app_timer = xTimerCreate("t1",
                            pdMS_TO_TICKS(2000),
                            pdTRUE,
                            NULL,
                            app_timer_callback);
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

