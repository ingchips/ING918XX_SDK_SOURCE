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

type                          ##  platform callback for putc (for logging)
    ##  NOTE: param (void *data) is casted from char *
    ##  example: uint32_t cb_putc(char *c, void *dummy)
  platform_ver_t* {.importc: "platform_ver_t", header: "platform_api.h", bycopy.} = object
    major* {.importc: "major".}: cushort
    minor* {.importc: "minor".}: char
    patch* {.importc: "patch".}: char

  hard_fault_info_t* {.importc: "hard_fault_info_t", header: "platform_api.h", bycopy.} = object
    r0* {.importc: "r0".}: uint32
    r1* {.importc: "r1".}: uint32
    r2* {.importc: "r2".}: uint32
    r3* {.importc: "r3".}: uint32
    r12* {.importc: "r12".}: uint32
    lr* {.importc: "lr".}: uint32
    pc* {.importc: "pc".}: uint32
    psr* {.importc: "psr".}: uint32

  assertion_info_t* {.importc: "assertion_info_t", header: "platform_api.h", bycopy.} = object
    file_name* {.importc: "file_name".}: cstring
    line_no* {.importc: "line_no".}: cint

  platform_evt_callback_type_t* {.size: sizeof(cint).} = enum
    PLATFORM_CB_EVT_PUTC,     ##  when bluetooth protocol stack ask app to initialize
    PLATFORM_CB_EVT_PROFILE_INIT, ##  periphrals need to be re-initialized after deep-sleep, user can handle this event
    PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, ##  return 0 if deep sleep is not allowed now; else deep sleep is allowed
                                         ##  e.g. when periphrals still have data to process (UART is tx buffer not empty)
    PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, ##  when hard fault occurs
                                             ##  NOTE: param (void *data) is casted from hard_fault_info_t *
                                             ##  example: uint32_t cb_hard_fault(hard_fault_info_t *info, void *dummy)
                                             ##  if this callback is not defined, CPU enters a dead loop
    PLATFORM_CB_EVT_HARD_FAULT, ##  when software assertion fails
                               ##  NOTE: param (void *data) is casted from assertion_info_t *
                               ##  if this callback is not defined, CPU enters a dead loop
    PLATFORM_CB_EVT_ASSERTION, ##  when LLE is initializing
    PLATFORM_CB_EVT_LLE_INIT, ##  when allocation on heap fails (heap out of memory)
                             ##  if this callback is not defined, CPU enters a dead loop
    PLATFORM_CB_EVT_HEAP_OOM, ##  platform callback for output or save a trace item
                             ##  NOTE: param (void *data) is casted from platform_trace_evt_t *
    PLATFORM_CB_EVT_TRACE, PLATFORM_CB_EVT_MAX
  platform_irq_callback_type_t* {.size: sizeof(cint).} = enum
    PLATFORM_CB_IRQ_RTC, PLATFORM_CB_IRQ_TIMER0, PLATFORM_CB_IRQ_TIMER1,
    PLATFORM_CB_IRQ_TIMER2, PLATFORM_CB_IRQ_GPIO, PLATFORM_CB_IRQ_SPI0,
    PLATFORM_CB_IRQ_SPI1, PLATFORM_CB_IRQ_UART0, PLATFORM_CB_IRQ_UART1,
    PLATFORM_CB_IRQ_I2C0, PLATFORM_CB_IRQ_I2C1, PLATFORM_CB_IRQ_MAX
  f_platform_evt_cb* = proc (data: pointer; user_data: pointer): uint32 {.noconv.}
  f_platform_irq_cb* = proc (user_data: pointer): uint32 {.noconv.}



##  A trace item is a combination of data1 and data2. Note:
##  1. len1 or len2 might be 0, but not both
##  2. if callback function finds that it can't output data of size len1 + len2, then, both data1
##     & data2 should be discarded to avoid trace item corruption.

type
  platform_evt_trace_t* {.importc: "platform_evt_trace_t",
                         header: "platform_api.h", bycopy.} = object
    data1* {.importc: "data1".}: pointer
    data2* {.importc: "data2".}: pointer
    len1* {.importc: "len1".}: uint16
    len2* {.importc: "len2".}: uint16


##  A trace item is has an ID

type
  platform_trace_item_t* {.size: sizeof(cint).} = enum
    PLATFORM_TRACE_ID_EVENT = 0, PLATFORM_TRACE_ID_HCI_CMD = 1,
    PLATFORM_TRACE_ID_HCI_EVENT = 2, PLATFORM_TRACE_ID_HCI_ACL = 3,
    PLATFORM_TRACE_ID_LLCP = 4


## *
## ***************************************************************************************
##  @brief register callback function for platform events
##
##  @param[in] type          the event
##  @param[in] f             the callback function
##  @param[in] user_data     user data that will be passed into callback function `f`
## ***************************************************************************************
##

proc platform_set_evt_callback*(`type`: platform_evt_callback_type_t;
                               f: f_platform_evt_cb; user_data: pointer) {.
    importc: "platform_set_evt_callback", header: "platform_api.h".}
## *
## ***************************************************************************************
##  @brief register callback function for platform interrupt requests
##
##  @param[in] type          the irq
##  @param[in] f             the callback function
##  @param[in] user_data     user data that will be passed into callback function `f`
## ***************************************************************************************
##

proc platform_set_irq_callback*(`type`: platform_irq_callback_type_t;
                               f: f_platform_irq_cb; user_data: pointer) {.
    importc: "platform_set_irq_callback", header: "platform_api.h".}
## *
## ***************************************************************************************
##  @brief Get platform version number
##
##  @return                  version number
## ***************************************************************************************
##

proc platform_get_version*(): ptr platform_ver_t {.importc: "platform_get_version",
    header: "platform_api.h".}
## *
## ***************************************************************************************
##  @brief raise an assertion. This function will raise the assertion event:
##         `PLATFORM_CB_EVT_ASSERTION`
##
## ***************************************************************************************
##

proc platform_raise_assertion*(file_name: cstring; line_no: cint) {.
    importc: "platform_raise_assertion", header: "platform_api.h".}
type
  platform_heap_status_t* {.importc: "platform_heap_status_t",
                           header: "platform_api.h", bycopy.} = object
    bytes_free* {.importc: "bytes_free".}: uint32 ##  total free bytes
    bytes_minimum_ever_free* {.importc: "bytes_minimum_ever_free".}: uint32 ##  mininum of bytes_free from startup


## *
## ***************************************************************************************
##  @brief Get heap status
##
##  @param[out]  status              heap status
## ***************************************************************************************
##

proc platform_get_heap_status*(status: ptr platform_heap_status_t) {.
    importc: "platform_get_heap_status", header: "platform_api.h".}
## *
## ***************************************************************************************
##  @brief Reset platform.
##
##  Note: when calling this function, the code after it will not be executed.
## ***************************************************************************************
##

proc platform_reset*() {.importc: "platform_reset", header: "platform_api.h".}
## *
## ***************************************************************************************
##  @brief Switch to a secondary app.
##
##  @param[in] app_addr              app entry addr (i.e. ISR vector address)
## ***************************************************************************************
##

proc platform_switch_app*(app_addr: uint32) {.importc: "platform_switch_app",
    header: "platform_api.h".}
type
  platform_cfg_item_t* {.size: sizeof(cint).} = enum
    PLATFORM_CFG_LOG_HCI,     ##  flag is ENABLE or DISABLE. default: DISABLE
    PLATFORM_CFG_POWER_SAVING, ##  flag is ENABLE or DISABLE. default: DISABLE
    PLATFORM_CFG_TRACE_MASK   ##  flag is bitmap of platform_trace_item_t. default: 0


const
  PLATFORM_CFG_ENABLE* = 1
  PLATFORM_CFG_DISABLE* = 0

## *
## ***************************************************************************************
##  @brief platform configurations.
##
##  @param[in] item          Configuration item
##  @param[in] flag          see platform_cfg_item_t.
## ***************************************************************************************
##

proc platform_config*(item: platform_cfg_item_t; flag: uint32) {.
    importc: "platform_config", header: "platform_api.h".}
## *
## ***************************************************************************************
##  @brief Shutdown the whole system, and power on again after a duration
##         specified by duration_cycles.
##         Optionally, a portion of SYS memory can be retentioned during shutdown.
##
##  @param[in] duration_cycles       Duration before power on again (measured in cycles of 32k clock)
##                                   Mininum value: 825 cycles (about 25.18ms)
##  @param[in] p_retention_data      Pointer to the start of data to be retentioned
##  @param[in] data_size             Size of the data to be retentioned
## ***************************************************************************************
##

proc platform_shutdown*(duration_cycles: uint32; p_retention_data: pointer;
                       data_size: uint32) {.importc: "platform_shutdown",
    header: "platform_api.h".}
## *
## ***************************************************************************************
##  @brief generate random bytes by using true hardware random-number generator
##
##  @param[out] bytes            random data output
##  @param[in]  len              byte number of random data
## ***************************************************************************************
##
##  void platform_hrng(uint8_t *bytes, const uint32_t len);
##  WARNING: ^^^ this API is not available in this release
## *
## ***************************************************************************************
##  @brief the printf function
##
##  @param[in]  format           format string
##  @param[in]  ...              arguments
## ***************************************************************************************
##

proc platform_printf*(format: cstring) {.varargs, importc: "platform_printf",
                                      header: "platform_api.h".}
##  NOTE: for debug only

proc sysSetPublicDeviceAddr*(`addr`: ptr cuchar) {.
    importc: "sysSetPublicDeviceAddr", header: "platform_api.h".}
##  set rf source
##  0: use external crystal
##  1: use internal clock loopback

proc platform_set_rf_clk_source*(source: uint8) {.
    importc: "platform_set_rf_clk_source", header: "platform_api.h".}
proc platform_set_rf_init_data*(rf_init_data: ptr uint32) {.
    importc: "platform_set_rf_init_data", header: "platform_api.h".}
proc platform_set_rf_power_mapping*(rf_power_mapping: ptr int16) {.
    importc: "platform_set_rf_power_mapping", header: "platform_api.h".}
type
  coded_scheme_t* {.size: sizeof(cint).} = enum
    BLE_CODED_S8, BLE_CODED_S2


## *
## ***************************************************************************************
##  @brief set coded scheme of a advertising set
##         Note: this function should be called before the advertising set is enabled.
##               Default scheme for all advertising sets are S=8
##
##  @param[in]  adv_hdl          handle of the advertising set
##  @param[in]  scheme           selected scheme
## ***************************************************************************************
##

proc ll_set_adv_coded_scheme*(adv_hdl: uint8; scheme: coded_scheme_t) {.
    importc: "ll_set_adv_coded_scheme", header: "platform_api.h".}
## *
## ***************************************************************************************
##  @brief set coded scheme for initiating PDU
##         Note: this function should be called before initiating is enabled.
##
##  @param[in]  scheme           selected scheme (default S=8)
## ***************************************************************************************
##

proc ll_set_initiating_coded_scheme*(scheme: coded_scheme_t) {.
    importc: "ll_set_initiating_coded_scheme", header: "platform_api.h".}
## *
## ***************************************************************************************
##  @brief Give link layer (slave role) a hint on ce_len of a connection
##
##  @param[in]  conn_handle      handle of an existing connection
##  @param[in]  min_ce_len       information parameter about the minimum length of connection
##                               event needed for this LE connection.
##                               Range: 0x0000 – 0xFFFF
##                               Time = N * 0.625 ms.
##  @param[in]  max_ce_len       information parameter about the maximum length of connection
##                               event needed for this LE connection.
## ***************************************************************************************
##

proc ll_hint_on_ce_len*(conn_handle: uint16; min_ce_len: uint16; max_ce_len: uint16) {.
    importc: "ll_hint_on_ce_len", header: "platform_api.h".}