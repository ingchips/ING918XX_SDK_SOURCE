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

const
  INGCHIPS_FAMILY_918            = 0
  INGCHIPS_FAMILY_916            = 1
  INGCHIPS_FAMILY_20             = 2

  INGCHIPS_FAMILY {.intdefine: "INGCHIPS_FAMILY"} = INGCHIPS_FAMILY_918

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

when INGCHIPS_FAMILY == INGCHIPS_FAMILY_918:
  const
    APB_UART0_BASE   =  (APB_BASE + 0x20000)
    APB_UART1_BASE   =  (APB_BASE + 0x21000)
elif INGCHIPS_FAMILY == INGCHIPS_FAMILY_916:
  const
    APB_UART0_BASE   =  (APB_BASE + 0x11000)
    APB_UART1_BASE   =  (APB_BASE + 0x12000)
elif INGCHIPS_FAMILY == INGCHIPS_FAMILY_20:
  const
    APB_UART0_BASE   =  (APB_BASE + 0x11000)
    APB_UART1_BASE   =  (APB_BASE + 0x12000)
else:
  {.error: "unknown chip family".}

const
  APB_SCI0_BASE* = (APB_BASE + (2 shl 16)) ## 0x20000
  APB_SCI1_BASE* = (APB_BASE + (2 shl 16) + (1 shl 12)) ## 0x21000

const
  APB_UART0* = (cast[ptr UART_TypeDef](APB_UART0_BASE))
  APB_UART1* = (cast[ptr UART_TypeDef](APB_UART1_BASE))

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
