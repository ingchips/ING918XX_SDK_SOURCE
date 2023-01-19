#include "ingsoc.h"
#include "peripheral_pinctrl.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

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

#define OFFSET_MUX_CTRL0        (0x44)
#define IO_MUX_CTRL5            (0x54)
#define IO_MUX_CTRL6            (0x58)
#define IO_MUX_CTRL7            (0x68)

#define OFFSET_PWM_OUT_IO_SEL0  (0x80)
#define OFFSET_PWM_OUT_IO_SEL1  (0x84)
#define OFFSET_ANT_SEL          (0x88)

int PINCTRL_SetPadMux(const uint8_t io_pin_index, const io_source_t source)
{
    volatile uint32_t * reg = (volatile uint32_t *)(SYSCTRL_BASE + OFFSET_MUX_CTRL0) + (io_pin_index >> 3);
    uint8_t offset = (io_pin_index & 0x7) << 2;
    *reg = (*reg & ~(IO_SOURCE_MASK << offset)) | (source << offset);
    return 0;
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
                shift = (io_pin_index - 8) * 4;
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

void PINCTRL_Pull(const uint8_t io_pin_index, const pinctrl_pull_mode_t mode)
{
    volatile uint32_t *pe = (volatile uint32_t *)(APB_PINC_BASE + 0x10);
    volatile uint32_t *ps = (volatile uint32_t *)(APB_PINC_BASE + 0x18);
    if (PINCTRL_PULL_DISABLE == mode)
    {
        *pe = *pe & ~(1 << io_pin_index);
    }
    else
    {
        if (PINCTRL_PULL_UP == mode)
            *ps = *ps | (1 << io_pin_index);
        else
            *ps = *ps & ~(1 << io_pin_index);

        *pe = *pe | (1 << io_pin_index);
    }
}

void PINCTRL_SetSlewRate(uint8_t io_pin_index, const pinctrl_slew_rate_t rate)
{
    volatile uint32_t *sr = (volatile uint32_t *)(APB_PINC_BASE + 0x20);
    if (rate)
    {
        *sr = *sr | (1 << io_pin_index);
    }
    else
    {
        *sr = *sr & ~(1 << io_pin_index);
    }
}

void PINCTRL_SetDriveStrength(const uint8_t io_pin_index, const pinctrl_drive_strength_t strength)
{
    volatile uint32_t *ds0= (volatile uint32_t *)(APB_PINC_BASE + 0x28);
    volatile uint32_t *ds1= (volatile uint32_t *)(APB_PINC_BASE + 0x30);
    if (((int)strength) & 1)
    {
        *ds0 = *ds0 | (1 << io_pin_index);
    }
    else
    {
        *ds0 = *ds0 & ~(1 << io_pin_index);
    }

    if (((int)strength) & 2)
    {
        *ds1 = *ds1 | (1 << io_pin_index);
    }
    else
    {
        *ds1 = *ds1 & ~(1 << io_pin_index);
    }
}

void PINCTRL_EnableAntSelPins(int count, const uint8_t *io_pins)
{
    int i;
    for (i = 0; i < count; i++)
    {
        PINCTRL_SetPadMux(io_pins[i], IO_SOURCE_GENERAL);
        PINCTRL_SetGeneralPadMode(io_pins[i], IO_MODE_ANT_SEL, 0, 0);
    }
}

void PINCTRL_EnableAllAntSelPins(void)
{
    static const uint8_t ant_pins[] = {7, 8, 10, 11, 16, 17, 18, 19};
    PINCTRL_EnableAntSelPins(sizeof(ant_pins) / sizeof(ant_pins[0]), ant_pins);
}

#elif  (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

static void set_reg_bits(volatile uint32_t *reg, uint32_t v, uint8_t bit_width, uint8_t bit_offset)
{
    uint32_t mask1 = (1 << bit_width) - 1;
    uint32_t mask = ~(mask1 << bit_offset);
    *reg = (*reg & mask) | ((v & mask1) << bit_offset);
}

const uint8_t io_output_source_map[IO_PIN_NUMBER][19] =
{
    {0xff,0xff,0xff,0xfa,0xaa,0xaa,0x48,0xc0,0x00,0x02,0x0f,0xff,0xff,0xff,0x00,0x00,0x03,0x87,0xe0},
    {0xff,0xff,0xff,0xf5,0x55,0x55,0x24,0xa0,0x00,0x01,0x0f,0xff,0xff,0xfe,0x80,0x00,0x03,0xa7,0xe0},
    {0xff,0xff,0xff,0xfa,0xaa,0xa8,0x90,0x90,0x00,0x00,0x8f,0xff,0xff,0xfe,0x40,0x00,0x03,0x97,0xe0},
    {0xff,0xff,0xff,0xf5,0x55,0x56,0x48,0x88,0x00,0x00,0x4f,0xff,0xff,0xfe,0x20,0x00,0x03,0xc7,0xe0},
    {0xff,0xff,0xff,0xfa,0xaa,0xa9,0x26,0x84,0x00,0x00,0x2f,0xff,0xff,0xfe,0x10,0x00,0x03,0x87,0xe0},
    {0xff,0xff,0xff,0xf5,0x55,0x54,0x92,0x82,0x00,0x00,0x1f,0xff,0xff,0xfe,0x08,0x00,0x03,0x8f,0xe0},
    {0xff,0xff,0xff,0xfa,0xaa,0xaa,0x4a,0x81,0x00,0x02,0x0f,0xff,0xff,0xfe,0x04,0x00,0x03,0x87,0xe0},
    {0xff,0xff,0xff,0xf5,0x55,0x55,0x26,0x80,0x80,0x01,0x0f,0xff,0xff,0xfe,0x02,0x00,0x03,0xa7,0xe0},
    {0xff,0xff,0xff,0xfa,0xaa,0xa8,0x92,0x80,0x40,0x00,0x8f,0xff,0xff,0xfe,0x01,0x00,0x03,0x97,0xe0},
    {0xff,0xff,0xff,0xf5,0x55,0x56,0x4a,0x80,0x20,0x00,0x4f,0xff,0xff,0xfe,0x00,0x80,0x03,0xc7,0xe0},
    {0xff,0xff,0xff,0xfa,0xaa,0xa9,0x26,0x80,0x10,0x00,0x2f,0xff,0xff,0xfe,0x00,0x40,0x03,0x87,0xe0},
    {0xff,0xff,0xff,0xf5,0x55,0x54,0x91,0x80,0x08,0x00,0x1f,0xff,0xff,0xfe,0x00,0x20,0x03,0x8f,0xe0},
    {0xff,0xff,0xff,0xfa,0xaa,0xaa,0x49,0x80,0x04,0x00,0x0f,0xff,0xff,0xfe,0x00,0x10,0x03,0x87,0xe0},
    {0xff,0xff,0xff,0xf5,0x55,0x55,0x25,0x80,0x02,0x00,0x0f,0xff,0xff,0xfe,0x00,0x08,0x03,0x87,0xe0},
    {0xff,0xff,0xff,0xfa,0xaa,0xa8,0x91,0x80,0x01,0x00,0x0f,0xff,0xff,0xfe,0x00,0x04,0x03,0x87,0xe0},
    {0xff,0xff,0xff,0xf5,0x55,0x56,0x49,0x80,0x00,0x80,0x0f,0xff,0xff,0xfe,0x00,0x02,0x13,0x87,0xe0},
    {0xff,0xff,0xff,0xfa,0xaa,0xa9,0x25,0x80,0x00,0x40,0x0f,0xff,0xff,0xfe,0x00,0x01,0x0b,0x87,0xe0},
    {0xff,0xff,0xff,0xf5,0x55,0x54,0x91,0x80,0x00,0x24,0x0f,0xff,0xff,0xfe,0x00,0x00,0x87,0x87,0xe0},
    {0xc8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xd0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xc4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xff,0xff,0xff,0xfa,0xaa,0xaa,0x48,0x80,0x00,0x10,0x0f,0xff,0xff,0xfe,0x00,0x00,0x43,0x87,0xe0},
    {0xff,0xff,0xff,0xf5,0x55,0x55,0x24,0x80,0x00,0x08,0x0f,0xff,0xff,0xfe,0x00,0x00,0x23,0x87,0xe0},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00},
    {0xc2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xc1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xc0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00},
    {0xff,0xff,0xff,0xfa,0xaa,0xa8,0x90,0x82,0x00,0x00,0x0f,0xff,0xff,0xfe,0x08,0x00,0x03,0x87,0xe0},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00},
    {0xff,0xff,0xff,0xf5,0x55,0x56,0x4b,0x80,0x40,0x00,0x0f,0xff,0xff,0xfe,0x01,0x00,0x03,0x87,0xe0},
    {0xff,0xff,0xff,0xfa,0xaa,0xa9,0x27,0x80,0x20,0x00,0x0f,0xff,0xff,0xfe,0x00,0x80,0x03,0x87,0xe0},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00},
    {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00}
};

static int source_id_on_pin(uint8_t io_pin_index, io_source_t source)
{
    if (io_pin_index >= IO_PIN_NUMBER) return -1;

    const uint8_t *map = io_output_source_map[io_pin_index];
    int r = 0;
    int i = source - 1;
    if ((map[source / 8] & (0x80 >> (source & 7))) == 0) return -1;

    for (; i >= 0; i--)
    {
        if (map[i / 8] & (0x80 >> (i & 7)))
            r++;
    }
    return r;
}

static int pin_id_for_input_source(int source, uint8_t io_pin_index)
{
    int byte_n = source / 8;
    int bit_mask = (0x80 >> (source & 7));
    int r = 0;
    int i = (int)io_pin_index - 1;
    const uint8_t *map = io_output_source_map[io_pin_index];
    if ((map[byte_n] & bit_mask) == 0) return -1;

    for (; i >= 0; i--)
    {
        map = io_output_source_map[i];
        if (map[byte_n] & bit_mask)
            r++;
    }

    return r;
}

static int PINCTRL_SelInput(uint8_t io_pin,
                        int source_id,
                        int reg_index,
                        int bit_width,
                        int bit_offset)
{
    if (io_pin == IO_NOT_A_PIN)
    {
        set_reg_bits(&APB_PINCTRL->IN_CTRL[reg_index], (uint32_t)(-1), bit_width, bit_offset);
        return 0;
    }

    int id = pin_id_for_input_source(source_id, io_pin);
    if (id < 0) return id;
    set_reg_bits(&APB_PINCTRL->IN_CTRL[reg_index], (uint32_t)id, bit_width, bit_offset);
    PINCTRL_SetPadMux((uint8_t)io_pin, (io_source_t)(source_id));
    return 0;
}

int PINCTRL_SelSwIn(uint8_t io_pin_dio, uint8_t io_pin_clk)
{
    if (PINCTRL_SelInput(io_pin_dio, IO_SOURCE_SW_DIO, 0, 5, 0)) return -1;
    if (PINCTRL_SelInput(io_pin_clk, IO_SOURCE_SW_CLK, 0, 5, 5)) return -2;
    PINCTRL_SetPadMux(io_pin_dio, IO_SOURCE_SW_DATA_OUT);
    return 0;
}

int PINCTRL_SelSpiIn(spi_port_t port,
                      uint8_t io_pin_clk,
                      uint8_t io_pin_csn,
                      uint8_t io_pin_hold,
                      uint8_t io_pin_wp,
                      uint8_t io_pin_miso,
                      uint8_t io_pin_mosi)
{
    static const io_source_t source_tab[][6] =
    {
        {IO_SOURCE_SPI0_CLK_IN, IO_SOURCE_SPI0_CSN_IN, IO_SOURCE_SPI0_HOLD_IN, IO_SOURCE_SPI0_WP_IN, IO_SOURCE_SPI0_MISO_IN, IO_SOURCE_SPI0_MOSI_IN},
        {IO_SOURCE_SPI1_CLK_IN, IO_SOURCE_SPI1_CSN_IN, IO_SOURCE_SPI1_HOLD_IN, IO_SOURCE_SPI1_WP_IN, IO_SOURCE_SPI1_MISO_IN, IO_SOURCE_SPI1_MOSI_IN},
    };

    static const uint8_t reg_tab[][6] =
    {
        { 0, 0, 0, 1, 1, 1 },
        { 1, 1, 2, 2, 2, 2 },
    };

    static const uint8_t bit_offset_tab[][6] =
    {
        { 10, 15, 20, 0, 5, 10, },
        { 15, 20, 10,15, 0,  5, },
    };

    if (PINCTRL_SelInput(io_pin_clk, source_tab[port][0], reg_tab[port][0], 5, bit_offset_tab[port][0])) return -1;
    if (PINCTRL_SelInput(io_pin_csn, source_tab[port][1], reg_tab[port][1], 5, bit_offset_tab[port][1])) return -2;
    if (PINCTRL_SelInput(io_pin_hold,source_tab[port][2], reg_tab[port][2], 5, bit_offset_tab[port][2])) return -3;
    if (PINCTRL_SelInput(io_pin_wp,  source_tab[port][3], reg_tab[port][3], 5, bit_offset_tab[port][3])) return -4;
    if (PINCTRL_SelInput(io_pin_miso,source_tab[port][4], reg_tab[port][4], 5, bit_offset_tab[port][4])) return -5;
    if (PINCTRL_SelInput(io_pin_mosi,source_tab[port][5], reg_tab[port][5], 5, bit_offset_tab[port][5])) return -6;
    return 0;
}

int PINCTRL_SelIrIn(uint8_t io_pin_data)
{
    return PINCTRL_SelInput(io_pin_data, IO_SOURCE_IR_DATA_IN, 2, 5, 20);
}

int PINCTRL_SelI2sIn(uint8_t io_pin_bclk,
                      uint8_t io_pin_lrclk,
                      uint8_t io_pin_data)
{
    if (PINCTRL_SelInput(io_pin_bclk, IO_SOURCE_I2S_BCLK_IN, 3, 5, 0)) return -1;
    if (PINCTRL_SelInput(io_pin_lrclk,IO_SOURCE_I2S_LRCLK_IN,3, 5, 5)) return -1;
    if (PINCTRL_SelInput(io_pin_data, IO_SOURCE_I2S_DATA_IN, 3, 5, 10)) return -1;
    return 0;
}

int PINCTRL_SelUartIn(uart_port_t port,
                      uint8_t io_pin_rxd,
                      uint8_t io_pin_cts)
{
    switch (port)
    {
    case UART_PORT_0:
        if (PINCTRL_SelInput(io_pin_rxd, IO_SOURCE_UART0_RXD, 3, 5, 15)) return -1;
        if (PINCTRL_SelInput(io_pin_cts, IO_SOURCE_UART0_CTS, 3, 5, 20)) return -1;
        break;
    case UART_PORT_1:
        if (PINCTRL_SelInput(io_pin_rxd, IO_SOURCE_UART1_RXD, 4, 5, 0)) return -1;
        if (PINCTRL_SelInput(io_pin_cts, IO_SOURCE_UART1_CTS, 4, 5, 5)) return -1;
        break;
    default:
        break;
    }
    return 0;
}

void PINCTRL_SelUartRxdIn(const uart_port_t port, const uint8_t io_pin_index)
{
    switch (port)
    {
    case UART_PORT_0:
        PINCTRL_SelInput(io_pin_index, IO_SOURCE_UART0_RXD, 3, 5, 15);
        break;
    case UART_PORT_1:
        PINCTRL_SelInput(io_pin_index, IO_SOURCE_UART1_RXD, 4, 5, 0);
        break;
    default:
        break;
    }
}

void PINCTRL_SelUartCtsIn(const uart_port_t port, const uint8_t io_pin_index)
{
    switch (port)
    {
    case UART_PORT_0:
        PINCTRL_SelInput(io_pin_index, IO_SOURCE_UART0_CTS, 3, 5, 20);
        break;
    case UART_PORT_1:
        PINCTRL_SelInput(io_pin_index, IO_SOURCE_UART1_CTS, 4, 5, 5);
        break;
    default:
        break;
    }
}

int PINCTRL_SelI2cIn(i2c_port_t port,
                      uint8_t io_pin_scl,
                      uint8_t io_pin_sda)
{
    switch (port)
    {
    case I2C_PORT_0:
        if (PINCTRL_SelInput(io_pin_scl, IO_SOURCE_I2C0_SCL_IN, 4, 5, 10)) return -1;
        if (PINCTRL_SelInput(io_pin_sda, IO_SOURCE_I2C0_SDA_IN, 4, 5, 15)) return -1;
        PINCTRL_SetPadMux(io_pin_scl, IO_SOURCE_I2C0_SCL_OUT);
        PINCTRL_SetPadMux(io_pin_sda, IO_SOURCE_I2C0_SDA_OUT);
        break;
    case I2C_PORT_1:
        if (PINCTRL_SelInput(io_pin_scl, IO_SOURCE_I2C1_SCL_IN, 4, 5, 20)) return -1;
        if (PINCTRL_SelInput(io_pin_sda, IO_SOURCE_I2C1_SDA_IN, 5, 5, 0)) return -1;
        PINCTRL_SetPadMux(io_pin_scl, IO_SOURCE_I2C1_SCL_OUT);
        PINCTRL_SetPadMux(io_pin_sda, IO_SOURCE_I2C1_SDA_OUT);
        break;
    default:
        break;
    }
    if (io_pin_scl < IO_PIN_NUMBER)
        PINCTRL_Pull(io_pin_scl, PINCTRL_PULL_UP);
    if (io_pin_sda < IO_PIN_NUMBER)
        PINCTRL_Pull(io_pin_sda, PINCTRL_PULL_UP);
    return 0;
}

void PINCTRL_SelI2cSclIn(const i2c_port_t port, const uint8_t io_pin_index)
{
    switch (port)
    {
    case I2C_PORT_0:
        PINCTRL_SelInput(io_pin_index, IO_SOURCE_I2C0_SCL_IN, 4, 5, 10);
        PINCTRL_SetPadMux(io_pin_index, IO_SOURCE_I2C0_SCL_OUT);
        break;
    case I2C_PORT_1:
        PINCTRL_SelInput(io_pin_index, IO_SOURCE_I2C1_SCL_IN, 4, 5, 20);
        PINCTRL_SetPadMux(io_pin_index, IO_SOURCE_I2C1_SCL_OUT);
        break;
    default:
        break;
    }
    if (io_pin_index < IO_PIN_NUMBER)
        PINCTRL_Pull(io_pin_index, PINCTRL_PULL_UP);
}

int PINCTRL_SelPdmIn(uint8_t io_pin_dmic)
{
    return PINCTRL_SelInput(io_pin_dmic, IO_SOURCE_PDM_DMIC_IN, 5, 5, 5);
}

int PINCTRL_SelKeyScanColIn(int index, uint8_t io_pin)
{
    if (index <= 2)
        return PINCTRL_SelInput(io_pin, IO_SOURCE_KEYSCN_IN_COL_0 + index, 5, 2, 10 + index * 2);
    else if (index <= 7)
        return PINCTRL_SelInput(io_pin, IO_SOURCE_KEYSCN_IN_COL_0 + index, 6, 2, (index - 3) * 2);
    else if (index <= 15)
        return PINCTRL_SelInput(io_pin, IO_SOURCE_KEYSCN_IN_COL_0 + index, 7, 2, 11 + (index - 8) * 2);
    else if (index <= 17)
        return PINCTRL_SelInput(io_pin, IO_SOURCE_KEYSCN_IN_COL_0 + index, 7, 1, 27 + (index - 16));
    else if (index <= 19)
        return PINCTRL_SelInput(io_pin, IO_SOURCE_KEYSCN_IN_COL_0 + index, 9, 1, (index - 18));
    else
        return -1;
}

int PINCTRL_SelPCAPIn(int index, uint8_t io_pin)
{
    return PINCTRL_SelInput(io_pin, IO_SOURCE_PCAP0_IN + index, 10, 5, index * 5);
}

int PINCTRL_SelQDECIn(uint8_t phase_a,
                      uint8_t phase_b)
{
    if (PINCTRL_SelInput(phase_a, IO_SOURCE_QDEC_PHASEA, 9, 5, 5)) return -1;
    if (PINCTRL_SelInput(phase_b, IO_SOURCE_QDEC_PHASEB, 9, 5, 10)) return -2;
    return 0;
}

int PINCTRL_Pull(const uint8_t io_pin, const pinctrl_pull_mode_t mode)
{
    int index = io_pin;
    int reg = index >= 32 ? 1 : 0;
    int bit = 1ul << (index & 0x1f);
    volatile uint32_t *pe = (volatile uint32_t *)&APB_PINCTRL->PE_CTRL[reg];
    volatile uint32_t *ps = (volatile uint32_t *)&APB_PINCTRL->PS_CTRL[reg];
    if (PINCTRL_PULL_DISABLE == mode)
    {
        *pe &= ~bit;
    }
    else
    {
        if (PINCTRL_PULL_UP == mode)
            *ps |= bit;
        else
            *ps &= ~bit;

        *pe |= bit;
    }
    return 0;
}

int PINCTRL_SetPadMux(const uint8_t io_pin_index, const io_source_t source)
{
    int r = source_id_on_pin(io_pin_index, source);
    if (r < 0) return r;

    if (io_pin_index <= 17)
        set_reg_bits(&APB_PINCTRL->OUT_CTRL[io_pin_index >> 2], (uint32_t)r, 7, 7 * (io_pin_index & 0x3));
    else if (io_pin_index <= 20)
        set_reg_bits(&APB_PINCTRL->OUT_CTRL[4], (uint32_t)r, 2, 14 + 2 * (io_pin_index - 18));
    else if (io_pin_index == 21)
        set_reg_bits(&APB_PINCTRL->OUT_CTRL[4], (uint32_t)r, 7, 20);
    else if (io_pin_index == 22)
        set_reg_bits(&APB_PINCTRL->OUT_CTRL[5], (uint32_t)r, 7, 0);
    else if (io_pin_index <= 30)
        set_reg_bits(&APB_PINCTRL->OUT_CTRL[5], (uint32_t)r, 2, 7 + 2 * (io_pin_index - 23));
    else if (io_pin_index == 31)
        set_reg_bits(&APB_PINCTRL->OUT_CTRL[5], (uint32_t)r, 7, 23);
    else if (io_pin_index == 32)
        set_reg_bits(&APB_PINCTRL->OUT_CTRL[5], (uint32_t)r, 2, 30);
    else if (io_pin_index == 33)
        set_reg_bits(&APB_PINCTRL->OUT_CTRL[6], (uint32_t)r, 2, 0);
    else if (io_pin_index <= 35)
        set_reg_bits(&APB_PINCTRL->OUT_CTRL[6], (uint32_t)r, 7, 2 + 7 * (io_pin_index - 34));
    else
        set_reg_bits(&APB_PINCTRL->OUT_CTRL[6], (uint32_t)r, 2, 16 + 2 * (io_pin_index - 36));
    return 0;
}

void PINCTRL_DisableAllInputs(void)
{
    int i;
    for (i = 0; i <= sizeof(APB_PINCTRL->IN_CTRL) / sizeof(APB_PINCTRL->IN_CTRL[0]); i++)
        APB_PINCTRL->IN_CTRL[i] = (uint32_t)-1;
}

void PINCTRL_SetDriveStrength(const uint8_t io_pin_index, const pinctrl_drive_strength_t strength)
{
    int reg = io_pin_index / 16;
    int index = io_pin_index & 0xf;
    uint32_t mask = 3 << (2 * index);
    uint32_t val = ((uint32_t)strength) << (index * 2);
    if (io_pin_index == 1) val ^= 0x4ul;
    volatile uint32_t *dr = (volatile uint32_t *)&APB_PINCTRL->DR_CTRL[reg];
    *dr &= ~mask;
    *dr |= val;
}

int PINCTRL_EnableAntSelPins(int count, const uint8_t *io_pins)
{
    int i;
    for (i = 0; i < count; i++)
    {
        if (io_pins[i] == IO_NOT_A_PIN) continue;
        if (PINCTRL_SetPadMux(io_pins[i], (io_source_t)(IO_SOURCE_ANT_SW0 + i)))
            return -(i + 1);
    }
    return 0;
}

int PINCTRL_SelUSB(const uint8_t dp_io_pin_index, const uint8_t dm_io_pin_index)
{
    if ((dp_io_pin_index != 16) || (dm_io_pin_index != 17))
        return -1;
    set_reg_bits(&APB_PINCTRL->IN_CTRL[9], (uint32_t)1, 1, 3);
    set_reg_bits(&APB_PINCTRL->IN_CTRL[9], (uint32_t)1, 1, 4);
    PINCTRL_EnableAnalog((GIO_Index_t)dp_io_pin_index);
    PINCTRL_EnableAnalog((GIO_Index_t)dm_io_pin_index);
    return 0;
}

void PINCTRL_EnableAnalog(const uint8_t io_index)
{
    PINCTRL_SetPadMux(io_index, IO_SOURCE_GPIO);
    PINCTRL_Pull(io_index, PINCTRL_PULL_DISABLE);

    // GIO_SetDirection(io_index, GIO_DIR_NONE)
    uint8_t start_gpio1 = GIO_GPIO_NUMBER / 2;
    GIO_TypeDef *pDef = io_index >= start_gpio1 ? APB_GPIO1 : APB_GPIO0;
    int index = io_index >= start_gpio1 ? io_index - start_gpio1 : io_index;
    uint32_t mask = ~(1ul << index);
    pDef->ChDir &= mask;
    pDef->IOIE &= mask;
}

#endif
