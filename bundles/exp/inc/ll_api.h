// ----------------------------------------------------------------------------
// Copyright Message
// ----------------------------------------------------------------------------
//
// INGCHIPS confidential and proprietary.
// COPYRIGHT (c) 2018-2022 by INGCHIPS
//
// All rights are reserved. Reproduction in whole or in part is
// prohibited without the written consent of the copyright owner.
//
//
// ----------------------------------------------------------------------------

#ifndef _LL_API_H_
#define _LL_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ll_config_item_e
{
    // Pre-wake up to schedule connection events properly
    // when slave latency is used.
    // Range: 1~255. Default: 4 (Unit: 0.625ms)
    LL_CFG_SLAVE_LATENCY_PRE_WAKE_UP,

    // configure feature set mask
    // `value` should be a pointer casted from `const uint8_t *`
    // The mask should be always available.
    LL_CFG_FEATURE_SET_MASK,

    LL_CFG_INTERNAL_ITEM_0,
} ll_config_item_t;

typedef struct ll_capabilities
{
    const uint8_t *features;                // Link Layer features
    uint16_t adv_set_num;                   // max number of advertising sets
    uint16_t conn_num;                      // max number of connections
    uint16_t whitelist_size;                // size of whitelists
    uint16_t resolving_list_size;           // size of resolving list
    uint16_t periodic_advertiser_list_size; // size of periodic advertiser list
    uint16_t adv_dup_filter_size;           // size of items for filtering
                                            // advertising duplication
} ll_capabilities_t;

/**
 * @brief Get Link Layer capabilities.
 *
 * @param[out]  capabilities    see `ll_capabilities_t`.
 */
void ll_get_capabilities(ll_capabilities_t *capabilities);

/**
 ****************************************************************************************
 * @brief Config LL specific parameters
 *
 * @param[in]  item             parameter to be configured (see `ll_config_item_t`)
 * @param[in]  value            value of the parameter
 ****************************************************************************************
 */
void ll_config(ll_config_item_t item, uint32_t value);

/**
 ****************************************************************************************
 * @brief Get states of LL
 *
 * Note: Parameters can be set to NULL if the relevant states are not needed.
 *
 * @param[out]  adv_states[1]   states of all advertising sets
 *                                  Bit[n]: advertising set n is enabled
 * @param[out]  conn_states[1]  states of all connections
 *                                  Bit[n]: connection n is active
 * @param[out]  sync_states[1]  states of all synchronized periodic adv sets
 *                                  Bit[n]: sync_handle n is active
 * @param[out]  other_states[1] other states:
 *                                  Bit[0]: if scanning is ON
 *                                  Bit[1]: if initiating is ON
 ****************************************************************************************
 */
void ll_get_states(uint32_t *adv_states, uint32_t *conn_states,
                   uint32_t *sync_states, uint32_t *other_states);

/**
 ****************************************************************************************
 * @brief set Tx power range
 *
 * Note: This range applies to all following advertisings, initiations, and connections.
 *
 * @param[in]  min_dBm          minimum Tx power in dBm
 * @param[in]  max_dBm          maximum Tx power in dBm
 ****************************************************************************************
 */
void ll_set_tx_power_range(int16_t min_dBm, int16_t max_dBm);

typedef enum coded_scheme_e
{
    BLE_CODED_S8,
    BLE_CODED_S2
} coded_scheme_t;

/**
 ****************************************************************************************
 * @brief set coded scheme of a advertising set
 *        Note: this function should be called before the advertising set is enabled.
 *              Default scheme for all advertising sets are S=8
 *
 * @param[in]  adv_hdl          handle of the advertising set
 * @param[in]  scheme           selected scheme
 ****************************************************************************************
 */
void ll_set_adv_coded_scheme(const uint8_t adv_hdl, const coded_scheme_t scheme);

/**
 ****************************************************************************************
 * @brief set coded scheme for initiating PDU
 *        Note: this function should be called before initiating is enabled.
 *
 * @param[in]  scheme           selected scheme (default S=8)
 ****************************************************************************************
 */
void ll_set_initiating_coded_scheme(const coded_scheme_t scheme);

/**
 ****************************************************************************************
 * @brief Give link layer a hint on ce_len of a connection
 *
 * @param[in]  conn_handle      handle of an existing connection
 * @param[in]  min_ce_len       information parameter about the minimum length of connection
 *                              event needed for this LE connection.
 *                              Range: 0x0000 ? 0xFFFF
 *                              Time = N * 0.625 ms.
 * @param[in]  max_ce_len       information parameter about the maximum length of connection
 *                              event needed for this LE connection.
 ****************************************************************************************
 */
void ll_hint_on_ce_len(const uint16_t conn_handle, const uint16_t min_ce_len, const uint16_t max_ce_len);

/**
 ****************************************************************************************
 * @brief Create/Resume a connection directly (without advertising & initiating)
 *
 * @param[in]  role             connection role. master (0), slave (1)
 * @param[in]  addr_types       address types for advertiser and initiator
 *                              bit [0] for slave (advertiser)
 *                              bit [1] for master (initiator)
 *                              0: public address; 1: random address
 * @param[in]  adv_addr         address of advertiser (little-endian)
 * @param[in]  init_addr        address of initiator (little-endian)
 * @param[in]  rx_phy           Rx PHY (1: 1M, 2: 2M, 3: Coded)
 * @param[in]  tx_phy           Tx PHY (1: 1M, 2: 2M, 3: Coded)
 * @param[in]  access_addr      access address
 * @param[in]  crc_init         CRC init
 * @param[in]  interval         connection interval (unit: us)
 * @param[in]  sup_timeout      supervision timeout (unit: 10ms)
 * @param[in]  channel_map      channel map
 * @param[in]  ch_sel_algo      channel selection algorithm (0: ALG #1, 1: ALG #2)
 * @param[in]  hop_inc          hop increment for CSA#1 ([5..16]) (only for ALG #1)
 * @param[in]  last_unmapped_ch last unmapped channel index  (only for ALG #1)
 * @param[in]  min_ce_len       information parameter about the minimum length of connection
 *                              event needed for this LE connection.
 * @param[in]  max_ce_len       information parameter about the maximum length of connection
 *                              event needed for this LE connection.
 * @param[in]  start_time       start time of the 1st connection event
 * @param[in]  event_counter    event counter for the 1st connection event
 * @param[in]  slave_latency    slave latency
 * @param[in]  sleep_clk_acc    sleep clock accuracy (only for SLAVE role)
 * @param[in]  sync_window      slave's sync window for 1st connection event
 * @param[in]  security         link layer security context
 *                              NULL: security not used
 *                              otherwise: security is used, which is `le_security_ctx_t *` in
 *                                         `le_meta_event_vendor_connection_aborted_t`.
 * @return                      0 if successful else error code
 ****************************************************************************************
 */
int ll_create_conn(uint8_t role,
                   uint8_t addr_types,
                   const uint8_t *adv_addr,
                   const uint8_t *init_addr,
                   uint8_t rx_phy,
                   uint8_t tx_phy,
                   uint32_t access_addr,
                   uint32_t crc_init,
                   uint32_t interval,
                   uint16_t sup_timeout,
                   const uint8_t *channel_map,
                   uint8_t  ch_sel_algo,
                   uint8_t  hop_inc,
                   uint8_t  last_unmapped_ch,
                   uint16_t min_ce_len,
                   uint16_t max_ce_len,
                   uint64_t start_time,
                   uint16_t event_counter,
                   uint16_t slave_latency,
                   uint8_t  sleep_clk_acc,
                   uint32_t sync_window,
                   const void *security);

/**
 ****************************************************************************************
 * @brief Set tx power of a connection
 *
 * @param[in]  conn_handle      handle of an existing connection
 * @param[in]  tx_power         tx power in dBm
 ****************************************************************************************
 */
void ll_set_conn_tx_power(uint16_t conn_handle, int16_t tx_power);

/**
 ****************************************************************************************
 * @brief Try to adjust peer's tx power of a connection
 *
 * @param[in]  conn_handle      handle of an existing connection
 * @param[in]  delta            delta of tx power in dB
 *                              Note: A positive value indicates a request to increase the
 *                                    transmit power level; negative to decrease the transmit
 *                                    power level.
 ****************************************************************************************
 */
void ll_adjust_conn_peer_tx_power(uint16_t conn_handle, int8_t delta);

/**
 ****************************************************************************************
 * @brief Set coded scheme of a connection when CODED is used
 *
 * @param[in]  conn_handle      handle of an existing connection
 * @param[in]  ci               0: S8 (default), 1: S2
 ****************************************************************************************
 */
void ll_set_conn_coded_scheme(uint16_t conn_handle, int ci);

/**
 ****************************************************************************************
 * @brief Force latency parameter of a connection (slave role)
 *
 * @param[in]  conn_handle      handle of an existing connection
 * @param[in]  latency          latency
 ****************************************************************************************
 */
void ll_set_conn_latency(uint16_t conn_handle, int latency);

/**
 ****************************************************************************************
 * @brief Get connection basic information of a connection
 *
 * @param[in]  conn_handle      handle of an existing connection
 * @param[out] access_addr      Access address
 * @param[out] crc_init         CRC init value
 * @param[out] hop_inc          Hop increment (valid when using Algo #1)
 * @return                      0 if successful else non-0
 ****************************************************************************************
 */
int ll_get_conn_info(const uint16_t conn_handle,
                    uint32_t *access_addr,
                    uint32_t *crc_init,
                    uint8_t *hop_inc);

/**
 ****************************************************************************************
 * @brief Get information of upcoming connection events of a connection
 *
 * Information of consecutive N connection events after `from_time` can be retrieved,
 * where N = `number`.
 *
 * Sub-rating is ignored. If channel map updated within these events, then `channel_ids`
 * are not reliable.
 *
 * @param[in]  conn_handle      handle of an existing connection
 * @param[in]  number           number of connection events to be queried
 * @param[in]  from_time        the first connection event shall be after `from_time` (unit: us)
 * @param[out] interval         connection interval (unit: us)
 * @param[out] time_offset      time offset between the start of the first connection event and `from_time` (unit: us)
 * @param[out] event_count      connection event count of the first connection event
 * @param[out] channel_ids      physical channel IDs of the upcoming `number` connection events
 * @return                      0 if successful else non-0
 ****************************************************************************************
 */
int ll_get_conn_events_info(const uint16_t conn_handle,
                            int number,
                            uint64_t from_time,
                            uint32_t *interval,
                            uint32_t *time_offset,
                            uint16_t *event_count,
                            uint8_t *channel_ids);

/**
 ****************************************************************************************
 * @brief Abort an existing connection
 *
 * After a connection is aborted, `HCI_SUBEVENT_VENDOR_CONNECTION_ABORTED` is emitted.
 *
 * @param[in]  conn_handle      handle of an existing connection
 * @return                      0 if aborting is ongoing else non-0
 ****************************************************************************************
 */
int ll_conn_abort(uint16_t conn_handle);

/**
 ****************************************************************************************
 * @brief Set default antenna ID
 *
 *          Note: This ID restored to default value (i.e. 0) when LLE is resetted.
 *
 * @param[in]  ant_id           ID of default antenna (default: 0)
 *
 ****************************************************************************************
 */
void ll_set_def_antenna(uint8_t ant_id);

/**
 ****************************************************************************************
 * @brief Set legacy advertising PDU interval within a single event
 *
 * @param[in]  for_hdc      interval for high duty cycle advertising in micro sec  (default 1250)
 * @param[in]  not_hdc      interval for normal duty cycle advertising is micro sec (default 1500)
 ****************************************************************************************
 */
void ll_legacy_adv_set_interval(uint16_t for_hdc, uint16_t not_hdc);

/**
 ****************************************************************************************
 * @brief Attach CTE to an extended advertising set
 *
 * Note: This function will fail when the specified advertising set has not been instantiated.
 *
 * @param[in]  adv_handle               handle of the advertising set
 * @param[in]  cte_type                 CTE type (0: AoA)
 * @param[in]  cte_len                  CTE length in 8us
 * @param[in]  switching_pattern_len    switching pattern len
 * @param[in]  switching_pattern        switching pattern
 * @return                              0 if successful else error code
 ****************************************************************************************
 */
int ll_attach_cte_to_adv_set(uint8_t adv_handle, uint8_t cte_type,
                             uint8_t cte_len,
                             uint8_t switching_pattern_len,
                             const uint8_t *switching_pattern);

/**
 ****************************************************************************************
 * @brief Enable IQ sampling after scanning is enabled
 *
 * IQ samples in periodic advertising or other extended advertising sets
 * (see `ll_attach_cte_to_adv_set`) are sampled and reported through a vendor defined LE
 * sub-events.
 *
 * Note: This function will fail if scanning has not been started. IQ sampling is
 * disabled when scanning is disabled.
 *
 * @param[in]   cte_type                cte_type (0: AoA; 1: AoD 1us; 2: AoD 2us)
 * @param[in]   slot_len                slot length for AoA
 * @param[in]   switching_pattern_len   switching pattern len
 * @param[in]   switching_pattern       switching pattern
 * @param[in]   slot_sampling_offset    sampling offset (0..23) in a slot
 * @param[in]   slot_sample_count       sample count within a slot (1..5)
 * @return                              0 if successful else error code
 *
 * Note:
 * Recommended value: slot_sampling_offset = 12, slot_sample_count = 1
 * (slot_sampling_offset + slot_sample_count) should be <= 24
 ****************************************************************************************
 */
int ll_scanner_enable_iq_sampling(uint8_t cte_type,
                          uint8_t slot_len,
                          uint8_t switching_pattern_len,
                          const uint8_t *switching_pattern,
                          uint8_t slot_sampling_offset,
                          uint8_t slot_sample_count);

/**
 ****************************************************************************************
 * @brief Enable IQ sampling on legacy adv after scanning is enabled
 *
 * CAUTION: This feature might cause system into dead-lock, or hard fault. Be sure to enable watchdog.
 *
 * A portion of legacy advertising is treated as CTE, and IQ samples are sampled
 * and reported through a vendor defined LE sub-events.
 *
 * Note: Before calling this function, scanning mode must be configured to LEGACY ONLY.
 * This function will fail if scanning has not been started. IQ sampling is
 * disabled when scanning is disabled.
 *
 * @param[in]   sampling_offset         CTE position (**bit** offset) in payload (>= 8bits)
 *                                      For example, in ADV_IND, to start CTE sampling
 *                                      from the first bit of AdvData, `sampling_offset`
 *                                      is 6 * 8 (bits).
 * @param[in]   cte_type                cte_type (0: AoA; 1: AoD 1us; 2: AoD 2us)
 * @param[in]   slot_len                slot length for AoA
 * @param[in]   switching_pattern_len   switching pattern len
 * @param[in]   switching_pattern       switching pattern
 * @param[in]   slot_sampling_offset    sampling offset (0..23) in a slot
 * @param[in]   slot_sample_count       sample count within a slot (1..5)
 * @return                              0 if successful else error code
 *
 * Note:
 * Recommended value: slot_sampling_offset = 12, slot_sample_count = 1
 * (slot_sampling_offset + slot_sample_count) should be <= 24
 ****************************************************************************************
 */
int ll_scanner_enable_iq_sampling_on_legacy(
                          uint16_t sampling_offset,
                          uint8_t cte_type,
                          uint8_t cte_time,
                          uint8_t slot_len,
                          uint8_t switching_pattern_len,
                          const uint8_t *switching_pattern,
                          uint8_t slot_sampling_offset,
                          uint8_t slot_sample_count);

struct ll_raw_packet;

typedef void (* f_ll_raw_packet_done)(struct ll_raw_packet *packet, void *user_data);

/**
 ****************************************************************************************
 * @brief Free a raw packet object
 *
 * @param[in]  packet      the packet
 ****************************************************************************************
 */
void ll_raw_packet_free(struct ll_raw_packet *packet);

/**
 ****************************************************************************************
 * @brief Create a raw packet object
 *
 * @param[in]   for_tx      1 if this packet is for Tx else 0
 * @param[in]   on_done     callback function when packet Rx/Tx is done
 * @param[in]   user_data   extra user defined data passed to on_done callback
 * @return                  the new packet object (NULL if out of memory)
 ****************************************************************************************
 */
struct ll_raw_packet *ll_raw_packet_alloc(uint8_t for_tx, f_ll_raw_packet_done on_done, void *user_data);

/**
 ****************************************************************************************
 * @brief Set parameters of a raw packet object
 *
 * @param[in]   packet              the packet object
 * @param[in]   tx_power            tx power in dBm (ignored in Rx)
 * @param[in]   rf_channel_id       RF channel ID (0: 2402MHz, 1: 2404MHz, ...)
 * @param[in]   phy                 PHY
 *                                  For Tx: 1: 1M, 2: 2M, 3: S8, 4: S2.
 *                                  For Rx, 1: 1M, 2: 2M, 3: Coded.
 * @param[in]   access_addr         access address
 * @param[in]   crc_init            CRC initialization value
 * @return                          0 if successful else error code
 ****************************************************************************************
 */
int ll_raw_packet_set_param(struct ll_raw_packet *packet,
                          int8_t tx_power,
                          int8_t rf_channel_id,
                          uint8_t phy,
                          uint32_t access_addr,
                          uint32_t crc_init);

/**
 ****************************************************************************************
 * @brief Set bare mode of a raw packet object
 *
 * @param[in]   packet              the packet object
 * @param[in]   header              header sending before packet length
 * @param[in]   freq_mhz            frequency in MHz
 *                                  When 0: use the channel specified in `ll_raw_packet_set_param`)
 * @return                          0 if successful else error code
 *
 *
 * Note: Only a subset of uint8_t are supported in `header`;
 *       When bare mode is used, CRC & whitening are all handled by apps;
 *       When bare mode is used, `crc_init` is ignored;
 *       When `freq_mhz` is not zero, BLE activities near this raw packets might be affected.
 ****************************************************************************************
 */
int ll_raw_packet_set_bare_mode(struct ll_raw_packet *packet,
                                uint8_t header,
                                int freq_mhz);

/**
 ****************************************************************************************
 * @brief Set Tx data of a raw packet object (bare mode)
 *
 * @param[in]   packet              the packet object
 * @param[in]   data                point to the data
 * @param[in]   size                data size (<= 255)
 * @param[in]   crc_value           CRC value
 * @return                          0 if successful else error code
 ****************************************************************************************
 */
int ll_raw_packet_set_bare_data(struct ll_raw_packet *packet,
                                const void *data,
                                int size, uint32_t crc_value);

/**
 ****************************************************************************************
 * @brief Set Tx data of a raw packet object
 *
 * @param[in]   packet              the packet object
 * @param[in]   header              extra header data (only the lowest 2bits are transmitted)
 * @param[in]   data                point to the data
 * @param[in]   size                data size (<= 255)
 * @return                          0 if successful else error code
 ****************************************************************************************
 */
int ll_raw_packet_set_tx_data(struct ll_raw_packet *packet,
                               uint8_t header,
                               const void *data,
                               int size);

/**
 ****************************************************************************************
 * @brief Send a raw packet object
 *
 * @param[in]   packet              the packet object
 * @param[in]   when                start time of the packet (in us)
 * @return                          0 if successful else error code. Error occurs if
 *                                  * the last `ll_raw_packet_send` is not completed, or
 *                                  * LL can't schedule RF activity at specifed time (too late, or busy).
 ****************************************************************************************
 */
int ll_raw_packet_send(struct ll_raw_packet *packet,
                       uint64_t when);

/**
 ****************************************************************************************
 * @brief Get received data of a raw packet object
 *
 * @param[in]   packet              the packet object
 * @param[out]  air_time            start time of the received packet (in us)
 * @param[out]  header              extra header data
 * @param[out]  data                point to the data
 * @param[out]  size                data size
 * @param[out]  rssi                RSSI in dBm
 * @return                          0 if successful else error code
 *                                  Note: `air_time`, `header` and `rssi` are also available
 *                                        even if error code is not in {1, 2}.
 ****************************************************************************************
 */
int ll_raw_packet_get_rx_data(struct ll_raw_packet *packet,
                               uint64_t *air_time,
                               uint8_t *header,
                               void *data,
                               int *size,
                               int *rssi);

/**
 ****************************************************************************************
 * @brief Get received data of a raw packet object (bare mode)
 *
 * @param[in]   packet              the packet object
 * @param[out]  air_time            start time of the received packet (in us)
 * @param[out]  header              received `header`
 * @param[out]  data                point to the data
 * @param[out]  size                data size
 * @param[out]  rssi                RSSI in dBm
 * @param[out]  crc_value           CRC value
 * @return                          0 if successful else error code
 ****************************************************************************************
 */
int ll_raw_packet_get_bare_rx_data(struct ll_raw_packet *packet,
                               uint64_t *air_time,
                               uint8_t *header,
                               void *data,
                               int *size,
                               int *rssi,
                               uint32_t *crc_value);

/**
 ****************************************************************************************
 * @brief Receive a packet using a raw packet object
 *
 * @param[in]   packet              the packet object
 * @param[in]   when                start time of receiving (in us)
 * @param[in]   rx_window           Rx window length to scanning for a packet (in us)
 * @return                          0 if successful else error code. Error occurs if
 *                                  * the last `ll_raw_packet_recv` is not completed, or
 *                                  * LL can't schedule RF activity at specifed time (too late, or busy).
 ****************************************************************************************
 */
int ll_raw_packet_recv(struct ll_raw_packet *packet,
                        uint64_t when,
                        uint32_t rx_window);

/**
 ****************************************************************************************
 * @brief Set parameters for CTE transmission of a raw packet object
 *
 * @param[in]   packet                  the packet object
 * @param[in]   cte_type                cte_type (0: AoA; 1: AoD 1us; 2: AoD 2us)
 * @param[in]   cte_len                 CTE length in 8us
 * @param[in]   switching_pattern_len   switching pattern len
 * @param[in]   switching_pattern       switching pattern
 * @return                              0 if successful else error code
 ****************************************************************************************
 */
int ll_raw_packet_set_tx_cte(struct ll_raw_packet *packet,
                          uint8_t cte_type,
                          uint8_t cte_len,
                          uint8_t switching_pattern_len,
                          const uint8_t *switching_pattern);

/**
 ****************************************************************************************
 * @brief Set a fake CTEInfo value for CTE transmission of a raw packet object
 *
 * Note: When using this function, this function should be called after `ll_raw_packet_set_tx_cte`.
 *
 *       A possible use case: Both Rx/Tx sides of CTE switch antennas:
 *         1) Call `ll_raw_packet_set_tx_cte` setup AoD transmission;
 *         2) Call `ll_raw_packet_set_fake_cte`  to modify CTEInfo to AoA.
 *
 * @param[in]   packet                  the packet object
 * @param[in]   cte_type                cte_type (0: AoA; 1: AoD 1us; 2: AoD 2us)
 * @param[in]   cte_len                 CTE length in 8us
 * @return                              0 if successful else error code
 ****************************************************************************************
 */
int ll_raw_packet_set_fake_cte_info(struct ll_raw_packet *packet,
                          uint8_t cte_type,
                          uint8_t cte_len);

/**
 ****************************************************************************************
 * @brief Set parameters for CTE reception of a raw packet object
 *
 * @param[in]   packet                  the packet object
 * @param[in]   cte_type                cte_type (0: AoA; 1: AoD 1us; 2: AoD 2us)
 * @param[in]   slot_len                slot length for AoA
 * @param[in]   switching_pattern_len   switching pattern len
 * @param[in]   switching_pattern       switching pattern
 * @param[in]   slot_sampling_offset    sampling offset (0..23) in a slot
 * @param[in]   slot_sample_count       sample count within a slot (1..5)
 * @return                              0 if successful else error code
 *
 * Note:
 * Recommended value: slot_sampling_offset = 12, slot_sample_count = 1
 * (slot_sampling_offset + slot_sample_count) should be <= 24
 ****************************************************************************************
 */
int ll_raw_packet_set_rx_cte(struct ll_raw_packet *packet,
                          uint8_t cte_type,
                          uint8_t slot_len,
                          uint8_t switching_pattern_len,
                          const uint8_t *swiching_pattern,
                          uint8_t slot_sampling_offset,
                          uint8_t slot_sample_count);

/**
 ****************************************************************************************
 * @brief Get IQ samples of a raw packet object
 *
 * @param[in]   packet              the packet object
 * @param[out]  iq_samples          buffer to store IQ samples (must be large enough)
 * @param[out]  iq_sample_cnt       number of IQ pairs
 * @param[in]   preprocess          do preprocessing (non-0) or don't (0) do preprocessing
 *                                  Note: 1) Preprocessing is only available when
 *                                           `slot_sample_count` = 1
 *                                        2) IQ samples format of each component:
 *                                           * When preprocessing is on: `int8_t`
 *                                           * When preprocessing is off: `int16_t`
 * @return                          0 if successful else error code
 ****************************************************************************************
 */
int ll_raw_packet_get_iq_samples(struct ll_raw_packet *packet,
                               void *iq_samples,
                               int *iq_sample_cnt,
                               int preprocess);

/**
 ****************************************************************************************
 * @brief Create an ack-able packet object
 *
 * @param[in]   for_initiator       1 if this packet is for initiator else responder
 * @param[in]   on_done             callback function when packet Rx/Tx is done
 * @param[in]   user_data           extra user defined data passed to on_done callback
 * @return                          the new packet object (NULL if out of memory)
 ****************************************************************************************
 */
struct ll_raw_packet *ll_ackable_packet_alloc(uint8_t for_initiator, f_ll_raw_packet_done
                                              on_done, void *user_data);


/**
 ****************************************************************************************
 * @brief Set Tx data of an ack-able packet object
 *
 * @param[in]   packet              the packet object
 * @param[in]   data                point to the data
 * @param[in]   size                data size (<= 255)
 * @return                          0 if successful else error code
 ****************************************************************************************
 */
int ll_ackable_packet_set_tx_data(struct ll_raw_packet *packet,
                               const void *data,
                               int size);

/**
 ****************************************************************************************
 * @brief Get status of an ack-able packet object
 *
 * @param[in]   packet              the packet object
 * @param[out]  acked               if tx data has been acked or not
 * @param[out]  air_time            start time of the received packet (in us)
 * @param[out]  data                point to the data
 * @param[out]  size                data size
 * @param[out]  rssi                RSSI in dBm
 * @return                          0 if a packet is received else error code
 ****************************************************************************************
 */
int ll_ackable_packet_get_status(struct ll_raw_packet *packet,
                               int *acked,
                               uint64_t *air_time,
                               void *data,
                               int *size,
                               int *rssi);

/**
 ****************************************************************************************
 * @brief Receive a packet using an ack-able packet object
 *
 * @param[in]   packet              the packet object
 * @param[in]   when                start time of receiving (in us)
 * @param[in]   window              window length to run ack-able packet
 * @return                          0 if successful else error code. Error occurs if
 *                                  * the last `ll_ackable_packet_run` is not completed, or
 *                                  * LL can't schedule RF activity at specifed time (too late, or busy).
 ****************************************************************************************
 */
int ll_ackable_packet_run(struct ll_raw_packet *packet,
                        uint64_t when,
                        uint32_t window);

/**
 ****************************************************************************************
 * @brief Create a channel monitor packet object
 *
 * A channel monitor receives all PDUs using the given channel configuration (\ref `ll_raw_packet_set_param`).
 *
 * Possible Usages:
 *
 * 1. Scan for Adv on a single channel;
 *
 * 1. Receive Connection packages from both roles.
 *
 *    Ideally, if this monitor is started just before the beginning of a connection
 *    event, the 0th PDU will be the one from Master, the 1st from Slave, and so on.
 *
 * @param[in]   pdu_num             number of PDUs that can be received in a single run
 * @param[in]   on_done             callback function when packet Rx/Tx is done
 * @param[in]   user_data           extra user defined data passed to on_done callback
 * @return                          the new packet object (NULL if out of memory)
 ****************************************************************************************
 */
struct ll_raw_packet *ll_channel_monitor_alloc(int pdu_num, f_ll_raw_packet_done on_done, void *user_data);

/**
 ****************************************************************************************
 * @brief Scheduling the channel monitor
 *
 * @param[in]   packet              the packet object
 * @param[in]   when                start time of receiving (in us)
 * @param[in]   window              window length to run the monitor
 * @return                          0 if successful else error code. Error occurs if
 *                                  * the last `ll_channel_monitor_run` is not completed, or
 *                                  * LL can't schedule RF activity at specifed time (too late, or busy).
 ****************************************************************************************
 */
int ll_channel_monitor_run(struct ll_raw_packet *packet,
                        uint64_t when,
                        uint32_t window);

/**
 ****************************************************************************************
 * @brief PDU visitor for channel monitor
 *
 * This visitor is called on each received PDU.
 *
 * For ING918, `data` and `size` shall be ignored.
 *
 * @param[in]   index               index of this PDU
 *                                  Range: [0 .. pdu_num - 1]
 * @param[in]   status              0 if successfully received else error code.
 *                                  When status is not 0, all of bellow params (except `user_data`)
 *                                  shall be ignored.
 * @param[in]   reserved            (Reversed)
 * @param[in]   data                Data of the PDU
 * @param[in]   size                size of data
 * @param[in]   rssi                RSSI in dBm
 * @param[in]   user_data           extra user defined data
 * @return                          0 if successful else error code
 ****************************************************************************************
 */
typedef void (* f_ll_channel_monitor_pdu_visitor)(int index, int status, uint8_t resevered,
              const void *data, int size, int rssi, void *user_data);

/**
 ****************************************************************************************
 * @brief Check each of the received PDU
 *
 * Call this after link monitor is done (for example, in the `on_done` callback).
 *
 * @param[in]   packet              the packet object
 * @param[in]   visitor             the visitor callback
 * @param[in]   user_data           extra user defined data passed to `visitor` callback
 * @return                          number of successfully received PDUs
 ****************************************************************************************
 */
int ll_channel_monitor_check_each_pdu(struct ll_raw_packet *packet,
                                f_ll_channel_monitor_pdu_visitor visitor,
                                void *user_data);

/**
 ****************************************************************************************
 * @brief Get air time of the first received PDU
 *
 * @param[in]   packet              the packet object
 * @param[out]  air_time            air time
 * @return                          0 if successful else error code
 *                                  error conditions: 1) still running; 2) no PDU is received.
 ****************************************************************************************
 */
int ll_channel_monitor_get_1st_pdu_time(struct ll_raw_packet *packet, uint64_t *air_time);

/**
 * @brief Enumeration for Burst Transmission Order
 *
 * This enumeration defines the possible burst transmission orders for the whole data packets.
 *
 * This order applies to each data element. A data element could be a 8-bit byte in payload,
 * 2-bit header, 16-bit CRC etc.
 *
 * @details The burst transmission order determines the order in which data bytes are transmitted
 *          within a burst transfer. The enum values represent the transmission order as follows:
 *
 *          - BURST_TRANS_ORDER_MSB_FIRST: Most Significant Bit (MSB) first
 *          - BURST_TRANS_ORDER_LSB_FIRST: Least Significant Bit (LSB) first
 */
typedef enum {
    BURST_TRANS_ORDER_MSB_FIRST = 0,
    BURST_TRANS_ORDER_LSB_FIRST = 1
} burst_packet_trans_order_e;

/**
 * @brief Enumeration for PID Mode in Burst Packet Acknowledgment
 *
 * This enumeration defines the possible modes for the Burst Packet Acknowledgment PID.
 *
 * - BURST_USE_CUSTOMIZED_PID mode allows the user to specify a custom PID in the acknowledgment.
 * - BURST_USE_RECEIVED_PID mode uses the PID received in the burst packet in the acknowledgment.
 */
typedef enum {
    BURST_USE_CUSTOMIZED_PID = 0,
    BURST_USE_RECEIVED_PID   = 1
} burst_packet_ack_pid_mode_e;

/**
 * @brief Enumeration for Burst Packet Acknowledge Polarity
 *
 * This enumeration defines the possible polarities for acknowledging burst packets.
 * The polarity determines whether the acknowledgment signal is active high or active low.
 *
 * - BURST_ACK_0_DISABLE_1_ENABLE: Acknowledgment is disabled when the bit is set to 0
 *                                 and enabled when the bit is set to 1.
 *
 * - BURST_ACK_1_DISABLE_0_ENABLE: Acknowledgment is disabled when the bit is set to 1
 *                                 and enabled when the bit is set to 0.
 */
typedef enum
{
    BURST_ACK_0_DISABLE_1_ENABLE = 0,
    BURST_ACK_1_DISABLE_0_ENABLE = 1,
} burst_packet_ack_polarity_e;

/**
 * @brief Structure for LL Burst Packet Common Parameters
 *
 * This structure defines the common parameters for a Low Latency Burst Packet.
 *
 * @note The CRC length is represented in bytes and can range from 0 to 2,
 *       where 0 means CRC is not used.
 *       The address length is represented in bytes and can range from 3 to 5.
 */
typedef struct {
    burst_packet_trans_order_e trans_order; /**< Transmission order of the burst packet */
    uint8_t crc_len_bytes;                  /**< CRC length in bytes (0..2) */
    uint8_t addr_len_bytes;                 /**< Address length in bytes (3..5) */
} ll_burst_packet_common_param_t;

/**
 * @brief Structure for LL Burst Packet (Enhanced mode)
 *
 * This structure contains the parameters for the Low-Level Burst Packet Enhanced mode.
 * The parameters are used to configure the behavior of the burst packet transmission.
 *
 * @note The length of payload length field can be either 6 or 8 bits.
 */
typedef struct {
    uint8_t payload_len_field_bits;             /**< length of length field in bits */
    burst_packet_ack_pid_mode_e ack_pid_mode;
    burst_packet_ack_polarity_e ack_polarity;
    uint8_t rtx_turn_around_time_us;            /**< turn-around time from Rx to Tx in microseconds */
    uint16_t tx_retrans_delay_us;               /**< Transmission retransmission delay in microseconds */
    uint16_t tx_packet_delay_us;                /**< Transmission packet delay in microseconds */
} ll_burst_packet_enhanced_param_t;

/**
 * @brief Structure for LL Burst Packet (Legacy mode)
 *
 * @note The maximum length of payload can be 1 to 255.
 */
typedef struct
{
    uint8_t payload_len;                         /**< maximum length of payload */
} ll_burst_packet_legacy_param_t;

/**
* @brief Enable and Configure the enhanced burst mode
*
* This function allows you to set up advanced parameters for enhanced burst transmission mode.
*
* @param param          Pointer to the common burst packet parameter structure.
* @param enhanced       Pointer to the enhanced burst packet parameter structure.
* @return               0 on success, or an error code on failure.
*
* @warning This function should not be called again after one or more burst packet
* objects are created.
*/
// int ll_burst_packet_config_enhanced(const ll_burst_packet_common_param_t *param,
//    const ll_burst_packet_enhanced_param_t *enhanced);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Enable and Configure the legacy burst mode
 *
 * This function allows you to set up advanced parameters for legacy burst transmission mode.
 *
 * @param param         Pointer to a structure containing common burst packet parameters.
 * @param legacy        Pointer to a structure containing legacy burst packet parameters.
 * @return              0 on success, or an error code on failure.
 *
 * @warning This function should not be called again after one or more burst packet
* objects are created.
 */
// int ll_burst_packet_config_legacy(const ll_burst_packet_common_param_t *param,
//     const ll_burst_packet_legacy_param_t *legacy);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Create a burst packet object in initiator role
 *
 * This function initializes a packet object in initiator role with specified
 * parameters for the depth of the receive and transmit FIFOs, the timeout for
 * waiting for an acknowledgment, and whether to enable the whitening feature.
 *
 * @param rx_fifo_depth             The depth of the receive FIFO, representing the size of the receive buffer.
 * @param tx_fifo_depth             The depth of the transmit FIFO, representing the size of the transmit buffer.
 * @param wait_for_ack_timeout_us   The timeout for waiting for an acknowledgment, in microseconds.
 * @param enable_whiten             A boolean value indicating whether to enable the whitening feature.
 * @param on_done                   A callback function pointer to be called when packet Rx/Tx is done.
 * @param user_data                 A pointer to user data to be passed to the callback function.
 * @return                          On success, returns a pointer to the `ll_raw_packet` structure. On failure, returns `NULL`.
 */
// struct ll_raw_packet *ll_burst_packet_initiator_alloc(uint8_t rx_fifo_depth, uint8_t tx_fifo_depth,
//     uint16_t wait_for_ack_timeout_us, uint8_t enable_whiten,
//     f_ll_raw_packet_done on_done, void *user_data);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Set additional parameter of a burst packet object in initiator role
 *
 * @param packet                    The burst packet object.
 * @param tx_power                  Transmission power in dBm.
 * @param rf_channel_id             RF channel ID (0: 2402MHz, 1: 2404MHz, ...)
 * @param phy                       PHY (1: 1M, 2: 2M)
 * @param auto_retrans_cnt          Maximum automatic retransmission count when ack is not received.
 * @param addr                      Address.
 * @param crc_init                  CRC init value.
 *                                      - if `crc_len_bytes == 0`: ignored
 *                                      - if `crc_len_bytes == 1`: lowest 8 bits are used
 * @return                          0 on success, or an error code on failure.
 */
// int ll_burst_packet_initiator_set_param(struct ll_raw_packet *packet,
//     int8_t tx_power, int8_t phy_channel_id, uint8_t phy,
//     uint16_t auto_retrans_cnt, const uint8_t *addr, uint16_t crc_init);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Pushes data into the transmit FIFO of a burst packet in initiator role.
 *
 * @param packet                The burst packet object.
 * @param header                The header bits (lowest 2 bits) to be included in the packet.
 * @param enable_ack            A boolean value indicating whether to enable acknowledgment (1) or not (0).
 * @param data                  Pointer to the data to be included in the packet as payload.
 * @param size                  The size of the data to be included in the packet.
 * @return                      0 on success, or an error code on failure.
 *
 * @warning Ensure that the data size does not exceed the maximum payload length.
 */
// int ll_burst_packet_initiator_tx_fifo_push_data(struct ll_raw_packet *packet,
//     uint8_t header, uint8_t enable_ack, const void *data, int size);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Create a burst packet object in responder role
 *
 * This function initializes a packet object in responder role with specified
 * parameters for the depth of the receive and transmit FIFOs, number of lanes,
 * and whether to enable the whitening feature.
 *
 * @param rx_fifo_depth             The depth of the receive FIFO, representing the size of the receive buffer.
 * @param tx_fifo_depth             The depth of the transmit FIFO, representing the size of the transmit buffer.
 * @param lanes_num                 number of lanes.
 * @param enable_whiten             A boolean value indicating whether to enable the whitening feature.
 * @param on_done                   A callback function pointer to be called when packet Rx/Tx is done.
 * @param user_data                 A pointer to user data to be passed to the callback function.
 * @return                          On success, returns a pointer to the `ll_raw_packet` structure. On failure, returns `NULL`.
 */
// struct ll_raw_packet *ll_burst_packet_responder_alloc(uint8_t rx_fifo_depth, uint8_t tx_fifo_depth,
//     uint8_t lanes_num, uint8_t enable_whiten,
//     f_ll_raw_packet_done on_done, void *user_data);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Configuration of a lane
 *
 * - addr:      address of this lane. only the lowest `addr_len_bytes` is used.
 * - crc_init:  CRC init value.
 * - auto_ack:  Automatically generate and send an ACK packet when a packet is received.
 */
typedef struct
{
    uint8_t addr[6];
    uint16_t crc_init;
    uint8_t auto_ack;
} ll_burst_responder_lane_cfg_t;

/**
 * @brief Sets parameters for a burst packet in responder role.
 *
 * This function configures the properties of a burst packet in responder role,
 * including the transmission power, physical channel ID, PHY mode,
 * and lane configurations.
 *
 * @param packet            Pointer to the raw packet structure.
 * @param tx_power          Transmission power in dBm.
 * @param rf_channel_id     RF channel ID (0: 2402MHz, 1: 2404MHz, ...)
 * @param phy               PHY (1: 1M, 2: 2M)
 * @param lanes             Pointer to configurations of each lane.
 * @return                  0 on success, non-zero on failure.
 */
// int ll_burst_packet_responder_set_param(struct ll_raw_packet *packet,
//     int8_t tx_power, int8_t phy_channel_id, uint8_t phy,
//     const ll_burst_responder_lane_cfg_t *lanes);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Pushes data into the transmit FIFO for a burst packet in responder role.
 *
 * This function is used to push data into the transmit FIFO of a burst packet responder.
 *
 * @param packet            The burst packet object.
 * @param lane_id           Identifier for the lane on which the data is to be transmitted.
 * @param header            The header bits (lowest 2 bits) to be included in the packet.
 * @param data              Pointer to the data to be included in the packet as payload.
 * @param size              The size of the data to be included in the packet.
 * @return                  0 on success, non-zero on failure.
 *
 * @warning Ensure that the size does not exceed the maximum allowed payload length.
 */
// int ll_burst_packet_responder_tx_fifo_push_data(struct ll_raw_packet *packet,
//                                 uint8_t lane_id, uint8_t header, const void *data, int size);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Flushes the transmit FIFO of a burst packet.
 *
 * This function is used to clear the transmit FIFO (First In, First Out) buffer
 * associated with the given burst packet. This is typically done to ensure that
 * no packets are left in the buffer that may cause issues with subsequent transmissions.
 *
 * @param packet            The burst packet object.
 * @return                  0 on success, non-zero on failure.
 *
 * @warning When the burst packet is still running, this function will fail.
 */
// int ll_burst_packet_flush_tx_fifo(struct ll_raw_packet *packet);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Runs a burst packet operation.
 *
 * This function schedules a burst packet transmission for the given burst packet.
 * The transmission is scheduled to occur at the specified time and within the
 * given transmission window.
 *
 * @param packet                The burst packet object.
 * @param when                  The time at which the burst transmission should start
 *                              in microseconds.
 * @param window                The duration of the transmission window in microseconds.
 *                              Ignored in initiator role.
 * @return                      0 on success, non-zero on failure.
 *
 * @note "Success" means that the burst packet operation will be scheduled.
 * This function returns immediately.
 */
// int ll_burst_packet_run(struct ll_raw_packet *packet, uint64_t when, uint32_t window);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Callback function type for visiting each received PDU in a burst packet.
 *
 * This type defines a function pointer that is used to visit each received PDU in a burst.
 * The function should match this prototype to be compatible with the
 * `ll_burst_packet_check_each_rx_pdu` function.
 *
 * @param index             The index of the PDU counting from 0.
 * @param lane_id           The lane ID of the PDU (ignored in initiator role).
 * @param status            The status of the packet (always be 0).
 * @param header            The header (lowest 2 bits) information of the PDU.
 * @param data              A pointer to the payload contained in the PDU.
 * @param size              The size of the payload.
 * @param rssi              The received signal strength indication of the PDU in dBm.
 * @param user_data         User-defined data passed to the callback function.
 */
typedef void (* f_ll_burst_pdu_visitor)(int index, uint8_t lane_id,
              uint8_t status, uint8_t header,
              const void *data, int size, int rssi, void *user_data);

/**
 * @brief Check each received PDU in a burst packet.
 *
 * This function iterates through each received PDU in the specified burst packet,
 * invoking the provided visitor callback for each one.
 *
 * @param packet            The burst packet object.
 * @param visitor           The callback function to invoke for each received PDU.
 * @param user_data         User-defined data to pass to the visitor callback.
 * @return                  Total number of PDUs visited. When error occurs,
 *                          a negative error code is returned.
 */
// int ll_burst_packet_check_each_rx_pdu(struct ll_raw_packet *packet,
//     f_ll_burst_pdu_visitor visitor, void *user_data);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Retrieves the number of transmit (TX) PDUs in the burst packet in
 * initiator role.
 *
 * @param packet            The burst packet object.
 * @return                  The number of transmit PDUs in the burst.
 */
// int ll_burst_packet_initiator_get_tx_num(struct ll_raw_packet *packet);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Retrieves the number of transmit (TX) PDUs on a lane in the burst packet in
 * responder role.
 *
 * @param packet            The burst packet object.
 * @param lane_id           The lane ID
 * @return                  The number of transmit PDUs on the lane.
 */
// int ll_burst_packet_responder_get_tx_num(struct ll_raw_packet *packet, uint8_t lane_id);
// WARNING: ^^^ this API is not available in this release


/**
 ****************************************************************************************
 * @brief Lock RF frequency
 *
 * Once locked, all RF activities will occur on the specified channel, no matter
 * BLE ADV/SCAN/CONNECTION, or raw packets.
 *
 * `ll_raw_packet_set_bare_mode` with `freq_mhz != 0` also relies on this functionality.
 * It will call this and `ll_unlock_frequency()` automatically.
 *
 * Locking is a low level configuration, of which BLE sub-system is totally unaware.
 * Do NOT forget whitening: after locking to 2402MHz (channel 37), do not expecting
 * all adv on 37/38/39 can be received, since BLE sub-system is still transmitting
 * adv on 37/38/39 with whitening corresponding to each channel.
 *
 * To make all 3 adv on a single channel receivable (properly whitened as on channel 37),
 * call `ll_override_whitening_init_value(1, 0x53)`.
 *
 * Nesting of locking:
 *      lock(f0);       // locked to f0
 *          lock(f1);   // locked to f1
 *          unlock();
 *          ...         // stilling locked to f1
 *      unlock();
 *      ...             // unlocked
 *
 * @param[in]   freq_mhz            channel frequency in MHz
 ****************************************************************************************
 */
void ll_lock_frequency(int freq_mhz);

/**
 ****************************************************************************************
 * @brief Unlock RF frequency
 *
 * After unlocking, RF behaviors are back to normal again.
 ****************************************************************************************
 */
void ll_unlock_frequency(void);

/**
 ****************************************************************************************
 * @brief Allocate memory from LL internal heap
 *
 * @param[in]   size                memory size in bytes
 * @return                          memory pointer if successful else NULL
 ****************************************************************************************
 */
void *ll_malloc(uint16_t size);

/**
 ****************************************************************************************
 * @brief Free memory allocated from LL internal heap
 *
 * @param[in]   buffer              memory pointer
 ****************************************************************************************
 */
void ll_free(void *buffer);

/**
 ****************************************************************************************
 * @brief Get unallocated size of LL internal heap
 *
 * @return                          unallocated size in bytes
 ****************************************************************************************
 */
int ll_get_heap_free_size(void);

/**
 ****************************************************************************************
 * @brief Set the max possbible number of connections
 *
 * @param[in]  max_number       max number of connections
 * @return                      max number of connections (which is <= max_number)
 ****************************************************************************************
 */
int ll_set_max_conn_number(int max_number);

/**
 ****************************************************************************************
 * @brief Fix on one single channel or all channels
 *
 * @param[in]  channel_index    channel index (37/38/39, or 0x0 for all channels)
 ****************************************************************************************
 */
void ll_scan_set_fixed_channel(int channel_index);

/**
 ****************************************************************************************
 * @brief Modify the default access address of SIG advertising PDU
 *
 * Note: Once changed, advertising data can't be detected by standard scanners.
 *       This is used for debugging only.
 *
 * @param[in]  acc_addr     new access address
 ****************************************************************************************
 */
void ll_set_adv_access_address(uint32_t acc_addr);

/**
 ****************************************************************************************
 * @brief set Link Layer connection interval unit.
 *        Note:
 *          * This API can only be called after Link Layer got initialized.
 *          * This API can be used to achieve a non-standard smaller connection inteval (< 1ms).
 *          * A non-standard interval may cause inconsistent within HCI commands/events.
 *
 * @param[in]  unit         connection interval unit in micro-seconds. (default: 1250us)
 ****************************************************************************************
 */
void ll_set_conn_interval_unit(uint16_t unit);

/**
 ****************************************************************************************
 * @brief Set ACL report latency within an event of a connection
 *
 * `latency` determines how many Data PDU are received within a single connection event
 * before reporting to Host, for example,
 *
 *  - `latency` = 0: _maximum latency_, report to Host after a connection event ends;
 *  - `latency` = 1: _minimum latency_, Whenever a PDU is received, report to Host as soon as possible.
 *
 * Default: ~4. Vary between different bundles.
 *
 * @param[in]  conn_handle      handle of an existing connection
 * @param[in]  latency          latency
 ****************************************************************************************
 */
void ll_set_conn_acl_report_latency(uint16_t conn_handle, int latency);

/**
 * @brief Signature of callback for ACL data previewer
 *
 * @param[in] conn_handle           connection handle
 * @param[in] acl_flags             0x01: Continuation fragment of an L2CAP message,
 *                                        or an Empty PDU.
 *                                  0x02: Start of an L2CAP message,
 *                                        or a complete L2CAP message with no fragmentation.
 * @param[in] data                  payload data
 * @param[in] len                   length of payload data
*/
typedef void (*f_ll_hci_acl_data_preview)(uint16_t conn_handle,
    const uint8_t acl_flags, const void *data, int len);

/**
 * @brief Register a function to _preview_ BLE ACL data before posting to Host.
 *
 * ACL data is posted to Host as usual.
 *
 * This function is be called in the context of controller task. This may be used
 * to achieve **least** ACL processing latency.
 *
 * @param[in] cb                    the callback function
*/
void ll_register_hci_acl_previewer(f_ll_hci_acl_data_preview preview);

/**
 ****************************************************************************************
 * @brief Override standard whitening init value
 *
 * @param[in]  override     enable override: 1; disable override: 0 (default: disable)
 * @param[in]  value        customized whitening init value when override is enabled:
 *                          lfsr[0] = bit[0]; ...; lfsr[6] = bit[6].
 *                          For example, standard value for channel 37 is 0x53.
 ****************************************************************************************
 */
void ll_override_whitening_init_value(uint8_t override, uint8_t value);

/**
 ****************************************************************************************
 * @brief Allow the reception of adv packets that use a nonstandard ADV TYPE
 *
 * @param[in]  allowed      allowed: 1; not allowed: 0 (default: not allowed)
 * @param[in]  type         nonstandard ADV TYPE
 ****************************************************************************************
 */
void ll_allow_nonstandard_adv_type(uint8_t allowed, uint8_t type);

/**
 ****************************************************************************************
 * @brief Set CTE bit
 *
 * @param[in]  bit          CTE bit: 0/1 (default: 1)
 ****************************************************************************************
 */
void ll_set_cte_bit(uint8_t bit);

/**
 ****************************************************************************************
 * @brief Generate a new P-256 public/private key pair
 ****************************************************************************************
 */
typedef void (*f_start_generate_p256_key_pair)(void);

/**
 ****************************************************************************************
 * @brief Initiate generation of a Diffie-Hellman key
 *
 * @param[in]  key_type         0: Use the generated private key; 1: Use the debug private key
 * @param[in]  remote_pub_key   The remote P-256 public key: X, Y format
 *                              Octets  0-31: X co-ordinate
 *                              Octets 32-63: Y co-ordinate
 *                              Big-endian Format
 * @return                      0 if `remote_pub_key` is an valid key else invalid
 ****************************************************************************************
 */
typedef int  (*f_start_generate_dhkey)(int key_type, const uint8_t *remote_pub_key);

/**
 ****************************************************************************************
 * @brief A new pair of P-256 keys are genereted
 *
 * @param[in]  status           0: ok; else error
 * @param[in]  pub_key          public key
 ****************************************************************************************
 */
void ll_p256_key_pair_generated(int status, const uint8_t *pub_key);

/**
 ****************************************************************************************
 * @brief Diffie Hellman key generation has been completed
 *
 * @param[in]  status           0: ok; else error
 * @param[in]  dh_key           Diffie Hellman Key
 ****************************************************************************************
 */
void ll_dhkey_generated(int status, const uint8_t *dh_key);

/**
 ****************************************************************************************
 * @brief Setup extern ECC engine
 *
 * @param[in]  start_generate_p256_key_pair             trigger for key pair generation
 * @param[in]  start_generate_dhkey                     trigger for DHKey generation
 ****************************************************************************************
 */
void ll_install_ecc_engine(f_start_generate_p256_key_pair start_generate_p256_key_pair,
                          f_start_generate_dhkey start_generate_dhkey);

/**
 ****************************************************************************************
 * @brief Low level AES encryption in blocking mode
 *
 * @param[in]  key                      key (little-endian)
 * @param[in]  plaintext                plaintext (little-endian)
 * @param[out] ciphertext               ciphertext (big-endian)
 * @return                              0 if completed successfully; non-0 if hardware is not ready or busy
 *                                      When non-0 is returned, it is possible to retry later.
 ****************************************************************************************
 */
int ll_aes_encrypt(const uint8_t *key, const uint8_t *plaintext,
                  uint8_t *ciphertext);

#ifdef __cplusplus
}
#endif

#endif
