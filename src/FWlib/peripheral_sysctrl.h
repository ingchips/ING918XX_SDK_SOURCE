#ifndef __PERIPHERAL_SYSCTRL_H__
#define __PERIPHERAL_SYSCTRL_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"

typedef enum
{
    SYSCTRL_ClkGate_APB_I2C0                =  4,
    SYSCTRL_ClkGate_APB_SPI1                =  5,
    SYSCTRL_ClkGate_APB_TMR0                =  6,
    SYSCTRL_ClkGate_APB_TMR1                =  7,
    SYSCTRL_ClkGate_APB_TMR2                =  8,
    SYSCTRL_ClkGate_APB_UART0               =  9,
    SYSCTRL_ClkGate_APB_UART1               = 10,
    SYSCTRL_ClkGate_APB_GPIO                = 13,
    SYSCTRL_ClkGate_APB_PWM                 = 16,
    SYSCTRL_ClkGate_AHB_SPI0                = 17,
    SYSCTRL_ClkGate_APB_PinCtrl             = 18,
    SYSCTRL_ClkGate_APB_I2C1                = 19
} SYSCTRL_ClkGateItem;

/**
 * \brief Set clock gating state of a component
 * \param item      the component to enable clock gating
 */
void SYSCTRL_SetClkGate(SYSCTRL_ClkGateItem item);

/**
 * \brief Clear clock gating state of a component
 * \param item      the component to clear clock gating
 */
void SYSCTRL_ClearClkGate(SYSCTRL_ClkGateItem item);

/**
 * \brief Set clock gating state of several components
 * \param items      the bitmap of components to enable clock gating
 */
void SYSCTRL_SetClkGateMulti(uint32_t items);

/**
 * \brief Clear clock gating state of several components
 * \param items      the bitmap of components to clear clock gating
 */
void SYSCTRL_ClearClkGateMulti(uint32_t items);

/**
 * \brief Set clock gating state of all components
 * \param data      clock gating state of each component
 */
void SYSCTRL_WriteClkGate(uint32_t data);

/**
 * \brief Get clock gating state of all components
 * \return clock gating state of each components
 */
uint32_t SYSCTRL_ReadClkGate(void);

typedef enum
{
    SYSCTRL_Reset_AHB_DMA                 =  0,
    SYSCTRL_Reset_AHB_LLE                 =  1,
    SYSCTRL_Reset_AHB_IOC                 =  2,
    SYSCTRL_Reset_APB_I2C0                =  3,
    SYSCTRL_Reset_APB_SPI1                =  4,
    SYSCTRL_Reset_APB_TMR0                =  5,
    SYSCTRL_Reset_APB_TMR1                =  6,
    SYSCTRL_Reset_APB_TMR2                =  7,
    SYSCTRL_Reset_APB_SCI0                =  8,
    SYSCTRL_Reset_APB_SCI1                =  9,
    SYSCTRL_Reset_APB_ISOL                = 10,
    SYSCTRL_Reset_RtcClk                  = 11,
    SYSCTRL_Reset_APB_GPIOA               = 12,
    SYSCTRL_Reset_APB_GPIOB               = 13,
    SYSCTRL_Reset_APB_GPIOC               = 14,
    SYSCTRL_Reset_APB_PWM                 = 15,
    SYSCTRL_Reset_AHB_SPI0                = 16,
    SYSCTRL_Reset_APB_PinCtrl             = 17,
    SYSCTRL_Reset_APB_I2C1                = 18,
    SYSCTRL_Reset_RF                      = 19,
    SYSCTRL_Reset_LLE_RFCtrl              = 20,
    SYSCTRL_Reset_APB_TRNG                = 21
} SYSCTRL_ResetItem;

/**
 * \brief Reset a component
 * \param item      the component to be resetted
 */
void SYSCTRL_ResetBlock(SYSCTRL_ResetItem item);

/**
 * \brief Release a component from reset
 * \param item      the component to be released
 */
void SYSCTRL_ReleaseBlock(SYSCTRL_ResetItem item);

/**
 * \brief Reset/Release control of all components
 * \param data      Reset/Release control of each component
 */
void SYSCTRL_WriteBlockRst(uint32_t data) ;

/**
 * \brief Get Reset/Release state of all components
 * \return Reset/Release state of each components
 */
uint32_t SYSCTRL_ReadBlockRst(void) ;

enum 
{
    SYSCTRL_LDO_OUPUT_1V60 = 0x13,  // Recommended for Vbat = 1.8V
    SYSCTRL_LDO_OUPUT_2V00 = 0x18,  // Recommended for Vbat = 3.3V
    SYSCTRL_LDO_OUPUT_2V35 = 0x19,  // Recommended for Vbat = 2.5V
    SYSCTRL_LDO_OUPUT_2V50 = 0x1f,  // Recommended for Vbat = 3.0V
};

void SYSCTRL_SetLDOOutput(int level);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

