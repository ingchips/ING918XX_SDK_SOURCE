#ifndef __PERIPHERAL_SYSCTRL_H__
#define __PERIPHERAL_SYSCTRL_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"
#include "peripheral_pinctrl.h"

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

// compatible definitions with ING916xx
#define SYSCTRL_ClkGate_APB_GPIO0           SYSCTRL_ClkGate_APB_GPIO
#define SYSCTRL_ClkGate_APB_GPIO1           SYSCTRL_ClkGate_APB_GPIO
#define SYSCTRL_ClkGate_APB_WDT             SYSCTRL_ClkGate_APB_TMR0

typedef enum
{
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

typedef enum
{
    SYSCTRL_LDO_OUPUT_1V60 = 0x13,  // Recommended for Vbat = 1.8V
    SYSCTRL_LDO_OUPUT_1V80 = 0x16,  // Recommended for Vbat = 2.5V
    SYSCTRL_LDO_OUPUT_2V50 = 0x1f,  // Recommended for Vbat = 3.3V
}SYSCTRL_LDOOutputCore;

enum
{
    SYSCTRL_BOR_0V85 = 0x06,        // BOR Vdd threshold = 0.85V
    SYSCTRL_BOR_0V90 = 0x07,        // BOR Vdd threshold = 0.90V
    SYSCTRL_BOR_0V95 = 0x08,        // BOR Vdd threshold = 0.95V
    SYSCTRL_BOR_1V00 = 0x09,        // BOR Vdd threshold = 1.00V
    SYSCTRL_BOR_1V05 = 0x0A,        // BOR Vdd threshold = 1.05V
};

/**
 * @brief Set clock gating state of all components
 * @param data      clock gating state of each component
 */
void SYSCTRL_WriteClkGate(uint32_t data);

/**
 * @brief Get clock gating state of all components
 * @return clock gating state of each components
 */
uint32_t SYSCTRL_ReadClkGate(void);

/**
 * @brief Reset/Release control of all components
 * @param data      Reset/Release control of each component
 */
void SYSCTRL_WriteBlockRst(uint32_t data) ;

/**
 * @brief Get Reset/Release state of all components
 * @return Reset/Release state of each components
 */
uint32_t SYSCTRL_ReadBlockRst(void) ;

typedef enum
{
    SYSCTRL_MEM_BLOCK_0 = 0x20,     // block 0 is  8KiB starting from 0x20000000
    SYSCTRL_MEM_BLOCK_1 = 0x40,     // block 1 is  8KiB following block 0
    SYSCTRL_MEM_BLOCK_2 = 0x80,     // block 2 is 16KiB following block 1
    SYSCTRL_MEM_BLOCK_3 = 0x100,    // block 3 is 16KiB following block 2
    SYSCTRL_MEM_BLOCK_4 = 0x200,    // block 4 is 16KiB following block 3
    SYSCTRL_SHARE_BLOCK_0 = 0x1,    // share memory block 0 is  8KiB starting from 0x400A0000
    SYSCTRL_SHARE_BLOCK_1 = 0x2,    // share memory block 1 is  8KiB following block 0 (0x400A2000)
    SYSCTRL_SHARE_BLOCK_2 = 0x4,    // share memory block 2 is 16KiB following block 1
    SYSCTRL_SHARE_BLOCK_3 = 0x8,    // share memory block 3 is 16KiB following block 2
    SYSCTRL_SHARE_BLOCK_4 = 0x10,   // share memory block 4 is 16KiB following block 3
} SYSCTRL_MemBlock;

// these 3 blocks (16 + 8) KiB are reversed in _mini_bundles
#define SYSCTRL_RESERVED_MEM_BLOCKS (SYSCTRL_MEM_BLOCK_0 | SYSCTRL_MEM_BLOCK_1 | SYSCTRL_SHARE_BLOCK_0)

/**
 * @brief Get current HClk (same as MCU working clock) in Hz
 *
 * Compatible API with ING916's
 *
 * @return              clock in Hz
 */
#define SYSCTRL_GetHClk()       48000000

#define SYSCTRL_WAKEUP_SOURCE_AUTO          1       // waken up automatically by internal timer
#define SYSCTRL_WAKEUP_SOURCE_EXT_INT       2       // waken up by EXT_INT

typedef struct
{
    uint32_t source;     // bit combination of `SYSCTRL_WAKEUP_SOURCE_...`
} SYSCTRL_WakeupSource_t;

/**
 * @brief Enable the PA, the TXEN is set to GPIO2 and RXEN is set to GPIO6.
 * This function is only allowed to be called after the stack initialized. For example, at
 * the BTSTACK_EVENT_STATE of `user_packet_handler`.
 * When PLATFORM_CB_EVT_LLE_INIT occurred, the function should also be called.
 */
void SYSCTRL_PAEnable(void);

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
    SYSCTRL_ITEM_APB_USB       ,
    SYSCTRL_ITEM_APB_LPC       ,
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

/**
 * @brief LDO Core Output
 *
 * @see `SYSCTRL_SetLDOOutput`
 *
 * Range: [1.000, 1.300]V
 *
 * Default: 1.200V. Step: 20mV
 */
typedef enum
{
    SYSCTRL_LDO_OUTPUT_CORE_1V000 = 0,      // 1.000V
    SYSCTRL_LDO_OUTPUT_CORE_1V020 = 1,
    SYSCTRL_LDO_OUTPUT_CORE_1V040 = 2,
    SYSCTRL_LDO_OUTPUT_CORE_1V060 = 3,
    SYSCTRL_LDO_OUTPUT_CORE_1V080 = 4,
    SYSCTRL_LDO_OUTPUT_CORE_1V100 = 5,
    SYSCTRL_LDO_OUTPUT_CORE_1V120 = 6,
    SYSCTRL_LDO_OUTPUT_CORE_1V140 = 7,
    SYSCTRL_LDO_OUTPUT_CORE_1V160 = 8,
    SYSCTRL_LDO_OUTPUT_CORE_1V180 = 9,
    SYSCTRL_LDO_OUTPUT_CORE_1V200 = 10,
    SYSCTRL_LDO_OUTPUT_CORE_1V220 = 11,
    SYSCTRL_LDO_OUTPUT_CORE_1V240 = 12,
    SYSCTRL_LDO_OUTPUT_CORE_1V260 = 13,
    SYSCTRL_LDO_OUTPUT_CORE_1V280 = 14,
    SYSCTRL_LDO_OUTPUT_CORE_1V300 = 15
} SYSCTRL_LDOOutputCore;

/**
 * @brief LDO Flash Output
 *
 * @see `SYSCTRL_SetLDOOutputFlash`
 *
 * Range: [2.100, 3.100]V
 *
 * Default: 2.100V. Step: 100mV
 */
typedef enum
{
    SYSCTRL_LDO_OUTPUT_FLASH_2V100 = 5,     // 2.100V
    SYSCTRL_LDO_OUTPUT_FLASH_2V200 = 6,
    SYSCTRL_LDO_OUTPUT_FLASH_2V300 = 7,
    SYSCTRL_LDO_OUTPUT_FLASH_2V400 = 8,
    SYSCTRL_LDO_OUTPUT_FLASH_2V500 = 9,
    SYSCTRL_LDO_OUTPUT_FLASH_2V600 = 10,
    SYSCTRL_LDO_OUTPUT_FLASH_2V700 = 11,
    SYSCTRL_LDO_OUTPUT_FLASH_2V800 = 12,
    SYSCTRL_LDO_OUTPUT_FLASH_2V900 = 13,
    SYSCTRL_LDO_OUTPUT_FLASH_3V000 = 14,
    SYSCTRL_LDO_OUTPUT_FLASH_3V100 = 15
} SYSCTRL_LDOOutputFlash;

/**
 * @brief BOR Threshold on VBAT
 *
 * @see `SYSCTRL_ConfigBOR`
 *
 * Range: [1.5, 3]V
 */
enum
{
    SYSCTRL_BOR_1V5 = 0,
    SYSCTRL_BOR_1V6 = 1,
    SYSCTRL_BOR_1V7 = 2,
    SYSCTRL_BOR_1V8 = 3,
    SYSCTRL_BOR_1V9 = 4,
    SYSCTRL_BOR_2V0 = 5,
    SYSCTRL_BOR_2V1 = 6,
    SYSCTRL_BOR_2V2 = 7,
    SYSCTRL_BOR_2V3 = 8,
    SYSCTRL_BOR_2V4 = 9,
    SYSCTRL_BOR_2V5 = 10,
    SYSCTRL_BOR_2V6 = 11,
    SYSCTRL_BOR_2V7 = 12,
    SYSCTRL_BOR_2V8 = 13,
    SYSCTRL_BOR_2V9 = 14,
    SYSCTRL_BOR_3V0 = 15
};

typedef enum
{
    SYSCTRL_CLK_SLOW = 0,            // use slow clock
    SYSCTRL_CLK_32k = 0,             // use 32kHz clock
    SYSCTRL_CLK_HCLK = 1,            // use HCLK (same as MCU)
    SYSCTRL_CLK_ADC_DIV = 1,         // use clock from ADC divider

    SYSCTRL_CLK_PLL_DIV_1 = 1,       // use (PLL clock div 1)
                                     // SYSCTRL_TMR_CLK_PLL_DIV_2: use (PLL clock div 2)
                                     // ..
                                     // SYSCTRL_TMR_CLK_PLL_DIV_15: use (PLL clock div 15)
                                     // Feel free to cast [1..15] to SYSCTRL_ClkMode
    SYSCTRL_CLK_PLL_DIV_2 = 2,
    SYSCTRL_CLK_PLL_DIV_3 = 3,
    SYSCTRL_CLK_PLL_DIV_4 = 4,
    SYSCTRL_CLK_PLL_DIV_5 = 5,
    SYSCTRL_CLK_PLL_DIV_6 = 6,
    SYSCTRL_CLK_PLL_DIV_7 = 7,
    SYSCTRL_CLK_PLL_DIV_8 = 8,
    SYSCTRL_CLK_PLL_DIV_9 = 9,
    SYSCTRL_CLK_PLL_DIV_10 = 10,
    SYSCTRL_CLK_PLL_DIV_11 = 11,
    SYSCTRL_CLK_PLL_DIV_12 = 12,
    SYSCTRL_CLK_PLL_DIV_13 = 13,
    SYSCTRL_CLK_PLL_DIV_14 = 14,
    SYSCTRL_CLK_PLL_DIV_15 = 15,

    SYSCTRL_CLK_SLOW_DIV_1 = 1,      // use RF OSC clock div 1
                                     // SYSCTRL_CLK_SLOW_DIV_2: use (RF OSC clock div 2)
                                     // ..
                                     // Feel free to cast [1..15] to SYSCTRL_ClkMode
    SYSCTRL_CLK_SLOW_DIV_2 = 2,
    SYSCTRL_CLK_SLOW_DIV_3 = 3,
    SYSCTRL_CLK_SLOW_DIV_4 = 4,
    SYSCTRL_CLK_SLOW_DIV_5 = 5,
    SYSCTRL_CLK_SLOW_DIV_6 = 6,
    SYSCTRL_CLK_SLOW_DIV_7 = 7,
    SYSCTRL_CLK_SLOW_DIV_8 = 8,
    SYSCTRL_CLK_SLOW_DIV_9 = 9,
    SYSCTRL_CLK_SLOW_DIV_10 = 10,
    SYSCTRL_CLK_SLOW_DIV_11 = 11,
    SYSCTRL_CLK_SLOW_DIV_12 = 12,
    SYSCTRL_CLK_SLOW_DIV_13 = 13,
    SYSCTRL_CLK_SLOW_DIV_14 = 14,
    SYSCTRL_CLK_SLOW_DIV_15 = 15,
    SYSCTRL_CLK_SLOW_DIV_4095 = 4095,
} SYSCTRL_ClkMode;

/**
 * @brief Select clock mode of TIMER
 *
 * All timers share the same clock divider, which means that if timer K is
 * set to use (SYSCTRL_CLK_SLOW_DIV_X), all previously configures timers that
 * uses (SYSCTRL_CLK_SLOW_DIV_X) are overwritten by (SYSCTRL_CLK_SLOW_DIV_X).
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_N`, where N = 1..15;
 *
 * @param port          the timer
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectTimerClk(timer_port_t port, SYSCTRL_ClkMode mode);

/**
 * @brief Select clock mode of PWM
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_N`, where N = 1..15;
 *
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectPWMClk(SYSCTRL_ClkMode mode);

/**
 * @brief Select clock mode of KeyScan
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_N`, where N = 1..15;
 *
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectKeyScanClk(SYSCTRL_ClkMode mode);

/**
 * @brief Select clock mode of PDM
 *
 * Clock of PDM is divided from SLOW_CLK.`mode` should be `(SYSCTRL_ClkMode)N`, where N = 1..63;
 *
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectPDMClk(SYSCTRL_ClkMode mode);

/**
 * @brief Select SPI clock mode
 * @param port          the port
 * @param mode          clock mode
 *
 * Note: For SPI0: mode should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;
 *       For SPI1: mode should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_HCLK`.
 */
void SYSCTRL_SelectSpiClk(spi_port_t port, SYSCTRL_ClkMode mode);

/**
 * @brief Select UART clock mode
 * @param port          the port
 * @param mode          clock mode (SYSCTRL_CLK_SLOW, or SYSCTRL_CLK_HCLK)
 */
void SYSCTRL_SelectUartClk(uart_port_t port, SYSCTRL_ClkMode mode);

/**
 * @brief Select I2S clock mode
 * @param mode          clock mode
 *
 * Note: mode should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;.
 */
void SYSCTRL_SelectI2sClk(SYSCTRL_ClkMode mode);

/**
 * @brief Get current PLL clock in Hz
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetPLLClk(void);

/**
 * @brief Config PLL clock in Hz
 *
 *               f_in * loop
 * f_vco = ------------------------
 *                  div_pre
 *
 *                 f_vco
 * f_pll = ------------------------
 *               div_output
 *
 * Requirements:
 *    1. f_in/div_pref should be in [2, 30]MHz, where f_in is slow clock;
 *    1. f_vco should be in [60, 600]MHz;
 *    1. f_pll should not exceed 500MHz.
 *
 * @param div_pre       div_pre (8 bits)
 * @param loop          loop (6 bits)
 * @param div_output    div_output (6 bits)
 * @return              0 if config is proper else non-0
 */
int SYSCTRL_ConfigPLLClk(uint32_t div_pre, uint32_t loop, uint32_t div_output);

/**
 * @brief Select HClk clock mode
 * @param mode          clock mode
 *
 * Note: mode should be SYSCTRL_CLK_SLOW, or SYSCTRL_CLK_PLL_DIV_N,
 *       where N = 1..15.
 *
 * Note: While changing, both clocks (OSC & PLL) must be running.
 */
void SYSCTRL_SelectHClk(SYSCTRL_ClkMode mode);

/**
 * @brief Get current HClk (same as MCU working clock) in Hz
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetHClk(void);

/**
 * @brief Select PClk (APB Clk) divider
 * @param div           divider (1..15)
 *
 * Note:  PClk is divided from HClk.
 */
void SYSCTRL_SetPClkDiv(uint8_t div);

/**
 * @brief Get PClk (APB Clk) divider
 * @return              divider (1..15)
 */
uint8_t SYSCTRL_GetPClkDiv(void);

/**
 * @brief Get current PClk in Hz
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetPClk(void);

/**
 * @brief Get the clock of an item in Hz
 * @param item          the item
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetClk(SYSCTRL_Item item);

/**
 * @brief Select ADC Clk divider
 * @param denom         denominator (6 bits)
  */
void SYSCTRL_SetAdcClkDiv(uint8_t denom);

/**
 * @brief Get ADC Clk from divider
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetAdcClkDiv(void);

/**
 * @brief Select Clk mode for a type of items
 * @param item          item of type A (IR/ADC/EFUSE)
 * @param mode          clock mode ({SYSCTRL_CLK_SLOW, SYSCTRL_CLK_ADC_DIV})
 */
void SYSCTRL_SelectTypeAClk(SYSCTRL_Item item, SYSCTRL_ClkMode mode);

/**
 * @brief Select clock mode of USB
 *
 * `mode` should be `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;
 *
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectUSBClk(SYSCTRL_ClkMode mode);

/**
 * @brief Select clock mode of Flash
 *
 * `mode` should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;
 *
 * Default mode: see `SYSCTRL_ConfigClocksAfterWakeup()`.
 *
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectFlashClk(SYSCTRL_ClkMode mode);

/**
 * @brief Get the frequency of Flash clock
 *
 * @return                      frequency
 */
uint32_t SYSCTRL_GetFlashClk(void);

/**
 * @brief Select the two clocks of QDEC
 *
 * This function configures two clocks for QDEC:
 *
 * 1. `clk_qdec`: selected by `mode` (`SYSCTRL_CLK_SLOW` or `SYSCTRL_CLK_HCLK`);
 * 1. `clk_qdec_div`: divided from `qdec_clk`, specified by `div`.
 *
 * Note: `clk_qdec` must be >= PClk (see `SYSCTRL_GetPClk()`).
 *
 * @param mode          clock mode
 * @param div           clock divider (10 bits)
 *
 */
void SYSCTRL_SelectQDECClk(SYSCTRL_ClkMode mode, uint16_t div);

/**
 * @brief Select clock of 32k which can be used by IR/WDT/GPIO/KeyScan
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `(SYSCTRL_ClkMode)N`,
 *  where N is in [1..0xfff], `SYSCTRL_CLK_32k` is referring to the internal 32k
 *  clock source (32k OSC or 32k RC).
 *
 * Note: The default mode is (`(SYSCTRL_ClkMode)750`), i.e. (SLOW_CLK / 750).
 *
 * @param mode                  clock mode
 */
void SYSCTRL_SelectCLK32k(SYSCTRL_ClkMode mode);

/**
 * @brief Get the frequency of 32k which can be used by IR/WDT/GPIO/KeyScan
 *
 * @return                      frequency of the 32k
 */
int SYSCTRL_GetCLK32k(void);

// default PLL settings for automatic config after wakeup in boot loader
#define PLL_BOOT_DEF_DIV_PRE        5
#define PLL_BOOT_DEF_LOOP           70
#define PLL_BOOT_DEF_DIV_OUTPUT     1

// default hardware PLL settings when automatic config after wakeup in boot loader
// is disabled
#define PLL_HW_DEF_DIV_PRE          5
#define PLL_HW_DEF_LOOP             80
#define PLL_HW_DEF_DIV_OUTPUT       1

/**
 * @brief Automatically config core clocks after wakeup
 *
 * There is a functionality in the boot loader which can automatically configure
 * core clocks after wakeup. This function update its parameters.
 *
 * This functionality in the boot loader is Enabled by default with following parameters:
 * - enable_pll:        1
 * - pll_loop:          PLL_BOOT_DEF_LOOP
 * - hclk:              SYSCTRL_CLK_PLL_DIV_3
 * - flash_clk:         SYSCTRL_CLK_PLL_DIV_2
 * - enable_watchdog:   Disabled
 *
 * Note: For PLL, `div_pre` and `div_output` are fixed as PLL_BOOT_DEF_DIV_PRE and
 *                PLL_BOOT_DEF_DIV_OUTPUT respectively.
 * So, PLL output is 336MHz, HClk is 112MHz, Flash clock is 168MHz.
 *
 * @param   enable_pll          enable(1)/disable(0) PLL
 * @param   pll_loop            loop of PLL (see `SYSCTRL_ConfigPLLClk`)
 *                              ignored when PLL is disabled
 * @param   hclk                HCLK clock mode (see `SYSCTRL_SelectHClk`)
 * @param   flash_clk           Flash clock mode (see `SYSCTRL_SelectFlashClk`)
 * @param   enable_watchdog    enable(1)/disable(0) watchdog
 *                              When enabled, watchdog is configured to be timed out
 *                              after about 3 seconds. Developer are free to update
 *                              its configuration later.
 */
void SYSCTRL_EnableConfigClocksAfterWakeup(uint8_t enable_pll, uint8_t pll_loop,
        SYSCTRL_ClkMode hclk,
        SYSCTRL_ClkMode flash_clk,
        uint8_t enable_watchdog);

/**
 * @brief Disable automatic configuration of core clocks after wakeup
 *
 * Once disabled, after wake up, following settings of core clocks apply:
 * - PLL enable is kept with `div_pre`, `loop` and `div_output` defaults to 5, 80 and 1 respectively;
 * - hclk:          SYSCTRL_CLK_SLOW
 * - flash_clk:     SYSCTRL_CLK_SLOW
 */
void SYSCTRL_DisableConfigClocksAfterWakeup(void);

typedef enum
{
    SYSCTRL_CPU_32k_CLK_32k = 0,    // use the clock configured by `SYSCTRL_SelectCLK32k`
    SYSCTRL_CPU_32k_INTERNAL = 1,   // use the internal 32k clock source (32k OSC or 32k RC)
} SYSCTRL_CPU32kMode;

/**
 * @brief Select clock of 32k for MCU
 *
 * Note: The default mode is `SYSCTRL_CPU_32k_INTERNAL`.
 *
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectCPU32k(SYSCTRL_CPU32kMode mode);

/**
 * @brief Get the frequency of CPU 32k
 *
 * @return                      frequency of CPU 32k
 */
int SYSCTRL_GetCPU32k(void);

typedef enum
{
    SYSCTRL_SLOW_RC_CLK = 0,        // RC clock (which is tunable)
    SYSCTRL_SLOW_CLK_24M_RF = 1,    // 24MHz RF OSC clock (default)
} SYSCTRL_SlowClkMode;

/**
 * @brief Select clock source of slow clock
 *
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectSlowClk(SYSCTRL_SlowClkMode mode);

/**
 * @brief Get current slow clock in Hz
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetSlowClk(void);

/**
 * @brief Enable/Disable PLL
 *
 * Caution: Before turning off PLL, all parts (such as Flash) using PLL clock
 * must be switched to other clocks.
 *
 * @param enable        Enable(1)/Disable(0)
 *
 */
void SYSCTRL_EnablePLL(uint8_t enable);

typedef enum
{
    SYSCTRL_SLOW_RC_8M = 0,
    SYSCTRL_SLOW_RC_16M = 1,
    SYSCTRL_SLOW_RC_24M = 3,
    SYSCTRL_SLOW_RC_32M = 7,
    SYSCTRL_SLOW_RC_48M = 0xf,
} SYSCTRL_SlowRCClkMode;

/**
 * @brief Enable/Disable RC clock for slow clock
 *
 * @param enable        Enable(1)/Disable(0)
 * @param mode          Clock frequency mode
 *                      Ignored if (enable == 0)
 *
 */
void SYSCTRL_EnableSlowRC(uint8_t enable, SYSCTRL_SlowRCClkMode mode);

/**
 * @brief Tune the RC clock for slow clock to the frequency given by
 * `SYSCTRL_SlowRCClkMode` automatically.
 *
 * Note: 1. The returned value can be stored in NVM for later use (see `SYSCTRL_TuneSlowRC`)
 *       2. The internal configuration is different for different `SYSCTRL_SlowRCClkMode`.
 *
 * @return              the internal configuration after tunning
 *
 */
uint32_t SYSCTRL_AutoTuneSlowRC(void);

/**
 * @brief Set the internal configuration the RC clock for slow clock
 *
 * @param value         the internal configuration which is returned from
 *                      `SYSCTRL_AutoTuneSlowRC` to tune the clock
 */
void SYSCTRL_TuneSlowRC(uint32_t value);

/**
 * @brief Configure clock output functionality
 *
 * There is a dedicated divider dividing PLL output. The output of this divider
 * can be routed to some PINs for debugging (see `PINCTRL_SelClockOutput`).
 *
 * @param enable        Enable(1) or Disable(0) this divider
 * @param denom         denominator (10 bits) (ignored when `enabled` is 0)
 */
void SYSCTRL_EnableClockOutput(uint8_t enable, uint16_t denom);

typedef enum
{
    SYSCTRL_DMA_UART0_RX = 0,
    SYSCTRL_DMA_UART1_RX = 1,
    SYSCTRL_DMA_SPI0_TX  = 2,
    SYSCTRL_DMA_SPI1_TX  = 3,
    SYSCTRL_DMA_I2C0     = 4,
    SYSCTRL_DMA_QDEC0    = 5,
    SYSCTRL_DMA_PWM1     = 6,
    SYSCTRL_DMA_I2S_RX   = 7,
    SYSCTRL_DMA_PDM      = 8,
    SYSCTRL_DMA_ADC      = 9,

    SYSCTRL_DMA_UART0_TX = 0x10,
    SYSCTRL_DMA_UART1_TX = 0x11,
    SYSCTRL_DMA_SPI0_RX  = 0x12,
    SYSCTRL_DMA_SPI1_RX  = 0x13,
    SYSCTRL_DMA_I2C1     = 0x14,
    SYSCTRL_DMA_QDEC1    = 0x15,
    SYSCTRL_DMA_KeyScan  = 0x16,
    SYSCTRL_DMA_I2S_TX   = 0x17,
    SYSCTRL_DMA_PWM0     = 0x18,
    SYSCTRL_DMA_PWM2     = 0x19,
    SYSCTRL_DMA_QDEC2    = 0x1A,
    SYSCTRL_DMA_LAST     = SYSCTRL_DMA_QDEC2,
} SYSCTRL_DMA;

/**
 * @brief Select used DMA items
 * @param items         combination of bits whose positions are listed in `SYSCTRL_DMA`
 * @return              0 if no error occurred else non-0
 *
 * Note:
 *   1. This function should be used when using DMA on hardware peripherals.
 *   1. This configuration is recommended to be fixed and not to be modified dynamically.
 *   1. If too many items are configured, errors will occur.
 */
int SYSCTRL_SelectUsedDmaItems(uint32_t items);

/**
 * @brief Get the DMA request ID of an item
 * @param item          Previously configured `SYSCTRL_DMA` in `SYSCTRL_SelectUsedDmaItems`
 * @return              request ID of the item (if not found, -1 is returned)
 */
int SYSCTRL_GetDmaId(SYSCTRL_DMA item);

/**
 * @brief Set LDO output level for Flash
 *
 * @param[in] level         output level
 */
void SYSCTRL_SetLDOOutputFlash(SYSCTRL_LDOOutputFlash level);

/**
 * @brief LDO for RF output level
 *
 * Range: [1.200, 2.750]V
 *
 * Default: 1.500V. Step: 50mV
 */
typedef enum
{
    SYSCTRL_LDO_RF_OUTPUT_1V200 = 0,    // 1.200V
    SYSCTRL_LDO_RF_OUTPUT_1V250 = 1,
    SYSCTRL_LDO_RF_OUTPUT_1V300 = 2,
    SYSCTRL_LDO_RF_OUTPUT_1V350 = 3,
    SYSCTRL_LDO_RF_OUTPUT_1V400 = 4,
    SYSCTRL_LDO_RF_OUTPUT_1V450 = 5,
    SYSCTRL_LDO_RF_OUTPUT_1V500 = 6,
    SYSCTRL_LDO_RF_OUTPUT_1V550 = 7,
    SYSCTRL_LDO_RF_OUTPUT_1V600 = 8,
    SYSCTRL_LDO_RF_OUTPUT_1V650 = 9,
    SYSCTRL_LDO_RF_OUTPUT_1V700 = 10,
    SYSCTRL_LDO_RF_OUTPUT_1V750 = 11,
    SYSCTRL_LDO_RF_OUTPUT_1V800 = 12,
    SYSCTRL_LDO_RF_OUTPUT_1V850 = 13,
    SYSCTRL_LDO_RF_OUTPUT_1V900 = 14,
    SYSCTRL_LDO_RF_OUTPUT_1V950 = 15,
    SYSCTRL_LDO_RF_OUTPUT_2V000 = 16,
    SYSCTRL_LDO_RF_OUTPUT_2V050 = 17,
    SYSCTRL_LDO_RF_OUTPUT_2V100 = 18,
    SYSCTRL_LDO_RF_OUTPUT_2V150 = 19,
    SYSCTRL_LDO_RF_OUTPUT_2V200 = 20,
    SYSCTRL_LDO_RF_OUTPUT_2V250 = 21,
    SYSCTRL_LDO_RF_OUTPUT_2V300 = 22,
    SYSCTRL_LDO_RF_OUTPUT_2V350 = 23,
    SYSCTRL_LDO_RF_OUTPUT_2V400 = 24,
    SYSCTRL_LDO_RF_OUTPUT_2V450 = 25,
    SYSCTRL_LDO_RF_OUTPUT_2V500 = 26,
    SYSCTRL_LDO_RF_OUTPUT_2V550 = 27,
    SYSCTRL_LDO_RF_OUTPUT_2V600 = 28,
    SYSCTRL_LDO_RF_OUTPUT_2V650 = 29,
    SYSCTRL_LDO_RF_OUTPUT_2V700 = 30,
    SYSCTRL_LDO_RF_OUTPUT_2V750 = 31,
} SYSCTRL_LDOOutputRF;

/**
 * @brief Set LDO output level for RF
 *
 * @param[in] level         output level
 */
void SYSCTRL_SetLDOOutputRF(SYSCTRL_LDOOutputRF level);

/**
 * @brief ADC V1.2 reference (VREF12_ADC) level
 *
 * Range: [1.184, 1.215]V
 *
 * Default: 1.200V. Step: 1mV
 */
typedef enum
{
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V184 = 0,  // 1.184V
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V185 = 1,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V186 = 2,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V187 = 3,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V188 = 4,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V189 = 5,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V190 = 6,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V191 = 7,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V192 = 8,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V193 = 9,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V194 = 10,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V195 = 11,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V196 = 12,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V197 = 13,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V198 = 14,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V199 = 15,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V200 = 16,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V201 = 17,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V202 = 18,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V203 = 19,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V204 = 20,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V205 = 21,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V206 = 22,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V207 = 23,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V208 = 24,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V209 = 25,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V210 = 26,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V211 = 27,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V212 = 28,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V213 = 29,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V214 = 30,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V215 = 31,
} SYSCTRL_AdcVrefOutput;

/**
 * @brief Set LDO ADC V1.2 reference (VREF12_ADC) output level
 *
 * @param[in] level         output level
 */
void SYSCTRL_SetAdcVrefOutput(SYSCTRL_AdcVrefOutput level);

/**
 * @brief BUCK DC-DC output level
 *
 * @see `SYSCTRL_SetLDOOutputFlash`
 *
 * Range: [1.200, 2.700]V
 *
 * Default: 1.800V. Step: 100mV
 */
typedef enum
{
    SYSCTRL_BUCK_DCDC_OUTPUT_1V200 = 0, // 1.2V
    SYSCTRL_BUCK_DCDC_OUTPUT_1V300 = 1,
    SYSCTRL_BUCK_DCDC_OUTPUT_1V400 = 2,
    SYSCTRL_BUCK_DCDC_OUTPUT_1V500 = 3,
    SYSCTRL_BUCK_DCDC_OUTPUT_1V600 = 4,
    SYSCTRL_BUCK_DCDC_OUTPUT_1V700 = 5,
    SYSCTRL_BUCK_DCDC_OUTPUT_1V800 = 6,
    SYSCTRL_BUCK_DCDC_OUTPUT_1V900 = 7,
    SYSCTRL_BUCK_DCDC_OUTPUT_2V000 = 8,
    SYSCTRL_BUCK_DCDC_OUTPUT_2V100 = 9,
    SYSCTRL_BUCK_DCDC_OUTPUT_2V200 = 10,
    SYSCTRL_BUCK_DCDC_OUTPUT_2V300 = 11,
    SYSCTRL_BUCK_DCDC_OUTPUT_2V400 = 12,
    SYSCTRL_BUCK_DCDC_OUTPUT_2V500 = 13,
    SYSCTRL_BUCK_DCDC_OUTPUT_2V600 = 14,
    SYSCTRL_BUCK_DCDC_OUTPUT_2V700 = 15,
} SYSCTRL_BuckDCDCOutput;

/**
 * @brief Set BUCK DC-DC output level
 *
 * @param[in] level         output level
 */
void SYSCTRL_SetBuckDCDCOutput(SYSCTRL_BuckDCDCOutput level);

/**
 * @brief Enable BUCK DC-DC
 *
 * Default: Enabled.
 *
 * @param[in] enable        enable(1)/disable(0)
 */
void SYSCTRL_EnableBuckDCDC(uint8_t enable);

/**
 * @brief Enable PVD interrupt
 *
 * Note: BOR of ING916 relies on both PVD (Power Voltage Detector) & PDR (Power Down Reset):
 *      * If BOR threshold > SYSCTRL_BOR_1V5:
 *          1) PDR disabled;
 *          2) PVD is enabled, threshold is configured, and PVD is configured to reset the SoC.
 *      * If BOR threshold == SYSCTRL_BOR_1V5:
 *          1) PVD disabled;
 *          2) PDR is enabled and configured to reset the SoC.
 *
 * Effects of PVD & PDR are: either raise an interrupt or reset the SoC.
 *
 * This function reconfigures PVD to raise an interrupt.
 *
 * @param[in] enable        Enable(1)/disable(0)
 * @param[in] polarity      Generate interrupt when V-bat becomes lower(0)/higher(1) than `level`
 * @param[in] level         trigger level (see SYSCTRL_BOR_...)
 */
void SYSCTRL_EnablePVDInt(uint8_t enable, uint8_t polarity, uint8_t level);

/**
 * @brief Clear PVD interrupt state
 */
void SYSCTRL_ClearPVDInt(void);

/**
 * @brief Enable PDR interrupt
 *
 * See also `SYSCTRL_EnablePVDInt`.
 *
 * @param[in] enable        Enable(1)/disable(0)
 */
void SYSCTRL_EnablePDRInt(uint8_t enable);

/**
 * @brief Clear PDR interrupt state
 */
void SYSCTRL_ClearPDRInt(void);

/**
 * @brief Config USB PHY functionality
 *
 * @param[in] enable            Enable(1)/Disable(0) usb phy module
 * @param[in] pull_sel          Pull selection (ignored when `enable` is 0)
 *                              0x1: DP pull up (Slave mode, full speed)
 *                              0x2: DM pull up (Slave mode, low speed)
 *                              0x3: DP & DM pull down (Host mode)
 *                              0x0: no pull from USB PHY
 */
void SYSCTRL_USBPhyConfig(uint8_t enable, uint8_t pull_sel);

#define SYSCTRL_WAKEUP_SOURCE_AUTO          1       // waken up automatically by internal timer
#define SYSCTRL_WAKEUP_SOURCE_COMPARATOR    2       // waken up by comparator
#define SYSCTRL_WAKEUP_SOURCE_RTC_ALARM     4       // waken up by RTC alarm

typedef struct
{
    uint64_t gpio;      // if any GPIO (bit n for GPIO #n) has triggered wake up
    uint32_t other;     // bit combination of `SYSCTRL_WAKEUP_SOURCE_...`
} SYSCTRL_WakeupSource_t;

/**
 * @brief Enable wake up source detection once
 *
 * After enabled, wake up source can be read through `SYSCTRL_GetLastWakeupSource`.
 *
 * This function only enable the detection for one time, and needs to be called
 * each time before entering DEEP/DEEPER sleep modes if apps need to know the
 * wake up sources.
 *
 */
void SYSCTRL_EnableWakeupSourceDetection(void);

/**
 * @brief Enable/Disable p_cap mode for a certain pwm channel
 *
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] enable            Enable(1)/Disable(0)
 */
void SYSCTRL_EnablePcapMode(const uint8_t channel_index, uint8_t enable);

typedef enum
{
    // SYS RAM block #0, 16KiB starting from 0x20000000 (0x20000000~0x20003fff)
    // This block is always ON, and can't be turned off.
    SYSCTRL_SYS_MEM_BLOCK_0 = 0x10,
    // SYS RAM block #1, 16KiB following block #0       (0x20004000~0x20007fff)
    SYSCTRL_SYS_MEM_BLOCK_1 = 0x08,
    // SYS RAM block #2, 16KiB following block #1       (0x20008000~0x2000bfff)
    // remapped from `SYSCTRL_MEM_REMAPPABLE_BLOCK_0`
    SYSCTRL_SYS_MEM_BLOCK_2 = 0x02,
    // SYS RAM block #3,  8KiB following block #2       (0x2000c000~0x2000dfff)
    // remapped from `SYSCTRL_MEM_REMAPPABLE_BLOCK_1`
    SYSCTRL_SYS_MEM_BLOCK_3 = 0x04,

    // SHARE RAM block #0, 8KiB starting from 0x40120000    (0x40120000~0x40121fff)
    // This shall not be turned off.
    SYSCTRL_SHARE_MEM_BLOCK_0 = 0x01,
    // SHARE RAM block #1, 8KiB following block #0          (0x40122000~0x40123fff)
    // remapped from `SYSCTRL_MEM_REMAPPABLE_BLOCK_1`
    SYSCTRL_SHARE_MEM_BLOCK_1 = 0x04,
    // SHARE RAM block #2,16KiB following block #1          (0x40124000~0x40127fff)
    // remapped from `SYSCTRL_MEM_REMAPPABLE_BLOCK_0`
    SYSCTRL_SHARE_MEM_BLOCK_2 = 0x02,

    // remapppable memory block 0, 16KiB
    // for `mini` and `noos_mini` bundles, this block is mapped to `SYSCTRL_SYS_MEM_BLOCK_2`
    // for other variants,                 this block is mapped to `SYSCTRL_SHARE_MEM_BLOCK_2`
    SYSCTRL_MEM_REMAPPABLE_BLOCK_0 = 0x02,
    // remapppable memory block 1, 8KiB
    // for `mini` and `noos_mini` bundles, this block is mapped to `SYSCTRL_SYS_MEM_BLOCK_3`
    // for other variants,                 this block is mapped to `SYSCTRL_SHARE_MEM_BLOCK_1`
    SYSCTRL_MEM_REMAPPABLE_BLOCK_1 = 0x04,

    // below definitions are kept for compatibility
    SYSCTRL_MEM_BLOCK_0 = 0x10,     // block 0 is 16KiB starting from 0x20000000
                                    // This block is always ON, and can't be turned off.
    SYSCTRL_MEM_BLOCK_1 = 0x08,     // block 1 is 16KiB following block 0

    SYSCTRL_SHARE_BLOCK_0 = 0x01,   // share memory block 0 is  8KiB starting from 0x40120000
    SYSCTRL_SHARE_BLOCK_1 = 0x02,   // share memory block 1 is 16KiB following block 2 (0x40124000)
    SYSCTRL_SHARE_BLOCK_2 = 0x04,   // share memory block 2 is  8KiB following block 0 (0x40122000)
} SYSCTRL_MemBlock;

// this blocks (16 + 8) KiB are reversed in _mini_bundles
#define SYSCTRL_RESERVED_MEM_BLOCKS (SYSCTRL_SYS_MEM_BLOCK_0 | SYSCTRL_SYS_MEM_BLOCK_1 | SYSCTRL_SHARE_MEM_BLOCK_0)

typedef enum
{
    SYSCTRL_MEM_BLOCK_AS_CACHE = 0,
    SYSCTRL_MEM_BLOCK_AS_SYS_MEM = 1,
} SYSCTRL_CacheMemCtrl;

/**
 * @brief Control the usage of cache memory blocks which can be used as Cache or
 *        system memory.
 *
 * Address and size of each block when used as system memory:
 *     1. D-Cache: 8KiB starting from `SYSCTRL_D_CACHE_AS_MEM_BASE_ADDR`
 *     2. I-Cache: 8KiB starting from `SYSCTRL_I_CACHE_AS_MEM_BASE_ADDR`
 *
 * CAUTION:
 *     1. When used as system memory, more RAM are available, but performance might be degraded;
 *     2. When used as system memory, contents are *NOT* retained in sleep modes;
 *     3. These settings are not kept in sleep modes and will restore to defaults during waking up.
 *
 * @param[in] i_cache           usage of I-Cache (default: AS_CACHE)
 * @param[in] d_cache           usage of D-Cache (default: AS_CACHE)
 */
void SYSCTRL_CacheControl(SYSCTRL_CacheMemCtrl i_cache, SYSCTRL_CacheMemCtrl d_cache);

#define SYSCTRL_D_CACHE_AS_MEM_BASE_ADDR 0x2000E000
#define SYSCTRL_I_CACHE_AS_MEM_BASE_ADDR 0x20010000

/**
 * @brief Control the usage of D-Cache
 *
 * Ref to `SYSCTRL_CacheControl`.
 *
 * @param[in] d_cache           usage of D-Cache (default: AS_CACHE)
 */
void SYSCTRL_DCacheControl(SYSCTRL_CacheMemCtrl d_cache);

/**
 * @brief Control the usage of I-Cache
 *
 * Ref to `SYSCTRL_CacheControl`.
 *
 * @param[in] i_cache           usage of I-Cache (default: AS_CACHE)
 */
void SYSCTRL_ICacheControl(SYSCTRL_CacheMemCtrl i_cache);

/**
 * @brief Flush I-Cache
 *
 */
void SYSCTRL_ICacheFlush(void);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

typedef enum
{
    SYSCTRL_ITEM_APB_GPIO0     ,
    SYSCTRL_ITEM_APB_GPIO1     ,
    SYSCTRL_ITEM_APB_TMR0      ,
    SYSCTRL_ITEM_APB_TMR1      ,
    SYSCTRL_ITEM_APB_WDT       ,
    SYSCTRL_ITEM_APB_PWM       ,
    SYSCTRL_ITEM_APB_QDEC      ,
    SYSCTRL_ITEM_APB_KeyScan   ,
    SYSCTRL_ITEM_APB_DMA       ,
    SYSCTRL_ITEM_AHB_SPI0      ,
    SYSCTRL_ITEM_APB_SPI1      ,
    SYSCTRL_ITEM_APB_ADC       ,
    SYSCTRL_ITEM_APB_I2S       ,
    SYSCTRL_ITEM_APB_UART0     ,
    SYSCTRL_ITEM_APB_UART1     ,
    SYSCTRL_ITEM_APB_I2C0      ,
    SYSCTRL_ITEM_APB_SysCtrl   ,
    SYSCTRL_ITEM_APB_PinCtrl   ,
    SYSCTRL_ITEM_APB_USB       ,
    SYSCTRL_ITEM_APB_ASDM       ,
    SYSCTRL_ITEM_APB_RTIMER0    ,
    SYSCTRL_ITEM_APB_RTIMER1    ,
    SYSCTRL_ITEM_APB_PTE        ,
    SYSCTRL_ITEM_APB_GPIOTE     ,
    SYSCTRL_ITEM_NUMBER,
} SYSCTRL_Item;

// compatible definitions with ING918xx
#define  SYSCTRL_ClkGate_APB_GPIO0              SYSCTRL_ITEM_APB_GPIO0
#define  SYSCTRL_ClkGate_APB_GPIO1              SYSCTRL_ITEM_APB_GPIO1
#define  SYSCTRL_ClkGate_APB_TMR0               SYSCTRL_ITEM_APB_TMR0
#define  SYSCTRL_ClkGate_APB_TMR1               SYSCTRL_ITEM_APB_TMR1
#define  SYSCTRL_ClkGate_APB_WDT                SYSCTRL_ITEM_APB_WDT
#define  SYSCTRL_ClkGate_APB_PWM                SYSCTRL_ITEM_APB_PWM
#define  SYSCTRL_ClkGate_APB_QDEC               SYSCTRL_ITEM_APB_QDEC
#define  SYSCTRL_ClkGate_APB_KeyScan            SYSCTRL_ITEM_APB_KeyScan
#define  SYSCTRL_ClkGate_APB_DMA                SYSCTRL_ITEM_APB_DMA
#define  SYSCTRL_ClkGate_AHB_SPI0               SYSCTRL_ITEM_AHB_SPI0
#define  SYSCTRL_ClkGate_APB_SPI1               SYSCTRL_ITEM_APB_SPI1
#define  SYSCTRL_ClkGate_APB_ADC                SYSCTRL_ITEM_APB_ADC
#define  SYSCTRL_ClkGate_APB_I2S                SYSCTRL_ITEM_APB_I2S
#define  SYSCTRL_ClkGate_APB_UART0              SYSCTRL_ITEM_APB_UART0
#define  SYSCTRL_ClkGate_APB_UART1              SYSCTRL_ITEM_APB_UART1
#define  SYSCTRL_ClkGate_APB_I2C0               SYSCTRL_ITEM_APB_I2C0
#define  SYSCTRL_ClkGate_APB_PinCtrl            SYSCTRL_ITEM_APB_PinCtrl
#define  SYSCTRL_ClkGate_APB_ASDM               SYSCTRL_ITEM_APB_ASDM
#define  SYSCTRL_ClkGate_APB_RTIMER0            SYSCTRL_ITEM_APB_RTIMER0
#define  SYSCTRL_ClkGate_APB_RTIMER1            SYSCTRL_ITEM_APB_RTIMER1
#define  SYSCTRL_ClkGate_APB_PTE                SYSCTRL_ITEM_APB_PTE
#define  SYSCTRL_ClkGate_APB_GPIOTE             SYSCTRL_ITEM_APB_GPIOTE

typedef SYSCTRL_Item SYSCTRL_ClkGateItem;

typedef SYSCTRL_Item SYSCTRL_ResetItem;

/**
 * @brief BOR (Brown-Out Reset) Threshold on VBAT
 *
 * This enumeration defines the threshold values for low voltage detection using PDR (Power Down Reset)
 * and PVD (Programmable Voltage Detector) on the ING20 series.
 *
 * - **PDR Thresholds**:
 *   - Range: 1.35V to 1.80V.
 *   - Available values: `SYSCTRL_PDR_1V35` to `SYSCTRL_PDR_1V80`.
 *   - When a PDR threshold is selected, PDR is enabled, and PVD is disabled.
 *
 * - **PVD Thresholds**:
 *   - Mode 0 (VM pin connected to GND):
 *     - Range: 1.51V to 1.69V.
 *     - Available values: `SYSCTRL_PVD_1V51` to `SYSCTRL_PVD_1V69`.
 *   - Mode 1 (VM pin connected to VCC):
 *     - Range: 2.38V to 3.19V.
 *     - Available values: `SYSCTRL_PVD_2V38` to `SYSCTRL_PVD_3V19`.
 *   - When a PVD threshold is selected, PVD is enabled, and PDR is disabled.
 *
 * - **Default Behavior**:
 *   - For packages without a VM pin, the default mode is Mode 0 (VM pin internally grounded),
 *     and only thresholds in the range of 1.51V to 1.69V are supported.
 *
 * @see `SYSCTRL_ConfigBOR`: Configures the BOR threshold and enables either PDR or PVD based on the selected threshold.
 * @see `SYSCTRL_EnablePVDInt`: Enables interrupt generation for PVD events.
 * @see `SYSCTRL_EnablePDRInt`: Enables interrupt generation for PDR events.
 */
enum
{
    SYSCTRL_PDR_1V35 = 0,
    SYSCTRL_PDR_1V44 = 1,
    SYSCTRL_PDR_1V50 = 2,
    SYSCTRL_PDR_1V56 = 3,
    SYSCTRL_PDR_1V62 = 4,
    SYSCTRL_PDR_1V68 = 5,
    SYSCTRL_PDR_1V74 = 6,
    SYSCTRL_PDR_1V80 = 7,
    SYSCTRL_PDR_END = SYSCTRL_PDR_1V80,
    SYSCTRL_PVD_1V51 = 8,
    SYSCTRL_PVD_1V54 = 9,
    SYSCTRL_PVD_1V56 = 10,
    SYSCTRL_PVD_1V59 = 11,
    SYSCTRL_PVD_1V61 = 12,
    SYSCTRL_PVD_1V64 = 13,
    SYSCTRL_PVD_1V66 = 14,
    SYSCTRL_PVD_1V69 = 15,
    SYSCTRL_PVD_2V38 = SYSCTRL_PVD_1V51,
    SYSCTRL_PVD_2V48 = SYSCTRL_PVD_1V54,
    SYSCTRL_PVD_2V59 = SYSCTRL_PVD_1V56,
    SYSCTRL_PVD_2V70 = SYSCTRL_PVD_1V59,
    SYSCTRL_PVD_2V81 = SYSCTRL_PVD_1V61,
    SYSCTRL_PVD_2V93 = SYSCTRL_PVD_1V64,
    SYSCTRL_PVD_3V06 = SYSCTRL_PVD_1V66,
    SYSCTRL_PVD_3V19 = SYSCTRL_PVD_1V69,
    SYSCTRL_PVD_END = SYSCTRL_PVD_3V19,
};

typedef enum
{
    SOURCE_SLOW_CLK,
    SOURCE_32K_CLK,
    SOURCE_PLL_CLK,
} pre_clk_source_t;

typedef enum
{
    SYSCTRL_CLK_SLOW = 0,            // use slow clock
    SYSCTRL_CLK_32k = 0,             // use 32kHz clock
    SYSCTRL_CLK_FAST_PER = 1,        // use fast peripheral clock
    SYSCTRL_CLK_HCLK = 1,            // use HCLK (same as MCU)
    SYSCTRL_CLK_ADC_DIV = 1,         // use clock from ADC divider

    SYSCTRL_CLK_PLL_DIV_1 = 1,       // use (PLL clock div 1)
                                     // SYSCTRL_TMR_CLK_PLL_DIV_2: use (PLL clock div 2)
                                     // ..
                                     // SYSCTRL_TMR_CLK_PLL_DIV_15: use (PLL clock div 15)
                                     // Feel free to cast [1..15] to SYSCTRL_ClkMode
    SYSCTRL_CLK_PLL_DIV_2 = 2,
    SYSCTRL_CLK_PLL_DIV_3 = 3,
    SYSCTRL_CLK_PLL_DIV_4 = 4,
    SYSCTRL_CLK_PLL_DIV_5 = 5,
    SYSCTRL_CLK_PLL_DIV_6 = 6,
    SYSCTRL_CLK_PLL_DIV_7 = 7,
    SYSCTRL_CLK_PLL_DIV_8 = 8,
    SYSCTRL_CLK_PLL_DIV_9 = 9,
    SYSCTRL_CLK_PLL_DIV_10 = 10,
    SYSCTRL_CLK_PLL_DIV_11 = 11,
    SYSCTRL_CLK_PLL_DIV_12 = 12,
    SYSCTRL_CLK_PLL_DIV_13 = 13,
    SYSCTRL_CLK_PLL_DIV_14 = 14,
    SYSCTRL_CLK_PLL_DIV_15 = 15,
    
    SYSCTRL_CLK_FAST_PER_DIV1 = 20,     
    SYSCTRL_CLK_FAST_PER_DIV2 = 21,
    SYSCTRL_CLK_FAST_PER_DIV3 = 22,
    SYSCTRL_CLK_FAST_PER_DIV4 = 23,
    SYSCTRL_CLK_FAST_PER_DIV5 = 24,
    SYSCTRL_CLK_FAST_PER_DIV6 = 25,
    SYSCTRL_CLK_FAST_PER_DIV7 = 26,
    SYSCTRL_CLK_FAST_PER_DIV8 = 27,
    SYSCTRL_CLK_FAST_PER_DIV9 = 28,
    SYSCTRL_CLK_FAST_PER_DIV10 = 29,
    SYSCTRL_CLK_FAST_PER_DIV11 = 30,
    SYSCTRL_CLK_FAST_PER_DIV12 = 31,
    SYSCTRL_CLK_FAST_PER_DIV13 = 32,
    SYSCTRL_CLK_FAST_PER_DIV14 = 33,
    SYSCTRL_CLK_FAST_PER_DIV15 = 34,

    SYSCTRL_CLK_SLOW_DIV_1 = 1,      // use RF OSC clock div 1
                                     // SYSCTRL_CLK_SLOW_DIV_2: use (RF OSC clock div 2)
                                     // ..
                                     // Feel free to cast [1..15] to SYSCTRL_ClkMode
    SYSCTRL_CLK_SLOW_DIV_2 = 2,
    SYSCTRL_CLK_SLOW_DIV_3 = 3,
    SYSCTRL_CLK_SLOW_DIV_4 = 4,
    SYSCTRL_CLK_SLOW_DIV_5 = 5,
    SYSCTRL_CLK_SLOW_DIV_6 = 6,
    SYSCTRL_CLK_SLOW_DIV_7 = 7,
    SYSCTRL_CLK_SLOW_DIV_8 = 8,
    SYSCTRL_CLK_SLOW_DIV_9 = 9,
    SYSCTRL_CLK_SLOW_DIV_10 = 10,
    SYSCTRL_CLK_SLOW_DIV_11 = 11,
    SYSCTRL_CLK_SLOW_DIV_12 = 12,
    SYSCTRL_CLK_SLOW_DIV_13 = 13,
    SYSCTRL_CLK_SLOW_DIV_14 = 14,
    SYSCTRL_CLK_SLOW_DIV_15 = 15,
    SYSCTRL_CLK_SLOW_DIV_4095 = 4095,
} SYSCTRL_ClkMode;

/**
 * @brief Get system rest source
 *
 * SYSCTRL_RESET_PDR: PDR module tigger reset.
 * SYSCTRL_RESET_PVD: PVD module tigger reset.
 * SYSCTRL_RESET_GLOBAL_SOFT: register reset is set.
 * SYSCTRL_RESET_CPU: CPU core reset.
 * SYSCTRL_RESET_PIN: Pressing the external reset button.
 * SYSCTRL_RESET_WDT: Watchdog timeout.
 * SYSCTRL_RESET_POR: Power on reset or feature is note open.
 */
typedef enum
{
    SYSCTRL_RESET_PDR = 0x4,
    SYSCTRL_RESET_PVD = 0x8,
    SYSCTRL_RESET_GLOBAL_SOFT = 0x10,
    SYSCTRL_RESET_CPU = 0x20,
    SYSCTRL_RESET_PIN = 0x31,
    SYSCTRL_RESET_WDT = 0x32,
    SYSCTRL_RESET_POR = 0x3f,
} SYSCTRL_ResetSource;

/**
 * @brief Select clock mode of TIMER
 *
 * All timers share the same clock divider, which means that if timer K is
 * set to use (SYSCTRL_CLK_SLOW_DIV_X), all previously configures timers that
 * uses (SYSCTRL_CLK_SLOW_DIV_X) are overwritten by (SYSCTRL_CLK_SLOW_DIV_X).
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_N`, where N = 1..15;
 *
 * @param port          the timer
 * @param div           clock divider
 * @param source        clock source
 *
 */
void SYSCTRL_SelectTimerClk(timer_port_t port, uint8_t div, pre_clk_source_t source);

/**
 * @brief Select clock mode of PWM
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_N`, where N = 1..15;
 *
 * @param div           clock divider
 * @param source        clock source
 *
 */
void SYSCTRL_SelectPWMClk(uint8_t div, pre_clk_source_t source);

/**
 * @brief Select clock mode of KeyScan
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_N`, where N = 1..15;
 *
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectKeyScanClk(SYSCTRL_ClkMode mode);

/**
 * @brief Select SPI clock mode
 * @param port          the port
q * @param mode          clock mode
 *
 * Note: For SPI0: mode should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;
 *       For SPI1: mode should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_FAST_PER`.
 */
void SYSCTRL_SelectSpiClk(spi_port_t port, SYSCTRL_ClkMode mode);

/**
 * @brief Select UART clock mode
 * @param port          the port
 * @param mode          clock mode (SYSCTRL_CLK_SLOW, or SYSCTRL_CLK_FAST_PER)
 */
void SYSCTRL_SelectUartClk(uart_port_t port, SYSCTRL_ClkMode mode);

/**
 * @brief Select I2S clock mode
 * @param mode          clock mode
 *
 * Note: mode should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;.
 */
void SYSCTRL_SelectI2sClk(SYSCTRL_ClkMode mode);

/**
 * @brief Get current PLL clock in Hz
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetPLLClk(void);

/**
 * @brief Config PLL clock in Hz
 *
 *               f_in * loop
 * f_vco = ------------------------
 *                  div_pre
 *
 *                 f_vco
 * f_pll = ------------------------
 *               div_output
 *
 * Requirements:
 *    1. f_in/div_pref should be in [2, 30]MHz, where f_in is slow clock;
 *    1. f_vco should be in [60, 600]MHz;
 *    1. f_pll should not exceed 500MHz.
 *
 * @param div_pre       div_pre (8 bits)
 * @param loop          loop (6 bits)
 * @param div_output    div_output (6 bits)
 * @return              0 if config is proper else non-0
 */
int SYSCTRL_ConfigPLLClk(uint32_t div_pre, uint32_t loop, uint32_t div_output);

/**
 * @brief Select HClk clock mode
 * @param mode          clock mode
 *
 * Note: mode should be SYSCTRL_CLK_SLOW, or SYSCTRL_CLK_PLL_DIV_N,
 *       where N = 1..15.
 *
 * Note: While changing, both clocks (OSC & PLL) must be running.
 */
void SYSCTRL_SelectHClk(SYSCTRL_ClkMode mode);

/**
 * @brief Get current HClk (same as MCU working clock) in Hz
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetHClk(void);

/**
 * @brief Select PClk (APB Clk) divider
 * @param div           divider (1..15)
 *
 * Note:  PClk is divided from HClk.
 */
void SYSCTRL_SetPClkDiv(uint8_t div);

/**
 * @brief Get PClk (APB Clk) divider
 * @return              divider (1..15)
 */
uint8_t SYSCTRL_GetPClkDiv(void);

/**
 * @brief Get current PClk in Hz
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetPClk(void);

/**
 * @brief Get the clock of an item in Hz
 * @param item          the item
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetClk(SYSCTRL_Item item);

/**
 * @brief Select ADC Clk divider
 * @param denom         denominator (6 bits)
  */
void SYSCTRL_SetAdcClkDiv(uint8_t denom);

/**
 * @brief Get ADC Clk from divider
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetAdcClkDiv(void);

/**
 * @brief Select Clk mode for a type of items
 * @param item          item of type A (IR/ADC/EFUSE)
 * @param mode          clock mode ({SYSCTRL_CLK_SLOW, SYSCTRL_CLK_ADC_DIV})
 */
void SYSCTRL_SelectTypeAClk(SYSCTRL_Item item, SYSCTRL_ClkMode mode);

/**
 * @brief Select clock mode of USB
 *
 * `mode` should be `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;
 *
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectUSBClk(SYSCTRL_ClkMode mode);

/**
 * @brief Select clock mode of Flash
 *
 * `mode` should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;
 *
 * Default mode: see `SYSCTRL_ConfigClocksAfterWakeup()`.
 *
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectFlashClk(SYSCTRL_ClkMode mode);

/**
 * @brief Get the frequency of Flash clock
 *
 * @return                      frequency
 */
uint32_t SYSCTRL_GetFlashClk(void);

/**
 * @brief Select the two clocks of QDEC
 *
 * This function configures two clocks for QDEC:
 *
 * 1. `clk_qdec`: selected by `mode` (`SYSCTRL_CLK_SLOW` or `SYSCTRL_CLK_FAST_PER`);
 * 1. `clk_qdec_div`: divided from `qdec_clk`, specified by `div`.
 *
 * Note: `clk_qdec` must be >= PClk (see `SYSCTRL_GetPClk()`).
 *
 * @param mode          clock mode
 * @param div           clock divider (10 bits)
 *
 */
void SYSCTRL_SelectQDECClk(SYSCTRL_ClkMode mode, uint16_t div);

/**
 * @brief Get the frequency of 32k which can be used by IR/WDT/GPIO/KeyScan
 *
 * @return                      frequency of the 32k
 */
uint32_t SYSCTRL_GetCLK32k(void);

// default PLL settings for automatic config after wakeup in boot loader
#define PLL_BOOT_DEF_DIV_PRE        5
#define PLL_BOOT_DEF_LOOP           70
#define PLL_BOOT_DEF_DIV_OUTPUT     1

// default hardware PLL settings when automatic config after wakeup in boot loader
// is disabled
#define PLL_HW_DEF_DIV_PRE          5
#define PLL_HW_DEF_LOOP             80
#define PLL_HW_DEF_DIV_OUTPUT       1

typedef enum
{
    SYSCTRL_SLOW_RC_CLK = 0,        // RC clock (which is tunable)
    SYSCTRL_SLOW_CLK_24M_RF = 1,    // 24MHz RF OSC clock (default)
} SYSCTRL_SlowClkMode;

/**
 * @brief Select clock source of slow clock
 *
 * @param mode          clock mode
 *
 */
void SYSCTRL_SelectSlowClk(SYSCTRL_SlowClkMode mode);

/**
 * @brief Get current slow clock in Hz
 * @return              clock in Hz
 */
uint32_t SYSCTRL_GetSlowClk(void);

/**
 * @brief Enable/Disable PLL
 *
 * Caution: Before turning off PLL, all parts (such as Flash) using PLL clock
 * must be switched to other clocks.
 *
 * @param enable        Enable(1)/Disable(0)
 *
 */
void SYSCTRL_EnablePLL(uint8_t enable);

typedef enum
{
    SYSCTRL_SLOW_RC_8M = 1,
    SYSCTRL_SLOW_RC_16M = 2,
    SYSCTRL_SLOW_RC_24M = 3,
    SYSCTRL_SLOW_RC_32M = 4,
    SYSCTRL_SLOW_RC_48M = 6,
    SYSCTRL_SLOW_RC_64M = 8,
} SYSCTRL_SlowRCClkMode;

/**
 * @brief Enable/Disable RC clock for slow clock
 *
 * @param enable        Enable(1)/Disable(0)
 * @param mode          Clock frequency mode
 *                      Ignored if (enable == 0)
 *
 */
void SYSCTRL_EnableSlowRC(uint8_t enable, SYSCTRL_SlowRCClkMode mode);

/**
 * @brief Set the internal configuration the RC clock for slow clock
 *
 * @param value         the internal configuration which is returned from
 *                      `SYSCTRL_AutoTuneSlowRC` to tune the clock
 *
 * @return [out] get turn vale.
 */
uint32_t SYSCTRL_AutoTuneSlowRC(SYSCTRL_SlowRCClkMode value);

/**
 * @brief Tune the slow RC clock
 *
 * @param tune          Tune value
 *
 */
void SYSCTRL_TuneSlowRC(uint32_t tune);
/**
 * @brief Configure clock output functionality
 *
 * There is a dedicated divider dividing PLL output. The output of this divider
 * can be routed to some PINs for debugging (see `PINCTRL_SelClockOutput`).
 *
 * @param enable        Enable(1) or Disable(0) this divider
 * @param denom         denominator (10 bits) (ignored when `enabled` is 0)
 */
void SYSCTRL_EnableClockOutput(uint8_t enable, uint16_t denom);


typedef enum
{
    SYSCTRL_DMA_UART0_RX = 0,
    SYSCTRL_DMA_UART1_RX,
    SYSCTRL_DMA_SPI0_TX,
    SYSCTRL_DMA_SPI1_TX,
    SYSCTRL_DMA_I2C0,
    SYSCTRL_DMA_QDEC0,
    SYSCTRL_DMA_PWM1,
    SYSCTRL_DMA_I2S_RX,
    SYSCTRL_DMA_SPIFLASH,
    SYSCTRL_DMA_SADC,
    SYSCTRL_DMA_ASDM_RX,

    SYSCTRL_DMA_UART0_TX = 0x10,
    SYSCTRL_DMA_UART1_TX,
    SYSCTRL_DMA_SPI0_RX,
    SYSCTRL_DMA_SPI1_RX,
    SYSCTRL_DMA_ASDM_TX,
    SYSCTRL_DMA_QDEC1,
    SYSCTRL_DMA_KeyScan,
    SYSCTRL_DMA_I2S_TX,
    SYSCTRL_DMA_PWM0,
    SYSCTRL_DMA_PWM2,
    SYSCTRL_DMA_QDEC2,
    SYSCTRL_DMA_SDADC_RX,
    SYSCTRL_DMA_LAST,
} SYSCTRL_DMA;
/**
 * @brief Select used DMA items
 * @param items         combination of bits whose positions are listed in `SYSCTRL_DMA`
 * @return              0 if no error occurred else non-0
 *
 * Note:
 *   1. This function should be used when using DMA on hardware peripherals.
 *   1. This configuration is recommended to be fixed and not to be modified dynamically.
 *   1. If too many items are configured, errors will occur.
 */
int SYSCTRL_SelectUsedDmaItems(uint32_t items);

/**
 * @brief Get the DMA request ID of an item
 * @param item          Previously configured `SYSCTRL_DMA` in `SYSCTRL_SelectUsedDmaItems`
 * @return              request ID of the item (if not found, -1 is returned)
 */
int SYSCTRL_GetDmaId(SYSCTRL_DMA item);

/**
 * @brief LDO for RF output level
 *
 * Range: [1.200, 2.750]V
 *
 * Default: 1.500V. Step: 50mV
 */
typedef enum
{
    SYSCTRL_LDO_RF_OUTPUT_1V200 = 0,    // 1.200V
    SYSCTRL_LDO_RF_OUTPUT_1V250 = 1,
    SYSCTRL_LDO_RF_OUTPUT_1V300 = 2,
    SYSCTRL_LDO_RF_OUTPUT_1V350 = 3,
    SYSCTRL_LDO_RF_OUTPUT_1V400 = 4,
    SYSCTRL_LDO_RF_OUTPUT_1V450 = 5,
    SYSCTRL_LDO_RF_OUTPUT_1V500 = 6,
    SYSCTRL_LDO_RF_OUTPUT_1V550 = 7,
    SYSCTRL_LDO_RF_OUTPUT_1V600 = 8,
    SYSCTRL_LDO_RF_OUTPUT_1V650 = 9,
    SYSCTRL_LDO_RF_OUTPUT_1V700 = 10,
    SYSCTRL_LDO_RF_OUTPUT_1V750 = 11,
    SYSCTRL_LDO_RF_OUTPUT_1V800 = 12,
    SYSCTRL_LDO_RF_OUTPUT_1V850 = 13,
    SYSCTRL_LDO_RF_OUTPUT_1V900 = 14,
    SYSCTRL_LDO_RF_OUTPUT_1V950 = 15,
    SYSCTRL_LDO_RF_OUTPUT_2V000 = 16,
    SYSCTRL_LDO_RF_OUTPUT_2V050 = 17,
    SYSCTRL_LDO_RF_OUTPUT_2V100 = 18,
    SYSCTRL_LDO_RF_OUTPUT_2V150 = 19,
    SYSCTRL_LDO_RF_OUTPUT_2V200 = 20,
    SYSCTRL_LDO_RF_OUTPUT_2V250 = 21,
    SYSCTRL_LDO_RF_OUTPUT_2V300 = 22,
    SYSCTRL_LDO_RF_OUTPUT_2V350 = 23,
    SYSCTRL_LDO_RF_OUTPUT_2V400 = 24,
    SYSCTRL_LDO_RF_OUTPUT_2V450 = 25,
    SYSCTRL_LDO_RF_OUTPUT_2V500 = 26,
    SYSCTRL_LDO_RF_OUTPUT_2V550 = 27,
    SYSCTRL_LDO_RF_OUTPUT_2V600 = 28,
    SYSCTRL_LDO_RF_OUTPUT_2V650 = 29,
    SYSCTRL_LDO_RF_OUTPUT_2V700 = 30,
    SYSCTRL_LDO_RF_OUTPUT_2V750 = 31,
} SYSCTRL_LDOOutputRF;

/**
 * @brief ADC V1.2 reference (VREF12_ADC) level
 *
 * Range: [1.184, 1.215]V
 *
 * Default: 1.200V. Step: 1mV
 */
typedef enum
{
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V184 = 0,  // 1.184V
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V185 = 1,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V186 = 2,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V187 = 3,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V188 = 4,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V189 = 5,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V190 = 6,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V191 = 7,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V192 = 8,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V193 = 9,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V194 = 10,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V195 = 11,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V196 = 12,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V197 = 13,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V198 = 14,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V199 = 15,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V200 = 16,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V201 = 17,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V202 = 18,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V203 = 19,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V204 = 20,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V205 = 21,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V206 = 22,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V207 = 23,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V208 = 24,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V209 = 25,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V210 = 26,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V211 = 27,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V212 = 28,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V213 = 29,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V214 = 30,
    SYSCTRL_ADC_VREF_1V2_OUTPUT_1V215 = 31,
} SYSCTRL_AdcVrefOutput;

/**
 * @brief BUCK DC-DC output level
 *
 * @see `SYSCTRL_SetLDOOutputFlash`
 *
 * Range: [1.200, 2.700]V
 *
 * Default: 1.800V. Step: 100mV
 */
typedef enum
{
    SYSCTRL_BUCK_DCDC_OUTPUT_1V200 = 0x3f, // 1.2V
    SYSCTRL_BUCK_DCDC_OUTPUT_1V300 = 0x35,
    SYSCTRL_BUCK_DCDC_OUTPUT_1V400 = 0x2a,
    SYSCTRL_BUCK_DCDC_OUTPUT_1V500 = 0x20,
    SYSCTRL_BUCK_DCDC_OUTPUT_1V600 = 0x15,
    SYSCTRL_BUCK_DCDC_OUTPUT_1V700 = 0xb,
    SYSCTRL_BUCK_DCDC_OUTPUT_1V800 = 0,// 1.8V
} SYSCTRL_BuckDCDCOutput;

/**
 * @brief Set BUCK DC-DC output level
 *
 * @param[in] level         output level
 *
 * @note
 * The output level is set to 1.4 V by default.
 * The DCDC output range is 1.2 V to 1.8 V, adjustable in approximately 95 mV steps.
 * The enumeration (SYSCTRL_BuckDCDCOutput) lists typical values in 100 mV increments.
 * A trim value of zero corresponds to the maximum output voltage (1.8 V).
 */
void SYSCTRL_SetBuckDCDCOutput(SYSCTRL_BuckDCDCOutput level);

/**
 * @brief Enable BUCK DC-DC
 *
 * Default: Enabled.
 *
 * @param[in] enable        enable(1)/disable(0)
 */
void SYSCTRL_EnableBuckDCDC(uint8_t enable);

/**
 * @brief Enable PVD interrupt
 *
 * Note:
 * ING916:
 *      BOR of ING916 relies on both PVD (Power Voltage Detector) & PDR (Power Down Reset):
 *      * If BOR threshold > SYSCTRL_BOR_1V5:
 *          1) PDR disabled;
 *          2) PVD is enabled, threshold is configured, and PVD is configured to reset the SoC.
 *      * If BOR threshold == SYSCTRL_BOR_1V5:
 *          1) PVD disabled;
 *          2) PDR is enabled and configured to reset the SoC.
 * ING20:
 *      BOR (Brown-Out Reset) of ING20 relies on both PVD (Programmable Voltage Detector) and PDR (Power Down Reset):
 *
 *      * If BOR threshold is set to SYSCTRL_PVD_1V51 (VM pin connected to GND, Mode 0):
 *          1) PDR is disabled;
 *          2) PVD is enabled, threshold is configured to 1.51V, and PVD is configured to reset the SoC.
 *
 *      * If BOR threshold is set to SYSCTRL_PVD_2V38 (VM pin connected to VCC, Mode 1):
 *          1) PDR is disabled;
 *          2) PVD is enabled, threshold is configured to 2.38V, and PVD is configured to reset the SoC.
 *
 *      * If BOR threshold is set to SYSCTRL_PDR_1V80:
 *          1) PVD is disabled;
 *          2) PDR is enabled and configured to reset the SoC when voltage drops below 1.80V.
 *
 *      - PVD and PDR cannot be enabled simultaneously for a given threshold configuration.
 *      - For packages without a VM pin, only Mode 0 is supported (VM pin is internally grounded),
 *        and the PVD detection range is fixed at 1.51V (SYSCTRL_PVD_1V51).
 *
 * Effects of PVD & PDR are: either raise an interrupt or reset the SoC.
 *
 * This function reconfigures PVD to raise an interrupt.
 *
 * @param[in] enable        Enable(1)/disable(0)
 * @param[in] polarity      Generate interrupt when V-bat becomes lower(0)/higher(1) than `level`
 * @param[in] level         trigger level (see SYSCTRL_BOR_...)
 */
void SYSCTRL_EnablePVDInt(uint8_t enable, uint8_t polarity, uint8_t level);

/**
 * @brief Clear PVD interrupt state
 */
void SYSCTRL_ClearPVDInt(void);

/**
 * @brief Enable PDR interrupt
 *
 * Note:
 * ING916:
 *      BOR of ING916 relies on both PVD (Power Voltage Detector) & PDR (Power Down Reset):
 *      * If BOR threshold > SYSCTRL_BOR_1V5:
 *          1) PDR disabled;
 *          2) PVD is enabled, threshold is configured, and PVD is configured to reset the SoC.
 *      * If BOR threshold == SYSCTRL_BOR_1V5:
 *          1) PVD disabled;
 *          2) PDR is enabled and configured to reset the SoC.
 * ING20:
 *      BOR (Brown-Out Reset) of ING20 relies on both PVD (Programmable Voltage Detector) and PDR (Power Down Reset):
 *
 *      * If BOR threshold is set to SYSCTRL_PVD_1V51 (VM pin connected to GND, Mode 0):
 *          1) PDR is disabled;
 *          2) PVD is enabled, threshold is configured to 1.51V, and PVD is configured to reset the SoC.
 *
 *      * If BOR threshold is set to SYSCTRL_PVD_2V38 (VM pin connected to VCC, Mode 1):
 *          1) PDR is disabled;
 *          2) PVD is enabled, threshold is configured to 2.38V, and PVD is configured to reset the SoC.
 *
 *      * If BOR threshold is set to SYSCTRL_PDR_1V80:
 *          1) PVD is disabled;
 *          2) PDR is enabled and configured to reset the SoC when voltage drops below 1.80V.
 *
 *      - PVD and PDR cannot be enabled simultaneously for a given threshold configuration.
 *      - For packages without a VM pin, only Mode 0 is supported (VM pin is internally grounded),
 *        and the PVD detection range is fixed at 1.51V (SYSCTRL_PVD_1V51).
 *
 * Effects of PVD & PDR are: either raise an interrupt or reset the SoC.
 *
 * This function reconfigures PVD to raise an interrupt.
 *
 * @param[in] enable        Enable(1)/disable(0)
 * @param[in] level         trigger level (see SYSCTRL_BOR_...)
 */
void SYSCTRL_EnablePDRInt(uint8_t enable, uint8_t level);

/**
 * @brief Clear PDR interrupt state
 */
void SYSCTRL_ClearPDRInt(void);

/**
 * @brief Config USB PHY functionality
 *
 * @param[in] enable            Enable(1)/Disable(0) usb phy module
 * @param[in] pull_sel          Pull selection (ignored when `enable` is 0)
 *                              0x1: DP pull up (Slave mode, full speed)
 *                              0x2: DM pull up (Slave mode, low speed)
 *                              0x3: DP & DM pull down (Host mode)
 *                              0x0: no pull from USB PHY
 */
void SYSCTRL_USBPhyConfig(uint8_t enable, uint8_t pull_sel);

#define SYSCTRL_WAKEUP_SOURCE_AUTO          1       // waken up automatically by internal timer

typedef struct
{
    uint64_t gpio;      // if any GPIO (bit n for GPIO #n) has triggered wake up
    uint32_t other;     // bit combination of `SYSCTRL_WAKEUP_SOURCE_...`
} SYSCTRL_WakeupSource_t;

/**
 * @brief Enable wake up source detection once
 *
 * After enabled, wake up source can be read through `SYSCTRL_GetLastWakeupSource`.
 *s
 * This function only enable the detection for one time, and needs to be called
 * each time before entering DEEP/DEEPER sleep modes if apps need to know the
 * wake up sources.
 *
 */
void SYSCTRL_EnableWakeupSourceDetection(void);

/**
 * @brief Enable/Disable p_cap mode for a certain pwm channel
 *
 * @param[in] channel_index     channel index (0 .. PWM_CHANNEL_NUMBER - 1)
 * @param[in] enable            Enable(1)/Disable(0)
 */
void SYSCTRL_EnablePcapMode(uint8_t channel_index, uint8_t enable);

typedef enum
{
    // remapppable memory block 1, 16KiB
    // for `mini` and `noos_mini` bundles, this block is mapped to `SYSCTRL_SYS_MEM_BLOCK_2`
    // for other variants,                 this block is mapped to `SYSCTRL_SHARE_MEM_BLOCK_2`
    SYSCTRL_MEM_REMAPPABLE_BLOCK_0 = 0x04,
    // remapppable memory block 2, 8KiB
    // for `mini` and `noos_mini` bundles, this block is mapped to `SYSCTRL_SYS_MEM_BLOCK_3`
    // for other variants,                 this block is mapped to `SYSCTRL_SHARE_MEM_BLOCK_1`
    SYSCTRL_MEM_REMAPPABLE_BLOCK_1 = 0x02,

    // SYS RAM block #0, 16KiB starting from 0x20000000 (0x20000000~0x20003fff)
    SYSCTRL_SYS_BLOCK00 = 0x10,
    SYSCTRL_SYS_BLOCK01 = 0x20,
    SYSCTRL_SYS_MEM_BLOCK_0 = SYSCTRL_SYS_BLOCK00|SYSCTRL_SYS_BLOCK01,
    // SYS RAM block #1, 8KiB following block #0        (0x20004000~0x20005fff)
    SYSCTRL_SYS_MEM_BLOCK_1 = 0x08,
    // SYS RAM block #2, 16KiB following block #1       (0x20006000~0x20009fff)
    // remapped from `SYSCTRL_MEM_REMAPPABLE_BLOCK_0`
    SYSCTRL_SYS_MEM_BLOCK_2 = SYSCTRL_MEM_REMAPPABLE_BLOCK_0,
    // SYS RAM block #3,  8KiB following block #2       (0x2000a000~0x2000bfff)
    // remapped from `SYSCTRL_MEM_REMAPPABLE_BLOCK_1`
    SYSCTRL_SYS_MEM_BLOCK_3 = SYSCTRL_MEM_REMAPPABLE_BLOCK_1,

    // SHARE RAM block #0, 8KiB starting from 0x40120000    (0x40120000~0x40121fff)
    SYSCTRL_SHARE_MEM_BLOCK_0 = 0x01,
    // SHARE RAM block #1, 8KiB following block #0          (0x40122000~0x40123fff)
    // remapped from `SYSCTRL_MEM_REMAPPABLE_BLOCK_1`
    SYSCTRL_SHARE_MEM_BLOCK_1 = SYSCTRL_MEM_REMAPPABLE_BLOCK_1,
    // SHARE RAM block #2,16KiB following block #1          (0x40124000~0x40127fff)
    // remapped from `SYSCTRL_MEM_REMAPPABLE_BLOCK_0`
    SYSCTRL_SHARE_MEM_BLOCK_2 = SYSCTRL_MEM_REMAPPABLE_BLOCK_0,

    // below definitions are kept for compatibility
    SYSCTRL_MEM_BLOCK_0 = SYSCTRL_SYS_MEM_BLOCK_0,
    SYSCTRL_MEM_BLOCK_1 = SYSCTRL_SYS_MEM_BLOCK_1,

    SYSCTRL_SHARE_BLOCK_0 = SYSCTRL_SHARE_MEM_BLOCK_0,
    SYSCTRL_SHARE_BLOCK_1 = SYSCTRL_SHARE_MEM_BLOCK_1,
    SYSCTRL_SHARE_BLOCK_2 = SYSCTRL_SHARE_MEM_BLOCK_2,
    SYSCTRL_CACHE_BLOCK = 0x40,

} SYSCTRL_MemBlock;

// this blocks (16 + 8) KiB are reversed in _mini_bundles
#define SYSCTRL_RESERVED_MEM_BLOCKS (SYSCTRL_SYS_MEM_BLOCK_0 | SYSCTRL_SYS_MEM_BLOCK_1 | SYSCTRL_SHARE_MEM_BLOCK_0 | SYSCTRL_CACHE_BLOCK)

typedef enum
{
    SYSCTRL_MEM_BLOCK_AS_CACHE = 0,
    SYSCTRL_MEM_BLOCK_AS_SYS_MEM = 1,
} SYSCTRL_CacheMemCtrl;

#define SYSCTRL_I_CACHE_AS_MEM_BASE_ADDR 0x2000C000

/**
 * @brief Configure memory retention settings for different sleep modes.
 *
 * This function controls which memory blocks remain powered (retained) during
 * low-power modes such as shutdown or deep sleep.
 *
 * - In **shutdown mode (mode = 0)**: Only MEM0 and MEM1 (each 8 KB) can be retained.
 * - In **deep sleep mode (mode = 1)**: Any combination of memory blocks can be retained,
 *   but retaining more blocks increases power consumption.
 *
 * @param[in] mode      Sleep mode selection:
 *                      - 0: Shutdown mode (only MEM0 and MEM1 can be retained)
 *                      - 1: Deep sleep mode (configurable retention)
 * @param[in] block_map Bitmap indicating which memory blocks to retain.
 *                      Each bit corresponds to a memory block, as defined in
 *                      the enumeration #SYSCTRL_MemBlock.
 *                      Example: (1 << SYSCTRL_MEM0) | (1 << SYSCTRL_MEM1)
 *
 * @return              0 if successful, 1 if failed (invalid input)
 *
 * @note                For shutdown mode, only bits corresponding to MEM0 and MEM1 are valid.
 *                      Other bits will result in an error.
 */
uint8_t SYSCTRL_MemoryRetentionCtrl(uint8_t mode, uint32_t block_map);

/**
 * @brief Control the usage of I-Cache
 *
 * Ref to `SYSCTRL_CacheControl`.
 *
 * @param[in] i_cache           usage of I-Cache (default: AS_CACHE)
 */
void SYSCTRL_ICacheControl(SYSCTRL_CacheMemCtrl i_cache);

/**
 * @brief Flush I-Cache
 *
 */
void SYSCTRL_ICacheFlush(void);

/**
 * @brief Enable DCDC mode or switch back to LDO mode for the internal power supply.
 *
 * This function configures the chip's internal regulator to use either DCDC or LDO mode.
 * - Mode 0: Use LDO (Low-Dropout Regulator) mode. This is the default at power-on.
 * - Mode 1: Use DCDC (Buck Converter) mode.
 *
 * @param[in] mode  Power mode selection:
 *                  - 0: LDO mode
 *                  - 1: DCDC mode
 */
void SYSCTRL_EnableDCDCMode(uint8_t mode);

/**
 * @brief Enable or disable the internal voltage reference.
 *
 * @note The internal voltage reference is used by the ADC and ASDM. If your ADC and ASDM
 *     modules use the internal reference voltage source, you must enable it.
 *
 * @param enable [in] 1: enable, 0: disable
 */
void SYSCTRL_EnableInternalVref(uint8_t enable);

/**
 * @brief Enable or disable the ASDM Vref output.
 *
 * @param enable [in] 1: enable, 0: disable
 *
 * @note The ASDM Vref output is used by the ASDM module. If your ASDM module uses the ASDM
 *    Vref output, you must enable it. If you enable VrefOut, the external IO11 must
 *    be connected to a filter capacitor, otherwise it will affect the sound noise floor.
 * */
void SYSCTRL_EnableAsdmVrefOutput(uint8_t enable);

/**
 * @brief Set ADC reference voltage selection
 * @param val [in]
 */
//TODO
void SYSCTRL_SetAdcVrefSel(uint8_t val);

/**
 * @brief Update asdm clock
 *
 * This function update the asdm clock divider.
 *
 * @param[in]   div             divider
 */
void SYSCTRL_UpdateAsdmClk(uint32_t div);

/**
 * @brief Set FastPreCLK(Fast Peripheral CLK) divider
 * @param[in]   div             divider
 */
void SYSCTRL_SetFastPreDiv(uint8_t div);

/**
 * @brief current FastPreCLK(Fast Peripheral CLK) in Hz.
 */
uint32_t SYSCTRL_GetFastPreCLK(void);

/**
 * @brief Set Qdec pclk divider
 * @param[in]   div             divider
 */
void SYSCTRL_UpdateQdecClk(uint32_t div);

/**
 * @brief Enable feature for getting the current reset source.
 *
 * @param[in] enable 1 : enable 0: disable
 *
 * @note When this feature is enabled, the reset cause recording module will be reset;
 * therefore, the reset cause from the previous reset must be read prior to enabling this feature.
 */
void SYSCTRL_EnableResetSource(uint8_t enable);

/**
 * @brief Get reset source.
 * You can determine the previous reset cause
 * by checking the return value of this function during system startup.
 * The reset cause is one of the values from the 'SYSCTRL_ResetSource' enumeration.
 *
 * @return reset source.
 */
SYSCTRL_ResetSource SYSCTRL_GetResetSource(void);


#endif

/**
 * @brief Set clock gating state of a component
 * @param item      the component to enable clock gating
 */
void SYSCTRL_SetClkGate(SYSCTRL_ClkGateItem item);

/**
 * @brief Clear clock gating state of a component
 * @param item      the component to clear clock gating
 */
void SYSCTRL_ClearClkGate(SYSCTRL_ClkGateItem item);

/**
 * @brief Set clock gating state of several components
 * @param items      the bitmap of components to enable clock gating
 */
void SYSCTRL_SetClkGateMulti(uint32_t items);

/**
 * @brief Clear clock gating state of several components
 * @param items      the bitmap of components to clear clock gating
 */
void SYSCTRL_ClearClkGateMulti(uint32_t items);

/**
 * @brief Reset a component
 * @param item      the component to be reset
 */
void SYSCTRL_ResetBlock(SYSCTRL_ResetItem item);

/**
 * @brief Reset all components
 */
void SYSCTRL_ResetAllBlocks(void);

/**
 * @brief Release a component from reset
 * @param item      the component to be released
 */
void SYSCTRL_ReleaseBlock(SYSCTRL_ResetItem item);

#if(INGCHIPS_FAMILY != INGCHIPS_FAMILY_20)
/**
 * @brief Set LDO Core output level
 *
 * @param[in] level         output level (available values see `SYSCTRL_LDO_OUTPUT...`)
 */
void SYSCTRL_SetLDOOutput(SYSCTRL_LDOOutputCore level);

/**
 * @brief Wait for LDO state ready
 */
void SYSCTRL_WaitForLDO(void);

/**
 * @brief Select the set of memory blocks to be used and power off unused blocks.
 *
 * Note: Only allowed to be used in _mini_ bundles (and, for ING918 MCU mode,
 * i.e. without platform/BLE stack). NEVER use this in other ones.
 *
 * All blocks are selected as default.
 *
 * @param[in] block_map         combination of `SYSCTRL_MemBlock`
 *                              When a bit is absent from `block_map`, the corresponding
 *                              memory block is powered off.
 */
void SYSCTRL_SelectMemoryBlocks(uint32_t block_map);
#endif

/**
 * @brief Config BOR (Brownout Reset) functionality
 *
 * Note:
 *
 * ING918: Power consumption is larger when enabled, so it's not recommended to
 *         enable for sleep mode.
 *
 * ING916: `enable_active` and `enable_sleep` should be the same.
 *         Power consumption is larger when enabled, ~2.x uA.
 *         When `threshold` is set to 1.5V, a dedicated logic (PDR) is used, only ~0.x uA is
 *         consumed.
 *
 *         See also `SYSCTRL_EnablePVDInt`, `SYSCTRL_EnablePDRInt`.
 *
 * ING20: `enable_active` and `enable_sleep` should be the same.
 *         Power consumption is higher when PVD (Programmable Voltage Detector) is enabled,
 *         typically ~2.x uA. When using PDR (Power Down Reset) detection, power consumption
 *         is lower, typically ~0.x uA.
 *
 *         The PVD and PDR reset functions need to be enabled or disabled by setting the `threshold`
 *         value separately. Both PVD and PDR trigger a reset when the voltage drops below the
 *         specified threshold. By default, low voltage detection reset functionality is disabled.
 *
 *         PDR voltage detection range:
 *           - 1.35V to 1.80V
 *
 *         PVD voltage detection range depends on the chip package and VM pin configuration:
 *           - For chips with a VM pin:
 *               - If VM is connected to GND, the detection range is 1.51V to 1.69V.
 *               - If VM is connected to VCC, the detection range is 2.38V to 3.19V.
 *           - For chips without a VM pin (default VM = GND), the detection range is 1.51V to 1.69V.
 *
 *         See also `SYSCTRL_EnablePVDInt`, `SYSCTRL_EnablePDRInt`.
 *
 * @param[in] threshold         Threshold value for low voltage detection.
 *                              - For ING918 and ING916, use values defined in `SYSCTRL_BOR_...`.
 *                                Default thresholds:
 *                                  - 0.95V (ING918)
 *                                  - 2.30V (ING916)
 *
 *                              - For ING20, use values defined in `SYSCTRL_PDR_...` or `SYSCTRL_PVD_...`:
 *                                - When using PVD (Programmable Voltage Detector):
 *                                    - Mode 0 (VM pin connected to GND): 1.51V
 *                                    - Mode 1 (VM pin connected to VCC): 2.38V
 *                                - When using PDR (Power Down Reset): 1.80V
 *
 *                                Notes:
 *                                  - Mode 0 corresponds to VM pin grounded (default for packages without a VM pin).
 *                                  - Mode 1 corresponds to VM pin connected to VCC.
 *                                  - Packages without a VM pin default to Mode 0 and only support Mode 0.
 *
 * @param[in] enable_active     Enable(1)/Disable(0) for active mode
 *                              default: DISABLED
 * @param[in] enable_sleep      Enable(1)/Disable(0) for sleep mode
 *                              default: DISABLED
 */
void SYSCTRL_ConfigBOR(int threshold, int enable_active, int enable_sleep);

/**
 * @brief Get wake up source of last wake up from DEEP/DEEPER sleep
 *
 * @param[out] source           source of the last wake-up
 * @return                      1 if any wake up source is found else 0
 */
uint8_t SYSCTRL_GetLastWakeupSource(SYSCTRL_WakeupSource_t *source);

/**
 * @brief Extra initialization of system
 *
 * This function shall be called once after power up.
 *
 * For ING916:
 *      Use factory calibration data from Flash to initialize PMU registers/settings:
 *          - VREF 0.800V
 *          - VCore 1.200V
 *          - VAON 1.200V
 *
 *      Depend on `flash_get_factory_calib_data` of `eflash.c`.
 *
 *      This function uses `flash_prepare_factory_data()`.
 *
 * For ING918: This function does nothing.
 *
 * @return                      0 if initialized else failed
 */
int SYSCTRL_Init(void);

/**
 * @brief Delay a number of clock cycles roughly using loop
 *
 *
 * @param[in]   freq            clock frequency
 * @param[in]   cycles          number of cycles
 */
void SYSCTRL_DelayCycles(uint32_t freq, uint32_t cycles);

/**
 * @brief System reset
 *
 * This function resets the system with a watchdog,
 * and all registers are restored to their default state
 */
void SYSCTRL_Reset(void);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif
