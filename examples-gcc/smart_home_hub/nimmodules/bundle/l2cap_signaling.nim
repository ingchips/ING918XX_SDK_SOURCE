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
  bluetooth

type
  L2CAP_SIGNALING_COMMANDS* {.size: sizeof(cint).} = enum
    COMMAND_REJECT = 1, CONNECTION_REQUEST, CONNECTION_RESPONSE, CONFIGURE_REQUEST,
    CONFIGURE_RESPONSE, DISCONNECTION_REQUEST, DISCONNECTION_RESPONSE,
    ECHO_REQUEST, ECHO_RESPONSE, INFORMATION_REQUEST, INFORMATION_RESPONSE, ##  0x0c - 0x11 used for AMP
    CONNECTION_PARAMETER_UPDATE_REQUEST = 0x00000012,
    CONNECTION_PARAMETER_UPDATE_RESPONSE, LE_CREDIT_BASED_CONNECTION_REQUEST,
    LE_CREDIT_BASED_CONNECTION_RESPONSE, LE_FLOW_CONTROL_CREDIT,
    COMMAND_REJECT_LE = 0x0000001F


proc l2cap_send_signaling_le*(handle: hci_con_handle_t;
                             cmd: L2CAP_SIGNALING_COMMANDS; identifier: uint8;
                             argptr: va_list): uint8 {.
    importc: "l2cap_send_signaling_le", header: "l2cap_signaling.h".}
proc l2cap_next_sig_id*(): uint8 {.importc: "l2cap_next_sig_id",
                                header: "l2cap_signaling.h".}
proc l2cap_next_local_cid*(): uint16 {.importc: "l2cap_next_local_cid",
                                    header: "l2cap_signaling.h".}