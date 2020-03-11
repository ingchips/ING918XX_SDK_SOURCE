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
  bluetooth, bt_types, gap

proc l2cap_register_fixed_channel*(packet_handler: btstack_packet_handler_t;
                                  channel_id: uint16) {.
    importc: "l2cap_register_fixed_channel", header: "l2cap.h".}
proc l2cap_can_send_fixed_channel_packet_now*(con_handle: hci_con_handle_t;
    channel_id: uint16): cint {.importc: "l2cap_can_send_fixed_channel_packet_now",
                             header: "l2cap.h".}
proc l2cap_request_can_send_fix_channel_now_event*(con_handle: hci_con_handle_t;
    channel_id: uint16) {.importc: "l2cap_request_can_send_fix_channel_now_event",
                        header: "l2cap.h".}
proc l2cap_send_connectionless*(con_handle: hci_con_handle_t; cid: uint16;
                               data1: ptr uint8; data1_len: uint16; data2: ptr uint8;
                               data2_len: uint16): cint {.
    importc: "l2cap_send_connectionless", header: "l2cap.h".}
proc l2cap_send_connectionless3*(con_handle: hci_con_handle_t; cid: uint16;
                                data1: ptr uint8; data1_len: uint16;
                                data2: ptr uint8; data2_len: uint16;
                                data3: ptr uint8; data3_len: uint16): cint {.
    importc: "l2cap_send_connectionless3", header: "l2cap.h".}
proc l2cap_le_send_flow_control_credit*(con_handle: hci_con_handle_t; id: uint8;
                                       cid: uint16; credits: uint16): cint {.
    importc: "l2cap_le_send_flow_control_credit", header: "l2cap.h".}
##  PTS Testing

proc l2cap_send_echo_request*(con_handle: hci_con_handle_t; data: ptr uint8;
                             len: uint16): cint {.
    importc: "l2cap_send_echo_request", header: "l2cap.h".}
proc l2cap_require_security_level_2_for_outgoing_sdp*() {.
    importc: "l2cap_require_security_level_2_for_outgoing_sdp", header: "l2cap.h".}
##  Used by RFCOMM - similar to l2cap_can-send_packet_now but does not check if outgoing buffer is reserved
##  int  l2cap_can_send_prepared_packet_now(uint16_t local_cid);
##  WARNING: ^^^ this API is not available in this release
##  API_START
## *
##  @brief Set up L2CAP and register L2CAP with HCI layer.
##

proc l2cap_init*() {.importc: "l2cap_init", header: "l2cap.h".}
## *
##  @brief Registers packet handler for LE Connection Parameter Update events
##

proc l2cap_register_packet_handler*(handler: btstack_packet_handler_t) {.
    importc: "l2cap_register_packet_handler", header: "l2cap.h".}
## *
##  @brief Get max MTU for Classic connections based on btstack configuration
##

proc l2cap_max_mtu*(): uint16 {.importc: "l2cap_max_mtu", header: "l2cap.h".}
## *
##  @brief Get max MTU for LE connections based on btstack configuration
##

proc l2cap_max_le_mtu*(): uint16 {.importc: "l2cap_max_le_mtu", header: "l2cap.h".}
## *
##  @brief Creates L2CAP channel to the PSM of a remote device with baseband address. A new baseband connection will be initiated if necessary.
##  @param packet_handler
##  @param address
##  @param psm
##  @param mtu
##  @param local_cid
##  @param status
##

proc l2cap_create_channel*(packet_handler: btstack_packet_handler_t;
                          address: bd_addr_t; psm: uint16; mtu: uint16;
                          out_local_cid: ptr uint16): uint8 {.
    importc: "l2cap_create_channel", header: "l2cap.h".}
## *
##  @brief Disconnects L2CAP channel with given identifier.
##

proc l2cap_disconnect*(local_cid: uint16; reason: uint8) {.
    importc: "l2cap_disconnect", header: "l2cap.h".}
## *
##  @brief Queries the maximal transfer unit (MTU) for L2CAP channel with given identifier.
##

proc l2cap_get_remote_mtu_for_local_cid*(local_cid: uint16): uint16 {.
    importc: "l2cap_get_remote_mtu_for_local_cid", header: "l2cap.h".}
## *
##  @brief Sends L2CAP data packet to the channel with given identifier.
##

proc l2cap_send*(local_cid: uint16; data: ptr uint8; len: uint16): cint {.
    importc: "l2cap_send", header: "l2cap.h".}
## *
##  @brief Registers L2CAP service with given PSM and MTU, and assigns a packet handler.
##

proc l2cap_register_service*(packet_handler: btstack_packet_handler_t; psm: uint16;
                            mtu: uint16; security_level: gap_security_level_t): uint8 {.
    importc: "l2cap_register_service", header: "l2cap.h".}
## *
##  @brief Unregisters L2CAP service with given PSM.  On embedded systems, use NULL for connection parameter.
##

proc l2cap_unregister_service*(psm: uint16) {.importc: "l2cap_unregister_service",
    header: "l2cap.h".}
## *
##  @brief Accepts incoming L2CAP connection.
##
##  void l2cap_accept_connection(uint16_t local_cid);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Deny incoming L2CAP connection.
##
##  void l2cap_decline_connection(uint16_t local_cid, uint8_t reason);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief Check if outgoing buffer is available and that there's space on the Bluetooth module
##

proc l2cap_can_send_packet_now*(local_cid: uint16): cint {.
    importc: "l2cap_can_send_packet_now", header: "l2cap.h".}
## *
##  @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible
##  @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
##        so packet handler should be ready to handle it
##  @param local_cid
##

proc l2cap_request_can_send_now_event*(local_cid: uint16) {.
    importc: "l2cap_request_can_send_now_event", header: "l2cap.h".}
## *
##  @brief Request an update of the connection parameter for a given LE connection
##  @param handle
##  @param conn_interval_min (unit: 1.25ms)
##  @param conn_interval_max (unit: 1.25ms)
##  @param conn_latency
##  @param supervision_timeout (unit: 10ms)
##  @returns 0 if ok
##

proc l2cap_request_connection_parameter_update*(con_handle: hci_con_handle_t;
    conn_interval_min: uint16; conn_interval_max: uint16; conn_latency: uint16;
    supervision_timeout: uint16): cint {.importc: "l2cap_request_connection_parameter_update",
                                      header: "l2cap.h".}
when defined(CONNECTION_PARAMETER_DYNAMICALLY_ADJUST):
  proc l2cap_update_MTU_on_LL_link*(channel: ptr l2cap_channel_t): uint8 {.
      importc: "l2cap_update_MTU_on_LL_link", header: "l2cap.h".}
##  API_END

## to define the error result of the connection response

when defined(L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE):
  const
    L2CAP_CONNECTION_REFUSED_LEPSM_NOT_SUPPORTED* = 0x00000002
    L2CAP_CONNECTION_REFUSED_RESERVED* = 0x00000003
    L2CAP_CONNECTION_REFUSED_NO_RESOURCE_AVAILABLE* = 0x00000004
    L2CAP_CONNECTION_REFUSED_INSUFFICIENT_AUTHEN* = 0x00000005
    L2CAP_CONNECTION_REFUSED_INSUFFICIENT_AUTHOR* = 0x00000006
    L2CAP_CONNECTION_REFUSED_INSUFFICIENT_ENCRYPT_KEY_SIZE* = 0x00000007
    L2CAP_CONNECTION_REFUSED_INSUFFICIENT_ENCRYPT* = 0x00000008
    L2CAP_CONNECTION_REFUSED_INVALID_SCID* = 0x00000009
    L2CAP_CONNECTION_REFUSED_SCID_ALREADY_ALLOCATED* = 0x0000000A