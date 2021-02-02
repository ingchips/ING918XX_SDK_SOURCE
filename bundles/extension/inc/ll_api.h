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

#ifndef _LL_API_H_
#define _LL_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief Give link layer (slave role) a hint on ce_len of a connection
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
 * @brief Set coded scheme of a connection when CODED is used
 *
 * @param[in]  conn_handle      handle of an existing connection
 * @param[in]  ci               0: S8, 1: S2 (default)
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
 * @brief Set default antenna ID
 *
 *          Note: This ID restored to default value (i.e. 0) when LLE is resetted.
 *
 * @param[in]  ant_id           ID of default antenna (default: 0)
 *
 ****************************************************************************************
 */
void ll_set_def_antenna(uint8_t ant_id);

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

#ifdef __cplusplus
}
#endif
   
#endif
