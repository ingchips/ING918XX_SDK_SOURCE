#include "ingsoc.h"
#include "peripheral_sysctrl.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

void SYSCTRL_SetClkGate(SYSCTRL_ClkGateItem item)
{
    AHB_SYSCTRL->SYSCTRL_ClkGate &= ~(1 << item);
}

void SYSCTRL_ClearClkGate(SYSCTRL_ClkGateItem item)
{
    AHB_SYSCTRL->SYSCTRL_ClkGate |= (1 << item);
}

void SYSCTRL_SetClkGateMulti(uint32_t items)
{
    AHB_SYSCTRL->SYSCTRL_ClkGate &= ~items;
}

void SYSCTRL_ClearClkGateMulti(uint32_t items)
{
    AHB_SYSCTRL->SYSCTRL_ClkGate |= items;
}

void SYSCTRL_WriteClkGate(uint32_t data)
{
    AHB_SYSCTRL->SYSCTRL_ClkGate = data;
}

uint32_t SYSCTRL_ReadClkGate(void)
{
    return AHB_SYSCTRL->SYSCTRL_ClkGate;
}

void SYSCTRL_ResetBlock(SYSCTRL_ResetItem item)
{
    AHB_SYSCTRL->RstSet &= ~(1 << item);
}

void SYSCTRL_ResetAllBlocks(void)
{
    AHB_SYSCTRL->RstSet = 0;
}

void SYSCTRL_ReleaseBlock(SYSCTRL_ResetItem item)
{
    AHB_SYSCTRL->RstSet |= (1 << item);
}

void SYSCTRL_WriteBlockRst(uint32_t data)
{
    AHB_SYSCTRL->RstSet = data;
}

uint32_t SYSCTRL_ReadBlockRst(void)
{
    return AHB_SYSCTRL->RstSet;
}

void SYSCTRL_SetLDOOutput(int level)
{
    *(volatile uint32_t*)(0x40040060) =  ((*(volatile uint32_t*)(0x40040060)) & 0xffffff03)|(level<<2);
}

void SYSCTRL_WaitForLDO(void)
{
    while ((io_read(0x40040088) & (0x7 << 14)) != (0x7 << 14)) ;
}

void SYSCTRL_ConfigBOR(int threshold, int enable_active, int enable_sleep)
{
    #define RTC_POR0 (APB_RTC_BASE + 0x70)
    #define RTC_POR1 (APB_RTC_BASE + 0x74)

    io_write(RTC_POR1, (io_read(RTC_POR1) & ~0x1f) | threshold);
    io_write(RTC_POR0, (io_read(RTC_POR0) & ~0x3ul) | (enable_active << 1) | enable_sleep);
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

static void set_reg_bits(volatile uint32_t *reg, uint32_t v, uint8_t bit_width, uint8_t bit_offset)
{
    uint32_t mask = ((1 << bit_width) - 1) << bit_offset;
    *reg = (*reg & ~mask) | (v << bit_offset);
}

static void set_reg_bit(volatile uint32_t *reg, uint8_t v, uint8_t bit_offset)
{
    uint32_t mask = 1 << bit_offset;
    *reg = (*reg & ~mask) | (v << bit_offset);
}

static void SYSCTRL_ClkGateCtrl(SYSCTRL_ClkGateItem item, uint8_t v)
{
    switch (item)
    {
    case SYSCTRL_ITEM_APB_GPIO0:
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 21);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 18);
        break;
    case SYSCTRL_ITEM_APB_GPIO1:
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 22);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 18);
        break;
    case SYSCTRL_ITEM_APB_TMR0      :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 2);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 0);
        break;
    case SYSCTRL_ITEM_APB_TMR1      :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 3);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 1);
        break;
    case SYSCTRL_ITEM_APB_TMR2      :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 4);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 2);
        break;
    case SYSCTRL_ITEM_APB_WDT       :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 1);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 17);
        break;
    case SYSCTRL_ITEM_APB_PWM       :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 5);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 10);
        break;
    case SYSCTRL_ITEM_APB_PDM       :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 8);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 9);
        break;
    case SYSCTRL_ITEM_APB_QDEC      :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 9);
        set_reg_bit(&APB_SYSCTRL->QdecCfg, v, 12);
        break;
    case SYSCTRL_ITEM_APB_KeyScan   :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 10);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 19);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 20);
        break;
    case SYSCTRL_ITEM_APB_IR        :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 11);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 11);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 16);
        break;
    case SYSCTRL_ITEM_APB_DMA       :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 12);
        set_reg_bit(APB_SYSCTRL->CguCfg + 2, v, 0);
        break;
    case SYSCTRL_ITEM_AHB_SPI0      :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 13);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 6);
        break;
    case SYSCTRL_ITEM_APB_SPI1      :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 14);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 7);
        break;
    case SYSCTRL_ITEM_APB_ADC       :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 15);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 12);
        break;
    case SYSCTRL_ITEM_APB_I2S       :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 16);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 8);
        break;
    case SYSCTRL_ITEM_APB_UART0     :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 17);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 4);
        break;
    case SYSCTRL_ITEM_APB_UART1     :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 18);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 5);
        break;
    case SYSCTRL_ITEM_APB_I2C0      :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 19);
        break;
    case SYSCTRL_ITEM_APB_I2C1      :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 20);
        break;
    case SYSCTRL_ITEM_APB_SysCtrl   :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 0);
        break;
    case SYSCTRL_ITEM_APB_PinCtrl   :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 6);
        break;
    case SYSCTRL_ITEM_APB_EFUSE     :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 23);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 14);
        break;
    case SYSCTRL_ITEM_APB_USB:
        set_reg_bit(&APB_SYSCTRL->USBCfg, v, 5);
        set_reg_bit(&APB_SYSCTRL->USBCfg, v, 7);
        break;
    case SYSCTRL_ITEM_APB_LPC:
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 25);
        break;
    default:
        break;
    }
}

void SYSCTRL_SetClkGate(SYSCTRL_ClkGateItem item)
{
    SYSCTRL_ClkGateCtrl(item, 0);
}

void SYSCTRL_ClearClkGate(SYSCTRL_ClkGateItem item)
{
    SYSCTRL_ClkGateCtrl(item, 1);
}

void SYSCTRL_SetClkGateMulti(uint32_t items)
{
    SYSCTRL_Item i;
    for (i = (SYSCTRL_Item)0; i < SYSCTRL_ITEM_NUMBER; i++)
        if (items & (1 << i))
            SYSCTRL_SetClkGate(i);
}

void SYSCTRL_ClearClkGateMulti(uint32_t items)
{
    SYSCTRL_Item i;
    for (i = (SYSCTRL_Item)0; i < SYSCTRL_ITEM_NUMBER; i++)
        if (items & (1 << i))
            SYSCTRL_ClearClkGate(i);
}

static void SYSCTRL_ResetBlockCtrl(SYSCTRL_ResetItem item, uint8_t v)
{
    switch (item)
    {
    case SYSCTRL_ITEM_APB_GPIO0:
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 31);
        break;
    case SYSCTRL_ITEM_APB_GPIO1:
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 0);
        break;
    case SYSCTRL_ITEM_APB_TMR0      :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 12);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 2);
        break;
    case SYSCTRL_ITEM_APB_TMR1      :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 13);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 3);
        break;
    case SYSCTRL_ITEM_APB_TMR2      :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 14);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 4);
        break;
    case SYSCTRL_ITEM_APB_WDT       :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 11);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 18);
        break;
    case SYSCTRL_ITEM_APB_PWM       :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 15);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 12);
        break;
    case SYSCTRL_ITEM_APB_PDM       :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 18);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 11);
        break;
    case SYSCTRL_ITEM_APB_QDEC      :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 19);
        set_reg_bit(&APB_SYSCTRL->QdecCfg, v, 14);
        break;
    case SYSCTRL_ITEM_APB_KeyScan   :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 20);
        break;
    case SYSCTRL_ITEM_APB_IR        :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 21);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 13);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 17);
        break;
    case SYSCTRL_ITEM_APB_DMA       :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 22);
        break;
    case SYSCTRL_ITEM_AHB_SPI0      :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 23);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 8);
        break;
    case SYSCTRL_ITEM_APB_SPI1      :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 24);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 9);
        break;
    case SYSCTRL_ITEM_APB_ADC       :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 25);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 14);
        break;
    case SYSCTRL_ITEM_APB_I2S       :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 26);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 10);
        break;
    case SYSCTRL_ITEM_APB_UART0     :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 27);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 6);
        break;
    case SYSCTRL_ITEM_APB_UART1     :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 28);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 7);
        break;
    case SYSCTRL_ITEM_APB_I2C0      :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 29);
        break;
    case SYSCTRL_ITEM_APB_I2C1      :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 30);
        break;
    case SYSCTRL_ITEM_APB_SysCtrl   :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 10);
        break;
    case SYSCTRL_ITEM_APB_PinCtrl   :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 16);
        break;
    case SYSCTRL_ITEM_APB_EFUSE     :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 1);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 15);
        break;
    default:
        break;
    }
}

void SYSCTRL_ResetBlock(SYSCTRL_ResetItem item)
{
    SYSCTRL_ResetBlockCtrl(item, 0);
}

void SYSCTRL_ReleaseBlock(SYSCTRL_ResetItem item)
{
    SYSCTRL_ResetBlockCtrl(item, 1);
}

void SYSCTRL_SelectTimerClk(timer_port_t port, SYSCTRL_ClkMode mode)
{
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, (mode == 0) ? 1 : 0, 15 + port);
    if (mode >= SYSCTRL_CLK_SLOW_DIV_1)
    {
        set_reg_bits(&APB_SYSCTRL->CguCfg8, mode, 4, 20);
        set_reg_bit(&APB_SYSCTRL->CguCfg8, 1, 24);
    }
}

void SYSCTRL_SelectPWMClk(SYSCTRL_ClkMode mode)
{
    set_reg_bit(&APB_SYSCTRL->CguCfg8, (mode == 0) ? 1 : 0, 15);
    if (mode >= SYSCTRL_CLK_SLOW_DIV_1)
    {
        set_reg_bits(&APB_SYSCTRL->CguCfg8, mode, 4, 27);
        set_reg_bit(&APB_SYSCTRL->CguCfg8, 1, 31);
    }
}

void SYSCTRL_SelectKeyScanClk(SYSCTRL_ClkMode mode)
{
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, (mode == 0) ? 1 : 0, 13);
    if (mode >= SYSCTRL_CLK_SLOW_DIV_1)
    {
        set_reg_bits(APB_SYSCTRL->CguCfg, mode, 4, 24);
        set_reg_bit(APB_SYSCTRL->CguCfg, 1, 31);
    }
}

void SYSCTRL_SelectPDMClk(SYSCTRL_ClkMode mode)
{
    set_reg_bits(&APB_SYSCTRL->CguCfg8, (1 << 6) | mode, 12, 0);
    set_reg_bit(&APB_SYSCTRL->CguCfg8, 1, 12);
}

void SYSCTRL_SelectSpiClk(spi_port_t port, SYSCTRL_ClkMode mode)
{
    switch (port)
    {
    case SPI_PORT_0:
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, mode == 0 ? 0 : 1, 21);
        if (mode >= SYSCTRL_CLK_PLL_DIV_1)
        {
            set_reg_bits(APB_SYSCTRL->CguCfg, mode, 4, 20);
            set_reg_bit(APB_SYSCTRL->CguCfg + 1, 1, 30);
        }
        break;
    case SPI_PORT_1:
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, mode & 1, 22);
        break;
    default:
        break;
    }
}

void SYSCTRL_SelectUartClk(uart_port_t port, SYSCTRL_ClkMode mode)
{
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, mode & 1, port == UART_PORT_0 ? 19 : 20);
}

void SYSCTRL_SelectI2sClk(SYSCTRL_ClkMode mode)
{
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, mode == 0 ? 0 : 1, 23);
    if (mode >= SYSCTRL_CLK_PLL_DIV_1)
    {
        set_reg_bits(APB_SYSCTRL->CguCfg, mode, 4, 12);
        set_reg_bit(APB_SYSCTRL->CguCfg, 1, 31);
    }
}

#define AON1_BOOT   (volatile uint32_t *)(AON1_CTRL_BASE + 0x14)

void SYSCTRL_SelectHClk(SYSCTRL_ClkMode mode)
{
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, mode == 0 ? 0 : 1, 14);
    set_reg_bit(AON1_BOOT, mode == 0 ? 0 : 1, 13);
    if (mode >= SYSCTRL_CLK_PLL_DIV_1)
    {
        set_reg_bits(APB_SYSCTRL->CguCfg, mode, 4, 0);
        set_reg_bit(APB_SYSCTRL->CguCfg, 1, 28);
        set_reg_bits(AON1_BOOT, mode, 4, 14);
    }
}

void SYSCTRL_SelectUSBClk(SYSCTRL_ClkMode mode)
{
    set_reg_bits(&APB_SYSCTRL->USBCfg, mode, 4, 0);
    set_reg_bit(&APB_SYSCTRL->USBCfg, 1, 4);
}

void SYSCTRL_SelectFlashClk(SYSCTRL_ClkMode mode)
{
    if (mode >= SYSCTRL_CLK_PLL_DIV_1)
    {
        set_reg_bits(APB_SYSCTRL->CguCfg, mode, 4, 6);
        set_reg_bits(AON1_BOOT, mode, 4, 19);
    }
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, mode == 0 ? 0 : 1, 18);
    set_reg_bit(AON1_BOOT, mode == 0 ? 0 : 1, 18);
}

void SYSCTRL_SelectSlowClk(SYSCTRL_SlowClkMode mode)
{
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), mode, 8);
    volatile uint32_t *reg = (volatile uint32_t *)(APB_SYSCTRL_BASE + 0x1c8);
    if (mode == SYSCTRL_SLOW_RC_CLK)
    {
        while ((*reg & (1 << 2)) == 0);
        while ((*reg & (1 << 3)) != 0);
    }
    else
    {
        while ((*reg & (1 << 2)) != 0);
        while ((*reg & (1 << 3)) == 0);
    }
}

void SYSCTRL_SelectQDECClk(SYSCTRL_ClkMode mode, uint16_t div)
{
    set_reg_bit(&APB_SYSCTRL->QdecCfg, mode, 15);
    set_reg_bits(&APB_SYSCTRL->QdecCfg, div, 10, 1);
    set_reg_bit(&APB_SYSCTRL->QdecCfg, 1, 11);
}

void SYSCTRL_EnablePLL(uint8_t enable)
{
    set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x1A8), enable, 20);
    set_reg_bit(AON1_BOOT, enable, 1);
}

#define SLOW_RC_CFG0     ((volatile uint32_t *)(AON2_CTRL_BASE + 0x17c))
#define SLOW_RC_CFG1     ((volatile uint32_t *)(AON2_CTRL_BASE + 0x180))

void SYSCTRL_EnableSlowRC(uint8_t enable, SYSCTRL_SlowRCClkMode mode)
{
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0xc), enable, 1);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0xc), 1, 0);
    set_reg_bits(SLOW_RC_CFG0, mode, 5, 12);
}

uint32_t SYSCTRL_GetSlowClk(void)
{
    uint8_t mode = ((*(volatile uint32_t *)(AON1_CTRL_BASE + 0x10)) >> 8) & 1;
    if (mode)
    {
        return OSC_CLK_FREQ;
    }
    else
    {
        switch (((*SLOW_RC_CFG0) >> 12) & 0x1f)
        {
        case SYSCTRL_SLOW_RC_8M:
            return  8000000;
        case SYSCTRL_SLOW_RC_16M:
            return 16000000;
        case SYSCTRL_SLOW_RC_24M:
            return 24000000;
        case SYSCTRL_SLOW_RC_32M:
            return 32000000;
        case SYSCTRL_SLOW_RC_48M:
            return 48000000;
        default:
            return 64000000;
        }
    }
}

void SYSCTRL_SelectCLK32k(SYSCTRL_ClkMode mode)
{
    if (mode == SYSCTRL_CLK_32k)
    {
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 1, 12);
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 0, 11);
    }
    else
    {
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 1, 11);
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 0, 12);
        set_reg_bits(APB_SYSCTRL->CguCfg + 7, mode, 12, 20);
    }
}

int SYSCTRL_GetCLK32k(void)
{
    if (APB_SYSCTRL->CguCfg[1] & (1 << 12))
    {
        return RTC_CLK_FREQ;
    }
    else
        return SYSCTRL_GetSlowClk() / (APB_SYSCTRL->CguCfg[7] >> 20);
}

void SYSCTRL_SelectCPU32k(SYSCTRL_CPU32kMode mode)
{
    set_reg_bit(&APB_SYSCTRL->CguCfg8, mode, 13);
}

int SYSCTRL_GetCPU32k(void)
{
    return APB_SYSCTRL->CguCfg8 & (1 << 13) ? RTC_CLK_FREQ : SYSCTRL_GetCLK32k();
}

uint32_t SYSCTRL_AutoTuneSlowRC(void)
{
    uint8_t switch_slowclk = 0;
    uint8_t clk32 = (APB_SYSCTRL->CguCfg[1] >> 11) & 0x3;
    uint16_t clkdiv = APB_SYSCTRL->CguCfg[7] >> 20;

    if (*(volatile uint32_t *)(AON1_CTRL_BASE + 0x10) & (1 << 8))
    {
        // OK, RF OSC
    }
    else
    {
        switch_slowclk = 1;
        SYSCTRL_SelectSlowClk(SYSCTRL_SLOW_CLK_24M_RF);
    }

    switch (((*SLOW_RC_CFG0) >> 12) & 0x1f)
    {
    case SYSCTRL_SLOW_RC_8M:
        set_reg_bits(APB_SYSCTRL->CguCfg + 7, OSC_CLK_FREQ /  6400, 12, 20);
        break;
    case SYSCTRL_SLOW_RC_16M:
        set_reg_bits(APB_SYSCTRL->CguCfg + 7, OSC_CLK_FREQ / 12800, 12, 20);
        break;
    case SYSCTRL_SLOW_RC_24M:
        set_reg_bits(APB_SYSCTRL->CguCfg + 7, OSC_CLK_FREQ / 19200, 12, 20);
        break;
    case SYSCTRL_SLOW_RC_32M:
        set_reg_bits(APB_SYSCTRL->CguCfg + 7, OSC_CLK_FREQ / 25600, 12, 20);
        break;
    case SYSCTRL_SLOW_RC_48M:
        set_reg_bits(APB_SYSCTRL->CguCfg + 7, OSC_CLK_FREQ / 38400, 12, 20);
        break;
    default:
        set_reg_bits(APB_SYSCTRL->CguCfg + 7, OSC_CLK_FREQ / 51200, 12, 20);
        break;
    }

    set_reg_bits(APB_SYSCTRL->CguCfg + 1, 1, 2, 11);
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, 1, 10);

    set_reg_bits(SLOW_RC_CFG1, (0x10000u | 1250ul), 17, 0);

    while ((*SLOW_RC_CFG0 & (0x80000000u)) == 0) ;

    uint16_t r = (*SLOW_RC_CFG0 >> 19) & 0xfffu;
    SYSCTRL_TuneSlowRC(r);

    set_reg_bits(APB_SYSCTRL->CguCfg + 1, clk32, 2, 11);
    if (switch_slowclk)
        SYSCTRL_SelectSlowClk(SYSCTRL_SLOW_RC_CLK);
    set_reg_bits(APB_SYSCTRL->CguCfg + 7, clkdiv, 12, 20);

    return r;
}

void SYSCTRL_TuneSlowRC(uint32_t value)
{
    set_reg_bits(SLOW_RC_CFG0, value, 12, 0);
}

static int get_safe_divider(int reg_offset, int offset)
{
    int r = (*(APB_SYSCTRL->CguCfg + reg_offset) >> offset) & 0xf;
    return r == 0 ? 1 : r;
}

static int get_safe_divider6(int reg_offset, int offset)
{
    int r = (*(APB_SYSCTRL->CguCfg + reg_offset) >> offset) & 0x3f;
    return r == 0 ? 1 : r;
}

static int get_safe_divider10(int reg_offset, int offset)
{
    int r = (*(APB_SYSCTRL->CguCfg + reg_offset) >> offset) & 0x3ff;
    return r == 0 ? 1 : r;
}

uint32_t SYSCTRL_GetPLLClk()
{
    if (io_read(AON2_CTRL_BASE + 0x1A8) & (1ul << 20))
    {
        uint32_t div = ((APB_SYSCTRL->PllCtrl >> 1) & 0x3f) * ((APB_SYSCTRL->PllCtrl >> 15) & 0x3f);
        return SYSCTRL_GetSlowClk() / div * ((APB_SYSCTRL->PllCtrl >> 7) & 0xff);
    }
    else
        return 0;
}

int SYSCTRL_ConfigPLLClk(uint32_t div_pre, uint32_t loop, uint32_t div_output)
{
    uint32_t ref = SYSCTRL_GetSlowClk() / 1000000 / div_pre;
    if (ref < 2) return 1;
    uint32_t freq = ref * loop;
    uint32_t vco = 0;

    if ((460 <= freq) && (freq <= 600))
        vco = 7;
    else if ((380 <= freq) && (freq <= 520))
        vco = 6;
    else if ((300 <= freq) && (freq <= 440))
        vco = 5;
    else if ((220 <= freq) && (freq <= 360))
        vco = 4;
    else if ((160 <= freq) && (freq <= 280))
        vco = 3;
    else if ((100 <= freq) && (freq <= 200))
        vco = 2;
    else if ((60 <= freq) && (freq <= 120))
        vco = 1;
    else
        return 2;

    uint32_t t = APB_SYSCTRL->PllCtrl;
    t &= ~(0xfffffful);
    t |= (div_pre << 1) | (loop << 7) | (div_output << 15) | (vco << 21);
    APB_SYSCTRL->PllCtrl = t;
    io_write(AON2_CTRL_BASE + 0x1A8, io_read(AON2_CTRL_BASE + 0x1A8) | (1ul << 20));
    set_reg_bits(AON1_BOOT, loop, 6, 5);
    return 0;
}

uint32_t SYSCTRL_GetHClk()
{
    if (APB_SYSCTRL->CguCfg[1] & (1 << 14))
        return SYSCTRL_GetPLLClk() / get_safe_divider(0, 0);
    else
        return SYSCTRL_GetSlowClk();
}

void SYSCTRL_SetPClkDiv(uint8_t div)
{
    set_reg_bits(APB_SYSCTRL->CguCfg, div, 4, 4);
    set_reg_bit(APB_SYSCTRL->CguCfg, 1, 29);
}

uint32_t SYSCTRL_GetPClk()
{
    return SYSCTRL_GetHClk() / get_safe_divider(0, 4);
}

uint32_t SYSCTRL_GetClk(SYSCTRL_Item item)
{
    switch (item)
    {
    case SYSCTRL_ITEM_APB_TMR0:
    case SYSCTRL_ITEM_APB_TMR1:
    case SYSCTRL_ITEM_APB_TMR2:
        if (APB_SYSCTRL->CguCfg[1] & (1 << (15 + item - SYSCTRL_ITEM_APB_TMR0)))
            return SYSCTRL_GetCLK32k();
        else
            return SYSCTRL_GetSlowClk() / get_safe_divider(11, 20);
    case SYSCTRL_ITEM_APB_PDM:
        return SYSCTRL_GetSlowClk() * get_safe_divider6(22, 6) / get_safe_divider6(22, 0);
    case SYSCTRL_ITEM_APB_PWM:
        if (APB_SYSCTRL->CguCfg8 & (1 << 15))
            return SYSCTRL_GetCLK32k();
        else
            return SYSCTRL_GetSlowClk() / get_safe_divider(11, 27);
    case SYSCTRL_ITEM_APB_KeyScan:
        if (APB_SYSCTRL->CguCfg[1] & (1 << 13))
            return SYSCTRL_GetCLK32k();
        else
            return SYSCTRL_GetSlowClk() / get_safe_divider(0, 24);
    case SYSCTRL_ITEM_AHB_SPI0:
        if (APB_SYSCTRL->CguCfg[1] & (1 << 21))
            return SYSCTRL_GetPLLClk() / get_safe_divider(0, 20);
        else
            return SYSCTRL_GetSlowClk();
    case SYSCTRL_ITEM_APB_SPI1:
        if (APB_SYSCTRL->CguCfg[1] & (1 << 22))
            return SYSCTRL_GetHClk();
        else
            return SYSCTRL_GetSlowClk();
    case SYSCTRL_ITEM_APB_I2S:
        if (APB_SYSCTRL->CguCfg[1] & (1 << 23))
            return SYSCTRL_GetPLLClk() / get_safe_divider(0, 12);
        else
            return SYSCTRL_GetSlowClk();
    case SYSCTRL_ITEM_APB_UART0:
    case SYSCTRL_ITEM_APB_UART1:
        if (APB_SYSCTRL->CguCfg[1] & (1 << (19 + item - SYSCTRL_ITEM_APB_UART0)))
            return SYSCTRL_GetHClk();
        else
            return SYSCTRL_GetSlowClk();
    case SYSCTRL_ITEM_APB_IR:
        if (APB_SYSCTRL->CguCfg8 & (1 << 16))
            return SYSCTRL_GetAdcClkDiv();
        else
            return SYSCTRL_GetSlowClk();
    case SYSCTRL_ITEM_APB_ADC:
        if (APB_SYSCTRL->CguCfg8 & (1 << 17))
            return SYSCTRL_GetAdcClkDiv();
        else
            return SYSCTRL_GetSlowClk();
    case SYSCTRL_ITEM_APB_EFUSE:
        if (APB_SYSCTRL->CguCfg8 & (1 << 19))
            return SYSCTRL_GetAdcClkDiv();
        else
            return SYSCTRL_GetSlowClk();
    case SYSCTRL_ITEM_APB_USB:
        return SYSCTRL_GetPLLClk() / get_safe_divider(0x20, 0);
    case SYSCTRL_ITEM_APB_QDEC:
        if (APB_SYSCTRL->QdecCfg & (1 << 15))
            return SYSCTRL_GetHClk() / get_safe_divider10(21, 1);
        else
            return SYSCTRL_GetSlowClk() / get_safe_divider10(21, 1);
    case SYSCTRL_ITEM_APB_I2C0:
    case SYSCTRL_ITEM_APB_I2C1:
        // generally, this clock is not needed in programming I2C
        return SYSCTRL_GetPClk();
    default:
        // TODO
        return SYSCTRL_GetSlowClk();
    }
}

void SYSCTRL_SetAdcClkDiv(uint8_t denom)
{
    APB_SYSCTRL->CguCfg8 &= ~0x1fff;
    APB_SYSCTRL->CguCfg8 |= 0x1000 | (denom & 0x2f);
}

uint32_t SYSCTRL_GetAdcClkDiv(void)
{
    uint16_t denom = APB_SYSCTRL->CguCfg8 & 0x2f;
    uint16_t num = (APB_SYSCTRL->CguCfg8 >> 6) & 0x2f;
    return SYSCTRL_GetSlowClk() * num / denom;
}

void SYSCTRL_SelectTypeAClk(SYSCTRL_Item item, SYSCTRL_ClkMode mode)
{
    switch (item)
    {
    case SYSCTRL_ITEM_APB_PDM:
        set_reg_bit(&APB_SYSCTRL->CguCfg8, mode, 14);
        break;
    case SYSCTRL_ITEM_APB_PWM:
        set_reg_bit(&APB_SYSCTRL->CguCfg8, mode, 15);
        break;
    case SYSCTRL_ITEM_APB_IR:
        set_reg_bit(&APB_SYSCTRL->CguCfg8, mode, 16);
        break;
    case SYSCTRL_ITEM_APB_ADC:
        set_reg_bit(&APB_SYSCTRL->CguCfg8, mode, 17);
        break;
    case SYSCTRL_ITEM_APB_EFUSE:
        set_reg_bit(&APB_SYSCTRL->CguCfg8, mode, 19);
        break;
    default:
        break;
    }
}

int SYSCTRL_SelectUsedDmaItems(uint32_t items)
{
    int i = 0;
    uint32_t value = 0;
    int cnt = 0;
    for (i = 0; i <= SYSCTRL_DMA_ADC; i++)
    {
        if (items & (1ul << i))
        {
            if (cnt >= 8) return -1;
            value <<= 4;
            value |= i;
            cnt++;
        }
    }
    APB_SYSCTRL->DmaCtrl[0] = value;

    value = 0;
    cnt = 0;
    for (i = SYSCTRL_DMA_UART0_TX; i <= SYSCTRL_DMA_LAST; i++)
    {
        if (items & (1ul << i))
        {
            if (cnt >= 8) return -2;
            value <<= 4;
            value |= i - SYSCTRL_DMA_UART0_TX;
            cnt++;
        }
    }
    APB_SYSCTRL->DmaCtrl[1] = value;

    return 0;
}

int SYSCTRL_GetDmaId(SYSCTRL_DMA item)
{
    int offset = 0;
    uint32_t value = APB_SYSCTRL->DmaCtrl[0];
    int i;
    if (item >= SYSCTRL_DMA_UART0_TX)
    {
        offset = 8;
        item -= SYSCTRL_DMA_UART0_TX;
        value = APB_SYSCTRL->DmaCtrl[1];
    }
    for (i = 0; i <= 28; i += 4)
    {
        if ((value & 0xf) == item)
            return offset + i / 4;
        value >>= 4;
    }
    return -1;
}

void SYSCTRL_ConfigBOR(int threshold, int enable_active, int enable_sleep)
{
    uint8_t enable = enable_active || enable_sleep;

    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x8), threshold, 4, 0);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), enable, 17);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), enable ^ 0x1, 5);
}

void SYSCTRL_SetLDOOutputFlash(int level)
{
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x8), level & 0xf, 4, 11);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x8), 1, 31);
}

void SYSCTRL_SetLDOOutput(int level)
{
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x0), level & 0xf, 4, 1);
}

void SYSCTRL_WaitForLDO(void)
{
}

void SYSCTRL_USBPhyConfig(uint8_t enable, uint8_t pull_sel)
{
    if(enable)
    {
        io_write(AON2_CTRL_BASE + 0x174, 1ul | (1 << pull_sel) | (4 << 4));
    }
    else
    {
        io_write(AON2_CTRL_BASE + 0x174, 0);
    }
}

void SYSCTRL_ResetAllBlocks(void)
{
    APB_SYSCTRL->RstuCfg[1] &= 0x23;
}

#endif
