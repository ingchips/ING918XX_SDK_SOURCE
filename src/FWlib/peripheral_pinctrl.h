#ifndef __PERIPHERAL_PINCTRL_H__
#define __PERIPHERAL_PINCTRL_H__

#include <stdint.h>
#include "ingsoc.h"

#ifdef    __cplusplus
extern "C" {    /* allow C++ to use these headers */
#endif    /* __cplusplus */

typedef enum
{
    UART_PORT_0,
    UART_PORT_1
} uart_port_t;

typedef enum
{
    I2C_PORT_0,
    I2C_PORT_1
} i2c_port_t;

typedef enum
{
    SPI_PORT_0,
    SPI_PORT_1
} spi_port_t;

typedef enum
{
    TMR_PORT_0,
    TMR_PORT_1,
    TMR_PORT_2,
} timer_port_t;

typedef enum
{
    PINCTRL_PULL_DISABLE,
    PINCTRL_PULL_UP,
    PINCTRL_PULL_DOWN
} pinctrl_pull_mode_t;

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define IO_PIN_NUMBER                  32

// placeholder value when don't want to select a pin for some IO source
#define IO_NOT_A_PIN                    0xf

typedef enum
{
    IO_SOURCE_GENERAL,      // GPIO or PWM selected by io_sel_pwm
    IO_SOURCE_SPI0_CLK,
    IO_SOURCE_SPI0_DO,
    IO_SOURCE_SPI0_SSN,
    IO_SOURCE_UART0_TXD,
    IO_SOURCE_I2C0_SCL_O,
    IO_SOURCE_I2C0_SDO,
    IO_SOURCE_SPI1_CLK,
    IO_SOURCE_SPI1_DO,
    IO_SOURCE_SPI1_SSN,
    IO_SOURCE_UART1_TXD,
    IO_SOURCE_I2C1_SCL_O,
    IO_SOURCE_I2C1_SDO,
    IO_SOURCE_UART0_RTS,
    IO_SOURCE_UART1_RTS,
    IO_SOURCE_DEBUG_BUS
} io_source_t;

// compatible definitions with ING916xx
#define IO_SOURCE_GPIO           IO_SOURCE_GENERAL
#define IO_SOURCE_SPI0_CLK_OUT   IO_SOURCE_SPI0_CLK
#define IO_SOURCE_SPI0_MOSI_OUT  IO_SOURCE_SPI0_DO
#define IO_SOURCE_SPI0_CSN_OUT   IO_SOURCE_SPI0_SSN
#define IO_SOURCE_I2C0_SCL_OUT   IO_SOURCE_I2C0_SCL_O
#define IO_SOURCE_I2C0_SDA_OUT   IO_SOURCE_I2C0_SDO
#define IO_SOURCE_SPI1_CLK_OUT   IO_SOURCE_SPI1_CLK
#define IO_SOURCE_SPI1_MOSI_OUT  IO_SOURCE_SPI1_DO
#define IO_SOURCE_SPI1_CSN_OUT   IO_SOURCE_SPI1_SSN
#define IO_SOURCE_I2C1_SCL_OUT   IO_SOURCE_I2C1_SCL_O
#define IO_SOURCE_I2C1_SDA_OUT   IO_SOURCE_I2C1_SDO

#define IO_SOURCE_MASK      0xF

typedef enum
{
    PINCTRL_SLEW_RATE_FAST,
    PINCTRL_SLEW_RATE_SLOW
} pinctrl_slew_rate_t;

typedef enum
{
    PINCTRL_DRIVE_2mA,
    PINCTRL_DRIVE_4mA,
    PINCTRL_DRIVE_8mA,
    PINCTRL_DRIVE_12mA,
} pinctrl_drive_strength_t;

// Select input io_pin for UART RXD
void PINCTRL_SelUartRxdIn(const uart_port_t port, const uint8_t io_pin_index);
// Select input io_pin for UART CTS
void PINCTRL_SelUartCtsIn(const uart_port_t port, const uint8_t io_pin_index);

// Select input io_pin for SPI DI
void PINCTRL_SelSpiDiIn(const spi_port_t port, const uint8_t io_pin_index);
// Select input io_pin for SPI CLK
void PINCTRL_SelSpiClkIn(const spi_port_t port, const uint8_t io_pin_index);

// Select input io_pin for I2C SCL
void PINCTRL_SelI2cSclIn(const i2c_port_t port, const uint8_t io_pin_index);

typedef enum
{
    IO_MODE_GPIO,
    IO_MODE_PWM,
    IO_MODE_ANT_SEL
} gio_mode_t;

/**
 * @brief Set working mode of a pad which has been mux-ed as IO_SOURCE_GENERAL
 *
 * @param io_pin_index      The io pad to be configured. Valid range:
 *                          For IO_MODE_GPIO    : [0..32]
 *                          For IO_MODE_PWM     : [0..11]
 *                          For IO_MODE_ANT_SEL : {7, 8, 10, 11, 16, 17, 18, 19}
 * @param mode              The mode to be configured
 * @param pwm_channel       0..6. (Ignored when mode != IO_MODE_PWM)
 * @param pwm_neg           If use the negated PWM signal. (Ignored when mode != IO_MODE_PWM)
 */
void PINCTRL_SetGeneralPadMode(const uint8_t io_pin_index, const gio_mode_t mode, const uint8_t pwm_channel, const uint8_t pwm_neg);

/**
 * @brief Set pull mode of a IO pad
 *
 * @param io_pin_index      The io pad to be configured.
 * @param mode              The mode to be configured
 */
void PINCTRL_Pull(const uint8_t io_pin_index, const pinctrl_pull_mode_t mode);

// io_pin_index: 0~11 (obsoleted, use PINCTRL_SetGeneralPadMode instead)
void PINCTRL_SetPadPwmSel(const uint8_t io_pin_index, const uint8_t pwm1_gpio0);

/**
 * @brief Select antenna control PINs
 *
 * Note: ANT_SEL[0..count - 1] is setup according to `io_pins`, i.e.
 *
 * ANT_SEL[0] -> io_pins[0]
 * ...
 * ANT_SEL[count - 1] -> io_pins[count - 1]
 *
 * @param count             PIN count in io_pins
 * @param io_pins           PIN array
 */
void PINCTRL_EnableAntSelPins(int count, const uint8_t *io_pins);

/**
 * @brief Enable all antenna control PINs
 *
 */
void PINCTRL_EnableAllAntSelPins(void);

/**
 * @brief Set slew rate of a GPIO
 *
 * @param io_pin_index      The io pad to be configured.
 * @param rate              The rate to be configured (default: SLOW)
 */
void PINCTRL_SetSlewRate(uint8_t io_pin_index, const pinctrl_slew_rate_t rate);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define IO_PIN_NUMBER                   42

// placeholder value when don't want to select a pin for some IO source
#define IO_NOT_A_PIN                    0x7f

typedef enum
{
    // below sources are for output
    IO_SOURCE_GPIO                      = 0,
    IO_SOURCE_DEBUG_BUS                 = 1,
    IO_SOURCE_SW_DATA_OUT               = 2,
    IO_SOURCE_SPI0_CLK_OUT              = 3,
    IO_SOURCE_SPI0_CSN_OUT              = 4,
    IO_SOURCE_SPI0_HOLD_OUT             = 5,
    IO_SOURCE_SPI0_WP_OUT               = 6,
    IO_SOURCE_SPI0_MISO_OUT             = 7,
    IO_SOURCE_SPI0_MOSI_OUT             = 8,
    IO_SOURCE_SPI1_CLK_OUT              = 9,
    IO_SOURCE_SPI1_CSN_OUT              = 10,
    IO_SOURCE_SPI1_MISO_OUT             = 11,
    IO_SOURCE_SPI1_MOSI_OUT             = 12,
    IO_SOURCE_SPI1_HOLD_OUT             = 13,
    IO_SOURCE_SPI1_WP_OUT               = 14,
    IO_SOURCE_IR_WAKEUP                 = 15,
    IO_SOURCE_IR_DATA_OUT               = 16,
    IO_SOURCE_I2S_BCLK_OUT              = 17,
    IO_SOURCE_I2S_LRCLK_OUT             = 18,
    IO_SOURCE_I2S_DATA_OUT              = 19,
    IO_SOURCE_UART0_TXD                 = 20,
    IO_SOURCE_UART0_RTS                 = 21,
    IO_SOURCE_UART1_TXD                 = 22,
    IO_SOURCE_UART1_RTS                 = 23,
    IO_SOURCE_I2C0_SCL_OUT              = 24,
    IO_SOURCE_I2C0_SDA_OUT              = 25,
    IO_SOURCE_I2C1_SCL_OUT              = 26,
    IO_SOURCE_I2C1_SDA_OUT              = 27,
    // below PWM outputs are from TIMERs
    IO_SOURCE_TIMER0_PWM0_A                    = 28,
    IO_SOURCE_TIMER0_PWM0_B                    = 29,
    IO_SOURCE_TIMER0_PWM1_A                    = 30,
    IO_SOURCE_TIMER0_PWM1_B                    = 31,
    IO_SOURCE_TIMER1_PWM0_A                    = 32,
    IO_SOURCE_TIMER1_PWM0_B                    = 33,
    IO_SOURCE_TIMER1_PWM1_A                    = 34,
    IO_SOURCE_TIMER1_PWM1_B                    = 35,
    IO_SOURCE_TIMER2_PWM0_A                    = 36,
    IO_SOURCE_TIMER2_PWM0_B                    = 37,
    IO_SOURCE_TIMER2_PWM1_A                    = 38,
    IO_SOURCE_TIMER2_PWM1_B                    = 39,
    // below PWM outputs are from PWM
    IO_SOURCE_PWM0_A                    = 40,
    IO_SOURCE_PWM0_B                    = 41,
    IO_SOURCE_PWM1_A                    = 42,
    IO_SOURCE_PWM1_B                    = 43,
    IO_SOURCE_PWM2_A                    = 44,
    IO_SOURCE_PWM2_B                    = 45,
    IO_SOURCE_ANT_SW0                   = 46,
    IO_SOURCE_ANT_SW1                   = 47,
    IO_SOURCE_ANT_SW2                   = 48,
    IO_SOURCE_ANT_SW3                   = 49,
    IO_SOURCE_ANT_SW4                   = 50,
    IO_SOURCE_ANT_SW5                   = 51,
    IO_SOURCE_ANT_SW6                   = 52,
    IO_SOURCE_ANT_SW7                   = 53,
    IO_SOURCE_PA_TXEN                   = 54,
    IO_SOURCE_PA_RXEN                   = 55,
    IO_SOURCE_PDM_DMIC_MCLK             = 56,
    IO_SOURCE_KEYSCN_ROW_0              = 57,
    IO_SOURCE_KEYSCN_ROW_1              = 58,
    IO_SOURCE_KEYSCN_ROW_2              = 59,
    IO_SOURCE_KEYSCN_ROW_3              = 60,
    IO_SOURCE_KEYSCN_ROW_4              = 61,
    IO_SOURCE_KEYSCN_ROW_5              = 62,
    IO_SOURCE_KEYSCN_ROW_6              = 63,
    IO_SOURCE_KEYSCN_ROW_7              = 64,
    IO_SOURCE_KEYSCN_ROW_8              = 65,
    IO_SOURCE_KEYSCN_ROW_9              = 66,
    IO_SOURCE_KEYSCN_ROW_10             = 67,
    IO_SOURCE_KEYSCN_ROW_11             = 68,
    IO_SOURCE_KEYSCN_ROW_12             = 69,
    IO_SOURCE_KEYSCN_ROW_13             = 70,
    IO_SOURCE_KEYSCN_ROW_14             = 71,
    IO_SOURCE_KEYSCN_ROW_15             = 72,
    IO_SOURCE_KEYSCN_ROW_16             = 73,
    IO_SOURCE_KEYSCN_ROW_17             = 74,
    IO_SOURCE_KEYSCN_ROW_18             = 75,
    IO_SOURCE_KEYSCN_ROW_19             = 76,
    IO_SOURCE_SPI2AHB_DATA_OUT          = 77,
    IO_SOURCE_QDEC_TIMER_EXT_OUT0_A     = 78,
    IO_SOURCE_QDEC_TIMER_EXT_OUT1_A     = 79,
    IO_SOURCE_QDEC_TIMER_EXT_OUT2_A     = 80,
    IO_SOURCE_QDEC_TIMER_EXT_OUT0_B     = 81,
    IO_SOURCE_QDEC_TIMER_EXT_OUT1_B     = 82,
    IO_SOURCE_QDEC_TIMER_EXT_OUT2_B     = 83,
    // below sources are for input
    IO_SOURCE_SW_DIO                      = 84,
    IO_SOURCE_SW_CLK                      = 85,
    IO_SOURCE_SPI0_CLK_IN                 = 86,
    IO_SOURCE_SPI0_CSN_IN                 = 87,
    IO_SOURCE_SPI0_HOLD_IN                = 88,
    IO_SOURCE_SPI0_WP_IN                  = 89,
    IO_SOURCE_SPI0_MISO_IN                = 90,
    IO_SOURCE_SPI0_MOSI_IN                = 91,
    IO_SOURCE_SPI1_CLK_IN                 = 92,
    IO_SOURCE_SPI1_CSN_IN                 = 93,
    IO_SOURCE_SPI1_MISO_IN                = 94,
    IO_SOURCE_SPI1_MOSI_IN                = 95,
    IO_SOURCE_SPI1_HOLD_IN                = 96,
    IO_SOURCE_SPI1_WP_IN                  = 97,
    IO_SOURCE_IR_DATA_IN                  = 98,
    IO_SOURCE_I2S_BCLK_IN                 = 99,
    IO_SOURCE_I2S_LRCLK_IN                = 100,
    IO_SOURCE_I2S_DATA_IN                 = 101,
    IO_SOURCE_UART0_RXD                   = 102,
    IO_SOURCE_UART0_CTS                   = 103,
    IO_SOURCE_UART1_RXD                   = 104,
    IO_SOURCE_UART1_CTS                   = 105,
    IO_SOURCE_I2C0_SCL_IN                 = 106,
    IO_SOURCE_I2C0_SDA_IN                 = 107,
    IO_SOURCE_I2C1_SCL_IN                 = 108,
    IO_SOURCE_I2C1_SDA_IN                 = 109,
    IO_SOURCE_PDM_DMIC_IN                 = 110,
    IO_SOURCE_KEYSCN_IN_COL_0             = 111,
    IO_SOURCE_KEYSCN_IN_COL_1             = 112,
    IO_SOURCE_KEYSCN_IN_COL_2             = 113,
    IO_SOURCE_KEYSCN_IN_COL_3             = 114,
    IO_SOURCE_KEYSCN_IN_COL_4             = 115,
    IO_SOURCE_KEYSCN_IN_COL_5             = 116,
    IO_SOURCE_KEYSCN_IN_COL_6             = 117,
    IO_SOURCE_KEYSCN_IN_COL_7             = 118,
    IO_SOURCE_KEYSCN_IN_COL_8             = 119,
    IO_SOURCE_KEYSCN_IN_COL_9             = 120,
    IO_SOURCE_KEYSCN_IN_COL_10            = 121,
    IO_SOURCE_KEYSCN_IN_COL_11            = 122,
    IO_SOURCE_KEYSCN_IN_COL_12            = 123,
    IO_SOURCE_KEYSCN_IN_COL_13            = 124,
    IO_SOURCE_KEYSCN_IN_COL_14            = 125,
    IO_SOURCE_KEYSCN_IN_COL_15            = 126,
    IO_SOURCE_KEYSCN_IN_COL_16            = 127,
    IO_SOURCE_KEYSCN_IN_COL_17            = 128,
    IO_SOURCE_KEYSCN_IN_COL_18            = 129,
    IO_SOURCE_KEYSCN_IN_COL_19            = 130,
    IO_SOURCE_SPI2AHB_SCLK                = 131,
    IO_SOURCE_SPI2AHB_CS                  = 132,
    IO_SOURCE_SPI2AHB_DI                  = 133,
    IO_SOURCE_QDEC_PHASEA                 = 134,
    IO_SOURCE_QDEC_PHASEB                 = 135,
    IO_SOURCE_QDEC_INDEX                  = 136,
    IO_SOURCE_QDEC_EXT_IN_CLK             = 137,
    IO_SOURCE_QDEC_TIMER_EXT_IN1_A        = 138,
    IO_SOURCE_QDEC_TIMER_EXT_IN2_A        = 139,
    IO_SOURCE_QDEC_TIMER_EXT_IN2_B        = 140,
    IO_SOURCE_PCAP0_IN                    = 141,
    IO_SOURCE_PCAP1_IN                    = 142,
    IO_SOURCE_PCAP2_IN                    = 143,
    IO_SOURCE_PCAP3_IN                    = 144,
    IO_SOURCE_PCAP4_IN                    = 145,
    IO_SOURCE_PCAP5_IN                    = 146,

    IO_SOURCE_NUMBER
} io_source_t;

// compatible definitions with ING918xx
#define IO_SOURCE_GENERAL           IO_SOURCE_GPIO
#define IO_SOURCE_SPI0_CLK          IO_SOURCE_SPI0_CLK_OUT
#define IO_SOURCE_SPI0_DO           IO_SOURCE_SPI0_MOSI_OUT
#define IO_SOURCE_SPI0_SSN          IO_SOURCE_SPI0_CSN_OUT
#define IO_SOURCE_I2C0_SCL_O        IO_SOURCE_I2C0_SCL_OUT
#define IO_SOURCE_I2C0_SDO          IO_SOURCE_I2C0_SDA_OUT
#define IO_SOURCE_SPI1_CLK          IO_SOURCE_SPI1_CLK_OUT
#define IO_SOURCE_SPI1_DO           IO_SOURCE_SPI1_MOSI_OUT
#define IO_SOURCE_SPI1_SSN          IO_SOURCE_SPI1_CSN_OUT
#define IO_SOURCE_I2C1_SCL_O        IO_SOURCE_I2C1_SCL_OUT
#define IO_SOURCE_I2C1_SDO          IO_SOURCE_I2C1_SDA_OUT

#define IO_SOURCE_MASK      0x3F

typedef enum
{
    PINCTRL_DRIVE_2mA,
    PINCTRL_DRIVE_4mA,
    PINCTRL_DRIVE_8mA,
    PINCTRL_DRIVE_12mA,
} pinctrl_drive_strength_t;

/**
 * @brief Select SWD input IOs
 *
 * Note: If an input is not used or invalid, set it to `IO_NOT_A_PIN`.
 *
 * This function fails when IO pin do not support such configurations.
 *
 * @param[in] io_pin_dio        DIO
 * @param[in] io_pin_clk        CLK
 * @return                      0 if successful else non-0
 */
int PINCTRL_SelSwIn(uint8_t io_pin_dio, uint8_t io_pin_clk);

/**
 * @brief Select SPI input IOs
 *
 * Note: If an input is not used or invalid, set it to `IO_NOT_A_PIN`.
 *
 * @param[in] io_pin_clk        CLK
 * @param[in] io_pin_csn        CS_n
 * @param[in] io_pin_hold       HOLD
 * @param[in] io_pin_wp         WP
 * @param[in] io_pin_miso       MISO
 * @param[in] io_pin_mosi       MOSI
 * @return                      0 if successful else non-0
 */
int PINCTRL_SelSpiIn(spi_port_t port,
                      uint8_t io_pin_clk,
                      uint8_t io_pin_csn,
                      uint8_t io_pin_hold,
                      uint8_t io_pin_wp,
                      uint8_t io_pin_miso,
                      uint8_t io_pin_mosi);

/**
 * @brief Select IR input IOs
 *
 * @param[in] io_pin_data       Data input
 * @return                      0 if successful else non-0
 */
int PINCTRL_SelIrIn(uint8_t io_pin_data);

/**
 * @brief Select I2S input IOs
 *
 * @param[in] io_pin_bclk       BCLK
 * @param[in] io_pin_lrclk      LRCLK
 * @param[in] io_pin_data       DATA
 * @return                      0 if successful else non-0
 */
int PINCTRL_SelI2sIn(uint8_t io_pin_bclk,
                      uint8_t io_pin_lrclk,
                      uint8_t io_pin_data);

/**
 * @brief Select UART input IOs
 *
 * Note: If an input is not used or invalid, set it to `IO_NOT_A_PIN`.
 *
 * @param[in] io_pin_rxd        RXD (Rx Data)
 * @param[in] io_pin_cts        CTS (Clear to Send)
 * @return                      0 if successful else non-0
 */
int PINCTRL_SelUartIn(uart_port_t port,
                      uint8_t io_pin_rxd,
                      uint8_t io_pin_cts);

/**
 * @brief ING918xx Compatible API: Select input io_pin for UART CTS
 *
 * Warning: this function will fail if `io_pin_index` can't support this IO function.
 */
void PINCTRL_SelUartRxdIn(const uart_port_t port, const uint8_t io_pin_index);

/**
 * @brief ING918xx Compatible API: Select input io_pin for UART CTS
 *
 * Warning: this function will fail if `io_pin_index` can't support this IO function.
 */
void PINCTRL_SelUartCtsIn(const uart_port_t port, const uint8_t io_pin_index);

/**
 * @brief Select I2C input IOs
 *
 * Note: Both SCL and SDA are configured properly.
 * Developer don't need to call `PINCTRL_SetPadMux(io_pin_sda, IO_SOURCE_I2C0_SDA_OUT)`
 * or `PINCTRL_Pull(...)`.
 *
 * @param[in] io_pin_scl        SCL
 * @param[in] io_pin_sda        SDA
 * @return                      0 if successful else non-0
 */
int PINCTRL_SelI2cIn(i2c_port_t port,
                      uint8_t io_pin_scl,
                      uint8_t io_pin_sda);

/**
 * @brief ING918xx Compatible API: Select input io_pin for I2C SCL
 *
 * Warning: this function will fail if `io_pin_index` can't support this IO function.
 */
void PINCTRL_SelI2cSclIn(const i2c_port_t port, const uint8_t io_pin_index);

/**
 * @brief Select PDM input IOs
 *
 * @param[in] io_pin_dmic       DMIC
 * @return                      0 if successful else non-0
 */
int PINCTRL_SelPdmIn(uint8_t io_pin_dmic);

/**
 * @brief Select KeyScan column input IOs
 *
 * @param[in] index             column index (0..19)
 * @param[in] io_pin            IO pin index
 * @return                      0 if successful else non-0
 */
int PINCTRL_SelKeyScanColIn(int index, uint8_t io_pin);

/**
 * @brief Select PCAP input IOs
 *
 * @param[in] index             column index (0..5)
 * @param[in] io_pin            IO pin index
 * @return                      0 if successful else non-0
 */
int PINCTRL_SelPCAPIn(int index, uint8_t io_pin);

/**
 * @brief Select QDEC input IOs
 *
 * @param[in] phase_a           Phase A
 * @param[in] phase_b           Phase B
 * @return                      0 if successful else non-0
 */
int PINCTRL_SelQDECIn(uint8_t phase_a,
                      uint8_t phase_b);

/**
 * @brief Set pull mode of a IO source
 *
 *
 * @param io_pin            IO source
 * @param mode              The mode to be configured
 * @return                  0 if successful else non-0
 */
int PINCTRL_Pull(const uint8_t io_pin, const pinctrl_pull_mode_t mode);

/**
 * @brief Select antenna control PINs
 *
 * Note: ANT_SEL[0..count - 1] is setup according to `io_pins`, i.e.
 *
 * ANT_SEL[0] -> io_pins[0]
 * ...
 * ANT_SEL[count - 1] -> io_pins[count - 1]
 *
 * If ANT_SEL[n] output does not need to be configured, set io_pins[n] to `IO_NOT_A_PIN`.
 *
 * @param count             PIN count in io_pins
 * @param io_pins           PIN array
 * @return                  0 if successful else non-0
 */
int PINCTRL_EnableAntSelPins(int count, const uint8_t *io_pins);

/**
 * @brief Set USB function of dp and dm
 *
 * @param dp_io_pin_index      fixed, should be GPIO_16.
 * @param dm_io_pin_index      fixed, should be GPIO_17.
 * @return                  0 if successful else non-0
 */
int PINCTRL_SelUSB(const uint8_t dp_io_pin_index, const uint8_t dm_io_pin_index);

/**
 * @brief Enable analog function of a certain IO (used for USB/ADC)
 *
 */
void PINCTRL_EnableAnalog(const uint8_t io_index);

#endif

/**
 * @brief Select source of an IO pad
 *
 * When the specified IO pad do not support such source, this function will fail
 * and a non-0 value is returned.
 *
 * Note: To select an input source for an IO pad, DO NOT call this.
 * Use `PINCTRL_Sel...In(...)` instead.
 *
 * @param io_pin_index      The io pad to be configured (0 .. IO_PIN_NUMBER - 1).
 * @param source            IO source (output)
 * @return                  0 if successful else non-0
 */
int PINCTRL_SetPadMux(const uint8_t io_pin_index, const io_source_t source);

// Disable all input io_pins for UART/SPI/I2C, etc.
void PINCTRL_DisableAllInputs(void);

/**
 * @brief Set slew rate of a GPIO
 *
 * @param io_pin_index      The io pad to be configured.
 * @param strength          The strength to be configured (default: 8mA; Exception: IO1 12mA)
 */
void PINCTRL_SetDriveStrength(const uint8_t io_pin_index, const pinctrl_drive_strength_t strength);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif    /* __cplusplus */

#endif

