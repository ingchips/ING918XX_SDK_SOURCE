import
  ingsoc

type
  f_uart_rx_byte* = proc (user_data: pointer; c: uint8) {.noconv.}

proc uart_driver_init*(port: ptr UART_TypeDef; user_data: pointer;
                      rx_byte_cb: f_uart_rx_byte) {.importc: "uart_driver_init",
    header: "uart_driver.h".}
proc driver_flush_tx*() {.importc: "driver_flush_tx", header: "uart_driver.h".}
proc driver_append_tx_data*(data: pointer; len: cint): uint32 {.
    importc: "driver_append_tx_data", header: "uart_driver.h".}
proc uart_driver_isr*(user_data: pointer): uint32 {.importc: "uart_driver_isr",
    header: "uart_driver.h".}