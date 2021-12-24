/** @file
*   @brief basic function APIs and events  for bluetooth
*  Copyright Message
*
*
*  INGCHIPS confidential and proprietary.
*  COPYRIGHT (c) 2018 by INGCHIPS
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
 * @brief post a msg to btstack task, and handle it in the context of btstack task
 * @param msg_id        (Max allowed ID: 0x80000000)
 * @param data
 * @param len
 * @return 0 if success; else error occured
 */
uint32_t btstack_push_user_msg(uint32_t msg_id, const void *data, const uint16_t len);

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
 * @return latencey
 * @note: btstack_type 2
 */
static __INLINE uint16_t l2cap_event_connection_parameter_update_request_get_latencey(const uint8_t * event){
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
 * @brief Get field local_cid from event L2CAP_EVENT_CAN_SEND_NOW
 * @param event packet
 * @return local_cid
 * @note: btstack_type 2
 */
static __INLINE uint16_t l2cap_event_can_send_now_get_local_cid(const uint8_t * event){
    return little_endian_read_16(event, 2);
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

typedef struct event_disconn_complete
{
    uint8_t status;
    uint16_t conn_handle;
    uint8_t reason;
} event_disconn_complete_t;

typedef struct
{
    uint8_t  status;        // for DEC, 0 means OK, 1 means ERROR
    uint8_t  type;          // 0: encrypt  1: decrypt
    uint8_t  mic_size;
    uint16_t msg_len;
    uint16_t aad_len;
    uint32_t tag;           // same value as in command
    uint8_t *out_msg;
} event_vendor_ccm_complete_t;

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

//  LE Connection Update Complete Event
typedef struct le_meta_event_conn_update_complete
{
    uint8_t             status;
    uint16_t            handle;
    uint16_t            interval;
    uint16_t            latency;
    uint16_t            sup_timeout;
} le_meta_event_conn_update_complete_t;

// LE Read Remote Features Complete Event
typedef struct le_meta_event_read_remote_feature_complete
{
    uint8_t             status;
    uint16_t            handle;
    uint8_t             features[8];
} le_meta_event_read_remote_feature_complete_t;

// LE Long Term Key Request Event
typedef struct le_meta_event_long_term_key_request
{
    uint16_t            handle;
    uint8_t             random_number[8];
    uint8_t             encryption_div[2];
} le_meta_event_long_term_key_request_t;

// LE Remote Connection Parameter Request Event
typedef struct le_meta_event_remote_conn_param_request
{
    uint16_t            handle;
    uint16_t            interval_min;
    uint16_t            interval_max;
    uint16_t            latency;
    uint16_t            timeout;
} le_meta_event_remote_conn_param_request_t;

// LE Data Length Change Event
typedef struct le_meta_event_data_length_changed
{
    uint16_t            handle;
    uint16_t            max_tx_octets;
    uint16_t            mx_tx_time;
    uint16_t            max_rx_octets;
    uint16_t            mx_rx_time;
} le_meta_event_data_length_changed_t;

typedef struct le_directed_adv_report
{
    uint16_t        evt_type;
    bd_addr_type_t  addr_type;
    bd_addr_t       address;
    bd_addr_type_t  direct_addr_type;
    bd_addr_t       direct_addr;
    int8_t          rssi;
} le_directed_adv_report_t;

// LE Directed Advertising Report Event
typedef struct le_meta_directed_adv_report
{
    uint8_t                  num_of_reports;
    le_directed_adv_report_t reports[1];
} le_meta_directed_adv_report_t;

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

// LE PHY Update Complete Event
typedef struct le_meta_phy_update_complete
{
    uint8_t             status;
    uint16_t            handle;
    phy_type_t          tx_phy;
    phy_type_t          rx_phy;
} le_meta_phy_update_complete_t;

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

typedef struct le_ext_adv_report
{
    uint16_t        evt_type;
    bd_addr_type_t  addr_type;
    bd_addr_t       address;
    uint8_t         p_phy;             // primary phy
    uint8_t         s_phy;             // secondary phy
    uint8_t         sid;
     int8_t         tx_power;
     int8_t         rssi;
    uint16_t        prd_adv_interval;
    bd_addr_type_t  direct_addr_type;
    bd_addr_t       direct_addr;
    uint8_t         data_len;
    uint8_t         data[0];
} le_ext_adv_report_t;

//  LE Extended Advertising Report Event
typedef struct le_meta_event_ext_adv_report
{
    uint8_t             num_of_reports;         // this is always 1
    le_ext_adv_report_t reports[1];
} le_meta_event_ext_adv_report_t;

// LE Periodic Advertising Sync Established Event
typedef struct le_meta_event_periodic_adv_sync_established
{
    uint8_t             status;
    uint16_t            handle;
    uint8_t             sid;
    bd_addr_type_t      addr_type;
    bd_addr_t           address;
    phy_type_t          phy;
    uint16_t            interval;
    uint8_t             clk_accuracy;
} le_meta_event_periodic_adv_sync_established_t;

#define HCI_PRD_ADV_DATA_STATUS_CML        (0)
#define HCI_PRD_ADV_DATA_STATUS_HAS_MORE   (1)
#define HCI_PRD_ADV_DATA_STATUS_TRUNCED    (2)

// LE Periodic Advertising Report Event
typedef struct le_meta_event_periodic_adv_report
{
    uint16_t            handle;
    int8_t              tx_power;
    int8_t              rssi;
    uint8_t             unused;
    uint8_t             data_status;
    uint8_t             data_length;
    uint8_t             data[0];
} le_meta_event_periodic_adv_report_t;

// LE Periodic Advertising Sync Lost Event
typedef struct le_meta_event_periodic_adv_sync_lost
{
    uint16_t            handle;
} le_meta_event_periodic_adv_sync_lost_t;

// LE Scan Timeout Event
// {}

// LE Advertising Set Terminated Event
typedef struct le_meta_adv_set_terminated
{
    uint8_t  status;
    uint8_t  adv_handle;
    uint16_t conn_handle;
    uint8_t  num_events;  // Num_Completed_Extended_Advertising_Events
} le_meta_adv_set_terminated_t;

//  LE Scan Request Received Event
typedef struct le_meta_scan_req_received
{
    uint8_t        adv_handle;
    bd_addr_type_t scanner_addr_type;
    bd_addr_t      scanner_addr;
} le_meta_scan_req_received_t;

typedef enum ble_ch_sel_algo
{
    BLE_ALGO_1,
    BLE_ALGO_2,
    BLE_ALGO_NUMBER
} ble_ch_sel_algo_t;

// LE Channel Selection Algorithm Event
typedef struct le_meta_ch_sel_algo
{
    uint16_t conn_handle;
    ble_ch_sel_algo_t algo;
} le_meta_ch_sel_algo;

typedef struct le_iq_sample
{
    int8_t i;
    int8_t q;
} le_iq_sample_t;

typedef struct le_meta_connless_iq_report
{
    uint16_t sync_handle;
    uint8_t  channel_index;
    int16_t  rssi;
    uint8_t  rssi_ant_id;
    uint8_t  cte_type;
    uint8_t  slot_durations;
    uint8_t  packet_status;
    uint16_t event_counter;
    uint8_t  sample_count;
    le_iq_sample_t samples[0];
} le_meta_connless_iq_report_t;

typedef struct le_meta_pro_connless_iq_report
{
    bd_addr_type_t addr_type;
    bd_addr_t  addr;
    phy_type_t rx_phy;
    uint8_t  channel_index;
    int16_t  rssi;
    uint8_t  rssi_ant_id;
    uint8_t  cte_type;
    uint8_t  slot_durations;
    uint8_t  packet_status;
    uint8_t  sample_count;
    le_iq_sample_t samples[0];
} le_meta_pro_connless_iq_report_t;

typedef struct le_meta_conn_iq_report
{
    uint16_t conn_handle;
    phy_type_t rx_phy;
    uint8_t  channel_index;
    int16_t  rssi;
    uint8_t  rssi_ant_id;
    uint8_t  cte_type;
    uint8_t  slot_durations;
    uint8_t  packet_status;
    uint16_t event_counter;
    uint8_t  sample_count;
    le_iq_sample_t samples[0];
} le_meta_conn_iq_report_t;

typedef struct le_meta_cte_req_failed
{
    uint8_t  status;
    uint16_t conn_handle;
} le_meta_cte_req_failed_t;

typedef struct le_meta_prd_adv_sync_transfer_recv
{
    uint8_t  status;
    uint16_t conn_handle;
    uint16_t service_data;
    uint16_t sync_handle;
    uint8_t  adv_sid;
    bd_addr_type_t  addr_type;
    bd_addr_t  addr;
    phy_type_t phy;
    uint16_t   prdAdvInterval;
    uint8_t    clkAcc;
} le_meta_prd_adv_sync_transfer_recv_t;

#pragma pack (pop)

// packet is from host callbacks
#define decode_hci_le_meta_event(packet, T)          decode_event_offset(packet, T, 3)

#define decode_hci_event_disconn_complete(packet)    decode_event_offset(packet, event_disconn_complete_t, 2)
#define decode_hci_event_vendor_ccm_complete(packet) decode_event_offset(packet, event_vendor_ccm_complete_t, 5)

#define decode_hci_event(packet, T) decode_event_offset(packet, T, 2)

/* API_END */

#ifdef __cplusplus
}
#endif

#endif // __BTSTACK_EVENT_H
