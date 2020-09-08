#ifndef __PERIPHERAL_PINCTRL_H__
#define __PERIPHERAL_PINCTRL_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"
#include "peripheral_i2c.h"

#define bsPINCTRL_PULL_UP               0
#define bwPINCTRL_PULL_UP                     1
#define bsPINCTRL_PULL_DOWN             1
#define bwPINCTRL_PULL_DOWN                   1
#define bsPINCTRL_SCHMITT_TRIGGER       2
#define bwPINCTRL_SCHMITT_TRIGGER             1
#define bsPINCTRL_SLEW_RATE             3
#define bwPINCTRL_SLEW_RATE                   1
#define bsPINCTRL_DRIVER_STRENGTH       4
#define bwPINCTRL_DRIVER_STRENGTH             3

#define bsPINCTRL_FUNC_MUX              8
#define bwPINCTRL_FUNC_MUX                    2

#define PINCTRL_PULL_UP                 (1<<bsPINCTRL_PULL_UP)
#define PINCTRL_PULL_DOWN               (1<<bsPINCTRL_PULL_DOWN)
#define PINCTRL_SCHMITT_TRIGGER         (1<<bsPINCTRL_SCHMITT_TRIGGER)
#define PINCTRL_SLEW_RATE               (1<<bsPINCTRL_SLEW_RATE)

#define PINCTRL_DS_0                   0
#define PINCTRL_DS_1                   1
#define PINCTRL_DS_2                   2
#define PINCTRL_DS_4                   4

#define PINCTRL_FUNC_0                 0
#define PINCTRL_FUNC_1                 1
#define PINCTRL_FUNC_2                 2
#define PINCTRL_FUNC_3                 3

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

