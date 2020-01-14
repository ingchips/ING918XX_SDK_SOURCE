import
  cm32gpm3

## --------------
##  TPO
##

const
  bwPWRC_TPO* = 8

proc PWRC_Set_TPO*(tpo: uint8) {.importc: "PWRC_Set_TPO", header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_TPO*(): uint8 {.importc: "PWRC_Get_TPO", header: "cm32gpm3_pwrc.h".}
## --------------
##  TPD
##

const
  bwPWRC_TPD* = 8

proc PWRC_Set_TPD*(tpd: uint8) {.importc: "PWRC_Set_TPD", header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_TPD*(): uint8 {.importc: "PWRC_Get_TPD", header: "cm32gpm3_pwrc.h".}
## --------------
##  TAO
##

const
  bwPWRC_TAO* = 4

proc PWRC_Set_TAO*(tao: uint8) {.importc: "PWRC_Set_TAO", header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_TAO*(): uint8 {.importc: "PWRC_Get_TAO", header: "cm32gpm3_pwrc.h".}
## --------------
##  TAD
##

const
  bwPWRC_TAD* = 4

proc PWRC_Set_TAD*(tad: uint8) {.importc: "PWRC_Set_TAD", header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_TAD*(): uint8 {.importc: "PWRC_Get_TAD", header: "cm32gpm3_pwrc.h".}
## --------------
##  TTO
##

const
  bwPWRC_TTO* = 4

proc PWRC_Set_TTO*(tto: uint8) {.importc: "PWRC_Set_TTO", header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_TTO*(): uint8 {.importc: "PWRC_Get_TTO", header: "cm32gpm3_pwrc.h".}
## --------------
##  TTD
##

const
  bwPWRC_TTD* = 4

proc PWRC_Set_TTD*(ttd: uint8) {.importc: "PWRC_Set_TTD", header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_TTD*(): uint8 {.importc: "PWRC_Get_TTD", header: "cm32gpm3_pwrc.h".}
## --------------
##  DP_SLP
##

const
  bwPWRC_DP_SLP* = 1

proc PWRC_Set_DP_SLP*() {.importc: "PWRC_Set_DP_SLP", header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_DP_SLP*(): uint8 {.importc: "PWRC_Get_DP_SLP",
                              header: "cm32gpm3_pwrc.h".}
## --------------
##  WRM_BOOT
##

const
  bwPWRC_WRM_BOOT* = 2
  PWRC_WRM_BOOT_COLD_BOOT* = 0
  PWRC_WRM_BOOT_WAKEUP_HAPPENED* = 1
  PWRC_WRM_BOOT_SOFTWARE_RST* = 2
  PWRC_WRM_BOOT_WATCHDOG_RST* = 3

proc PWRC_Get_WRM_BOOT*(): uint8 {.importc: "PWRC_Get_WRM_BOOT",
                                header: "cm32gpm3_pwrc.h".}
## --------------
##  AE
##

const
  bwPWRC_AE* = 1

proc PWRC_Open_AE*() {.importc: "PWRC_Open_AE", header: "cm32gpm3_pwrc.h".}
proc PWRC_Close_AE*() {.importc: "PWRC_Close_AE", header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_AE*(): uint8 {.importc: "PWRC_Get_AE", header: "cm32gpm3_pwrc.h".}
## --------------
##  TE
##

const
  bwPWRC_TE* = 1

proc PWRC_Open_TE*() {.importc: "PWRC_Open_TE", header: "cm32gpm3_pwrc.h".}
proc PWRC_Close_TE*() {.importc: "PWRC_Close_TE", header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_TE*(): uint8 {.importc: "PWRC_Get_TE", header: "cm32gpm3_pwrc.h".}
## --------------
##  AE_STAT
##

const
  bwPWRC_AE_STAT* = 1

proc PWRC_Get_AE_STAT*(): uint8 {.importc: "PWRC_Get_AE_STAT",
                               header: "cm32gpm3_pwrc.h".}
## --------------
##  TE_STAT
##

const
  bwPWRC_TE_STAT* = 1

proc PWRC_Get_TE_STAT*(): uint8 {.importc: "PWRC_Get_TE_STAT",
                               header: "cm32gpm3_pwrc.h".}
## --------------
##  UART_EN
##

const
  bwPWRC_UART_EN* = 1

proc PWRC_Open_UART_EN*() {.importc: "PWRC_Open_UART_EN", header: "cm32gpm3_pwrc.h".}
proc PWRC_Close_UART_EN*() {.importc: "PWRC_Close_UART_EN",
                           header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_UART_EN*(): uint8 {.importc: "PWRC_Get_UART_EN",
                               header: "cm32gpm3_pwrc.h".}
## --------------
##  RTC_M0_EN
##

const
  bwPWRC_RTC_M0_EN* = 1

proc PWRC_Open_RTC_M0_EN*() {.importc: "PWRC_Open_RTC_M0_EN",
                            header: "cm32gpm3_pwrc.h".}
proc PWRC_Close_RTC_M0_EN*() {.importc: "PWRC_Close_RTC_M0_EN",
                             header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_RTC_M0_EN*(): uint8 {.importc: "PWRC_Get_RTC_M0_EN",
                                 header: "cm32gpm3_pwrc.h".}
## --------------
##  RTC_M1_EN
##

const
  bwPWRC_RTC_M1_EN* = 1

proc PWRC_Open_RTC_M1_EN*() {.importc: "PWRC_Open_RTC_M1_EN",
                            header: "cm32gpm3_pwrc.h".}
proc PWRC_Close_RTC_M1_EN*() {.importc: "PWRC_Close_RTC_M1_EN",
                             header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_RTC_M1_EN*(): uint8 {.importc: "PWRC_Get_RTC_M1_EN",
                                 header: "cm32gpm3_pwrc.h".}
## --------------
##  RTC_EXINT_CTRL
##

const
  bwPWRC_RTC_EXINT_CTRL* = 6
  bsPWRC_EXINT_CTRL_INT_EN* = 0
  bwPWRC_EXINT_CTRL_INT_EN* = 1

##  GPIO

const
  bsPWRC_EXINT_CTRL_GIO_OE* = 1
  bwPWRC_EXINT_CTRL_GIO_OE* = 1
  bsPWRC_EXINT_CTRL_GIO_O* = 2
  bwPWRC_EXINT_CTRL_GIO_O* = 1
  bsPWRC_EXINT_CTRL_GIO_I* = 3
  bwPWRC_EXINT_CTRL_GIO_I* = 1

##  PAD Func Sel

const
  bsPWRC_EXINT_CTRL_PIN_CTRL* = 4
  bwPWRC_EXINT_CTRL_PIN_CTRL* = 2
  EXINT_CTRL_PIN_AS_INT* = 0
  EXINT_CTRL_PIN_AS_GIO* = 1
  EXINT_CTRL_PIN_AS_DBG* = 2

proc PWRC_Set_EXINT_CTRL*(ctrl: uint8) {.importc: "PWRC_Set_EXINT_CTRL",
                                      header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_EXINT_CTRL*(): uint8 {.importc: "PWRC_Get_EXINT_CTRL",
                                  header: "cm32gpm3_pwrc.h".}

proc PWRC_Set_EXINT_AS_INT*() {.importc: "PWRC_Set_EXINT_AS_INT",
                              header: "cm32gpm3_pwrc.h".}
proc PWRC_Set_EXINT_AS_GIO*() {.importc: "PWRC_Set_EXINT_AS_GIO",
                              header: "cm32gpm3_pwrc.h".}
proc PWRC_Set_EXINT_AS_DBG*() {.importc: "PWRC_Set_EXINT_AS_DBG",
                              header: "cm32gpm3_pwrc.h".}
proc PWRC_Set_EXINT_GIO_OUTPUT*() {.importc: "PWRC_Set_EXINT_GIO_OUTPUT",
                                  header: "cm32gpm3_pwrc.h".}
proc PWRC_Set_EXINT_GIO_INPUT*() {.importc: "PWRC_Set_EXINT_GIO_INPUT",
                                 header: "cm32gpm3_pwrc.h".}
proc PWRC_Set_EXINT_GIO_OUTPUT_HIGH*() {.importc: "PWRC_Set_EXINT_GIO_OUTPUT_HIGH",
                                       header: "cm32gpm3_pwrc.h".}
proc PWRC_Set_EXINT_GIO_OUTPUT_LOW*() {.importc: "PWRC_Set_EXINT_GIO_OUTPUT_LOW",
                                      header: "cm32gpm3_pwrc.h".}
proc PWRC_Set_EXINT_INT_ENABLE*() {.importc: "PWRC_Set_EXINT_INT_ENABLE",
                                  header: "cm32gpm3_pwrc.h".}
proc PWRC_Set_EXINT_INT_DISABLE*() {.importc: "PWRC_Set_EXINT_INT_DISABLE",
                                   header: "cm32gpm3_pwrc.h".}
## --------------
##  RTC_M0_HP
##

const
  bwPWRC_RTC_M0_HP* = 1

proc PWRC_Clear_RTC_M0_HP*() {.importc: "PWRC_Clear_RTC_M0_HP",
                             header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_RTC_M0_HP*(): uint8 {.importc: "PWRC_Get_RTC_M0_HP",
                                 header: "cm32gpm3_pwrc.h".}
## --------------
##  RTC_M1_HP
##

const
  bwPWRC_RTC_M1_HP* = 1

proc PWRC_Clear_RTC_M1_HP*() {.importc: "PWRC_Clear_RTC_M1_HP",
                             header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_RTC_M1_HP*(): uint8 {.importc: "PWRC_Get_RTC_M1_HP",
                                 header: "cm32gpm3_pwrc.h".}
## --------------
##  EXINT_HP
##

const
  bwPWRC_EXINT_HP* = 1

proc PWRC_Clear_EXINT_HP*() {.importc: "PWRC_Clear_EXINT_HP",
                            header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_EXINT_HP*(): uint8 {.importc: "PWRC_Get_EXINT_HP",
                                header: "cm32gpm3_pwrc.h".}
## --------------
##  EXINT_TP
##

const
  bwPWRC_EXINT_TP* = 3
  PWRC_EXINT_TP_LOW_LEVEL* = 1
  PWRC_EXINT_TP_HIGH_LEVEL* = 0
  PWRC_EXINT_TP_POSI_EDGE* = 2
  PWRC_EXINT_TP_NEGE_EDGE* = 3
  PWRC_EXINT_TP_BOTH_EDGE* = 4

proc PWRC_Set_EXINT_TP*(tp: uint8) {.importc: "PWRC_Set_EXINT_TP",
                                  header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_EXINT_TP*(): uint8 {.importc: "PWRC_Get_EXINT_TP",
                                header: "cm32gpm3_pwrc.h".}
## -------------
##  EXINT_CL
##

const
  bwPWRC_EXINT_SYS_INT_CL* = 1

proc PWRC_Set_EXINT_SYS_INT_CL*() {.importc: "PWRC_Set_EXINT_SYS_INT_CL",
                                  header: "cm32gpm3_pwrc.h".}
## -------------
##  TM
##

const
  bwPWRC_TM* = 2

proc PWRC_Get_TM*(): uint8 {.importc: "PWRC_Get_TM", header: "cm32gpm3_pwrc.h".}
## -------------
##  NVREG0~7
##

const
  bwPWRC_NVREG* = 32

proc PWRC_Set_NVREG*(num: uint8; data: uint32) {.importc: "PWRC_Set_NVREG",
    header: "cm32gpm3_pwrc.h".}
proc PWRC_Get_NVREG*(num: uint8): uint32 {.importc: "PWRC_Get_NVREG",
                                       header: "cm32gpm3_pwrc.h".}