import
  FreeRTOS

import
  ingsoc, platform_api

when not defined(TRACE_BUFF_SIZE):
  const
    TRACE_BUFF_SIZE* = (4096)   ##  must be 2^n
const
  TRACE_BUFF_SIZE_MASK* = (TRACE_BUFF_SIZE - 1)

type
  trace_info_t* {.importc: "trace_info_t", header: "trace.h", bycopy.} = object
    buffer* {.importc: "buffer".}: array[TRACE_BUFF_SIZE, uint8]
    write_next* {.importc: "write_next".}: uint16
    read_next* {.importc: "read_next".}: uint16
    handle* {.importc: "handle".}: TaskHandle_t
    tx_sem* {.importc: "tx_sem".}: SemaphoreHandle_t
    mutex* {.importc: "mutex".}: SemaphoreHandle_t
    port* {.importc: "port".}: ptr UART_TypeDef


## *
## ***************************************************************************************
##  @brief Initialize UART trace
##
##  @param[in] trace_info_t  trace context
## ***************************************************************************************
##

proc trace_init*(ctx: ptr trace_info_t) {.importc: "trace_init", header: "trace.h".}
## *
## ***************************************************************************************
##  @brief UART ISR callback
##
##  @param[in] trace_info_t  trace context
## ***************************************************************************************
##

proc trace_uart_isr*(ctx: ptr trace_info_t): uint32 {.importc: "trace_uart_isr",
    header: "trace.h".}
## *
## ***************************************************************************************
##  @brief Trace event callback
##
##  @param[in] trace         trace event
##  @param[in] trace_info_t  trace context
## ***************************************************************************************
##

proc cb_trace*(trace: ptr platform_evt_trace_t; ctx: ptr trace_info_t): uint32 {.
    importc: "cb_trace", header: "trace.h".}
## *
## ***************************************************************************************
##  @brief Flush saved trace data to UART
##
##  @param[in] trace_info_t  trace context
## ***************************************************************************************
##

proc trace_flush*(ctx: ptr trace_info_t) {.importc: "trace_flush", header: "trace.h".}