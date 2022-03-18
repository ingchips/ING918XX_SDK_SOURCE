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

void PINCTRL_SetSlewRate(const uint8_t io_pin_index, const pinctrl_slew_rate_t rate)
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

void PINCTRL_SetDriveStrength(const uint8_t io_pin_index, const pinctrl_drive_strenght_t strenght)
{
    volatile uint32_t *ds0= (volatile uint32_t *)(APB_PINC_BASE + 0x28);
    volatile uint32_t *ds1= (volatile uint32_t *)(APB_PINC_BASE + 0x30);
    if (((int)strenght) & 1)
    {
        *ds0 = *ds0 | (1 << io_pin_index);
    }
    else
    {
        *ds0 = *ds0 & ~(1 << io_pin_index);
    }

    if (((int)strenght) & 2)
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
    uint32_t mask = ((1 << bit_width) - 1) << bit_offset;
    *reg = (*reg & ~mask) | (v << bit_offset);
}

void PINCTRL_SelSwIn(uint8_t io_pin_tms, uint8_t io_pin_tck)
{
    PINCTRL_SetPadMux(io_pin_tms, IO_SOURCE_SW_TMS);
    PINCTRL_SetPadMux(io_pin_tms, IO_SOURCE_SW_TCK);
    APB_PINCTRL->IN_CTRL[0] &= ~0xfff;
    APB_PINCTRL->IN_CTRL[0] |= (io_pin_tck << 5) | io_pin_tms;
}

static void PINCTRL_SelInput(uint8_t io_pin,
                        io_source_t source_id,
                        int reg_index,
                        int bit_offset)
{
    if (io_pin != IO_NOT_A_PIN)
        PINCTRL_SetPadMux(io_pin, source_id);
    set_reg_bits(&APB_PINCTRL->IN_CTRL[reg_index], io_pin, 6, bit_offset);
}

void PINCTRL_SelSpiIn(spi_port_t port,
                      uint8_t io_pin_clk,
                      uint8_t io_pin_csn,
                      uint8_t io_pin_hold,
                      uint8_t io_pin_wp,
                      uint8_t io_pin_miso,
                      uint8_t io_pin_mosi)
{
    static const io_source_t source_tab[][6] =
    {
        {IO_SOURCE_SPI0_CLK_IN, IO_SOURCE_SPI0_CSN_IN, IO_SOURCE_SPI0_HOLD_OUT, IO_SOURCE_SPI0_WP_OUT, IO_SOURCE_SPI0_MISO_OUT, IO_SOURCE_SPI0_MOSI_OUT, },
        {IO_SOURCE_SPI1_CLK_IN, IO_SOURCE_SPI1_CSN_IN, IO_SOURCE_SPI1_HOLD_OUT, IO_SOURCE_SPI1_WP_OUT, IO_SOURCE_SPI1_MISO_OUT, IO_SOURCE_SPI1_MOSI_OUT, },
    };

    static const uint8_t reg_tab[][6] =
    {
        { 0, 0, 0, 1, 1, 1 },
        { 1, 1, 2, 2, 2, 2 },
    };

    static const uint8_t bit_offset_tab[][6] =
    {
        { 12, 18, 24, 0, 6, 12, },
        { 18, 24, 12,18, 0,  6, },
    };

    PINCTRL_SelInput(io_pin_clk, source_tab[port][0], reg_tab[port][0], bit_offset_tab[port][0]);
    PINCTRL_SelInput(io_pin_csn, source_tab[port][1], reg_tab[port][1], bit_offset_tab[port][1]);
    PINCTRL_SelInput(io_pin_hold,source_tab[port][2], reg_tab[port][2], bit_offset_tab[port][2]);
    PINCTRL_SelInput(io_pin_wp,  source_tab[port][3], reg_tab[port][3], bit_offset_tab[port][3]);
    PINCTRL_SelInput(io_pin_miso,source_tab[port][4], reg_tab[port][4], bit_offset_tab[port][4]);
    PINCTRL_SelInput(io_pin_mosi,source_tab[port][5], reg_tab[port][5], bit_offset_tab[port][5]);
}

void PINCTRL_SelIrIn(uint8_t io_pin_data)
{
    PINCTRL_SelInput(io_pin_data, IO_SOURCE_IR_DATA_IN, 2, 24);
}

void PINCTRL_SelI2sIn(uint8_t io_pin_bclk,
                      uint8_t io_pin_lrclk,
                      uint8_t io_pin_data)
{
    PINCTRL_SelInput(io_pin_bclk, IO_SOURCE_I2S_BCLK_IN, 3, 0);
    PINCTRL_SelInput(io_pin_lrclk,IO_SOURCE_I2S_LRCLK_IN,3, 6);
    PINCTRL_SelInput(io_pin_data, IO_SOURCE_I2S_DATA_IN, 3, 12);
}

void PINCTRL_SelUartIn(uart_port_t port,
                      uint8_t io_pin_rxd,
                      uint8_t io_pin_cts)
{
    switch (port)
    {
    case UART_PORT_0:
        PINCTRL_SelInput(io_pin_rxd, IO_SOURCE_UART0_RXD, 3, 18);
        PINCTRL_SelInput(io_pin_cts, IO_SOURCE_UART0_CTS, 3, 24);
        break;
    case UART_PORT_1:
        PINCTRL_SelInput(io_pin_rxd, IO_SOURCE_UART1_RXD, 4, 0);
        PINCTRL_SelInput(io_pin_cts, IO_SOURCE_UART1_CTS, 4, 6);
        break;
    default:
        break;
    }
}

void PINCTRL_SelI2cIn(i2c_port_t port,
                      uint8_t io_pin_scl,
                      uint8_t io_pin_sda)
{
    switch (port)
    {
    case I2C_PORT_0:
        PINCTRL_SelInput(io_pin_scl, IO_SOURCE_I2C0_SCL_OUT, 4, 12);
        PINCTRL_SelInput(io_pin_sda, IO_SOURCE_I2C0_SDA_OUT, 4, 18);
        break;
    case I2C_PORT_1:
        PINCTRL_SelInput(io_pin_scl, IO_SOURCE_I2C1_SCL_OUT, 4, 24);
        PINCTRL_SelInput(io_pin_sda, IO_SOURCE_I2C1_SDA_OUT, 5, 0);
        break;
    default:
        break;
    }
}

void PINCTRL_SelPdmIn(uint8_t io_pin_dmic)
{
    PINCTRL_SelInput(io_pin_dmic, IO_SOURCE_PDM_DMIC_IN, 5, 6);
}

void PINCTRL_SelKeyScanColIn(int index, uint8_t io_pin)
{
    if (index <= 2)
        PINCTRL_SelInput(io_pin, (io_source_t)(IO_SOURCE_KEYSCN_COL_0 + index), 5, 12 + index * 6);
    else
    {
        int reg = (index - 3) / 5 + 6;
        PINCTRL_SelInput(io_pin, (io_source_t)(IO_SOURCE_KEYSCN_COL_0 + index), reg, (index - 3) % 5 * 6);
    }
}

static int index_of_pull(io_source_t io_source)
{
    switch (io_source)
    {
    case IO_SOURCE_SW_DATA_OUT:
        return 0;
    case IO_SOURCE_SPI0_CLK_OUT:
        return 7;
    case IO_SOURCE_SPI0_CSN_OUT:
        return 8;
    case IO_SOURCE_SPI0_HOLD_OUT:
        return 9;
    case IO_SOURCE_SPI0_WP_OUT:
        return 10;
    case IO_SOURCE_SPI0_MISO_OUT:
        return 11;
    case IO_SOURCE_SPI0_MOSI_OUT:
        return 12;
    case IO_SOURCE_SPI1_CLK_OUT:
        return 13;
    case IO_SOURCE_SPI1_CSN_OUT:
        return 14;
    case IO_SOURCE_SPI1_MISO_OUT:
        return 15;
    case IO_SOURCE_SPI1_MOSI_OUT:
        return 16;
    case IO_SOURCE_SPI1_HOLD_OUT:
        return 17;
    case IO_SOURCE_SPI1_WP_OUT:
        return 18;
    case IO_SOURCE_IR_WAKEUP:
        return 19;
    case IO_SOURCE_IR_DATA_OUT:
        return 20;
    case IO_SOURCE_I2S_BCLK_OUT:
        return 21;
    case IO_SOURCE_I2S_LRCLK_OUT:
        return 22;
    case IO_SOURCE_I2S_DATA_OUT:
        return 23;
    case IO_SOURCE_UART0_RTS:
        return 24;
    case IO_SOURCE_UART0_TXD:
        return 25;
    case IO_SOURCE_UART1_RTS:
        return 26;
    case IO_SOURCE_UART1_TXD:
        return 27;
    case IO_SOURCE_I2C0_SCL_OUT:
        return 28;
    case IO_SOURCE_I2C0_SDA_OUT:
        return 29;
    case IO_SOURCE_I2C1_SCL_OUT:
        return 30;
    case IO_SOURCE_I2C1_SDA_OUT:
        return 31;
    case IO_SOURCE_SW_TMS:
        return 32 + 0;
    case IO_SOURCE_SW_TCK:
        return 32 + 1;
    case IO_SOURCE_SPI0_CLK_IN:
        return 32 + 2;
    case IO_SOURCE_SPI0_CSN_IN:
        return 32 + 3;
/*
    case IO_SOURCE_SPI0_HOLD_IN:
        return 32 + 4;
    case IO_SOURCE_SPI0_WP_IN:
        return 32 + 5;
    case IO_SOURCE_SPI0_MISO_IN:
        return 32 + 6;
    case IO_SOURCE_SPI0_MOSI_IN:
        return 32 + 7;
*/
    case IO_SOURCE_SPI1_CLK_IN:
        return 32 + 8;
    case IO_SOURCE_SPI1_CSN_IN:
        return 32 + 9;
/*
    case IO_SOURCE_SPI1_MISO_IN:
        return 32 + 10;
    case IO_SOURCE_SPI1_MOSI_IN:
        return 32 + 11;
    case IO_SOURCE_SPI1_HOLD_IN:
        return 32 + 12;
    case IO_SOURCE_SPI1_WP_IN:
        return 32 + 13;
*/
    case IO_SOURCE_IR_DATA_IN:
        return 32 + 14;
    case IO_SOURCE_I2S_BCLK_IN:
        return 32 + 15;
    case IO_SOURCE_I2S_LRCLK_IN:
        return 32 + 16;
    case IO_SOURCE_I2S_DATA_IN:
        return 32 + 17;
    case IO_SOURCE_UART0_RXD:
        return 32 + 18;
    case IO_SOURCE_UART0_CTS:
        return 32 + 19;
    case IO_SOURCE_UART1_RXD:
        return 32 + 20;
    case IO_SOURCE_UART1_CTS:
        return 32 + 21;
/*
    case IO_SOURCE_I2C0_SCL_IN:
        return 32 + 22;
    case IO_SOURCE_I2C0_SDA_IN:
        return 32 + 23;
    case IO_SOURCE_I2C1_SCL_IN:
        return 32 + 24;
    case IO_SOURCE_I2C1_SDA_IN:
        return 32 + 25;
*/
    case IO_SOURCE_KEYSCN_ROW_0:
        return 32 + 26;
    case IO_SOURCE_KEYSCN_COL_0:
        return 32 + 27;
    default:
        return -1;
    }
}

void PINCTRL_Pull(const io_source_t io_source, const pinctrl_pull_mode_t mode)
{
    int index = index_of_pull(io_source);
    if (index < 0) return;

    int reg = index >= 32 ? 1 : 0;
    int bit = 1 << (index & 0x1f);
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

}

void PINCTRL_SetPadMux(const uint8_t io_pin_index, const io_source_t source)
{
    set_reg_bits(&APB_PINCTRL->OUT_CTRL[io_pin_index >> 2], source, 8, 8 * (io_pin_index & 0x3));
}

void PINCTRL_DisableAllInputs(void)
{
    int i;
    for (i = 0; i <= 9; i++)
        APB_PINCTRL->IN_CTRL[i] = (uint32_t)-1;
}

void PINCTRL_SetSlewRate(const uint8_t io_pin_index, const pinctrl_slew_rate_t rate)
{

}

void PINCTRL_SetDriveStrength(const uint8_t io_pin_index, const pinctrl_drive_strenght_t strenght)
{

}

void PINCTRL_EnableAntSelPins(int count, const uint8_t *io_pins)
{
    int i;
    for (i = 0; i < count; i++)
        PINCTRL_SetPadMux(io_pins[i], (io_source_t)(IO_SOURCE_ANT_SW0 + i));
}

void PINCTRL_EnableAllAntSelPins(void)
{
    static const uint8_t ant_pins[] = {7, 8, 10, 11, 16, 17, 18, 19};

    PINCTRL_EnableAntSelPins(sizeof(ant_pins), ant_pins);
}

#endif
