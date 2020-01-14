// ----------------------------------------------------------------------------
// Copyright Message
// ----------------------------------------------------------------------------
//
// INGCHIPS confidential and proprietary.
// COPYRIGHT (c) 2018 by INGCHIPS
//
// All rights are reserved. Reproduction in whole or in part is
// prohibited without the written consent of the copyright owner.
//
//
// ----------------------------------------------------------------------------

#ifndef __GAP_EXTERNAL_H
#define __GAP_EXTERNAL_H

#include <stdint.h>
#include "bluetooth.h"
#include "bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct le_connection_parameter_range{
    uint16_t le_conn_interval_min;
    uint16_t le_conn_interval_max;
    uint16_t le_conn_latency_min;
    uint16_t le_conn_latency_max;
    uint16_t le_supervision_timeout_min;
    uint16_t le_supervision_timeout_max;
} le_connection_parameter_range_t;

//LE Scan type
typedef enum scan_type
{
    //Passive scan
    SCAN_PASSIVE = 0x00,
    //Active scan
    SCAN_ACTIVE = 0x01,
} scan_type_t;

void gap_set_random_device_address(const uint8_t *address);

/**
 * Disconnect connection with handle
 *
 * @param handle
 */
uint8_t gap_disconnect(hci_con_handle_t handle);

uint8_t gap_add_whitelist(const uint8_t *address,bd_addr_type_t  addtype);
uint8_t gap_remove_whitelist(const uint8_t *address,bd_addr_type_t addtype);

typedef enum phy_type
{
     PHY_1M = 0x01,
     PHY_2M = 0x02,
     PHY_CODED = 0x03,
} phy_type_t;

/*bit configure for */
typedef enum phy_bittype
{
    PHY_1M_BIT    = BIT(0),
    PHY_2M_BIT    = BIT(1),
    PHY_CODED_BIT = BIT(2),
} phy_bittype_t;

typedef uint8_t phy_bittypes_t;

typedef enum
{
    ADV_CONNECTABLE,
    ADV_SCANNABLE,
    ADV_DIRECT,
    ADV_HIGH_DUTY_DIR_ADV,
    ADV_LEGACY,
    ADV_ANONYMOUS,
    ADV_INC_TX_POWER
} adv_event_property_t;

#ifndef C2NIM
#define    CONNECTABLE_ADV_BIT       BIT(ADV_CONNECTABLE)
#define    SCANNABLE_ADV_BIT         BIT(ADV_SCANNABLE)
#define    DIRECT_ADV_BIT            BIT(ADV_DIRECT)
#define    HIGH_DUTY_CIR_DIR_ADV_BIT BIT(ADV_HIGH_DUTY_DIR_ADV)
#define    LEGACY_PDU_BIT            BIT(ADV_LEGACY)
#define    ANONY_ADV_BIT             BIT(ADV_ANONYMOUS)
#define    INC_TX_ADV_BIT            BIT(ADV_INC_TX_POWER)
#endif

typedef uint8_t adv_event_properties_t;

#define PERIODIC_ADV_BIT_INC_TX      BIT(6)
typedef uint8_t periodic_adv_properties_t;

uint8_t gap_read_phy(const uint16_t con_handle);

uint8_t gap_set_def_phy(const uint8_t all_phys, const phy_bittypes_t tx_phys, const phy_bittypes_t rx_phys);

typedef enum phy_option
{
    HOST_NO_PREFERRED_CODING,
    HOST_PREFER_S2_CODING,
    HOST_PREFER_S8_CODING
} phy_option_t;

uint8_t gap_set_phy(const uint16_t con_handle, const uint8_t all_phys, const phy_bittypes_t tx_phys,
                    const phy_bittypes_t rx_phys, const phy_option_t phy_opt);

uint8_t gap_set_adv_set_random_addr(const uint8_t adv_handle, const uint8_t *random_addr);

typedef struct scan_phy_config
{
    // Which phy to scan (valid phys: 1M & Coded)
    phy_type_t phy;
    //Scan type - 0=passive / 1=active
    scan_type_t type;
    //Scan interval, units is 625us
    uint16_t interval;
    //Scan window size, units is 625us
    uint16_t window;
} scan_phy_config_t;

//Scan filter policy
typedef enum scan_filter_policy
{
    // Accept all advertising packets except directed advertising packets not
    // addressed to this device (default).
    SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED,
    // Accept only advertising packets from devices where the advertiser¡¯s
    // address is in the White List. Directed advertising packets which are not
    // addressed to this device shall be ignored
    SCAN_ACCEPT_WLIST_EXCEPT_NOT_DIRECTED,
    // Accept all advertising packets except directed advertising packets
    // where the initiator's identity address does not address this device
    SCAN_ACCEPT_ALL_EXCEPT_IDENTITY_NOT_MATCH,
    // Accept all advertising packets except:
    // 1.  advertising packets where the advertiser's identity address is not in
    //     the White List; and
    // 2.  directed advertising packets where the initiator's identity address
    //     does not address this device
    SCAN_ACCEPT_WLIST_EXCEPT_IDENTITY_NOT_MATCH
} scan_filter_policy_t;

uint8_t gap_set_ext_scan_para(const bd_addr_type_t addr_type, const scan_filter_policy_t filter, const uint8_t config_num,
                              const scan_phy_config_t *configs);
uint8_t gap_set_ext_scan_response_data(const uint8_t adv_handle, const uint16_t length, const uint8_t *data);
uint8_t gap_set_ext_scan_enable(const uint8_t enable, const uint8_t filter, const uint16_t duration, const uint16_t period);


typedef struct ext_adv_set_en
{
    uint8_t handle;
    // Advertising duration, unit 10ms. 0 for continuous advertising
    uint16_t duration;
    // Maximum number of extended advertising events the Controller shall attempt to send prior to terminating the extended advertising
    // 0: no maximum number
    uint8_t max_events;
} ext_adv_set_en_t;

uint8_t gap_set_ext_adv_enable(const uint8_t enable, const uint8_t set_number, const ext_adv_set_en_t *adv_sets);

// channel map bit
#define PRIMARY_ADV_CH_37  BIT(0)
#define PRIMARY_ADV_CH_38  BIT(1)
#define PRIMARY_ADV_CH_39  BIT(2)
#define PRIMARY_ADV_ALL_CHANNELS    (PRIMARY_ADV_CH_37 | PRIMARY_ADV_CH_38 | PRIMARY_ADV_CH_39)

typedef uint8_t adv_channel_bits_t;

//Advertising filter policy
typedef enum adv_filter_policy
{
    //Allow both scan and connection requests from anyone
    ADV_FILTER_ALLOW_ALL    = 0x00,
    //Allow both scan req from White List devices only and connection req from anyone
    ADV_FILTER_ALLOW_SCAN_WLST_CON_ALL,
    //Allow both scan req from anyone and connection req from White List devices only
    ADV_FILTER_ALLOW_SCAN_ALL_CON_WLST,
    //Allow scan and connection requests from White List devices only
    ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST
} adv_filter_policy_t;

uint8_t gap_set_ext_adv_para(const uint8_t adv_handle,
                             const adv_event_properties_t properties,
                             const uint32_t interval_min,
                             const uint32_t interval_max,
                             const adv_channel_bits_t primary_adv_channel_map,
	                         const bd_addr_type_t own_addr_type,
	                         const bd_addr_type_t peer_addr_type,
	                         const uint8_t *peer_addr,
	                         const adv_filter_policy_t adv_filter_policy,
	                         const int8_t tx_power,
	                         const phy_type_t primary_adv_phy,
	                         const uint8_t secondary_adv_max_skip,
	                         const phy_type_t secondary_adv_phy,
	                         const uint8_t sid,
	                         const uint8_t scan_req_notification_enable);

typedef enum adv_data_frag_pref
{
    ADV_DATA_CTRL_MAY_FRAG = 0,
    ADV_DATA_CTRL_NOT_FRAG = 1
} adv_data_frag_pref_t_t;

uint8_t gap_set_ext_adv_data(const uint8_t adv_handle, uint16_t length, const uint8_t *data);

uint8_t gap_set_periodic_adv_data(const uint8_t adv_handle, uint16_t length, const uint8_t *data);
uint8_t gap_set_periodic_adv_enable(const uint8_t enable, const uint8_t adv_handle);
uint8_t gap_set_periodic_adv_para(const uint8_t adv_handle,
                                  const uint16_t interval_min,
                                  const uint16_t interval_max,
                                  const periodic_adv_properties_t properties);

uint8_t gap_clr_adv_set(void);
uint8_t gap_rmv_adv_set(const uint8_t adv_handle);

typedef enum periodic_adv_filter_policy
{
    PERIODIC_ADVERTISER_FROM_PARAM = 0,
    PERIODIC_ADVERTISER_FROM_LIST  = 1
} periodic_adv_filter_policy_t;

uint8_t gap_periodic_adv_create_sync(const periodic_adv_filter_policy_t filter_policy,
                                     const uint8_t adv_sid,
                                     const bd_addr_type_t addr_type,
                                     const uint8_t *addr,
                                     const uint16_t skip,
                                     const uint16_t sync_timeout,
                                     const uint8_t sync_cte_type
                                    );
uint8_t gap_periodic_adv_create_sync_cancel(void);
uint8_t gap_periodic_adv_term_sync(const uint16_t sync_handle);
uint8_t gap_add_dev_to_periodic_list(const uint8_t addr_type, const uint8_t *addr, const uint8_t sid);
uint8_t gap_rmv_dev_from_periodic_list(const uint8_t addr_type, const uint8_t *addr, const uint8_t sid);
uint8_t gap_clr_periodic_adv_list(void);
uint8_t gap_read_periodic_adv_list_size(void);

typedef enum initiating_filter_policy
{
    INITIATING_ADVERTISER_FROM_PARAM = 0,
    INITIATING_ADVERTISER_FROM_LIST  = 1
} initiating_filter_policy_t;

typedef struct {
    //Scan interval, units is 625us
    uint16_t scan_int;
    //Scan window size, units is 625us
    uint16_t scan_win;
    // connection interval limiations. unit is 1.25ms
    uint16_t interval_min;
    uint16_t interval_max;
    // Slave latency for the connection in number of connection events
    uint16_t latency;
    // Supervision timeout for the LE Link, unit is 10ms
    uint16_t supervision_timeout;
    // Informative parameter recommending the min/max length of connection event needed for this LE connection
    uint16_t min_ce_len;
    uint16_t max_ce_len;
} conn_para_t;

typedef struct initiating_phy_config
{
    phy_type_t phy;
    conn_para_t conn_param;
} initiating_phy_config_t;


uint8_t gap_ext_create_connection(const initiating_filter_policy_t filter_policy,
                                  const bd_addr_type_t own_addr_type,
	                              const bd_addr_type_t peer_addr_type,
	                              const uint8_t *peer_addr,
                                  const uint8_t initiating_phy_num,
                                  const initiating_phy_config_t *phy_configs);

/**
 * @brief Cancel connection process initiated by gap_ext_create_connection
 */
uint8_t gap_create_connection_cancel(void);

typedef enum
{
    CTE_AOA,
    CTE_AOD_1US,
    CTE_AOD_2US
} cte_type_t;

/**
 * @brief Set the type, length, and antenna switching pattern for the transmission
 *            of Constant Tone Extensions in any periodic advertising on the advertising set
 *            identified by the Advertising_Handle parameter.
 *
 * @param adv_handle            Used to identify an advertising set. Range: 0x00 to 0xEF
 * @param cte_len               Constant Tone Extension length in 8 ¦Ìs units. Range: 0x02 to 0x14
 * @param cte_type              CTE Type
 * @param cte_count             The number of Constant Tone Extensions to transmit in each periodic
 *                              advertising interval.
 *                              Range: 0x01 to 0x10
 * @param switching_pattern_len The number of Antenna IDs in the pattern. Range: 0x02 to 0x4B.
 * @param antenna_ids           List of Antenna IDs in the pattern
 * @return                      0: Message is sent out; Other: Message is not sent out
 */
// uint8_t gap_set_connectionless_cte_tx_param(const uint8_t       adv_handle,
//                                             const uint8_t       cte_len,
//                                             const cte_type_t    cte_type,
//                                             const uint8_t       cte_count,
//                                             const uint8_t       switching_pattern_len,
//                                             const uint8_t      *antenna_ids);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Request that the Controller enables or disables the use of Constant Tone
 *          Extensions in any periodic advertising on the advertising set identified by
 *          Advertising_Handle.
 *
 * @param adv_handle            Identifier for the advertising set in which Constant Tone Extension is
 *                              being enabled or disabled
 * @param cte_enable            Disable (0x00), Enable (0x01)
 * @return                      0: Message is sent out; Other: Message is not sent out
 */
// uint8_t gap_set_connectionless_cte_tx_enable(const uint8_t       adv_handle,
//                                              const uint8_t       cte_enable);
// WARNING: ^^^ this API is not available in this release


typedef enum
{
    CTE_SLOT_DURATION_1US,
    CTE_SLOT_DURATION_2US
} cte_slot_duration_type_t;

/**
 * @brief Request that the Controller enables or disables capturing IQ samples from the
 *          Constant Tone Extension of periodic advertising packets in the periodic
 *          advertising train identified by the Sync_Handle parameter.
 *
 * @param sync_handle           identifying the periodic advertising train.
 * @param sampling_enable       Disable (0x00), Enable (0x01)
 * @param slot_durations        combination of bits representing cte_slot_duration_type_t
 * @param max_sampled_ctes      The maximum number of Constant Tone Extensions to sample and
 *                              report in each periodic advertising interval.
 *                              Range: 0x01 to 0x10. 0x00: sample and report all CTEs.
 * @param switching_pattern_len The number of Antenna IDs in the pattern. Range: 0x02 to 0x4B.
 * @param antenna_ids           List of Antenna IDs in the pattern
 *
 * @return                      0: Message is sent out; Other: Message is not sent out
 */
// uint8_t gap_set_connectionless_iq_sampling_enable(const uint16_t      sync_handle,
//                                                   const uint8_t       sampling_enable,
//                                                   const uint8_t       slot_durations,
//                                                   const uint8_t       max_sampled_ctes,
//                                                   const uint8_t       switching_pattern_len,
//                                                   const uint8_t      *antenna_ids);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Enable or disable sampling received Constant Tone Extension fields on the
 *          connection identified by the Connection_Handle parameter and to set the
 *          antenna switching pattern and switching and sampling slot durations to be
 *          used.
 *
 * @param conn_handle           Connection handle
 * @param sampling_enable       Disable (0x00), Enable (0x01)
 * @param slot_durations        combination of bits representing cte_slot_duration_type_t
 * @param switching_pattern_len The number of Antenna IDs in the pattern. Range: 0x02 to 0x4B.
 * @param antenna_ids           List of Antenna IDs in the pattern
 * @return                      0: Message is sent out; Other: Message is not sent out
 *
 */
// uint8_t gap_set_connection_cte_rx_param(const hci_con_handle_t  conn_handle,
//                                         const uint8_t           sampling_enable,
//                                         const uint8_t           slot_durations,
//                                         const uint8_t           switching_pattern_len,
//                                         const uint8_t          *antenna_ids);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Set the antenna switching pattern and permitted Constant Tone Extension
 *          types used for transmitting Constant Tone Extensions requested by the peer
 *          device on the connection identified by the Connection_Handle parameter.
 *
 * @param conn_handle           Connection handle
 * @param cte_types             Combination of bits representing cte_type_t
 * @param switching_pattern_len The number of Antenna IDs in the pattern. Range: 0x02 to 0x4B.
 * @param antenna_ids           List of Antenna IDs in the pattern
 * @return                      0: Message is sent out; Other: Message is not sent out
 *
 */
// uint8_t gap_set_connection_cte_tx_param(const hci_con_handle_t  conn_handle,
//                                         const uint8_t           cte_types,
//                                         const uint8_t           switching_pattern_len,
//                                         const uint8_t          *antenna_ids);
// WARNING: ^^^ this API is not available in this release



/**
 * @brief Set the antenna switching pattern and permitted Constant Tone Extension
 *          types used for transmitting Constant Tone Extensions requested by the peer
 *          device on the connection identified by the Connection_Handle parameter.
 *
 * @param conn_handle           Connection handle
 * @param enable                Disable (0x00), Enable (0x01)
 * @param requested_interval    0x0000: Initiate the Constant Tone Extension Request procedure once, at the
 *                                     earliest practical opportunity
 *                              0x0001 to 0xFFFF:
 *                                  Requested interval for initiating the Constant Tone Extension Request
 *                                  procedure in number of connection events.
 * @param requested_cte_length  Minimum length of the Constant Tone Extension being requested in 8
 *                              ¦Ìs units. Range: 0x02 to 0x14.
 * @param requested_cte_type    Requested CTE type
 * @return                      0: Message is sent out; Other: Message is not sent out
 *
 */
// uint8_t gap_set_connection_cte_request_enable(const hci_con_handle_t  conn_handle,
//                                               const uint8_t           enable,
//                                               const uint16_t          requested_cte_interval,
//                                               const uint8_t           requested_cte_length,
//                                               const cte_type_t        requested_cte_type);
// WARNING: ^^^ this API is not available in this release



/**
 * @brief Request the Controller to respond to LL_CTE_REQ PDUs with LL_CTE_RSP
 *          PDUs on the specified connection.
 *
 * @param conn_handle           Connection handle
 * @param enable                Disable (0x00), Enable (0x01)
 * @return                      0: Message is sent out; Other: Message is not sent out
 */
// uint8_t gap_set_connection_cte_response_enable(const hci_con_handle_t  conn_handle,
//                                                const uint8_t           enable);
// WARNING: ^^^ this API is not available in this release

/**
 * @brief Read the switching rates, the sampling rates, the number of antennae, and the
 *          maximum length of a transmitted Constant Tone Extension supported by the
 *          Controller.
 * @return                      0: Message is sent out; Other: Message is not sent out
 */
// uint8_t gap_read_antenna_info(void);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Enable or disable reports for the periodic advertising train identified by the
 *          Sync_Handle parameter.
 *
 * @param sync_handle           identifying the periodic advertising train.
 * @param enable                Disable (0x00), Enable (0x01)
 *
 * @return                      0: Message is sent out; Other: Message is not sent out
 */
// uint8_t gap_set_periodic_adv_rx_enable(const uint16_t       sync_handle,
//                                        const uint8_t        enable);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Instruct the Controller to send synchronization information about the periodic
 *          advertising train identified by the Sync_Handle parameter to a connected
 *          device.
 *
 * @param conn_handle           Connection handle
 * @param service_data          A value provided by the app for use by the app of the peer device.
 *                              It is not used by the host or controller.
 * @param sync_handle           identifying the periodic advertising train.
 *
 * @return                      0: Message is sent out; Other: Message is not sent out
 */
// uint8_t gap_periodic_adv_sync_transfer(const hci_con_handle_t   conn_handle,
//                                        const uint16_t           service_data,
//                                        const uint16_t           sync_handle);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Instruct the Controller to send synchronization information about the periodic
 *          advertising in an advertising set to a connected device.
 *
 * @param conn_handle           Connection handle
 * @param service_data          A value provided by the app for use by the app of the peer device.
 *                              It is not used by the host or controller.
 * @param adv_handle            identifying the advertising set.
 *
 * @return                      0: Message is sent out; Other: Message is not sent out
 */
// uint8_t gap_periodic_adv_set_info_transfer(const hci_con_handle_t   conn_handle,
//                                            const uint16_t           service_data,
//                                            const uint8_t            adv_handle);
// WARNING: ^^^ this API is not available in this release



typedef enum
{
    PERIODIC_TRANS_MODE_NULL,
    PERIODIC_TRANS_MODE_SEND_EVT_DISABLE_REPORT,
    PERIODIC_TRANS_MODE_SEND_EVT_ENABLE_REPORT
} periodic_adv_sync_transfer_mode_t;

// synchronize exclusion flags
#define SYNC_EXCL_AOA            1
#define SYNC_EXCL_AOD_1US        2
#define SYNC_EXCL_AOD_2US        4
#define SYNC_EXCL_WITHOUT_CTE    8

/**
 * @brief Specify how the Controller will process periodic advertising
 *          synchronization information received from the device identified by the
 *          Connection_Handle parameter (the "transfer mode")
 *
 * @param conn_handle           Connection handle
 * @param mode                  The action to be taken when periodic advertising
 *                              synchronization information is received.
 * @param skip                  Number of consecutive periodic advertising
 *                              packets that the receiver may skip after successfully receiving a periodic
 *                              advertising packet.
 *                              Range: 0x0000 to 0x01F3
 * @param sync_timeout          maximum permitted time between successful receives. If this time is exceeded, synchronization is lost.
 *                              Range: 0x000A to 0x4000. Time = N*10 ms
 * @param cte_excl_types        whether to only synchronize to periodic advertising with certain types of Constant Tone Extension.
 *                              Combination of SYNC_EXCL_XXX
 * @return                      0: Message is sent out; Other: Message is not sent out
 */
// uint8_t gap_periodic_adv_sync_transfer_param(const hci_con_handle_t                     conn_handle,
//                                              const periodic_adv_sync_transfer_mode_t    mode,
//                                              const uint16_t                             skip,
//                                              const uint16_t                             sync_timeout,
//                                              const uint8_t                              cte_excl_types);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Specify the initial value for the mode, skip, timeout, and
 *          Constant Tone Extension type (set by the
 *          HCI_LE_Set_Periodic_Advertising_Sync_Transfer_Parameters command;
 *          see Section 7.8.91) to be used for all subsequent connections over the LE
 *          transport.
 *
 * @param mode                  The action to be taken when periodic advertising
 *                              synchronization information is received.
 * @param skip                  Number of consecutive periodic advertising
 *                              packets that the receiver may skip after successfully receiving a periodic
 *                              advertising packet.
 *                              Range: 0x0000 to 0x01F3
 * @param sync_timeout          maximum permitted time between successful receives. If this time is exceeded, synchronization is lost.
 *                              Range: 0x000A to 0x4000. Time = N*10 ms
 * @param cte_excl_types        whether to only synchronize to periodic advertising with certain types of Constant Tone Extension.
 *                              Combination of SYNC_EXCL_XXX
 * @return                      0: Message is sent out; Other: Message is not sent out
 */
// uint8_t gap_default_periodic_adv_sync_transfer_param(
//                                              const periodic_adv_sync_transfer_mode_t    mode,
//                                              const uint16_t                             skip,
//                                              const uint16_t                             sync_timeout,
//                                              const uint8_t                              cte_excl_types);
// WARNING: ^^^ this API is not available in this release


uint8_t gap_set_host_channel_classification(const uint32_t channel_low, const uint8_t channel_high);


/**
 * @brief Sets update period for random address
 * @param period_ms in ms
 */
// void gap_random_address_set_update_period(int period_ms);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Updates the connection parameters for a given LE connection
 * @param handle
 * @param conn_interval_min (unit: 1.25ms)
 * @param conn_interval_max (unit: 1.25ms)
 * @param conn_latency
 * @param supervision_timeout (unit: 10ms)
 * @returns 0 if ok
 */
int gap_update_connection_parameters(hci_con_handle_t con_handle, uint16_t conn_interval_min,
    uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout,
    uint16_t min_ce_len, uint16_t max_ce_len);

/**
 * @brief Set accepted connection parameter range
 * @param range
 */
void gap_get_connection_parameter_range(le_connection_parameter_range_t * range);

/**
 * @brief Get accepted connection parameter range
 * @param range
 */
void gap_set_connection_parameter_range(le_connection_parameter_range_t * range);

/**
 * @brief Encrypt the Plaintext_Data in the command using the Key given in the command and
 *          returns the Encrypted_Data to the Host. The AES-128 bit block cypher is
 *          defined in NIST Publication FIPS-197.
 * @param key                   Key, 16 Octets.
 *                              0x1234.... is represented by {0x12, 0x34, ...}
 * @param plaintext             Plaintext_Data, 16 Octets
 *                              0x1234.... is represented by {0x12, 0x34, ...}
 * @return                      0: Message is sent out; Other: Message is not sent out
 */
uint8_t gap_aes_encrypt(const uint8_t *key, const uint8_t *plaintext);

#define HCI_VENDOR_CCM  0xFC01

uint8_t gap_start_ccm(
        uint8_t  type,          // 0: encrypt  1: decrypt
        uint8_t  mic_size,
        uint16_t msg_len,
        uint16_t aad_len,
        uint32_t tag,          // same value will be reported in event
        uint8_t *key,
        uint8_t *nonce,
        uint8_t *msg,
        uint8_t *aad,
        uint8_t *out_msg);

//HCI func interface

/**
 * @brief Add event packet handler.
 */
void hci_add_event_handler(btstack_packet_callback_registration_t * callback_handler);

/**
 * @brief Registers a packet handler for ACL data. Used by L2CAP
 */
void hci_register_acl_packet_handler(btstack_packet_handler_t handler);

/**
 * @brief Requests the change of BTstack power mode.
 */
int  hci_power_control(HCI_POWER_MODE mode);


/*********************************/
typedef enum {
    // No security
    // No encryption required
    // No authentication
    LEVEL_1 = 1,

    // Unauthenticated pairing with encryption
    LEVEL_2,

    // Authenticated pairing with encryption
    LEVEL_3,

    // Authenticated LE Secure Connections pairing with encryption
    LEVEL_4,
} gap_security_level_t;

#ifdef __cplusplus
}
#endif

#endif
