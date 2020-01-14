import
  cm32gpm3

const
  ADC_CLK_16* = 0
  ADC_CLK_32* = 1
  ADC_CLK_64* = 2
  ADC_CLK_128* = 3
  ADC_CLK_EN* = 0x00000004

##  total 8 ADC channels

const
  ADC_CHANNEL_NUM* = 8

## * \brief  Structure type to access the ADC
##

type
  ADC_Type* {.importc: "ADC_Type", header: "cm32gpm3_adc.h", bycopy.} = object
    ctrl0* {.importc: "ctrl0".}: uint32
    ctrl1* {.importc: "ctrl1".}: uint32
    ctrl2* {.importc: "ctrl2".}: uint32
    ctrl3* {.importc: "ctrl3".}: uint32
    ctrl4* {.importc: "ctrl4".}: uint32


## * \brief  Structure type to access the ADC power control
##

type
  ADC_Pwr_Ctrl_Type* {.importc: "ADC_Pwr_Ctrl_Type", header: "cm32gpm3_adc.h", bycopy.} = object
    pon* {.importc: "pon".}: uint32
    act* {.importc: "act".}: uint32


const
  ADC* = (cast[ptr ADC_Type]((SYSCTRL_BASE + 0x00000030)))
  ADC_Pwr_Ctrl* = (cast[ptr ADC_Pwr_Ctrl_Type]((APB_RTC_BASE + 0x00000090)))

## * \brief ADC power control
##  flag: 1: power on
##        0: power off
##

proc ADC_PowerCtrl*(flag: uint8) {.importc: "ADC_PowerCtrl", header: "cm32gpm3_adc.h".}
## * \brief ADC global enable/disable
##  flag: 1: power on
##        0: power off
##

proc ADC_Enable*(flag: uint8) {.importc: "ADC_Enable", header: "cm32gpm3_adc.h".}
## * \brief ADC reset
##

proc ADC_Reset*() {.importc: "ADC_Reset", header: "cm32gpm3_adc.h".}
## * \brief Set ADC clock selection
##   Note that: remeber to include ADC_CLK_EN.
##

proc ADC_SetClkSel*(clk_sel: uint8) {.importc: "ADC_SetClkSel",
                                   header: "cm32gpm3_adc.h".}
const
  ADC_MODE_SINGLE* = 1
  ADC_MODE_LOOP* = 0

proc ADC_SetMode*(mode: uint8) {.importc: "ADC_SetMode", header: "cm32gpm3_adc.h".}
##  the number of clock circles between two calculation at loop mode

proc ADC_SetLoopDelay*(delay: uint32) {.importc: "ADC_SetLoopDelay",
                                     header: "cm32gpm3_adc.h".}
proc ADC_EnableChannel*(channel_id: uint8; flag: uint8) {.
    importc: "ADC_EnableChannel", header: "cm32gpm3_adc.h".}
proc ADC_ClearChannelDataValid*(channel_id: uint8) {.
    importc: "ADC_ClearChannelDataValid", header: "cm32gpm3_adc.h".}
proc ADC_IsChannelDataValid*(channel_id: uint8): uint8 {.
    importc: "ADC_IsChannelDataValid", header: "cm32gpm3_adc.h".}
proc ADC_ReadChannelData*(channel_id: uint8): uint16 {.
    importc: "ADC_ReadChannelData", header: "cm32gpm3_adc.h".}