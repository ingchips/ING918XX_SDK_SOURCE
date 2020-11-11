import
  ingsoc

##
##  Description:
##  Bit shifts and widths for Control register 0 (SSPCR0)
##

const
  bsSSP_CONTROL0_CLOCKRATE* = 8
  bwSSP_CONTROL0_CLOCKRATE* = 0x000000FF
  bsSSP_CONTROL0_SCLKPHASE* = 7
  bwSSP_CONTROL0_SCLKPHASE* = 1
  bsSSP_CONTROL0_SCLKPOLARITY* = 6
  bwSSP_CONTROL0_SCLKPOLARITY* = 1
  bsSSP_CONTROL0_FRAMEFORMAT* = 4
  bwSSP_CONTROL0_FRAMEFORMAT* = 0x00000003
  bsSSP_CONTROL0_DATASIZE* = 0
  bwSSP_CONTROL0_DATASIZE* = 0x0000000F

##
##  Description:
##  Bit shifts and widths for Control register 1 (SSPCR1)
##

const
  bsSSP_CONTROL1_SLAVEOUTPUT* = 3
  bwSSP_CONTROL1_SLAVEOUTPUT* = 1
  bsSSP_CONTROL1_MASTERSLAVEMODE* = 2
  bwSSP_CONTROL1_MASTERSLAVEMODE* = 1
  bsSSP_CONTROL1_SSPENABLE* = 1
  bwSSP_CONTROL1_SSPENABLE* = 1
  bsSSP_CONTROL1_LOOPBACK* = 0
  bwSSP_CONTROL1_LOOPBACK* = 1

##
##  Description:
##  Bit shifts and widths for Status register
##

const
  bsSSP_STATUS_BUSY* = 4
  bwSSP_STATUS_BUSY* = 1
  bsSSP_STATUS_RXFULL* = 3
  bwSSP_STATUS_RXFULL* = 1
  bsSSP_STATUS_RXNOTEMPTY* = 2
  bwSSP_STATUS_RXNOTEMPTY* = 1
  bsSSP_STATUS_TXNOTFULL* = 1
  bwSSP_STATUS_TXNOTFULL* = 1
  bsSSP_STATUS_TXEMPTY* = 0
  bwSSP_STATUS_TXEMPTY* = 1

##
##  Description:
##  Bit shifts and widths for Clock Prescale register
##

const
  bsSSP_CLOCKPRESCALE_CLOCKDIVISOR* = 0
  bwSSP_CLOCKPRESCALE_CLOCKDIVISOR* = 0x000000FF

##
##  Description:
##  Bit shifts and widths for Interrupt Enables in Mask register
##

const
  bsSSP_MASK_TXINTENABLE* = 3
  bwSSP_MASK_TXINTENABLE* = 1
  bsSSP_MASK_RXINTENABLE* = 2
  bwSSP_MASK_RXINTENABLE* = 1
  bsSSP_MASK_RTMINTENABLE* = 1
  bwSSP_MASK_RTMINTENABLE* = 1
  bsSSP_MASK_RORINTENABLE* = 0
  bwSSP_MASK_RORINTENABLE* = 1

##
##  Description:
##  Bit shifts and widths for Raw and Masked Interrupt Status register
##

const
  bsSSP_INTERRUPTID_TXINT* = 3
  bwSSP_INTERRUPTID_TXINT* = 1
  bsSSP_INTERRUPTID_RXINT* = 2
  bwSSP_INTERRUPTID_RXINT* = 1
  bsSSP_INTERRUPTID_RTMINT* = 1
  bwSSP_INTERRUPTID_RTMINT* = 1
  bsSSP_INTERRUPTID_RORINT* = 0
  bwSSP_INTERRUPTID_RORINT* = 1

##
##  Descripton:
##  Bit shifts and widths for interrupt clear register
##

const
  bsSSP_INTERRUPTCLEAR_RTMINT* = 1
  bwSSP_INTERRUPTCLEAR_RTMINT* = 1
  bsSSP_INTERRUPTCLEAR_RORINT* = 0
  bwSSP_INTERRUPTCLEAR_RORINT* = 1

##
##  Descripton:
##  Bit shifts and widths for DMA Control Register
##

const
  bsSSP_DMA_TRANSMIT_ENABLE* = 1
  bwSSP_DMA_TRANSMIT_ENABLE* = 1
  bsSSP_DMA_RECEIVE_ENABLE* = 0
  bwSSP_DMA_RECEIVE_ENABLE* = 1

##
##  Description:
##  This specifies the frame format options
##

type
  apSSP_eFrameFormat* {.size: sizeof(cint).} = enum
    apSSP_FRAMEFORMAT_MOTOROLASPI = 0x00000000, ##  Motorola SPI frame format
    apSSP_FRAMEFORMAT_TISYNCHRONOUS = 0x00000001, ##  TI synchronous serial frame format
    apSSP_FRAMEFORMAT_NATIONALMICROWIRE = 0x00000002


##
##  Description:
##  This specifies the data size select options
##

type
  apSSP_eDataSize* {.size: sizeof(cint).} = enum
    apSSP_DATASIZE_4BITS = 0x00000003, ##   4 bit data
    apSSP_DATASIZE_5BITS = 0x00000004, ##   5 bit data
    apSSP_DATASIZE_6BITS = 0x00000005, ##   6 bit data
    apSSP_DATASIZE_7BITS = 0x00000006, ##   7 bit data
    apSSP_DATASIZE_8BITS = 0x00000007, ##   8 bit data
    apSSP_DATASIZE_9BITS = 0x00000008, ##   9 bit data
    apSSP_DATASIZE_10BITS = 0x00000009, ##  10 bit data
    apSSP_DATASIZE_11BITS = 0x0000000A, ##  11 bit data
    apSSP_DATASIZE_12BITS = 0x0000000B, ##  12 bit data
    apSSP_DATASIZE_13BITS = 0x0000000C, ##  13 bit data
    apSSP_DATASIZE_14BITS = 0x0000000D, ##  14 bit data
    apSSP_DATASIZE_15BITS = 0x0000000E, ##  15 bit data
    apSSP_DATASIZE_16BITS = 0x0000000F


##
##  Description:
##  This specifies the SCLK phase options. It is only applicable to
##  Motorola SPI frame format.
##

type
  apSSP_eSCLKPhase* {.size: sizeof(cint).} = enum
    apSSP_SCLKPHASE_TRAILINGEDGE = 0, ##  Data changes on trailing edge of the clock
    apSSP_SCLKPHASE_LEADINGEDGE = 1


##
##  Description:
##  This specifies the SCLK polarity options. It is only applicable to
##  Motorola SPI frame format.
##

type
  apSSP_eSCLKPolarity* {.size: sizeof(cint).} = enum
    apSSP_SCLKPOLARITY_IDLELOW = 0, ##  Clock low when idle and pulsed high
    apSSP_SCLKPOLARITY_IDLEHIGH = 1


##
##  Description:
##  This specifies the master/slave mode options
##

type
  apSSP_eMasterSlaveMode* {.size: sizeof(cint).} = enum
    apSSP_MASTER = 0,           ##  Device is Master
    apSSP_SLAVE = 1


##
##  Description:
##  This specifies the slave output mode options
##

type
  apSSP_eSlaveOutput* {.size: sizeof(cint).} = enum
    apSSP_SLAVEOUTPUTENABLED = 0, ##  SSP may drive output in slave mode
    apSSP_SLAVEOUTPUTDISABLED = 1


##
##  Description:
##  This specifies the loop back mode options
##

type
  apSSP_eLoopBackMode* {.size: sizeof(cint).} = enum
    apSSP_LOOPBACKOFF = 0,      ##  Loop back off, normal serial port operation
    apSSP_LOOPBACKON = 1


##
##  Description:
##  This specifies the DMA mode options.
##

type
  apSSP_eDMAMode* {.size: sizeof(cint).} = enum
    apSSP_DMA_TX_ON = 1, apSSP_DMA_TX_OFF, apSSP_DMA_RX_ON, apSSP_DMA_RX_OFF


##
##  Description:
##  The data structure to hold parameters to control SSP device.
##

type
  apSSP_sDeviceControlBlock* {.importc: "apSSP_sDeviceControlBlock",
                              header: "peripheral_ssp.h", bycopy.} = object
    ClockPrescale* {.importc: "ClockPrescale".}: uint8 ##  Clock rate prescale divisor
    ClockRate* {.importc: "ClockRate".}: uint8 ##  Clock rate divisor
    eSCLKPhase* {.importc: "eSCLKPhase".}: apSSP_eSCLKPhase
    eSCLKPolarity* {.importc: "eSCLKPolarity".}: apSSP_eSCLKPolarity
    eFrameFormat* {.importc: "eFrameFormat".}: apSSP_eFrameFormat
    eDataSize* {.importc: "eDataSize".}: apSSP_eDataSize
    eLoopBackMode* {.importc: "eLoopBackMode".}: apSSP_eLoopBackMode
    eMasterSlaveMode* {.importc: "eMasterSlaveMode".}: apSSP_eMasterSlaveMode
    eSlaveOutput* {.importc: "eSlaveOutput".}: apSSP_eSlaveOutput


## ==============================================================================================//

proc apSSP_Initialize*(SSP_Ptr: ptr SSP_TypeDef) {.importc: "apSSP_Initialize",
    header: "peripheral_ssp.h".}
proc apSSP_DeviceParametersSet*(SSP_Ptr: ptr SSP_TypeDef;
                               pParam: ptr apSSP_sDeviceControlBlock) {.
    importc: "apSSP_DeviceParametersSet", header: "peripheral_ssp.h".}
proc apSSP_DeviceParametersGet*(SSP_Ptr: ptr SSP_TypeDef;
                               pParam: ptr apSSP_sDeviceControlBlock) {.
    importc: "apSSP_DeviceParametersGet", header: "peripheral_ssp.h".}
proc apSSP_DeviceEnable*(SSP_Ptr: ptr SSP_TypeDef) {.importc: "apSSP_DeviceEnable",
    header: "peripheral_ssp.h".}
proc apSSP_GetIntRawStatus*(SSP_Ptr: ptr SSP_TypeDef): uint32 {.
    importc: "apSSP_GetIntRawStatus", header: "peripheral_ssp.h".}
proc apSSP_ClearInt*(SSP_Ptr: ptr SSP_TypeDef; bits: uint32) {.
    importc: "apSSP_ClearInt", header: "peripheral_ssp.h".}
##  void apSSP_DMAModeSet(SSP_TypeDef * SSP_Ptr, apSSP_eDMAMode DMAMode);
##  uint32_t apSSP_DMAAddressGet(uint32_t BaseAddr); // AHB_SPI0_BASE or APB_SPI1_BASE

proc apSSP_DeviceDisable*(SSP_Ptr: ptr SSP_TypeDef) {.
    importc: "apSSP_DeviceDisable", header: "peripheral_ssp.h".}
proc apSSP_DeviceReceiveEnable*(SSP_Ptr: ptr SSP_TypeDef) {.
    importc: "apSSP_DeviceReceiveEnable", header: "peripheral_ssp.h".}
proc apSSP_DeviceReceiveOverrunEnable*(SSP_Ptr: ptr SSP_TypeDef) {.
    importc: "apSSP_DeviceReceiveOverrunEnable", header: "peripheral_ssp.h".}
proc apSSP_DeviceReceiveDisable*(SSP_Ptr: ptr SSP_TypeDef) {.
    importc: "apSSP_DeviceReceiveDisable", header: "peripheral_ssp.h".}
proc apSSP_DeviceTransmitEnable*(SSP_Ptr: ptr SSP_TypeDef) {.
    importc: "apSSP_DeviceTransmitEnable", header: "peripheral_ssp.h".}
proc apSSP_DeviceTransmitDisable*(SSP_Ptr: ptr SSP_TypeDef) {.
    importc: "apSSP_DeviceTransmitDisable", header: "peripheral_ssp.h".}
proc apSSP_DeviceReceiveClear*(SSP_Ptr: ptr SSP_TypeDef) {.
    importc: "apSSP_DeviceReceiveClear", header: "peripheral_ssp.h".}
proc apSSP_DeviceBusyGet*(SSP_Ptr: ptr SSP_TypeDef): uint8 {.
    importc: "apSSP_DeviceBusyGet", header: "peripheral_ssp.h".}
proc apSSP_WriteFIFO*(SSP_Ptr: ptr SSP_TypeDef; data: uint16) {.
    importc: "apSSP_WriteFIFO", header: "peripheral_ssp.h".}
proc apSSP_ReadFIFO*(SSP_Ptr: ptr SSP_TypeDef): uint16 {.importc: "apSSP_ReadFIFO",
    header: "peripheral_ssp.h".}
proc apSSP_RxFifoFull*(SSP_Ptr: ptr SSP_TypeDef): uint8 {.
    importc: "apSSP_RxFifoFull", header: "peripheral_ssp.h".}
proc apSSP_RxFifoNotEmpty*(SSP_Ptr: ptr SSP_TypeDef): uint8 {.
    importc: "apSSP_RxFifoNotEmpty", header: "peripheral_ssp.h".}
proc apSSP_TxFifoNotFull*(SSP_Ptr: ptr SSP_TypeDef): uint8 {.
    importc: "apSSP_TxFifoNotFull", header: "peripheral_ssp.h".}
proc apSSP_TxFifoEmpty*(SSP_Ptr: ptr SSP_TypeDef): uint8 {.
    importc: "apSSP_TxFifoEmpty", header: "peripheral_ssp.h".}