#ifndef __PERIPHERAL_SYSCTRL_H__
#define __PERIPHERAL_SYSCTRL_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

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

#define SYSCTRL_ClkGate_APB_GPIO0           SYSCTRL_ClkGate_APB_GPIO
#define SYSCTRL_ClkGate_APB_GPIO1           SYSCTRL_ClkGate_APB_GPIO

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

enum
{
    SYSCTRL_LDO_OUPUT_1V60 = 0x13,  // Recommended for Vbat = 1.8V
    SYSCTRL_LDO_OUPUT_1V80 = 0x16,  // Recommended for Vbat = 2.5V
    SYSCTRL_LDO_OUPUT_2V50 = 0x1f,  // Recommended for Vbat = 3.3V
};

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

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

typedef enum
{
    SYSCTRL_ITEM_APB_GPIO0     ,
    SYSCTRL_ITEM_APB_GPIO1     ,
    SYSCTRL_ITEM_APB_TMR0      ,
    SYSCTRL_ITEM_APB_TMR1      ,
    SYSCTRL_ITEM_APB_TMR2      ,
    SYSCTRL_ITEM_APB_WDT       ,
    SYSCTRL_ITEM_APB_PWM       ,
    SYSCTRL_ITEM_APB_PDM       ,
    SYSCTRL_ITEM_APB_QDEC      ,
    SYSCTRL_ITEM_APB_KeyScan   ,
    SYSCTRL_ITEM_APB_IR        ,
    SYSCTRL_ITEM_APB_DMA       ,
    SYSCTRL_ITEM_AHB_SPI0      ,
    SYSCTRL_ITEM_APB_SPI1      ,
    SYSCTRL_ITEM_APB_ADC       ,
    SYSCTRL_ITEM_APB_I2S       ,
    SYSCTRL_ITEM_APB_UART0     ,
    SYSCTRL_ITEM_APB_UART1     ,
    SYSCTRL_ITEM_APB_I2C0      ,
    SYSCTRL_ITEM_APB_I2C1      ,
    SYSCTRL_ITEM_APB_SysCtrl   ,
    SYSCTRL_ITEM_APB_PinCtrl   ,
    SYSCTRL_ITEM_APB_EFUSE     ,
    SYSCTRL_ITEM_NUMBER,
} SYSCTRL_Item;

// compatible definitions with ING918xx
#define  SYSCTRL_ClkGate_APB_GPIO0              SYSCTRL_ITEM_APB_GPIO0
#define  SYSCTRL_ClkGate_APB_GPIO1              SYSCTRL_ITEM_APB_GPIO1
#define  SYSCTRL_ClkGate_APB_TMR0               SYSCTRL_ITEM_APB_TMR0
#define  SYSCTRL_ClkGate_APB_TMR1               SYSCTRL_ITEM_APB_TMR1
#define  SYSCTRL_ClkGate_APB_TMR2               SYSCTRL_ITEM_APB_TMR2
#define  SYSCTRL_ClkGate_APB_WDT                SYSCTRL_ITEM_APB_WDT
#define  SYSCTRL_ClkGate_APB_PWM                SYSCTRL_ITEM_APB_PWM
#define  SYSCTRL_ClkGate_APB_PDM                SYSCTRL_ITEM_APB_PDM
#define  SYSCTRL_ClkGate_APB_QDEC               SYSCTRL_ITEM_APB_QDEC
#define  SYSCTRL_ClkGate_APB_KeyScan            SYSCTRL_ITEM_APB_KeyScan
#define  SYSCTRL_ClkGate_APB_IR                 SYSCTRL_ITEM_APB_IR
#define  SYSCTRL_ClkGate_APB_DMA                SYSCTRL_ITEM_APB_DMA
#define  SYSCTRL_ClkGate_AHB_SPI0               SYSCTRL_ITEM_AHB_SPI0
#define  SYSCTRL_ClkGate_APB_SPI1               SYSCTRL_ITEM_APB_SPI1
#define  SYSCTRL_ClkGate_APB_ADC                SYSCTRL_ITEM_APB_ADC
#define  SYSCTRL_ClkGate_APB_I2S                SYSCTRL_ITEM_APB_I2S
#define  SYSCTRL_ClkGate_APB_UART0              SYSCTRL_ITEM_APB_UART0
#define  SYSCTRL_ClkGate_APB_UART1              SYSCTRL_ITEM_APB_UART1
#define  SYSCTRL_ClkGate_APB_I2C0               SYSCTRL_ITEM_APB_I2C0
#define  SYSCTRL_ClkGate_APB_I2C1               SYSCTRL_ITEM_APB_I2C1
#define  SYSCTRL_ClkGate_APB_PinCtrl            SYSCTRL_ITEM_APB_PinCtrl
#define  SYSCTRL_ClkGate_APB_EFUSE              SYSCTRL_ITEM_APB_EFUSE

typedef SYSCTRL_Item SYSCTRL_ClkGateItem;

typedef SYSCTRL_Item SYSCTRL_ResetItem;

enum
{
    SYSCTRL_LDO_OUPUT_1V60 = 0x13,  // Recommended for Vbat = 1.8V
    SYSCTRL_LDO_OUPUT_1V80 = 0x16,  // Recommended for Vbat = 2.5V
    SYSCTRL_LDO_OUPUT_2V50 = 0x1f,  // Recommended for Vbat = 3.3V
};

typedef enum
{
    SYSCTRL_TMR_CLK_OSC_DIV_4,      // use RF OSC clock div 4
    SYSCTRL_TMR_CLK_32k             // use 32k clock
} SYSCTRL_TimerClkMode;

/**
 * \brief Select timer clock mode
 * \param port          the timer
 * \param mode          clock mode
 */
void SYSCTRL_SelectTimerClk(timer_port_t port, SYSCTRL_TimerClkMode mode);

typedef enum
{
    SYSCTRL_CLK_OSC,                            // use RF OSC clock
    SYSCTRL_CLK_HCLK,                           // use HCLK (same as MCU)
    SYSCTRL_CLK_ADC_DIV = SYSCTRL_CLK_HCLK,     // use clock from ADC divider
    SYSCTRL_CLK_PLL_DIV_1 = SYSCTRL_CLK_HCLK,   // use (PLL clock div 1)
                                                // SYSCTRL_TMR_CLK_PLL_DIV_1 + 1: use (PLL clock div 2)
                                                // ..
                                                // SYSCTRL_TMR_CLK_PLL_DIV_1 + 14: use (PLL clock div 15)
} SYSCTRL_ClkMode;

/**
 * \brief Select SPI clock mode
 * \param port          the port
 * \param mode          clock mode
 *
 * Note: For SPI0: mode should be SYSCTRL_CLK_OSC, or SYSCTRL_CLK_PLL_DIV_1 + N, where N = 0..14;
 *       For SPI1: mode should be SYSCTRL_CLK_OSC or SYSCTRL_CLK_HCLK.
 */
void SYSCTRL_SelectSpiClk(spi_port_t port, SYSCTRL_ClkMode mode);

/**
 * \brief Select UART clock mode
 * \param port          the port
 * \param mode          clock mode (SYSCTRL_CLK_OSC, or SYSCTRL_CLK_HCLK)
 */
void SYSCTRL_SelectUartClk(uart_port_t port, SYSCTRL_ClkMode mode);

/**
 * \brief Select I2S clock mode
 * \param mode          clock mode
 *
 * Note: mode should be SYSCTRL_CLK_OSC, or SYSCTRL_CLK_PLL_DIV_1 + N, where N = 0..14.
 */
void SYSCTRL_SelectI2sClk(SYSCTRL_ClkMode mode);

/**
 * \brief Get current PLL clock in Hz
 * \return              clock in Hz
 */
uint32_t SYSCTRL_GetPLLClk(void);

/**
 * \brief Config PLL clock in Hz
 *
 *               f_osc * loop
 * f_vco = ------------------------
 *                  div_pre
 *
 *                 f_vco
 * f_pll = ------------------------
 *               div_output
 *
 * Requirements:
 *    1. f_vco should be in [60, 600]MHz;
 *    1. f_osc/div_pref should be in [2, 24]MHz;
 *
 * \param loop          loop (6 bits)
 * \param div_pre       div_pre (8 bits)
 * \param div_output    div_output (6 bits)
 * \return              0 if config is proper else non-0
 */
int SYSCTRL_ConfigPLLClk(uint32_t div_pre, uint32_t loop, uint32_t div_output);

/**
 * \brief Select HClk clock mode
 * \param mode          clock mode
 *
 * Note: mode should be SYSCTRL_CLK_OSC, or SYSCTRL_CLK_PLL_DIV_1 + N, where N = 0..14.
 */
void SYSCTRL_SelectHClk(SYSCTRL_ClkMode mode);

/**
 * \brief Get current HClk (same as MCU working clock) in Hz
 * \return              clock in Hz
 */
uint32_t SYSCTRL_GetHClk(void);

/**
 * \brief Select PClk (APB Clk) divider
 * \param div           divider (1..15)
 *
 * Note:  PClk is divided from HClk.
 */
void SYSCTRL_SetPClkDiv(uint8_t div);

/**
 * \brief Get current PClk in Hz
 * \return              clock in Hz
 */
uint32_t SYSCTRL_GetPClk(void);

/**
 * \brief Get the clock of an item in Hz
 * \param item          the item
 * \return              clock in Hz
 */
uint32_t SYSCTRL_GetClk(SYSCTRL_Item item);

/**
 * \brief Select ADC Clk divider
 * \param denom         denom (6 bits)
 * \param num           num (6 bits)
 */
void SYSCTRL_SetAdcClkDiv(uint8_t denom, uint8_t num);

/**
 * \brief Get ADC Clk from divider
 * \return              clock in Hz
 */
uint32_t SYSCTRL_GetAdcClkDiv(void);

/**
 * \brief Select Clk mode for a type of items
 * \param item          item of type A (PDM/PWM/IR/ADC/EFUSE)
 * \param mode          clock mode ({SYSCTRL_CLK_OSC, SYSCTRL_CLK_ADC_DIV})
 */
void SYSCTRL_SelectTypeAClk(SYSCTRL_Item item, SYSCTRL_ClkMode mode);

typedef enum
{
    SYSCTRL_DMA_UART0_RX = 0,
    SYSCTRL_DMA_UART1_RX = 1,
    SYSCTRL_DMA_SPI0_TX  = 2,
    SYSCTRL_DMA_SPI1_TX  = 3,
    SYSCTRL_DMA_I2C0     = 4,
    SYSCTRL_DMA_QDEC0    = 5,
    SYSCTRL_DMA_QDEC1    = 6,
    SYSCTRL_DMA_I2S_RX   = 7,
    SYSCTRL_DMA_PDM      = 8,
    SYSCTRL_DMA_ADC      = 9,

    SYSCTRL_DMA_UART0_TX = 0x10,
    SYSCTRL_DMA_UART1_TX = 0x11,
    SYSCTRL_DMA_SPI0_RX  = 0x12,
    SYSCTRL_DMA_SPI1_RX  = 0x13,
    SYSCTRL_DMA_I2C1     = 0x14,
    SYSCTRL_DMA_QDEC2    = 0x15,
    SYSCTRL_DMA_KeyScan  = 0x16,
    SYSCTRL_DMA_I2S_TX   = 0x17,
    SYSCTRL_DMA_PWM      = 0x19,
    SYSCTRL_DMA_LAST     = SYSCTRL_DMA_PWM,
} SYSCTRL_DMA;

/**
 * \brief Select used DMA items
 * \param items         combination of bits whose positions are listed in `SYSCTRL_DMA`
 * \return              0 if no error occurred else non-0
 *
 * Note:
 *   1. This function should be used when using DMA on hardware peripherals.
 *   1. This configuration is recommended to be fixed and not to be modified dynamically.
 *   1. If too many items are configrued, errors will occur.
 */
int SYSCTRL_SelectUsedDmaItems(uint32_t items);

/**
 * \brief Get the DMA request ID of an item
 * \param item          Previously configured `SYSCTRL_DMA` in `SYSCTRL_SelectUsedDmaItems`
 * \return              request ID of the item (if not found, -1 is returned)
 */
int SYSCTRL_GetDmaId(SYSCTRL_DMA item);

#endif

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
 * @brief Set LDO output level
 *
 * @param[in] level         output level
 */
void SYSCTRL_SetLDOOutput(int level);

/**
 * @brief Wait for LDO state ready
 */
void SYSCTRL_WaitForLDO(void);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

