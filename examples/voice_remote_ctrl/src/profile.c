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
#include "kb_service.h"

#include "att_db_util.h"

#include "../../hid_keyboard/src/USBHID_Types.h"
#include "../../hid_keyboard/src/USBKeyboard.h"

#include "profile.h"

// GATT characteristic handles
#define HANDLE_DEVICE_NAME                                   3
#define HANDLE_VOICE_CONTROL                                 6
#define HANDLE_VOICE_OUTPUT                                  8

static TimerHandle_t app_timer = 0;
extern int8_t mic_dig_gain;

#ifdef DEV_BOARD
const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

#else
const static uint8_t adv_pSID[] = {
    #include "../data/adv_pSID.adv"
};

const static uint8_t adv_sSID[] = {
    #include "../data/adv_sSID.adv"
};

const static uint8_t scan_data_sselfi[] = {
    #include "../data/scan_resp_sselfi.adv"
};
#endif

#define INVALID_HANDLE 0xffff

#ifndef DEV_BOARD
static uint8_t att_db_storage[800];
static uint16_t att_handle_mic;
static uint16_t att_handle_pair;
extern uint16_t att_handle_report;
extern uint8_t kb_notify_enable;
#endif

uint8_t is_paring_mode = 0;

static hci_con_handle_t handle_send = INVALID_HANDLE;

static uint16_t next_block = 0;

uint8_t audio_notify_enable = 0;

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, 
                                  uint8_t * buffer, uint16_t buffer_size)
{
#ifndef DEV_BOARD
    uint16_t r = kb_att_read_callback(connection_handle, att_handle, offset, buffer, buffer_size);
    return r != 0xffff ? r : 0;
#else
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
#endif
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, 
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
#ifndef DEV_BOARD
    int r = 0;
    if (att_handle == att_handle_pair)
    {
        xTimerStop(app_timer, portMAX_DELAY);
    }
    else if (att_handle == att_handle_mic + 1)
    {
        if (*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
        {
            next_block = 0;
            audio_notify_enable = 1;
            kb_notify_enable = 1;
            l2cap_request_connection_parameter_update(handle_send, 6, 6, 90, 400);
            gatt_client_is_ready(handle_send);
        }
        else
        {
            audio_notify_enable = 0;
            kb_notify_enable = 0;
        }
    }
    else
        r = kb_att_write_callback(connection_handle, att_handle, transaction_mode, offset, buffer, buffer_size);
    return r != -1 ? r : 0;

#else
    switch (att_handle)
    {
    case HANDLE_VOICE_CONTROL:
        mic_dig_gain = *(int8_t *)buffer;
        return 0;

    case HANDLE_VOICE_OUTPUT + 1:
        if(*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
        {
            next_block = 0;
            audio_notify_enable = 1;
            audio_start();
        }
        else
        {
            audio_notify_enable = 0;
            audio_stop();
        }
        return 0;

    default:
        return 0;
    }
#endif
}

#define USER_MSG_ID_REQUEST_SEND_AUDIO            1
#define USER_MSG_ID_REQUEST_SEND_KB               2
#define USER_MSG_ID_REQUEST_PARING_ADV            3
#define USER_MSG_ID_SEND_PARING_ADDR              4
#define USER_MSG_ID_START_TALKING                 5

#ifndef DEV_BOARD
extern my_kb_report_t report;

static void app_timer_callback(TimerHandle_t xTimer)
{
    btstack_push_user_msg(USER_MSG_ID_SEND_PARING_ADDR, NULL, 0);
}
#endif

void start_paring(void)
{
    btstack_push_user_msg(USER_MSG_ID_REQUEST_PARING_ADV, NULL, 0);
}

void audio_trigger_send(void)
{
    if (audio_notify_enable)
        btstack_push_user_msg(USER_MSG_ID_REQUEST_SEND_AUDIO, NULL, 0);
}

#ifndef DEV_BOARD
void kb_report_trigger_send(void)
{
    if (kb_notify_enable)
        btstack_push_user_msg(USER_MSG_ID_REQUEST_SEND_KB, NULL, 0);
}

void start_talking(void)
{
    if (kb_notify_enable)
        btstack_push_user_msg(USER_MSG_ID_START_TALKING, NULL, 0);
}
#endif

static void send_audio_data()
{
    if (!audio_notify_enable)
        return;

    uint16_t curr = audio_get_curr_block();
    if (next_block != curr)
    {
#ifndef DEV_BOARD
        att_server_notify(handle_send, att_handle_mic, audio_get_block_buff(next_block), VOICE_BUF_BLOCK_SIZE);
#else
        att_server_notify(handle_send, HANDLE_VOICE_OUTPUT, audio_get_block_buff(next_block), VOICE_BUF_BLOCK_SIZE);
#endif
        next_block++;
        if (next_block >= VOICE_BUF_BLOCK_NUM) next_block = 0;
        if (next_block != curr)
            att_server_request_can_send_now_event(handle_send);
    }
}

static void setup_adv(uint8_t is_paring);
const bd_addr_t rand_addr = {0xC3, 0x8E,0x05,0x3E,0x7E,0xA3};
bd_addr_t rand_addr_rev;
 
static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
#ifndef DEV_BOARD
    static uint8_t audio_on = 0;
#endif
    switch (msg_id)
    {
    case USER_MSG_ID_REQUEST_SEND_AUDIO:
        if (att_server_can_send_packet_now(handle_send))
            send_audio_data();
        else
            att_server_request_can_send_now_event(handle_send);
        break;
#ifndef DEV_BOARD
    case USER_MSG_ID_REQUEST_SEND_KB:
        if (audio_on & (0 == report.keycode))
        {
            audio_on = 0;
            audio_stop();
        }
        if (att_server_can_send_packet_now(handle_send))
            att_server_notify(handle_send, att_handle_report, (uint8_t*)&report, sizeof(report));
        else
            att_server_request_can_send_now_event(handle_send);
        break;
    case USER_MSG_ID_REQUEST_PARING_ADV:
        if (INVALID_HANDLE != handle_send)
        {
            gap_disconnect(handle_send);
            break;
        }
        setup_adv(1);
        break;
    case USER_MSG_ID_SEND_PARING_ADDR:
        att_server_notify(handle_send, att_handle_pair, (uint8_t *)&rand_addr_rev, sizeof(rand_addr_rev));
        break;
    case USER_MSG_ID_START_TALKING:
        audio_on = 1;
        audio_start();
        report.keycode = 0x91;
        att_server_notify(handle_send, att_handle_mic, (uint8_t*)&report, sizeof(report));
        break;
#endif
    default:
        ;
    }
}

const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};

static void setup_adv(uint8_t is_paring)
{    
    const bd_addr_t pairing_addr = {0xAA, 0xA9,0xA8,0xA7,0xA6,0xA5};
    is_paring_mode = is_paring;
    app_state_changed(is_paring ? APP_PAIRING : APP_WAIT_CONN);
    gap_set_ext_adv_enable(0, 0, NULL);
    gap_set_adv_set_random_addr(0, is_paring ? pairing_addr : rand_addr);
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
#ifndef DEV_BOARD
    if (is_paring)
        gap_set_ext_adv_data(0, sizeof(adv_pSID), (uint8_t*)adv_pSID);
    else
        gap_set_ext_adv_data(0, sizeof(adv_sSID), (uint8_t*)adv_sSID);
    gap_set_ext_scan_response_data(0, sizeof(scan_data_sselfi), (uint8_t*)scan_data_sselfi);
#else
    gap_set_ext_adv_data(0, sizeof(adv_data), (uint8_t*)adv_data);
    gap_set_ext_scan_response_data(0, sizeof(scan_data), (uint8_t*)scan_data);
#endif
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
        
        setup_adv(0);
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
            handle_send = decode_hci_le_meta_event(packet, le_meta_event_create_conn_complete_t)->handle;
#ifndef DEV_BOARD
            app_state_changed(APP_CONN);            
            att_set_db(handle_send, att_db_storage);
            if (is_paring_mode) xTimerStart(app_timer, portMAX_DELAY);
#else
            att_set_db(handle_send, profile_data);
#endif
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        xTimerStop(app_timer,  portMAX_DELAY);
        audio_notify_enable = 0;
        audio_stop();
        handle_send = INVALID_HANDLE;
#ifndef DEV_BOARD
        kb_notify_enable = 0;
#endif
        setup_adv(0);
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

#ifndef DEV_BOARD
void init_service()
{
    const char dev_name[] = "sSelfi";
    const uint16_t appearance = 0x0180;
    uint8_t level = 98;

    att_db_util_init(att_db_storage, sizeof(att_db_storage));
    
    att_db_util_add_service_uuid16(GAP_SERVICE_UUID);
    att_db_util_add_characteristic_uuid16(GAP_DEVICE_NAME_UUID, ATT_PROPERTY_READ, (uint8_t *)dev_name, sizeof(dev_name) - 1);
    att_db_util_add_characteristic_uuid16(SIG_UUID_CHARACT_GAP_APPEARANCE, ATT_PROPERTY_READ, (uint8_t *)&appearance, 2);

    att_db_util_add_service_uuid16(SIG_UUID_SERVICE_BATTERY_SERVICE);
    att_db_util_add_characteristic_uuid16(SIG_UUID_CHARACT_BATTERY_LEVEL,
        ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY, &level, 1);

    init_kb_service();

    att_db_util_add_service_uuid16(0x1911);
    // Mic: PCM 8bit 8kHz
    att_handle_mic = att_db_util_add_characteristic_uuid16(0x2b18,
        ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY, &level, 1); //2901
    att_db_util_add_descriptor_uuid16(SIG_UUID_DESCRIP_GATT_CHARACTERISTIC_USER_DESCRIPTION,
        ATT_PROPERTY_READ, "Mic", 3);
    // speaker
    att_db_util_add_characteristic_uuid16(0x2b19,
        ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC, &level, 1);
    att_db_util_add_descriptor_uuid16(SIG_UUID_DESCRIP_GATT_CHARACTERISTIC_USER_DESCRIPTION,
        ATT_PROPERTY_READ, "Speaker", 7);
    // ota
    att_db_util_add_characteristic_uuid16(0x2b12,
        ATT_PROPERTY_READ | ATT_PROPERTY_WRITE, &level, 1);
    att_db_util_add_descriptor_uuid16(SIG_UUID_DESCRIP_GATT_CHARACTERISTIC_USER_DESCRIPTION,
        ATT_PROPERTY_READ, "Ota", 3);
    // pair
    att_handle_pair = att_db_util_add_characteristic_uuid16(0x2b14,
        ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC, rand_addr_rev, 2);
    att_db_util_add_descriptor_uuid16(SIG_UUID_DESCRIP_GATT_CHARACTERISTIC_USER_DESCRIPTION,
        ATT_PROPERTY_READ, "pair", 4);
        
    if (att_db_util_get_size() > sizeof(att_db_storage))
        platform_printf("db size should be >= %d\n", att_db_util_get_size());
}
#endif

uint32_t setup_profile(void *data, void *user_data)
{
#ifndef DEV_BOARD
    reverse_bd_addr(rand_addr, rand_addr_rev);
    app_timer = xTimerCreate("t1",
                            pdMS_TO_TICKS(300),
                            pdTRUE,
                            NULL,
                            app_timer_callback);
    init_service();
#endif

    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}
