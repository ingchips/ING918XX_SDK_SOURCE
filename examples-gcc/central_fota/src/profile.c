#include <stdio.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "att_dispatch.h"
#include "btstack_util.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client.h"

#include "ad_parser.h"
#include "gatt_client_util.h"
#include "ota_service.h"
#include "fota_client.h"

struct gatt_client_discoverer * discoverer = NULL;

#define DEF_UUID(var, ID)  static const uint8_t var[] = ID;

DEF_UUID(uuid_ota_ver,      INGCHIPS_UUID_OTA_VER);
DEF_UUID(uuid_ota_data,     INGCHIPS_UUID_OTA_DATA);
DEF_UUID(uuid_ota_ctrl,     INGCHIPS_UUID_OTA_CTRL);

const ota_item_t ota_items[] =
{
    {.local_storage_addr = 0x44000, .target_storage_addr = 0x44000, .target_load_addr = 0x4000,  .size = 0x22000 },
    {.local_storage_addr = 0x66000, .target_storage_addr = 0x66000, .target_load_addr = 0x26000, .size = 0x2000},
};

static ota_ver_t ota_ver =
{
    .app = { 1, 2, 0}
};

#define ENTRY   0x4000

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

static void fota_done(int err_code)
{
    platform_printf("fota_done: %d\n", err_code);
}

static void fully_discovered(service_node_t *first)
{
    char_node_t *char_ver = gatt_client_util_find_char_uuid128(discoverer, uuid_ota_ver);
    char_node_t *char_ctrl = gatt_client_util_find_char_uuid128(discoverer, uuid_ota_ctrl);
    char_node_t *char_data = gatt_client_util_find_char_uuid128(discoverer, uuid_ota_data);
    platform_printf("fully_discovered: %d, %d, %d, %n", char_ver->chara.value_handle, char_data->chara.value_handle, char_ctrl->chara.value_handle);
    fota_client_do_update(&ota_ver,
                         0,
                         char_ver->chara.value_handle, char_ctrl->chara.value_handle, char_data->chara.value_handle,
                         2, ota_items,
                         ENTRY,
                         fota_done);
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

static initiating_phy_config_t phy_configs[] =
{
    {
        .phy = PHY_1M,
        .conn_param =
        {
            .scan_int = 200,
            .scan_win = 200,
            .interval_min = 30,
            .interval_max = 30,
            .latency = 0,
            .supervision_timeout = 200,
            .min_ce_len = 20,
            .max_ce_len = 20
        }
    }
};

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    static const bd_addr_t rand_addr = { 0xCE, 0x06, 0x32, 0x83, 0x75, 0xBD };
    static const bd_addr_t peer_addr = { 0xCD, 0xA3, 0x28, 0x11, 0x89, 0x3E };
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
        gap_set_random_device_address(rand_addr);
        platform_printf("connecting...\n");
        gap_ext_create_connection(    INITIATING_ADVERTISER_FROM_PARAM, // Initiator_Filter_Policy,
                                      BD_ADDR_TYPE_LE_RANDOM,           // Own_Address_Type,
                                      BD_ADDR_TYPE_LE_RANDOM,           // Peer_Address_Type,
                                      peer_addr,                        // Peer_Address,
                                      sizeof(phy_configs) / sizeof(phy_configs[0]),
                                      phy_configs);
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
            {
                const le_meta_event_enh_create_conn_complete_t *conn_complete
                    = decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);

                if (conn_complete->status != 0)
                {
                    platform_reset();
                    break;
                }

                platform_printf("discovering...\n");
                discoverer = gatt_client_util_discover_all(conn_complete->handle, fully_discovered);
            }
            break;

        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        platform_printf("disconnected\n");
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
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(user_packet_handler);
    gatt_client_register_handler(user_packet_handler);

    const platform_ver_t * v = platform_get_version();
    ota_ver.platform.major = v->major;
    ota_ver.platform.minor = v->minor;
    ota_ver.platform.patch = v->patch;
    return 0;
}
