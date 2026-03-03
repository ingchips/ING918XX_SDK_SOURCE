#include "platform_api.h"
#include "profile.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_util.h"
#include "btstack_defines.h"
#include "gatt_client.h"
#include "l2cap.h"
#include "sig_uuid.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "str_util.h"
#include "sm.h"
#include "btstack_mt.h"
#include "ll_api.h"
#include "bluetooth_hci.h"
#include "sig_uuid.h"
#include "att_db_util.h"
#include "btstack_sync.h"

#include "gatt_client_util.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "../data/advertising.const"

#define CONST_MASTER    0
#define CONST_SLAVE     1

#define CLIENT_SUPPORT_EATT_BEARER          (0x02)
#define SERVER_SUPPORT_EATT_BEARER          (0x01)

#ifndef APP_ROLE
#define APP_ROLE        CONST_MASTER
#endif

#define UUID_CUSTOM_SERVICE     0xFF00
#define UUID_CUSTOM_CHARACT     0xFF01

const sm_persistent_t slave_sm_persistent =
{
    .er = {1, 2, 3},
    .ir = {4, 5, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = {0xC5, 0xA5, 0x5C, 0x20, 0x87, 0xA7}
};

const sm_persistent_t master_sm_persistent =
{
    .er = {1, 2, 3},
    .ir = {4, 5, 6},
    .identity_addr_type     = BD_ADDR_TYPE_LE_RANDOM,
    .identity_addr          = {0xC6, 0xA6, 0x5C, 0x20, 0x87, 0xA7}
};

#define STORAGE_SIZE            (1024 * 4)
#define NUM_EATT_BEARERS        (5)
static uint16_t custom_char_handles[NUM_EATT_BEARERS] = {0};

#if (APP_ROLE == CONST_SLAVE)
static uint8_t eatt_server_buffer[STORAGE_SIZE] = {0};
static uint8_t client_supported_features[1] = {0};
static uint16_t handle_client_supported_features = 0;

static char custom_char_value[80] = {0};
static uint16_t custom_char_value_len = 0;

const uint8_t server_supported_features[1] =
{
    0x02,   // Enhanced ATT bearer supported
};

// GATT characteristic handles
#include "../data/gatt.const"

const static uint8_t adv_data[] = {
    #include "../data/advertising.adv"
};

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

#else
static uint8_t gatt_client_storage[STORAGE_SIZE] = {0};
static uint8_t is_server_eatt_supported = 0;

struct btstack_synced_runner  *synced_runner = NULL;

#define find_char_16(uuid)   gatt_client_util_find_char_uuid16(discoverer, uuid)

static void gatt_event_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size);
static void read_characteristic_value_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size);
#endif

#define INVALID_HANDLE  0xffff
uint16_t conn_handle = INVALID_HANDLE;

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
#if (APP_ROLE == CONST_SLAVE)
    if (att_handle == handle_client_supported_features)
    {
        if (NULL == buffer)
            return sizeof(client_supported_features);
        memcpy(buffer, client_supported_features, sizeof(client_supported_features));
        return sizeof(client_supported_features);
    }
    else if (att_handle >= custom_char_handles[0])
    {
        if (NULL == buffer)
        {
            sprintf(custom_char_value, "Id:%d, t = %llu", att_handle, platform_get_us_time());
            custom_char_value_len = strlen(custom_char_value);
            return custom_char_value_len;
        }

        memcpy(buffer, custom_char_value, custom_char_value_len);
        return custom_char_value_len;
    }
    else;
#endif
    return 0;
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
#if (APP_ROLE == CONST_SLAVE)
    if (att_handle == handle_client_supported_features)
    {
        buffer_size = buffer_size <= sizeof(client_supported_features) ? buffer_size : sizeof(client_supported_features);
        memcpy(client_supported_features, buffer, buffer_size);
        return 0;
    }
#endif
    return 0;
}

void print_fun(const char *s)
{
    printf("%s\n", s);
}

#define USER_MSG_START_EATT_CLIENT  1
#define USER_MSG_READ_CHAR          2

static void user_msg_handler(uint32_t msg_id, void *data, uint16_t size)
{
    switch (msg_id)
    {
#if (APP_ROLE == ROLE_MASTER)
    case USER_MSG_START_EATT_CLIENT:
        {
            uint8_t status = gatt_client_le_enhanced_connect(
                gatt_event_handler,
                conn_handle,
                NUM_EATT_BEARERS,
                gatt_client_storage,
                sizeof(gatt_client_storage)
            );

            if (status != 0)
                platform_raise_assertion(__FILE__, __LINE__);
            else
                iprintf("EATT client connected.\n");
        }
        break;
    case USER_MSG_READ_CHAR:
        {
            int i;
            iprintf("Try to read multiple characteristics simultaneously:\n");
            for (i = 0; i < NUM_EATT_BEARERS; i++)
            {
                if (custom_char_handles[i] == 0) break;
                uint8_t err = gatt_client_read_value_of_characteristic_using_value_handle(
                    read_characteristic_value_callback,
                    conn_handle,
                    custom_char_handles[i]);
                iprintf("* start read %d status %d\n", custom_char_handles[i], err);
                if (err) break;
            }
            iprintf("=====================\n");
        }
        break;
#endif
    default:
        break;
    }
}

#if (APP_ROLE == CONST_SLAVE)

const static ext_adv_set_en_t adv_sets_en[] = {{.handle = 0, .duration = 0, .max_events = 0}};

void start_adv()
{
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
    printf("adv started\n");
}

static uint8_t att_db_storage[800];

uint8_t *init_service()
{
    int i;
    static const char dev_name[] = "EATT";

    att_db_util_init(att_db_storage, sizeof(att_db_storage));

    att_db_util_add_service_uuid16(SIG_UUID_SERVICE_GENERIC_ACCESS);
        att_db_util_add_characteristic_uuid16(GAP_DEVICE_NAME_UUID, ATT_PROPERTY_READ, (uint8_t *)dev_name, sizeof(dev_name) - 1);

    att_db_util_add_service_uuid16(SIG_UUID_SERVICE_GENERIC_ATTRIBUTE);
        handle_client_supported_features = att_db_util_add_characteristic_uuid16(SIG_UUID_CHARACT_CLIENT_SUPPORTED_FEATURES,
            ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC, NULL, 0);
        att_db_util_add_characteristic_uuid16(SIG_UUID_CHARACT_SERVER_SUPPORTED_FEATURES,
            ATT_PROPERTY_READ, server_supported_features, sizeof(server_supported_features));

    att_db_util_add_service_uuid16(UUID_CUSTOM_SERVICE);
    for (i = 0; i < NUM_EATT_BEARERS; i++)
    {
        custom_char_handles[i] = att_db_util_add_characteristic_uuid16(UUID_CUSTOM_CHARACT + i,
            ATT_PROPERTY_READ | ATT_PROPERTY_DYNAMIC,
            NULL, 0);
    }

    return att_db_util_get_address();
}

#else

void read_characteristic_value_callback(uint8_t packet_type, uint16_t _, const uint8_t *packet, uint16_t size)
{
    switch (packet[0])
    {
    case GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT:
        {
            uint16_t value_size;
            const gatt_event_value_packet_t *value =
                gatt_event_characteristic_value_query_result_parse(packet, size, &value_size);
            if (value_size)
            {
                iprintf("VALUE of %d:\n", value->handle);
                print_hex_table(value->value, value_size, print_fun);
            }
        }
        break;
    case GATT_EVENT_QUERY_COMPLETE:
        iprintf("read_characteristic_value COMPLETE: %d\n", gatt_event_query_complete_parse(packet)->status);
        if (gatt_event_query_complete_parse(packet)->status != 0)
        {
            gap_disconnect(0);
            break;
        }
        break;
    }
}

static void timer_read_value(void)
{
    btstack_push_user_msg(USER_MSG_READ_CHAR, NULL, 0);
    platform_set_timer(timer_read_value, 16 * 139);
}

static void gatt_event_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    platform_printf("my_gatt_callback: packet_type=%d, channel=0x%04X, size=%d\n", packet_type, channel, size);
    switch (packet_type)
    {
    case GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT:
        {
            uint16_t value_size;
            const gatt_event_value_packet_t *value =
                gatt_event_characteristic_value_query_result_parse(packet, size, &value_size);
            iprintf("Value of %d:\n", value->handle);
            print_hex_table(value->value, value_size, print_fun);
        }
        break;
    }
}

static void check_server_supported_features(const uint8_t *value)
{
    iprintf("Server supported features:\n");
    if (value[0] & SERVER_SUPPORT_EATT_BEARER)
    {
        is_server_eatt_supported = 1;
        iprintf("* EATT Supported\n");
    }
    iprintf("==========================\n");
}

void discover_and_check_eatt(struct btstack_synced_runner * runner, void *user_data)
{
    static uint8_t value[251];
    uint16_t length = 0;

    int err_code = 0;
    int i;
    char_node_t *ch = NULL;
    struct gatt_client_discoverer *discoverer =
        gatt_client_sync_discover_all(synced_runner, conn_handle, &err_code);
    if (err_code != 0)
    {
        iprintf("error occurred during discovering: %d", err_code);
        goto clean_up;
    }

    gatt_client_util_dump_profile(gatt_client_util_get_first_service(discoverer), NULL, err_code);

    ch = find_char_16(SIG_UUID_CHARACT_SERVER_SUPPORTED_FEATURES);
    if (NULL == ch)
    {
        iprintf("SERVER_SUPPORTED_FEATURES not found.\n");
        goto clean_up;
    }
    printf("handl = %d\n", ch->chara.value_handle);
    length = 1;
    if ((gatt_client_sync_read_value_of_characteristic(synced_runner, conn_handle, ch->chara.value_handle, value, &length) != 0)
        || (length < 1))
    {
        iprintf("failed to read SERVER_SUPPORTED_FEATURES.\n");
        goto clean_up;
    }

    check_server_supported_features(value);
    if (is_server_eatt_supported == 0) goto clean_up;

    ch = find_char_16(SIG_UUID_CHARACT_CLIENT_SUPPORTED_FEATURES);
    if (NULL == ch)
    {
        iprintf("CLIENT_SUPPORTED_FEATURES not found.\n");
        goto clean_up;
    }
    value[0] = CLIENT_SUPPORT_EATT_BEARER;
    length = 1;
    if (gatt_client_sync_write_value_of_characteristic(synced_runner, conn_handle, ch->chara.value_handle,
        value, 1) != 0)
    {
        iprintf("CLIENT_SUPPORTED_FEATURES not found.\n");
        goto clean_up;
    }

    for (i = 0; i < NUM_EATT_BEARERS; i++)
    {
        ch = find_char_16(UUID_CUSTOM_CHARACT + i);
        if (NULL == ch) break;

        custom_char_handles[i] = ch->chara.value_handle;
    }
    btstack_push_user_msg(USER_MSG_START_EATT_CLIENT, NULL, 0);
    platform_set_timer(timer_read_value, 16 * 150);

clean_up:
    gatt_client_util_free(discoverer);
}

#define CONN_PARAM  {                   \
            .scan_int = 200,            \
            .scan_win = 100,            \
            .interval_min = 50,         \
            .interval_max = 50,         \
            .latency = 0,               \
            .supervision_timeout = 100, \
            .min_ce_len = 90,           \
            .max_ce_len = 90            \
    }

static initiating_phy_config_t phy_configs[] =
{
    {
        .phy = PHY_1M,
        .conn_param = CONN_PARAM
    },
    {
        .phy = PHY_2M,
        .conn_param = CONN_PARAM
    },
    {
        .phy = PHY_CODED,
        .conn_param = CONN_PARAM
    }
};

static void start_conn(void)
{
    is_server_eatt_supported = true;
    memset(custom_char_handles, 0, sizeof(custom_char_handles));
    gap_ext_create_connection(INITIATING_ADVERTISER_FROM_PARAM,
                                BD_ADDR_TYPE_LE_RANDOM,
                                slave_sm_persistent.identity_addr_type,
                                slave_sm_persistent.identity_addr,
                                sizeof(phy_configs) / sizeof(phy_configs[0]),
                                phy_configs);
}
#endif

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
#if (APP_ROLE == CONST_SLAVE)
        gap_set_adv_set_random_addr(0, slave_sm_persistent.identity_addr);
        start_adv();
#else
        synced_runner = btstack_create_sync_runner(0);
        gap_set_random_device_address(master_sm_persistent.identity_addr);
        start_conn();
#endif
        break;

    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet))
        {

        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            {
                const le_meta_event_enh_create_conn_complete_t * complete =
                    decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t);

                iprintf("connection complete: status = %d\n", complete->status);

                if (complete->status != 0) break;

                conn_handle = complete->handle;

#if (APP_ROLE == CONST_SLAVE)
                att_set_db(conn_handle, att_db_storage);
                sm_request_pairing(conn_handle);
#endif
            }
            break;
        default:
            break;
        }

        break;
    case HCI_EVENT_DISCONNECTION_COMPLETE:
        {
            iprintf("disconnected\n");
#if (APP_ROLE == CONST_SLAVE)
            start_adv();
#else
            platform_reset();
#endif
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

static void sm_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case SM_EVENT_JUST_WORKS_REQUEST:
        iprintf("JUST_WORKS confirmed\n");
        sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
        break;
    case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
        platform_printf("===================\npasskey: %06d\n===================\n",
            sm_event_passkey_display_number_get_passkey(packet));
        break;
    case SM_EVENT_PASSKEY_DISPLAY_CANCEL:
        platform_printf("TODO: DISPLAY_CANCEL\n");
        break;
    case SM_EVENT_PASSKEY_INPUT_NUMBER:
        // TODO: intput number
        platform_printf("===================\ninput number:\n");
        break;
    case SM_EVENT_PASSKEY_INPUT_CANCEL:
        platform_printf("TODO: INPUT_CANCEL\n");
        break;
    case SM_EVENT_IDENTITY_RESOLVING_FAILED:
        platform_printf("not authourized\n");
        break;
     case SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED:
        platform_printf("authourized\n");
        break;
    case SM_EVENT_STATE_CHANGED:
        {
            const sm_event_state_changed_t *state_changed = decode_hci_event(packet, sm_event_state_changed_t);
            switch (state_changed->reason)
            {
                case SM_STARTED:
                    platform_printf("SM: STARTED\n");
                    break;
                case SM_FINAL_PAIRED:
                    platform_printf("SM: PAIRED\n");
#if (APP_ROLE == CONST_MASTER)
                    btstack_sync_run(synced_runner, discover_and_check_eatt, NULL);
#endif
                    break;
                case SM_FINAL_REESTABLISHED:
                    platform_printf("SM: REESTABLISHED\n");
                    break;
                default:
                    platform_printf("SM: FINAL ERROR: %d\n", state_changed->reason);
                    break;
            }
        }

        break;
    default:
        break;
    }
}

static btstack_packet_callback_registration_t sm_event_callback_registration  = {.callback = &sm_packet_handler};

uint32_t setup_profile(void *data, void *user_data)
{
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    gatt_client_register_handler(&user_packet_handler);
    sm_add_event_handler(&sm_event_callback_registration);
    l2cap_register_packet_handler(&user_packet_handler);
    platform_printf("role: %s\n", APP_ROLE == CONST_SLAVE ? "Slave" : "Master");
#if (APP_ROLE == CONST_SLAVE)
    init_service();
    int ret = att_server_eatt_init(NUM_EATT_BEARERS, eatt_server_buffer, STORAGE_SIZE);
    platform_printf("att_server_eatt_init: %d\n", ret);
    sm_config(1, IO_CAPABILITY_NO_INPUT_NO_OUTPUT, 0, &slave_sm_persistent);
#else
    sm_config(1, IO_CAPABILITY_NO_INPUT_NO_OUTPUT, 0, &master_sm_persistent);
#endif
    return 0;
}
