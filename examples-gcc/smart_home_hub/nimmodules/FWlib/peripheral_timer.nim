import
  ingsoc

##  we have 3 timers in total. Timer 0 can be used as watchdog.

const
  TIMER_NUMBER* = 3
  TMR_CLK_FREQ* = OSC_CLK_FREQ

## *
## ***************************************************************************************
##  @brief Get counter of a timer
##
##  @param[in] pTMR          timer address
##  @return                  counter
## ***************************************************************************************
##

proc TMR_GetCNT*(pTMR: ptr TMR_TypeDef): uint32 {.importc: "TMR_GetCNT",
    header: "peripheral_timer.h".}
## *
## ***************************************************************************************
##  @brief Reload a timer: set its counter to zero
##
##  @param[in] pTMR          timer address
## ***************************************************************************************
##

proc TMR_Reload*(pTMR: ptr TMR_TypeDef) {.importc: "TMR_Reload",
                                      header: "peripheral_timer.h".}
## -----------
##  TMR_CMP
##

proc TMR_SetCMP*(pTMR: ptr TMR_TypeDef; value: uint32) {.importc: "TMR_SetCMP",
    header: "peripheral_timer.h".}
## *
## ***************************************************************************************
##  @brief Set comparison value of a timer
##
##  @param[in] pTMR          timer address
##  @param[in] value         comparison value
## ***************************************************************************************
##

proc TMR_GetCMP*(pTMR: ptr TMR_TypeDef): uint32 {.importc: "TMR_GetCMP",
    header: "peripheral_timer.h".}
## *
## ***************************************************************************************
##  @brief Enable a timer
##
##  @param[in] pTMR          timer address
## ***************************************************************************************
##

proc TMR_Enable*(pTMR: ptr TMR_TypeDef) {.importc: "TMR_Enable",
                                      header: "peripheral_timer.h".}
## *
## ***************************************************************************************
##  @brief Disable a timer
##
##  @param[in] pTMR          timer address
## ***************************************************************************************
##

proc TMR_Disable*(pTMR: ptr TMR_TypeDef) {.importc: "TMR_Disable",
                                       header: "peripheral_timer.h".}
##  timer work mode

const
  TMR_CTL_OP_MODE_WRAPPING* = 0
  TMR_CTL_OP_MODE_ONESHOT* = 1
  TMR_CTL_OP_MODE_FREERUN* = 2

## *
## ***************************************************************************************
##  @brief Set work mode of a timer
##
##  @param[in] pTMR          timer address
##  @param[in] mode          work mode
## ***************************************************************************************
##

proc TMR_SetOpMode*(pTMR: ptr TMR_TypeDef; mode: uint8) {.importc: "TMR_SetOpMode",
    header: "peripheral_timer.h".}
## *
## ***************************************************************************************
##  @brief Enable interrupt of a timer
##
##  @param[in] pTMR          timer address
## ***************************************************************************************
##

proc TMR_IntEnable*(pTMR: ptr TMR_TypeDef) {.importc: "TMR_IntEnable",
    header: "peripheral_timer.h".}
## *
## ***************************************************************************************
##  @brief Disable interrupt of a timer
##
##  @param[in] pTMR          timer address
## ***************************************************************************************
##

proc TMR_IntDisable*(pTMR: ptr TMR_TypeDef) {.importc: "TMR_IntDisable",
    header: "peripheral_timer.h".}
## *
## ***************************************************************************************
##  @brief Clear interrupt request of a timer
##
##  @param[in] pTMR          timer address
## ***************************************************************************************
##

proc TMR_IntClr*(pTMR: ptr TMR_TypeDef) {.importc: "TMR_IntClr",
                                      header: "peripheral_timer.h".}
## *
## ***************************************************************************************
##  @brief Get interrupt status of a timer
##
##  @param[in] pTMR          timer address
## ***************************************************************************************
##

proc TMR_IntHappened*(pTMR: ptr TMR_TypeDef): uint8 {.importc: "TMR_IntHappened",
    header: "peripheral_timer.h".}
## *
## ***************************************************************************************
##  @brief Enable watchdog.
##         Watchdog timed out after its counter counts to `timeout` for 2 times,
##         i.e., if `timeout` == TMR_CLK_FREQ (aka 1s), watchdog timed out after 2sec.
##
##  @param[in] timeout    timeout counter.
## ***************************************************************************************
##

proc TMR_WatchDogEnable*(timeout: uint32) {.importc: "TMR_WatchDogEnable",
    header: "peripheral_timer.h".}
## *
## ***************************************************************************************
##  @brief Disable watchdog.
## ***************************************************************************************
##

proc TMR_WatchDogDisable*() {.importc: "TMR_WatchDogDisable",
                            header: "peripheral_timer.h".}
## *
## ***************************************************************************************
##  @brief Watchdog restart (aka feeding)
## ***************************************************************************************
##

template TMR_WatchDogRestart*(): untyped =
  TMR_Reload(APB_TMR0)

## -----------
##  TMR_LOCK
##

proc TMR0_LOCK*() {.importc: "TMR0_LOCK", header: "peripheral_timer.h".}
proc TMR0_UNLOCK*() {.importc: "TMR0_UNLOCK", header: "peripheral_timer.h".}