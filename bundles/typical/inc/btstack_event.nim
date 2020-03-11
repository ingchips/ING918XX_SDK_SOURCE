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
##  ----------------------------------------------------------------------------

import
  bluetooth, compiler, btstack_util, gap

##  API_START

template decode_event_offset*(packet, T, offset: untyped): untyped =
  (cast[ptr T]((cast[int32]((packet)) + offset)))

## *
##  @brief Get event type
##  @param event
##  @return type of event
##

proc hci_event_packet_get_type*(event: ptr uint8): uint8 =
  return event[]

## *
##   User messge
##

type
  btstack_user_msg_t* {.importc: "btstack_user_msg_t", header: "btstack_event.h",
                       bycopy.} = object
    msg_id* {.importc: "msg_id".}: uint32
    data* {.importc: "data".}: pointer
    len* {.importc: "len".}: uint16


## *
##  @brief Get user message
##  @param event
##  @return type of event
##

proc hci_event_packet_get_user_msg*(event: ptr uint8): ptr btstack_user_msg_t =
  return decode_event_offset(event, btstack_user_msg_t, 4)

## *
##  @brief post a msg to btstack task, and handle it in the context of btstack task
##  @param msg_id        (Max allowed ID: 0x80000000)
##  @param data
##  @param len
##  @return 0 if success; else error occured
##

proc btstack_push_user_msg*(msg_id: uint32; data: pointer; len: uint16): uint32 {.
    importc: "btstack_push_user_msg", header: "btstack_event.h".}
## **
##  @brief Get subevent code for le event
##  @param event packet
##  @return subevent_code
##

proc hci_event_le_meta_get_subevent_code*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 2)[]

type
  hci_encryption_change_event_t* {.importc: "hci_encryption_change_event_t",
                                  header: "btstack_event.h", bycopy.} = object
    status* {.importc: "status".}: uint8
    conn_handle* {.importc: "conn_handle".}: uint16
    enabled* {.importc: "enabled".}: uint8


## *
##  @brief Get field status from event HCI_EVENT_ENCRYPTION_CHANGE
##  @param event packet
##  @return status
##  @note: btstack_type 1
##

proc hci_event_encryption_change_get_status*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 2)[]

## *
##  @brief Get field connection_handle from event HCI_EVENT_ENCRYPTION_CHANGE
##  @param event packet
##  @return connection_handle
##  @note: btstack_type 2
##

proc hci_event_encryption_change_get_connection_handle*(event: ptr uint8): uint16 =
  return little_endian_read_16(event, 3)

## *
##  @brief Get field encryption_enabled from event HCI_EVENT_ENCRYPTION_CHANGE
##  @param event packet
##  @return encryption_enabled
##  @note: btstack_type 1
##

proc hci_event_encryption_change_get_encryption_enabled*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 5)[]

## *
##  @brief Get field num_hci_command_packets from event HCI_EVENT_COMMAND_COMPLETE
##  @param event packet
##  @return num_hci_command_packets
##  @note: btstack_type 1
##

proc hci_event_command_complete_get_num_hci_command_packets*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 2)[]

## *
##  @brief Get field command_opcode from event HCI_EVENT_COMMAND_COMPLETE
##  @param event packet
##  @return command_opcode
##  @note: btstack_type 2
##

proc hci_event_command_complete_get_command_opcode*(event: ptr uint8): uint16 =
  return little_endian_read_16(event, 3)

## *
##  @brief Get field return_parameters from event HCI_EVENT_COMMAND_COMPLETE
##  @param event packet
##  @return return_parameters
##  @note: btstack_type R
##

proc hci_event_command_complete_get_return_parameters*(event: ptr uint8): ptr uint8 =
  return decode_event_offset(event, uint8, 5)

## *
##  @brief Get field status from event HCI_EVENT_COMMAND_STATUS
##  @param event packet
##  @return status
##  @note: btstack_type 1
##

proc hci_event_command_status_get_status*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 2)[]

## *
##  @brief Get field num_hci_command_packets from event HCI_EVENT_COMMAND_STATUS
##  @param event packet
##  @return num_hci_command_packets
##  @note: btstack_type 1
##

proc hci_event_command_status_get_num_hci_command_packets*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 3)[]

## *
##  @brief Get field command_opcode from event HCI_EVENT_COMMAND_STATUS
##  @param event packet
##  @return command_opcode
##  @note: btstack_type 2
##

proc hci_event_command_status_get_command_opcode*(event: ptr uint8): uint16 =
  return little_endian_read_16(event, 4)

## *
##  @brief Get field hardware_code from event HCI_EVENT_HARDWARE_ERROR
##  @param event packet
##  @return hardware_code
##  @note: btstack_type 1
##

proc hci_event_hardware_error_get_hardware_code*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 2)[]

## *
##  @brief Get field status from event HCI_EVENT_ENCRYPTION_KEY_REFRESH_COMPLETE
##  @param event packet
##  @return status
##  @note: btstack_type 1
##

proc hci_event_encryption_key_refresh_complete_get_status*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 2)[]

## *
##  @brief Get field handle from event HCI_EVENT_ENCRYPTION_KEY_REFRESH_COMPLETE
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc hci_event_encryption_key_refresh_complete_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 3)

## *
##  @brief Get field state from event BTSTACK_EVENT_STATE
##  @param event packet
##  @return state
##  @note: btstack_type 1
##

proc btstack_event_state_get_state*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 2)[]

## *
##  @brief Get field discoverable from event BTSTACK_EVENT_DISCOVERABLE_ENABLED
##  @param event packet
##  @return discoverable
##  @note: btstack_type 1
##

proc btstack_event_discoverable_enabled_get_discoverable*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 2)[]

## *
##  @brief Get field handle from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc l2cap_event_connection_parameter_update_request_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field interval_min from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST
##  @param event packet
##  @return interval_min
##  @note: btstack_type 2
##

proc l2cap_event_connection_parameter_update_request_get_interval_min*(
    event: ptr uint8): uint16 =
  return little_endian_read_16(event, 4)

## *
##  @brief Get field interval_max from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST
##  @param event packet
##  @return interval_max
##  @note: btstack_type 2
##

proc l2cap_event_connection_parameter_update_request_get_interval_max*(
    event: ptr uint8): uint16 =
  return little_endian_read_16(event, 6)

## *
##  @brief Get field latencey from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST
##  @param event packet
##  @return latencey
##  @note: btstack_type 2
##

proc l2cap_event_connection_parameter_update_request_get_latencey*(
    event: ptr uint8): uint16 =
  return little_endian_read_16(event, 8)

## *
##  @brief Get field timeout_multiplier from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST
##  @param event packet
##  @return timeout_multiplier
##  @note: btstack_type 2
##

proc l2cap_event_connection_parameter_update_request_get_timeout_multiplier*(
    event: ptr uint8): uint16 =
  return little_endian_read_16(event, 10)

## *
##  @brief Get field handle from event L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc l2cap_event_connection_parameter_update_response_get_handle*(
    event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field local_cid from event L2CAP_EVENT_CAN_SEND_NOW
##  @param event packet
##  @return local_cid
##  @note: btstack_type 2
##

proc l2cap_event_can_send_now_get_local_cid*(event: ptr uint8): uint16 =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field handle from event ATT_EVENT_MTU_EXCHANGE_COMPLETE
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc att_event_mtu_exchange_complete_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field MTU from event ATT_EVENT_MTU_EXCHANGE_COMPLETE
##  @param event packet
##  @return MTU
##  @note: btstack_type 2
##

proc att_event_mtu_exchange_complete_get_MTU*(event: ptr uint8): uint16 =
  return little_endian_read_16(event, 4)

## *
##  @brief Get field status from event ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
##  @param event packet
##  @return status
##  @note: btstack_type 1
##

proc att_event_handle_value_indication_complete_get_status*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 2)[]

## *
##  @brief Get field conn_handle from event ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
##  @param event packet
##  @return conn_handle
##  @note: btstack_type H
##

proc att_event_handle_value_indication_complete_get_conn_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 3)

## *
##  @brief Get field attribute_handle from event ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
##  @param event packet
##  @return attribute_handle
##  @note: btstack_type 2
##

proc att_event_handle_value_indication_complete_get_attribute_handle*(
    event: ptr uint8): uint16 =
  return little_endian_read_16(event, 5)

## *
##  @brief Get field handle from event SM_EVENT_JUST_WORKS_REQUEST
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc sm_event_just_works_request_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field addr_type from event SM_EVENT_JUST_WORKS_REQUEST
##  @param event packet
##  @return addr_type
##  @note: btstack_type 1
##

proc sm_event_just_works_request_get_addr_type*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field address from event SM_EVENT_JUST_WORKS_REQUEST
##  @param event packet
##  @param Pointer to storage for address
##  @note: btstack_type B
##

proc sm_event_just_works_request_get_address*(event: ptr uint8; address: ptr uint8) =
  reverse_bd_addr(decode_event_offset(event, uint8, 5), address)

## *
##  @brief Get field handle from event SM_EVENT_JUST_WORKS_CANCEL
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc sm_event_just_works_cancel_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field addr_type from event SM_EVENT_JUST_WORKS_CANCEL
##  @param event packet
##  @return addr_type
##  @note: btstack_type 1
##

proc sm_event_just_works_cancel_get_addr_type*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field address from event SM_EVENT_JUST_WORKS_CANCEL
##  @param event packet
##  @param Pointer to storage for address
##  @note: btstack_type B
##

proc sm_event_just_works_cancel_get_address*(event: ptr uint8; address: ptr uint8) =
  reverse_bd_addr(decode_event_offset(event, uint8, 5), address)

## *
##  @brief Get field handle from event SM_EVENT_PASSKEY_DISPLAY_NUMBER
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc sm_event_passkey_display_number_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field addr_type from event SM_EVENT_PASSKEY_DISPLAY_NUMBER
##  @param event packet
##  @return addr_type
##  @note: btstack_type 1
##

proc sm_event_passkey_display_number_get_addr_type*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field address from event SM_EVENT_PASSKEY_DISPLAY_NUMBER
##  @param event packet
##  @param Pointer to storage for address
##  @note: btstack_type B
##

proc sm_event_passkey_display_number_get_address*(event: ptr uint8;
    address: ptr uint8) =
  reverse_bd_addr(decode_event_offset(event, uint8, 5), address)

## *
##  @brief Get field passkey from event SM_EVENT_PASSKEY_DISPLAY_NUMBER
##  @param event packet
##  @return passkey
##  @note: btstack_type 4
##

proc sm_event_passkey_display_number_get_passkey*(event: ptr uint8): uint32 =
  return little_endian_read_32(event, 11)

## *
##  @brief Get field handle from event SM_EVENT_PASSKEY_DISPLAY_CANCEL
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc sm_event_passkey_display_cancel_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field addr_type from event SM_EVENT_PASSKEY_DISPLAY_CANCEL
##  @param event packet
##  @return addr_type
##  @note: btstack_type 1
##

proc sm_event_passkey_display_cancel_get_addr_type*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field address from event SM_EVENT_PASSKEY_DISPLAY_CANCEL
##  @param event packet
##  @param Pointer to storage for address
##  @note: btstack_type B
##

proc sm_event_passkey_display_cancel_get_address*(event: ptr uint8;
    address: ptr uint8) =
  reverse_bd_addr(decode_event_offset(event, uint8, 5), address)

## *
##  @brief Get field handle from event SM_EVENT_PASSKEY_INPUT_NUMBER
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc sm_event_passkey_input_number_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field addr_type from event SM_EVENT_PASSKEY_INPUT_NUMBER
##  @param event packet
##  @return addr_type
##  @note: btstack_type 1
##

proc sm_event_passkey_input_number_get_addr_type*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field address from event SM_EVENT_PASSKEY_INPUT_NUMBER
##  @param event packet
##  @param Pointer to storage for address
##  @note: btstack_type B
##

proc sm_event_passkey_input_number_get_address*(event: ptr uint8; address: ptr uint8) =
  reverse_bd_addr(decode_event_offset(event, uint8, 5), address)

## *
##  @brief Get field handle from event SM_EVENT_PASSKEY_INPUT_CANCEL
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc sm_event_passkey_input_cancel_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field addr_type from event SM_EVENT_PASSKEY_INPUT_CANCEL
##  @param event packet
##  @return addr_type
##  @note: btstack_type 1
##

proc sm_event_passkey_input_cancel_get_addr_type*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field address from event SM_EVENT_PASSKEY_INPUT_CANCEL
##  @param event packet
##  @param Pointer to storage for address
##  @note: btstack_type B
##

proc sm_event_passkey_input_cancel_get_address*(event: ptr uint8; address: ptr uint8) =
  reverse_bd_addr(decode_event_offset(event, uint8, 5), address)

## *
##  @brief Get field handle from event SM_EVENT_IDENTITY_RESOLVING_STARTED
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc sm_event_identity_resolving_started_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field addr_type from event SM_EVENT_IDENTITY_RESOLVING_STARTED
##  @param event packet
##  @return addr_type
##  @note: btstack_type 1
##

proc sm_event_identity_resolving_started_get_addr_type*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field address from event SM_EVENT_IDENTITY_RESOLVING_STARTED
##  @param event packet
##  @param Pointer to storage for address
##  @note: btstack_type B
##

proc sm_event_identity_resolving_started_get_address*(event: ptr uint8;
    address: ptr uint8) =
  reverse_bd_addr(decode_event_offset(event, uint8, 5), address)

## *
##  @brief Get field handle from event SM_EVENT_IDENTITY_RESOLVING_FAILED
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc sm_event_identity_resolving_failed_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field addr_type from event SM_EVENT_IDENTITY_RESOLVING_FAILED
##  @param event packet
##  @return addr_type
##  @note: btstack_type 1
##

proc sm_event_identity_resolving_failed_get_addr_type*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field address from event SM_EVENT_IDENTITY_RESOLVING_FAILED
##  @param event packet
##  @param Pointer to storage for address
##  @note: btstack_type B
##

proc sm_event_identity_resolving_failed_get_address*(event: ptr uint8;
    address: ptr uint8) =
  reverse_bd_addr(decode_event_offset(event, uint8, 5), address)

## *
##  @brief Get field handle from event SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc sm_event_identity_resolving_succeeded_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field addr_type from event SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED
##  @param event packet
##  @return addr_type
##  @note: btstack_type 1
##

proc sm_event_identity_resolving_succeeded_get_addr_type*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field address from event SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED
##  @param event packet
##  @param Pointer to storage for address
##  @note: btstack_type B
##

proc sm_event_identity_resolving_succeeded_get_address*(event: ptr uint8;
    address: ptr uint8) =
  reverse_bd_addr(decode_event_offset(event, uint8, 5), address)

## *
##  @brief Get field le_device_db_index from event SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED
##  @param event packet
##  @return le_device_db_index
##  @note: btstack_type 2
##

proc sm_event_identity_resolving_succeeded_get_le_device_db_index*(
    event: ptr uint8): uint16 =
  return little_endian_read_16(event, 11)

## *
##  @brief Get field handle from event SM_EVENT_AUTHORIZATION_REQUEST
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc sm_event_authorization_request_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field addr_type from event SM_EVENT_AUTHORIZATION_REQUEST
##  @param event packet
##  @return addr_type
##  @note: btstack_type 1
##

proc sm_event_authorization_request_get_addr_type*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field address from event SM_EVENT_AUTHORIZATION_REQUEST
##  @param event packet
##  @param Pointer to storage for address
##  @note: btstack_type B
##

proc sm_event_authorization_request_get_address*(event: ptr uint8;
    address: ptr uint8) =
  reverse_bd_addr(decode_event_offset(event, uint8, 5), address)

## *
##  @brief Get field handle from event SM_EVENT_AUTHORIZATION_RESULT
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc sm_event_authorization_result_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field addr_type from event SM_EVENT_AUTHORIZATION_RESULT
##  @param event packet
##  @return addr_type
##  @note: btstack_type 1
##

proc sm_event_authorization_result_get_addr_type*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field address from event SM_EVENT_AUTHORIZATION_RESULT
##  @param event packet
##  @param Pointer to storage for address
##  @note: btstack_type B
##

proc sm_event_authorization_result_get_address*(event: ptr uint8; address: ptr uint8) =
  reverse_bd_addr(decode_event_offset(event, uint8, 5), address)

## *
##  @brief Get field authorization_result from event SM_EVENT_AUTHORIZATION_RESULT
##  @param event packet
##  @return authorization_result
##  @note: btstack_type 1
##

proc sm_event_authorization_result_get_authorization_result*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 11)[]

## *
##  @brief Get field handle from event GAP_EVENT_SECURITY_LEVEL
##  @param event packet
##  @return handle
##  @note: btstack_type H
##

proc gap_event_security_level_get_handle*(event: ptr uint8): hci_con_handle_t =
  return little_endian_read_16(event, 2)

## *
##  @brief Get field security_level from event GAP_EVENT_SECURITY_LEVEL
##  @param event packet
##  @return security_level
##  @note: btstack_type 1
##

proc gap_event_security_level_get_security_level*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 4)[]

## *
##  @brief Get field status from event GAP_EVENT_DEDICATED_BONDING_COMPLETED
##  @param event packet
##  @return status
##  @note: btstack_type 1
##

proc gap_event_dedicated_bonding_completed_get_status*(event: ptr uint8): uint8 =
  return decode_event_offset(event, uint8, 2)[]

type
  event_disconn_complete_t* {.importc: "event_disconn_complete_t",
                             header: "btstack_event.h", bycopy.} = object
    status* {.importc: "status".}: uint8
    conn_handle* {.importc: "conn_handle".}: uint16
    reason* {.importc: "reason".}: uint8

  event_vendor_ccm_complete_t* {.importc: "event_vendor_ccm_complete_t",
                                header: "btstack_event.h", bycopy.} = object
    status* {.importc: "status".}: uint8 ##  for DEC, 0 means OK, 1 means ERROR
    `type`* {.importc: "type".}: uint8 ##  0: encrypt  1: decrypt
    mic_size* {.importc: "mic_size".}: uint8
    msg_len* {.importc: "msg_len".}: uint16
    aad_len* {.importc: "aad_len".}: uint16
    tag* {.importc: "tag".}: uint32 ##  same value as in command
    out_msg* {.importc: "out_msg".}: ptr uint8

  le_meta_event_create_conn_complete_t* {.
      importc: "le_meta_event_create_conn_complete_t", header: "btstack_event.h",
      bycopy.} = object
    status* {.importc: "status".}: uint8 ## Status of received command
    ## Connection handle
    handle* {.importc: "handle".}: uint16 ## Device role - 0=Master/ 1=Slave
    role* {.importc: "role".}: uint8 ## Peer address type - 0=public/1=random
    peer_addr_type* {.importc: "peer_addr_type".}: bd_addr_type_t ## Peer address
    peer_addr* {.importc: "peer_addr".}: bd_addr_t ## Connection interval
    interval* {.importc: "interval".}: uint16 ## Connection latency
    latency* {.importc: "latency".}: uint16 ## Link supervision timeout
    sup_timeout* {.importc: "sup_timeout".}: uint16 ## Master clock accuracy
    clk_accuracy* {.importc: "clk_accuracy".}: uint8


##   LE Connection Update Complete Event

type
  le_meta_event_conn_update_complete_t* {.
      importc: "le_meta_event_conn_update_complete_t", header: "btstack_event.h",
      bycopy.} = object
    status* {.importc: "status".}: uint8
    handle* {.importc: "handle".}: uint16
    interval* {.importc: "interval".}: uint16
    latency* {.importc: "latency".}: uint16
    sup_timeout* {.importc: "sup_timeout".}: uint16


##  LE Read Remote Features Complete Event

type
  le_meta_event_read_remote_feature_complete_t* {.
      importc: "le_meta_event_read_remote_feature_complete_t",
      header: "btstack_event.h", bycopy.} = object
    status* {.importc: "status".}: uint8
    handle* {.importc: "handle".}: uint16
    features* {.importc: "features".}: array[8, uint8]


##  LE Long Term Key Request Event

type
  le_meta_event_long_term_key_request_t* {.
      importc: "le_meta_event_long_term_key_request_t", header: "btstack_event.h",
      bycopy.} = object
    handle* {.importc: "handle".}: uint16
    random_number* {.importc: "random_number".}: array[8, uint8]
    encryption_div* {.importc: "encryption_div".}: array[2, uint8]


##  LE Remote Connection Parameter Request Event

type
  le_meta_event_remote_conn_param_request_t* {.
      importc: "le_meta_event_remote_conn_param_request_t",
      header: "btstack_event.h", bycopy.} = object
    handle* {.importc: "handle".}: uint16
    interval_min* {.importc: "interval_min".}: uint16
    interval_max* {.importc: "interval_max".}: uint16
    latency* {.importc: "latency".}: uint16
    timeout* {.importc: "timeout".}: uint16


##  LE Data Length Change Event

type
  le_meta_event_data_length_changed_t* {.importc: "le_meta_event_data_length_changed_t",
                                        header: "btstack_event.h", bycopy.} = object
    handle* {.importc: "handle".}: uint16
    max_tx_octets* {.importc: "max_tx_octets".}: uint16
    mx_tx_time* {.importc: "mx_tx_time".}: uint16
    max_rx_octets* {.importc: "max_rx_octets".}: uint16
    mx_rx_time* {.importc: "mx_rx_time".}: uint16

  le_directed_adv_report_t* {.importc: "le_directed_adv_report_t",
                             header: "btstack_event.h", bycopy.} = object
    evt_type* {.importc: "evt_type".}: uint16
    addr_type* {.importc: "addr_type".}: bd_addr_type_t
    address* {.importc: "address".}: bd_addr_t
    direct_addr_type* {.importc: "direct_addr_type".}: bd_addr_type_t
    direct_addr* {.importc: "direct_addr".}: bd_addr_t
    rssi* {.importc: "rssi".}: int8


##  LE Directed Advertising Report Event

type
  le_meta_directed_adv_report_t* {.importc: "le_meta_directed_adv_report_t",
                                  header: "btstack_event.h", bycopy.} = object
    num_of_reports* {.importc: "num_of_reports".}: uint8
    reports* {.importc: "reports".}: array[1, le_directed_adv_report_t]


##  LE PHY Update Complete Event

type
  le_meta_phy_update_complete_t* {.importc: "le_meta_phy_update_complete_t",
                                  header: "btstack_event.h", bycopy.} = object
    status* {.importc: "status".}: uint8
    handle* {.importc: "handle".}: uint16
    tx_phy* {.importc: "tx_phy".}: phy_type_t
    rx_phy* {.importc: "rx_phy".}: phy_type_t


##  evt_type

const
  HCI_EXT_ADV_PROP_CONNECTABLE* = (1 shl 0)
  HCI_EXT_ADV_PROP_SCANNABLE* = (1 shl 1)
  HCI_EXT_ADV_PROP_DIRECTED* = (1 shl 2)
  HCI_EXT_ADV_PROP_SCAN_RSP* = (1 shl 3)
  HCI_EXT_ADV_PROP_USE_LEGACY* = (1 shl 4)
  HCI_EXT_ADV_DATA_CML* = (0 shl 5)
  HCI_EXT_ADV_DATA_HAS_MORE* = (1 shl 5)
  HCI_EXT_ADV_DATA_TRUNCED* = (2 shl 5)
  HCI_EXT_ADV_DATA_MASK* = (3 shl 5)

type
  le_ext_adv_report_t* {.importc: "le_ext_adv_report_t", header: "btstack_event.h",
                        bycopy.} = object
    evt_type* {.importc: "evt_type".}: uint16
    addr_type* {.importc: "addr_type".}: bd_addr_type_t
    address* {.importc: "address".}: bd_addr_t
    p_phy* {.importc: "p_phy".}: uint8 ##  primary phy
    s_phy* {.importc: "s_phy".}: uint8 ##  secondary phy
    sid* {.importc: "sid".}: uint8
    tx_power* {.importc: "tx_power".}: int8
    rssi* {.importc: "rssi".}: int8
    prd_adv_interval* {.importc: "prd_adv_interval".}: uint16
    direct_addr_type* {.importc: "direct_addr_type".}: bd_addr_type_t
    direct_addr* {.importc: "direct_addr".}: bd_addr_t
    data_len* {.importc: "data_len".}: uint8
    data* {.importc: "data".}: UncheckedArray[uint8]


##   LE Extended Advertising Report Event

type
  le_meta_event_ext_adv_report_t* {.importc: "le_meta_event_ext_adv_report_t",
                                   header: "btstack_event.h", bycopy.} = object
    num_of_reports* {.importc: "num_of_reports".}: uint8 ##  this is always 1
    reports* {.importc: "reports".}: array[1, le_ext_adv_report_t]


##  LE Periodic Advertising Sync Established Event

type
  le_meta_event_periodic_adv_sync_established_t* {.
      importc: "le_meta_event_periodic_adv_sync_established_t",
      header: "btstack_event.h", bycopy.} = object
    status* {.importc: "status".}: uint8
    handle* {.importc: "handle".}: uint16
    sid* {.importc: "sid".}: uint8
    addr_type* {.importc: "addr_type".}: bd_addr_type_t
    address* {.importc: "address".}: bd_addr_t
    phy* {.importc: "phy".}: phy_type_t
    interval* {.importc: "interval".}: uint16
    clk_accuracy* {.importc: "clk_accuracy".}: uint8


const
  HCI_PRD_ADV_DATA_STATUS_CML* = (0)
  HCI_PRD_ADV_DATA_STATUS_HAS_MORE* = (1)
  HCI_PRD_ADV_DATA_STATUS_TRUNCED* = (2)

##  LE Periodic Advertising Report Event

type
  le_meta_event_periodic_adv_report_t* {.importc: "le_meta_event_periodic_adv_report_t",
                                        header: "btstack_event.h", bycopy.} = object
    handle* {.importc: "handle".}: uint16
    tx_power* {.importc: "tx_power".}: int8
    rssi* {.importc: "rssi".}: int8
    unused* {.importc: "unused".}: uint8
    data_status* {.importc: "data_status".}: uint8
    data_length* {.importc: "data_length".}: uint8
    data* {.importc: "data".}: UncheckedArray[uint8]


##  LE Periodic Advertising Sync Lost Event

type
  le_meta_event_periodic_adv_sync_lost_t* {.
      importc: "le_meta_event_periodic_adv_sync_lost_t",
      header: "btstack_event.h", bycopy.} = object
    handle* {.importc: "handle".}: uint16


##  LE Scan Timeout Event
##  {}
##  LE Advertising Set Terminated Event

type
  le_meta_adv_set_terminated_t* {.importc: "le_meta_adv_set_terminated_t",
                                 header: "btstack_event.h", bycopy.} = object
    status* {.importc: "status".}: uint8
    adv_handle* {.importc: "adv_handle".}: uint8
    conn_handle* {.importc: "conn_handle".}: uint16
    num_events* {.importc: "num_events".}: uint8 ##  Num_Completed_Extended_Advertising_Events


##   LE Scan Request Received Event

type
  le_meta_scan_req_received_t* {.importc: "le_meta_scan_req_received_t",
                                header: "btstack_event.h", bycopy.} = object
    adv_handle* {.importc: "adv_handle".}: uint8
    scanner_addr_type* {.importc: "scanner_addr_type".}: bd_addr_type_t
    scanner_addr* {.importc: "scanner_addr".}: bd_addr_t

  ble_ch_sel_algo_t* {.size: sizeof(cint).} = enum
    BLE_ALGO_1, BLE_ALGO_2, BLE_ALGO_NUMBER


##  LE Channel Selection Algorithm Event

type
  le_meta_ch_sel_algo* {.importc: "le_meta_ch_sel_algo", header: "btstack_event.h",
                        bycopy.} = object
    conn_handle* {.importc: "conn_handle".}: uint16
    algo* {.importc: "algo".}: ble_ch_sel_algo_t

  le_iq_sample_t* {.importc: "le_iq_sample_t", header: "btstack_event.h", bycopy.} = object
    i* {.importc: "i".}: int8
    q* {.importc: "q".}: int8

  le_meta_connless_iq_report_t* {.importc: "le_meta_connless_iq_report_t",
                                 header: "btstack_event.h", bycopy.} = object
    sync_handle* {.importc: "sync_handle".}: uint16
    channel_index* {.importc: "channel_index".}: uint8
    rssi* {.importc: "rssi".}: int16
    rssi_ant_id* {.importc: "rssi_ant_id".}: uint8
    cte_type* {.importc: "cte_type".}: uint8
    slot_durations* {.importc: "slot_durations".}: uint8
    packet_status* {.importc: "packet_status".}: uint8
    event_counter* {.importc: "event_counter".}: uint16
    sample_count* {.importc: "sample_count".}: uint8
    samples* {.importc: "samples".}: UncheckedArray[le_iq_sample_t]

  le_meta_conn_iq_report_t* {.importc: "le_meta_conn_iq_report_t",
                             header: "btstack_event.h", bycopy.} = object
    conn_handle* {.importc: "conn_handle".}: uint16
    rx_phy* {.importc: "rx_phy".}: phy_type_t
    channel_index* {.importc: "channel_index".}: uint8
    rssi* {.importc: "rssi".}: int16
    rssi_ant_id* {.importc: "rssi_ant_id".}: uint8
    cte_type* {.importc: "cte_type".}: uint8
    slot_durations* {.importc: "slot_durations".}: uint8
    packet_status* {.importc: "packet_status".}: uint8
    event_counter* {.importc: "event_counter".}: uint16
    sample_count* {.importc: "sample_count".}: uint8
    samples* {.importc: "samples".}: UncheckedArray[le_iq_sample_t]

  le_meta_cte_req_failed_t* {.importc: "le_meta_cte_req_failed_t",
                             header: "btstack_event.h", bycopy.} = object
    status* {.importc: "status".}: uint8
    conn_handle* {.importc: "conn_handle".}: uint16

  le_meta_prd_adv_sync_transfer_recv_t* {.
      importc: "le_meta_prd_adv_sync_transfer_recv_t", header: "btstack_event.h",
      bycopy.} = object
    status* {.importc: "status".}: uint8
    conn_handle* {.importc: "conn_handle".}: uint16
    service_data* {.importc: "service_data".}: uint16
    sync_handle* {.importc: "sync_handle".}: uint16
    adv_sid* {.importc: "adv_sid".}: uint8
    addr_type* {.importc: "addr_type".}: bd_addr_type_t
    `addr`* {.importc: "addr".}: bd_addr_t
    phy* {.importc: "phy".}: phy_type_t
    prdAdvInterval* {.importc: "prdAdvInterval".}: uint16
    clkAcc* {.importc: "clkAcc".}: uint8


##  packet is from host callbacks

template decode_hci_le_meta_event*(packet, T: untyped): untyped =
  decode_event_offset(packet, T, 3)

template decode_hci_event_disconn_complete*(packet: untyped): untyped =
  decode_event_offset(packet, event_disconn_complete_t, 2)

template decode_hci_event_vendor_ccm_complete*(packet: untyped): untyped =
  decode_event_offset(packet, event_vendor_ccm_complete_t, 5)

template decode_hci_event*(packet, T: untyped): untyped =
  decode_event_offset(packet, T, 2)

##  API_END
