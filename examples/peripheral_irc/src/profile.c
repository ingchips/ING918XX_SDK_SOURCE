#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include <string.h>
#include <stdio.h>

// GATT characteristic handles
#include "../data/gatt.const"

#define INVALID_SET_ID  0xff
#define PROFILE_NUM     4

const static uint8_t adv_data0[] = {
    #include "../data/advertising.adv"
};

const static uint8_t adv_data1[] = {
    #include "../data/advertising_1.adv"
};

const static uint8_t adv_data2[] = {
    #include "../data/advertising_2.adv"
};

const static uint8_t adv_data3[] = {
    #include "../data/advertising_3.adv"
};

const bd_addr_t rand_addr0 = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00};
const bd_addr_t rand_addr1 = {0xC0, 0x01, 0x00, 0x00, 0x00, 0x00};
const bd_addr_t rand_addr2 = {0xC0, 0x02, 0x00, 0x00, 0x00, 0x00};
const bd_addr_t rand_addr3 = {0xC0, 0x03, 0x00, 0x00, 0x00, 0x00};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

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
    case HANDLE_GENERIC_INPUT:
        handle_client_speak(connection_handle, (const char *)buffer, buffer_size);
        return 0;
    case HANDLE_GENERIC_OUTPUT + 1:
        if (*(uint16_t *)buffer == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
        {
            profile_cfgs[connection_handle].notify_enable = 1;
            send_greetings(connection_handle);
        }
        else
        {
            profile_cfgs[connection_handle].notify_enable = 0;
        }

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

static void adv_enable(uint8_t handle)
{
    ext_adv_set_en_t adv_sets_en[] = { {.handle = handle, .duration = 0, .max_events = 0} };
    gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
}

static void adv_enable_all()
{
    ext_adv_set_en_t adv_sets_en[] = { {.handle = 0, .duration = 0, .max_events = 0},
                                       {.handle = 1, .duration = 0, .max_events = 0},
                                       {.handle = 2, .duration = 0, .max_events = 0},
                                       {.handle = 3, .duration = 0, .max_events = 0} };
    gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
}

static void setup_adv(uint8_t handle, const uint8_t *adv_data, uint8_t adv_len, const uint8_t *rand_addr)
{
    gap_set_adv_set_random_addr(handle, rand_addr);
    gap_set_ext_adv_para(handle,
                            CONNECTABLE_ADV_BIT | SCANNABLE_ADV_BIT | LEGACY_PDU_BIT,
                            0x0200, 0x0200,            // Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
                            PRIMARY_ADV_ALL_CHANNELS,  // Primary_Advertising_Channel_Map
                            BD_ADDR_TYPE_LE_RANDOM,    // Own_Address_Type
                            BD_ADDR_TYPE_LE_PUBLIC,    // Peer_Address_Type (ignore)
                            NULL,                      // Peer_Address      (ignore)
                            ADV_FILTER_ALLOW_ALL,      // Advertising_Filter_Policy
                            0x00,                      // Advertising_Tx_Power
                            PHY_1M,                    // Primary_Advertising_PHY
                            0,                         // Secondary_Advertising_Max_Skip
                            PHY_1M,                    // Secondary_Advertising_PHY
                            handle,                    // Advertising_SID
                            0x00);                     // Scan_Request_Notification_Enable
    gap_set_ext_adv_data(handle, adv_len, adv_data);
    gap_set_ext_scan_response_data(handle, sizeof(scan_data), (uint8_t*)scan_data);
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

        setup_adv(0, adv_data0, sizeof(adv_data0), rand_addr0);
        setup_adv(1, adv_data1, sizeof(adv_data1), rand_addr1);
        setup_adv(2, adv_data2, sizeof(adv_data2), rand_addr2);
        setup_adv(3, adv_data3, sizeof(adv_data3), rand_addr3);
        adv_enable_all();
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            {
                const le_meta_event_enh_create_conn_complete_t *conn_cmpl = decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                att_set_db(conn_cmpl->handle, profile_data);
                profile_cfgs[conn_cmpl->handle].notify_enable = 0;
                profile_cfgs[conn_cmpl->handle].to_send.size = 0;
                ll_hint_on_ce_len(conn_cmpl->handle, 10, 10);
            }
            break;
        case HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED:
            {
                const le_meta_adv_set_terminated_t *adv_term = decode_hci_le_meta_event(packet, le_meta_adv_set_terminated_t);
                profile_cfgs[adv_term->conn_handle].adv_handle = adv_term->adv_handle;
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        {
            const event_disconn_complete_t *disc_cmpl = decode_hci_event_disconn_complete(packet);
            if (profile_cfgs[disc_cmpl->conn_handle].adv_handle != INVALID_SET_ID)
            {
                adv_enable(profile_cfgs[disc_cmpl->conn_handle].adv_handle);
                profile_cfgs[disc_cmpl->conn_handle].adv_handle = INVALID_SET_ID;
            }
        }
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        send_all();
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
    // Note: security has not been enabled.
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

