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
  trace_uart_t* {.importc: "trace_uart_t", header: "trace.h", bycopy.} = object
    buffer* {.importc: "buffer".}: array[TRACE_BUFF_SIZE, uint8]
    write_next* {.importc: "write_next".}: uint16
    read_next* {.importc: "read_next".}: uint16
    handle* {.importc: "handle".}: TaskHandle_t
    tx_sem* {.importc: "tx_sem".}: SemaphoreHandle_t
    mutex* {.importc: "mutex".}: SemaphoreHandle_t
    port* {.importc: "port".}: ptr UART_TypeDef

  trace_rtt_t* {.importc: "trace_rtt_t", header: "trace.h", bycopy.} = object
    mutex* {.importc: "mutex".}: SemaphoreHandle_t


## *
## ***************************************************************************************
##  @brief Initialize UART trace
##
##  @param[in] trace_uart_t  trace context
## ***************************************************************************************
##

proc trace_uart_init*(ctx: ptr trace_uart_t) {.importc: "trace_uart_init",
    header: "trace.h".}
## *
## ***************************************************************************************
##  @brief Initialize RTT trace
##
##  @param[in] trace_rtt_t  trace context
## ***************************************************************************************
##

proc trace_rtt_init*(ctx: ptr trace_rtt_t) {.importc: "trace_rtt_init",
    header: "trace.h".}
## *
## ***************************************************************************************
##  @brief UART ISR callback
##
##  @param[in] trace_uart_t  trace context
## ***************************************************************************************
##

proc trace_uart_isr*(ctx: ptr trace_uart_t): uint32 {.importc: "trace_uart_isr",
    header: "trace.h".}
## *
## ***************************************************************************************
##  @brief Trace event callback
##
##  @param[in] trace         trace event
##  @param[in] trace_uart_t  trace context
## ***************************************************************************************
##

proc cb_trace_uart*(trace: ptr platform_evt_trace_t; ctx: ptr trace_uart_t): uint32 {.
    importc: "cb_trace_uart", header: "trace.h".}
## *
## ***************************************************************************************
##  @brief Trace event callback
##
##  @param[in] trace         trace event
##  @param[in] trace_rtt_t  trace context
## ***************************************************************************************
##

proc cb_trace_rtt*(trace: ptr platform_evt_trace_t; ctx: ptr trace_rtt_t): uint32 {.
    importc: "cb_trace_rtt", header: "trace.h".}
## *
## ***************************************************************************************
##  @brief Flush saved trace data to UART
##
##  @param[in] trace_uart_t  trace context
## ***************************************************************************************
##

proc trace_flush*(ctx: ptr trace_uart_t) {.importc: "trace_flush", header: "trace.h".}