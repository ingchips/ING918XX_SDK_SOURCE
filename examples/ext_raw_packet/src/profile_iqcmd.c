#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_util.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include "sig_uuid.h"
#include <string.h>
#include <stdio.h>

#include "str_util.h"

// GATT characteristic handles
#define HANDLE_DEVICE_NAME                                   3

static uint8_t adv_data[31] = {
    #include "../data/advertising.adv"
};

#define INVALID_HANDLE  0xffff
uint16_t conn_handle = INVALID_HANDLE;

struct ll_raw_packet *raw_packet;

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

    default:
        return 0;
    }
}


extern int8_t adv_tx_power;

void do_set_data()
{
    gap_set_ext_adv_data(0, adv_data[0] + 1, (uint8_t *)(adv_data));
}

char data[300];
static int16_t samples[(8 + 74) * 2];
static char iq_str_buffer[500];
uint64_t start_time = 0;

#define SCAN_WINDOW         (1000 * 1000)

void show_rx(struct ll_raw_packet *packet)
{
    uint64_t air_time;
    uint8_t header;
    int len;
    int rssi;
    int count = 0;

    if (ll_raw_packet_get_rx_data(packet, &air_time, &header, data, &len, &rssi) != 0)
    {
        air_time = platform_get_us_time();
        if (air_time + 30000 < start_time + SCAN_WINDOW)
        {
            ll_raw_packet_recv(raw_packet, air_time + RX_DELAY, start_time + SCAN_WINDOW - air_time);
            return;
        }

        platform_printf("+ERR: Rx fail\n");
        return;
    }

    if ((0 != ll_raw_packet_get_iq_samples(packet, samples, &count, 0)) || (count < 1))
    {
        platform_printf("+ERR: No IQ\n");
        return;
    }

    sprintf(iq_str_buffer, "+IQ:%d,%d,",rssi,count);
    len = strlen(iq_str_buffer);
    base64_encode((const uint8_t *)samples,
        2 * count * sizeof(samples[0]),
        iq_str_buffer + len, sizeof(iq_str_buffer) - len - 1);

    platform_printf("%s\n", iq_str_buffer);
}

#define USER_MSG_SCAN_CTRL_START    4
#define USER_MSG_SCAN_CTRL_STOP     5
#define USER_MSG_RAW_PACKET_DONE    6

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_SCAN_CTRL_START:
        ll_raw_packet_set_param(raw_packet,
                          0,    // tx_power
                          size, // phy_channel_id
                          1,    // phy
                          (uint32_t)data,
                          0x555555);
        {
            static const uint8_t pattern[] = {0, 0};
            ll_raw_packet_set_rx_cte(raw_packet,
                                  0, // uint8_t cte_type,
                                  1, // uint8_t slot_len,
                                  sizeof(pattern), // uint8_t switching_pattern_len,
                                  pattern, 12, 1);
        }
        start_time = platform_get_us_time();
        ll_raw_packet_recv(raw_packet, start_time + RX_DELAY, SCAN_WINDOW);
        break;
    case USER_MSG_RAW_PACKET_DONE:
        show_rx(raw_packet);
        break;
    default:
        ;
    }
}

void on_raw_packet_done(struct ll_raw_packet *packet, void *user_data)
{
    btstack_push_user_msg(USER_MSG_RAW_PACKET_DONE, NULL, 0);
}

void scan_ctrl(int flag, int phy_chan, uint32_t acc_addr)
{
    if (flag)
        btstack_push_user_msg(USER_MSG_SCAN_CTRL_START, (void *)acc_addr, phy_chan);
    else
        btstack_push_user_msg(USER_MSG_SCAN_CTRL_STOP, NULL, 0);
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
        raw_packet = ll_raw_packet_alloc(0, on_raw_packet_done, NULL);
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        conn_handle = INVALID_HANDLE;
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        {
            const uint8_t *returns = hci_event_command_complete_get_return_parameters(packet);
            if (*returns != 0)
                platform_printf("COMMAND_COMPLETE: 0x%02x for OPCODE %04X\n",
                    *returns, hci_event_command_complete_get_command_opcode(packet));
        }
        break;
    case HCI_EVENT_COMMAND_STATUS:
        {
            const uint8_t status = hci_event_command_status_get_status(packet);
            if (status != 0)
                platform_printf("COMMAND_STATUS: 0x%02x for OPCODE %04X\n",
                    status, hci_event_command_status_get_command_opcode(packet));
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

uint32_t setup_profile(void *data, void *user_data)
{
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

