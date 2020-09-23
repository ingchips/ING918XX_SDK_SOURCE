#ifndef __PERIPHERAL_PINCTRL_H__
#define __PERIPHERAL_PINCTRL_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"
#include "peripheral_i2c.h"

#define IO_PIN_NUMBER                  32

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

#define IO_SOURCE_MASK      0xF

void PINCTRL_SetPadMux(const uint8_t io_pin_index, const io_source_t source);

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

// Disable all input io_pins for UART/SPI/I2C
void PINCTRL_DisableAllInputs(void);

typedef enum
{
    PINCTRL_PULL_DISABLE,
    PINCTRL_PULL_UP,
    PINCTRL_PULL_DOWN
} pinctrl_pull_mode_t;

/**
 * @brief Set pull mode of a GPIO
 *
 * @param io_pin_index      The io pad to be configured.
 * @param mode              The mode to be configured
 */
void PINCTRL_Pull(const uint8_t io_pin_index, const pinctrl_pull_mode_t mode);

typedef enum
{
    PINCTRL_SLEW_RATE_FAST,
    PINCTRL_SLEW_RATE_SLOW
} pinctrl_slew_rate_t;

/**
 * @brief Set slew rate of a GPIO
 *
 * @param io_pin_index      The io pad to be configured.
 * @param rate              The rate to be configured (default: SLOW)
 */
void PINCTRL_SetSlewRate(const uint8_t io_pin_index, const pinctrl_slew_rate_t rate);

typedef enum
{
    PINCTRL_DRIVE_2mA,
    PINCTRL_DRIVE_4mA,
    PINCTRL_DRIVE_8mA,
    PINCTRL_DRIVE_12mA,
} pinctrl_drive_strenght_t;

/**
 * @brief Set slew rate of a GPIO
 *
 * @param io_pin_index      The io pad to be configured.
 * @param strenght          The strenght to be configured (default: 8mA)
 */
void PINCTRL_SetDriveStrength(const uint8_t io_pin_index, const pinctrl_drive_strenght_t strenght);

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

// io_pin_index: 0~11 (obsoleted, use PINCTRL_SetGeneralPadMode instead)
void PINCTRL_SetPadPwmSel(const uint8_t io_pin_index, const uint8_t pwm1_gpio0);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

