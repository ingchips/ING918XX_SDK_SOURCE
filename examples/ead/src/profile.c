#include <stdio.h>
#include "platform_api.h"
#include "att_db.h"
#include "gap.h"
#include "btstack_event.h"
#include "btstack_defines.h"
#include "profile.h"
#include <string.h>
#include "ad_parser.h"
// GATT characteristic handles
#include "../data/gatt.const"
#define ADV_DATA_TOTAL_SIZE 31
uint8_t adv_data[ADV_DATA_TOTAL_SIZE];
static uint8_t adv_data_len = 0;

#include "../data/advertising.const"

const static uint8_t scan_data[] = {
    #include "../data/scan_response.adv"
};

#include "../data/scan_response.const"

const static uint8_t profile_data[] = {
    #include "../data/gatt.profile"
};
// Simulated sensor data
static uint16_t temperature_value = 2350; // 23.50°C (in 0.01°C units)
static uint32_t counter = 0;
#define BT_EAD_KEY_SIZE 16
#define BT_EAD_IV_SIZE 8
#define BT_EAD_RANDOMIZER_SIZE 5
#define BT_EAD_NONCE_SIZE 13
#define BT_EAD_MIC_SIZE 4
#define EAD_PAYLOAD_SIZE 6
#define ADV_DEVICE_NAME "EAD Demo"

#define APP_ROLE_TX 0
#define APP_ROLE_RX 1
#ifndef APP_ROLE
#define APP_ROLE APP_ROLE_RX
#endif
static const scan_phy_config_t configs[2] =
{
    {
        .phy = PHY_1M,
        .type = SCAN_PASSIVE,
        .interval = 200,
        .window = 100
    },
    {
        .phy = PHY_CODED,
        .type = SCAN_PASSIVE,
        .interval = 200,
        .window = 100
    }
};
static const bd_addr_t rand_addr_adv = { 0xFC, 0x4A, 0x85, 0xDF, 0xDA, 0x72 };
static const bd_addr_t rand_addr_scan = { 0xFC, 0x4A, 0x85, 0xDF, 0xDA, 0x73 };
uint8_t session_key[BT_EAD_KEY_SIZE] = {0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB,
			0xCC, 0xCD, 0xCE, 0xCF};
uint8_t user_iv[BT_EAD_IV_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
static const uint8_t ead_aad = 0xEA;
static uint8_t ccm_nonce[BT_EAD_NONCE_SIZE];
static uint8_t ead_payload[EAD_PAYLOAD_SIZE];
static uint8_t ccm_out[EAD_PAYLOAD_SIZE + BT_EAD_MIC_SIZE];
static uint8_t ccm_busy = 0;

#if (APP_ROLE == APP_ROLE_RX)
#define RX_CCM_BUF_SIZE 64
static uint8_t rx_ccm_nonce[BT_EAD_NONCE_SIZE];
static uint8_t rx_ccm_in[RX_CCM_BUF_SIZE];
static uint8_t rx_ccm_out[RX_CCM_BUF_SIZE];
static uint16_t rx_plain_len = 0;
static uint8_t rx_ccm_busy = 0;
#endif

const static ext_adv_set_en_t adv_sets_en[] = { {.handle = 0, .duration = 0, .max_events = 1} };

#define TX_ADV_UPDATE_TIMER_TICKS 1600



static void build_ead_nonce(uint8_t nonce[BT_EAD_NONCE_SIZE], const uint8_t iv[BT_EAD_IV_SIZE])
{
    uint32_t randomizer = counter;

    nonce[0] = (uint8_t)(randomizer & 0xFFu);
    nonce[1] = (uint8_t)((randomizer >> 8) & 0xFFu);
    nonce[2] = (uint8_t)((randomizer >> 16) & 0xFFu);
    nonce[3] = (uint8_t)((randomizer >> 24) & 0xFFu);
    nonce[4] = (uint8_t)((temperature_value++ >> 8) & 0x7Fu);
    nonce[4] |= 0x80u;
    memcpy(nonce + BT_EAD_RANDOMIZER_SIZE, iv, BT_EAD_IV_SIZE);
}

static void prepare_adv_data(void);

#if (APP_ROLE == APP_ROLE_TX)
static void tx_adv_timer_cb(void);
#endif

static void ccm_complete_cb(const uint8_t *return_params, void *user_data)
{
    const event_vendor_ccm_complete_t *evt = (const event_vendor_ccm_complete_t *)return_params;
    uint8_t name_len;

    (void)user_data;
    ccm_busy = 0;

    if ((evt == NULL) || (evt->status != 0) || (evt->type != 0) || (evt->mic_size != BT_EAD_MIC_SIZE)) {
        platform_printf("CCM failed\n");
        return;
    }

    adv_data_len = 0;
    memset(adv_data, 0, sizeof(adv_data));

    adv_data[adv_data_len++] = 2;
    adv_data[adv_data_len++] = BLUETOOTH_DATA_TYPE_FLAGS;
    adv_data[adv_data_len++] = 0x06;

    name_len = (uint8_t)strlen(ADV_DEVICE_NAME);
    adv_data[adv_data_len++] = name_len + 1;
    adv_data[adv_data_len++] = BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME;
    memcpy(&adv_data[adv_data_len], ADV_DEVICE_NAME, name_len);
    adv_data_len += name_len;

    if ((uint16_t)(adv_data_len + 2 + BT_EAD_RANDOMIZER_SIZE + EAD_PAYLOAD_SIZE + BT_EAD_MIC_SIZE)
        > ADV_DATA_TOTAL_SIZE) {
        platform_printf("ADV buffer too small\n");
        return;
    }

    adv_data[adv_data_len++] = 1 + BT_EAD_RANDOMIZER_SIZE + EAD_PAYLOAD_SIZE + BT_EAD_MIC_SIZE;
    adv_data[adv_data_len++] = BLUETOOTH_DATA_TYPE_ENCRYPTED_ADVERTISING_DATA;
    memcpy(&adv_data[adv_data_len], ccm_nonce, BT_EAD_RANDOMIZER_SIZE);
    adv_data_len += BT_EAD_RANDOMIZER_SIZE;
    memcpy(&adv_data[adv_data_len], evt->out_msg, EAD_PAYLOAD_SIZE + BT_EAD_MIC_SIZE);
    adv_data_len += EAD_PAYLOAD_SIZE + BT_EAD_MIC_SIZE;

    gap_set_ext_adv_data(0, adv_data_len, adv_data);
    gap_set_ext_adv_enable(1, sizeof(adv_sets_en) / sizeof(adv_sets_en[0]), adv_sets_en);
#if (APP_ROLE == APP_ROLE_TX)
    platform_set_timer(tx_adv_timer_cb, TX_ADV_UPDATE_TIMER_TICKS);
#endif
}

// Start EAD encryption via controller CCM engine.
static uint8_t ead_start_encrypt(const uint8_t *session_key, const uint8_t *iv,
                                 const uint8_t *aad, const uint8_t *payload,
                                 uint16_t payload_size, uint8_t *encrypted_payload)
{
    uint16_t aad_len = (aad != NULL) ? 1 : 0;

    (void)iv;

    if (ccm_busy) {
        return 1;
    }

    build_ead_nonce(ccm_nonce, user_iv);
    ccm_busy = 1;

    return gap_start_ccm(
        0,
        BT_EAD_MIC_SIZE,
        payload_size,
        aad_len,
        counter,
        session_key,
        ccm_nonce,
        payload,
        aad,
        encrypted_payload,
        ccm_complete_cb,
        NULL);
}

#if (APP_ROLE == APP_ROLE_RX)
static void ccm_decrypt_complete_cb(const uint8_t *return_params, void *user_data)
{
    const event_vendor_ccm_complete_t *evt = (const event_vendor_ccm_complete_t *)return_params;

    (void)user_data;
    rx_ccm_busy = 0;

    if (evt == NULL ) {
        platform_printf("CCM decrypt failed: no event data\n");
        return;
    }

    if (rx_plain_len >= EAD_PAYLOAD_SIZE) {
        uint16_t rx_temp = (uint16_t)rx_ccm_out[0] | ((uint16_t)rx_ccm_out[1] << 8);
        uint32_t rx_counter = (uint32_t)rx_ccm_out[2]
                            | ((uint32_t)rx_ccm_out[3] << 8)
                            | ((uint32_t)rx_ccm_out[4] << 16)
                            | ((uint32_t)rx_ccm_out[5] << 24);
        platform_printf("EAD RX ok: temp=%u.%02uC counter=%u\n",
                        rx_temp / 100,
                        rx_temp % 100,
                        rx_counter);
    }
}

static uint8_t ead_start_decrypt(const uint8_t *randomizer,
                                 const uint8_t *enc_and_mic,
                                 uint16_t enc_and_mic_len,
                                 const uint8_t *iv)
{
    uint16_t aad_len = 1;
    uint16_t cipher_len;

    if ((enc_and_mic_len <= BT_EAD_MIC_SIZE) || (enc_and_mic_len > RX_CCM_BUF_SIZE)) {
        return 2;
    }

    if (rx_ccm_busy) {
        return 1;
    }

    memcpy(rx_ccm_nonce, randomizer, BT_EAD_RANDOMIZER_SIZE);
    memcpy(rx_ccm_nonce + BT_EAD_RANDOMIZER_SIZE, iv, BT_EAD_IV_SIZE);

    memcpy(rx_ccm_in, enc_and_mic, enc_and_mic_len);
    cipher_len = (uint16_t)(enc_and_mic_len - BT_EAD_MIC_SIZE);
    rx_plain_len = cipher_len;
    rx_ccm_busy = 1;

    return gap_start_ccm(
        1,
        BT_EAD_MIC_SIZE,
        cipher_len,
        aad_len,
        counter,
        session_key,
        rx_ccm_nonce,
        rx_ccm_in,
        &ead_aad,
        rx_ccm_out,
        ccm_decrypt_complete_cb,
        NULL);
}

static uint8_t addr_matches_target(const bd_addr_t report_addr, const bd_addr_t target_addr)
{
    uint8_t i;

    if (memcmp(report_addr, target_addr, sizeof(bd_addr_t)) == 0) {
        return 1;
    }

    for (i = 0; i < sizeof(bd_addr_t); i++) {
        if (report_addr[i] != target_addr[sizeof(bd_addr_t) - 1 - i]) {
            return 0;
        }
    }

    return 1;
}


static void setup_scan(void)
{
    uint8_t ret;

    ret = gap_set_random_device_address(rand_addr_scan);
    if (ret != 0) {
        platform_printf("gap_set_random_device_address failed: %u\n", ret);
        return;
    }

    ret = gap_add_whitelist(rand_addr_adv, BD_ADDR_TYPE_LE_RANDOM);
    if (ret != 0) {
        platform_printf("gap_add_whitelist failed: %u\n", ret);
        return;
    }

    ret = gap_set_ext_scan_para(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_WLIST_EXCEPT_NOT_DIRECTED,
                                sizeof(configs) / sizeof(configs[0]),
                                configs);
    if (ret != 0) {
        platform_printf("gap_set_ext_scan_para failed: %u\n", ret);
        return;
    }

    ret = gap_set_ext_scan_enable(1, 0, 0, 0);   // start continuous scanning
    if (ret != 0) {
        platform_printf("gap_set_ext_scan_enable failed: %u\n", ret);
        return;
    }
}
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

#if (APP_ROLE == APP_ROLE_TX)
static void tx_adv_timer_cb(void)
{
    prepare_adv_data();
}
#endif

static void prepare_adv_data(void) {
    uint8_t ret;

    ead_payload[0] = (uint8_t)(temperature_value & 0xFFu);
    ead_payload[1] = (uint8_t)((temperature_value >> 8) & 0xFFu);
    ead_payload[2] = (uint8_t)(counter & 0xFFu);
    ead_payload[3] = (uint8_t)((counter >> 8) & 0xFFu);
    ead_payload[4] = (uint8_t)((counter >> 16) & 0xFFu);
    ead_payload[5] = (uint8_t)((counter >> 24) & 0xFFu);

    ret = ead_start_encrypt(session_key, user_iv, &ead_aad, ead_payload, sizeof(ead_payload), ccm_out);
    if (ret != 0) {
        ccm_busy = 0;
        platform_printf("gap_start_ccm failed: %u\n", ret);
        return;
    }

    counter++;
}
static void setup_adv(void)
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
    gap_set_ext_scan_response_data(0, sizeof(scan_data), (uint8_t*)scan_data);
}

static void user_packet_handler(uint8_t packet_type, uint16_t channel, const uint8_t *packet, uint16_t size)
{

#if (APP_ROLE == APP_ROLE_RX)
    const le_meta_event_ext_adv_report_t *ext_adv_evt;
#endif
    uint8_t event = hci_event_packet_get_type(packet);
    const btstack_user_msg_t *p_user_msg;
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (event)
    {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
            break;
#if (APP_ROLE == APP_ROLE_TX)
        gap_set_adv_set_random_addr(0, rand_addr_adv);
        setup_adv();
        prepare_adv_data();
#else
        setup_scan();
#endif
        break;

    case HCI_EVENT_COMMAND_COMPLETE:
        platform_printf("cmd_complete opcode=0x%04x status=%u\n",
                        hci_event_command_complete_get_command_opcode(packet),
                        hci_event_command_complete_get_return_parameters(packet)[0]);
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
#if (APP_ROLE == APP_ROLE_RX)
        case HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:
            platform_printf("Received extended advertising report\n");
            ext_adv_evt = decode_hci_le_meta_event(packet, le_meta_event_ext_adv_report_t);
            if ((ext_adv_evt != NULL) && (ext_adv_evt->num_of_reports > 0)) {

                if ((ext_adv_evt->reports[0].addr_type != BD_ADDR_TYPE_LE_RANDOM)
                    || (!addr_matches_target(ext_adv_evt->reports[0].address, rand_addr_adv))) {
                        platform_printf("Report from unknown device: addr_type=%u addr=%02X:%02X:%02X:%02X:%02X:%02X\n",
                                        ext_adv_evt->reports[0].addr_type,
                                        ext_adv_evt->reports[0].address[5], ext_adv_evt->reports[0].address[4],
                                        ext_adv_evt->reports[0].address[3], ext_adv_evt->reports[0].address[2],
                                        ext_adv_evt->reports[0].address[1], ext_adv_evt->reports[0].address[0]);
                        platform_printf("Expected addr_type=%u addr=%02X:%02X:%02X:%02X:%02X:%02X\n",
                                        BD_ADDR_TYPE_LE_RANDOM,
                                        rand_addr_adv[0], rand_addr_adv[1], rand_addr_adv[2],
                                        rand_addr_adv[3], rand_addr_adv[4], rand_addr_adv[5]);
                    platform_printf("Report from unknown device\n");
                    break;
                }

                uint16_t ead_data_len = 0;
                const uint8_t *ead_data = ad_data_from_type(ext_adv_evt->reports[0].data_len,
                                  (uint8_t *)ext_adv_evt->reports[0].data,
                                  BLUETOOTH_DATA_TYPE_ENCRYPTED_ADVERTISING_DATA,
                                  &ead_data_len);
                if ((ead_data != NULL)
                    && (ead_data_len == (BT_EAD_RANDOMIZER_SIZE + EAD_PAYLOAD_SIZE + BT_EAD_MIC_SIZE))) {
                    platform_printf("Found EAD data in advertising report\n");
                    uint8_t ret = ead_start_decrypt(ead_data,
                                    ead_data + BT_EAD_RANDOMIZER_SIZE,
                                    (uint16_t)(ead_data_len - BT_EAD_RANDOMIZER_SIZE),
                                    user_iv);
                    if (ret != 0) {
                        platform_printf("gap_start_ccm decrypt failed: %u\n", ret);
                    }
                } else if (ead_data != NULL) {
                    platform_printf("Unexpected EAD data len: %u\n", ead_data_len);
                }
            }
            break;
#endif
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE:
        case HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2:
            att_set_db(decode_hci_le_meta_event(packet, le_meta_event_enh_create_conn_complete_t)->handle,
                       profile_data);
            break;
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
    platform_printf("setup profile build@%s app_role=%d\n", __TIME__, APP_ROLE);
    // Note: security has not been enabled.
    att_server_init(att_read_callback, att_write_callback);
    hci_event_callback_registration.callback = &user_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(&user_packet_handler);
    return 0;
}

