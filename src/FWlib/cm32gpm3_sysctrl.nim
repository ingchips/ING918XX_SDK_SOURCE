import
  cm32gpm3

##
##  Description:
##  Bit shifts and widths for SYSCTRL_ClkSwitch  0x00
##

const
  bsSYSCTRL_ClkSwitch_Sys* = 0
  bwSYSCTRL_ClkSwitch_Sys* = 1
  bsSYSCTRL_ClkSwitch_CW* = 8
  bwSYSCTRL_ClkSwitch_CW* = 1
  bsSYSCTRL_ClkSwitch_Apb* = 16
  bwSYSCTRL_ClkSwitch_Apb* = 1
  bsSYSCTRL_ClkSwitch_PPS* = 24
  bwSYSCTRL_ClkSwitch_PPS* = 1

##
##  Description:
##  Bit shifts and widths for SYSCTRL_ClkDiv  0x04
##

const
  bsSYSCTRL_ClkDiv_Sys* = 0
  bwSYSCTRL_ClkDiv_Sys* = 5
  bsSYSCTRL_ClkDiv_CW* = 8
  bwSYSCTRL_ClkDiv_CW* = 5
  bsSYSCTRL_ClkDiv_Apb* = 16
  bwSYSCTRL_ClkDiv_Apb* = 5
  bsSYSCTRL_ClkDiv_PPS* = 24
  bwSYSCTRL_ClkDiv_PPS* = 5

##
##  Description:
##  Bit shifts and widths for SYSCTRL_ClkGate  0x10
##

const
  bsSYSCTRL_ClkGate_AHB_DMA* = 0
  bwSYSCTRL_ClkGate_AHB_DMA* = 1
  bsSYSCTRL_ClkGate_AHB_BB* = 1
  bwSYSCTRL_ClkGate_AHB_BB* = 1
  bsSYSCTRL_ClkGate_AHB_SPI0* = 2
  bwSYSCTRL_ClkGate_AHB_SPI0* = 1
  bsSYSCTRL_ClkGate_APB_GPIO* = 3
  bwSYSCTRL_ClkGate_APB_GPIO* = 1
  bsSYSCTRL_ClkGate_APB_I2C* = 4
  bwSYSCTRL_ClkGate_APB_I2C* = 1
  bsSYSCTRL_ClkGate_APB_SPI1* = 5
  bwSYSCTRL_ClkGate_APB_SPI1* = 1
  bsSYSCTRL_ClkGate_APB_TMR0* = 6
  bwSYSCTRL_ClkGate_APB_TMR0* = 1
  bsSYSCTRL_ClkGate_APB_TMR1* = 7
  bwSYSCTRL_ClkGate_APB_TMR1* = 1
  bsSYSCTRL_ClkGate_APB_TMR2* = 8
  bwSYSCTRL_ClkGate_APB_TMR2* = 1
  bsSYSCTRL_ClkGate_APB_SCI0* = 9
  bwSYSCTRL_ClkGate_APB_SCI0* = 1
  bsSYSCTRL_ClkGate_APB_SCI1* = 10
  bwSYSCTRL_ClkGate_APB_SCI1* = 1
  bsSYSCTRL_ClkGate_APB_ISOL* = 11
  bwSYSCTRL_ClkGate_APB_ISOL* = 1
  bsSYSCTRL_ClkGate_RtcClkDect* = 12
  bwSYSCTRL_ClkGate_RtcClkDect* = 1
  bsSYSCTRL_ClkGate_APB_PINC* = 13
  bwSYSCTRL_ClkGate_APB_PINC* = 1

##
##  Description:
##  Bit shifts and widths for SYSCTRL_BlockRst  0x20
##

const
  bsSYSCTRL_BlockRst_AHB_DMA* = 0
  bwSYSCTRL_BlockRst_AHB_DMA* = 1
  bsSYSCTRL_BlockRst_AHB_BB* = 1
  bwSYSCTRL_BlockRst_AHB_BB* = 1
  bsSYSCTRL_BlockRst_CW_BB* = 2
  bwSYSCTRL_BlockRst_CW_BB* = 1
  bsSYSCTRL_BlockRst_PPS_BB* = 3
  bwSYSCTRL_BlockRst_PPS_BB* = 1
  bsSYSCTRL_BlockRst_RTC_BB* = 4
  bwSYSCTRL_BlockRst_RTC_BB* = 1
  bsSYSCTRL_BlockRst_RF_BB* = 5
  bwSYSCTRL_BlockRst_RF_BB* = 1
  bsSYSCTRL_BlockRst_AHB_SPI0* = 6
  bwSYSCTRL_BlockRst_AHB_SPI0* = 1
  bsSYSCTRL_BlockRst_APB_GPIO* = 7
  bwSYSCTRL_BlockRst_APB_GPIO* = 1
  bsSYSCTRL_BlockRst_APB_I2C* = 8
  bwSYSCTRL_BlockRst_APB_I2C* = 1
  bsSYSCTRL_BlockRst_APB_SPI1* = 9
  bwSYSCTRL_BlockRst_APB_SPI1* = 1
  bsSYSCTRL_BlockRst_APB_TMR0* = 10
  bwSYSCTRL_BlockRst_APB_TMR0* = 1
  bsSYSCTRL_BlockRst_APB_TMR1* = 11
  bwSYSCTRL_BlockRst_APB_TMR1* = 1
  bsSYSCTRL_BlockRst_APB_TMR2* = 12
  bwSYSCTRL_BlockRst_APB_TMR2* = 1
  bsSYSCTRL_BlockRst_APB_SCI0* = 13
  bwSYSCTRL_BlockRst_APB_SCI0* = 1
  bsSYSCTRL_BlockRst_APB_SCI1* = 14
  bwSYSCTRL_BlockRst_APB_SCI1* = 1
  bsSYSCTRL_BlockRst_APB_ISOL* = 15
  bwSYSCTRL_BlockRst_APB_ISOL* = 1
  bsSYSCTRL_BlockRst_APB_PINC* = 16
  bwSYSCTRL_BlockRst_APB_PINC* = 1

##
##  Description:
##  Bit shifts and widths for SYSCTRL_PLLParamONE  0x38
##

const
  bsSYSCTRL_PLLParamONE_N* = 0
  bwSYSCTRL_PLLParamONE_N* = 4
  bsSYSCTRL_PLLParamONE_M* = 8
  bwSYSCTRL_PLLParamONE_M* = 8
  bsSYSCTRL_PLLParamONE_OD* = 16
  bwSYSCTRL_PLLParamONE_OD* = 2

##
##  Description:
##  Bit shifts and widths for SYSCTRL_PLLParamTWO  0x3C
##

const
  bsSYSCTRL_PLLParamTWO_RST* = 0
  bwSYSCTRL_PLLParamTWO_RST* = 1
  bsSYSCTRL_PLLParamTWO_BP* = 8
  bwSYSCTRL_PLLParamTWO_BP* = 1
  bsSYSCTRL_PLLParamTWO_LOCK* = 16
  bwSYSCTRL_PLLParamTWO_LOCK* = 1

## ------------------------------
##  SYSCTRL_ClkSwitch
##
##  set

proc SYSCTRL_SetSysClkSwitch*() {.importc: "SYSCTRL_SetSysClkSwitch",
                                header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetCWClkSwitch*() {.importc: "SYSCTRL_SetCWClkSwitch",
                               header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetApbClkSwitch*() {.importc: "SYSCTRL_SetApbClkSwitch",
                                header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetPPSClkSwitch*() {.importc: "SYSCTRL_SetPPSClkSwitch",
                                header: "cm32gpm3_sysctrl.h".}
##  clear

proc SYSCTRL_ClearSysClkSwitch*() {.importc: "SYSCTRL_ClearSysClkSwitch",
                                  header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearCWClkSwitch*() {.importc: "SYSCTRL_ClearCWClkSwitch",
                                 header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearApbClkSwitch*() {.importc: "SYSCTRL_ClearApbClkSwitch",
                                  header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearPPSClkSwitch*() {.importc: "SYSCTRL_ClearPPSClkSwitch",
                                  header: "cm32gpm3_sysctrl.h".}
##  get

proc SYSCTRL_GetClkSwitch*(): uint32 {.importc: "SYSCTRL_GetClkSwitch",
                                    header: "cm32gpm3_sysctrl.h".}
## -----------------------------------------
##  SYSCTRL_ClkDiv
##
##  set

proc SYSCTRL_SetSysClkDiv*(`div`: uint8) {.importc: "SYSCTRL_SetSysClkDiv",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetCWClkDiv*(`div`: uint8) {.importc: "SYSCTRL_SetCWClkDiv",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetApbClkDiv*(`div`: uint8) {.importc: "SYSCTRL_SetApbClkDiv",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetPPSClkDiv*(`div`: uint8) {.importc: "SYSCTRL_SetPPSClkDiv",
                                        header: "cm32gpm3_sysctrl.h".}
##  get

proc SYSCTRL_GetClkDiv*(): uint32 {.importc: "SYSCTRL_GetClkDiv",
                                 header: "cm32gpm3_sysctrl.h".}
## -----------------------------------------
##  SYSCTRL_ClkGate
##
##  set

proc SYSCTRL_SetClkGate_AHB_DMA*() {.importc: "SYSCTRL_SetClkGate_AHB_DMA",
                                   header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_AHB_BB*() {.importc: "SYSCTRL_SetClkGate_AHB_BB",
                                  header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_AHB_SPI0*() {.importc: "SYSCTRL_SetClkGate_AHB_SPI0",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_APB_GPIO*() {.importc: "SYSCTRL_SetClkGate_APB_GPIO",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_APB_I2C*() {.importc: "SYSCTRL_SetClkGate_APB_I2C",
                                   header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_APB_SPI1*() {.importc: "SYSCTRL_SetClkGate_APB_SPI1",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_APB_TMR0*() {.importc: "SYSCTRL_SetClkGate_APB_TMR0",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_APB_TMR1*() {.importc: "SYSCTRL_SetClkGate_APB_TMR1",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_APB_TMR2*() {.importc: "SYSCTRL_SetClkGate_APB_TMR2",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_APB_SCI0*() {.importc: "SYSCTRL_SetClkGate_APB_SCI0",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_APB_SCI1*() {.importc: "SYSCTRL_SetClkGate_APB_SCI1",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_APB_ISOL*() {.importc: "SYSCTRL_SetClkGate_APB_ISOL",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_APB_RtcClkDect*() {.
    importc: "SYSCTRL_SetClkGate_APB_RtcClkDect", header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetClkGate_APB_PINC*() {.importc: "SYSCTRL_SetClkGate_APB_PINC",
                                    header: "cm32gpm3_sysctrl.h".}
##  clear

proc SYSCTRL_ClearClkGate_AHB_DMA*() {.importc: "SYSCTRL_ClearClkGate_AHB_DMA",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_AHB_BB*() {.importc: "SYSCTRL_ClearClkGate_AHB_BB",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_AHB_SPI0*() {.importc: "SYSCTRL_ClearClkGate_AHB_SPI0",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_APB_GPIO*() {.importc: "SYSCTRL_ClearClkGate_APB_GPIO",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_APB_I2C*() {.importc: "SYSCTRL_ClearClkGate_APB_I2C",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_APB_SPI1*() {.importc: "SYSCTRL_ClearClkGate_APB_SPI1",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_APB_TMR0*() {.importc: "SYSCTRL_ClearClkGate_APB_TMR0",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_APB_TMR1*() {.importc: "SYSCTRL_ClearClkGate_APB_TMR1",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_APB_TMR2*() {.importc: "SYSCTRL_ClearClkGate_APB_TMR2",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_APB_SCI0*() {.importc: "SYSCTRL_ClearClkGate_APB_SCI0",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_APB_SCI1*() {.importc: "SYSCTRL_ClearClkGate_APB_SCI1",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_APB_ISOL*() {.importc: "SYSCTRL_ClearClkGate_APB_ISOL",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_APB_RtcClkDect*() {.
    importc: "SYSCTRL_ClearClkGate_APB_RtcClkDect", header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearClkGate_APB_PINC*() {.importc: "SYSCTRL_ClearClkGate_APB_PINC",
                                      header: "cm32gpm3_sysctrl.h".}
##  toggle

proc SYSCTRL_ToggleClkGate_AHB_DMA*() {.importc: "SYSCTRL_ToggleClkGate_AHB_DMA",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_AHB_BB*() {.importc: "SYSCTRL_ToggleClkGate_AHB_BB",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_AHB_SPI0*() {.importc: "SYSCTRL_ToggleClkGate_AHB_SPI0",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_APB_GPIO*() {.importc: "SYSCTRL_ToggleClkGate_APB_GPIO",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_APB_I2C*() {.importc: "SYSCTRL_ToggleClkGate_APB_I2C",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_APB_SPI1*() {.importc: "SYSCTRL_ToggleClkGate_APB_SPI1",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_APB_TMR0*() {.importc: "SYSCTRL_ToggleClkGate_APB_TMR0",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_APB_TMR1*() {.importc: "SYSCTRL_ToggleClkGate_APB_TMR1",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_APB_TMR2*() {.importc: "SYSCTRL_ToggleClkGate_APB_TMR2",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_APB_SCI0*() {.importc: "SYSCTRL_ToggleClkGate_APB_SCI0",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_APB_SCI1*() {.importc: "SYSCTRL_ToggleClkGate_APB_SCI1",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_APB_ISOL*() {.importc: "SYSCTRL_ToggleClkGate_APB_ISOL",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_APB_RtcClkDect*() {.
    importc: "SYSCTRL_ToggleClkGate_APB_RtcClkDect", header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleClkGate_APB_PINC*() {.importc: "SYSCTRL_ToggleClkGate_APB_PINC",
                                       header: "cm32gpm3_sysctrl.h".}
##  write

proc SYSCTRL_WriteClkGate*(data: uint32) {.importc: "SYSCTRL_WriteClkGate",
                                        header: "cm32gpm3_sysctrl.h".}
##  read

proc SYSCTRL_ReadClkGate*(): uint32 {.importc: "SYSCTRL_ReadClkGate",
                                   header: "cm32gpm3_sysctrl.h".}
## -----------------------------------------
##  SYSCTRL_BlockRst
##
##  set

proc SYSCTRL_SetBlockRst_AHB_DMA*() {.importc: "SYSCTRL_SetBlockRst_AHB_DMA",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_AHB_BB*() {.importc: "SYSCTRL_SetBlockRst_AHB_BB",
                                   header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_CW_BB*() {.importc: "SYSCTRL_SetBlockRst_CW_BB",
                                  header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_PPS_BB*() {.importc: "SYSCTRL_SetBlockRst_PPS_BB",
                                   header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_RTC_BB*() {.importc: "SYSCTRL_SetBlockRst_RTC_BB",
                                   header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_RF_BB*() {.importc: "SYSCTRL_SetBlockRst_RF_BB",
                                  header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_AHB_SPI0*() {.importc: "SYSCTRL_SetBlockRst_AHB_SPI0",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_APB_GPIO*() {.importc: "SYSCTRL_SetBlockRst_APB_GPIO",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_APB_I2C*() {.importc: "SYSCTRL_SetBlockRst_APB_I2C",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_APB_SPI1*() {.importc: "SYSCTRL_SetBlockRst_APB_SPI1",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_APB_TMR0*() {.importc: "SYSCTRL_SetBlockRst_APB_TMR0",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_APB_TMR1*() {.importc: "SYSCTRL_SetBlockRst_APB_TMR1",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_APB_TMR2*() {.importc: "SYSCTRL_SetBlockRst_APB_TMR2",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_APB_SCI0*() {.importc: "SYSCTRL_SetBlockRst_APB_SCI0",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_APB_SCI1*() {.importc: "SYSCTRL_SetBlockRst_APB_SCI1",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_APB_ISOL*() {.importc: "SYSCTRL_SetBlockRst_APB_ISOL",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetBlockRst_APB_PINC*() {.importc: "SYSCTRL_SetBlockRst_APB_PINC",
                                     header: "cm32gpm3_sysctrl.h".}
##  clear

proc SYSCTRL_ClearBlockRst_AHB_DMA*() {.importc: "SYSCTRL_ClearBlockRst_AHB_DMA",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_AHB_BB*() {.importc: "SYSCTRL_ClearBlockRst_AHB_BB",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_CW_BB*() {.importc: "SYSCTRL_ClearBlockRst_CW_BB",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_PPS_BB*() {.importc: "SYSCTRL_ClearBlockRst_PPS_BB",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_RTC_BB*() {.importc: "SYSCTRL_ClearBlockRst_RTC_BB",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_RF_BB*() {.importc: "SYSCTRL_ClearBlockRst_RF_BB",
                                    header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_AHB_SPI0*() {.importc: "SYSCTRL_ClearBlockRst_AHB_SPI0",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_APB_GPIO*() {.importc: "SYSCTRL_ClearBlockRst_APB_GPIO",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_APB_I2C*() {.importc: "SYSCTRL_ClearBlockRst_APB_I2C",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_APB_SPI1*() {.importc: "SYSCTRL_ClearBlockRst_APB_SPI1",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_APB_TMR0*() {.importc: "SYSCTRL_ClearBlockRst_APB_TMR0",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_APB_TMR1*() {.importc: "SYSCTRL_ClearBlockRst_APB_TMR1",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_APB_TMR2*() {.importc: "SYSCTRL_ClearBlockRst_APB_TMR2",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_APB_SCI0*() {.importc: "SYSCTRL_ClearBlockRst_APB_SCI0",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_APB_SCI1*() {.importc: "SYSCTRL_ClearBlockRst_APB_SCI1",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_APB_ISOL*() {.importc: "SYSCTRL_ClearBlockRst_APB_ISOL",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ClearBlockRst_APB_PINC*() {.importc: "SYSCTRL_ClearBlockRst_APB_PINC",
                                       header: "cm32gpm3_sysctrl.h".}
##  toggle

proc SYSCTRL_ToggleBlockRst_AHB_DMA*() {.importc: "SYSCTRL_ToggleBlockRst_AHB_DMA",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_AHB_BB*() {.importc: "SYSCTRL_ToggleBlockRst_AHB_BB",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_CW_BB*() {.importc: "SYSCTRL_ToggleBlockRst_CW_BB",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_PPS_BB*() {.importc: "SYSCTRL_ToggleBlockRst_PPS_BB",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_RTC_BB*() {.importc: "SYSCTRL_ToggleBlockRst_RTC_BB",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_RF_BB*() {.importc: "SYSCTRL_ToggleBlockRst_RF_BB",
                                     header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_AHB_SPI0*() {.importc: "SYSCTRL_ToggleBlockRst_AHB_SPI0",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_APB_GPIO*() {.importc: "SYSCTRL_ToggleBlockRst_APB_GPIO",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_APB_I2C*() {.importc: "SYSCTRL_ToggleBlockRst_APB_I2C",
                                       header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_APB_SPI1*() {.importc: "SYSCTRL_ToggleBlockRst_APB_SPI1",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_APB_TMR0*() {.importc: "SYSCTRL_ToggleBlockRst_APB_TMR0",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_APB_TMR1*() {.importc: "SYSCTRL_ToggleBlockRst_APB_TMR1",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_APB_TMR2*() {.importc: "SYSCTRL_ToggleBlockRst_APB_TMR2",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_APB_SCI0*() {.importc: "SYSCTRL_ToggleBlockRst_APB_SCI0",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_APB_SCI1*() {.importc: "SYSCTRL_ToggleBlockRst_APB_SCI1",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_APB_ISOL*() {.importc: "SYSCTRL_ToggleBlockRst_APB_ISOL",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_ToggleBlockRst_APB_PINC*() {.importc: "SYSCTRL_ToggleBlockRst_APB_PINC",
                                        header: "cm32gpm3_sysctrl.h".}
##  write

proc SYSCTRL_WriteBlockRst*(data: uint32) {.importc: "SYSCTRL_WriteBlockRst",
    header: "cm32gpm3_sysctrl.h".}
##  read

proc SYSCTRL_ReadBlockRst*(): uint32 {.importc: "SYSCTRL_ReadBlockRst",
                                    header: "cm32gpm3_sysctrl.h".}
## -----------------------------------------
##  SYSCTRL_SoftwareRst
##
##  set

proc SYSCTRL_SetSoftwareRst*() {.importc: "SYSCTRL_SetSoftwareRst",
                               header: "cm32gpm3_sysctrl.h".}
##  get

proc SYSCTRL_GetSoftwareRst*(): uint8 {.importc: "SYSCTRL_GetSoftwareRst",
                                     header: "cm32gpm3_sysctrl.h".}
## -----------------------------------------
##  SYSCTRL_RtcClkCount
##
##  read

proc SYSCTRL_ReadRtcClkCount*(): uint32 {.importc: "SYSCTRL_ReadRtcClkCount",
                                       header: "cm32gpm3_sysctrl.h".}
## -----------------------------------------
##  SYSCTRL_PLLParamONE
##
##  set

proc SYSCTRL_SetPLLONE_N*(data: uint8) {.importc: "SYSCTRL_SetPLLONE_N",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetPLLONE_M*(data: uint8) {.importc: "SYSCTRL_SetPLLONE_M",
                                      header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetPLLONE_OD*(data: uint8) {.importc: "SYSCTRL_SetPLLONE_OD",
                                       header: "cm32gpm3_sysctrl.h".}
##  get

proc SYSCTRL_GetPLLONE*(): uint32 {.importc: "SYSCTRL_GetPLLONE",
                                 header: "cm32gpm3_sysctrl.h".}
## -----------------------------------------
##  SYSCTRL_PLLParamTWO
##
##  set

proc SYSCTRL_SetPLLTWO_RST*(data: uint8) {.importc: "SYSCTRL_SetPLLTWO_RST",
                                        header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_SetPLLTWO_BP*(data: uint8) {.importc: "SYSCTRL_SetPLLTWO_BP",
                                       header: "cm32gpm3_sysctrl.h".}
##  get

proc SYSCTRL_GetPLLTWO*(): uint32 {.importc: "SYSCTRL_GetPLLTWO",
                                 header: "cm32gpm3_sysctrl.h".}
proc SYSCTRL_PLL_LOCKED*(): uint8 {.importc: "SYSCTRL_PLL_LOCKED",
                                 header: "cm32gpm3_sysctrl.h".}