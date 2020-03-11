#include "peripheral_pinctrl.h"

#define OFFSET_MUX_CTRL0        (0x44)
#define IO_MUX_CTRL5            (0x54)
#define IO_MUX_CTRL6            (0x58)
#define IO_MUX_CTRL7            (0x68)

void PINCTRL_SetPadMux(const uint8_t io_pin_index, const io_source_t source)
{ 
	volatile uint32_t * reg = (volatile uint32_t *)(SYSCTRL_BASE + OFFSET_MUX_CTRL0) + (io_pin_index >> 3);
    uint8_t offset = (io_pin_index & 0x7) << 2;
    *reg = (*reg & ~(IO_SOURCE_MASK << offset)) | (source << offset);
}

void PINCTRL_SetPadPwmSel(const uint8_t io_pin_index, const uint8_t pwm1_gpio0)
{
    volatile uint32_t * reg = (volatile uint32_t *)(SYSCTRL_BASE + IO_MUX_CTRL7);
    if (pwm1_gpio0)
        *reg = *reg | (1 << io_pin_index);
    else
        *reg = *reg & ~(1 << io_pin_index);
}

static void PINCTRL_SelIn(const uint32_t mux_offset, uint8_t offset, const uint8_t io_pin_index)
{
    volatile uint32_t * reg = (volatile uint32_t *)(SYSCTRL_BASE + mux_offset);
    const uint32_t mask = 0x3f << offset;
    *reg = (*reg & ~mask) | (io_pin_index << offset);
}

// Select input io_pin for UART RXD
void PINCTRL_SelUartRxdIn(const uart_port_t port, const uint8_t io_pin_index)
{
    PINCTRL_SelIn(IO_MUX_CTRL5, port == UART_PORT_0 ? 0 : 18, io_pin_index);
}

// Select input io_pin for UART CTS
void PINCTRL_SelUartCtsIn(const uart_port_t port, const uint8_t io_pin_index)
{
    PINCTRL_SelIn(IO_MUX_CTRL5, port == UART_PORT_0 ? 6 : 12, io_pin_index);
}

// Select input io_pin for SPI DI
void PINCTRL_SelSpiDiIn(const spi_port_t port, const uint8_t io_pin_index)
{
    PINCTRL_SelIn(IO_MUX_CTRL6, port == SPI_PORT_0 ? 6 : 24, io_pin_index);
}

// Select input io_pin for SPI CLK
void PINCTRL_SelSpiClkIn(const spi_port_t port, const uint8_t io_pin_index)
{
    if (port == SPI_PORT_1)
        PINCTRL_SelIn(IO_MUX_CTRL5, 24, io_pin_index);
    else
        PINCTRL_SelIn(IO_MUX_CTRL6, 12, io_pin_index);
}

void PINCTRL_SelI2cSclIn(const i2c_port_t port, const uint8_t io_pin_index)
{
    PINCTRL_SelIn(IO_MUX_CTRL6, port == I2C_PORT_0 ? 0 : 18, io_pin_index);
}

void PINCTRL_DisableAllInputs(void)
{
    *(volatile uint32_t *)(SYSCTRL_BASE + IO_MUX_CTRL5) = 0x3fffffff;
    *(volatile uint32_t *)(SYSCTRL_BASE + IO_MUX_CTRL6) = 0x3fffffff;
}

