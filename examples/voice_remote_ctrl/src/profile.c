#include <stddef.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "l2cap.h"
#include "gatt_client.h"
#include "btstack_util.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "sig_uuid.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "audio_service.h"

#include "profile.h" 

// GATT characteristic handles
#include "../data/gatt.const"

#define CMD_DIGITAL_GAIN            0
#define CMD_MIC_OPEN                1
#define CMD_MIC_CLOSE               2

extern int8_t mic_dig_gain;

const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};


#define INVALID_HANDLE 0xffff

static hci_con_handle_t handle_send = INVALID_HANDLE;

static uint16_t next_block = 0;

uint8_t audio_notify_enable = 0;

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, 
                                  uint8_t * buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {
    case HANDLE_VOICE_CONTROL:
        if (buffer)
        {
            *(int8_t *)buffer = mic_dig_gain;
            return buffer_size;
        }
        else
            return 1;

    default:
        return 0;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

void update_conn_interval(hci_con_handle_t conn_handle, uint16_t interval)
{
    l2cap_request_connection_parameter_update(conn_handle,
        interval, interval, 0, interval > 10 ? interval : 10);
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, 
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {
    case HANDLE_VOICE_CONTROL:
        switch (buffer[0])
        {
        case CMD_DIGITAL_GAIN:
            platform_printf("CMD_DIGITAL_GAIN事件::");
            mic_dig_gain = (int8_t)buffer[1];
            platform_printf("mic_dia_gain=%d ",mic_dig_gain);
            platform_printf("...OK\r\n\n");
            break;
        case CMD_MIC_OPEN:
            next_block = 0;
            if (audio_notify_enable)
                platform_printf("CMD_MIC_OPEN事件::");
                platform_printf("函数地址：[%x]\r\n",audio_t.audio_dev_start);
                audio_t.audio_dev_start();
				//audio_start();
                platform_printf("...OK\r\n\n");
            break;
        case CMD_MIC_CLOSE:
            platform_printf("CMD_MIC_CLOSE事件::");
            audio_t.audio_dev_stop();
            platform_printf("...OK\r\n\n");  
            //audio_stop();
            break;
        }
        
        return 0;

    case HANDLE_VOICE_OUTPUT + 1:
        if(*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
        {
            audio_notify_enable = 1;
            update_conn_interval(handle_send, 12); // 15ms
        }
        else
        {
            audio_notify_enable = 0;
            audio_t.audio_dev_stop();
        }
        return 0;

    default:
        return 0;
    }
}

#define USER_MSG_ID_REQUEST_SEND_AUDIO            1

void audio_trigger_send(void)
{
    if (audio_notify_enable)
        btstack_push_user_msg(USER_MSG_ID_REQUEST_SEND_AUDIO, NULL, 0);
}

static void send_audio_data()
{
    platform_printf("send_audio_data()\r\n");
    if (!audio_notify_enable)
        return;

    uint16_t curr = audio_get_curr_block();
    if (next_block != curr)
    {
        platform_printf("att_server_notify()\r\n");       
        att_server_notify(handle_send, HANDLE_VOICE_OUTPUT, audio_get_block_buff(next_block), VOICE_BUF_BLOCK_SIZE);
        next_block++;
        if (next_block >= VOICE_BUF_BLOCK_NUM) next_block = 0;
        if (next_block != curr)
            att_server_request_can_send_now_event(handle_send);
    }
}

static void setup_adv(void);
const bd_addr_t rand_addr = {0xC3, 0x8E,0x05,0x3E,0x7E,0xA8};

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_ID_REQUEST_SEND_AUDIO:
        if (att_server_can_send_packet_now(handle_send))
            send_audio_data();
        else
            att_server_request_can_send_now_event(handle_send);
        break;
    default:
        ;
    }
}

const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};

static void setup_adv()
{
    gap_set_ext_adv_enable(0, 0, NULL);
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

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            handle_send = decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle;
            att_set_db(handle_send, profile_data);
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        audio_notify_enable = 0;
        audio_t.audio_dev_stop();
        handle_send = INVALID_HANDLE;
        setup_adv();
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        send_audio_data();
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
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}
