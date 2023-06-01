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
} ll_config_item_t;

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
 * @param[in]  delta            delta of tx power in dBm
 *                              Note: A positive value indicates a request to increase the
 *                                    transmit power level; negtive to decrease the transmit
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
 * @param[out] interval         interval (unit: us)
 * @return                      0 if successful else non-0
 ****************************************************************************************
 */
// int ll_get_conn_info(const uint16_t conn_handle,
//                     uint32_t *access_addr,
//                     uint32_t *crc_init,
//                     uint32_t *interval);
// WARNING: ^^^ this API is not available in this release


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
 * @param[out] time_offset      time offset between the start of the first connection event and `from_time` (unit: us)
 * @param[out] event_count      connection event count of the first connection event
 * @param[out] channel_ids      physical channel IDs of the upcoming `number` connection events
 * @return                      0 if successful else non-0
 ****************************************************************************************
 */
// int ll_get_conn_events_info(const uint16_t conn_handle,
//                             int number,
//                             uint64_t from_time,
//                             uint32_t *time_offset,
//                             uint16_t *event_count,
//                             uint8_t *channel_ids);
// WARNING: ^^^ this API is not available in this release


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
 * @param[in]   phy_channel_id      physical channel ID (0: 2402MHz, 1: 2404MHz, ...)
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
                          int8_t phy_channel_id,
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
 * @return                          0 if successful else error code
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
 * @return                          0 if successful else error code
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
 * @param[in]   window              Window length to run ack-able packet
 * @return                          0 if successful else error code
 ****************************************************************************************
 */
int ll_ackable_packet_run(struct ll_raw_packet *packet,
                        uint64_t when,
                        uint32_t window);

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
 *          * A non-standard interval may cause inconsistant within HCI commands/events.
 *
 * @param[in]  unit         connection interval unit in micro-seconds. (default: 1250us)
 ****************************************************************************************
 */
void ll_set_conn_interval_unit(uint16_t unit);

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
// void ll_p256_key_pair_generated(int status, const uint8_t *pub_key);
// WARNING: ^^^ this API is not available in this release


/**
 ****************************************************************************************
 * @brief Diffie Hellman key generation has been completed
 *
 * @param[in]  status           0: ok; else error
 * @param[in]  dh_key           Diffie Hellman Key
 ****************************************************************************************
 */
// void ll_dhkey_generated(int status, const uint8_t *dh_key);
// WARNING: ^^^ this API is not available in this release


/**
 ****************************************************************************************
 * @brief Setup extern ECC engine
 *
 * @param[in]  start_generate_p256_key_pair             trigger for key pair generation
 * @param[in]  start_generate_dhkey                     trigger for DHKey generation
 ****************************************************************************************
 */
// void ll_install_ecc_engine(f_start_generate_p256_key_pair start_generate_p256_key_pair,
//                           f_start_generate_dhkey start_generate_dhkey);
// WARNING: ^^^ this API is not available in this release


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
