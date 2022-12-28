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

enum
{
    SYSCTRL_LDO_OUPUT_1V60 = 0x13,  // Recommended for Vbat = 1.8V
    SYSCTRL_LDO_OUPUT_1V80 = 0x16,  // Recommended for Vbat = 2.5V
    SYSCTRL_LDO_OUPUT_2V50 = 0x1f,  // Recommended for Vbat = 3.3V
};

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

enum
{
    SYSCTRL_LDO_OUPUT_CORE_1V000 = 0x0,     // LDO Core Output: 1.000V
                                            // Range: [1.000, 1.300]V
                                            // Step: 20mV
                                            // 1.300V = SYSCTRL_LDO_OUPUT_1V000
                                            //          + 15 * SYSCTRL_LDO_OUPUT_CORE_STEP_0V020
    SYSCTRL_LDO_OUPUT_FLASH_1V600  = 0x0,   // LDO Flash Output: 1.600V
                                            // Range: [1.600, 3.100]V
                                            // Step: 100mV
                                            // 3.100V = SYSCTRL_LDO_OUPUT_FLASH_1V600
                                            //          + 15 * SYSCTRL_LDO_OUPUT_FLASH_STEP_0V100
};

#define SYSCTRL_LDO_OUPUT_CORE_STEP_0V020           1   // step: 20mV
#define SYSCTRL_LDO_OUPUT_FLASH_STEP_0V100          1   // step: 100mV
enum
{
    SYSCTRL_BOR_1V5 = 0x0,          // BOR Threshold on VBAT: 1.5V
                                    // Range: [1.5, 3]V
                                    // 3V = SYSCTRL_BOR_1V5 + 15 * SYSCTRL_BOR_STEP_0V1
};

#define SYSCTRL_BOR_STEP_0V1                1   // step: 0.1V

typedef enum
{
    SYSCTRL_CLK_SLOW,                           // use slow clock
    SYSCTRL_CLK_32k = SYSCTRL_CLK_SLOW,         // use 32kHz clock
    SYSCTRL_CLK_HCLK,                           // use HCLK (same as MCU)
    SYSCTRL_CLK_ADC_DIV = SYSCTRL_CLK_HCLK,     // use clock from ADC divider

    SYSCTRL_CLK_PLL_DIV_1 = SYSCTRL_CLK_HCLK,   // use (PLL clock div 1)
                                                // SYSCTRL_TMR_CLK_PLL_DIV_1 + 1: use (PLL clock div 2)
                                                // ..
                                                // SYSCTRL_TMR_CLK_PLL_DIV_1 + 14: use (PLL clock div 15)

    SYSCTRL_CLK_SLOW_DIV_1 = SYSCTRL_CLK_HCLK,  // use RF OSC clock div 1
                                                // SYSCTRL_CLK_SLOW_DIV_1 + 1: use (RF OSC clock div 2)
                                                // ..
} SYSCTRL_ClkMode;

/**
 * \brief Select clock mode of TIMER
 *
 * All timers share the same clock divider, which means that if timer K is
 * set to use (SYSCTRL_CLK_SLOW_DIV_1 + X), all previously configures timers that
 * uses (SYSCTRL_CLK_SLOW_DIV_1 + ...) are overwritten by (SYSCTRL_CLK_SLOW_DIV_1 + X).
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_1` + N, where N = 0..14;
 *
 * \param port          the timer
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectTimerClk(timer_port_t port, SYSCTRL_ClkMode mode);

/**
 * \brief Select clock mode of PWM
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_1` + N, where N = 0..14;
 *
 * \param port          the timer
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectPWMClk(SYSCTRL_ClkMode mode);

/**
 * \brief Select clock mode of KeyScan
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_1` + N, where N = 0..14;
 *
 * \param port          the timer
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectKeyScanClk(SYSCTRL_ClkMode mode);

/**
 * \brief Select clock mode of PDM
 *
 * `mode` should be `SYSCTRL_CLK_SLOW_DIV_1` + N, where N = 0..62;
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
 * Note: For SPI0: mode should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_PLL_DIV_1` + N, where N = 0..14;
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
 * Note: mode should be SYSCTRL_CLK_SLOW, or SYSCTRL_CLK_PLL_DIV_1 + N, where N = 0..14.
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
 * Note: mode should be SYSCTRL_CLK_SLOW, or SYSCTRL_CLK_PLL_DIV_1 + N,
 *       where N = 0..14.
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
 * \param item          item of type A (IR/ADC/EFUSE)
 * \param mode          clock mode ({SYSCTRL_CLK_SLOW, SYSCTRL_CLK_ADC_DIV})
 */
void SYSCTRL_SelectTypeAClk(SYSCTRL_Item item, SYSCTRL_ClkMode mode);

/**
 * \brief Select clock mode of USB
 *
 * `mode` should be `SYSCTRL_CLK_PLL_DIV_1` + N, where N = 0..14;
 *
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectUSBClk(SYSCTRL_ClkMode mode);

/**
 * \brief Select clock mode of Flash
 *
 * `mode` should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_PLL_DIV_1` + N, where N = 0..14;
 *
 * Default mode: `SYSCTRL_CLK_PLL_DIV_1` + 1.
 *
 * \param mode          clock mode
 *
 */
void SYSCTRL_SelectFlashClk(SYSCTRL_ClkMode mode);

/**
 * \brief Select clock of QDEC
 *
 * The actual clock is clock (selected by `mode`) divided by `div`.
 *
 * `mode` should be `SYSCTRL_CLK_SLOW`, or `SYSCTRL_CLK_HCLK`.
 *
 * \param mode          clock mode
 * \param div           clock divider (10 bits)
 *
 */
void SYSCTRL_SelectQDECClk(SYSCTRL_ClkMode mode, uint16_t div);

/**
 * \brief Select clock of 32k which can be used by IR/WDT/GPIO/KeyScan, or MCU
 *
 * `mode` should be `SYSCTRL_CLK_32k`, or `SYSCTRL_CLK_SLOW_DIV_1` + N,
 *  where N is in [0..0xfff], `SYSCTRL_CLK_32k` is referring to the internal 32k
 *  clock source (32k OSC or 32k RC).
 *
 * Note: The default mode is (`SYSCTRL_CLK_SLOW_DIV_1` + 999), i.e. (SLOW_CLK / 1000).
 *
 * \param mode                  clock mode
 */
void SYSCTRL_SelectCLK32k(SYSCTRL_ClkMode mode);

/**
 * \brief Get the frequency of 32k which can be used by IR/WDT/GPIO/KeyScan, or MCU
 *
 * \return                      frequency of the 32k
 */
int SYSCTRL_GetCLK32k(void);

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
    SYSCTRL_SLOW_RC_64M = 0x1f,
} SYSCTRL_SlowRCClkMode;

/**
 * \brief Enable/Disable RC clock for slow clock
 *
 * \param enable        Enable(1)/Disable(0)
 * \param mode          Clock frequency mode
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
 * @param[in] level         output level (available values see `SYSCTRL_LDO_OUTPUT_FLASH...`)
 */
void SYSCTRL_SetLDOOutputFlash(int level);

/**
 * @brief Config USB PHY functionality
 *
 * @param[in] enable            Enable(1)/Disable(0) usb phy module
 * @param[in] pull_sel          DP pull up(0x1)/DM pull up(0x2)/DP&DM pull down(0x3)
 */
void SYSCTRL_USBPhyConfig(uint8_t enable, uint8_t pull_sel);

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
 * @brief Set LDO Core output level
 *
 * @param[in] level         output level (available values see `SYSCTRL_LDO_OUTPUT...`)
 */
void SYSCTRL_SetLDOOutput(int level);

/**
 * @brief Config BOR (Brownout Reset) functionality
 *
 * Note:
 *
 * ING918: Power consumption is larger when enabled, so it's not recommended to
 *         enable for sleep mode.
 *
 * ING916: `enable_active` and `enable_sleep` should be the same.
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

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

