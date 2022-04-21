#ifndef __PERIPHERAL_PINCTRL_H__
#define __PERIPHERAL_PINCTRL_H__

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

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
} pinctrl_drive_strenght_t;

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

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define IO_PIN_NUMBER                  36

// placeholder value when don't want to select a pin for some IO source
#define IO_NOT_A_PIN                    0x3f

typedef enum
{
    IO_SOURCE_GPIO          = 0x00,      // GPIO or PWM selected by io_sel_pwm
    IO_SOURCE_DEBUG_BUS     = 0x01,
    IO_SOURCE_SW_DATA_OUT   = 0x02,

    IO_SOURCE_SPI0_CLK_OUT  = 0x03,      // SPI0 == QSPI
    IO_SOURCE_SPI0_CSN_OUT  = 0x04,
    IO_SOURCE_SPI0_HOLD_OUT = 0x05,
    IO_SOURCE_SPI0_WP_OUT   = 0x06,
    IO_SOURCE_SPI0_MISO_OUT = 0x07,
    IO_SOURCE_SPI0_MOSI_OUT = 0x08,
    IO_SOURCE_SPI1_CLK_OUT  = 0x09,      // APB SPI
    IO_SOURCE_SPI1_CSN_OUT  = 0x0a,
    IO_SOURCE_SPI1_MISO_OUT = 0x0b,
    IO_SOURCE_SPI1_MOSI_OUT = 0x0c,
    IO_SOURCE_SPI1_HOLD_OUT = 0x0d,
    IO_SOURCE_SPI1_WP_OUT   = 0x0e,

    IO_SOURCE_IR_WAKEUP     = 0x0f,
    IO_SOURCE_IR_DATA_OUT   = 0x10,

    IO_SOURCE_I2S_BCLK_OUT  = 0x11,
    IO_SOURCE_I2S_LRCLK_OUT = 0x12,
    IO_SOURCE_I2S_DATA_OUT  = 0x13,

    IO_SOURCE_UART0_TXD     = 0x14,
    IO_SOURCE_UART0_RTS     = 0x15,
    IO_SOURCE_UART1_TXD     = 0x16,
    IO_SOURCE_UART1_RTS     = 0x17,

    IO_SOURCE_I2C0_SCL_OUT  = 0x18,
    IO_SOURCE_I2C0_SDA_OUT  = 0x19,
    IO_SOURCE_I2C1_SCL_OUT  = 0x1a,
    IO_SOURCE_I2C1_SDA_OUT  = 0x1b,

    IO_SOURCE_PWM0_A        = 0x1c, // PWM0~6 are generated by TMR
    IO_SOURCE_PWM0_B        = 0x1d,
    IO_SOURCE_PWM1_A        = 0x1e,
    IO_SOURCE_PWM1_B        = 0x1f,
    IO_SOURCE_PWM2_A        = 0x20,
    IO_SOURCE_PWM2_B        = 0x21,
    IO_SOURCE_PWM3_A        = 0x22,
    IO_SOURCE_PWM3_B        = 0x23,
    IO_SOURCE_PWM4_A        = 0x24,
    IO_SOURCE_PWM4_B        = 0x25,
    IO_SOURCE_PWM5_A        = 0x26,
    IO_SOURCE_PWM5_B        = 0x27,
    IO_SOURCE_PWM6_A        = 0x28, // generated by PWM
    IO_SOURCE_PWM6_B        = 0x29,

    IO_SOURCE_ANT_SW0       = 0x2a,
    IO_SOURCE_PDM_DMIC_MCLK = 0x32,
    IO_SOURCE_KEYSCN_ROW_0  = 0x33,
    IO_SOURCE_SW_TMS        = 0x48,
    IO_SOURCE_SW_TCK        = 0x49,

    IO_SOURCE_SPI0_CLK_IN   = 0x4a,
    IO_SOURCE_SPI0_CSN_IN   = 0x4b,
    //IO_SOURCE_SPI0_HOLD_IN  = 0x4c,
    //IO_SOURCE_SPI0_WP_IN    = 0x4d,
    //IO_SOURCE_SPI0_MISO_IN  = 0x4e,
    //IO_SOURCE_SPI0_MOSI_IN  = 0x4f,
    IO_SOURCE_SPI1_CLK_IN   = 0x50,
    IO_SOURCE_SPI1_CSN_IN   = 0x51,
    //IO_SOURCE_SPI1_MISO_IN  = 0x52,
    //IO_SOURCE_SPI1_MOSI_IN  = 0x53,
    //IO_SOURCE_SPI1_HOLD_IN  = 0x54,
    //IO_SOURCE_SPI1_WP_IN    = 0x55,

    IO_SOURCE_IR_DATA_IN    = 0x56,
    IO_SOURCE_I2S_BCLK_IN   = 0x57,
    IO_SOURCE_I2S_LRCLK_IN  = 0x58,
    IO_SOURCE_I2S_DATA_IN   = 0x59,

    IO_SOURCE_UART0_RXD     = 0x5a,
    IO_SOURCE_UART0_CTS     = 0x5b,
    IO_SOURCE_UART1_RXD     = 0x5c,
    IO_SOURCE_UART1_CTS     = 0x5d,

    //IO_SOURCE_I2C0_SCL_IN   = 0x5e,
    //IO_SOURCE_I2C0_SDA_IN   = 0x5f,
    //IO_SOURCE_I2C1_SCL_IN   = 0x60,
    //IO_SOURCE_I2C1_SDA_IN   = 0x61,

    IO_SOURCE_PDM_DMIC_IN   = 0x62,

    IO_SOURCE_KEYSCN_COL_0  = 0x63,
} io_source_t;

// compatible definitions with ING918xx
#define IO_SOURCE_GENERAL           IO_SOURCE_GPIO
#define IO_SOURCE_SPI0_CLK	        IO_SOURCE_SPI0_CLK_OUT
#define IO_SOURCE_SPI0_DO	        IO_SOURCE_SPI0_MOSI_OUT
#define IO_SOURCE_SPI0_SSN	        IO_SOURCE_SPI0_CSN_OUT
#define IO_SOURCE_I2C0_SCL_O	    IO_SOURCE_I2C0_SCL_OUT
#define IO_SOURCE_I2C0_SDO	        IO_SOURCE_I2C0_SDA_OUT
#define IO_SOURCE_SPI1_CLK	        IO_SOURCE_SPI1_CLK_OUT
#define IO_SOURCE_SPI1_DO	        IO_SOURCE_SPI1_MOSI_OUT
#define IO_SOURCE_SPI1_SSN	        IO_SOURCE_SPI1_CSN_OUT
#define IO_SOURCE_I2C1_SCL_O	    IO_SOURCE_I2C1_SCL_OUT
#define IO_SOURCE_I2C1_SDO	        IO_SOURCE_I2C1_SDA_OUT

#define IO_SOURCE_MASK      0x3F

typedef enum
{
    PINCTRL_SLEW_RATE_DUMMY,
} pinctrl_slew_rate_t;

typedef enum
{
    PINCTRL_DRIVE_DUMMY,
DUMMY} pinctrl_drive_strenght_t;

/**
 * @brief Select SWD input IOs
 *
 * @param[in] io_pin_tms        TMS
 * @param[in] io_pin_tck        TCK
 */
void PINCTRL_SelSwIn(uint8_t io_pin_tms, uint8_t io_pin_tck);

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
 */
void PINCTRL_SelSpiIn(spi_port_t port,
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
 */
void PINCTRL_SelIrIn(uint8_t io_pin_data);

/**
 * @brief Select I2S input IOs
 *
 * @param[in] io_pin_bclk       BCLK
 * @param[in] io_pin_lrclk      LRCLK
 * @param[in] io_pin_data       DATA
 */
void PINCTRL_SelI2sIn(uint8_t io_pin_bclk,
                      uint8_t io_pin_lrclk,
                      uint8_t io_pin_data);

/**
 * @brief Select UART input IOs
 *
 * Note: If an input is not used or invalid, set it to `IO_NOT_A_PIN`.
 *
 * @param[in] io_pin_rxd        RXD (Rx Data)
 * @param[in] io_pin_cts        CTS (Clear to Send)
 */
void PINCTRL_SelUartIn(uart_port_t port,
                      uint8_t io_pin_rxd,
                      uint8_t io_pin_cts);

/**
 * @brief Select I2C input IOs
 *
 * Note: If an input is not used or invalid, set it to `IO_NOT_A_PIN`.
 *
 * @param[in] io_pin_scl        SCL
 * @param[in] io_pin_sda        SDA
 */
void PINCTRL_SelI2cIn(i2c_port_t port,
                      uint8_t io_pin_scl,
                      uint8_t io_pin_sda);

/**
 * @brief Select PDM input IOs
 *
 * @param[in] io_pin_dmic       DMIC
 */
void PINCTRL_SelPdmIn(uint8_t io_pin_dmic);

/**
 * @brief Select KeyScan column input IOs
 *
 * @param[in] index             column index (0..19)
 * @param[in] io_pin            IO pin index
 */
void PINCTRL_SelKeyScanColIn(int index, uint8_t io_pin);

/**
 * @brief Set pull mode of a IO source
 *
 * Note:
 *      1. Not all `io_source_t` are supported;
 *      1. pull mode of GPIO is controlled in GPIO module.
 *
 * @param io_source         IO source
 * @param mode              The mode to be configured
 */
void PINCTRL_Pull(const io_source_t io_source, const pinctrl_pull_mode_t mode);

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
 * @brief Select antenna control PINs as ING918xx
 *
 */
void PINCTRL_EnableAllAntSelPins(void);

#endif

void PINCTRL_SetPadMux(const uint8_t io_pin_index, const io_source_t source);

// Disable all input io_pins for UART/SPI/I2C, etc.
void PINCTRL_DisableAllInputs(void);

/**
 * @brief Set slew rate of a GPIO
 *
 * @param io_pin_index      The io pad to be configured.
 * @param rate              The rate to be configured (default: SLOW)
 */
void PINCTRL_SetSlewRate(const uint8_t io_pin_index, const pinctrl_slew_rate_t rate);

/**
 * @brief Set slew rate of a GPIO
 *
 * @param io_pin_index      The io pad to be configured.
 * @param strenght          The strenght to be configured (default: 8mA)
 */
void PINCTRL_SetDriveStrength(const uint8_t io_pin_index, const pinctrl_drive_strenght_t strenght);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

