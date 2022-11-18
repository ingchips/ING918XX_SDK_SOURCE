#include <stdio.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include "sm.h"
#include "brpc.h"

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    send_att_read_event(connection_handle, att_handle);
    return ATT_DEFERRED_READ;
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    send_att_write_event(connection_handle, att_handle, transaction_mode, offset, buffer, buffer_size);
    return 0;
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    if (packet_type != HCI_EVENT_PACKET) return;
    uint8_t event = hci_event_packet_get_type(packet);
    send_hci_event(packet, size);
    switch (event)
    {
    case HCI_EVENT_LE_META:
        break;
    case HCI_EVENT_DISCONNECTION_COMPLETE:
        {
            const event_disconn_complete_t *complete = decode_hci_event_disconn_complete(packet);
            cleanup_packet_handlers(complete->conn_handle);
        }
        break;
    default:
        break;
    }
}

static void gatt_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    send_gatt_event(packet_type, channel, packet, size);
}

static void sm_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);

    if (packet_type != HCI_EVENT_PACKET) return;

    send_sm_event(packet, size);
}

static btstack_packet_callback_registration_t hci_event_callback_registration = {.callback = &user_packet_handler};
static btstack_packet_callback_registration_t sm_event_callback_registration  = {.callback = &sm_packet_handler};

uint32_t setup_profile(void *data, void *user_data)
{
    platform_printf("setup profile\n");
    brpc_init();
    sm_add_event_handler(&sm_event_callback_registration);
    att_server_init(att_read_callback, att_write_callback);
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    gatt_client_register_handler(gatt_packet_handler);
    return 0;
}

