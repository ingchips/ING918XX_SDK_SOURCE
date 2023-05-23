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
    SYSCTRL_BOR_0V95 = 0x09,        // BOR Vdd threshold = 0.95V
    SYSCTRL_BOR_1V00 = 0x09,        // BOR Vdd threshold = 1.00V
    SYSCTRL_BOR_1V05 = 0x0A,        // BOR Vdd threshold = 1.05V
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
 * \brief Get current HClk (same as MCU working clock) in Hz
 *
 * Compatible API with ING916's
 *
 * \return              clock in Hz
 */
#define SYSCTRL_GetHClk()       48000000

#define SYSCTRL_WAKEUP_SOURCE_AUTO          1       // waken up automatically by internal timer
#define SYSCTRL_WAKEUP_SOURCE_EXT_INT       2       // waken up by EXT_INT

typedef struct
{
    uint32_t source;     // bit combination of `SYSCTRL_WAKEUP_SOURCE_...`
} SYSCTRL_WakeupSource_t;

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
 * \brief Select clock mode of TIMER
 *
 * All timers share the same clock divider, which means that if timer K is
 * set to use (SYSCTRL_CLK_SLOW_DIV_X), all previously configures timers that
 * uses (SYSCTRL_CLK_SLOW_DIV_X) are overwritten by (SYSCTRL_CLK_SLOW_DIV_X).
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_N`, where N = 1..15;
 *
 * \param port          the timer
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectTimerClk(timer_port_t port, SYSCTRL_ClkMode mode);

/**
 * \brief Select clock mode of PWM
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_N`, where N = 1..15;
 *
 * \param port          the timer
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectPWMClk(SYSCTRL_ClkMode mode);

/**
 * \brief Select clock mode of KeyScan
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_N`, where N = 1..15;
 *
 * \param port          the timer
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectKeyScanClk(SYSCTRL_ClkMode mode);

/**
 * \brief Select clock mode of PDM
 *
 * Clock of PDM is divided from SLOW_CLK.`mode` should be `(SYSCTRL_ClkMode)N`, where N = 1..63;
 *
 * \param port          the timer
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectPDMClk(SYSCTRL_ClkMode mode);

/**
 * \brief Select SPI clock mode
 * \param port          the port
 * \param mode          clock mode
 *
 * Note: For SPI0: mode should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;
 *       For SPI1: mode should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_HCLK`.
 */
void SYSCTRL_SelectSpiClk(spi_port_t port, SYSCTRL_ClkMode mode);

/**
 * \brief Select UART clock mode
 * \param port          the port
 * \param mode          clock mode (SYSCTRL_CLK_SLOW, or SYSCTRL_CLK_HCLK)
 */
void SYSCTRL_SelectUartClk(uart_port_t port, SYSCTRL_ClkMode mode);

/**
 * \brief Select I2S clock mode
 * \param mode          clock mode
 *
 * Note: mode should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;.
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
 * \param div_pre       div_pre (8 bits)
 * \param loop          loop (6 bits)
 * \param div_output    div_output (6 bits)
 * \return              0 if config is proper else non-0
 */
int SYSCTRL_ConfigPLLClk(uint32_t div_pre, uint32_t loop, uint32_t div_output);

/**
 * \brief Select HClk clock mode
 * \param mode          clock mode
 *
 * Note: mode should be SYSCTRL_CLK_SLOW, or SYSCTRL_CLK_PLL_DIV_N,
 *       where N = 1..15.
 *
 * Note: While changing, both clocks (OSC & PLL) must be running.
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
 * \brief Get PClk (APB Clk) divider
 * \return              divider (1..15)
 */
uint8_t SYSCTRL_GetPClkDiv(void);

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
 * \param denom         denominator (6 bits)
  */
void SYSCTRL_SetAdcClkDiv(uint8_t denom);

/**
 * \brief Get ADC Clk from divider
 * \return              clock in Hz
 */
uint32_t SYSCTRL_GetAdcClkDiv(void);

/**
 * \brief Select Clk mode for a type of items
 * \param item          item of type A (IR/ADC/EFUSE)
 * \param mode          clock mode ({SYSCTRL_CLK_SLOW, SYSCTRL_CLK_ADC_DIV})
 */
void SYSCTRL_SelectTypeAClk(SYSCTRL_Item item, SYSCTRL_ClkMode mode);

/**
 * \brief Select clock mode of USB
 *
 * `mode` should be `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;
 *
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectUSBClk(SYSCTRL_ClkMode mode);

/**
 * \brief Select clock mode of Flash
 *
 * `mode` should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_PLL_DIV_N`, where N = 1..15;
 *
 * Default mode: see `SYSCTRL_ConfigClocksAfterWakeup()`.
 *
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectFlashClk(SYSCTRL_ClkMode mode);

/**
 * \brief Get the frequency of Flash clock
 *
 * \return                      frequency
 */
uint32_t SYSCTRL_GetFlashClk(void);

/**
 * \brief Select the two clocks of QDEC
 *
 * This function configures two clocks for QDEC:
 *
 * 1. `clk_qdec`: selected by `mode` (`SYSCTRL_CLK_SLOW` or `SYSCTRL_CLK_HCLK`);
 * 1. `clk_qdec_div`: divided from `qdec_clk`, specified by `div`.
 *
 * Note: `clk_qdec` must be >= PClk (see `SYSCTRL_GetPClk()`).
 *
 * \param mode          clock mode
 * \param div           clock divider (10 bits)
 *
 */
void SYSCTRL_SelectQDECClk(SYSCTRL_ClkMode mode, uint16_t div);

/**
 * \brief Select clock of 32k which can be used by IR/WDT/GPIO/KeyScan
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `(SYSCTRL_ClkMode)N`,
 *  where N is in [1..0xfff], `SYSCTRL_CLK_32k` is referring to the internal 32k
 *  clock source (32k OSC or 32k RC).
 *
 * Note: The default mode is (`(SYSCTRL_ClkMode)750`), i.e. (SLOW_CLK / 750).
 *
 * \param mode                  clock mode
 */
void SYSCTRL_SelectCLK32k(SYSCTRL_ClkMode mode);

/**
 * \brief Get the frequency of 32k which can be used by IR/WDT/GPIO/KeyScan
 *
 * \return                      frequency of the 32k
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
 * \brief Automatically config core clocks after wakeup
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
 * \param   enable_pll          enable(1)/disable(0) PLL
 * \param   pll_loop            loop of PLL (see `SYSCTRL_ConfigPLLClk`)
 *                              ignored when PLL is disabled
 * \param   hclk                HCLK clock mode (see `SYSCTRL_SelectHClk`)
 * \param   flash_clk           Flash clock mode (see `SYSCTRL_SelectFlashClk`)
 * \param   enabled_watchdog    enable(1)/disable(0) watchdog
 *                              When enabled, watchdog is configured to be timed out
 *                              after about 3 seconds. Developer are free to update
 *                              its configuration later.
 */
void SYSCTRL_EnableConfigClocksAfterWakeup(uint8_t enable_pll, uint8_t pll_loop,
        SYSCTRL_ClkMode hclk,
        SYSCTRL_ClkMode flash_clk,
        uint8_t enable_watchdog);

/**
 * \brief Disable automatic configuration of core clocks after wakeup
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
 * \brief Select clock of 32k for MCU
 *
 * Note: The default mode is `SYSCTRL_CPU_32k_INTERNAL`.
 *
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectCPU32k(SYSCTRL_CPU32kMode mode);

/**
 * \brief Get the frequency of CPU 32k
 *
 * \return                      frequency of CPU 32k
 */
int SYSCTRL_GetCPU32k(void);

typedef enum
{
    SYSCTRL_SLOW_RC_CLK = 0,        // RC clock (which is tunable)
    SYSCTRL_SLOW_CLK_24M_RF = 1,    // 24MHz RF OSC clock (default)
} SYSCTRL_SlowClkMode;

/**
 * \brief Select clock source of slow clock
 *
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectSlowClk(SYSCTRL_SlowClkMode mode);

/**
 * \brief Get current slow clock in Hz
 * \return              clock in Hz
 */
uint32_t SYSCTRL_GetSlowClk(void);

/**
 * \brief Enable/Disable PLL
 *
 * Caution: Before turning off PLL, all parts (such as Flash) using PLL clock
 * must be switched to other clocks.
 *
 * \param enable        Enable(1)/Disable(0)
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
 * \brief Enable/Disable RC clock for slow clock
 *
 * \param enable        Enable(1)/Disable(0)
 * \param mode          Clock frequency mode
 *                      Ignored if (enable == 0)
 *
 */
void SYSCTRL_EnableSlowRC(uint8_t enable, SYSCTRL_SlowRCClkMode mode);

/**
 * \brief Tune the RC clock for slow clock to the frequency given by
 * `SYSCTRL_SlowRCClkMode` automatically.
 *
 * Note: 1. The returned value can be stored in NVM for later use (see `SYSCTRL_TuneSlowRC`)
 *       2. The internal configuration is different for different `SYSCTRL_SlowRCClkMode`.
 *
 * \return              the internal configuration after tunning
 *
 */
uint32_t SYSCTRL_AutoTuneSlowRC(void);

/**
 * \brief Set the internal configuration the RC clock for slow clock
 *
 * \param value         the internal configuration which is returned from
 *                      `SYSCTRL_AutoTuneSlowRC` to tune the clock
 */
void SYSCTRL_TuneSlowRC(uint32_t value);

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
 * \brief Select used DMA items
 * \param items         combination of bits whose positions are listed in `SYSCTRL_DMA`
 * \return              0 if no error occurred else non-0
 *
 * Note:
 *   1. This function should be used when using DMA on hardware peripherals.
 *   1. This configuration is recommended to be fixed and not to be modified dynamically.
 *   1. If too many items are configured, errors will occur.
 */
int SYSCTRL_SelectUsedDmaItems(uint32_t items);

/**
 * \brief Get the DMA request ID of an item
 * \param item          Previously configured `SYSCTRL_DMA` in `SYSCTRL_SelectUsedDmaItems`
 * \return              request ID of the item (if not found, -1 is returned)
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
 * @param[in] pull_sel          DP pull up(0x1)/DM pull up(0x2)/DP&DM pull down(0x3)
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
    SYSCTRL_MEM_BLOCK_0 = 0x10,     // block 0 is 16KiB starting from 0x20000000
                                    // This block is always ON, and can't be turned off.
    SYSCTRL_MEM_BLOCK_1 = 0x08,     // block 1 is 16KiB following block 0
    SYSCTRL_SHARE_BLOCK_0 = 0x01,   // share memory block 0 is  8KiB starting from 0x40120000
    SYSCTRL_SHARE_BLOCK_1 = 0x02,   // share memory block 1 is 16KiB following block 0 (0x40122000)
    SYSCTRL_SHARE_BLOCK_2 = 0x04,   // share memory block 2 is  8KiB following block 1
} SYSCTRL_MemBlock;

// this blocks (16 + 8) KiB are reversed in _mini_bundles
#define SYSCTRL_RESERVED_MEM_BLOCKS (SYSCTRL_MEM_BLOCK_0 | SYSCTRL_MEM_BLOCK_1 | SYSCTRL_SHARE_BLOCK_0)

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
 *     1. D-Cache: 8KiB starting from 0x2000E000
 *     2. I-Cache: 8KiB starting from 0x20010000
 *
 * CAUTION:
 *     1. When used as system memory, more RAM are available, but performance might be degraded;
 *     2. When used as system memory, contents are *NOT* retained in sleep modes;
 *     3. These settings are not kept in sleep modes and will restore to defaults during waking up,
 *        so this function needs to be called each time the system wakes up.
 *
 * @param[in] i_cache           usage of I-Cache (default: AS_CACHE)
 * @param[in] d_cache           usage of D-Cache (default: AS_CACHE)
 */
void SYSCTRL_CacheControl(SYSCTRL_CacheMemCtrl i_cache, SYSCTRL_CacheMemCtrl d_cache);

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
 * \param item      the component to be reset
 */
void SYSCTRL_ResetBlock(SYSCTRL_ResetItem item);

/**
 * \brief Reset all components
 */
void SYSCTRL_ResetAllBlocks(void);

/**
 * \brief Release a component from reset
 * \param item      the component to be released
 */
void SYSCTRL_ReleaseBlock(SYSCTRL_ResetItem item);

/**
 * @brief Set LDO Core output level
 *
 * @param[in] level         output level (available values see `SYSCTRL_LDO_OUTPUT...`)
 */
void SYSCTRL_SetLDOOutput(SYSCTRL_LDOOutputCore level);

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
 * @param[in] threshold         Threshold (available values see `SYSCTRL_BOR_...`)
 *                              default: 0.95V (ING918)
 *                                       2.30V (ING916)
 * @param[in] enable_active     Enable(1)/Disable(0) for active mode
 *                              default: DISABLED
 * @param[in] enable_sleep      Enable(1)/Disable(0) for sleep mode
 *                              default: DISABLED
 */
void SYSCTRL_ConfigBOR(int threshold, int enable_active, int enable_sleep);

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

/**
 * @brief Get wake up source of last wake up from DEEP/DEEPER sleep
 *
 * @param[out] source           source of the last wake up
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
 * For ING918: This function does nothing.
 *
 * @return                      0 if initialized else failed
 */
int SYSCTRL_Init(void);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

