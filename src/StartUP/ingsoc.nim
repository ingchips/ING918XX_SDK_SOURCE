type                          ##  -------------------  Cortex-M3 Processor Exceptions Numbers  -------------------
  IRQn_Type* {.size: sizeof(cint).} = enum
    NonMaskableInt_IRQn = -14,  ## !<  2 Non Maskable Interrupt
    HardFault_IRQn = -13,       ## !<  3 HardFault Interrupt
    MemoryManagement_IRQn = -12, ## !<  4 Memory Management Interrupt
    BusFault_IRQn = -11,        ## !<  5 Bus Fault Interrupt
    UsageFault_IRQn = -10,      ## !<  6 Usage Fault Interrupt
    SVCall_IRQn = -5,           ## !< 11 SV Call Interrupt
    DebugMonitor_IRQn = -4,     ## !< 12 Debug Monitor Interrupt
    PendSV_IRQn = -2,           ## !< 14 Pend SV Interrupt
    SysTick_IRQn = -1           ## !< 15 System Tick Interrupt


##  ================================================================================
##  ================      Processor and Core Peripheral Section     ================
##  ================================================================================

##  ================================================================================
##  ================       Device Specific Peripheral Section       ================
##  ================================================================================

##  i2c

type
  I2C_RegDef* {.importc: "I2C_RegDef", header: "ingsoc.h", bycopy.} = object
    NRM* {.importc: "NRM".}: uint32 ##  normal access, "AS IS" operation
    SET* {.importc: "SET".}: uint32 ##  set    operation
    CLR* {.importc: "CLR".}: uint32 ##  clear  operation
    TOG* {.importc: "TOG".}: uint32 ##  toggle operation

  I2C_TypeDef* {.importc: "I2C_TypeDef", header: "ingsoc.h", bycopy.} = object
    I2C_CTRL0* {.importc: "I2C_CTRL0".}: I2C_RegDef ##  0x00
    I2C_TIMING0* {.importc: "I2C_TIMING0".}: I2C_RegDef
    I2C_TIMING1* {.importc: "I2C_TIMING1".}: I2C_RegDef
    I2C_TIMING2* {.importc: "I2C_TIMING2".}: I2C_RegDef
    I2C_CTRL1* {.importc: "I2C_CTRL1".}: I2C_RegDef ##  0x40
    I2C_STAT* {.importc: "I2C_STAT".}: uint32
    Reserved0* {.importc: "Reserved0".}: uint32
    Reserved1* {.importc: "Reserved1".}: uint32
    Reserved3* {.importc: "Reserved3".}: uint32
    I2C_QUEUECTRL* {.importc: "I2C_QUEUECTRL".}: I2C_RegDef
    I2C_QUEUESTAT* {.importc: "I2C_QUEUESTAT".}: uint32
    Reserved4* {.importc: "Reserved4".}: uint32
    Reserved5* {.importc: "Reserved5".}: uint32
    Reserved6* {.importc: "Reserved6".}: uint32
    I2C_QUEUECMD* {.importc: "I2C_QUEUECMD".}: I2C_RegDef ##  0x80
    I2C_QUEUEDATA* {.importc: "I2C_QUEUEDATA".}: uint32 ##  0x90
    Reserved7* {.importc: "Reserved7".}: uint32
    Reserved8* {.importc: "Reserved8".}: uint32
    Reserved9* {.importc: "Reserved9".}: uint32
    I2C_DATA* {.importc: "I2C_DATA".}: uint32 ##  0xa0
    Reserved10* {.importc: "Reserved10".}: uint32
    Reserved11* {.importc: "Reserved11".}: uint32
    Reserved12* {.importc: "Reserved12".}: uint32
    I2C_DEBUG0* {.importc: "I2C_DEBUG0".}: I2C_RegDef ##  0xb0
    I2C_DEBUG1* {.importc: "I2C_DEBUG1".}: I2C_RegDef
    I2C_VERSION* {.importc: "I2C_VERSION".}: uint32 ##  0xd0
    Reserved13* {.importc: "Reserved13".}: uint32
    Reserved14* {.importc: "Reserved14".}: uint32
    Reserved15* {.importc: "Reserved15".}: uint32


## uart

type
  UART_TypeDef* {.importc: "UART_TypeDef", header: "ingsoc.h", bycopy.} = object
    DataRead* {.importc: "DataRead".}: uint32 ##  0x00
    StatusClear* {.importc: "StatusClear".}: uint32 ##  0x04
    Reserved1* {.importc: "Reserved1".}: uint32 ##  0x08
    Reserved2* {.importc: "Reserved2".}: uint32 ##  0x0C
    Reserved3* {.importc: "Reserved3".}: uint32 ##  0x10
    Reserved4* {.importc: "Reserved4".}: uint32 ##  0x14
    Flag* {.importc: "Flag".}: uint32 ##  0x18
    Reserved5* {.importc: "Reserved5".}: uint32 ##  0x1C
    LowPower* {.importc: "LowPower".}: uint32 ##  0x20
    IntBaudDivisor* {.importc: "IntBaudDivisor".}: uint32 ##  0x24
    FractBaudDivisor* {.importc: "FractBaudDivisor".}: uint32 ##  0x28
    LineCon_H* {.importc: "LineCon_H".}: uint32 ##  0x2C
    Control* {.importc: "Control".}: uint32 ##  0x30
    FifoSelect* {.importc: "FifoSelect".}: uint32 ##  0x34
    IntMask* {.importc: "IntMask".}: uint32 ##  0x38
    IntRaw* {.importc: "IntRaw".}: uint32 ##  0x3C
    Interrupt* {.importc: "Interrupt".}: uint32 ##  0x40  // Masked Interrupt
    IntClear* {.importc: "IntClear".}: uint32 ##  0x44
    DmaCon* {.importc: "DmaCon".}: uint32 ##  0x48


##  timer

type
  TMR_TypeDef* {.importc: "TMR_TypeDef", header: "ingsoc.h", bycopy.} = object
    CNT* {.importc: "CNT".}: uint32
    CMP* {.importc: "CMP".}: uint32
    CTL* {.importc: "CTL".}: uint32
    LOCK* {.importc: "LOCK".}: uint32


## gpio

type
  GIO_RegDef* {.importc: "GIO_RegDef", header: "ingsoc.h", bycopy.} = object
    WRITE* {.importc: "WRITE".}: uint32
    SET* {.importc: "SET".}: uint32
    CLR* {.importc: "CLR".}: uint32
    MODIFY* {.importc: "MODIFY".}: uint32

  GIO_TypeDef* {.importc: "GIO_TypeDef", header: "ingsoc.h", bycopy.} = object
    DI* {.importc: "DI".}: uint32
    rev0* {.importc: "rev0".}: uint32
    rev1* {.importc: "rev1".}: uint32
    rev2* {.importc: "rev2".}: uint32
    DO* {.importc: "DO".}: GIO_RegDef
    OEB* {.importc: "OEB".}: GIO_RegDef
    rev3* {.importc: "rev3".}: GIO_RegDef
    IS* {.importc: "IS".}: GIO_RegDef
    IL* {.importc: "IL".}: GIO_RegDef
    IE1* {.importc: "IE1".}: GIO_RegDef
    IE0* {.importc: "IE0".}: GIO_RegDef


##  spi

type
  SSP_TypeDef* {.importc: "SSP_TypeDef", header: "ingsoc.h", bycopy.} = object
    ControlRegister0* {.importc: "ControlRegister0".}: uint32 ##  SSPCR0 Address SSP Base + 0x00
    ControlRegister1* {.importc: "ControlRegister1".}: uint32 ##  SSPCR1 Address SSP Base + 0x04
    DataRegister* {.importc: "DataRegister".}: uint32 ##  SSPDR Address SSP Base + 0x08
    StatusRegister* {.importc: "StatusRegister".}: uint32 ##  SSPSR Address SSP Base + 0x0C
    ClockPreScale* {.importc: "ClockPreScale".}: uint32 ##  SSPCPSR Address SSP Base + 0x10
    IntMask* {.importc: "IntMask".}: uint32 ##  SSPIMSC Interrupt mask set and clear register
    IntRawStatus* {.importc: "IntRawStatus".}: uint32 ##  SSPRIS Raw interrupt status register
    IntMaskedStatus* {.importc: "IntMaskedStatus".}: uint32 ##  SSPMIS Maked interrupt status register
    IntClearRegister* {.importc: "IntClearRegister".}: uint32 ##  SSPICR Interrupt clear register
    DMAControlReg* {.importc: "DMAControlReg".}: uint32 ##  SSPDMACR DMA control register


##  System Control

type
  SYSCTRL_RegDef* {.importc: "SYSCTRL_RegDef", header: "ingsoc.h", bycopy.} = object
    WRITE* {.importc: "WRITE".}: uint32
    SET* {.importc: "SET".}: uint32
    CLR* {.importc: "CLR".}: uint32
    MODIFY* {.importc: "MODIFY".}: uint32

  SYSCTRL_TypeDef* {.importc: "SYSCTRL_TypeDef", header: "ingsoc.h", bycopy.} = object
    Reversed* {.importc: "Reversed".}: uint32
    SYSCTRL_ClkGate* {.importc: "SYSCTRL_ClkGate".}: uint32
    RstSet* {.importc: "RstSet".}: uint32
    Ctrl0* {.importc: "Ctrl0".}: uint32
    RtcClkDet* {.importc: "RtcClkDet".}: uint32
    CacheCtrl* {.importc: "CacheCtrl".}: uint32
    CacheHitCtrl0* {.importc: "CacheHitCtrl0".}: uint32
    CacheHitCtrl1* {.importc: "CacheHitCtrl1".}: uint32
    CacheHitCtrl2* {.importc: "CacheHitCtrl2".}: uint32
    CacheHitCtrl3* {.importc: "CacheHitCtrl3".}: uint32
    CacheHitCtrl4* {.importc: "CacheHitCtrl4".}: uint32
    EflashModeCtrl* {.importc: "EflashModeCtrl".}: uint32


##  register access

template io_write_b*(a, d: untyped): untyped =
  (cast[ptr uint8]((a))[] = (d))

template io_read_b*(a: untyped): untyped =
  (cast[ptr uint8]((a))[])

template io_write*(a, d: untyped): untyped =
  (cast[ptr uint32]((a))[] = (d))

template io_read*(a: untyped): untyped =
  (cast[ptr uint32]((a))[])

##  Bits Width change to Mask Bits

template BW2M*(a: untyped): untyped =
  (1 shl a) - 1

## ****************************************************************************
##                          memory map
## ****************************************************************************
##  AHB

const
  APB_BASE* = (cast[uint32](0x40000000))
  AHB_SPI0_BASE* = (cast[uint32](0x40060000)) ##  SPI0 site in AHB BUS
  SYSCTRL_BASE* = (cast[uint32](0x40070000))
  BB_REG_BASE* = (cast[uint32](0x40090000))
  PWM_REG_BASE* = (cast[uint32](0x40011000))

##  APB

const
  APB_GIO_BASE* = (APB_BASE)    ## 0x00000  // gpio low  32
  APB_TMR0_BASE* = (APB_BASE + (1 shl 12)) ## 0x01000  // WatchDog
  APB_TMR1_BASE* = (APB_BASE + (2 shl 12)) ## 0x02000
  APB_TMR2_BASE* = (APB_BASE + (3 shl 12)) ## 0x03000
  APB_PINC_BASE* = (APB_BASE + (4 shl 12)) ## 0x04000
  APB_I2C0_BASE* = (APB_BASE + (1 shl 16)) ## 0x10000
  APB_I2C1_BASE* = (APB_BASE + 0x00012000)
  APB_SCI0_BASE* = (APB_BASE + (2 shl 16)) ## 0x20000
  APB_SCI1_BASE* = (APB_BASE + (2 shl 16) + (1 shl 12)) ## 0x21000
  APB_SPI1_BASE* = (APB_BASE + (3 shl 16)) ## 0x30000     // SPI1 site in APB BUS
  APB_RTC_BASE* = (APB_BASE + (4 shl 16)) ## 0x40000     // RTCDomain RTC

##  system control

const
  AHB_SYSCTRL* = (cast[ptr SYSCTRL_TypeDef](SYSCTRL_BASE))

##  i2c

const
  APB_I2C0* = (cast[ptr I2C_TypeDef](APB_I2C0_BASE))
  APB_I2C1* = (cast[ptr I2C_TypeDef](APB_I2C1_BASE))

##  uart

const
  APB_UART0* = (cast[ptr UART_TypeDef](APB_SCI0_BASE))
  APB_UART1* = (cast[ptr UART_TypeDef](APB_SCI1_BASE))

##  timer

const
  APB_TMR0* = (cast[ptr TMR_TypeDef](APB_TMR0_BASE))
  APB_TMR1* = (cast[ptr TMR_TypeDef](APB_TMR1_BASE))
  APB_TMR2* = (cast[ptr TMR_TypeDef](APB_TMR2_BASE))

##  spi

const
  AHB_SSP0* = (cast[ptr SSP_TypeDef](AHB_SPI0_BASE)) ##  Actually, APB_SPI0 site in AHB Bus.
  APB_SSP1* = (cast[ptr SSP_TypeDef](APB_SPI1_BASE))

##  Test if in interrupt mode

template IS_IN_INTERRUPT*(): untyped =
  ((SCB.ICSR and SCB_ICSR_VECTACTIVE_Msk) != 0)

when defined(USE_STDPERIPH_DRIVER):
  import
    peripheral_uart, peripheral_gpio, peripheral_sysctrl, peripheral_i2c,
    peripheral_pinctrl, peripheral_rtc, peripheral_ssp, peripheral_timer,
    peripheral_adc, peripheral_pwm

##  Clock Freq Define

when not defined(TARGET_FPGA):
  const
    PLL_CLK_FREQ* = 48000000
    OSC_CLK_FREQ* = 24000000
else:
  const
    PLL_CLK_FREQ* = 32000000
    OSC_CLK_FREQ* = 16000000
const
  RTC_CLK_FREQ* = 32768
