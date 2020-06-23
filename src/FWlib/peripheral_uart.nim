import
  ingsoc

##
##  Data read register UARTDR
##

const
  bwUART_DATA* = 8
  bsUART_DATA* = 0

##  Not used, left in for completeness
## #define bwUART_ERR_FRAME               1   /* Framing error */
## #define bwUART_ERR_PARITY              1   /* Parity error */
## #define bwUART_ERR_BREAK               1   /* Break error */
## #define bwUART_ERR_OVERRUN             1   /* Overrun error */
## #define bsUART_ERR_FRAME               8
## #define bsUART_ERR_PARITY              9
## #define bsUART_ERR_BREAK              10
## #define bsUART_ERR_OVERRUN            11
##  End of not used
##
##  Receive status / Error clear register UARTRSR/UARTECR
##

const
  bwUART_STAT_FRAME* = 1
  bwUART_STAT_PARITY* = 1
  bwUART_STAT_BREAK* = 1
  bwUART_STAT_OVERRUN* = 1
  bsUART_STAT_FRAME* = 0
  bsUART_STAT_PARITY* = 1
  bsUART_STAT_BREAK* = 2
  bsUART_STAT_OVERRUN* = 3

##
##  Flags register UARTFR
##

const
  bwUART_CLEAR_TO_SEND* = 1
  bwUART_READY* = 1
  bwUART_CARRIER* = 1
  bwUART_BUSY* = 1
  bwUART_RECEIVE_EMPTY* = 1
  bwUART_TRANSMIT_FULL* = 1
  bwUART_RECEIVE_FULL* = 1
  bwUART_TRANSMIT_EMPTY* = 1
  bwUART_RING* = 1
  bsUART_CLEAR_TO_SEND* = 0
  bsUART_READY* = 1
  bsUART_CARRIER* = 2
  bsUART_BUSY* = 3
  bsUART_RECEIVE_EMPTY* = 4
  bsUART_TRANSMIT_FULL* = 5
  bsUART_RECEIVE_FULL* = 6
  bsUART_TRANSMIT_EMPTY* = 7
  bsUART_RING* = 8

##
##  Low power divisor register UARTILPR
##

const
  bwUART_LP_DIVISOR* = 8

##
##  Baud rate divisor register
##

const
  bwUART_BAUD_DIVISOR* = 16

##
##  Line control register UARTLCR_H
##

const
  bwUART_SEND_BREAK* = 1
  bwUART_PARITY_SELECT* = 2
  bwUART_TWO_STOP_BITS* = 1
  bwUART_FIFO_ENABLE* = 1
  bwUART_WORD_LENGTH* = 2
  bwUART_STICK_PARITY* = 1
  bsUART_SEND_BREAK* = 0
  bsUART_PARITY_SELECT* = 1
  bsUART_TWO_STOP_BITS* = 3
  bsUART_FIFO_ENABLE* = 4
  bsUART_WORD_LENGTH* = 5
  bsUART_STICK_PARITY* = 7

##
##  Control register UARTCR
##

const
  bwUART_ENABLE* = 1
  bwUART_SIR_ENABLE* = 1
  bwUART_LOW_POWER_MODE* = 1
  bwUART_LOOP_BACK* = 1
  bsUART_ENABLE* = 0
  bsUART_SIR_ENABLE* = 1
  bsUART_LOW_POWER_MODE* = 2
  bsUART_LOOP_BACK* = 7
  bwUART_CONTROL_LOWBITS* = 3
  bwUART_CONTROL_HIGHBITS* = 9
  bwUART_TRANSMIT_ENABLE* = 1
  bwUART_RECEIVE_ENABLE* = 1
  bwUART_TRANSMIT_READY* = 1
  bwUART_REQUEST_SEND* = 1
  bwUART_OUT1* = 1
  bwUART_OUT2* = 1
  bwUART_HWFLOW_ENABLE* = 2
  bsUART_CONTROL_LOWBITS* = 0
  bsUART_CONTROL_HIGHBITS* = 7
  bsUART_TRANSMIT_ENABLE* = 8
  bsUART_RECEIVE_ENABLE* = 9
  bsUART_TRANSMIT_READY* = 10
  bsUART_REQUEST_SEND* = 11
  bsUART_OUT1* = 12
  bsUART_OUT2* = 13
  bsUART_HWFLOW_ENABLE* = 14
  bsUART_RTS_ENA* = 14
  bsUART_CTS_ENA* = 15

##
##  Interrupt identification / clear register UARTIIR/UARTICR
##

const
  bwUART_MODEMRI* = 1
  bwUART_MODEMCTS* = 1
  bwUART_MODEMDCD* = 1
  bwUART_MODEMDSR* = 1
  bwUART_RECEIVE* = 1
  bwUART_TRANSMIT* = 1
  bwUART_TIMEOUT* = 1
  bwUART_FRAME* = 1
  bwUART_PARITY* = 1
  bwUART_BREAK* = 1
  bwUART_OVERRUN* = 1
  bwUART_MODEM* = 4

##  Not used, left in for completeness
## #define bwUART_ERROR                   4   /* All Error interrupts */

const
  bwUART_ALL* = 11
  bsUART_MODEMRI* = 0
  bsUART_MODEMCTS* = 1
  bsUART_MODEMDCD* = 2
  bsUART_MODEMDSR* = 3
  bsUART_RECEIVE* = 4
  bsUART_TRANSMIT* = 5
  bsUART_TIMEOUT* = 6
  bsUART_FRAME* = 7
  bsUART_PARITY* = 8
  bsUART_BREAK* = 9
  bsUART_OVERRUN* = 10
  bsUART_MODEM* = 0

##  Not used, left in for completeness
## #define bsUART_ERROR                   7

const
  bsUART_ALL* = 0

##  Interrupt Mask
##  PL_011 has seperate mask set/clear register UARTIMSC

const
  bwUART_MODEM_INTENAB* = 4
  bwUART_RECEIVE_INTENAB* = 1
  bwUART_TRANSMIT_INTENAB* = 1
  bwUART_TIMEOUT_INTENAB* = 1
  bwUART_ERROR_INTENAB* = 4
  bwUART_FRAME_INTENAB* = 1
  bwUART_PARITY_INTENAB* = 1
  bwUART_BREAK_INTENAB* = 1
  bwUART_OVERRUN_INTENAB* = 1
  bwUART_ALL_INTENAB* = 11

## p64

const
  bsUART_MODEM_INTENAB* = 0
  bsUART_RECEIVE_INTENAB* = 4
  bsUART_TRANSMIT_INTENAB* = 5
  bsUART_TIMEOUT_INTENAB* = 6
  bsUART_ERROR_INTENAB* = 7
  bsUART_FRAME_INTENAB* = 7
  bsUART_PARITY_INTENAB* = 8
  bsUART_BREAK_INTENAB* = 9
  bsUART_OVERRUN_INTENAB* = 10
  bsUART_ALL_INTENAB* = 0
  UART_RECEIVE_INTCLR* = 4

##  Used by INTTEST within ISR

const
  UART_INTBIT_RECEIVE* = 0x00000010
  UART_INTBIT_TRANSMIT* = 0x00000020
  UART_INTBIT_TIMEOUT* = 0x00000040
  UART_INTBIT_MODEM* = 0x0000000F
  UART_INTBIT_MODEMRI* = 0x00000001
  UART_INTBIT_MODEMCTS* = 0x00000002
  UART_INTBIT_MODEMDCD* = 0x00000004
  UART_INTBIT_MODEMDSR* = 0x00000008
  UART_INTBIT_ERROR* = 0x00000780
  UART_INTBIT_FRAME* = 0x00000080
  UART_INTBIT_PARITY* = 0x00000100
  UART_INTBIT_BREAK* = 0x00000200
  UART_INTBIT_OVERRUN* = 0x00000400

##
##  Descripton:
##  Bit shifts and widths for DMA Control Register UARTDMACR
##

const
  bsUART_DMA_ON_ERROR_DISABLE* = 2
  bwUART_DMA_ON_ERROR_DISABLE* = 1
  bsUART_DMA_TRANSMIT_ENABLE* = 1
  bwUART_DMA_TRANSMIT_ENABLE* = 1
  bsUART_DMA_RECEIVE_ENABLE* = 0
  bwUART_DMA_RECEIVE_ENABLE* = 1

##
##  FIFO select register UARTIFLS
##

const
  bwUART_TRANS_INT_LEVEL* = 3
  bwUART_RECV_INT_LEVEL* = 3
  bsUART_TRANS_INT_LEVEL* = 0
  bsUART_RECV_INT_LEVEL* = 8

type
  UART_eWLEN* {.size: sizeof(cint).} = enum
    UART_WLEN_5_BITS = 0, UART_WLEN_6_BITS = 1, UART_WLEN_7_BITS = 2,
    UART_WLEN_8_BITS = 3


##  SPS EPS PEN

type
  UART_ePARITY* {.size: sizeof(cint).} = enum
    UART_PARITY_NOT_CHECK = 0, UART_PARITY_ODD_PARITY = 1,
    UART_PARITY_EVEN_PARITY = 3, UART_PARITY_FIX_ONE = 5, UART_PARITY_FIX_ZERO = 7


## p63

type
  UART_eFIFO_WATERLEVEL* {.size: sizeof(cint).} = enum
    UART_FIFO_ONE_EIGHTH = 0, UART_FIFO_ONE_FOURTH = 1, UART_FIFO_ONE_SECOND = 2,
    UART_FIFO_THREE_FOURTH = 3, UART_FIFO_SEVEN_EIGHTH = 4


##
##  UART status
##

type
  UART_sStateStruct* {.importc: "UART_sStateStruct", header: "peripheral_uart.h",
                      bycopy.} = object
    word_length* {.importc: "word_length".}: UART_eWLEN ##  Line Control Register, UARTLCR_H
    ##  WLEN
    parity* {.importc: "parity".}: UART_ePARITY ##  PEN, EPS, SPS
    fifo_enable* {.importc: "fifo_enable".}: uint8 ##  FEN
    two_stop_bits* {.importc: "two_stop_bits".}: uint8 ##  STP2
                                                   ##  Control Register, UARTCR
    receive_en* {.importc: "receive_en".}: uint8 ##  RXE
    transmit_en* {.importc: "transmit_en".}: uint8 ##  TXE
    UART_en* {.importc: "UART_en".}: uint8 ##  UARTEN
    cts_en* {.importc: "cts_en".}: uint8 ## CTSEN
    rts_en* {.importc: "rts_en".}: uint8 ## RTSEN
                                     ##  Interrupt FIFO Level Select Register, UARTIFLS
    rxfifo_waterlevel* {.importc: "rxfifo_waterlevel".}: uint8 ##  RXIFLSEL
    txfifo_waterlevel* {.importc: "txfifo_waterlevel".}: uint8 ##  TXIFLSEL
                                                           ## UART_eFIFO_WATERLEVEL    rxfifo_waterlevel; // RXIFLSEL
                                                           ## UART_eFIFO_WATERLEVEL    txfifo_watchlevel; // TXIFLSEL
                                                           ##  UART Clock Frequency
    ClockFrequency* {.importc: "ClockFrequency".}: uint32
    BaudRate* {.importc: "BaudRate".}: uint32


proc apUART_BaudRateSet*(pBase: ptr UART_TypeDef; ClockFrequency: uint32;
                        BaudRate: uint32) {.importc: "apUART_BaudRateSet",
    header: "peripheral_uart.h".}
proc apUART_BaudRateGet*(pBase: ptr UART_TypeDef; ClockFrequency: uint32): uint32 {.
    importc: "apUART_BaudRateGet", header: "peripheral_uart.h".}
proc apUART_Check_Rece_ERROR*(pBase: ptr UART_TypeDef): uint8 {.
    importc: "apUART_Check_Rece_ERROR", header: "peripheral_uart.h".}
proc apUART_Check_RXFIFO_EMPTY*(pBase: ptr UART_TypeDef): uint8 {.
    importc: "apUART_Check_RXFIFO_EMPTY", header: "peripheral_uart.h".}
proc apUART_Check_RXFIFO_FULL*(pBase: ptr UART_TypeDef): uint8 {.
    importc: "apUART_Check_RXFIFO_FULL", header: "peripheral_uart.h".}
proc apUART_Check_TXFIFO_EMPTY*(pBase: ptr UART_TypeDef): uint8 {.
    importc: "apUART_Check_TXFIFO_EMPTY", header: "peripheral_uart.h".}
proc apUART_Check_TXFIFO_FULL*(pBase: ptr UART_TypeDef): uint8 {.
    importc: "apUART_Check_TXFIFO_FULL", header: "peripheral_uart.h".}
proc apUART_Enable_TRANSMIT_INT*(pBase: ptr UART_TypeDef) {.
    importc: "apUART_Enable_TRANSMIT_INT", header: "peripheral_uart.h".}
proc apUART_Disable_TRANSMIT_INT*(pBase: ptr UART_TypeDef) {.
    importc: "apUART_Disable_TRANSMIT_INT", header: "peripheral_uart.h".}
proc apUART_Enable_RECEIVE_INT*(pBase: ptr UART_TypeDef) {.
    importc: "apUART_Enable_RECEIVE_INT", header: "peripheral_uart.h".}
proc apUART_Disable_RECEIVE_INT*(pBase: ptr UART_TypeDef) {.
    importc: "apUART_Disable_RECEIVE_INT", header: "peripheral_uart.h".}
proc apUART_Clr_RECEIVE_INT*(pBase: ptr UART_TypeDef) {.
    importc: "apUART_Clr_RECEIVE_INT", header: "peripheral_uart.h".}
proc apUART_Clr_TX_INT*(pBase: ptr UART_TypeDef) {.importc: "apUART_Clr_TX_INT",
    header: "peripheral_uart.h".}
proc apUART_Clr_NonRx_INT*(pBase: ptr UART_TypeDef) {.
    importc: "apUART_Clr_NonRx_INT", header: "peripheral_uart.h".}
proc apUART_Get_ITStatus*(pBase: ptr UART_TypeDef; UART_IT: uint8): uint8 {.
    importc: "apUART_Get_ITStatus", header: "peripheral_uart.h".}
proc apUART_Get_all_raw_int_stat*(pBase: ptr UART_TypeDef): uint32 {.
    importc: "apUART_Get_all_raw_int_stat", header: "peripheral_uart.h".}
proc apUART_Check_BUSY*(pBase: ptr UART_TypeDef): uint8 {.
    importc: "apUART_Check_BUSY", header: "peripheral_uart.h".}
proc apUART_Initialize*(pBase: ptr UART_TypeDef; UARTx: ptr UART_sStateStruct;
                       IntMask: uint32) {.importc: "apUART_Initialize",
                                        header: "peripheral_uart.h".}
proc UART_SendData*(pBase: ptr UART_TypeDef; Data: uint8) {.importc: "UART_SendData",
    header: "peripheral_uart.h".}
proc UART_ReceData*(pBase: ptr UART_TypeDef): uint8 {.importc: "UART_ReceData",
    header: "peripheral_uart.h".}
proc uart_reset*(pBase: ptr UART_TypeDef) {.importc: "uart_reset",
                                        header: "peripheral_uart.h".}