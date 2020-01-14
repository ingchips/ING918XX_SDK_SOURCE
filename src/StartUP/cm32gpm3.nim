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
    SysTick_IRQn = -1,          ## !< 15 System Tick Interrupt
                    ## *****  CM32GPM3 specific Interrupt Numbers ********************************************************
    n00_BB0_IRQn = 0, n01_BB1_IRQn = 1, n02_RTC_M0_IRQn = 2, n03_RTC_M1_IRQn = 3,
    n04_TMR0_IRQn = 4, n05_TMR1_IRQn = 5, n06_TMR2_IRQn = 6, n07_DMA_IRQn = 7,
    n08_EXINT_IRQn = 8, n09_GPIO_IRQn = 9, n10_BB0_IRQn = 10, n11_BB1_IRQn = 11,
    n12_DMA_IRQn = 12, n13_TMR1_IRQn = 13, n14_SPI0_IRQn = 14, n15_SPI1_IRQn = 15,
    n16_URT0_IRQn = 16, n17_URT1_IRQn = 17, n18_I2C_IRQn = 18, n19_DMA_IRQn = 19,
    n20_BB0_IRQn = 20, n21_BB1_IRQn = 21, n22_TMR2_IRQn = 22, n23_SPI1_IRQn = 23,
    n24_EXINT_IRQn = 24, n25_GPIO_IRQn = 25, n26_I2C_IRQn = 26, n27_URT0_IRQn = 27,
    n28_URT1_IRQn = 28


##  ================================================================================
##  ================      Processor and Core Peripheral Section     ================
##  ================================================================================

## /* --------  Configuration of the Cortex-M4 Processor and Core Peripherals  ------- */
const
  CM3_REV* = 0x0201
  MPU_PRESENT* = 1
  NVIC_PRIO_BITS* = 3
  Vendor_SysTickConfig* = 0

##  ================================================================================
##  ================       Device Specific Peripheral Section       ================
##  ================================================================================

##  i2c

type
  I2C_RegDef* {.importc: "I2C_RegDef", header: "cm32gpm3.h", bycopy.} = object
    NRM* {.importc: "NRM".}: uint32 ##  normal access, "AS IS" operation
    SET* {.importc: "SET".}: uint32 ##  set    operation
    CLR* {.importc: "CLR".}: uint32 ##  clear  operation
    TOG* {.importc: "TOG".}: uint32 ##  toggle operation

  I2C_TypeDef* {.importc: "I2C_TypeDef", header: "cm32gpm3.h", bycopy.} = object
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
  UART_TypeDef* {.importc: "UART_TypeDef", header: "cm32gpm3.h", bycopy.} = object
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
  TMR_TypeDef* {.importc: "TMR_TypeDef", header: "cm32gpm3.h", bycopy.} = object
    CNT* {.importc: "CNT".}: uint32
    CMP* {.importc: "CMP".}: uint32
    CTL* {.importc: "CTL".}: uint32
    LOCK* {.importc: "LOCK".}: uint32


## gpio

type
  GIO_RegDef* {.importc: "GIO_RegDef", header: "cm32gpm3.h", bycopy.} = object
    WRITE* {.importc: "WRITE".}: uint32
    SET* {.importc: "SET".}: uint32
    CLR* {.importc: "CLR".}: uint32
    MODIFY* {.importc: "MODIFY".}: uint32

  GIO_TypeDef* {.importc: "GIO_TypeDef", header: "cm32gpm3.h", bycopy.} = object
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
  SSP_TypeDef* {.importc: "SSP_TypeDef", header: "cm32gpm3.h", bycopy.} = object
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


##  dma

type
  DMA_ChDef* {.importc: "DMA_ChDef", header: "cm32gpm3.h", bycopy.} = object
    ChSrcAddr* {.importc: "ChSrcAddr".}: uint32
    ChDestAddr* {.importc: "ChDestAddr".}: uint32
    ChLLI* {.importc: "ChLLI".}: uint32
    ChControl* {.importc: "ChControl".}: uint32
    ChConfiguration* {.importc: "ChConfiguration".}: uint32
    Reserved1* {.importc: "Reserved1".}: uint32
    Reserved2* {.importc: "Reserved2".}: uint32
    Reserved3* {.importc: "Reserved3".}: uint32


const
  DMA_NUM_RSRVD_WRDS_BEFORE_CHANNELS* = ((0x00000100 - 0x00000038) shr 2)
  DMA_NUM_RSRVD_WRDS_BEFORE_PERIPHERAL_ID* = ((0x00000FE0 - 0x000001F4) shr 2)

type
  DMA_TypeDef* {.importc: "DMA_TypeDef", header: "cm32gpm3.h", bycopy.} = object
    IntStatus* {.importc: "IntStatus".}: uint32 ##  0x000
                                            ##  RO
    IntTCStatus* {.importc: "IntTCStatus".}: uint32 ##  RO
    IntTCClear* {.importc: "IntTCClear".}: uint32 ##  WO
    IntErrorStatus* {.importc: "IntErrorStatus".}: uint32 ##  RO
    IntErrorClear* {.importc: "IntErrorClear".}: uint32 ##  0x010
                                                    ##  WO
    RawIntTCStatus* {.importc: "RawIntTCStatus".}: uint32 ##  RO
    RawIntErrorStatus* {.importc: "RawIntErrorStatus".}: uint32 ##  RO
    ActiveChannels* {.importc: "ActiveChannels".}: uint32 ##  RO, A bit is cleared on completion of the DMA transfer.
    SoftBReq* {.importc: "SoftBReq".}: uint32 ##  0x020
                                          ##  RW
    SoftSReq* {.importc: "SoftSReq".}: uint32 ##  0x024
                                          ##  RW
    SoftLBReq* {.importc: "SoftLBReq".}: uint32 ##  0x028
                                            ##  RW
    SoftSBReq* {.importc: "SoftSBReq".}: uint32 ##  0x02C
                                            ##  RW
    Configuration* {.importc: "Configuration".}: uint32 ##  0x030
                                                    ##  RW
    Sync* {.importc: "Sync".}: uint32 ##  0x034
                                  ##  RW
    Reserved1* {.importc: "Reserved1".}: array[DMA_NUM_RSRVD_WRDS_BEFORE_CHANNELS,
        uint32]               ##  RW
    ChReg* {.importc: "ChReg".}: array[7, DMA_ChDef] ##  0x100 -
    Reserved2* {.importc: "Reserved2".}: array[
        DMA_NUM_RSRVD_WRDS_BEFORE_PERIPHERAL_ID, uint32]
    PeripheralId0* {.importc: "PeripheralId0".}: uint32 ##  0xFE0
    PeripheralId1* {.importc: "PeripheralId1".}: uint32
    PeripheralId2* {.importc: "PeripheralId2".}: uint32
    PeripheralId3* {.importc: "PeripheralId3".}: uint32
    CellId0* {.importc: "CellId0".}: uint32 ##  0xFF0
    CellId1* {.importc: "CellId1".}: uint32
    CellId2* {.importc: "CellId2".}: uint32
    CellId3* {.importc: "CellId3".}: uint32


##  System Control

type
  SYSCTRL_RegDef* {.importc: "SYSCTRL_RegDef", header: "cm32gpm3.h", bycopy.} = object
    WRITE* {.importc: "WRITE".}: uint32
    SET* {.importc: "SET".}: uint32
    CLR* {.importc: "CLR".}: uint32
    MODIFY* {.importc: "MODIFY".}: uint32

  SYSCTRL_TypeDef* {.importc: "SYSCTRL_TypeDef", header: "cm32gpm3.h", bycopy.} = object
    SYSCTRL_SysClkSwitch* {.importc: "SYSCTRL_SysClkSwitch".}: uint8 ##  0x00
    SYSCTRL_CWClkSwitch* {.importc: "SYSCTRL_CWClkSwitch".}: uint8
    SYSCTRL_ApbClkSwitch* {.importc: "SYSCTRL_ApbClkSwitch".}: uint8
    SYSCTRL_PPSClkSwitch* {.importc: "SYSCTRL_PPSClkSwitch".}: uint8 ##  0x04
    SYSCTRL_SysClkDiv* {.importc: "SYSCTRL_SysClkDiv".}: uint8
    SYSCTRL_CWClkDiv* {.importc: "SYSCTRL_CWClkDiv".}: uint8
    SYSCTRL_ApbClkDiv* {.importc: "SYSCTRL_ApbClkDiv".}: uint8
    SYSCTRL_PPSClkDiv* {.importc: "SYSCTRL_PPSClkDiv".}: uint8 ##  0x08
    Reversed1* {.importc: "Reversed1".}: uint32 ##  0x0c
    Reversed2* {.importc: "Reversed2".}: uint32 ##  0x10 - 0x14 - 0x18 - 0x1c
    SYSCTRL_ClkGate* {.importc: "SYSCTRL_ClkGate".}: SYSCTRL_RegDef ##  0x20 - 0x24 - 0x28 - 0x2c
    SYSCTRL_BlockRst* {.importc: "SYSCTRL_BlockRst".}: SYSCTRL_RegDef ##  0x30
    SYSCTRL_SoftwareRst* {.importc: "SYSCTRL_SoftwareRst".}: uint32 ##  0x34, RO
    SYSCTRL_RtcClkCount* {.importc: "SYSCTRL_RtcClkCount".}: uint32 ##  0x38
    SYSCTRL_PLLParamONE_N* {.importc: "SYSCTRL_PLLParamONE_N".}: uint8
    SYSCTRL_PLLParamONE_M* {.importc: "SYSCTRL_PLLParamONE_M".}: uint8
    SYSCTRL_PLLParamONE_OD* {.importc: "SYSCTRL_PLLParamONE_OD".}: uint8
    Reversed3* {.importc: "Reversed3".}: uint8 ##  0x3c
    SYSCTRL_PLLParamTWO_RST* {.importc: "SYSCTRL_PLLParamTWO_RST".}: uint8
    SYSCTRL_PLLParamTWO_BP* {.importc: "SYSCTRL_PLLParamTWO_BP".}: uint8
    SYSCTRL_PLLParamTWO_LKDT* {.importc: "SYSCTRL_PLLParamTWO_LKDT".}: uint8
    Reversed4* {.importc: "Reversed4".}: uint8


##  Pwrc

type
  PWRCCTRL_TypeDef* {.importc: "PWRCCTRL_TypeDef", header: "cm32gpm3.h", bycopy.} = object
    TPO* {.importc: "TPO".}: uint32 ##  0x00
    TPD* {.importc: "TPD".}: uint32 ##  0x04
    TAO* {.importc: "TAO".}: uint32 ##  0x08
    TAD* {.importc: "TAD".}: uint32 ##  0x0C
    TTO* {.importc: "TTO".}: uint32 ##  0x10
    TTD* {.importc: "TTD".}: uint32 ##  0x14
    DP_SLP* {.importc: "DP_SLP".}: uint32 ##  0x18
    WRM_BOOT* {.importc: "WRM_BOOT".}: uint32 ##  0x1C
    AE* {.importc: "AE".}: uint32 ##  0x20
    AE_STAT* {.importc: "AE_STAT".}: uint32 ##  0x24
    TE* {.importc: "TE".}: uint32 ##  0x28
    TE_STAT* {.importc: "TE_STAT".}: uint32 ##  0x2C
    UART_EN* {.importc: "UART_EN".}: uint32 ##  0x30
    RTC_M0_EN* {.importc: "RTC_M0_EN".}: uint32 ##  0x34
    RTC_M0_HP* {.importc: "RTC_M0_HP".}: uint32 ##  0x38
    RTC_M1_EN* {.importc: "RTC_M1_EN".}: uint32 ##  0x3C
    RTC_M1_HP* {.importc: "RTC_M1_HP".}: uint32 ##  0x40
    EXINT_CTRL* {.importc: "EXINT_CTRL".}: uint32 ##  0x44
    EXINT_HP* {.importc: "EXINT_HP".}: uint32 ##  0x48
    EXINT_TP* {.importc: "EXINT_TP".}: uint32 ##  0x4C
    EXINT_CL* {.importc: "EXINT_CL".}: uint32 ##  0x50
    Reserved1* {.importc: "Reserved1".}: uint32 ##  0x54
    Reserved2* {.importc: "Reserved2".}: uint32 ##  0x58
    Reserved3* {.importc: "Reserved3".}: uint32 ##  0x5C
    TM* {.importc: "TM".}: uint32 ##  0x60
    Reserved4* {.importc: "Reserved4".}: uint32 ##  0x64
    Reserved5* {.importc: "Reserved5".}: uint32 ##  0x68
    Reserved6* {.importc: "Reserved6".}: uint32 ##  0x6C
    NVREG0* {.importc: "NVREG0".}: uint32 ##  0x70
    NVREG1* {.importc: "NVREG1".}: uint32
    NVREG2* {.importc: "NVREG2".}: uint32
    NVREG3* {.importc: "NVREG3".}: uint32
    NVREG4* {.importc: "NVREG4".}: uint32
    NVREG5* {.importc: "NVREG5".}: uint32
    NVREG6* {.importc: "NVREG6".}: uint32
    NVREG7* {.importc: "NVREG7".}: uint32


##  Apb Pin Control

type
  PINC_RegDef* {.importc: "PINC_RegDef", header: "cm32gpm3.h", bycopy.} = object
    PadCtrl* {.importc: "PadCtrl".}: uint8
    FuncSel* {.importc: "FuncSel".}: uint8
    Reserved1* {.importc: "Reserved1".}: uint8
    Reserved2* {.importc: "Reserved2".}: uint8

  PINC_TypeDef* {.importc: "PINC_TypeDef", header: "cm32gpm3.h", bycopy.} = object
    RF_CLK* {.importc: "RF_CLK".}: PINC_RegDef ##   0
    RF_CHN0_4* {.importc: "RF_CHN0_4".}: PINC_RegDef ##   1
    RF_CHN0_3* {.importc: "RF_CHN0_3".}: PINC_RegDef ##   2
    RF_CHN0_2* {.importc: "RF_CHN0_2".}: PINC_RegDef ##   3
    RF_CHN0_1* {.importc: "RF_CHN0_1".}: PINC_RegDef ##   4
    RF_CHN0_0* {.importc: "RF_CHN0_0".}: PINC_RegDef ##   5
    RF_CHN1_4* {.importc: "RF_CHN1_4".}: PINC_RegDef ##   6
    RF_CHN1_3* {.importc: "RF_CHN1_3".}: PINC_RegDef ##   7
    RF_CHN1_2* {.importc: "RF_CHN1_2".}: PINC_RegDef ##   8
    RF_CHN1_1* {.importc: "RF_CHN1_1".}: PINC_RegDef ##   9
    RF_CHN1_0* {.importc: "RF_CHN1_0".}: PINC_RegDef ##  10
    RF_CHN2_4* {.importc: "RF_CHN2_4".}: PINC_RegDef ##  11
    RF_CHN2_3* {.importc: "RF_CHN2_3".}: PINC_RegDef ##  12
    RF_CHN2_2* {.importc: "RF_CHN2_2".}: PINC_RegDef ##  13
    RF_CHN2_1* {.importc: "RF_CHN2_1".}: PINC_RegDef ##  14
    RF_CHN2_0* {.importc: "RF_CHN2_0".}: PINC_RegDef ##  15
    UART_RX0* {.importc: "UART_RX0".}: PINC_RegDef ##  16
    UART_TX0* {.importc: "UART_TX0".}: PINC_RegDef ##  17
    UART_RX1* {.importc: "UART_RX1".}: PINC_RegDef ##  18
    UART_TX1* {.importc: "UART_TX1".}: PINC_RegDef ##  19
    SCL* {.importc: "SCL".}: PINC_RegDef ##  20
    SDA* {.importc: "SDA".}: PINC_RegDef ##  21
    SS0* {.importc: "SS0".}: PINC_RegDef ##  22
    SCLK1* {.importc: "SCLK1".}: PINC_RegDef ##  23
    MOSI1* {.importc: "MOSI1".}: PINC_RegDef ##  24
    MISO1* {.importc: "MISO1".}: PINC_RegDef ##  25
    SS1* {.importc: "SS1".}: PINC_RegDef ##  26
    GPIO_0* {.importc: "GPIO_0".}: PINC_RegDef ##  27
    GPIO_1* {.importc: "GPIO_1".}: PINC_RegDef ##  28
    GPIO_2* {.importc: "GPIO_2".}: PINC_RegDef ##  29
    GPIO_3* {.importc: "GPIO_3".}: PINC_RegDef ##  30
    GPIO_4* {.importc: "GPIO_4".}: PINC_RegDef ##  31
    GPIO_5* {.importc: "GPIO_5".}: PINC_RegDef ##  32
    PPS* {.importc: "PPS".}: PINC_RegDef ##  33
    WP* {.importc: "WP".}: PINC_RegDef ##  34
    HOLDN* {.importc: "HOLDN".}: PINC_RegDef ##  35
    SCLK0* {.importc: "SCLK0".}: PINC_RegDef ##  36
    MOSI0* {.importc: "MOSI0".}: PINC_RegDef ##  37
    MISO0* {.importc: "MISO0".}: PINC_RegDef ##  38


## ****************************************************************************
##                          memory map
## ****************************************************************************
##  AHB

const
  ROM_BASE* = (cast[uint32](0x00000000))
  ITCM_BASE* = (cast[uint32](0x00010000))
  SRAM_MIRROR_BASE* = (cast[uint32](0x00060000))
  SRAM_BASE* = (cast[uint32](0x20000000))
  ROM_MIRROR_BASE* = (cast[uint32](0x21000000))
  ITCM_MIRROR_BASE* = (cast[uint32](0x21010000))
  APB_BASE* = (cast[uint32](0x40000000))
  AHB_SPI0_BASE* = (cast[uint32](0x40060000)) ##  SPI0 site in AHB BUS
  SYSCTRL_BASE* = (cast[uint32](0x40070000))
  DMA_BASE* = (cast[uint32](0x40080000))
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
  APB_PWRC_BASE* = APB_RTC_BASE

##  dma

const
  AHB_DMA* = (cast[ptr DMA_TypeDef](DMA_BASE))

##  system control

const
  AHB_SYSCTRL* = (cast[ptr SYSCTRL_TypeDef](SYSCTRL_BASE))
  AHB_SYSCTRL_ClkSwitch_ADDR* = SYSCTRL_BASE
  AHB_SYSCTRL_ClkDiv_ADDR* = SYSCTRL_BASE + 0x00000004
  AHB_SYSCTRL_PLLParamONE_ADDR* = SYSCTRL_BASE + 0x00000038
  AHB_SYSCTRL_PLLParamTWO_ADDR* = SYSCTRL_BASE + 0x0000003C

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

##  pwrc

const
  APB_PWRC* = (cast[ptr PWRCCTRL_TypeDef](APB_PWRC_BASE))

##  update

const
  APB_UPDATE* = (APB_RTC_BASE + 0x000000F0)

##  register access

template io_write_b*(a, d: untyped): untyped =
  (cast[ptr uint8]((a))[] = (d))

template io_read_b*(a: untyped): untyped =
  (cast[ptr uint8]((a))[])

template io_write*(a, d: untyped): untyped =
  (cast[ptr uint32]((a))[] = (d))

template io_read*(a: untyped): untyped =
  (cast[ptr uint32]((a))[])

##  check RTC Domain Update regitser before Write other RTC Domain register

const
  RtcDomainUpdating* = io_read(APB_UPDATE)

##  apb pin ctrl

const
  APB_PINC* = (cast[ptr PINC_TypeDef](APB_PINC_BASE))

type
  i2c_port_t* {.size: sizeof(cint).} = enum
    I2C_PORT_0, I2C_PORT_1
  uart_port_t* {.size: sizeof(cint).} = enum
    UART_PORT_0, UART_PORT_1
  spi_port_t* {.size: sizeof(cint).} = enum
    SPI_PORT_0, SPI_PORT_1




when defined(USE_STDPERIPH_DRIVER):
  import
    cm32gpm3_uart, cm32gpm3_gpio, cm32gpm3_sysctrl, cm32gpm3_i2c, cm32gpm3_pinctrl,
    cm32gpm3_pwrc, cm32gpm3_rtc, cm32gpm3_ssp, cm32gpm3_timer, cm32gpm3_adc,
    cm32gpm3_pwm

##  Bits Width change to Mask Bits

template BW2M*(a: untyped): untyped =
  (1 shl a) - 1

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
