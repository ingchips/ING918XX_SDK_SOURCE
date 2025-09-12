#include <stdio.h>
#include <string.h>
#include "profile.h"
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "gatt_client.h"

#include "log.h"

#if (FreeRTOS == 1)
#warning Shell app may not work with FreeRTOS simulation
#endif

const uint8_t addr[6] = {0xFE, 0x23, 0x45, 0x78, 0xAB, 0xCD};

const static uint8_t adv_data[] = {
    #include "../data/shell_adv.adv"
};

#include "../data/shell_adv.const"

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

#include "../data/scan_response.const"

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};

#include "../data/gatt.const"

#define INVALID_HANDLE  0xffff
uint16_t mas_conn_handle = INVALID_HANDLE;
uint16_t sla_conn_handle = INVALID_HANDLE;
static int bonding_flag = 0;
uint8_t peer_feature_power_control = 0;
uint8_t peer_feature_subrate = 0;

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{

    return ATT_DEFERRED_READ;
}

void run_os(const char *cmd)
{
    char path[200] = {0};

    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
    {
        return;
    }

    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path) - 1, fp) != NULL) {
        att_server_notify(0, HANDLE_GENERIC_OUTPUT, (uint8_t *)path, strlen(path));
    }

    /* close */
    pclose(fp);
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {
    case HANDLE_GENERIC_INPUT:
        LOG_I("Cmd: %s", (const char *)buffer);
        run_os((const char *)buffer);
        break;

    default:
        break;
    }
    return 0;
}

#pragma pack (push, 1)
typedef struct read_remote_version
{
    uint8_t Status;
    uint16_t Connection_Handle;
    uint8_t Version;
    uint16_t Manufacturer_Name;
    uint16_t Subversion;
} read_remote_version_t;
#pragma pack (pop)

static void check_and_print_features(const le_meta_event_read_remote_feature_complete_t * complete)
{
    static const char features[][48] =
    {
        "LE Encryption",
        "Connection Parameters Request",
        "Extended Reject Indication",
        "Slave-initiated Features Exchange",
        "LE Ping",
        "LE Data Packet Length Extension",
        "LL Privacy",
        "Extended Scanner Filter Policies",
        "LE 2M PHY",
        "Stable Modulation Index - Transmitter",
        "Stable Modulation Index - Receiver",
        "LE Coded PHY",
        "LE Extended Advertising",
        "LE Periodic Advertising",
        "Channel Selection Algorithm #2",
        "LE Power Class 1",
        "Minimum Number of Used Channels Procedure",
        "Connection CTE Request",
        "Connection CTE Response",
        "Connectionless CTE Transmitter",
        "Connectionless CTE Receiver",
        "Antenna Switching During CTE Transmission (AoD)",
        "Antenna Switching During CTE Reception (AoA)",
        "Receiving Constant Tone Extensions",
        "Periodic Advertising Sync Transfer - Sender",
        "Periodic Advertising Sync Transfer - Recipient",
        "Sleep Clock Accuracy Updates",
        "Remote Public Key Validation",
        "Connected Isochronous Stream - Master",
        "Connected Isochronous Stream - Slave",
        "Isochronous Broadcaster",
        "Synchronized Receiver",
        "Isochronous Channels (Host Support)",
        "LE Power Control Request",
        "LE Power Control Request",
        "LE Path Loss Monitoring",
        "Periodic Advertising ADI",
        "Connection Subrating",
        "Connection Subrating (Host Support)",
        "Channel Classification",
    };
    int i;
    if (complete->status)
    {
        LOG_E("Features of peer #%d (status %d)", complete->handle, complete->status);
        return;
    }

    LOG_OK("Features of peer #%d", complete->handle);
    for (i = 0; i < sizeof(features) / sizeof(features[0]); i++)
    {
        int B_i = i >> 3;
        int b_i = i & 0x7;
        LOG_PLAIN("[%c] %s", complete->features[B_i] & (1 << b_i) ? '*' : ' ', features[i]);
    }
    peer_feature_power_control = (complete->features[4] & 0x6) == 0x6;
    peer_feature_subrate = (complete->features[4] & 0x60) == 0x60;
    LOG_I("peer_feature_power_control = %d", peer_feature_power_control);
    LOG_I("peer_feature_subrate = %d", peer_feature_subrate);
}

static const char *decode_version(int ver)
{
    switch (ver)
    {
        case 6: return "4.0";
        case 7:  return "4.1";
        case 8:  return "4.2";
        case 9:  return "5.0";
        case 10: return "5.1";
        case 11: return "5.2";
        case 12: return "5.3";
        case 13: return "5.4";
        case 14: return "5.5";
        default: return "??";
    }
}

static const char *decode_zone(le_path_loss_zone_event_t zone_entered)
{
    switch (zone_entered)
    {
    case PATH_LOSS_ZONE_ENTER_LOW:
        return "LOW";
    case PATH_LOSS_ZONE_ENTER_MIDDLE:
        return "MIDDLE";
    default:
        return "HIGH";
    }
}

static const char *decode_unified_phy(unified_phy_type_t phy)
{
    static const char phys[4][5] = {"1M", "2M", "S8", "S2"};
    return phys[phy];
}

static const char *decode_tx_power_reason(le_tx_power_reporting_reason_t reason)
{
    switch (reason)
    {
    case TX_POWER_REPORTING_REASON_LOCAL_CHANGED:
        return "LOCAL CHANGED";
    case TX_POWER_REPORTING_REASON_REMOTE_CHANGED:
        return "REMOTE CHANGED";
    default:
        return "HCI COMPLETE";
    }
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};
    const btstack_user_msg_t *p_user_msg;
    uint8_t event = hci_event_packet_get_type(packet);
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;

        gap_set_adv_set_random_addr(0, addr);
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
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            {
                const le_meta_event_enh_create_conn_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);

                if (complete->status != 0) break;

                LOG_OK("connected to %02X:%02X:%02X:%02X:%02X:%02X",
                    complete->peer_addr[5],complete->peer_addr[4],complete->peer_addr[3],
                    complete->peer_addr[2],complete->peer_addr[1],complete->peer_addr[0]);

                att_set_db(complete->handle, profile_data);
                gap_read_remote_info(complete->handle);
            }
            break;
        case HCI_SUBEVENT_LE_READ_REMOTE_USED_FEATURES_COMPLETE:
            {
                const le_meta_event_read_remote_feature_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_read_remote_feature_complete_t);
                check_and_print_features(complete);
            }
            break;
        default:
            break;
        }

        break;

    case HCI_EVENT_DISCONNECTION_COMPLETE:
        {
            const event_disconn_complete_t *complete = decode_hci_event_disconn_complete(packet);
            switch (complete->reason)
            {
            case 0x13:
                LOG_OK("Disconnected by remote");
                break;
            case 0x16:
                LOG_OK("Disconnected by local");
                break;
            default:
                LOG_E("Disconnect reason: %d", complete->reason);
                break;
            }
        }
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        {

        }
        break;

    case HCI_EVENT_COMMAND_STATUS:
        {
            const uint8_t status = hci_event_command_status_get_status(packet);
            if (status != 0)
                LOG_E("COMMAND_STATUS: 0x%02x for OPCODE %04X",
                    status, hci_event_command_status_get_command_opcode(packet));
        }
        break;

    case HCI_EVENT_READ_REMOTE_VERSION_INFORMATION_COMPLETE:
        {
            const read_remote_version_t *version = decode_event_offset(packet, read_remote_version_t, 2);
            if (version->Status == 0)
            {
                LOG_OK("Remote version\n"
                "\tVersion          : %d (%s)\n"
                "\tManufacturer Name: 0x%04X\n"
                "\tSubversion       : 0x%04X",
                version->Version, decode_version(version->Version),
                version->Manufacturer_Name, version->Subversion);
            }
            //gap_read_remote_used_features(version->Connection_Handle);
        }
        break;

    case L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST:
        LOG_OK("L2CAP_CONNECTION_PARAMETER_UPDATE_REQUEST:\n"
                        "\thandle      : %d\n"
                        "\tinterval min: %d\n"
                        "\tinterval max: %d\n"
                        "\tlatency     : %d\n"
                        "\ttimeout     : %d",
                        l2cap_event_connection_parameter_update_request_get_handle(packet),
                        l2cap_event_connection_parameter_update_request_get_interval_min(packet),
                        l2cap_event_connection_parameter_update_request_get_interval_max(packet),
                        l2cap_event_connection_parameter_update_request_get_latency(packet),
                        l2cap_event_connection_parameter_update_request_get_timeout_multiplier(packet));
        break;

    case L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE:
        LOG_OK("L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE:\n"
                        "\thandle: %d\n"
                        "\tresult: %d",
                        l2cap_event_connection_parameter_update_response_get_handle(packet),
                        l2cap_event_connection_parameter_update_response_get_result(packet));
        break;

    case L2CAP_EVENT_COMMAND_REJECT_RESPONSE:
        LOG_OK("L2CAP_COMMAND_REJECT_RESPONSE:\n"
                        "\thandle: %d\n"
                        "\treason: %d",
                        l2cap_event_command_reject_response_get_handle(packet),
                        l2cap_event_command_reject_response_get_reason(packet));
        break;

    case GATT_EVENT_MTU:
        LOG_OK("GATT client MTU updated: %d", gatt_event_mtu_get_mtu(packet));
        break;

    case ATT_EVENT_CAN_SEND_NOW:
        // add your code
        break;

    default:
        break;
    }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

uint32_t setup_profile(void *data, void *user_data)
{
    LOG_PLAIN("=== BRPC Client ===");
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

