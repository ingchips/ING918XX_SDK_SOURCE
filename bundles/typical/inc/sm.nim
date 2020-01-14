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