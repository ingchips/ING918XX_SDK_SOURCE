#include <stdio.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "att_dispatch.h"
#include "btstack_util.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include "ingsoc.h"
#include "platform_util.h"
#include "rom_tools.h"

#include "ad_parser.h"
#include "gatt_client_util.h"
#include "../../examples/thermo_ota/src/secure_ota_service.h"
#include "fota_client.h"

struct gatt_client_discoverer * discoverer = NULL;

#define DEF_UUID(var, ID)  static const uint8_t var[] = ID;

DEF_UUID(uuid_ota_ver,      INGCHIPS_UUID_OTA_VER);
DEF_UUID(uuid_ota_data,     INGCHIPS_UUID_OTA_DATA);
DEF_UUID(uuid_ota_ctrl,     INGCHIPS_UUID_OTA_CTRL);
DEF_UUID(uuid_ota_pubkey,   INGCHIPS_UUID_OTA_PUBKEY);

#ifndef TARGET_FAMILY
#warning TARGET_FAMILY default to: ING918xx
#define TARGET_FAMILY       INGCHIPS_FAMILY_918
#endif

const uint32_t storage_platform_bin[INGCHIPS_FAMILY_20 + 1] =
{
    0x00044000,
    0x02041000,
    0x02041000,
};

const uint32_t storage_app_bin[INGCHIPS_FAMILY_20 + 1] =
{
    0x00074000,
    0x02071000,
    0x02071000,
};

const uint32_t entry_address[INGCHIPS_FAMILY_20 + 1] =
{
    0x00004000,
    0x02002000,
    0x02002000,
};

// a little larger than the actual size of app
#define APP_BIN_SIZE        0x6000

static ota_item_t ota_items[] =
{
    // platform.bin
    {
    },
    // app.bin
    {
        .size                   = APP_BIN_SIZE,
    },
};

static ota_ver_t ota_ver =
{
    .app = {1, 2, 0}
};

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

static void fully_discovered(service_node_t *first, void *user_data, int err_code)
{
    if (err_code)
        return;

    char_node_t *char_ver = gatt_client_util_find_char_uuid128(discoverer, uuid_ota_ver);
    char_node_t *char_ctrl = gatt_client_util_find_char_uuid128(discoverer, uuid_ota_ctrl);
    char_node_t *char_data = gatt_client_util_find_char_uuid128(discoverer, uuid_ota_data);
    char_node_t *char_pk = gatt_client_util_find_char_uuid128(discoverer, uuid_ota_pubkey);
    if (char_pk)
    {
        extern const ecc_driver_t *get_ecc_driver(void);
        platform_printf("=== SECURE FOTA ===\ngenerating secrets...");
        const ecc_driver_t *driver = get_ecc_driver();
        platform_printf("done\n");
        secure_fota_client_do_update(TARGET_FAMILY,
                            &ota_ver,
                            0,
                            char_ver->chara.value_handle,
                            char_ctrl->chara.value_handle,
                            char_data->chara.value_handle,
                            char_pk->chara.value_handle,
                            2, ota_items,
                            entry_address[TARGET_FAMILY],
                            fota_done,
                            (f_crc_t)(ROM_FUNC_ADDR_CRC),
                            driver);
    }
    else
    {
        platform_printf("=== UNSECURE FOTA ===\n");
        fota_client_do_update(TARGET_FAMILY,
                            &ota_ver,
                            0,
                            char_ver->chara.value_handle, char_ctrl->chara.value_handle, char_data->chara.value_handle,
                            2, ota_items,
                            entry_address[TARGET_FAMILY],
                            fota_done,
                            (f_crc_t)(ROM_FUNC_ADDR_CRC));
    }

    gatt_client_util_free(discoverer);
    discoverer = NULL;
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
    static const bd_addr_t peer_addr = { 0xCD, 0xA3, 0x28, 0x11, 0x89, 0x3F };
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
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            {
                const le_meta_event_enh_create_conn_complete_t *conn_complete
                    = decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);

                if (conn_complete->status != 0)
                {
                    platform_reset();
                    break;
                }

                platform_printf("discovering...\n");
                discoverer = gatt_client_util_discover_all(conn_complete->handle, fully_discovered, NULL);
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

    const struct platform_info * v = platform_inspect2(storage_platform_bin[INGCHIPS_FAMILY], TARGET_FAMILY);
    ota_ver.platform.major = v->version.major;
    ota_ver.platform.minor = v->version.minor;
    ota_ver.platform.patch = v->version.patch;
    ota_items[0].local_storage_addr     = storage_platform_bin[INGCHIPS_FAMILY];
    ota_items[0].target_storage_addr    = storage_platform_bin[TARGET_FAMILY];
    ota_items[0].target_load_addr       = entry_address[TARGET_FAMILY];
    ota_items[1].local_storage_addr     = storage_app_bin[INGCHIPS_FAMILY];
    ota_items[1].target_storage_addr    = storage_app_bin[TARGET_FAMILY];
    ota_items[1].target_load_addr       = v->app_load_addr;
    ota_items[0].size                   = ota_items[1].target_load_addr - ota_items[0].target_load_addr;

    platform_printf("OTA Version:\n"
                    "platform: v%d.%d.%d\n"
                    "     app: v%d.%d.%d\n"
                    "platform size: 0x%08x\n"
                    "app load addr: 0x%08x\n"
                    "\n",
                    ota_ver.platform.major, ota_ver.platform.minor, ota_ver.platform.patch,
                    ota_ver.app.major,      ota_ver.app.minor,      ota_ver.app.patch,
                    ota_items[0].size,
                    ota_items[1].target_load_addr);
    return 0;
}
