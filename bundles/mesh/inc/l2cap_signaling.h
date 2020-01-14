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

#ifndef __L2CAP_SIGNALING_H
#define __L2CAP_SIGNALING_H

#include <stdint.h>
#include <stdarg.h>
#include "bluetooth.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    COMMAND_REJECT = 1,
    CONNECTION_REQUEST,
    CONNECTION_RESPONSE,
    CONFIGURE_REQUEST,
    CONFIGURE_RESPONSE,
    DISCONNECTION_REQUEST,
    DISCONNECTION_RESPONSE,
    ECHO_REQUEST,
    ECHO_RESPONSE,
    INFORMATION_REQUEST,
    INFORMATION_RESPONSE,
    /* 0x0c - 0x11 used for AMP */
    CONNECTION_PARAMETER_UPDATE_REQUEST = 0x12,
    CONNECTION_PARAMETER_UPDATE_RESPONSE,
    LE_CREDIT_BASED_CONNECTION_REQUEST,
    LE_CREDIT_BASED_CONNECTION_RESPONSE,
    LE_FLOW_CONTROL_CREDIT,
    COMMAND_REJECT_LE = 0x1F  // internal to BTstack
} L2CAP_SIGNALING_COMMANDS;

uint8_t l2cap_send_signaling_le(hci_con_handle_t handle, L2CAP_SIGNALING_COMMANDS cmd, uint8_t identifier, va_list argptr);
// uint8_t  l2cap_next_sig_id(void);
// WARNING: ^^^ this API is not available in this release

// uint16_t l2cap_next_local_cid(void);
// WARNING: ^^^ this API is not available in this release


#ifdef __cplusplus
}
#endif

#endif // __L2CAP_SIGNALING_H
