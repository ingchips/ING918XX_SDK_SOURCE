##
##  Copyright (C) INGCHIPS. All rights reserved.
##  This code is INGCHIPS proprietary and confidential.
##  Any use of the code for whatever purpose is subject to
##  specific written permission of INGCHIPS.
##

type
  prog_ver_t* {.importc: "prog_ver_t", header: "ota_service.h", bycopy.} = object
    major* {.importc: "major".}: cshort
    minor* {.importc: "minor".}: char
    patch* {.importc: "patch".}: char

  ota_ver_t* {.importc: "ota_ver_t", header: "ota_service.h", bycopy.} = object
    platform* {.importc: "platform".}: prog_ver_t
    app* {.importc: "app".}: prog_ver_t


const
  OTA_CTRL_START* = 0x000000AA
  OTA_CTRL_PAGE_BEGIN* = 0x000000B0
  OTA_CTRL_PAGE_END* = 0x000000B1
  OTA_CTRL_READ_PAGE* = 0x000000C0
  OTA_CTRL_SWITCH_APP* = 0x000000D0
  OTA_CTRL_REBOOT* = 0x000000FF
  OTA_STATUS_DISABLED* = 0
  OTA_STATUS_OK* = 1
  OTA_STATUS_ERROR* = 2

## *
## @brief Tell this module handles used by FOTA service
##
##    If FOTA related GATT service is *not* initialized by `ota_init_service`, then call this function.
##

proc ota_init_handles*(handler_ver: uint16; handle_ctrl: uint16; handle_data: uint16) {.
    importc: "ota_init_handles", header: "ota_service.h".}
proc ota_write_callback*(att_handle: uint16; transaction_mode: uint16;
                        offset: uint16; buffer: ptr uint8; buffer_size: uint16): cint {.
    importc: "ota_write_callback", header: "ota_service.h".}
proc ota_read_callback*(att_handle: uint16; offset: uint16; buffer: ptr uint8;
                       buffer_size: uint16): cint {.importc: "ota_read_callback",
    header: "ota_service.h".}