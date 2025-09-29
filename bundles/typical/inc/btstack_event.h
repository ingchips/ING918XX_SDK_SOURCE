/** @file
*   @brief basic function APIs and events  for bluetooth
*  Copyright Message
*
*
*  INGCHIPS confidential and proprietary.
*  COPYRIGHT (c) 2018-2023 by INGCHIPS
*
*  All rights are reserved. Reproduction in whole or in part is
*  prohibited without the written consent of the copyright owner.
*
*/
// ----------------------------------------------------------------------------

#ifndef __BTSTACK_EVENT_H
#define __BTSTACK_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bluetooth.h"
#include "compiler.h"
#include "btstack_util.h"
#include "gap.h"

/* API_START */

#ifdef C2NIM
#define decode_event_offset(packet, T, offset)     ((const T *)((const uint32_t)(packet) + offset))
#else
#define decode_event_offset(packet, T, offset)     ((const T *)((uint32_t)(packet) + offset))
#endif

/**
 * @brief Get event type
 * @param event
 * @return type of event
 */
static __INLINE uint8_t hci_event_packet_get_type(const uint8_t * event){

    return *event;
}

/**
 *  User messge
 */
typedef struct btstack_user_msg
{
    uint32_t    msg_id;
    void       *data;
    uint16_t    len;
} btstack_user_msg_t;

/**
 * @brief Get user message
 * @param event
 * @return type of event
 */
static __INLINE const btstack_user_msg_t *hci_event_packet_get_user_msg(const uint8_t * event){

    return decode_event_offset(event, btstack_user_msg_t, 4);
}

/**
 * @brief post a msg to bt stack task, and handle it in the context of bt stack task
 *
 * Note: this function is safe to be called in other threads/tasks.
 *
 * @param msg_id        (Max allowed ID: 0x80000000)
 * @param data
 * @param len
 * @return 0 if success; else error occurred
 */
uint32_t btstack_push_user_msg(uint32_t msg_id, const void *data, const uint16_t len);

typedef void (*f_btstack_user_runnable)(void *, uint16_t);

/**
 * @brief post a runnable object (function) to btstack task, and handle it in the context of bt stack task
 *
 * Note: this function is safe to be called in other threads/tasks.
 *
 * @param fun           user function to be called in the context of bt stack
 *                      `fun` will be called as `fun(data, user_value)`.
 * @param data          user data
 * @param user_value    another user data
 * @return 0 if success; else error occurred
 */
uint32_t btstack_push_user_runnable(f_btstack_user_runnable fun, void *data, const uint16_t user_value);

/**
 * @brief Reset the whole Host Controller stack as soon as possible
 *
 * HCI_Command_Complete event related to the HCI_Reset command marks the end of
 * this procedure. Apps shall not call any additional GAP APIs before it.
 *
 * Reset procedure:
 * 1. Host closes all connections gracefully;
 * 2. Send HCI_Reset command to Controller.
 *
 * Note: Controller heap is also reset, so all memory blocks allocated by
 *       `ll_malloc` are _lost_.
 *
 * @return              0: Message is sent to controller; otherwise failed to send
 */
uint8_t btstack_reset(void);

/**
 * capabilities of the stack
 */
typedef struct btstack_capabilities
{
    uint16_t adv_set_num;   // max number of advertising sets
    uint16_t conn_num;      // max number of connections
    uint16_t max_mtu;       // max MTU
    uint16_t device_db_num; // max number of devices in database
} btstack_capabilities_t;

/**
 * @brief Get BT stack capabilities.
 *
 * @param[out]  capabilities    see `btstack_capabilities_t`.
 */
void btstack_get_capabilities(btstack_capabilities_t *capabilities);

/***
 * @brief Get subevent code for le event
 * @param event packet
 * @return subevent_code
 */
static __INLINE uint8_t hci_event_le_meta_get_subevent_code(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 2);
}

#pragma pack(push, 1)

typedef struct
{
    uint8_t status;
    uint16_t conn_handle;
    uint8_t enabled;
} hci_encryption_change_event_t;

/**
 * @brief Get field status from event HCI_EVENT_ENCRYPTION_CHANGE
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static __INLINE uint8_t hci_event_encryption_change_get_status(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 2);
}
/**
 * @brief Get field connection_handle from event HCI_EVENT_ENCRYPTION_CHANGE
 * @param event packet
 * @return connection_handle
 * @note: btstack_type 2
 */
static __INLINE uint16_t hci_event_encryption_change_get_connection_handle(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field encryption_enabled from event HCI_EVENT_ENCRYPTION_CHANGE
 * @param event packet
 * @return encryption_enabled
 * @note: btstack_type 1
 */
static __INLINE uint8_t hci_event_encryption_change_get_encryption_enabled(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 5);
}


/**
 * @brief Get field num_hci_command_packets from event HCI_EVENT_COMMAND_COMPLETE
 * @param event packet
 * @return num_hci_command_packets
 * @note: btstack_type 1
 */
static __INLINE uint8_t hci_event_command_complete_get_num_hci_command_packets(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 2);
}
/**
 * @brief Get field command_opcode from event HCI_EVENT_COMMAND_COMPLETE
 * @param event packet
 * @return command_opcode
 * @note: btstack_type 2
 */
static __INLINE uint16_t hci_event_command_complete_get_command_opcode(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field return_parameters from event HCI_EVENT_COMMAND_COMPLETE
 * @param event packet
 * @return return_parameters
 * @note: btstack_type R
 */
static __INLINE const uint8_t * hci_event_command_complete_get_return_parameters(const uint8_t * event){
    return decode_event_offset(event, uint8_t, 5);
}

/**
 * @brief Get field status from event HCI_EVENT_COMMAND_STATUS
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static __INLINE uint8_t hci_event_command_status_get_status(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 2);
}
/**
 * @brief Get field num_hci_command_packets from event HCI_EVENT_COMMAND_STATUS
 * @param event packet
 * @return num_hci_command_packets
 * @note: btstack_type 1
 */
static __INLINE uint8_t hci_event_command_status_get_num_hci_command_packets(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 3);
}
/**
 * @brief Get field command_opcode from event HCI_EVENT_COMMAND_STATUS
 * @param event packet
 * @return command_opcode
 * @note: btstack_type 2
 */
static __INLINE uint16_t hci_event_command_status_get_command_opcode(const uint8_t * event){
    return little_endian_read_16(event, 4);
}

/**
 * @brief Get field hardware_code from event HCI_EVENT_HARDWARE_ERROR
 * @param event packet
 * @return hardware_code
 * @note: btstack_type 1
 */
static __INLINE uint8_t hci_event_hardware_error_get_hardware_code(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 2);
}

/**
 * @brief Get field status from event HCI_EVENT_ENCRYPTION_KEY_REFRESH_COMPLETE
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static __INLINE uint8_t hci_event_encryption_key_refresh_complete_get_status(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 2);
}
/**
 * @brief Get field handle from event HCI_EVENT_ENCRYPTION_KEY_REFRESH_COMPLETE
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t hci_event_encryption_key_refresh_complete_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 3);
}

/**
 * @brief Get field state from event BTSTACK_EVENT_STATE
 * @param event packet
 * @return state
 * @note: btstack_type 1
 */
static __INLINE uint8_t btstack_event_state_get_state(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 2);
}


/**
 * @brief Get field discoverable from event BTSTACK_EVENT_DISCOVERABLE_ENABLED
 * @param event packet
 * @return discoverable
 * @note: btstack_type 1
 */
static __INLINE uint8_t btstack_event_discoverable_enabled_get_discoverable(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 2);
}

/**
 * @brief Get field handle from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t l2cap_event_connection_parameter_update_request_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field interval_min from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST
 * @param event packet
 * @return interval_min
 * @note: btstack_type 2
 */
static __INLINE uint16_t l2cap_event_connection_parameter_update_request_get_interval_min(const uint8_t * event){
    return little_endian_read_16(event, 4);
}
/**
 * @brief Get field interval_max from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST
 * @param event packet
 * @return interval_max
 * @note: btstack_type 2
 */
static __INLINE uint16_t l2cap_event_connection_parameter_update_request_get_interval_max(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field latencey from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST
 * @param event packet
 * @return latency
 * @note: btstack_type 2
 */
static __INLINE uint16_t l2cap_event_connection_parameter_update_request_get_latency(const uint8_t * event){
    return little_endian_read_16(event, 8);
}
/**
 * @brief Get field timeout_multiplier from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST
 * @param event packet
 * @return timeout_multiplier
 * @note: btstack_type 2
 */
static __INLINE uint16_t l2cap_event_connection_parameter_update_request_get_timeout_multiplier(const uint8_t * event){
    return little_endian_read_16(event, 10);
}

/**
 * @brief Get field handle from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t l2cap_event_connection_parameter_update_response_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field result from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE
 * @param event packet
 * @return result
 */
static __INLINE uint16_t l2cap_event_connection_parameter_update_response_get_result(const uint8_t * event){
    return little_endian_read_16(event, 4);
}

/**
 * @brief Get field local_cid from event L2CAP_EVENT_CAN_SEND_NOW
 * @param event packet
 * @return local_cid
 * @note: btstack_type 2
 */
static __INLINE uint16_t l2cap_event_can_send_now_get_local_cid(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field handle from event L2CAP_COMMAND_REJECT_RSP
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t l2cap_event_command_reject_response_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field identifier from event L2CAP_COMMAND_REJECT_RSP
 * @param event packet
 * @return identifier
 */
static __INLINE uint8_t l2cap_event_command_reject_response_get_identifier(const uint8_t * event){
    return event[4];
}

/**
 * @brief Get field reason from event L2CAP_COMMAND_REJECT_RSP
 * @param event packet
 * @return reason
 */
static __INLINE uint16_t l2cap_event_command_reject_response_get_reason(const uint8_t * event){
    return little_endian_read_16(event, 5);
}

/**
 * @brief Get field status from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static inline uint8_t l2cap_event_channel_opened_get_status(const uint8_t * event){
    return event[2];
}
/**
 * @brief Get field address from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static inline void l2cap_event_channel_opened_get_address(const uint8_t * event, bd_addr_t address){
    reverse_bytes(&event[3], address, 6);
}
/**
 * @brief Get field handle from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static inline hci_con_handle_t l2cap_event_channel_opened_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 9);
}
/**
 * @brief Get field psm from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @return psm
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_channel_opened_get_psm(const uint8_t * event){
    return little_endian_read_16(event, 11);
}
/**
 * @brief Get field local_cid from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @return local_cid
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_channel_opened_get_local_cid(const uint8_t * event){
    return little_endian_read_16(event, 13);
}
/**
 * @brief Get field remote_cid from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @return remote_cid
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_channel_opened_get_remote_cid(const uint8_t * event){
    return little_endian_read_16(event, 15);
}
/**
 * @brief Get field local_mtu from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @return local_mtu
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_channel_opened_get_local_mtu(const uint8_t * event){
    return little_endian_read_16(event, 17);
}
/**
 * @brief Get field remote_mtu from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @return remote_mtu
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_channel_opened_get_remote_mtu(const uint8_t * event){
    return little_endian_read_16(event, 19);
}
/**
 * @brief Get field flush_timeout from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @return flush_timeout
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_channel_opened_get_flush_timeout(const uint8_t * event){
    return little_endian_read_16(event, 21);
}
/**
 * @brief Get field incoming from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @return incoming
 * @note: btstack_type 1
 */
static inline uint8_t l2cap_event_channel_opened_get_incoming(const uint8_t * event){
    return event[23];
}
/**
 * @brief Get field mode from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @return mode
 * @note: btstack_type 1
 */
static inline uint8_t l2cap_event_channel_opened_get_mode(const uint8_t * event){
    return event[24];
}
/**
 * @brief Get field fcs from event L2CAP_EVENT_CHANNEL_OPENED
 * @param event packet
 * @return fcs
 * @note: btstack_type 1
 */
static inline uint8_t l2cap_event_channel_opened_get_fcs(const uint8_t * event){
    return event[25];
}

/**
 * @brief Get field local_cid from event L2CAP_EVENT_CHANNEL_CLOSED
 * @param event packet
 * @return local_cid
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_channel_closed_get_local_cid(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field local_cid from event L2CAP_EVENT_PACKET_SENT
 * @param event packet
 * @return local_cid
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_packet_sent_get_local_cid(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field address_type from event L2CAP_EVENT_ECBM_INCOMING_CONNECTION
 * @param event packet
 * @return address_type
 * @note: btstack_type 1
 */
static inline uint8_t l2cap_event_ecbm_incoming_connection_get_address_type(const uint8_t * event){
    return event[2];
}
/**
 * @brief Get field address from event L2CAP_EVENT_ECBM_INCOMING_CONNECTION
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static inline void l2cap_event_ecbm_incoming_connection_get_address(const uint8_t * event, bd_addr_t address){
    reverse_bytes(&event[3], address, 6);
}
/**
 * @brief Get field handle from event L2CAP_EVENT_ECBM_INCOMING_CONNECTION
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static inline hci_con_handle_t l2cap_event_ecbm_incoming_connection_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 9);
}
/**
 * @brief Get field psm from event L2CAP_EVENT_ECBM_INCOMING_CONNECTION
 * @param event packet
 * @return psm
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_incoming_connection_get_psm(const uint8_t * event){
    return little_endian_read_16(event, 11);
}
/**
 * @brief Get field num_channels from event L2CAP_EVENT_ECBM_INCOMING_CONNECTION
 * @param event packet
 * @return num_channels
 * @note: btstack_type 1
 */
static inline uint8_t l2cap_event_ecbm_incoming_connection_get_num_channels(const uint8_t * event){
    return event[13];
}
/**
 * @brief Get field local_cid from event L2CAP_EVENT_ECBM_INCOMING_CONNECTION
 * @param event packet
 * @return local_cid
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_incoming_connection_get_local_cid(const uint8_t * event){
    return little_endian_read_16(event, 14);
}

/**
 * @brief Get field status from event L2CAP_EVENT_ECBM_CHANNEL_OPENED
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static inline uint8_t l2cap_event_ecbm_channel_opened_get_status(const uint8_t * event){
    return event[2];
}
/**
 * @brief Get field address_type from event L2CAP_EVENT_ECBM_CHANNEL_OPENED
 * @param event packet
 * @return address_type
 * @note: btstack_type 1
 */
static inline uint8_t l2cap_event_ecbm_channel_opened_get_address_type(const uint8_t * event){
    return event[3];
}
/**
 * @brief Get field address from event L2CAP_EVENT_ECBM_CHANNEL_OPENED
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static inline void l2cap_event_ecbm_channel_opened_get_address(const uint8_t * event, bd_addr_t address){
    reverse_bytes(&event[4], address, 6);
}
/**
 * @brief Get field handle from event L2CAP_EVENT_ECBM_CHANNEL_OPENED
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static inline hci_con_handle_t l2cap_event_ecbm_channel_opened_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 10);
}
/**
 * @brief Get field incoming from event L2CAP_EVENT_ECBM_CHANNEL_OPENED
 * @param event packet
 * @return incoming
 * @note: btstack_type 1
 */
static inline uint8_t l2cap_event_ecbm_channel_opened_get_incoming(const uint8_t * event){
    return event[12];
}
/**
 * @brief Get field psm from event L2CAP_EVENT_ECBM_CHANNEL_OPENED
 * @param event packet
 * @return psm
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_channel_opened_get_psm(const uint8_t * event){
    return little_endian_read_16(event, 13);
}
/**
 * @brief Get field local_cid from event L2CAP_EVENT_ECBM_CHANNEL_OPENED
 * @param event packet
 * @return local_cid
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_channel_opened_get_local_cid(const uint8_t * event){
    return little_endian_read_16(event, 15);
}
/**
 * @brief Get field remote_cid from event L2CAP_EVENT_ECBM_CHANNEL_OPENED
 * @param event packet
 * @return remote_cid
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_channel_opened_get_remote_cid(const uint8_t * event){
    return little_endian_read_16(event, 17);
}
/**
 * @brief Get field local_mtu from event L2CAP_EVENT_ECBM_CHANNEL_OPENED
 * @param event packet
 * @return local_mtu
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_channel_opened_get_local_mtu(const uint8_t * event){
    return little_endian_read_16(event, 19);
}
/**
 * @brief Get field remote_mtu from event L2CAP_EVENT_ECBM_CHANNEL_OPENED
 * @param event packet
 * @return remote_mtu
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_channel_opened_get_remote_mtu(const uint8_t * event){
    return little_endian_read_16(event, 21);
}

/**
 * @brief Get field remote_cid from event L2CAP_EVENT_ECBM_RECONFIGURED
 * @param event packet
 * @return remote_cid
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_reconfigured_get_remote_cid(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field mtu from event L2CAP_EVENT_ECBM_RECONFIGURED
 * @param event packet
 * @return mtu
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_reconfigured_get_mtu(const uint8_t * event){
    return little_endian_read_16(event, 4);
}
/**
 * @brief Get field mps from event L2CAP_EVENT_ECBM_RECONFIGURED
 * @param event packet
 * @return mps
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_reconfigured_get_mps(const uint8_t * event){
    return little_endian_read_16(event, 6);
}

/**
 * @brief Get field local_cid from event L2CAP_EVENT_ECBM_RECONFIGURATION_COMPLETE
 * @param event packet
 * @return local_cid
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_reconfiguration_complete_get_local_cid(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field reconfigure_result from event L2CAP_EVENT_ECBM_RECONFIGURATION_COMPLETE
 * @param event packet
 * @return reconfigure_result
 * @note: btstack_type 2
 */
static inline uint16_t l2cap_event_ecbm_reconfiguration_complete_get_reconfigure_result(const uint8_t * event){
    return little_endian_read_16(event, 4);
}

/**
 * @brief Get field handle from event GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static inline hci_con_handle_t gatt_event_characteristic_value_query_result_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field service_id from event GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT
 * @param event packet
 * @return service_id
 * @note: btstack_type 2
 */
static inline uint16_t gatt_event_characteristic_value_query_result_get_service_id(const uint8_t * event){
    return little_endian_read_16(event, 4);
}
/**
 * @brief Get field connection_id from event GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT
 * @param event packet
 * @return connection_id
 * @note: btstack_type 2
 */
static inline uint16_t gatt_event_characteristic_value_query_result_get_connection_id(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field value_handle from event GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT
 * @param event packet
 * @return value_handle
 * @note: btstack_type 2
 */
static inline uint16_t gatt_event_characteristic_value_query_result_get_value_handle(const uint8_t * event){
    return little_endian_read_16(event, 8);
}
/**
 * @brief Get field value_length from event GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT
 * @param event packet
 * @return value_length
 * @note: btstack_type L
 */
static inline uint16_t gatt_event_characteristic_value_query_result_get_value_length(const uint8_t * event){
    return little_endian_read_16(event, 10);
}
/**
 * @brief Get field value from event GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT
 * @param event packet
 * @return value
 * @note: btstack_type V
 */
static inline const uint8_t * gatt_event_characteristic_value_query_result_get_value(const uint8_t * event){
    return &event[12];
}
/**
 * @brief Get field handle from event GATT_EVENT_CHARACTERISTIC_QUERY_RESULT
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static inline hci_con_handle_t gatt_event_characteristic_query_result_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field service_id from event GATT_EVENT_CHARACTERISTIC_QUERY_RESULT
 * @param event packet
 * @return service_id
 * @note: btstack_type 2
 */
static inline uint16_t gatt_event_characteristic_query_result_get_service_id(const uint8_t * event){
    return little_endian_read_16(event, 4);
}
/**
 * @brief Get field connection_id from event GATT_EVENT_CHARACTERISTIC_QUERY_RESULT
 * @param event packet
 * @return connection_id
 * @note: btstack_type 2
 */
static inline uint16_t gatt_event_characteristic_query_result_get_connection_id(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field handle from event GATT_EVENT_SERVICE_QUERY_RESULT
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static inline hci_con_handle_t gatt_event_service_query_result_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field service_id from event GATT_EVENT_SERVICE_QUERY_RESULT
 * @param event packet
 * @return service_id
 * @note: btstack_type 2
 */
static inline uint16_t gatt_event_service_query_result_get_service_id(const uint8_t * event){
    return little_endian_read_16(event, 4);
}
/**
 * @brief Get field connection_id from event GATT_EVENT_SERVICE_QUERY_RESULT
 * @param event packet
 * @return connection_id
 * @note: btstack_type 2
 */
static inline uint16_t gatt_event_service_query_result_get_connection_id(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field handle from event GATT_EVENT_QUERY_COMPLETE
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static inline hci_con_handle_t gatt_event_query_complete_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field service_id from event GATT_EVENT_QUERY_COMPLETE
 * @param event packet
 * @return service_id
 * @note: btstack_type 2
 */
static inline uint16_t gatt_event_query_complete_get_service_id(const uint8_t * event){
    return little_endian_read_16(event, 4);
}
/**
 * @brief Get field connection_id from event GATT_EVENT_QUERY_COMPLETE
 * @param event packet
 * @return connection_id
 * @note: btstack_type 2
 */
static inline uint16_t gatt_event_query_complete_get_connection_id(const uint8_t * event){
    return little_endian_read_16(event, 6);
}
/**
 * @brief Get field att_status from event GATT_EVENT_QUERY_COMPLETE
 * @param event packet
 * @return att_status
 * @note: btstack_type 1
 */
static inline uint8_t gatt_event_query_complete_get_att_status(const uint8_t * event){
    return event[8];
}

/**
 * @brief Get field handle from event ATT_EVENT_MTU_EXCHANGE_COMPLETE
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t att_event_mtu_exchange_complete_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field MTU from event ATT_EVENT_MTU_EXCHANGE_COMPLETE
 * @param event packet
 * @return MTU
 * @note: btstack_type 2
 */
static __INLINE uint16_t att_event_mtu_exchange_complete_get_MTU(const uint8_t * event){
    return little_endian_read_16(event, 4);
}

/**
 * @brief Get field status from event ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static __INLINE uint8_t att_event_handle_value_indication_complete_get_status(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 2);
}
/**
 * @brief Get field conn_handle from event ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
 * @param event packet
 * @return conn_handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t att_event_handle_value_indication_complete_get_conn_handle(const uint8_t * event){
    return little_endian_read_16(event, 3);
}
/**
 * @brief Get field attribute_handle from event ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
 * @param event packet
 * @return attribute_handle
 * @note: btstack_type 2
 */
static __INLINE uint16_t att_event_handle_value_indication_complete_get_attribute_handle(const uint8_t * event){
    return little_endian_read_16(event, 5);
}

/**
 * @brief Get field handle from event SM_EVENT_JUST_WORKS_REQUEST
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_just_works_request_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field addr_type from event SM_EVENT_JUST_WORKS_REQUEST
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_just_works_request_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_JUST_WORKS_REQUEST
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_just_works_request_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field handle from event SM_EVENT_JUST_WORKS_CANCEL
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_just_works_cancel_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field addr_type from event SM_EVENT_JUST_WORKS_CANCEL
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_just_works_cancel_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_JUST_WORKS_CANCEL
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_just_works_cancel_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field handle from event SM_EVENT_PASSKEY_DISPLAY_NUMBER
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_passkey_display_number_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field addr_type from event SM_EVENT_PASSKEY_DISPLAY_NUMBER
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_passkey_display_number_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_PASSKEY_DISPLAY_NUMBER
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_passkey_display_number_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field passkey from event SM_EVENT_PASSKEY_DISPLAY_NUMBER
 * @param event packet
 * @return passkey
 * @note: btstack_type 4
 */
static __INLINE uint32_t sm_event_passkey_display_number_get_passkey(const uint8_t * event){
    return little_endian_read_32(event, 11);
}

/**
 * @brief Get field handle from event SM_EVENT_PASSKEY_DISPLAY_CANCEL
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_passkey_display_cancel_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field addr_type from event SM_EVENT_PASSKEY_DISPLAY_CANCEL
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_passkey_display_cancel_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_PASSKEY_DISPLAY_CANCEL
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_passkey_display_cancel_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field handle from event SM_EVENT_PASSKEY_INPUT_NUMBER
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_passkey_input_number_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field addr_type from event SM_EVENT_PASSKEY_INPUT_NUMBER
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_passkey_input_number_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_PASSKEY_INPUT_NUMBER
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_passkey_input_number_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field handle from event SM_EVENT_PASSKEY_INPUT_CANCEL
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_passkey_input_cancel_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}
/**
 * @brief Get field addr_type from event SM_EVENT_PASSKEY_INPUT_CANCEL
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_passkey_input_cancel_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_PASSKEY_INPUT_CANCEL
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_passkey_input_cancel_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field handle from event SM_EVENT_IDENTITY_RESOLVING_STARTED
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_identity_resolving_started_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field addr_type from event SM_EVENT_IDENTITY_RESOLVING_STARTED
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_identity_resolving_started_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_IDENTITY_RESOLVING_STARTED
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_identity_resolving_started_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field handle from event SM_EVENT_IDENTITY_RESOLVING_FAILED
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_identity_resolving_failed_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field addr_type from event SM_EVENT_IDENTITY_RESOLVING_FAILED
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_identity_resolving_failed_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_IDENTITY_RESOLVING_FAILED
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_identity_resolving_failed_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field handle from event SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_identity_resolving_succeeded_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field addr_type from event SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_identity_resolving_succeeded_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_identity_resolving_succeeded_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field le_device_db_index from event SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED
 * @param event packet
 * @return le_device_db_index
 * @note: btstack_type 2
 */
static __INLINE uint16_t sm_event_identity_resolving_succeeded_get_le_device_db_index(const uint8_t * event){
    return little_endian_read_16(event, 11);
}

/**
 * @brief Get field handle from event SM_EVENT_AUTHORIZATION_REQUEST
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_authorization_request_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field addr_type from event SM_EVENT_AUTHORIZATION_REQUEST
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_authorization_request_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_AUTHORIZATION_REQUEST
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_authorization_request_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field handle from event SM_EVENT_AUTHORIZATION_RESULT
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_authorization_result_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field addr_type from event SM_EVENT_AUTHORIZATION_RESULT
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_authorization_result_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_AUTHORIZATION_RESULT
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_authorization_result_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field authorization_result from event SM_EVENT_AUTHORIZATION_RESULT
 * @param event packet
 * @return authorization_result
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_authorization_result_get_authorization_result(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 11);
}

/**
 * @brief Get field handle from event SM_EVENT_NUMERIC_COMPARISON_REQUEST
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t sm_event_numeric_comparison_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field addr_type from event SM_EVENT_NUMERIC_COMPARISON_REQUEST
 * @param event packet
 * @return addr_type
 * @note: btstack_type 1
 */
static __INLINE uint8_t sm_event_numeric_comparison_get_addr_type(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field address from event SM_EVENT_NUMERIC_COMPARISON_REQUEST
 * @param event packet
 * @param Pointer to storage for address
 * @note: btstack_type B
 */
static __INLINE void sm_event_numeric_comparison_get_address(const uint8_t * event, uint8_t * address){
    reverse_bd_addr(decode_event_offset(event, uint8_t, 5), address);
}

/**
 * @brief Get field compare value from event SM_EVENT_NUMERIC_COMPARISON_REQUEST
 * @param event packet
 * @return passkey
 * @note: btstack_type 4
 */
static __INLINE uint32_t sm_event_numeric_comparison_get_compare_value(const uint8_t * event){
    return little_endian_read_32(event, 11);
}

/**
 * @brief Get IRK from event SM_EVENT_IRK_DHK_RESULT
 * @param event packet
 * @return irk
 */
static __INLINE const uint8_t *sm_event_irk_dhk_result_get_irk(const uint8_t *event) {
    return *decode_event_offset(event, uint8_t *, 2);
}

/**
 * @brief Get DHK from event SM_EVENT_IRK_DHK_RESULT
 * @param event packet
 * @return dhk
 */
static __INLINE const uint8_t *sm_event_irk_dhk_result_get_dhk(const uint8_t *event) {
    return *decode_event_offset(event, uint8_t *, 2 + sizeof(uintptr_t));
}

/**
 * @brief Get field handle from event GAP_EVENT_SECURITY_LEVEL
 * @param event packet
 * @return handle
 * @note: btstack_type H
 */
static __INLINE hci_con_handle_t gap_event_security_level_get_handle(const uint8_t * event){
    return little_endian_read_16(event, 2);
}

/**
 * @brief Get field security_level from event GAP_EVENT_SECURITY_LEVEL
 * @param event packet
 * @return security_level
 * @note: btstack_type 1
 */
static __INLINE uint8_t gap_event_security_level_get_security_level(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 4);
}

/**
 * @brief Get field status from event GAP_EVENT_DEDICATED_BONDING_COMPLETED
 * @param event packet
 * @return status
 * @note: btstack_type 1
 */
static __INLINE uint8_t gap_event_dedicated_bonding_completed_get_status(const uint8_t * event){
    return *decode_event_offset(event, uint8_t, 2);
}

/**
 * l2cap event complete SDU
 */
typedef struct l2cap_event_complete_sdu
{
    uint16_t total_length;          // total length of this SDU
    const uint8_t *payload;         // payload of this SDU
} l2cap_event_complete_sdu_t;

/**
 * l2cap event channel opened
 */
typedef struct l2cap_event_channel_opened
{
    uint8_t             status;         // 0: success, otherwise error
    bd_addr_t           peer_addr;      // peer address
    hci_con_handle_t    conn_handle;    // connection handle
    uint16_t            psm;            // Protocol/Service Multiplexer
    uint16_t            local_cid;      // local channel ID
    uint16_t            peer_cid;       // peer channel ID
    uint16_t            local_mtu;      // local MTU
    uint16_t            peer_mtu;       // peer MTU
    uint16_t            local_mps;      // local MPS (Maximum PDU Size)
    uint16_t            peer_mps;       // peer MPS (Maximum PDU Size)
    uint16_t            flush_timeout;  // flush timeout in milliseconds
    uint16_t            local_credits;  // local credits
    uint16_t            peer_credits;   // peer credits
} l2cap_event_channel_opened_t;

/**
 * l2cap event channel closed
 */
typedef struct l2cap_event_channel_closed
{
    uint16_t            local_cid;      // local channel ID
    uint8_t             reason;         // reason for closing the channel
} l2cap_event_channel_closed_t;

/**
 * l2cap event fragment SDU
 *
 * This is used to report fragments of a SDU that is larger than the MTU.
 * The first fragment will have offset 0, and the last fragment will have
 * length equal to total_length - offset.
 */
typedef struct l2cap_event_fragment_sdu
{
    uint16_t total_length;          // total length of this SDU
    uint16_t offset;                // offset of this fragment within a SDU
    uint16_t length;                // length of this fragment within a SDU
    const uint8_t *payload;         // payload of this fragment
} l2cap_event_fragment_sdu_t;

/**
 * read rssi return parameters
 */
typedef struct event_command_complete_return_param_read_rssi
{
    uint8_t          status;        // 0: success, otherwise error
    hci_con_handle_t conn_handle;   // connection handle
    int8_t           rssi;          // in dBm
} event_command_complete_return_param_read_rssi_t;

/**
 * read phy return parameters
 */
typedef struct event_command_complete_return_param_read_phy
{
    uint8_t          status;         // 0: success, otherwise error
    hci_con_handle_t conn_handle;    // connection handle
    phy_type_t       tx_phy;         // transmit PHY
    phy_type_t       rx_phy;         // receive PHY
} event_command_complete_return_param_read_phy_t;

/**
 * read antenna information return parameters
 */
typedef struct event_command_complete_return_param_read_antenna_info
{
    uint8_t          status;                        // 0: success, otherwise error
    uint8_t          supported_switching_rates;     // supported switching rates
    uint8_t          num_antennae;                  // number of antennas
    uint8_t          max_switching_pattern_len;     // maximum length of switching pattern
    uint8_t          max_cte_length;                // maximum length of a transmitted CTE supported in 8 µs units
} event_command_complete_return_param_read_antenna_info_t;

/**
 * conn packets
 */
typedef struct event_conn_packets
{
    hci_con_handle_t conn_handle;           // connection handle
    uint16_t         num_of_packets;        // number of packets
} event_conn_packets_t;

/**
 * event for number of complete packets
 */
typedef struct event_num_of_complete_packets
{
    uint8_t        num_handles;                 // number of connection handles
    event_conn_packets_t complete_packets[0];   // array of connection handles and number of packets
} event_num_of_complete_packets_t;

/**
 * event for sm state changed
 */
typedef struct sm_event_state_changed {
    uint16_t conn_handle;   // connection handle
    uint8_t reason;         // reason for state change
} sm_event_state_changed_t;

/**
 * event for disconnect complete
 */
typedef struct event_disconn_complete
{
    uint8_t status;             // 0: success, otherwise error
    uint16_t conn_handle;       // connection handle
    uint8_t reason;             // reason for disconnection
} event_disconn_complete_t;

/**
 * event for vendor ccm complete
 */
typedef struct
{
    uint8_t  status;        // for DEC, 0 means OK, 1 means ERROR
    uint8_t  type;          // 0: encrypt  1: decrypt
    uint8_t  mic_size;      // mic size in bytes
    uint16_t msg_len;       // length of the message to be encrypted/decrypted
    uint16_t aad_len;       // length of the additional authenticated data
    uint32_t tag;           // same value as in command
    uint8_t *out_msg;       // pointer to the output message buffer, which is the same as in command
} event_vendor_ccm_complete_t;

/**
 * le event for read local P-256 public key complete
 */
typedef struct le_meta_event_read_local_p256_pub_key_complete
{
    // Status of received command
    uint8_t             status;
    // Local P-256 public key X coordinate
    uint8_t             key_x_coordinate[32];
    // Local P-256 public key Y coordinate
    uint8_t             key_y_coordinate[32];
} le_meta_event_read_local_p256_pub_key_complete_t;

/**
 * le event for generate dhkey complete
 */
typedef struct le_meta_event_generate_dhkey_complete
{
    // Status of received command
    uint8_t             status;
    // Diffie Hellman Key
    uint8_t             dh_key[32];
} le_meta_event_generate_dhkey_complete_t;

/**
 * le event for create connection complete
 */
typedef struct le_meta_event_create_conn_complete
{
    //Status of received command
    uint8_t             status;
    //Connection handle
    uint16_t            handle;
    //Device role - 0=Master/ 1=Slave
    uint8_t             role;
    //Peer address type - 0=public/1=random
    bd_addr_type_t      peer_addr_type;
    //Peer address
    bd_addr_t           peer_addr;
    //Connection interval
    uint16_t            interval;
    //Connection latency
    uint16_t            latency;
    //Link supervision timeout
    uint16_t            sup_timeout;
    //Master clock accuracy
    uint8_t             clk_accuracy;
} le_meta_event_create_conn_complete_t;

/**
 * le event for connection update complete
 */
typedef struct le_meta_event_conn_update_complete
{
    uint8_t             status;         // Status of received command
    uint16_t            handle;         // Connection handle
    uint16_t            interval;       // Connection interval
    uint16_t            latency;        // Connection latency
    uint16_t            sup_timeout;    // Link supervision timeout
} le_meta_event_conn_update_complete_t;

/**
 * event for read remote features complete
 */
typedef struct le_meta_event_read_remote_feature_complete
{
    uint8_t             status;             // Range: 0x00 to 0xFF
                                            // 0x00 HCI_LE_Read_Remote_Features_Page_0 command successfully completed.
                                            // 0x01 to 0xFF HCI_LE_Read_Remote_Features_Page_0 command failed to complete. See [Vol 1]
                                            // Part F, Controller Error Codes for a list of error codes and descriptions.
    uint16_t            handle;             // Connection handle
                                            // Range: 0x0000 to 0x0EFF
    uint8_t             features[8];        // Bit Mask List of page 0 of the LE features. See [Vol 6] Part B, Section 4.6.
} le_meta_event_read_remote_feature_complete_t;

/**
 * le event for long term key request
 */
typedef struct le_meta_event_long_term_key_request
{
    uint16_t            handle;             // Connection handle Range: 0x0000 to 0x0EFF
    uint8_t             random_number[8];   // 64-bit random number.
    uint8_t             encryption_div[2];  // 16-bit encrypted diversifier.
} le_meta_event_long_term_key_request_t;

/**
 *  le event for remote connection parameter request
 */
typedef struct le_meta_event_remote_conn_param_request
{
    uint16_t            handle;             // Connection handle
    uint16_t            interval_min;       // Minimum value of the connection interval requested by the remote device.
                                            // Range: 0x0006 to 0x0C80
                                            // Time = N × 1.25 ms
                                            // Time Range: 7.5 ms to 4 s
    uint16_t            interval_max;       // Maximum value of the connection interval requested by the remote device.
                                            // Range: 0x0006 to 0x0C80
                                            // Time = N × 1.25 ms
                                            // Time Range: 7.5 ms to 4 s
    uint16_t            latency;            // Maximum allowed Peripheral latency for the connection specified as the number of subrated connection events requested by the remote device.
                                            // Range: 0x0000 to 0x01F3 (499)
    uint16_t            timeout;            // Supervision timeout for the connection requested by the remote device.
                                            // Range: 0x000A to 0x0C80
                                            // Time = N × 10 ms
                                            // Time Range: 100 ms to 32 s
} le_meta_event_remote_conn_param_request_t;

/**
 * The HCI_LE_Data_Length_Change event notifies the Host of a change to either
 * the maximum LL Data PDU Payload length or the maximum transmission time of
 * packets containing LL Data PDUs in either direction. The values reported are the limits
 * imposed on the connection by the Link Layer following the change (see [Vol 6] Part
 * B, Section 4.5.10); the actual maximum used on the connection may be less for other
 * reasons. This event shall not be generated if the values have not changed.
 */
typedef struct le_meta_event_data_length_changed
{
    uint16_t            handle;             // Connection handle Range 0x0000 to 0x0EFF
    uint16_t            max_tx_octets;      // The maximum number of payload octets in a LLData PDU that the local Controller will send
                                            // on this connection (connEffectiveMaxTxOctets defined in [Vol 6] Part B, Section 4.5.10).
                                            // Range 0x001B to 0x00FB
    uint16_t            mx_tx_time;         // The maximum time that the local Controller will take to send a Link Layer packet containing
                                            // an LL Data PDU on this connection (connEffectiveMaxTxTime defined in [Vol 6] Part B,
                                            // Section 4.5.10).
                                            // Range 0x0148 to 0x4290
    uint16_t            max_rx_octets;      // The maximum number of payload octets in a Link Layer packet that the local Controller
                                            // expects to receive on this connection (connEffectiveMaxRxOctets defined in [Vol 6] Part B,
                                            // Section 4.5.10).
                                            // Range 0x001B to 0x00FB
    uint16_t            mx_rx_time;         // The maximum time that the local Controller expects to take to receive a Link Layer packet
                                            // on this connection (connEffectiveMaxRxTime defined in [Vol 6] Part B, Section 4.5.10).
                                            // Range 0x0148 to 0x4290
} le_meta_event_data_length_changed_t;

/**
 * The HCI_LE_Directed_Advertising_Report event indicates that directed advertisements
 * have been received where the advertiser is using a resolvable private address
 * for the TargetA field of the advertising PDU which the Controller is unable
 * to resolve and the Scanning_Filter_Policy is equal to 0x02 or 0x03, see
 * Section 7.8.10. Direct_Address_Type and Direct_Address specify the address the
 * directed advertisements are being directed to. Address_Type and Address specify the
 * address of the advertiser sending the directed advertisements. The Controller may
 * queue these advertising reports and send information from multiple advertisers in one
 * HCI_LE_Directed_Advertising_Report event
 */
typedef struct le_directed_adv_report
{
    uint16_t        evt_type;           // 0x01 Connectable directed legacy advertising (ADV_DIRECT_IND)
                                        // All other values Reserved for future use
    bd_addr_type_t  addr_type;          // 0x00 Public Device Address (default)
                                        // 0x01 Random Device Address
                                        // 0x02 Public Identity Address (Corresponds to a resolved RPA)
                                        // 0x03 Random (static) Identity Address (Corresponds to a resolved RPA)
                                        // All other values Reserved for future use
    bd_addr_t       address;            // Public Device Address, Random Device Address, Public Identity Address or
                                        // Random (static) Identity Address of the advertising device.
    bd_addr_type_t  direct_addr_type;   // 0x01 Random Device Address (default)
                                        // All other values Reserved for future use
    bd_addr_t       direct_addr;        // Random Device Address
    int8_t          rssi;               // Range: -127 to +20
                                        // Units: dBm
                                        // 0xFF: RSSI is not available
} le_directed_adv_report_t;

/**
 * le event for directed advertising report
 */
typedef struct le_meta_event_directed_adv_report
{
    uint8_t                  num_of_reports;   // Number of reports in this event, always 1
    le_directed_adv_report_t reports[1];       // Array of directed advertising reports, always contains one report
} le_meta_event_directed_adv_report_t;

/**
 * le event for enhanced create connection complete
 * This event is used to report the result of an enhanced connection creation.
 */
typedef struct le_meta_event_enh_create_conn_complete
{
    //Status of received command
    uint8_t             status;
    //Connection handle
    uint16_t            handle;
    //Device role - 0=Master/ 1=Slave
    uint8_t             role;
    //Peer address type - 0=public/1=random
    bd_addr_type_t      peer_addr_type;
    //Peer address
    bd_addr_t           peer_addr;
    //Local_Resolvable_Private_Address
    bd_addr_t           local_resolv_priv_addr;
    //Peer_Resolvable_Private_Address
    bd_addr_t           peer_resolv_priv_addr;
    //Connection interval
    uint16_t            interval;
    //Connection latency
    uint16_t            latency;
    //Link supervision timeout
    uint16_t            sup_timeout;
    //Master clock accuracy
    uint8_t             clk_accuracy;
} le_meta_event_enh_create_conn_complete_t;

/**
 * le event for enhanced create connection complete v2
 * This event is used to report the result of an enhanced connection creation with additional fields.
 */
typedef struct le_meta_event_enh_create_conn_complete_v2
{
    //Status of received command
    uint8_t             status;
    //Connection handle
    uint16_t            handle;
    //Device role - 0=Master/ 1=Slave
    uint8_t             role;
    //Peer address type - 0=public/1=random
    bd_addr_type_t      peer_addr_type;
    //Peer address
    bd_addr_t           peer_addr;
    //Local_Resolvable_Private_Address
    bd_addr_t           local_resolv_priv_addr;
    //Peer_Resolvable_Private_Address
    bd_addr_t           peer_resolv_priv_addr;
    //Connection interval
    uint16_t            interval;
    //Connection latency
    uint16_t            latency;
    //Link supervision timeout
    uint16_t            sup_timeout;
    //Master clock accuracy
    uint8_t             clk_accuracy;
    //the advertising set
    uint8_t             adv_handle;
    //the periodic advertising train
    uint16_t            sync_Handle;
} le_meta_event_enh_create_conn_complete_v2_t;

// LE PHY Update Complete Event
/**
 * The HCI_LE_PHY_Update_Complete event is used to indicate that the Controller has
 * changed the transmitter PHY or receiver PHY in use.
 */
typedef struct le_meta_event_phy_update_complete
{
    uint8_t             status;     // 0x00 HCI_LE_Set_PHY command succeeded or autonomous PHY update made by the Controller.
                                    // 0x01 to 0xFF HCI_LE_Set_PHY command failed. See [Vol 1] Part F for a list of error codes and
                                    // descriptions.
    uint16_t            handle;     // Connection_Handle Range: 0x0000 to 0x0EFF
    phy_type_t          tx_phy;     // The transmitter PHY for the connection
    phy_type_t          rx_phy;     // The receiver PHY for the connection
} le_meta_event_phy_update_complete_t;

// evt_type
#define HCI_EXT_ADV_PROP_CONNECTABLE    (1 << 0)
#define HCI_EXT_ADV_PROP_SCANNABLE      (1 << 1)
#define HCI_EXT_ADV_PROP_DIRECTED       (1 << 2)
#define HCI_EXT_ADV_PROP_SCAN_RSP       (1 << 3)
#define HCI_EXT_ADV_PROP_USE_LEGACY     (1 << 4)
#define HCI_EXT_ADV_DATA_CML        (0 << 5)
#define HCI_EXT_ADV_DATA_HAS_MORE   (1 << 5)
#define HCI_EXT_ADV_DATA_TRUNCED    (2 << 5)
#define HCI_EXT_ADV_DATA_MASK       (3 << 5)

/**
 * le event for extended advertising report
 */
typedef struct le_ext_adv_report
{
    uint16_t        evt_type;           // Event type, e.g., connectable directed advertising
    bd_addr_type_t  addr_type;          // Address type of the advertiser, 0=public/1=random
    bd_addr_t       address;            // Address of the advertiser
    uint8_t         p_phy;              // primary phy
    uint8_t         s_phy;              // secondary phy
    uint8_t         sid;                // advertising Set ID
     int8_t         tx_power;           // Transmit power level in dBm
     int8_t         rssi;               // Received Signal Strength Indicator (RSSI) in dBm
    uint16_t        prd_adv_interval;   // Periodic advertising interval in units of 1.25 ms
    bd_addr_type_t  direct_addr_type;   // Address type of the directed advertiser, 0=public/1=random
    bd_addr_t       direct_addr;        // Address of the directed advertiser
    uint8_t         data_len;           // Length of the advertising data
#ifdef __ICCARM__
    uint8_t         data[1];
#else
    uint8_t         data[0];
#endif
} le_ext_adv_report_t;

/**
 * le event for extended advertising report
 */
typedef struct le_meta_event_ext_adv_report
{
    uint8_t             num_of_reports;         // this is always 1
    le_ext_adv_report_t reports[1];             // array of extended advertising reports, always contains one report
} le_meta_event_ext_adv_report_t;

/**
 * le event for periodic advertising sync established
 */
typedef struct le_meta_event_periodic_adv_sync_established
{
    uint8_t             status;       // 0x00 Periodic advertising sync successful
                                      // 0x01 to 0xFF Periodic advertising sync failed.
                                      // See [Vol 1] Part F, Controller Error Codes for a list of
                                      // error codes and descriptions
    uint16_t            handle;       // Connection handle
    uint8_t             sid;          // 0x00 to 0x0F Value of the Advertising SID subfield in the ADI field of the PDU
    bd_addr_type_t      addr_type;    // Address type of the advertiser
    bd_addr_t           address;      // Device Address
    phy_type_t          phy;          // PHY type used for periodic advertising, e.g., 1M, 2M, Coded
    uint16_t            interval;     // Periodic advertising interval
                                      // Range: 0x0006 to 0xFFFF
                                      // Time = N × 1.25 ms
                                      // Time Range: 7.5 ms to 81.91875 s
    uint8_t             clk_accuracy; // Clock accuracy of the periodic advertising train in ppm (parts per million)
                                      // see `le_clock_accuracy_t`
} le_meta_event_periodic_adv_sync_established_t;

/**
 * le event for periodic advertising sync established v2
 */
typedef struct le_meta_event_periodic_adv_sync_established_v2
{
    uint8_t             status;             // Status of received command
    uint16_t            handle;             // Connection handle
    uint8_t             sid;                // 0x00 to 0x0F Value of the Advertising SID used to advertise the periodic advertising
    bd_addr_type_t      addr_type;          // Address type of the advertiser, 0=public/1=random
    bd_addr_t           address;            // Address of the advertiser
    phy_type_t          phy;                // PHY type used for periodic advertising, e.g., 1M, 2M, Coded
    uint16_t            interval;           // Periodic advertising interval in units of 1.25 ms
                                            // Range: 0x0006 to 0xFFFF
                                            // Time = N × 1.25 ms
                                            // Time Range: 7.5 ms to 81.91875 s
    uint8_t             clk_accuracy;       // Clock accuracy of the periodic advertising train in ppm (parts per million)
                                            // see `le_clock_accuracy_t`
    // [V2] additional fields
    uint8_t             num_subevents;      // Number of subevents. Range: 0x01 to 0x80
    uint8_t             subevent_interval;  // Subevent interval.
                                            // Range: 0x06 to 0xFF
                                            // Time = N × 1.25 ms
                                            // Time Range: 7.5 ms to 318.75 ms
    uint8_t             rsp_slot_delay;     // Response slot delay.
                                            // Range: 0x01 to 0xFE
                                            // Time = N × 1.25 ms
                                            // Time Range: 1.25 ms to 317.5 ms
    uint8_t             rsp_slot_spacing;   // Response slot spacing
                                            // Range: 0x02 to 0xFF
                                            // Time = N × 0.125 ms
                                            // Time Range: 0.25 ms to 31.875 ms
} le_meta_event_periodic_adv_sync_established_v2_t;

// Data status for periodic advertising data
#define HCI_PRD_ADV_DATA_STATUS_CML        (0)
#define HCI_PRD_ADV_DATA_STATUS_HAS_MORE   (1)
#define HCI_PRD_ADV_DATA_STATUS_TRUNCED    (2)

// LE Periodic Advertising Report Event
/**
 * le event for periodic advertising report
 */
typedef struct le_meta_event_periodic_adv_report
{
    uint16_t            handle;             // Used to identify a periodic advertising train Range: 0x00 to 0xEF
    int8_t              tx_power;           // Range: -127 to +20
                                            // Units: dBm
                                            // 0x7F Tx Power information not available
    int8_t              rssi;               // Range: -127 to +20
                                            // Units: dBm
                                            // 0x7F RSSI is not available
    uint8_t             cte_type;           // 0x00 AoA Constant Tone Extension
                                            // 0x01 AoD Constant Tone Extension with 1 µs slots
                                            // 0x02 AoD Constant Tone Extension with 2 µs slots
                                            // 0xFF No Constant Tone Extension
                                            // All other values Reserved for future use
    uint8_t             data_status;        // 0x00 Data complete
                                            // 0x01 Data incomplete, more data to come
                                            // 0xFF Failed to receive or listen for an AUX_SYNC_SUBEVENT_RSP PDU
                                            // All other values Reserved for future use
    uint8_t             data_length;        // Length of the Data field
    uint8_t             data[0];            // Periodic advertising response data formatted as defined in [Vol 3] Part C, Section 11.
                                            // Note: Each element of this array has a variable length.
} le_meta_event_periodic_adv_report_t;

/**
 * le event for periodic advertising report v2
 */
typedef struct le_meta_event_periodic_adv_report_v2
{
    uint16_t            handle;                 // Used to identify a periodic advertising train Range: 0x00 to 0xEF
    int8_t              tx_power;               // Range: -127 to +20
                                                // Units: dBm
                                                // 0x7F Tx Power information not available
    int8_t              rssi;                   // Range: -127 to +20
                                                // Units: dBm
                                                // 0x7F RSSI is not available
    uint8_t             cte_type;               // 0x00 AoA Constant Tone Extension
                                                // 0x01 AoD Constant Tone Extension with 1 µs slots
                                                // 0x02 AoD Constant Tone Extension with 2 µs slots
                                                // 0xFF No Constant Tone Extension
                                                // All other values Reserved for future use
    uint16_t            periodic_event_counter; // The value of paEventCounter (see [Vol 6] Part B, Section 4.4.2.1) for the reported periodic
                                                // advertising packet
    uint8_t             subevent;               // 0xXX The subevent number.
                                                // Range: 0x00 to 0x7F
                                                // 0xFF No subevents
    uint8_t             data_status;            // 0x00 Data complete
                                                // 0x01 Data incomplete, more data to come
                                                // 0x02 Data incomplete, data truncated, no more to come
                                                // 0xFF Failed to receive an AUX_SYNC_SUBEVENT_IND PDU
                                                // All other values Reserved for future use
    uint8_t             data_length;            // Length of the Data field
    uint8_t             data[0];                // Data received from a Periodic Advertising packet
} le_meta_event_periodic_adv_report_v2_t;

/**
 * le event for periodic advertising sync lost
 */
typedef struct le_meta_event_periodic_adv_sync_lost
{
    uint16_t            handle;     // Sync_Handle identifying the periodic advertising train.
                                    // Range: 0x0000 to 0x0EFF
} le_meta_event_periodic_adv_sync_lost_t;

// LE Scan Timeout Event
// {}

/**
 * le event for advertising set terminated
 */
typedef struct le_meta_event_adv_set_terminated
{
    uint8_t  status;                // 0x00 Advertising successfully ended with a connection being created
                                    // 0x01 to
                                    // 0xFF Advertising ended for another reason. See [Vol 1] Part F, Controller Error Codes for a
                                    // list of error codes and descriptions
    uint8_t  adv_handle;            // Advertising_Handle in which advertising has ended Range: 0x00 to 0xEF
    uint16_t conn_handle;           // Connection_Handle of the connection whose creation ended the advertising
                                    // Range: 0x0000 to 0x0EFF
    uint8_t  num_events;            // Number of completed extended advertising events transmitted by the Controller
} le_meta_event_adv_set_terminated_t;

//  LE Scan Request Received Event
/**
 * le event for scan request received
 */
typedef struct le_meta_event_scan_req_received
{
    uint8_t        adv_handle;          // Used to identify an advertising set Range: 0x00 to 0xEF
    bd_addr_type_t scanner_addr_type;   // Address type of the scanner, 0=public/1=random
    bd_addr_t      scanner_addr;        // Public Device Address, Random Device Address, Public Identity Address or
                                        // Random (static) Identity Address of the advertising device
} le_meta_event_scan_req_received_t;

/**
 * le channel selection algorithm
 */
typedef enum ble_ch_sel_algo
{
    BLE_ALGO_1,         // LE Channel Selection Algorithm #1 is used
    BLE_ALGO_2,         // LE Channel Selection Algorithm #2 is used
    BLE_ALGO_NUMBER     // Number of channel selection algorithms
} ble_ch_sel_algo_t;

/**
 * le event for channel selection algorithm
 */
typedef struct le_meta_event_ch_sel_algo
{
    uint16_t conn_handle;       // Connection_Handle
                                // Range: 0x0000 to 0x0EFF
    ble_ch_sel_algo_t algo;     // Channel selection algorithm used for the connection
} le_meta_event_ch_sel_algo_t;

/**
 * le event for connectionless IQ report
 */
typedef struct le_iq_sample
{
    int8_t i;   // In-phase component of the IQ sample
    int8_t q;   // Quadrature component of the IQ sample
} le_iq_sample_t;

/**
 * le event for connectionless IQ report
 * This event is used to report IQ samples for connectionless CTE reception.
 */
typedef struct le_meta_event_connless_iq_report
{
    uint16_t sync_handle;       // Sync_Handle identifying the periodic advertising train.
                                // Range: 0x0000 to 0x0EFF
                                // 0x0FFF Receiver Test
    uint8_t  channel_index;     // 0x00 to 0x27 The index of the channel on which the packet was received.
                                // Note: 0x25 to 0x27 can be used only for packets generated during test modes.
                                // All other values Reserved for future use
    int16_t  rssi;              // RSSI of the packet
                                // Range: -1270 to +200
                                // Units: 0.1 dBm
    uint8_t  rssi_ant_id;       // Antenna ID for the RSSI measurement
    uint8_t  cte_type;          // 0x00 AoA Constant Tone Extension
                                // 0x01 AoD Constant Tone Extension with 1 µs slots
                                // 0x02 AoD Constant Tone Extension with 2 µs slots
                                // All other values Reserved for future use
    uint8_t  slot_durations;    // 0x01 Switching and sampling slots are 1 µs each
                                // 0x02 Switching and sampling slots are 2 µs each
                                // All other values Reserved for future use
    uint8_t  packet_status;     // 0x00 CRC was correct
                                // 0x01 CRC was incorrect and the Length and CTETime fields of the packet were used to
                                // determine sampling points
                                // 0x02 CRC was incorrect but the Controller has determined the position and length of the
                                // Constant Tone Extension in some other way
                                // 0xFF Insufficient resources to sample (Channel_Index, CTE_Type, and Slot_Durations invalid).
                                // All other values
                                // Reserved for future use
    uint16_t event_counter;     // The value of paEventCounter (see [Vol 6] Part B, Section 4.4.2.1) for the reported
                                // AUX_SYNC_IND PDU
    uint8_t  sample_count;      // 0x00 No samples provided (only permitted if Packet_Status is 0xFF).
                                // 0x09 to 0x52 Total number of sample pairs (there shall be the same number of I samples and Q
                                // samples).
                                // Note: This number is dependent on the switch and sample slot durations used.
                                // All other values Reserved for future use
    le_iq_sample_t samples[0];  // Array of IQ samples
} le_meta_event_connless_iq_report_t;

/**
 * le event for connectionless IQ report
 * This event is used to report IQ samples for connectionless CTE reception.
 */
typedef struct le_meta_event_pro_connless_iq_report
{
    bd_addr_type_t addr_type;   // Address type of the advertiser, 0=public/1=random
    bd_addr_t  addr;            // Address of the advertiser
    phy_type_t rx_phy;          // PHY type used for reception, e.g., 1M, 2M, Coded
    uint8_t  channel_index;     // Channel index for the IQ samples
    int16_t  rssi;              // Received Signal Strength Indicator (RSSI) in dBm
    uint8_t  rssi_ant_id;       // Antenna ID for the RSSI measurement
    uint8_t  cte_type;          // 0x00 AoA Constant Tone Extension
                                // 0x01 AoD Constant Tone Extension with 1 µs slots
                                // 0x02 AoD Constant Tone Extension with 2 µs slots
                                // All other values Reserved for future use
    uint8_t  slot_durations;    // 0x01 Switching and sampling slots are 1 µs each
                                // 0x02 Switching and sampling slots are 2 µs each
                                // All other values Reserved for future use
    uint8_t  packet_status;     // 0x00 CRC was correct
                                // 0x01 CRC was incorrect and the Length and CTETime fields of the packet were used to
                                // determine sampling points
                                // 0x02 CRC was incorrect but the Controller has determined the position and length of the
                                // Constant Tone Extension in some other way
                                // 0xFF Insufficient resources to sample (Channel_Index, CTE_Type, and Slot_Durations invalid).
                                // All other values
                                // Reserved for future use
    uint8_t  sample_count;      // 0x00 No samples provided (only permitted if Packet_Status is 0xFF).
                                // 0x09 to 0x52 Total number of sample pairs (there shall be the same number of I samples and Q
                                // samples).
                                // Note: This number is dependent on the switch and sample slot durations used.
                                // All other values Reserved for future use
    le_iq_sample_t samples[0];  // Array of IQ samples
} le_meta_event_pro_connless_iq_report_t;

/**
 * le event for connection IQ report
 */
typedef struct le_meta_event_conn_iq_report
{
    uint16_t conn_handle;       // Connection handle
    phy_type_t rx_phy;          // PHY type used for reception, e.g., 1M, 2M, Coded
    uint8_t  channel_index;     // Channel index for the IQ samples
    int16_t  rssi;              // Received Signal Strength Indicator (RSSI) in dBm
    uint8_t  rssi_ant_id;       // Antenna ID for the RSSI measurement
    uint8_t  cte_type;          // 0x00 AoA Constant Tone Extension
                                // 0x01 AoD Constant Tone Extension with 1 µs slots
                                // 0x02 AoD Constant Tone Extension with 2 µs slots
                                // All other values Reserved for future use
    uint8_t  slot_durations;    // 0x01 Switching and sampling slots are 1 µs each
                                // 0x02 Switching and sampling slots are 2 µs each
                                // All other values Reserved for future use
    uint8_t  packet_status;     // 0x00 CRC was correct
                                // 0x01 CRC was incorrect and the Length and CTETime fields of the packet were used to
                                // determine sampling points
                                // 0x02 CRC was incorrect but the Controller has determined the position and length of the
                                // Constant Tone Extension in some other way
                                // 0xFF Insufficient resources to sample (Channel_Index, CTE_Type, and Slot_Durations invalid).
                                // All other values
                                // Reserved for future use
    uint16_t event_counter;     // The value of connEventCounter (see [Vol 6] Part B, Section 4.5.1) for the reported PDU
    uint8_t  sample_count;      // 0x00 No samples provided (only permitted if Packet_Status is 0xFF).
                                // 0x09 to 0x52 Total number of sample pairs (there shall be the same number of I samples and Q
                                // samples).
                                // Note: This number is dependent on the switch and sample slot durations used.
                                // All other values Reserved for future use
    le_iq_sample_t samples[0];  // Array of IQ samples
} le_meta_event_conn_iq_report_t;

/**
 * le event for CTE request failed
 */
typedef struct le_meta_event_cte_req_failed
{
    uint8_t  status;                // 0x00 LL_CTE_RSP PDU received successfully but without a Constant Tone Extension field.
                                    // 0x01 to 0xFF
                                    // Peer rejected the request. See [Vol 1] Part F, Controller Error Codes for a list of error
                                    // codes and descriptions
    uint16_t conn_handle;           // Connection_Handle
                                    // Range 0x0000 to 0x0EFF
} le_meta_event_cte_req_failed_t;

/**
 * le event for periodic advertising sync transfer received
 * This event is used to report the reception of a periodic advertising sync transfer.
 */
typedef struct le_meta_event_prd_adv_sync_transfer_recv
{
    uint8_t  status;                // 0x00 Synchronization to the periodic advertising train succeeded.
                                    // 0x01 to 0xFF
                                    // Synchronization to the periodic advertising train failed. See [Vol 1] Part F, Controller
                                    // Error Codes for a list of error codes and descriptions.
    uint16_t conn_handle;           // Connection_Handle Range: 0x0000 to 0x0EFF
    uint16_t service_data;          // A value provided by the peer device
    uint16_t sync_handle;           // Sync_Handle identifying the periodic advertising train.
                                    // Range: 0x0000 to 0x0EFF
    uint8_t  adv_sid;               // 0x00 to 0x0F Value of the Advertising SID used to advertise the periodic advertising
                                    // All other values Reserved for future use
    bd_addr_type_t  addr_type;      // Address type of the advertiser, 0=public/1=random
    bd_addr_t  addr;                // Public Device Address, Random Device Address, Public Identity Address, or
                                    // Random (static) Identity Address of the advertiser
    phy_type_t phy;                 // PHY type used for periodic advertising, e.g., 1M, 2M, Coded
    uint16_t   prd_adv_interval;    // Periodic advertising interval
                                    // Range: 0x0006 to 0xFFFF
                                    // Time = N × 1.25 ms
                                    // Time Range: 7.5 ms to 81.91875 s
    uint8_t    clk_acc;             // see `le_clock_accuracy_t`
} le_meta_event_prd_adv_sync_transfer_recv_t;

/**
 * le event for periodic advertising sync transfer received v2
 */
typedef struct le_meta_event_prd_adv_sync_transfer_recv_v2
{
    uint8_t  status;                        // 0x00 Synchronization to the periodic advertising train succeeded.
                                            // 0x01 to 0xFF
                                            // Synchronization to the periodic advertising train failed. See [Vol 1] Part F, Controller
                                            // Error Codes for a list of error codes and descriptions.
    uint16_t conn_handle;                   // Connection_Handle Range: 0x0000 to 0x0EFF
    uint16_t service_data;                  // A value provided by the peer device
    uint16_t sync_handle;                   // Sync_Handle identifying the periodic advertising train.
                                            // Range: 0x0000 to 0x0EFF
    uint8_t  adv_sid;                       // 0x00 to 0x0F Value of the Advertising SID used to advertise the periodic advertising
                                            // All other values Reserved for future use
    bd_addr_type_t  addr_type;              // Address type of the advertiser, 0=public/1=random
    bd_addr_t  addr;                        // Public Device Address, Random Device Address, Public Identity Address, or
                                            // Random (static) Identity Address of the advertiser
    phy_type_t phy;                         // PHY type used for periodic advertising, e.g., 1M, 2M, Coded
    uint16_t   prd_adv_interval;            // Periodic advertising interval
                                            // Range: 0x0006 to 0xFFFF
                                            // Time = N × 1.25 ms
                                            // Time Range: 7.5 ms to 81.91875 s
    uint8_t    clk_acc;                     // see `le_clock_accuracy_t`
    // [V2] additional fields
    uint8_t             num_subevents;      // 0x00 No subevents
                                            // N=0xXX Number of subevents.
                                            // Range: 0x01 to 0x80
    uint8_t             subevent_interval;  // Subevent interval.
                                            // Range: 0x06 to 0xFF
                                            // Time = N × 1.25 ms
                                            // Time Range: 7.5 ms to 318.75 ms
    uint8_t             rsp_slot_delay;     // Response slot delay.
                                            // Range: 0x01 to 0xFE
                                            // Time = N × 1.25 ms
                                            // Time Range: 1.25 ms to 317.5 ms
    uint8_t             rsp_slot_spacing;   // Response slot spacing
                                            // Range: 0x02 to 0xFF
                                            // Time = N × 0.125 ms
                                            // Time Range: 0.25 ms to 31.875 ms
} le_meta_event_prd_adv_sync_transfer_recv_v2_t;

/**
 * le clock accuracy
 * This enum defines the clock accuracy levels for Bluetooth Low Energy connections.
 */
typedef enum le_clock_accuracy
{
    LE_CLOCK_ACCURACY_500_PPM = 0,              // 500 ppm
    LE_CLOCK_ACCURACY_250_PPM,                  // 250 ppm
    LE_CLOCK_ACCURACY_150_PPM,                  // 150 ppm
    LE_CLOCK_ACCURACY_100_PPM,                  // 100 ppm
    LE_CLOCK_ACCURACY_75_PPM,                   // 75 ppm
    LE_CLOCK_ACCURACY_50_PPM,                   // 50 ppm
    LE_CLOCK_ACCURACY_30_PPM,                   // 30 ppm
    LE_CLOCK_ACCURACY_20_PPM,                   // 20 ppm
} le_clock_accuracy_t;

/**
 * le event for request peer SCA complete
 */
typedef struct le_meta_event_request_peer_sca_complete
{
    uint8_t  status;                            // 0x00 The Peer_Clock_Accuracy parameter is successfully received.
                                                // 0x01 to 0xFF
                                                // The reception of Peer_Clock_Accuracy parameter failed. See [Vol 1] Part F, Controller
                                                // Error Codes for a list of error codes and descriptions
    uint16_t conn_handle;                       // Connection handle of the ACL
                                                // Range: 0x0000 to 0x0EFF
    le_clock_accuracy_t peer_clock_accuracy;    // Peer clock accuracy, indicating the accuracy of the peer's clock
} le_meta_event_request_peer_sca_complete_t;

/**
 * le event for path loss threshold
 */
typedef enum le_path_loss_zone_event
{
    PATH_LOSS_ZONE_ENTER_LOW = 0,               // Entered low path loss zone
    PATH_LOSS_ZONE_ENTER_MIDDLE = 1,            // Entered middle path loss zone
    PATH_LOSS_ZONE_ENTER_HIGH = 2,              // Entered high path loss zone
} le_path_loss_zone_event_t;

/**
 * le event for path loss threshold
 */
typedef struct le_meta_event_path_loss_threshold
{
    uint16_t conn_handle;                       // Connection_Handle
                                                // Range: 0x0000 to 0x0EFF
    uint8_t  current_path_loss;                 // 0xXX Current path loss (always zero or positive)
                                                // Units: dB
                                                // 0xFF Unavailable
    le_path_loss_zone_event_t  zone_entered;    // Zone entered based on the current path loss
} le_meta_event_path_loss_threshold_t;

/**
 * le event for transmit power reporting
 * This event is used to report changes in the transmit power level.
 */
typedef enum le_tx_power_reporting_reason
{
    TX_POWER_REPORTING_REASON_LOCAL_CHANGED = 0,    // Local device changed its transmit power level
    TX_POWER_REPORTING_REASON_REMOTE_CHANGED = 1,   // Remote device changed its transmit power level
    TX_POWER_REPORTING_REASON_HCI_COMPLETE = 2,     // HCI command completed, reporting the transmit power level
} le_tx_power_reporting_reason_t;

/**
 * le event for transmit power reporting
 */
typedef struct le_meta_event_tx_power_reporting
{
    uint8_t  status;                        // Status of received command, 0 for success, non-zero for failure
    uint16_t conn_handle;                   // Connection handle for which the transmit power level is reported
    le_tx_power_reporting_reason_t reason;  // Reason for the transmit power reporting event
    unified_phy_type_t phy;                 // PHY type for which the transmit power level is reported
    int8_t  tx_power_level;                 // Tx power level in dBm
    uint8_t tx_power_level_flag;            // Bit 0: Transmit power level is at minimum level
                                            // Bit 1: Transmit power level is at maximum level
    int8_t  delta;                          // Change in transmit power level (positive indicates increased power,
                                            // negative indicates decreased power, zero indicates unchanged)
                                            // Units: dB
                                            // 0x7F: Change is not available or is out of range
} le_meta_event_tx_power_reporting_t;

/**
 * le event for subrate change
 * This event is used to report changes in the subrate factor and related parameters for a connection.
 */
typedef struct le_meta_subrate_change
{
    uint8_t  status;                // Status of received command, 0 for success, non-zero for failure
    uint16_t conn_handle;           // Connection handle for which the subrate change is reported
    uint16_t subrate_factor;        // New subrate factor applied to the specified underlying connection interval
                                    // Range 0x0001 to 0x01F4
    uint16_t peripheral_latency;    // New Peripheral latency for the connection in number of subrated connection events
                                    // Range: 0x0000 to 0x01F3
    uint16_t continuation_number;   // Number of underlying connection events to remain active after a
                                    // packet containing a Link Layer PDU with a non-zero Length field is
                                    // sent or received
                                    // Range: 0x0000 to 0x01F3
    uint16_t supervision_timeout;   // New supervision timeout for this connection.
                                    // Range: 0x000A to 0x0C80
                                    // Time = N x 10 ms
                                    // Time Range: 100 ms to 32 s
} le_meta_subrate_change_t;

/**
 * le event for vendor channel map update
 */
typedef struct le_meta_event_vendor_channel_map_update
{
    // connection handle
    uint16_t conn_handle;
    // current channel map (the lower 37 bits are used)
    // channel `n` is identified by bit `(channel_map & 0x7)` of `channel_map[n / 8]`
    uint8_t  channel_map[5];
} le_meta_event_vendor_channel_map_update_t;

/**
 * BLE link layer security context
 */
typedef struct le_security_ctx
{
    uint8_t opaque[40]; // Opaque security context data, used for storing security-related information
} le_security_ctx_t;

/**
 * le event for vendor connection aborted
 */
typedef struct le_meta_event_vendor_connection_aborted
{
    uint16_t conn_handle;           // connection handle
    uint8_t role;                   // role (0: master, 1: slave)
    phy_type_t rx_phy;              // Rx PHY
    phy_type_t tx_phy;              // Tx PHY
    uint32_t access_addr;           // Access address
    uint32_t crc_init;              // CRC init value
    uint32_t interval;              // interval in us
    uint16_t sup_timeout;           // supervision timeout in 10ms
    uint8_t  channel_map[5];        // channel map (lower 37 bits)
    uint8_t  ch_sel_algo;           // channel selection algorithm (0: ALG #1, 1: ALG #2)
    uint8_t  hop_inc;               // frequency hopping increment (only for ALG #1)
    uint8_t  last_unmapped_ch;      // last unmapped channel (only for ALG #1)
    uint16_t slave_latency;         // slave latency
    uint8_t  sleep_clk_acc;         // sleep clock accuracy
    uint64_t next_event_time;       // start time of next connection event
    uint16_t next_event_counter;    // event counter for next connection event
    uint8_t  security_enabled;      // is security enabled
    le_security_ctx_t security;     // security context (available when enabled)
} le_meta_event_vendor_connection_aborted_t;

/**
 * le event for periodic advertising subevent data request
 */
typedef struct le_mete_event_prd_adv_subevent_data_req
{
    uint8_t adv_handle;             // Used to identify a periodic advertising train
                                    // Range: 0x00 to 0xEF
    uint8_t subevent_start;         // The first subevent that data is requested for.
                                    // Range: 0x00 to 0x7F
    uint8_t subevent_data_count;    // The number of subevents that data is requested for.
                                    // Range: 0x01 to 0x80
} le_mete_event_prd_adv_subevent_data_req_t;

/**
 * le event for periodic advertising response report
 * This event is used to report responses received during periodic advertising.
 */
typedef struct le_prd_adv_rsp_report
{
     int8_t tx_power;       // Range: -127 to +20. Unit dBm
                            // 0x7F: Tx Power information not available
     int8_t rssi;           // Range: -127 to +20. Unit dBm
                            // 0x7F: RSSI not available
    uint8_t cte_type;       // 0x00: AoA; 0x01: AoD (1us); 0x02: AoD (2us)
                            // 0xFF: no CTE
    uint8_t response_slot;  // The response slot the data was received in.
                            // Range: 0x00 to 0xFF
    uint8_t data_status;    // 0x00: Data complete;
                            // 0x01: Data incomplete, more data to come;
                            // 0xFF: Failed to receive an AUX_SYNC_SUBEVENT_RSP PDU
    uint8_t data_length;    // Length of the Data field
#ifdef __ICCARM__
    uint8_t data[1];
#else
    uint8_t data[0];
#endif
} le_prd_adv_rsp_report_t;

/**
 * le event for periodic advertising response report
 */
typedef struct le_mete_event_prd_adv_rsp_report
{
    uint8_t adv_handle;     // Used to identify a periodic advertising train
                            // Range: 0x00 to 0xEF
    uint8_t subevent;       // The subevent number.
                            // Range: 0x00 to 0x7F
    uint8_t tx_status;      // 0x00: AUX_SYNC_SUBEVENT_IND packet was transmitted
                            // 0x01: AUX_SYNC_SUBEVENT_IND packet was not transmitted.
                            // Others: reserved
    uint8_t num_responses;  // Number of responses in event.
                            // This is always <= 1
    le_prd_adv_rsp_report_t reports[1];
} le_mete_event_prd_adv_rsp_report_t;

// for naming compatibility
#define le_meta_tx_power_reporting_t        le_meta_event_tx_power_reporting_t
#define le_meta_tx_power_reporting          le_meta_event_tx_power_reporting
#define le_meta_path_loss_threshold_t       le_meta_event_path_loss_threshold_t
#define le_meta_path_loss_threshold         le_meta_event_path_loss_threshold
#define le_meta_request_peer_sca_complete_t     le_meta_event_request_peer_sca_complete_t
#define le_meta_request_peer_sca_complete       le_meta_event_request_peer_sca_complete
#define le_meta_prd_adv_sync_transfer_recv_v2_t le_meta_event_prd_adv_sync_transfer_recv_v2_t
#define le_meta_prd_adv_sync_transfer_recv_v2   le_meta_event_prd_adv_sync_transfer_recv_v2
#define le_meta_prd_adv_sync_transfer_recv_t    le_meta_event_prd_adv_sync_transfer_recv_t
#define le_meta_prd_adv_sync_transfer_recv      le_meta_event_prd_adv_sync_transfer_recv
#define le_meta_conn_iq_report_t            le_meta_event_conn_iq_report_t
#define le_meta_conn_iq_report              le_meta_event_conn_iq_report
#define le_meta_cte_req_failed_t            le_meta_event_cte_req_failed_t
#define le_meta_cte_req_failed              le_meta_event_cte_req_failed
#define le_meta_pro_connless_iq_report_t    le_meta_event_pro_connless_iq_report_t
#define le_meta_pro_connless_iq_report      le_meta_event_pro_connless_iq_report
#define le_meta_connless_iq_report_t        le_meta_event_connless_iq_report_t
#define le_meta_connless_iq_report          le_meta_event_connless_iq_report
#define le_meta_ch_sel_algo_t               le_meta_event_ch_sel_algo_t
#define le_meta_ch_sel_algo                 le_meta_event_ch_sel_algo
#define le_meta_scan_req_received_t         le_meta_event_scan_req_received_t
#define le_meta_scan_req_received           le_meta_event_scan_req_received
#define le_meta_phy_update_complete_t       le_meta_event_phy_update_complete_t
#define le_meta_phy_update_complete         le_meta_event_phy_update_complete
#define le_meta_directed_adv_report_t       le_meta_event_directed_adv_report_t
#define le_meta_directed_adv_report         le_meta_event_directed_adv_report
#define le_mete_prd_adv_subevent_data_req_t le_mete_event_prd_adv_subevent_data_req_t
#define le_mete_prd_adv_subevent_data_req   le_mete_event_prd_adv_subevent_data_req
#define le_meta_adv_set_terminated_t        le_meta_event_adv_set_terminated_t
#define le_meta_adv_set_terminated          le_meta_event_adv_set_terminated
/**
 * btstack l2cap message definitions
 */
typedef enum btstack_l2cap_msg_def
{
    BTSTACK_L2CAP_MSG_CAN_SEND_NOW = 0, // Can send now
} btstack_l2cap_msg_def_t;

#pragma pack (pop)

// packet is from host callbacks
#define decode_hci_le_meta_event(packet, T)          decode_event_offset(packet, T, 3)

#define decode_hci_event_disconn_complete(packet)    decode_event_offset(packet, event_disconn_complete_t, 2)
#define decode_hci_event_vendor_ccm_complete(packet) decode_event_offset(packet, event_vendor_ccm_complete_t, 5)

#define decode_hci_event(packet, T)     decode_event_offset(packet, T, 2)

#define decode_l2cap_event(packet, T)   decode_event_offset(packet, T, 2)

/* API_END */

#ifdef __cplusplus
}
#endif

#endif // __BTSTACK_EVENT_H
