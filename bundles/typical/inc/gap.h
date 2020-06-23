/** @file                                             
*  @brief    function API and data structure for BLE      
*  @version  1.0.0.                                                     *
*  @date     2019/10/10                                                 *
*                                                                       *
*  Copyright (c) 2019 IngChips corp.                                    *
*                                                                       *
*                                                                       *
 *************************************************************************************************************/

#ifndef __GAP_EXTERNAL_H
#define __GAP_EXTERNAL_H

#include <stdint.h>
#include "bluetooth.h"
#include "bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bluetooth 
 * @defgroup Bluetooth_gap
 * @ingroup bluetooth_host
 * @{
 */
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

/**
 * @brief set random address device
 *
 * @param address               random address
 */
void gap_set_random_device_address(const uint8_t *address);

/**
 * @brief Disconnect connection with handle
 *
 * @param handle                Used to identify an advertising set. Range: 0x00 to 0xEF
 * @return                      0: Message is sent out or the connection alreay release
 */
uint8_t gap_disconnect(hci_con_handle_t handle);

/**
 * @brief disconnect multi-connections
 */
void gap_disconnect_all(void);
/**
 * @brief add whitelist to the controller
 *
 * @param[in] address               BLE address 
 * @param[in] addtype               BLE address type
 * @return                      0: Message is sent to controller
 */
uint8_t gap_add_whitelist(const uint8_t *address,bd_addr_type_t  addtype);

/**
 * @brief remove whitelist from controller
 *
 * @param address               BLE address 
 * @param addtype               BLE address type
 * @return                      0: Message is sent to controller
 */
uint8_t gap_remove_whitelist(const uint8_t *address,bd_addr_type_t addtype);

/**
 * @brief clear white lists in controller
 *
 * @return             0: message sent out  others: failed
 */
uint8_t gap_clear_white_lists(void);

/**
 * @brief read rssi value of a appointed hci connection
 *
 * @param handle       connection handle
 *
 * @return              0: message sent out  others: failed
 */
uint8_t gap_read_rssi(hci_con_handle_t handle);

/**
 * @brief read remote features upon hci connection
 *
 * @param handle       connection handle
 *
 * @return             0: message sent out  others: failed
 */
uint8_t gap_read_remote_used_features(hci_con_handle_t handle);

/**
 * @brief read remote features upon hci connection
 *
 * @param handle       connection handle
 *
 * @return             0: message sent out  others: failed
 */
uint8_t gap_read_remote_info(hci_con_handle_t handle);

/**
 * @brief read the channel map of a hci connection
 *
 * @param handle       connection handle
 *
 * @return             0: message sent out  others: failed
 */
uint8_t gap_le_read_channel_map(hci_con_handle_t handle);

/**
 * @brief physical channel type
 */
typedef enum phy_type
{
     PHY_1M = 0x01,
     PHY_2M = 0x02,
     PHY_CODED = 0x03,
} phy_type_t;

/**
 * @brief bit of physical channel type
 */
typedef enum phy_bittype
{
    PHY_1M_BIT    = BIT(0),
    PHY_2M_BIT    = BIT(1),
    PHY_CODED_BIT = BIT(2),
} phy_bittype_t;

typedef uint8_t phy_bittypes_t;

/**
 * @brief enum of the definition about advertising event
 */
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

/**
 * @brief read physical channel of connected handle
 *
 * @param con_handle            connected handle
 * @return                      0: Message is sent to controller
 */
uint8_t gap_read_phy(const uint16_t con_handle);

/**
 * @brief Host to specify its preferred values for the transmitter PHY and receiver PHY
 *
 * @param all_phys              BIT(0): no preference for transmit channel
 *                              BIT(1): no preference for receive channel 
 *                              Others: reserved for future
 *
 * @param tx_phys               BIT(0): prefer LE 1M transmitter PHY
 *                              BIT(1): prefer LE 2M transmitter PHY
 *                              BIT(2): prefer LE coded transmitter PHY
 *                              Others: reserved for future
 *
 * @param rx_phys               BIT(0): prefer LE 1M receiver PHY
 *                              BIT(1): prefer LE 2M receiver PHY
 *                              BIT(2): prefer LE coded receiver PHY
 *                              Others: reserved for future
 *
 * @return                      0: Message is sent to controller
 */
uint8_t gap_set_def_phy(const uint8_t all_phys, const phy_bittypes_t tx_phys, const phy_bittypes_t rx_phys);

/**
 * @brief enum of the PHY option
 * 
 */
typedef enum phy_option
{
    HOST_NO_PREFERRED_CODING,
    HOST_PREFER_S2_CODING,
    HOST_PREFER_S8_CODING
} phy_option_t;

/**
 * @brief Host to set PHY preferences for a connection
 *
 * @param con_handle            connection handl. (0x000--0x0EFF)
 *
 * @param all_phys              BIT(0): no preference for transmit channel
 *                              BIT(1): no preference for receive channel 
 *                              Others: reserved for future
 *
 * @param tx_phys               BIT(0): prefer LE 1M transmitter PHY
 *                              BIT(1): prefer LE 2M transmitter PHY
 *                              BIT(2): prefer LE coded transmitter PHY
 *                              Others: reserved for future
 *
 * @param rx_phys               BIT(0): prefer LE 1M receiver PHY
 *                              BIT(1): prefer LE 2M receiver PHY
 *                              BIT(2): prefer LE coded receiver PHY
 *                              Others: reserved for future
 *
 * @return                      0: Message is sent to controller
 */
uint8_t gap_set_phy(const uint16_t con_handle, const uint8_t all_phys, const phy_bittypes_t tx_phys,
                    const phy_bittypes_t rx_phys, const phy_option_t phy_opt);

/**
 * @brief Host to set a random address for an advertising set
 *
 * @param adv_handle            handle to advertising set
 *
 * @param random_addr           random address
 *
 * @return                      0: Message is sent to controller
 */
uint8_t gap_set_adv_set_random_addr(const uint8_t adv_handle, const uint8_t *random_addr);

/**
 * @brief Structure of PHY configuration
 */
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

/**
 * @brief enum of definition: Scan filter policy
 */
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

/**
 * @brief to set the extended scan parameters
 *
 * @param addr_type            address type
 *
 * @param filter               @link #scan_filter_policy_t @endlink
 *
 * @param config_num           configure number 3 at most. It indicates that host would configure how many kinds of PHY
 *
 * @param configs              struture pointer to the configuration of each PHY host specify
 *
 * @return                     0: messgae sent to controller
 */
uint8_t gap_set_ext_scan_para(const bd_addr_type_t addr_type, const scan_filter_policy_t filter, const uint8_t config_num,
                              const scan_phy_config_t *configs);
/**
 * @brief to set the extended scan response data for an advertising set
 *
 * @param adv_handle           handle of advertising set handle
 *
 * @param length               length of advertising data
 *
 * @param data                 pointer to the advertising data.
 *
 * @return                     0: messgae sent to controller
 */
uint8_t gap_set_ext_scan_response_data(const uint8_t adv_handle, const uint16_t length, const uint8_t *data);
/**
 * @brief to enable or disable scanning
 *
 * @param enable               0: disable  1: enable
 *
 * @param filter               policy to filter the advertising package
 *                             0x00:  Duplicate filtering disabled
 *                             0x01:  Duplicate filtering enabled
 *                             0x02:  Duplicate filtering enabled, reset for each scan period
 *                             others:Reserved for future
 *
 * @param duration             0x0000: Scan continuously until explicitly disable
 *                             N=0xXXXX: Scan duration
 *                                       Range: 0x0001 to 0xFFFF
 *                                       Time = N * 10 ms
 *                                       Time Range: 10 ms to 655.35 s
 *
 * @param period               0x0000: Scan continuously
 *                             N=0xXXXX: Time interval from when the Controller started its last Scan_Duration
 *                                       until it begins the subsequent Scan_Duration
 *                                       Range: 0x0001 to 0xFFFF
 *                                       Time = N * 1.28 sec
 *                                       Time Range: 1.28 s to 83,884.8 s
 *
 * @return                     0: messgae sent to controller
 */
uint8_t gap_set_ext_scan_enable(const uint8_t enable, const uint8_t filter, const uint16_t duration, const uint16_t period);

/**
 * @brief structure of advertising set information
 */
typedef struct ext_adv_set_en
{
    uint8_t handle;
    // Advertising duration, unit 10ms. 0 for continuous advertising
    uint16_t duration;
    // Maximum number of extended advertising events the Controller shall attempt to send prior to terminating the extended advertising
    // 0: no maximum number
    uint8_t max_events;
} ext_adv_set_en_t;

/**
 * @brief to enable or disable an advertising set
 *
 * @param enalbe               0: disable advertising  specified by adv_sets
 *                             1: enable advertising sepecified by adv_sets
 * @param set_number           number of advertising set
 *
 * @param adv_sets             see structure @link #ext_adv_set_en_t @endlink
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_set_ext_adv_enable(const uint8_t enable, const uint8_t set_number, const ext_adv_set_en_t *adv_sets);

/**
 * @brief channel map bit
 *
 */
#define PRIMARY_ADV_CH_37  BIT(0)
#define PRIMARY_ADV_CH_38  BIT(1)
#define PRIMARY_ADV_CH_39  BIT(2)
#define PRIMARY_ADV_ALL_CHANNELS    (PRIMARY_ADV_CH_37 | PRIMARY_ADV_CH_38 | PRIMARY_ADV_CH_39)

typedef uint8_t adv_channel_bits_t;

/**
 * @brief Advertising filter policy
 *
 */
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

/**
 * @brief to set the advertising set parameter
 *
 * @param adv_handle                 handle of an advertising set
 *                                   1: enable advertising sepecified by adv_sets
 * @param properties                 see structure @link #adv_event_properties_t @endlink
 *
 * @param interval_min               Range: 0x000020 to 0xFFFFFF
 *                                   Time = N * 0.625 ms
 *                                   Time Range: 20 ms to 10,485.759375 s
 *
 * @param interal_max                see @link #interval_min @endlink
 *
 * @param primary_adv_channel_map    see structure @link #adv_channel_bits_t @endlink
 *
 * @param own_addr_type              own BLE device address type
 * 
 * @param peer_addr_type             peer BLE device address type
 *
 * @param peer_addr                  peer BLE device address
 *
 * @param adv_filter_policy          filter policy , see structure @link #adv_filter_policy_t @endlink
 *
 * @param tx_power                   tranmission power
 *                                   0xXX : Size: 1 octet (signed integer)
 *                                          Range: -127 to +20
 *                                          Units: dBm
 *                                   0x7f : Host has no preference
 *
 * @param primary_adv_phy            see structure @link #phy_type_t @endlink
 *
 * @param secondary_adv_max_skip     0x00 : AUX_ADV_IND shall be sent prior to the next advertising event
 *                                   0x01-0xFF: Maximum advertising events the Controller can skip
 *
 * @param secondary_adv_phy          see structure @link #phy_type_t @endlink
 *
 * @param sid                        0x00-0x0f:Value of the Advertising SID subfield in the ADI field of the PDU
 *
 * @param scan_req_notification_enable   0x00: Scan request notifications disabled
 *                                       0x01: Scan request notifications enabled
 *
 * @return                           0: message sent to controller
 */
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

/**
 * @brief structure adv_data_frag_pref_t_t
 */
typedef enum adv_data_frag_pref
{
    ADV_DATA_CTRL_MAY_FRAG = 0,
    ADV_DATA_CTRL_NOT_FRAG = 1
} adv_data_frag_pref_t_t;

/**
 * @brief to set extended advertising data 
 *
 * @param adv_handle           advertising set handle.
 *                             
 * @param length               advertising data length
 *
 * @param data                 pointer to advertising data
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_set_ext_adv_data(const uint8_t adv_handle, uint16_t length, const uint8_t *data);

/**
 * @brief LE Set Periodic Advertising Data command
 *
 * @param adv_handle           advertising set handle.
 *                             
 * @param length               advertising data length
 *
 * @param data                 pointer to periodic advertising data
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_set_periodic_adv_data(const uint8_t adv_handle, uint16_t length, const uint8_t *data);

/**
 * @brief LE Set Periodic Advertising Enable command
 *
 * @param enable               0x00: Periodic advertising is disabled (default)
 *                             0x01: Periodic advertising is enabled                            
 * 
 * @param adv_handle           handle of advertising set
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_set_periodic_adv_enable(const uint8_t enable, const uint8_t adv_handle);
/**
 * @brief LE Set Periodic Advertising Parameters command
 *
 * @param adv_handle            handle of advertising set                            
 * 
 * @param interval_min          0xXXXX Range: 0x0006 to 0xFFFF
 *                                     Time = N * 1.25 ms
 *                                     Time Range: 7.5ms to 81.91875 s
 *
 * @param interval_max          refer to interval_min
 *
 * @param properties            BIT(6): Include TxPower in the advertising PDU
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_set_periodic_adv_para(const uint8_t adv_handle,
                                  const uint16_t interval_min,
                                  const uint16_t interval_max,
                                  const periodic_adv_properties_t properties);

/**
 * @brief LE Clear Advertising Sets command
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_clr_adv_set(void);
/**
 * @brief LE Remove Advertising Set command
 *
 * @param adv_handle            handle of advertising set to be removed                        
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_rmv_adv_set(const uint8_t adv_handle);

/**
 * @brief enum of periodical advertising filter policy
 */
typedef enum periodic_adv_filter_policy
{
    PERIODIC_ADVERTISER_FROM_PARAM = 0,
    PERIODIC_ADVERTISER_FROM_LIST  = 1
} periodic_adv_filter_policy_t;

/**
 * @brief LE Periodic Advertising Create Sync command
 *
 * @param filter_policy        see structure periodic_adv_filter_policy_t
 *
 * @param adv_sid              0x00-0x0f: Advertising SID subfield in the ADI field used to identify the Periodic
 *                                        Advertising
 * @param addr_type            see structure @link #bd_addr_type_t @endlink
 *
 * @param addr                 pointer to address
 *
 * @param skip                 0x0000-0x01f3: The number of periodic advertising packets that can be skipped after a
 *                                             successful receive
 * @param sync_timeout         Synchronization timeout for the periodic advertising
 *                             Range: 0x000A to 0x4000
 *                             Time = N*10 ms
 *                             Time Range: 100 ms to 163.84 s
 * 
 * @return                     0: message sent to controller
 */
uint8_t gap_periodic_adv_create_sync(const periodic_adv_filter_policy_t filter_policy,
                                     const uint8_t adv_sid,
                                     const bd_addr_type_t addr_type,
                                     const uint8_t *addr,
                                     const uint16_t skip,
                                     const uint16_t sync_timeout,
                                     const uint8_t sync_cte_type
                                    );
/**
 * @brief LE Periodic Advertising Create Sync Cancel command
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_periodic_adv_create_sync_cancel(void);
/**
 * @brief LE Periodic Advertising Terminate Sync command
 *
 * @param sync_handle          Sync_Handle identifying the periodic advertising train                        
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_periodic_adv_term_sync(const uint16_t sync_handle);
/**
 * @brief LE Add Device To Periodic Advertiser List command
 *
 * @param addr_type            device address type
 *
 * @param addr                 pointer to device address
 *
 * @param sid                  0x00-0x0F: Advertising SID subfield in the ADI field used to identify the Periodic Advertising
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_add_dev_to_periodic_list(const uint8_t addr_type, const uint8_t *addr, const uint8_t sid);
/**
 * @brief LE Remove Device From Periodic Advertiser List command
 *
 * @param addr_type            device address type
 *
 * @param addr                 pointer to device address
 *
 * @param sid                  0x00-0x0F: Advertising SID subfield in the ADI field used to identify the Periodic Advertising
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_rmv_dev_from_periodic_list(const uint8_t addr_type, const uint8_t *addr, const uint8_t sid);
/**
 * @brief LE Clear Periodic Advertiser List command
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_clr_periodic_adv_list(void);
/**
 * @brief LE Read Periodic Advertiser List Size command
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_read_periodic_adv_list_size(void);

/**
 * @brief structure initiating_filter_policy_t
 *
 */
typedef enum initiating_filter_policy
{
    INITIATING_ADVERTISER_FROM_PARAM = 0,
    INITIATING_ADVERTISER_FROM_LIST  = 1
} initiating_filter_policy_t;

/**
 * @brief structure connection parameters
 *
 */
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

/**
 * @brief structure initiating PHY configuration
 *
 */
typedef struct initiating_phy_config
{
    phy_type_t phy;
    conn_para_t conn_param;
} initiating_phy_config_t;

/**
 * @brief LE Extended Create Connection command
 *
 * @param filter_policy        see structure @link #initiating_filter_policy_t @endlink
 *
 * @param own_addr_type        own device address type
 *
 * @param peer_addr_type       peer device address type
 *
 * @param peer_addr            pointer to peer device address
 *
 * @param initiating_phy_num   PHY numbers configured for initiating process
 *
 * @param phy_configs          see structure @link #initiating_phy_config_t @endlink. phy_configs type array according to initiating_phy_num.
 *
 * @return                     0: message sent to controller
 */
uint8_t gap_ext_create_connection(const initiating_filter_policy_t filter_policy,
                                  const bd_addr_type_t own_addr_type,
	                              const bd_addr_type_t peer_addr_type,
	                              const uint8_t *peer_addr,
                                  const uint8_t initiating_phy_num,
                                  const initiating_phy_config_t *phy_configs);

/**
 * @brief Cancel connection process initiated by gap_ext_create_connection
 *
 */
uint8_t gap_create_connection_cancel(void);

/**
 * @brief structure:cte_type_t
 *
 */
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
 * @param cte_len               Constant Tone Extension length in 8 us units. Range: 0x02 to 0x14
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

/**
 * @brief structure:cte_slot_duration_type_t
 *
 */
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
 *                              us units. Range: 0x02 to 0x14.
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
 * @param range                 see structure @link #le_connection_parameter_range_t  @endlink
 */
void gap_get_connection_parameter_range(le_connection_parameter_range_t * range);

/**
 * @brief Get accepted connection parameter range
 * @param range                 see structure @link #le_connection_parameter_range_t  @endlink
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

/**
 * @brief host require controller to do AES-CCM functionality, and CCM referece to
 *        the document rfc3610:Counter with CBC-MAC (CCM)
 *
 * @param type                  0: encrypt     1:decrypt
 *
 * @param mic_size              8: mic size in 8 bytes  4: mic size in 4 bytes
 *
 * @param msg_len               messge length to be encrypt or decrypt
 *
 * @param aad_len               Additional authenticated data length
 *
 * @param tag                   a value to identify a AES-CCM request.
 *
 * @param key                   128 bits long key as required by AES-CCM
 *
 * @param nonce                 random value of each AES-CCM require 13 bytes long
 *
 * @param msg                   pointer to the message input
 *
 * @param aad                   Additional authenticated data
 *
 * @param out_msg               the output buffer to store the result of the AES-CCM ,calculated by controller.
 *
 * @return                      0: success    others: failed
 */
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
 * @brief Add event packet handler into HCI callback queue
 *
 * @param callback_handler       see structure @link #btstack_packet_callback_registration_t @endlink
 */
void hci_add_event_handler(btstack_packet_callback_registration_t * callback_handler);

/**
 * @brief Registers a packet handler for ACL data. Used by L2CAP
 *
 * @param handler                see function type @link btstack_packet_handler_t() @endlink
 */
void hci_register_acl_packet_handler(btstack_packet_handler_t handler);

/**
 * @brief Requests the change of BTstack power mode.
 *
 * @param mode                   0:HCI_POWER_OFF  ,1:HCI_POWER_ON , 2:HCI_POWER_SLEEP
 * 
 * @return                       0: succss
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

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif
