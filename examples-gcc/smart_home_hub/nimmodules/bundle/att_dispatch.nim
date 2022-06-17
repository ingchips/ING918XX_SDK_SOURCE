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
## *
##  Dispatcher for independent implementation of ATT client and server
##

import
  btstack_util, bt_types

## *
##  @brief reset att dispatchter
##  @param packet_hander for ATT client packets
##

proc att_dispatch_register_client*(packet_handler: btstack_packet_handler_t) {.
    importc: "att_dispatch_register_client", header: "att_dispatch.h".}
## *
##  @brief reset att dispatchter
##  @param packet_hander for ATT server packets
##

proc att_dispatch_register_server*(packet_handler: btstack_packet_handler_t) {.
    importc: "att_dispatch_register_server", header: "att_dispatch.h".}
## *
##  @brief can send packet for client
##  @param con_handle
##

proc att_dispatch_client_can_send_now*(con_handle: hci_con_handle_t): cint {.
    importc: "att_dispatch_client_can_send_now", header: "att_dispatch.h".}
## *
##  @brief can send packet for server
##  @param con_handle
##

proc att_dispatch_server_can_send_now*(con_handle: hci_con_handle_t): cint {.
    importc: "att_dispatch_server_can_send_now", header: "att_dispatch.h".}
## *
##  @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible for client
##  @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
##        so packet handler should be ready to handle it
##  @param con_handle
##

proc att_dispatch_client_request_can_send_now_event*(con_handle: hci_con_handle_t) {.
    importc: "att_dispatch_client_request_can_send_now_event",
    header: "att_dispatch.h".}
## *
##  @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible for server
##  @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
##        so packet handler should be ready to handle it
##  @param con_handle
##

proc att_dispatch_server_request_can_send_now_event*(con_handle: hci_con_handle_t) {.
    importc: "att_dispatch_server_request_can_send_now_event",
    header: "att_dispatch.h".}