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
  compiler, bluetooth, bt_types, btstack_event, btstack_linked_list

##  API_START

proc sm_private_random_addr_update_get_address*(packet: ptr uint8): ptr uint8 =
  return decode_event_offset(packet, uint8, 2)

## *
##  @brief persistent data for security & privacy
##

type
  sm_persistent_t* {.importc: "sm_persistent_t", header: "sm.h", bycopy.} = object
    er* {.importc: "er".}: sm_key_t
    ir* {.importc: "ir".}: sm_key_t
    identity_addr* {.importc: "identity_addr".}: bd_addr_t ##  A public device address or static random address used as identity address
                                                       ##  When privacy is not enabled, this should be the public device address or static random address.
                                                       ##  This should not be changed, once changed, paring is lost
    identity_addr_type* {.importc: "identity_addr_type".}: bd_addr_type_t ##  Note: only PUBLIC & RANDOM are allowed


##  Authorization state

type
  authorization_state_t* {.size: sizeof(cint).} = enum
    AUTHORIZATION_UNKNOWN, AUTHORIZATION_PENDING, AUTHORIZATION_DECLINED,
    AUTHORIZATION_GRANTED


## *
##  @brief Security configurations
##
##  @param[in]   io_capability       IO Capabilities
##  @param[in]   request_security    Let Peripheral request an encrypted connection right after connecting
##                                   Not used normally. Bonding is triggered by access to protected attributes in ATT Server
##  @param[in]   persistent          persistent data for security & privacy
##

proc sm_config*(io_capability: io_capability_t; request_security: cint;
               persistent: ptr sm_persistent_t) {.importc: "sm_config",
    header: "sm.h".}
## *
##  @brief add an sm event handler
##
##  @param callback_handler      the handler
##

proc sm_add_event_handler*(callback_handler: ptr btstack_packet_callback_registration_t) {.
    importc: "sm_add_event_handler", header: "sm.h".}
## *
##  @brief Enable private random address generation
##         When a new random address is generated, an SM_EVENT_PRIVATE_RANDOM_ADDR_UPDATE event is emitted
##
##  @param random_address_type to use (incl. OFF)
##

proc sm_private_random_address_generation_set_mode*(
    random_address_type: gap_random_address_type_t) {.
    importc: "sm_private_random_address_generation_set_mode", header: "sm.h".}
## *
##  @brief Get private random address generation mode
##

proc sm_private_random_address_generation_get_mode*(): gap_random_address_type_t {.
    importc: "sm_private_random_address_generation_get_mode", header: "sm.h".}
## *
##  @brief Set private random address generation period
##  @param period_ms         generation period
##

proc sm_private_random_address_generation_set_update_period*(period_ms: cint) {.
    importc: "sm_private_random_address_generation_set_update_period",
    header: "sm.h".}
## *
##  @brief Get current private random address
##  @return      the address
##

proc sm_private_random_address_generation_get*(): ptr uint8 {.
    importc: "sm_private_random_address_generation_get", header: "sm.h".}
## *
##
##  @brief Registers OOB Data Callback. The callback should set the oob_data and return 1 if OOB data is availble
##  @param get_oob_data_callback
##

proc sm_register_oob_data_callback*(get_oob_data_callback: proc (
    addres_type: uint8; `addr`: bd_addr_t; oob_data: ptr uint8): cint {.noconv.}) {.
    importc: "sm_register_oob_data_callback", header: "sm.h".}
## *
##  @brief Decline bonding triggered by event before
##  @param addr_type and address
##

proc sm_bonding_decline*(con_handle: hci_con_handle_t) {.
    importc: "sm_bonding_decline", header: "sm.h".}
## *
##  @brief Confirm Just Works bonding
##  @param addr_type and address
##

proc sm_just_works_confirm*(con_handle: hci_con_handle_t) {.
    importc: "sm_just_works_confirm", header: "sm.h".}
## *
##  @brief Reports passkey input by user
##  @param addr_type and address
##  @param passkey in [0..999999]
##

proc sm_passkey_input*(con_handle: hci_con_handle_t; passkey: uint32) {.
    importc: "sm_passkey_input", header: "sm.h".}
## *
##  @brief Limit the STK generation methods. Bonding is stopped if the resulting one isn't in the list
##  @param OR combination of SM_STK_GENERATION_METHOD_
##

proc sm_set_accepted_stk_generation_methods*(
    accepted_stk_generation_methods: uint8) {.
    importc: "sm_set_accepted_stk_generation_methods", header: "sm.h".}
## *
##  @brief Set the accepted encryption key size range. Bonding is stopped if the result isn't within the range
##  @param min_size (default 7)
##  @param max_size (default 16)
##

proc sm_set_encryption_key_size_range*(min_size: uint8; max_size: uint8) {.
    importc: "sm_set_encryption_key_size_range", header: "sm.h".}
## *
##  @brief Sets the requested authentication requirements, bonding yes/no, MITM yes/no
##  @param OR combination of SM_AUTHREQ_ flags
##

proc sm_set_authentication_requirements*(auth_req: uint8) {.
    importc: "sm_set_authentication_requirements", header: "sm.h".}
## *
##  @brief Let Peripheral request an encrypted connection right after connecting
##  @note Not used normally. Bonding is triggered by access to protected attributes in ATT Server
##
##  void sm_set_request_security(int enable);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Trigger Security Request
##  @note Not used normally. Bonding is triggered by access to protected attributes in ATT Server
##

proc sm_send_security_request*(con_handle: hci_con_handle_t) {.
    importc: "sm_send_security_request", header: "sm.h".}
## *
##
##  @brief Get encryption key size.
##  @param addr_type and address
##  @return 0 if not encrypted, 7-16 otherwise
##

proc sm_encryption_key_size*(con_handle: hci_con_handle_t): cint {.
    importc: "sm_encryption_key_size", header: "sm.h".}
## *
##  @brief Get authentication property.
##  @param addr_type and address
##  @return 1 if bonded with OOB/Passkey (AND MITM protection)
##

proc sm_authenticated*(con_handle: hci_con_handle_t): cint {.
    importc: "sm_authenticated", header: "sm.h".}
## *
##  @brief Queries authorization state.
##  @param addr_type and address
##  @return authorization_state for the current session
##

proc sm_authorization_state*(con_handle: hci_con_handle_t): authorization_state_t {.
    importc: "sm_authorization_state", header: "sm.h".}
## *
##  @brief Used by att_server.c to request user authorization.
##  @param addr_type and address
##

proc sm_request_pairing*(con_handle: hci_con_handle_t) {.
    importc: "sm_request_pairing", header: "sm.h".}
## *
##  @brief Report user authorization decline.
##  @param addr_type and address
##

proc sm_authorization_decline*(con_handle: hci_con_handle_t) {.
    importc: "sm_authorization_decline", header: "sm.h".}
## *
##  @brief Report user authorization grant.
##  @param addr_type and address
##

proc sm_authorization_grant*(con_handle: hci_con_handle_t) {.
    importc: "sm_authorization_grant", header: "sm.h".}
##
##  @brief Match address against bonded devices
##  @return 0 if successfully added to lookup queue
##  @note Triggers SM_IDENTITY_RESOLVING_* events
##

proc sm_address_resolution_lookup*(addr_type: uint8; `addr`: bd_addr_t): cint {.
    importc: "sm_address_resolution_lookup", header: "sm.h".}
## *
##  @brief Identify device in LE Device DB.
##  @param handle
##  @return index from le_device_db or -1 if not found/identified
##

proc sm_le_device_key*(con_handle: hci_con_handle_t): cint {.
    importc: "sm_le_device_key", header: "sm.h".}