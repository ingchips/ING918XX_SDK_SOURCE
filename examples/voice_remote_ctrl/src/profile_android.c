#include <stdio.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "sm.h"
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

#include "kb_service.h"

#include "profile.h"

// GATT characteristic handles
#define HANDLE_DEVICE_NAME                                   3
#define HANDLE_APPEARANCE                                    5

const sm_persistent_t sm_persistent =
{
    .er = {1, 2, 3},
    .ir = {4, 5, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = {0xC3, 0x22, 0x63, 0x14, 0x85, 0x96}
};

const static uint8_t adv_data[] = {
    #include "../data/advertising_atv2.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

#define INVALID_HANDLE 0xffff

uint8_t audio_notify_enable = 0;
static uint16_t next_block = 0;

extern uint16_t att_handle_report;
extern uint8_t kb_notify_enable;

hci_con_handle_t handle_send;

uint16_t g_audio_handle_tx;
uint16_t g_audio_handle_rx;
uint16_t g_audio_handle_ctrl;

uint8_t g_audio_tx_buf[20];
uint8_t g_audio_ctrl_buf[20];

uint16_t g_audio_char_rx_cfg = 0;
uint16_t g_audio_char_ctrl_cfg = 0;

#define ATV_GET_CAPS        0x0A
#define ATV_MIC_OPEN        0x0C
#define ATV_MIC_CLOSE       0x0D
#define AUDIO_STOP          0x00
#define AUDIO_START         0x04
#define DPAD_SELECT         0x07
#define START_SEARCH        0x08
#define AUDIO_SYNC          0x0A
#define GET_CAPS_RESP       0x0B
#define MIC_OPEN_ERROR      0x0C

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, 
                                  uint8_t * buffer, uint16_t buffer_size)
{
    kb_att_read_callback(connection_handle, att_handle, offset, buffer, buffer_size);
    return 0;
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

const uint8_t caps_rsp[] = {GET_CAPS_RESP, 0, 4,
                            0, 1,
                            0, 0x86,
                            0, 0x14};

static int handle_atv_cmd(const uint8_t *buffer, uint16_t buffer_size)
{
    uint8_t rsp;
    if (buffer_size < 1) return 0;
    switch (buffer[0])
    {
    case ATV_GET_CAPS:
        att_server_notify(handle_send, g_audio_handle_ctrl, (uint8_t *)caps_rsp, sizeof(caps_rsp));
        break;
    case ATV_MIC_OPEN:
        audio_start();
        rsp = AUDIO_START;
        att_server_notify(handle_send, g_audio_handle_ctrl, &rsp, 1);
        break;
    case ATV_MIC_CLOSE:
        audio_stop();
        rsp = AUDIO_STOP;
        att_server_notify(handle_send, g_audio_handle_ctrl, &rsp, 1);
        break;
    }
    return 0;
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, 
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    handle_send = connection_handle;
    if (att_handle == g_audio_handle_tx)
    {
        return handle_atv_cmd(buffer, buffer_size);
    }
    else if (att_handle == g_audio_handle_rx + 1)
    {
        g_audio_char_rx_cfg = *(uint16_t *)(buffer);
    }
    else if (att_handle == g_audio_handle_ctrl + 1)
    {
        g_audio_char_ctrl_cfg = *(uint16_t *)(buffer);
    }
    else
    {
        return kb_att_write_callback(connection_handle, att_handle, transaction_mode, offset, buffer, buffer_size);
    }
    return 0;
}

#define USER_MSG_ID_REQUEST_SEND_AUDIO            1
#define USER_MSG_ID_REQUEST_SEND_KB               2
#define USER_MSG_ID_START_TALKING                 3

extern my_kb_report_t report;

void audio_trigger_send(void)
{
    if (audio_notify_enable)
        btstack_push_user_msg(USER_MSG_ID_REQUEST_SEND_AUDIO, NULL, 0);
}

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

static void send_audio_data()
{
    if (!audio_notify_enable)
        return;

    uint16_t curr = audio_get_curr_block();
    if (next_block != curr)
    {
        att_server_notify(handle_send, g_audio_handle_rx, audio_get_block_buff(next_block), VOICE_BUF_BLOCK_SIZE);
        next_block++;
        if (next_block >= VOICE_BUF_BLOCK_NUM) next_block = 0;
        if (next_block != curr)
            att_server_request_can_send_now_event(handle_send);
    }
}

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
    case USER_MSG_ID_REQUEST_SEND_KB:
        att_server_notify(handle_send, att_handle_report, (uint8_t*)&report, sizeof(report));
        if (report.k_map & 0x8)
        {
            uint8_t rsp = START_SEARCH;
            att_server_notify(handle_send, g_audio_handle_ctrl, &rsp, 1);
        }
        break;
    case USER_MSG_ID_START_TALKING:
        audio_start();
        break;
    default:
        ;
    }
}

uint8_t *init_service(void);

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;

        gap_set_adv_set_random_addr(0, sm_persistent.identity_addr);
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
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle, init_service());
            break;
        default:
            break;
        }

        break;

     case HCI_EVENT_DISCONNECTION_COMPLETE:
        gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        kb_send_report();
        break;

    case BTSTACK_EVENT_USER_MSG:
        p_user_msg = hci_event_packet_get_user_msg(packet);
        user_msg_handler(p_user_msg->msg_id, p_user_msg->data, p_user_msg->len);
        break;
    default:
        break;
    }
}

#define GATT_CLIENT_CHARACTERISTICS_DESC_REPORT_REF     0x2908

static uint8_t att_db_storage[800];

const uint8_t UUID_VOICE[]     = {0xAB, 0x5E, 0x00, 0x01, 0x5A, 0x21, 0x4F, 0x05, 0xBC, 0x7D, 0xAF, 0x01, 0xF6, 0x17, 0xB6, 0x64};
const uint8_t UUID_CHAR_TX[]   = {0xAB, 0x5E, 0x00, 0x02, 0x5A, 0x21, 0x4F, 0x05, 0xBC, 0x7D, 0xAF, 0x01, 0xF6, 0x17, 0xB6, 0x64};
const uint8_t UUID_CHAR_RX[]   = {0xAB, 0x5E, 0x00, 0x03, 0x5A, 0x21, 0x4F, 0x05, 0xBC, 0x7D, 0xAF, 0x01, 0xF6, 0x17, 0xB6, 0x64};
const uint8_t UUID_CHAR_CTRL[] = {0xAB, 0x5E, 0x00, 0x04, 0x5A, 0x21, 0x4F, 0x05, 0xBC, 0x7D, 0xAF, 0x01, 0xF6, 0x17, 0xB6, 0x64};

uint8_t *init_service()
{
    const char dev_name[] = "ING Voice Control";
    const uint16_t appearance = 0x03C1;

    att_db_util_init(att_db_storage, sizeof(att_db_storage));
    
    att_db_util_add_service_uuid16(GAP_SERVICE_UUID);
    att_db_util_add_characteristic_uuid16(GAP_DEVICE_NAME_UUID, ATT_PROPERTY_READ, (uint8_t *)dev_name, sizeof(dev_name) - 1);
    // Characteristic Appearance: 2A01 
    att_db_util_add_characteristic_uuid16(0x2A01, ATT_PROPERTY_READ, (uint8_t *)&appearance, 2);
    
    init_kb_service();
       
    att_db_util_add_service_uuid128((uint8_t *)UUID_VOICE);

    g_audio_handle_tx = att_db_util_add_characteristic_uuid128((uint8_t *)UUID_CHAR_TX,
        ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC,
        NULL, 0);
    
    g_audio_handle_rx = att_db_util_add_characteristic_uuid128((uint8_t *)UUID_CHAR_RX,
        ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC,
        NULL, 0);
        
    g_audio_handle_ctrl = att_db_util_add_characteristic_uuid128((uint8_t *)UUID_CHAR_CTRL,
        ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC,
        NULL, 0);

    return att_db_util_get_address();
}

static void sm_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);

    if (packet_type != HCI_EVENT_PACKET) return;
    switch (event)
    {
    case SM_EVENT_JUST_WORKS_REQUEST:
        sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
        break;
    default:
        break;
    }
}

static btstack_packet_callback_registration_t sm_event_callback_registration  = {.callback = &sm_packet_handler};

uint32_t setup_profile(void *data, void *user_data)
{
    sm_add_event_handler(&sm_event_callback_registration);
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    sm_config(IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
              0,
              &sm_persistent);
    return 0;
}
