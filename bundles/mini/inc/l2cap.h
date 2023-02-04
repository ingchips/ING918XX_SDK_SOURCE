// ----------------------------------------------------------------------------
// Copyright Message
// ----------------------------------------------------------------------------
//
// INGCHIPS confidential and proprietary.
// COPYRIGHT (c) 2018-2023 by INGCHIPS
//
// All rights are reserved. Reproduction in whole or in part is
// prohibited without the written consent of the copyright owner.
//
//
// ----------------------------------------------------------------------------

#ifndef __L2CAP_H
#define __L2CAP_H

#include "bluetooth.h"
#include "bt_types.h"
#include "gap.h"

#ifdef __cplusplus
extern "C" {
#endif

/* API_START */

/**
 * @brief Set up L2CAP and register L2CAP with HCI layer.
 */
void l2cap_init(void);

/**
 * @brief add event handler with packet type `L2CAP_EVENT_PACKET`
 *
 * Note: Event code includes:
 *  * L2CAP_CONNECTION_REFUSED_INSUFFICIENT_ENCRYPT_KEY_SIZE
 *
 * @param[in]   callback_handler        the handler to be chained together
 */
void l2cap_add_event_handler(btstack_packet_callback_registration_t *callback_handler);

/**
 * @brief Registers packet handler for LE Connection Parameter Update related events
 *
 * Note: Packet type `HCI_EVENT_PACKET`. Event code includes:
 *  * L2CAP_EVENT_COMMAND_REJECT_RESPONSE
 *  * L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE
 *  * L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST
 *
 * @param[in]   handler                 the callback function
 */
void l2cap_register_packet_handler(btstack_packet_handler_t handler);

/**
 * @brief Get max MTU for LE connections based on btstack configuration
 *
 * @return      max LE MTU
 */
uint16_t l2cap_max_le_mtu(void);

#define l2cap_max_mtu               l2cap_max_le_mtu

/**
 * @brief Disconnects L2CAP channel with given identifier.
 *
 * @param[in]   local_cid               local channel identifier
 * @param[in]   reason                  reason (ignored)
 */
void l2cap_disconnect(uint16_t local_cid, uint8_t reason);

/**
 * @brief Queries the maximal transfer unit (MTU) for L2CAP channel with given identifier.
 *
 * @param[in]   local_cid               local channel identifier
 * @return                              peer MTU
 */
uint16_t l2cap_get_peer_mtu_for_local_cid(uint16_t local_cid);

/**
 * @brief Registers L2CAP service with given PSM and MTU, and assigns a packet handler.
 *
 * Note: Before creating a connection with a SPSM, the service must be registered.
 *
 * @param[in]   packet_handler          packet handler
 * @param[in]   psm                     SPSM (LE_PSM)
 * @param[in]   mtu                     MTU (23..max, max = l2cap_max_le_mtu())
 *                                      when mtu = 0, max LE MTU is used
 * @param[in]   security_level          security level
 * @return                              0 if successful else error occurs:
 *                                          * L2CAP_SERVICE_ALREADY_REGISTERED
 *                                          * BTSTACK_MEMORY_ALLOC_FAILED
 */
uint8_t l2cap_register_service(btstack_packet_handler_t packet_handler, uint16_t psm, uint16_t mtu,
    gap_security_level_t security_level);

/**
 * @brief Unregisters L2CAP service with given PSM.
 *
 * @param[in]   psm                     SPSM (LE_PSM)
 */
void l2cap_unregister_service(uint16_t psm);

/**
 * @brief Create a credit based connection for SPSM
 *
 *
 * @param[in]   credits                 initial credits assigned to peer
 * @param[in]   psm                     SPSM (LE_PSM)
 * @param[in]   handle                  connection handle
 * @param[out]  local_cid               identifier of the newly create channel
 * @return                              0 if successful else error occurs:
 *                                          * L2CAP_SERVICE_NOT_REGISTERED
 *                                          * BTSTACK_LE_CHANNEL_NOT_EXIST
 *                                          * L2CAP_CONNECTION_INSUFFICIENT_SECURITY
 *                                          * BTSTACK_MEMORY_ALLOC_FAILED
 */
uint8_t l2cap_create_le_credit_based_connection_request(uint16_t credits, uint16_t psm, uint16_t handle, uint16_t *local_cid);

/**
 * @brief Get credits information of a credit based connection
 *
 * `local_credits`: how many K-frames the peer is capable to receive,
 *  i.e. how many K-frames we (the _local_) can transmit;
 *
 * `peer_credits`: `local_credits` in the peer.
 *
 * @param[in]   local_cid               local channel identifier
 * @param[in]   peer_credits            peer credits at present
 * @param[in]   local_credits           local credits at present
 * @return                              0 if successful else error occurs:
 *                                          * BTSTACK_LE_CHANNEL_NOT_EXIST
 */
uint8_t l2cap_get_le_credit_based_connection_credits(uint16_t local_cid,
    uint32_t *peer_credits, uint32_t *local_credits);

/**
 * @brief send an SDU to peer of a credit based connection
 *
 * Note: SDU will be truncated according to peer MTU.
 *
 * @param[in]   local_cid               local channel identifier
 * @param[in]   data                    data of the SDU
 * @param[in]   len                     total length of the SDU
 * @return                              when >= 0: number of bytes that has been sent (to Tx queue)
 *                                          * if `return` == len: whole SDU has been sent;
 *                                          * if `return` < len: only a portion has been sent.
 *                                            (see `l2cap_credit_based_send_continue`)
 *                                      when < 0, error occurs:
 *                                          * -BTSTACK_LE_CHANNEL_NOT_EXIST
 */
int l2cap_credit_based_send(uint16_t local_cid, const uint8_t *data, uint16_t len);

/**
 * @brief continue sending an SDU to peer of a credit based connection
 *
 * Note: SDU will be truncated according to peer MTU.
 *
 * @param[in]   local_cid               local channel identifier
 * @param[in]   data                    remaining data of the SDU that has not been sent
 * @param[in]   len                     length of remaining data of the SDU
 * @return                              when >= 0: number of bytes that has been sent (to Tx queue)
 *                                          * if `return` == len: whole SDU has been sent;
 *                                          * if `return` < len: only a portion has been sent.
 *                                      when < 0, error occurs:
 *                                          * -BTSTACK_LE_CHANNEL_NOT_EXIST
 */
int l2cap_credit_based_send_continue(uint16_t local_cid, const uint8_t *data, uint16_t len);

/**
 * @brief add more credits to the peer of the specified channel, or rather notify
 * the peer that _local_ are capable of receiving more K-frames.
 *
 * @param[in]   local_cid               local channel identifier
 * @param[in]   credits                 credits to be added to the peer
 * @return                              0 if successful else error occurs:
 *                                          * BTSTACK_LE_CHANNEL_NOT_EXIST
 */
uint8_t l2cap_le_send_flow_control_credit(uint16_t local_cid, uint16_t credits);

/**
 * @brief Check if outgoing buffer is available and that there's space on the Bluetooth module
 */
int  l2cap_can_send_packet_now(uint16_t local_cid);

/**
 * @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible
 * @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
 *       so packet handler should be ready to handle it
 * @param[in]   local_cid               local channel identifier
 */
void l2cap_request_can_send_now_event(uint16_t local_cid);

/**
 * @brief Request an update of the connection parameter for a given LE connection
 *
 * Note: only Peripheral role can call this API.
 *
 * @param[in]   handle                  connection handle
 * @param[in]   conn_interval_min       (unit: 1.25ms)
 * @param[in]   conn_interval_max       (unit: 1.25ms)
 * @param[in]   conn_latency            slave latency
 * @param[in]   supervision_timeout     (unit: 10ms)
 * @returns                             0 if successful else error occurs:
 *                                          * ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER
 */
uint8_t l2cap_request_connection_parameter_update(hci_con_handle_t con_handle, uint16_t conn_interval_min,
    uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);

/** @brief for PTS Testing
 */
uint8_t l2cap_send_echo_request(hci_con_handle_t con_handle, const uint8_t *data, uint16_t len);

/* API_END */

#ifdef __cplusplus
}
#endif

/* Error result of the connection response */
#define L2CAP_CONNECTION_REFUSED_LEPSM_NOT_SUPPORTED    0x0002
#define L2CAP_CONNECTION_REFUSED_RESERVED               0x0003
#define L2CAP_CONNECTION_REFUSED_NO_RESOURCE_AVAILABLE  0x0004
#define L2CAP_CONNECTION_REFUSED_INSUFFICIENT_AUTHEN    0x0005
#define L2CAP_CONNECTION_REFUSED_INSUFFICIENT_AUTHOR    0x0006
#define L2CAP_CONNECTION_REFUSED_INSUFFICIENT_ENCRYPT_KEY_SIZE  0x0007
#define L2CAP_CONNECTION_REFUSED_INSUFFICIENT_ENCRYPT           0x0008
#define L2CAP_CONNECTION_REFUSED_INVALID_SCID                   0x0009
#define L2CAP_CONNECTION_REFUSED_SCID_ALREADY_ALLOCATED         0x000A

#endif // __L2CAP_H
