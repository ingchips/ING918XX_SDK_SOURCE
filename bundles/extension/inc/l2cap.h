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

#ifndef __L2CAP_H
#define __L2CAP_H

#include "bluetooth.h"
#include "bt_types.h"
#include "gap.h"

#ifdef __cplusplus
extern "C" {
#endif

void l2cap_register_fixed_channel(btstack_packet_handler_t packet_handler, uint16_t channel_id);
int  l2cap_can_send_fixed_channel_packet_now(hci_con_handle_t con_handle, uint16_t channel_id);
void l2cap_request_can_send_fix_channel_now_event(hci_con_handle_t con_handle, uint16_t channel_id);
int l2cap_send_connectionless(hci_con_handle_t con_handle, uint16_t cid,
                               const uint8_t *data1, uint16_t data1_len,
                               const uint8_t *data2, uint16_t data2_len);
int l2cap_send_connectionless3(hci_con_handle_t con_handle, uint16_t cid,
                               const uint8_t *data1, uint16_t data1_len,
                               const uint8_t *data2, uint16_t data2_len,
                               const uint8_t *data3, uint16_t data3_len);

int l2cap_le_send_flow_control_credit(hci_con_handle_t con_handle,uint8_t id ,uint16_t cid,uint16_t credits);

// PTS Testing
int l2cap_send_echo_request(hci_con_handle_t con_handle, uint8_t *data, uint16_t len);
void l2cap_require_security_level_2_for_outgoing_sdp(void);

// Used by RFCOMM - similar to l2cap_can-send_packet_now but does not check if outgoing buffer is reserved
// int  l2cap_can_send_prepared_packet_now(uint16_t local_cid);
// WARNING: ^^^ this API is not available in this release


/* API_START */

/**
 * @brief Set up L2CAP and register L2CAP with HCI layer.
 */
void l2cap_init(void);

/**
 * @brief Registers packet handler for LE Connection Parameter Update events
 */
void l2cap_register_packet_handler(btstack_packet_handler_t handler);

/**
 * @brief Get max MTU for Classic connections based on btstack configuration
 */
uint16_t l2cap_max_mtu(void);

/**
 * @brief Get max MTU for LE connections based on btstack configuration
 */
uint16_t l2cap_max_le_mtu(void);

/**
 * @brief Creates L2CAP channel to the PSM of a remote device with baseband address. A new baseband connection will be initiated if necessary.
 * @param packet_handler
 * @param address
 * @param psm
 * @param mtu
 * @param local_cid
 * @param status
 */
uint8_t l2cap_create_channel(btstack_packet_handler_t packet_handler, bd_addr_t address, uint16_t psm, uint16_t mtu, uint16_t * out_local_cid);

/**
 * @brief Disconnects L2CAP channel with given identifier.
 */
void l2cap_disconnect(uint16_t local_cid, uint8_t reason);

/**
 * @brief Queries the maximal transfer unit (MTU) for L2CAP channel with given identifier.
 */
uint16_t l2cap_get_remote_mtu_for_local_cid(uint16_t local_cid);

/**
 * @brief Sends L2CAP data packet to the channel with given identifier.
 */
int l2cap_send(uint16_t local_cid, uint8_t *data, uint16_t len);

/**
 * @brief Registers L2CAP service with given PSM and MTU, and assigns a packet handler.
 */
uint8_t l2cap_register_service(btstack_packet_handler_t packet_handler, uint16_t psm, uint16_t mtu, gap_security_level_t security_level);

/**
 * @brief Unregisters L2CAP service with given PSM.  On embedded systems, use NULL for connection parameter.
 */
void l2cap_unregister_service(uint16_t psm);

/**
 * @brief Accepts incoming L2CAP connection.
 */
// void l2cap_accept_connection(uint16_t local_cid);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Deny incoming L2CAP connection.
 */
// void l2cap_decline_connection(uint16_t local_cid, uint8_t reason);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief Check if outgoing buffer is available and that there's space on the Bluetooth module
 */
int  l2cap_can_send_packet_now(uint16_t local_cid);

/**
 * @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible
 * @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
 *       so packet handler should be ready to handle it
 * @param local_cid
 */
void l2cap_request_can_send_now_event(uint16_t local_cid);

/**
 * @brief Request an update of the connection parameter for a given LE connection
 * @param handle
 * @param conn_interval_min (unit: 1.25ms)
 * @param conn_interval_max (unit: 1.25ms)
 * @param conn_latency
 * @param supervision_timeout (unit: 10ms)
 * @returns 0 if ok
 */
int l2cap_request_connection_parameter_update(hci_con_handle_t con_handle, uint16_t conn_interval_min,
    uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);

#ifdef CONNECTION_PARAMETER_DYNAMICALLY_ADJUST
uint8_t  l2cap_update_MTU_on_LL_link(l2cap_channel_t * channel);
#endif

/* API_END */

#ifdef __cplusplus
}
#endif
/*to define the error result of the connection response*/
#ifdef L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE
#define L2CAP_CONNECTION_REFUSED_LEPSM_NOT_SUPPORTED 0x0002
#define L2CAP_CONNECTION_REFUSED_RESERVED              0x0003
#define L2CAP_CONNECTION_REFUSED_NO_RESOURCE_AVAILABLE 0x0004
#define L2CAP_CONNECTION_REFUSED_INSUFFICIENT_AUTHEN   0x0005
#define L2CAP_CONNECTION_REFUSED_INSUFFICIENT_AUTHOR   0x0006
#define L2CAP_CONNECTION_REFUSED_INSUFFICIENT_ENCRYPT_KEY_SIZE 0x0007
#define L2CAP_CONNECTION_REFUSED_INSUFFICIENT_ENCRYPT 0x0008
#define L2CAP_CONNECTION_REFUSED_INVALID_SCID  0x0009
#define L2CAP_CONNECTION_REFUSED_SCID_ALREADY_ALLOCATED 0x000A
#endif

#endif // __L2CAP_H
