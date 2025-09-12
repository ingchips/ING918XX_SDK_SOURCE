#include <stdio.h>
#include <string.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "att_dispatch.h"
#include "btstack_util.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client.h"

#include "ad_parser.h"

#include "FreeRTOS.h"
#include "timers.h"

#ifndef MAX_CONN_NUMBER
    #if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
        #define MAX_CONN_NUMBER     24
    #elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
        #define MAX_CONN_NUMBER     10
    #elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
        #define MAX_CONN_NUMBER     5
    #else
        #error unknown INGCHIPS_FAMILY
    #endif
#endif

#define INVALID_HANDLE  0xffff

hci_con_handle_t conn_handles[MAX_CONN_NUMBER];

static uint8_t is_initiating = 0;
static TimerHandle_t app_timer = 0;

void show_heap(void)
{
    static char buffer[200];
    platform_heap_status_t status;
    platform_get_heap_status(&status);
    sprintf(buffer, "heap status:\n"
                    "    free: %d B\n"
                    "min free: %d B\n", status.bytes_free, status.bytes_minimum_ever_free);
    platform_printf(buffer, strlen(buffer) + 1);
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

void print_conn_state(void)
{
    int i;
    platform_printf("Conn Map:");
    for (i = 0; i < MAX_CONN_NUMBER; i++)
    {
        if ((i & 7) == 0)
            platform_printf("\n%2d: ", i);
        platform_printf(conn_handles[i] == INVALID_HANDLE ? "." : "*");
    }
    platform_printf("\n");
}

int total_conn_num(void)
{
    int i;
    int r = 0;

    for (i = 0; i < MAX_CONN_NUMBER; i++)
    {
        if (conn_handles[i] != INVALID_HANDLE)
            r++;
    }
    return r;
}

void start_scan_if_needed0(void)
{
    int i;

    print_conn_state();

    if (is_initiating) return;

    for (i = 0; i < MAX_CONN_NUMBER; i++)
    {
        if (conn_handles[i] == INVALID_HANDLE)
            break;
    }

    if (i >= MAX_CONN_NUMBER)
    {
        platform_printf("ALL Connected\n");
        gap_set_ext_scan_enable(0, 0, 0, 0);
        return;
    }

    gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
}

const static bd_addr_t rand_addr = { 0xD1, 0xF8, 0x33, 0x65, 0xF4, 0x23 };
bd_addr_t peer_addr              = { 0xC3, 0xD4, 0x17, 0x9A, 0xED, 0x00 };

static initiating_phy_config_t phy_configs[] =
{
    {
        .phy = PHY_1M,
        .conn_param =
        {
            .scan_int = 550,
            .scan_win = 520,
            .interval_min = 350,
            .interval_max = 350,
            .latency = 0,
            .supervision_timeout = 800,
            .min_ce_len = 7,
            .max_ce_len = 7
        }
    }
};

void initiate_if_needed(void)
{
    int i;
    int c = 0;

    print_conn_state();

    if (is_initiating) return;

    gap_clear_white_lists();

    for (i = 0; i < MAX_CONN_NUMBER; i++)
    {
        if (conn_handles[i] == INVALID_HANDLE)
        {
            c++;
            peer_addr[BD_ADDR_LEN - 1] = i;
            gap_add_whitelist(peer_addr, BD_ADDR_TYPE_LE_RANDOM);
        }
    }

    if (0 == c)
    {
        platform_printf("ALL Connected\n");
        gap_set_ext_scan_enable(0, 0, 0, 0);
        return;
    }

    platform_printf("connecting...\n");

    gap_ext_create_connection(INITIATING_ADVERTISER_FROM_LIST, // Initiator_Filter_Policy,
                              BD_ADDR_TYPE_LE_RANDOM,          // Own_Address_Type,
                              BD_ADDR_TYPE_LE_RANDOM,          // Peer_Address_Type,
                              NULL,                            // Peer_Address,
                              sizeof(phy_configs) / sizeof(phy_configs[0]),
                              phy_configs);
    is_initiating = 1;
    xTimerReset(app_timer, portMAX_DELAY);
}

#define USER_MSG_INITIATE_TIMOUT    5

static void app_timer_callback(TimerHandle_t xTimer)
{
    btstack_push_user_msg(USER_MSG_INITIATE_TIMOUT, NULL, 0);
}


static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
    case USER_MSG_INITIATE_TIMOUT:
        if (is_initiating)
        {
            platform_printf("initiate timout\n");
            is_initiating = 0;
            gap_create_connection_cancel();
            initiate_if_needed();
        }
        break;
    default:
        ;
    }
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const le_meta_event_enh_create_conn_complete_t *conn_complete;
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        gap_set_random_device_address(rand_addr);
        initiate_if_needed();
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            {
                int id;
                is_initiating = 0;
                xTimerStop(app_timer, portMAX_DELAY);
                conn_complete = decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);
                id = conn_complete->peer_addr[0];
                if (conn_complete->status == 0)
                {
                    conn_handles[id] = conn_complete->handle;
                    platform_printf("Connected to #%d. Total = %d\n", id, total_conn_num());
                    gatt_client_is_ready(conn_complete->handle);
                    gap_update_connection_parameters(conn_complete->handle,
                             350, 350, 0, 800, 5, 5);
                }
                else
                    platform_printf("fails\n");
                initiate_if_needed();
                show_heap();
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        {
            const event_disconn_complete_t *disconn = decode_hci_event_disconn_complete(packet);
            int i;
            for (i = 0; i < MAX_CONN_NUMBER; i++)
            {
                if (conn_handles[i] == disconn->conn_handle)
                {
                    conn_handles[i] = INVALID_HANDLE;
                    platform_printf("Disc #%d. ", i);
                }
            }
            platform_printf("Total = %d\n", total_conn_num());
            initiate_if_needed();
            show_heap();
        }
        break;

    case L2CAP_EVENT_CAN_SEND_NOW:
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
    platform_printf("setup\n");
    show_heap();
    memset(conn_handles, 0xff, sizeof(conn_handles));
    app_timer = xTimerCreate("a",
                            pdMS_TO_TICKS(5000),
                            pdFALSE,
                            NULL,
                            app_timer_callback);
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(user_packet_handler);
    gatt_client_register_handler(user_packet_handler);
    return 0;
}

