#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "str_util.h"

#define MAX_SAMPLES_PER_SLOT    5

#define SAMPLES_PER_SLOT        1
#define SAMPLING_OFFSET         12
#define CH_ID                   10

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#define RX_DELAY                300
#else
#define RX_DELAY                600
#endif

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

#ifdef CTE_ON

typedef struct
{
    int len;
    const uint8_t ant_ids[8];
} pattern_info_t;

pattern_info_t patterns[] = {
    {2, {0, 0}},
    {2, {0, 0}},
};

static int pattern_index = 0;

static int16_t samples[(8 + 74) * 2 * 24] = {0};

#ifdef USE_BASE64

#pragma pack (push, 1)

typedef struct complex
{
    int16_t i;
    int16_t q;
} complex_t;

struct le_proprietary_iq_report_header
{
    uint16_t id;
    phy_type_t rx_phy;
    uint8_t  phy_chan_id;
    int16_t  rssi;
    uint8_t  rssi_ant_id;
    uint8_t  cte_type;
    uint8_t  slot_durations;
    uint8_t  packet_status;
    uint16_t event_counter;
    uint16_t sample_count;
    uint8_t  sampling_offset;
    uint8_t  sample_count_per_slot;
};

typedef struct le_proprietary_iq_report
{
    struct le_proprietary_iq_report_header header;
    complex_t samples[0];
} le_proprietary_iq_report_t;

typedef struct le_proprietary_iq_report_full
{
    struct le_proprietary_iq_report_header header;
    complex_t samples[(8 + 74) * MAX_SAMPLES_PER_SLOT];
} le_proprietary_iq_report_full_t;

#pragma pack (pop)

le_proprietary_iq_report_full_t full_report =
{
    .header = {
        .sample_count_per_slot = SAMPLES_PER_SLOT,
        .sampling_offset = SAMPLING_OFFSET,
        .cte_type = 0,
        .slot_durations = 1,
        .phy_chan_id = CH_ID
    },
};

void show_iq(struct ll_raw_packet *packet)
{
    int count = 0;
    if ((0 != ll_raw_packet_get_iq_samples(packet, samples, &count, 0)) || (count < 1))
    {
        platform_printf("No IQ samples\n");
        return;
    }

    static char iq_str_buffer[3500];
    int len;
    sprintf(iq_str_buffer, "PRO%d:", pattern_index);
    len = strlen(iq_str_buffer);
    base64_encode((const uint8_t *)&full_report,
        sizeof(full_report.header) + sizeof(full_report.samples[0]) * full_report.header.sample_count,
        iq_str_buffer + len,
        sizeof(iq_str_buffer) - len - 1);

    platform_printf("\n%s\n", iq_str_buffer);
}

#else
void show_iq(struct ll_raw_packet *packet)
{
    int count = 0;
    int i;
    if ((0 != ll_raw_packet_get_iq_samples(packet, samples, &count, 0)) || (count < 1))
    {
        platform_printf("No IQ samples\n");
        return;
    }

    platform_printf("Pattern Index: %d\n", pattern_index);
    platform_printf("IQ samples:");
    for (i = 0; i < count; i++)
    {
        if (0 == (i & 0x3)) platform_printf("\n");
        platform_printf("%5d %+5d I, ", samples[i * 2 + 0], samples[i * 2 + 1]);
    }
}
#endif

#endif

void show_rx(struct ll_raw_packet *packet)
{
    uint64_t air_time;
    uint8_t header;
    int len;
    int rssi;

    if (ll_raw_packet_get_rx_data(packet, &air_time, &header, data, &len, &rssi) == 0)
    {
        platform_printf("T: %llu\n", air_time);
        platform_printf("H: 0x%02x\n", header);
        platform_printf("RSSI: %ddBm\n", rssi);
        data[len] = '\0';
        platform_printf("Rx: ");
        platform_printf(data);
        platform_printf("\n");

#ifdef CTE_ON
        show_iq(packet);
#endif
    }
    else
        platform_printf("Rx error\n");
}

void packet_action(void)
{
#if (FOR_TX)
    static int counter = 0;
    platform_printf("Tx done\n");
    sprintf(data, "RAW PACKET #%d", counter++);
#ifdef CTE_ON
    ll_raw_packet_set_tx_cte(raw_packet, 0, 20, 0, NULL);
#endif
    ll_raw_packet_set_tx_data(raw_packet, counter & 0x7f, data, strlen(data));
    ll_raw_packet_send(raw_packet, platform_get_us_time() + 500000);
#else
    show_rx(raw_packet);

#ifdef CTE_ON
    {
        pattern_index = pattern_index >= sizeof(patterns) / sizeof(patterns[0]) - 1 ? 0 : pattern_index + 1;
        ll_raw_packet_set_rx_cte(raw_packet,
                              0, // uint8_t cte_type,
                              1, // uint8_t slot_len,
                              patterns[pattern_index].len,
                              patterns[pattern_index].ant_ids,
                              SAMPLING_OFFSET,
                              SAMPLES_PER_SLOT);
    }
#endif
    ll_raw_packet_recv(raw_packet, platform_get_us_time() + RX_DELAY, 1000 * 1000);
#endif
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
        raw_packet = ll_raw_packet_alloc(FOR_TX, on_raw_packet_done, NULL);
        ll_raw_packet_set_param(raw_packet,
                          3,        // tx_power
                          CH_ID,    // phy_channel_id
                          1,        // phy
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

