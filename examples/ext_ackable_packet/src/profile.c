#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"

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

#define USER_TRIGGER_RAW_PKT        1

struct ll_raw_packet *raw_packet;
char data[300];

void show_rx(struct ll_raw_packet *packet)
{
    uint64_t air_time;
    int len;
    int rssi;
    int acked;
    int rx_status = ll_ackable_packet_get_status(packet, &acked, &air_time, data, &len, &rssi);

#if (FOR_INITIATOR)    
    platform_printf("ACK: %d\n", acked);
    if (rx_status == 0)
    {
        platform_printf("T: %llu\n", air_time);
        platform_printf("RSSI: %ddBm\n", rssi);
        data[len] = '\0';
        platform_printf("Rx: ");
        platform_printf(data);
        platform_printf("\n");
    }
    else;
#else    
    if (rx_status == 0)
    {
        platform_printf("ACK: %d\n", acked);
        platform_printf("T: %llu\n", air_time);
        platform_printf("RSSI: %ddBm\n", rssi);
        data[len] = '\0';
        platform_printf("Rx: ");
        platform_printf(data);
        platform_printf("\n");
    }
    else
        platform_printf("Rx error\n");
#endif
}

void packet_action(void)
{
    static int counter = 0;
    
    show_rx(raw_packet);
    
#if (FOR_INITIATOR)    
    sprintf(data, "PING PACKET #%d", counter++);
#else
    sprintf(data, "PONG PACKET #%d", counter++);
#endif

    ll_ackable_packet_set_tx_data(raw_packet, data, strlen(data));

    ll_ackable_packet_run(raw_packet, platform_get_us_time() + 500000, 1000 * 1000);
}

void on_raw_packet_done(struct ll_raw_packet *packet, void *user_data)
{
    btstack_push_user_msg(USER_TRIGGER_RAW_PKT, NULL, 0);
}

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_TRIGGER_RAW_PKT:
        packet_action();
        break;
    default:
        ;
    }
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
        raw_packet = ll_ackable_packet_alloc(FOR_INITIATOR, on_raw_packet_done, NULL);
        ll_raw_packet_set_param(raw_packet,
                          10,   // tx_power
                          3,   // phy_channel_id
                          1,    // phy
                          0x8E89BED6,//0xabcdef,
                          0x555555);
        btstack_push_user_msg(USER_TRIGGER_RAW_PKT, NULL, 0);
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
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
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
    // Note: security has not been enabled.
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

