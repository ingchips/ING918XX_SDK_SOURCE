#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"

// GATT characteristic handles
#include "../data/gatt.const"

#if (CONN_ROLE != HCI_ROLE_SLAVE)
const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};
#endif

struct ll_raw_packet *paging_packet;
struct ll_raw_packet *access_packet;

static uint8_t pkt_data[300];

#define PAGING_CH           8
#define PAGING_PHY          1
#define PAGING_ACC          0xabcdef
#define PAGING_CRC_INIT     0x1234
#define ACCESS_CH           9
#define ACCESS_PHY          1
#define ACCESS_ACC          0xfedc98
#define ACCESS_CRC_INIT     0x1234

struct paging_pkt
{
    uint32_t acc_offset;
    uint32_t start_time_offset;         // offset of 1st connection event
    uint8_t rx_phy;                     // see `ll_create_conn`
    uint8_t tx_phy;
    uint32_t access_addr;
    uint32_t crc_init;
    uint32_t interval;
    uint16_t sup_timeout;
    uint8_t  channel_map[5];
    uint8_t  ch_sel_algo;
    uint8_t  hop_inc;
    uint16_t min_ce_len;
    uint16_t max_ce_len;
    uint16_t slave_latency;
    uint8_t  sleep_clk_acc;
};

#if (CONN_ROLE == HCI_ROLE_MASTER)
#define ACC_OFFSET 2000

static struct paging_pkt paging_pkt =
{
    .acc_offset = ACC_OFFSET,
    .start_time_offset = ACC_OFFSET + ACC_OFFSET,
    .rx_phy = 1,
    .tx_phy = 1,
    .access_addr = 0x91809160,
    .crc_init = 0x912,
    .interval = 1250 * 20,
    .sup_timeout = 200,
    .channel_map = {0xff, 0xff, 0xff, 0xff, 0x1f},
    .ch_sel_algo = 1,
    .hop_inc = 8,
    .min_ce_len = 20,
    .max_ce_len = 20,
    .slave_latency = 0,
    .sleep_clk_acc = 0
};
#else
static struct paging_pkt paging_pkt = {0};

// clock jitter should be caculated from sleep clock accuracy
// this is demo only
#define JITTER      (100)
#endif

static const bd_addr_t adv_addr = { 0xEC, 0xA5, 0x1B, 0x06, 0xBB, 0x33 };
static const bd_addr_t init_addr = { 0xEC, 0xA5, 0x1B, 0x06, 0xBB, 0x55 };

static uint64_t paging_time = 0;

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

#define USER_TRIGGER_PAGING             1
#define USER_CREATE_CONN                2

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_TRIGGER_PAGING:
#if (CONN_ROLE == HCI_ROLE_MASTER)
        paging_time = platform_get_us_time() + 500000;
        ll_raw_packet_send(paging_packet, paging_time);
        ll_raw_packet_recv(access_packet, paging_time + ACC_OFFSET - 50, 80000);
#else
        ll_raw_packet_recv(paging_packet, platform_get_us_time() + 800, 1000000);
#endif
        break;
    case USER_CREATE_CONN:
        ll_create_conn(
            CONN_ROLE,
            (BD_ADDR_TYPE_LE_RANDOM << 1) | BD_ADDR_TYPE_LE_RANDOM,
            adv_addr,
            init_addr,
            paging_pkt.rx_phy,
            paging_pkt.tx_phy,
            paging_pkt.access_addr,
            paging_pkt.crc_init,
            paging_pkt.interval,
            paging_pkt.sup_timeout,
            paging_pkt.channel_map,
            paging_pkt.ch_sel_algo,
            paging_pkt.hop_inc,
            paging_pkt.min_ce_len,
            paging_pkt.max_ce_len,
#if (CONN_ROLE == HCI_ROLE_MASTER)
            paging_time + paging_pkt.start_time_offset,
            paging_pkt.slave_latency,
            paging_pkt.sleep_clk_acc,
            0
#else
            paging_time + paging_pkt.start_time_offset - JITTER,
            paging_pkt.slave_latency,
            paging_pkt.sleep_clk_acc,
            JITTER * 2
#endif
            );
        break;
    default:
        ;
    }
}

void dummy_callback(struct ll_raw_packet *packet, void *user_data)
{
}

void on_access_rx(struct ll_raw_packet *packet, void *user_data)
{
    uint64_t air_time;
    uint8_t header;
    int len = 0;
    int rssi;
    
    if ((ll_raw_packet_get_rx_data(packet, &air_time, &header, pkt_data, &len, &rssi) != 0)
        || (len != sizeof(adv_addr))
        || memcmp(pkt_data, adv_addr, sizeof(adv_addr)))
    {
        platform_printf("no access: %d\n", len);
        btstack_push_user_msg(USER_TRIGGER_PAGING, NULL, 0);
        return;
    }

    btstack_push_user_msg(USER_CREATE_CONN, NULL, 0);
}

void on_paging_rx(struct ll_raw_packet *packet, void *user_data)
{
    uint8_t header;
    int len = 0;
    int rssi;
    
    if ((ll_raw_packet_get_rx_data(packet, &paging_time, &header, pkt_data, &len, &rssi) != 0)
        || (len != sizeof(struct paging_pkt)))
    {
        platform_printf("no paging\n");
        btstack_push_user_msg(USER_TRIGGER_PAGING, NULL, 0);
        return;
    }

    memcpy(&paging_pkt, pkt_data, sizeof(struct paging_pkt));
    ll_raw_packet_send(access_packet, paging_time + paging_pkt.acc_offset);
    btstack_push_user_msg(USER_CREATE_CONN, NULL, 0);
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
#if (CONN_ROLE == HCI_ROLE_MASTER)
        paging_packet = ll_raw_packet_alloc(1, dummy_callback, NULL);
        access_packet = ll_raw_packet_alloc(0, on_access_rx, NULL);
#else
        paging_packet = ll_raw_packet_alloc(0, on_paging_rx, NULL);
        access_packet = ll_raw_packet_alloc(1, dummy_callback, NULL);
#endif
        ll_raw_packet_set_param(paging_packet,
                          3,
                          PAGING_CH,
                          PAGING_PHY,
                          PAGING_ACC,
                          PAGING_CRC_INIT);
        ll_raw_packet_set_param(access_packet,
                          3,
                          ACCESS_CH,
                          ACCESS_PHY,
                          ACCESS_ACC,
                          ACCESS_CRC_INIT);
#if (CONN_ROLE == HCI_ROLE_MASTER)
        ll_raw_packet_set_tx_data(paging_packet, 0, &paging_pkt, sizeof(paging_pkt));
#else
        ll_raw_packet_set_tx_data(access_packet, 0, adv_addr, sizeof(adv_addr));
#endif
        btstack_push_user_msg(USER_TRIGGER_PAGING, NULL, 0);
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
            platform_printf("connected\n");
#if (CONN_ROLE == HCI_ROLE_MASTER)
#else
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle,
                profile_data);
#endif
            
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        platform_printf("disconnected\n");
        btstack_push_user_msg(USER_TRIGGER_PAGING, NULL, 0);
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

