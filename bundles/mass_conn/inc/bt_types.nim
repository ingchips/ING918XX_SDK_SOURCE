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
  bluetooth, btstack_linked_list

template BIT*(pos: untyped): untyped =
  (1 shl (pos))

type
  gap_random_address_type_t* {.size: sizeof(cint).} = enum
    GAP_RANDOM_ADDRESS_OFF = 0, GAP_RANDOM_ADDRESS_NON_RESOLVABLE,
    GAP_RANDOM_ADDRESS_RESOLVABLE


## *
##  @brief 128 bit key used with AES128 in Security Manager
##

type
  sm_key_t* = array[16, uint8]

##  packet handler

type
  btstack_packet_handler_t* = proc (packet_type: uint8; channel: uint16;
                                 packet: ptr uint8; size: uint16) {.noconv.}

##  packet callback supporting multiple registrations

type
  btstack_packet_callback_registration_t* {.
      importc: "btstack_packet_callback_registration_t", header: "bt_types.h",
      bycopy.} = object
    item* {.importc: "item".}: btstack_linked_item_t
    callback* {.importc: "callback".}: btstack_packet_handler_t


##  packet handler

type
  user_packet_handler_t* = proc (packet_type: uint8; channel: uint16;
                              packet: ptr uint8; size: uint16) {.noconv.}

## macro definition:
## *
##   state of Bluetooth controller
##

type
  HCI_POWER_MODE* {.size: sizeof(cint).} = enum
    HCI_POWER_OFF = 0, HCI_POWER_ON, HCI_POWER_SLEEP


## *
##  State of BTstack
##

type
  HCI_STATE* {.size: sizeof(cint).} = enum
    HCI_STATE_OFF = 0, HCI_STATE_INITIALIZING, HCI_STATE_WORKING, HCI_STATE_HALTING,
    HCI_STATE_SLEEPING, HCI_STATE_FALLING_ASLEEP


## *
##  @format 1
##  @param state
##

const
  BTSTACK_EVENT_STATE* = 0x00000060

## *
##  @format
##

const
  ATT_EVENT_CAN_SEND_NOW* = 0x000000B7
  BTSTACK_EVENT_USER_MSG* = 0x000000FF
