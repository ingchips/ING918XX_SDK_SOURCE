import
  FreeRTOS, task, semphr, queue, platform_api

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


proc trace_init*(ctx: ptr trace_info_t) {.importc: "trace_init", header: "trace.h".}
proc trace_uart_isr*(ctx: ptr trace_info_t): uint32 {.importc: "trace_uart_isr",
    header: "trace.h".}
proc cb_trace*(trace: ptr platform_evt_trace_t; ctx: ptr trace_info_t): uint32 {.
    importc: "cb_trace", header: "trace.h".}