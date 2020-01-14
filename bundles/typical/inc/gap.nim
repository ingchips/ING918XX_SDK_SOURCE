##  ----------------------------------------------------------------------------
##  Copyright Message
##  ----------------------------------------------------------------------------
##
##  INGCHIPS confidential and proprietary.
##  COPYRIGHT (c) 2018 by INGCHIPS
##
##  All rights are reserved. Reproduction in whole or in part is
##  prohibited without the written consent of the copyright owner.
##
##
##  ----------------------------------------------------------------------------

import
  bluetooth, bt_types

type
  le_connection_parameter_range_t* {.importc: "le_connection_parameter_range_t",
                                    header: "gap.h", bycopy.} = object
    le_conn_interval_min* {.importc: "le_conn_interval_min".}: uint16
    le_conn_interval_max* {.importc: "le_conn_interval_max".}: uint16
    le_conn_latency_min* {.importc: "le_conn_latency_min".}: uint16
    le_conn_latency_max* {.importc: "le_conn_latency_max".}: uint16
    le_supervision_timeout_min* {.importc: "le_supervision_timeout_min".}: uint16
    le_supervision_timeout_max* {.importc: "le_supervision_timeout_max".}: uint16


## LE Scan type

type                          ## Passive scan
  scan_type_t* {.size: sizeof(cint).} = enum
    SCAN_PASSIVE = 0x00000000,  ## Active scan
    SCAN_ACTIVE = 0x00000001


proc gap_set_random_device_address*(address: ptr uint8) {.
    importc: "gap_set_random_device_address", header: "gap.h".}
## *
##  Disconnect connection with handle
##
##  @param handle
##

proc gap_disconnect*(handle: hci_con_handle_t): uint8 {.importc: "gap_disconnect",
    header: "gap.h".}
proc gap_add_whitelist*(address: ptr uint8; addtype: bd_addr_type_t): uint8 {.
    importc: "gap_add_whitelist", header: "gap.h".}
proc gap_remove_whitelist*(address: ptr uint8; addtype: bd_addr_type_t): uint8 {.
    importc: "gap_remove_whitelist", header: "gap.h".}
type
  phy_type_t* {.size: sizeof(cint).} = enum
    PHY_1M = 0x00000001, PHY_2M = 0x00000002, PHY_CODED = 0x00000003


## bit configure for

type
  phy_bittype_t* {.size: sizeof(cint).} = enum
    PHY_1M_BIT = BIT(0), PHY_2M_BIT = BIT(1), PHY_CODED_BIT = BIT(2)
  phy_bittypes_t* = uint8
  adv_event_property_t* {.size: sizeof(cint).} = enum
    ADV_CONNECTABLE, ADV_SCANNABLE, ADV_DIRECT, ADV_HIGH_DUTY_DIR_ADV, ADV_LEGACY,
    ADV_ANONYMOUS, ADV_INC_TX_POWER



type
  adv_event_properties_t* = uint8

const
  PERIODIC_ADV_BIT_INC_TX* = BIT(6)

type
  periodic_adv_properties_t* = uint8

proc gap_read_phy*(con_handle: uint16): uint8 {.importc: "gap_read_phy",
    header: "gap.h".}
proc gap_set_def_phy*(all_phys: uint8; tx_phys: phy_bittypes_t;
                     rx_phys: phy_bittypes_t): uint8 {.importc: "gap_set_def_phy",
    header: "gap.h".}
type
  phy_option_t* {.size: sizeof(cint).} = enum
    HOST_NO_PREFERRED_CODING, HOST_PREFER_S2_CODING, HOST_PREFER_S8_CODING


proc gap_set_phy*(con_handle: uint16; all_phys: uint8; tx_phys: phy_bittypes_t;
                 rx_phys: phy_bittypes_t; phy_opt: phy_option_t): uint8 {.
    importc: "gap_set_phy", header: "gap.h".}
proc gap_set_adv_set_random_addr*(adv_handle: uint8; random_addr: ptr uint8): uint8 {.
    importc: "gap_set_adv_set_random_addr", header: "gap.h".}
type
  scan_phy_config_t* {.importc: "scan_phy_config_t", header: "gap.h", bycopy.} = object
    phy* {.importc: "phy".}: phy_type_t ##  Which phy to scan (valid phys: 1M & Coded)
    ## Scan type - 0=passive / 1=active
    `type`* {.importc: "type".}: scan_type_t ## Scan interval, units is 625us
    interval* {.importc: "interval".}: uint16 ## Scan window size, units is 625us
    window* {.importc: "window".}: uint16


## Scan filter policy

type ##  Accept all advertising packets except directed advertising packets not
    ##  addressed to this device (default).
  scan_filter_policy_t* {.size: sizeof(cint).} = enum
    SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED, ##  Accept only advertising packets from devices where the advertiser¡¯s
                                        ##  address is in the White List. Directed advertising packets which are not
                                        ##  addressed to this device shall be ignored
    SCAN_ACCEPT_WLIST_EXCEPT_NOT_DIRECTED, ##  Accept all advertising packets except directed advertising packets
                                          ##  where the initiator's identity address does not address this device
    SCAN_ACCEPT_ALL_EXCEPT_IDENTITY_NOT_MATCH, ##  Accept all advertising packets except:
                                              ##  1.  advertising packets where the advertiser's identity address is not in
                                              ##      the White List; and
                                              ##  2.  directed advertising packets where the initiator's identity address
                                              ##      does not address this device
    SCAN_ACCEPT_WLIST_EXCEPT_IDENTITY_NOT_MATCH


proc gap_set_ext_scan_para*(addr_type: bd_addr_type_t;
                           filter: scan_filter_policy_t; config_num: uint8;
                           configs: ptr scan_phy_config_t): uint8 {.
    importc: "gap_set_ext_scan_para", header: "gap.h".}
proc gap_set_ext_scan_response_data*(adv_handle: uint8; length: uint16;
                                    data: ptr uint8): uint8 {.
    importc: "gap_set_ext_scan_response_data", header: "gap.h".}
proc gap_set_ext_scan_enable*(enable: uint8; filter: uint8; duration: uint16;
                             period: uint16): uint8 {.
    importc: "gap_set_ext_scan_enable", header: "gap.h".}
type
  ext_adv_set_en_t* {.importc: "ext_adv_set_en_t", header: "gap.h", bycopy.} = object
    handle* {.importc: "handle".}: uint8 ##  Advertising duration, unit 10ms. 0 for continuous advertising
    duration* {.importc: "duration".}: uint16 ##  Maximum number of extended advertising events the Controller shall attempt to send prior to terminating the extended advertising
                                          ##  0: no maximum number
    max_events* {.importc: "max_events".}: uint8


proc gap_set_ext_adv_enable*(enable: uint8; set_number: uint8;
                            adv_sets: ptr ext_adv_set_en_t): uint8 {.
    importc: "gap_set_ext_adv_enable", header: "gap.h".}
##  channel map bit

const
  PRIMARY_ADV_CH_37* = BIT(0)
  PRIMARY_ADV_CH_38* = BIT(1)
  PRIMARY_ADV_CH_39* = BIT(2)
  PRIMARY_ADV_ALL_CHANNELS* = (
    PRIMARY_ADV_CH_37 or PRIMARY_ADV_CH_38 or PRIMARY_ADV_CH_39)

type
  adv_channel_bits_t* = uint8

## Advertising filter policy

type                          ## Allow both scan and connection requests from anyone
  adv_filter_policy_t* {.size: sizeof(cint).} = enum
    ADV_FILTER_ALLOW_ALL = 0x00000000, ## Allow both scan req from White List devices only and connection req from anyone
    ADV_FILTER_ALLOW_SCAN_WLST_CON_ALL, ## Allow both scan req from anyone and connection req from White List devices only
    ADV_FILTER_ALLOW_SCAN_ALL_CON_WLST, ## Allow scan and connection requests from White List devices only
    ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST


proc gap_set_ext_adv_para*(adv_handle: uint8; properties: adv_event_properties_t;
                          interval_min: uint32; interval_max: uint32;
                          primary_adv_channel_map: adv_channel_bits_t;
                          own_addr_type: bd_addr_type_t;
                          peer_addr_type: bd_addr_type_t; peer_addr: ptr uint8;
                          adv_filter_policy: adv_filter_policy_t; tx_power: int8;
                          primary_adv_phy: phy_type_t;
                          secondary_adv_max_skip: uint8;
                          secondary_adv_phy: phy_type_t; sid: uint8;
                          scan_req_notification_enable: uint8): uint8 {.
    importc: "gap_set_ext_adv_para", header: "gap.h".}
type
  adv_data_frag_pref_t_t* {.size: sizeof(cint).} = enum
    ADV_DATA_CTRL_MAY_FRAG = 0, ADV_DATA_CTRL_NOT_FRAG = 1


proc gap_set_ext_adv_data*(adv_handle: uint8; length: uint16; data: ptr uint8): uint8 {.
    importc: "gap_set_ext_adv_data", header: "gap.h".}
proc gap_set_periodic_adv_data*(adv_handle: uint8; length: uint16; data: ptr uint8): uint8 {.
    importc: "gap_set_periodic_adv_data", header: "gap.h".}
proc gap_set_periodic_adv_enable*(enable: uint8; adv_handle: uint8): uint8 {.
    importc: "gap_set_periodic_adv_enable", header: "gap.h".}
proc gap_set_periodic_adv_para*(adv_handle: uint8; interval_min: uint16;
                               interval_max: uint16;
                               properties: periodic_adv_properties_t): uint8 {.
    importc: "gap_set_periodic_adv_para", header: "gap.h".}
proc gap_clr_adv_set*(): uint8 {.importc: "gap_clr_adv_set", header: "gap.h".}
proc gap_rmv_adv_set*(adv_handle: uint8): uint8 {.importc: "gap_rmv_adv_set",
    header: "gap.h".}
type
  periodic_adv_filter_policy_t* {.size: sizeof(cint).} = enum
    PERIODIC_ADVERTISER_FROM_PARAM = 0, PERIODIC_ADVERTISER_FROM_LIST = 1


proc gap_periodic_adv_create_sync*(filter_policy: periodic_adv_filter_policy_t;
                                  adv_sid: uint8; addr_type: bd_addr_type_t;
                                  `addr`: ptr uint8; skip: uint16;
                                  sync_timeout: uint16; sync_cte_type: uint8): uint8 {.
    importc: "gap_periodic_adv_create_sync", header: "gap.h".}
proc gap_periodic_adv_create_sync_cancel*(): uint8 {.
    importc: "gap_periodic_adv_create_sync_cancel", header: "gap.h".}
proc gap_periodic_adv_term_sync*(sync_handle: uint16): uint8 {.
    importc: "gap_periodic_adv_term_sync", header: "gap.h".}
proc gap_add_dev_to_periodic_list*(addr_type: uint8; `addr`: ptr uint8; sid: uint8): uint8 {.
    importc: "gap_add_dev_to_periodic_list", header: "gap.h".}
proc gap_rmv_dev_from_periodic_list*(addr_type: uint8; `addr`: ptr uint8; sid: uint8): uint8 {.
    importc: "gap_rmv_dev_from_periodic_list", header: "gap.h".}
proc gap_clr_periodic_adv_list*(): uint8 {.importc: "gap_clr_periodic_adv_list",
                                        header: "gap.h".}
proc gap_read_periodic_adv_list_size*(): uint8 {.
    importc: "gap_read_periodic_adv_list_size", header: "gap.h".}
type
  initiating_filter_policy_t* {.size: sizeof(cint).} = enum
    INITIATING_ADVERTISER_FROM_PARAM = 0, INITIATING_ADVERTISER_FROM_LIST = 1
  conn_para_t* {.importc: "conn_para_t", header: "gap.h", bycopy.} = object
    scan_int* {.importc: "scan_int".}: uint16 ## Scan interval, units is 625us
    ## Scan window size, units is 625us
    scan_win* {.importc: "scan_win".}: uint16 ##  connection interval limiations. unit is 1.25ms
    interval_min* {.importc: "interval_min".}: uint16
    interval_max* {.importc: "interval_max".}: uint16 ##  Slave latency for the connection in number of connection events
    latency* {.importc: "latency".}: uint16 ##  Supervision timeout for the LE Link, unit is 10ms
    supervision_timeout* {.importc: "supervision_timeout".}: uint16 ##  Informative parameter recommending the min/max length of connection event needed for this LE connection
    min_ce_len* {.importc: "min_ce_len".}: uint16
    max_ce_len* {.importc: "max_ce_len".}: uint16

  initiating_phy_config_t* {.importc: "initiating_phy_config_t", header: "gap.h",
                            bycopy.} = object
    phy* {.importc: "phy".}: phy_type_t
    conn_param* {.importc: "conn_param".}: conn_para_t



proc gap_ext_create_connection*(filter_policy: initiating_filter_policy_t;
                               own_addr_type: bd_addr_type_t;
                               peer_addr_type: bd_addr_type_t;
                               peer_addr: ptr uint8; initiating_phy_num: uint8;
                               phy_configs: ptr initiating_phy_config_t): uint8 {.
    importc: "gap_ext_create_connection", header: "gap.h".}
## *
##  @brief Cancel connection process initiated by gap_ext_create_connection
##

proc gap_create_connection_cancel*(): uint8 {.
    importc: "gap_create_connection_cancel", header: "gap.h".}
type
  cte_type_t* {.size: sizeof(cint).} = enum
    CTE_AOA, CTE_AOD_1US, CTE_AOD_2US


## *
##  @brief Set the type, length, and antenna switching pattern for the transmission
##             of Constant Tone Extensions in any periodic advertising on the advertising set
##             identified by the Advertising_Handle parameter.
##
##  @param adv_handle            Used to identify an advertising set. Range: 0x00 to 0xEF
##  @param cte_len               Constant Tone Extension length in 8 ¦Ìs units. Range: 0x02 to 0x14
##  @param cte_type              CTE Type
##  @param cte_count             The number of Constant Tone Extensions to transmit in each periodic
##                               advertising interval.
##                               Range: 0x01 to 0x10
##  @param switching_pattern_len The number of Antenna IDs in the pattern. Range: 0x02 to 0x4B.
##  @param antenna_ids           List of Antenna IDs in the pattern
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##  uint8_t gap_set_connectionless_cte_tx_param(const uint8_t       adv_handle,
##                                              const uint8_t       cte_len,
##                                              const cte_type_t    cte_type,
##                                              const uint8_t       cte_count,
##                                              const uint8_t       switching_pattern_len,
##                                              const uint8_t      *antenna_ids);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Request that the Controller enables or disables the use of Constant Tone
##           Extensions in any periodic advertising on the advertising set identified by
##           Advertising_Handle.
##
##  @param adv_handle            Identifier for the advertising set in which Constant Tone Extension is
##                               being enabled or disabled
##  @param cte_enable            Disable (0x00), Enable (0x01)
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##  uint8_t gap_set_connectionless_cte_tx_enable(const uint8_t       adv_handle,
##                                               const uint8_t       cte_enable);
##  WARNING: ^^^ this API is not available in this release

type
  cte_slot_duration_type_t* {.size: sizeof(cint).} = enum
    CTE_SLOT_DURATION_1US, CTE_SLOT_DURATION_2US


## *
##  @brief Request that the Controller enables or disables capturing IQ samples from the
##           Constant Tone Extension of periodic advertising packets in the periodic
##           advertising train identified by the Sync_Handle parameter.
##
##  @param sync_handle           identifying the periodic advertising train.
##  @param sampling_enable       Disable (0x00), Enable (0x01)
##  @param slot_durations        combination of bits representing cte_slot_duration_type_t
##  @param max_sampled_ctes      The maximum number of Constant Tone Extensions to sample and
##                               report in each periodic advertising interval.
##                               Range: 0x01 to 0x10. 0x00: sample and report all CTEs.
##  @param switching_pattern_len The number of Antenna IDs in the pattern. Range: 0x02 to 0x4B.
##  @param antenna_ids           List of Antenna IDs in the pattern
##
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##  uint8_t gap_set_connectionless_iq_sampling_enable(const uint16_t      sync_handle,
##                                                    const uint8_t       sampling_enable,
##                                                    const uint8_t       slot_durations,
##                                                    const uint8_t       max_sampled_ctes,
##                                                    const uint8_t       switching_pattern_len,
##                                                    const uint8_t      *antenna_ids);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Enable or disable sampling received Constant Tone Extension fields on the
##           connection identified by the Connection_Handle parameter and to set the
##           antenna switching pattern and switching and sampling slot durations to be
##           used.
##
##  @param conn_handle           Connection handle
##  @param sampling_enable       Disable (0x00), Enable (0x01)
##  @param slot_durations        combination of bits representing cte_slot_duration_type_t
##  @param switching_pattern_len The number of Antenna IDs in the pattern. Range: 0x02 to 0x4B.
##  @param antenna_ids           List of Antenna IDs in the pattern
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##
##  uint8_t gap_set_connection_cte_rx_param(const hci_con_handle_t  conn_handle,
##                                          const uint8_t           sampling_enable,
##                                          const uint8_t           slot_durations,
##                                          const uint8_t           switching_pattern_len,
##                                          const uint8_t          *antenna_ids);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Set the antenna switching pattern and permitted Constant Tone Extension
##           types used for transmitting Constant Tone Extensions requested by the peer
##           device on the connection identified by the Connection_Handle parameter.
##
##  @param conn_handle           Connection handle
##  @param cte_types             Combination of bits representing cte_type_t
##  @param switching_pattern_len The number of Antenna IDs in the pattern. Range: 0x02 to 0x4B.
##  @param antenna_ids           List of Antenna IDs in the pattern
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##
##  uint8_t gap_set_connection_cte_tx_param(const hci_con_handle_t  conn_handle,
##                                          const uint8_t           cte_types,
##                                          const uint8_t           switching_pattern_len,
##                                          const uint8_t          *antenna_ids);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Set the antenna switching pattern and permitted Constant Tone Extension
##           types used for transmitting Constant Tone Extensions requested by the peer
##           device on the connection identified by the Connection_Handle parameter.
##
##  @param conn_handle           Connection handle
##  @param enable                Disable (0x00), Enable (0x01)
##  @param requested_interval    0x0000: Initiate the Constant Tone Extension Request procedure once, at the
##                                      earliest practical opportunity
##                               0x0001 to 0xFFFF:
##                                   Requested interval for initiating the Constant Tone Extension Request
##                                   procedure in number of connection events.
##  @param requested_cte_length  Minimum length of the Constant Tone Extension being requested in 8
##                               ¦Ìs units. Range: 0x02 to 0x14.
##  @param requested_cte_type    Requested CTE type
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##
##  uint8_t gap_set_connection_cte_request_enable(const hci_con_handle_t  conn_handle,
##                                                const uint8_t           enable,
##                                                const uint16_t          requested_cte_interval,
##                                                const uint8_t           requested_cte_length,
##                                                const cte_type_t        requested_cte_type);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Request the Controller to respond to LL_CTE_REQ PDUs with LL_CTE_RSP
##           PDUs on the specified connection.
##
##  @param conn_handle           Connection handle
##  @param enable                Disable (0x00), Enable (0x01)
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##  uint8_t gap_set_connection_cte_response_enable(const hci_con_handle_t  conn_handle,
##                                                 const uint8_t           enable);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Read the switching rates, the sampling rates, the number of antennae, and the
##           maximum length of a transmitted Constant Tone Extension supported by the
##           Controller.
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##  uint8_t gap_read_antenna_info(void);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Enable or disable reports for the periodic advertising train identified by the
##           Sync_Handle parameter.
##
##  @param sync_handle           identifying the periodic advertising train.
##  @param enable                Disable (0x00), Enable (0x01)
##
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##  uint8_t gap_set_periodic_adv_rx_enable(const uint16_t       sync_handle,
##                                         const uint8_t        enable);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Instruct the Controller to send synchronization information about the periodic
##           advertising train identified by the Sync_Handle parameter to a connected
##           device.
##
##  @param conn_handle           Connection handle
##  @param service_data          A value provided by the app for use by the app of the peer device.
##                               It is not used by the host or controller.
##  @param sync_handle           identifying the periodic advertising train.
##
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##  uint8_t gap_periodic_adv_sync_transfer(const hci_con_handle_t   conn_handle,
##                                         const uint16_t           service_data,
##                                         const uint16_t           sync_handle);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Instruct the Controller to send synchronization information about the periodic
##           advertising in an advertising set to a connected device.
##
##  @param conn_handle           Connection handle
##  @param service_data          A value provided by the app for use by the app of the peer device.
##                               It is not used by the host or controller.
##  @param adv_handle            identifying the advertising set.
##
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##  uint8_t gap_periodic_adv_set_info_transfer(const hci_con_handle_t   conn_handle,
##                                             const uint16_t           service_data,
##                                             const uint8_t            adv_handle);
##  WARNING: ^^^ this API is not available in this release

type
  periodic_adv_sync_transfer_mode_t* {.size: sizeof(cint).} = enum
    PERIODIC_TRANS_MODE_NULL, PERIODIC_TRANS_MODE_SEND_EVT_DISABLE_REPORT,
    PERIODIC_TRANS_MODE_SEND_EVT_ENABLE_REPORT


##  synchronize exclusion flags

const
  SYNC_EXCL_AOA* = 1
  SYNC_EXCL_AOD_1US* = 2
  SYNC_EXCL_AOD_2US* = 4
  SYNC_EXCL_WITHOUT_CTE* = 8

## *
##  @brief Specify how the Controller will process periodic advertising
##           synchronization information received from the device identified by the
##           Connection_Handle parameter (the "transfer mode")
##
##  @param conn_handle           Connection handle
##  @param mode                  The action to be taken when periodic advertising
##                               synchronization information is received.
##  @param skip                  Number of consecutive periodic advertising
##                               packets that the receiver may skip after successfully receiving a periodic
##                               advertising packet.
##                               Range: 0x0000 to 0x01F3
##  @param sync_timeout          maximum permitted time between successful receives. If this time is exceeded, synchronization is lost.
##                               Range: 0x000A to 0x4000. Time = N*10 ms
##  @param cte_excl_types        whether to only synchronize to periodic advertising with certain types of Constant Tone Extension.
##                               Combination of SYNC_EXCL_XXX
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##  uint8_t gap_periodic_adv_sync_transfer_param(const hci_con_handle_t                     conn_handle,
##                                               const periodic_adv_sync_transfer_mode_t    mode,
##                                               const uint16_t                             skip,
##                                               const uint16_t                             sync_timeout,
##                                               const uint8_t                              cte_excl_types);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Specify the initial value for the mode, skip, timeout, and
##           Constant Tone Extension type (set by the
##           HCI_LE_Set_Periodic_Advertising_Sync_Transfer_Parameters command;
##           see Section 7.8.91) to be used for all subsequent connections over the LE
##           transport.
##
##  @param mode                  The action to be taken when periodic advertising
##                               synchronization information is received.
##  @param skip                  Number of consecutive periodic advertising
##                               packets that the receiver may skip after successfully receiving a periodic
##                               advertising packet.
##                               Range: 0x0000 to 0x01F3
##  @param sync_timeout          maximum permitted time between successful receives. If this time is exceeded, synchronization is lost.
##                               Range: 0x000A to 0x4000. Time = N*10 ms
##  @param cte_excl_types        whether to only synchronize to periodic advertising with certain types of Constant Tone Extension.
##                               Combination of SYNC_EXCL_XXX
##  @return                      0: Message is sent out; Other: Message is not sent out
##
##  uint8_t gap_default_periodic_adv_sync_transfer_param(
##                                               const periodic_adv_sync_transfer_mode_t    mode,
##                                               const uint16_t                             skip,
##                                               const uint16_t                             sync_timeout,
##                                               const uint8_t                              cte_excl_types);
##  WARNING: ^^^ this API is not available in this release

proc gap_set_host_channel_classification*(channel_low: uint32; channel_high: uint8): uint8 {.
    importc: "gap_set_host_channel_classification", header: "gap.h".}
## *
##  @brief Sets update period for random address
##  @param period_ms in ms
##
##  void gap_random_address_set_update_period(int period_ms);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Updates the connection parameters for a given LE connection
##  @param handle
##  @param conn_interval_min (unit: 1.25ms)
##  @param conn_interval_max (unit: 1.25ms)
##  @param conn_latency
##  @param supervision_timeout (unit: 10ms)
##  @returns 0 if ok
##

proc gap_update_connection_parameters*(con_handle: hci_con_handle_t;
                                      conn_interval_min: uint16;
                                      conn_interval_max: uint16;
                                      conn_latency: uint16;
                                      supervision_timeout: uint16;
                                      min_ce_len: uint16; max_ce_len: uint16): cint {.
    importc: "gap_update_connection_parameters", header: "gap.h".}
## *
##  @brief Set accepted connection parameter range
##  @param range
##

proc gap_get_connection_parameter_range*(range: ptr le_connection_parameter_range_t) {.
    importc: "gap_get_connection_parameter_range", header: "gap.h".}
## *
##  @brief Get accepted connection parameter range
##  @param range
##

proc gap_set_connection_parameter_range*(range: ptr le_connection_parameter_range_t) {.
    importc: "gap_set_connection_parameter_range", header: "gap.h".}
## *
##  @brief Encrypt the Plaintext_Data in the command using the Key given in the command and
##           returns the Encrypted_Data to the Host. The AES-128 bit block cypher is
##           defined in NIST Publication FIPS-197.
##  @param key                   Key, 16 Octets.
##                               0x1234.... is represented by {0x12, 0x34, ...}
##  @param plaintext             Plaintext_Data, 16 Octets
##                               0x1234.... is represented by {0x12, 0x34, ...}
##  @return                      0: Message is sent out; Other: Message is not sent out
##

proc gap_aes_encrypt*(key: ptr uint8; plaintext: ptr uint8): uint8 {.
    importc: "gap_aes_encrypt", header: "gap.h".}
const
  HCI_VENDOR_CCM* = 0x0000FC01

proc gap_start_ccm*(`type`: uint8; mic_size: uint8; msg_len: uint16; aad_len: uint16;
                   tag: uint32; key: ptr uint8; nonce: ptr uint8; msg: ptr uint8;
                   aad: ptr uint8; out_msg: ptr uint8): uint8 {.
    importc: "gap_start_ccm", header: "gap.h".}
  ##  0: encrypt  1: decrypt
  ##  same value will be reported in event
## HCI func interface
## *
##  @brief Add event packet handler.
##

proc hci_add_event_handler*(callback_handler: ptr btstack_packet_callback_registration_t) {.
    importc: "hci_add_event_handler", header: "gap.h".}
## *
##  @brief Registers a packet handler for ACL data. Used by L2CAP
##

proc hci_register_acl_packet_handler*(handler: btstack_packet_handler_t) {.
    importc: "hci_register_acl_packet_handler", header: "gap.h".}
## *
##  @brief Requests the change of BTstack power mode.
##

proc hci_power_control*(mode: HCI_POWER_MODE): cint {.importc: "hci_power_control",
    header: "gap.h".}
## *******************************

type                          ##  No security
    ##  No encryption required
    ##  No authentication
  gap_security_level_t* {.size: sizeof(cint).} = enum
    LEVEL_1 = 1,                ##  Unauthenticated pairing with encryption
    LEVEL_2,                  ##  Authenticated pairing with encryption
    LEVEL_3,                  ##  Authenticated LE Secure Connections pairing with encryption
    LEVEL_4

