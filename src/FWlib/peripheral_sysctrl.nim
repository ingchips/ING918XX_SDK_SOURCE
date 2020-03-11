import
  ingsoc

type
  SYSCTRL_ClkGateItem* {.size: sizeof(cint).} = enum
    SYSCTRL_ClkGate_AHB_DMA = 0, SYSCTRL_ClkGate_AHB_LLE = 1,
    SYSCTRL_ClkGate_AHB_IOC = 2, SYSCTRL_ClkGate_AHB_WDT = 3,
    SYSCTRL_ClkGate_APB_I2C0 = 4, SYSCTRL_ClkGate_APB_SPI1 = 5,
    SYSCTRL_ClkGate_APB_TMR0 = 6, SYSCTRL_ClkGate_APB_TMR1 = 7,
    SYSCTRL_ClkGate_APB_TMR2 = 8, SYSCTRL_ClkGate_APB_SCI0 = 9,
    SYSCTRL_ClkGate_APB_SCI1 = 10, SYSCTRL_ClkGate_APB_ISOL = 11,
    SYSCTRL_ClkGate_RtcClkDect = 12, SYSCTRL_ClkGate_APB_GPIOA = 13,
    SYSCTRL_ClkGate_APB_GPIOB = 14, SYSCTRL_ClkGate_APB_GPIOC = 15,
    SYSCTRL_ClkGate_APB_PWM = 16, SYSCTRL_ClkGate_AHB_SPI0 = 17,
    SYSCTRL_ClkGate_APB_PinCtrl = 18, SYSCTRL_ClkGate_APB_I2C1 = 19,
    SYSCTRL_ClkGate_APB_TRNG = 20, SYSCTRL_ClkGate_SysClkOut = 21


## *
##  \brief Set clock gating state of a component
##  \param item      the component to enable clock gating
##

proc SYSCTRL_SetClkGate*(item: SYSCTRL_ClkGateItem) {.
    importc: "SYSCTRL_SetClkGate", header: "peripheral_sysctrl.h".}
## *
##  \brief Clear clock gating state of a component
##  \param item      the component to clear clock gating
##

proc SYSCTRL_ClearClkGate*(item: SYSCTRL_ClkGateItem) {.
    importc: "SYSCTRL_ClearClkGate", header: "peripheral_sysctrl.h".}
## *
##  \brief Set clock gating state of all components
##  \param data      clock gating state of each component
##

proc SYSCTRL_WriteClkGate*(data: uint32) {.importc: "SYSCTRL_WriteClkGate",
                                        header: "peripheral_sysctrl.h".}
## *
##  \brief Get clock gating state of all components
##  \return clock gating state of each components
##

proc SYSCTRL_ReadClkGate*(): uint32 {.importc: "SYSCTRL_ReadClkGate",
                                   header: "peripheral_sysctrl.h".}
type
  SYSCTRL_ResetItem* {.size: sizeof(cint).} = enum
    SYSCTRL_Reset_AHB_DMA = 0, SYSCTRL_Reset_AHB_LLE = 1, SYSCTRL_Reset_AHB_IOC = 2,
    SYSCTRL_Reset_APB_I2C0 = 3, SYSCTRL_Reset_APB_SPI1 = 4,
    SYSCTRL_Reset_APB_TMR0 = 5, SYSCTRL_Reset_APB_TMR1 = 6,
    SYSCTRL_Reset_APB_TMR2 = 7, SYSCTRL_Reset_APB_SCI0 = 8,
    SYSCTRL_Reset_APB_SCI1 = 9, SYSCTRL_Reset_APB_ISOL = 10,
    SYSCTRL_Reset_RtcClk = 11, SYSCTRL_Reset_APB_GPIOA = 12,
    SYSCTRL_Reset_APB_GPIOB = 13, SYSCTRL_Reset_APB_GPIOC = 14,
    SYSCTRL_Reset_APB_PWM = 15, SYSCTRL_Reset_AHB_SPI0 = 16,
    SYSCTRL_Reset_APB_PinCtrl = 17, SYSCTRL_Reset_APB_I2C1 = 18,
    SYSCTRL_Reset_RF = 19, SYSCTRL_Reset_LLE_RFCtrl = 20, SYSCTRL_Reset_APB_TRNG = 21


## *
##  \brief Reset a component
##  \param item      the component to be resetted
##

proc SYSCTRL_ResetBlock*(item: SYSCTRL_ResetItem) {.importc: "SYSCTRL_ResetBlock",
    header: "peripheral_sysctrl.h".}
## *
##  \brief Release a component from reset
##  \param item      the component to be released
##

proc SYSCTRL_ReleaseBlock*(item: SYSCTRL_ResetItem) {.
    importc: "SYSCTRL_ReleaseBlock", header: "peripheral_sysctrl.h".}
## *
##  \brief Reset/Release control of all components
##  \param data      Reset/Release control of each component
##

proc SYSCTRL_WriteBlockRst*(data: uint32) {.importc: "SYSCTRL_WriteBlockRst",
    header: "peripheral_sysctrl.h".}
## *
##  \brief Get Reset/Release state of all components
##  \return Reset/Release state of each components
##

proc SYSCTRL_ReadBlockRst*(): uint32 {.importc: "SYSCTRL_ReadBlockRst",
                                    header: "peripheral_sysctrl.h".}