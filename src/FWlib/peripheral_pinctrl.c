#include "peripheral_pinctrl.h"

#define OFFSET_MUX_CTRL0        (0x44)
#define IO_MUX_CTRL5            (0x54)
#define IO_MUX_CTRL6            (0x58)
#define IO_MUX_CTRL7            (0x68)

#define OFFSET_PWM_OUT_IO_SEL0  (0x80)
#define OFFSET_PWM_OUT_IO_SEL1  (0x84)
#define OFFSET_ANT_SEL          (0x88)

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

static int PINCTRL_AntSelIndex(const uint8_t io_pin_index)
{
    switch (io_pin_index)
    {
    case 7:
        return 0;
    case 8:
        return 1;
    case 10:
        return 2;
    case 11:
        return 3;
    case 16:
        return 4;
    case 17:
        return 5;
    case 18:
        return 6;
    case 19:
        return 7;
    }
    return -1;
}

void PINCTRL_SetGeneralPadMode(const uint8_t io_pin_index, const gio_mode_t mode,
    const uint8_t pwm_channel, const uint8_t pwm_neg)
{
    int index;
    switch (mode)
    {
    case IO_MODE_GPIO:
        if (io_pin_index <= 11)
        {
            volatile uint32_t * reg = (volatile uint32_t *)(SYSCTRL_BASE + IO_MUX_CTRL7);
            *reg = *reg & ~(1 << io_pin_index);
        }
        index = PINCTRL_AntSelIndex(io_pin_index);
        if (index >= 0)
        {
            volatile uint32_t * reg = (volatile uint32_t *)(SYSCTRL_BASE + OFFSET_ANT_SEL);
            *reg = *reg & ~(1 << index);
        }
        break;
    case IO_MODE_PWM:
        if (io_pin_index <= 11)
        {
            uint8_t shift;
            volatile uint32_t * reg = (volatile uint32_t *)(SYSCTRL_BASE + IO_MUX_CTRL7);
            *reg |= 1 << io_pin_index;

            if (io_pin_index <= 7)
            {
                reg = (volatile uint32_t *)(SYSCTRL_BASE + OFFSET_PWM_OUT_IO_SEL0);
                shift = io_pin_index * 4;
            }
            else
            {
                reg = (volatile uint32_t *)(SYSCTRL_BASE + OFFSET_PWM_OUT_IO_SEL1);
                shift = (io_pin_index - 7) * 4;
            }

            *reg &= ~(0xf << shift);
            *reg |= (pwm_channel * 2 + pwm_neg) << shift;
        }
        break;
    case IO_MODE_ANT_SEL:
        index = PINCTRL_AntSelIndex(io_pin_index);
        if (index >= 0)
        {
            volatile uint32_t * reg = (volatile uint32_t *)(SYSCTRL_BASE + OFFSET_ANT_SEL);
            *reg |= 1 << index;

            reg = (volatile uint32_t *)(SYSCTRL_BASE + IO_MUX_CTRL7);
            *reg &= ~(1 << io_pin_index);
        }
        break;
    }
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

