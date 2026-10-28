#include "ingsoc.h"
#include "peripheral_sysctrl.h"
#include "peripheral_timer.h"

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

void SYSCTRL_SetLDOOutput(SYSCTRL_LDOOutputCore level)
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

void SYSCTRL_SelectMemoryBlocks(uint32_t block_map)
{
    #define RTC_MEM1 (APB_RTC_BASE + 0x78)
    #define RTC_MEM2 (APB_RTC_BASE + 0x8c)
    uint32_t mask = ~((uint32_t)0x3ff << 22);
    uint32_t shutdown = ((~block_map) & 0x3ff) << 22;
    io_write(RTC_MEM1, (io_read(RTC_MEM1) & mask) | shutdown);
    io_write(RTC_MEM2, (io_read(RTC_MEM2) & mask) | shutdown);
}

uint8_t SYSCTRL_GetLastWakeupSource(SYSCTRL_WakeupSource_t *source)
{
    int i = 0;
    while (((io_read(BB_REG_BASE + 0x58) & 0x20) == 0) && (i < 1000)) i++;
    source->source = (io_read(BB_REG_BASE + 0x304) >> 8) & 0x3;
    return source->source != 0;
}

int SYSCTRL_Init(void)
{
    return 0;
}

void SYSCTRL_PAEnable(void)
{
    io_write(0x40090000, (io_read(0x40090000) & (~(0x3FF<<16))) | (70 << 16));
    io_write(0x4007005c, 0x82);
    io_write(0x40070044,  io_read(0x40070044) | (0xf<<8) | (0xf<<24));
    io_write(0x40090064, 0x400);
    io_write(0x40090000, (io_read(0x40090000) & (~(0x3FF<<16))) | (70 << 16)); // adjust_rf_txen_rxen_duty
}

void SYSCTRL_Reset(void)
{
    SYSCTRL_ClearClkGate(SYSCTRL_ClkGate_APB_WDT);
    // watchdog may be locked, so unlock it by disabling it.
    TMR_WatchDogDisable();
    TMR_WatchDogEnable(1);
    while(1);
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#include "eflash.h"

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
        set_reg_bit(APB_SYSCTRL->CguCfg + 2, v, 12);
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
        if (items & (1u << i))
            SYSCTRL_SetClkGate(i);
}

void SYSCTRL_ClearClkGateMulti(uint32_t items)
{
    SYSCTRL_Item i;
    for (i = (SYSCTRL_Item)0; i < SYSCTRL_ITEM_NUMBER; i++)
        if (items & (1u << i))
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
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 1, 31);
    }
}

void SYSCTRL_SelectPDMClk(SYSCTRL_ClkMode mode)
{
    set_reg_bits(&APB_SYSCTRL->CguCfg8, (1 << 6) | (mode & 0x3f), 12, 0);
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
        else
        {
            set_reg_bits(APB_SYSCTRL->CguCfg, 1, 4, 20);
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

void SYSCTRL_SelectHClk(SYSCTRL_ClkMode mode)
{
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, mode == 0 ? 0 : 1, 14);
    if (mode >= SYSCTRL_CLK_PLL_DIV_1)
    {
        set_reg_bits(APB_SYSCTRL->CguCfg, mode, 4, 0);
        set_reg_bit(APB_SYSCTRL->CguCfg, 1, 28);
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
        set_reg_bits(APB_SYSCTRL->CguCfg, mode, 4, 16);
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 1, 29);
    }
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, mode == 0 ? 0 : 1, 18);
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
}

#define SLOW_RC_CFG0     ((volatile uint32_t *)(AON2_CTRL_BASE + 0x17c))
#define SLOW_RC_CFG1     ((volatile uint32_t *)(AON2_CTRL_BASE + 0x180))

void SYSCTRL_EnableSlowRC(uint8_t enable, SYSCTRL_SlowRCClkMode mode)
{
    if (enable)
    {
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0xc), 0, 0);
        set_reg_bits(SLOW_RC_CFG0, mode, 5, 12);
    }
    else
    {
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0xc), 0, 1);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0xc), 1, 0);
    }
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
    }
    else
    {
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 1, 11);
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 0, 12);
        set_reg_bits(APB_SYSCTRL->CguCfg + 7, mode, 12, 20);
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 1, 10);
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

    set_reg_bit(SLOW_RC_CFG1, 0, 16);

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

void SYSCTRL_EnableClockOutput(uint8_t enable, uint16_t denom)
{
    if (enable)
    {
        io_write(APB_SYSCTRL_BASE + 0x1e0, (denom & 0x3ff) | (3 << 10));
    }
    else
    {
        io_write(APB_SYSCTRL_BASE + 0x1e0, 1);
    }
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
        return (uint32_t)((uint64_t)SYSCTRL_GetSlowClk() * ((APB_SYSCTRL->PllCtrl >> 7) & 0xff) / div);
    }
    else
        return 0;
}

uint32_t SYSCTRL_GetFlashClk(void)
{
    if (APB_SYSCTRL->CguCfg[1] & (0x1 << 18))
    {
        return SYSCTRL_GetPLLClk() / get_safe_divider(0, 16);
    }
    else
        return SYSCTRL_GetSlowClk();
}

int SYSCTRL_ConfigPLLClk(uint32_t div_pre, uint32_t loop, uint32_t div_output)
{
    uint32_t ref = SYSCTRL_GetSlowClk() / 1000000 / div_pre;
    if (ref < 2) return 1;
    uint32_t freq = (uint64_t)SYSCTRL_GetSlowClk() * loop / 1000000 / div_pre;
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
    return 0;
}

#define AON1_BOOT   (volatile uint32_t *)(AON1_CTRL_BASE + 0x14)

void SYSCTRL_EnableConfigClocksAfterWakeup(uint8_t enable_pll, uint8_t pll_loop,
        SYSCTRL_ClkMode hclk,
        SYSCTRL_ClkMode flash_clk,
        uint8_t enable_watchdog)
{
    *AON1_BOOT =(0x1 << 0 ) |
                (enable_pll << 1 ) |
                (0x1 << 2 ) |
                (0x0 << 3 ) |
                (((uint32_t)pll_loop)  << 5 ) |
                (hclk > 0 ? 0x1 << 13 : 0) |
                (((uint32_t)hclk)   << 14) |
                (flash_clk > 0 ? 0x1 << 18 : 0) |
                (((uint32_t)flash_clk)   << 19) |
                (0x1 << 23) |
                (0x2 << 24) |
                (0x1 << 27) |
                (enable_watchdog << 28) |
                (*AON1_BOOT & (7UL << 29));
}

void SYSCTRL_DisableConfigClocksAfterWakeup(void)
{
    *AON1_BOOT &= ~0x1;
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

uint8_t SYSCTRL_GetPClkDiv(void)
{
    return get_safe_divider(0, 4);
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
    APB_SYSCTRL->CguCfg8 |= 0x1000 | 0x40 | (denom & 0x3f);
}

uint32_t SYSCTRL_GetAdcClkDiv(void)
{
    uint16_t denom = APB_SYSCTRL->CguCfg8 & 0x3f;
    uint16_t num = (APB_SYSCTRL->CguCfg8 >> 6) & 0x3f;
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

    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 0x1, 16);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 0x0, 17);
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), 0x3, 2, 4);

    if (0 == enable) return;

    if (SYSCTRL_BOR_1V5 == threshold)
    {
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 0x0, 16);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), 0x0, 4);
    }
    else
    {
        set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x8), threshold, 4, 0);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 1, 17);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), 0x0, 5);
    }
}

void SYSCTRL_EnablePVDInt(uint8_t enable, uint8_t polarity, uint8_t level)
{
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 0, 17);
    if (0 == enable) return;
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x8), level, 4, 0);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), 0x1, 5);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 1, 17);
    set_reg_bit(&APB_SYSCTRL->AnaCtrl, polarity, 2);
    APB_SYSCTRL->AnaCtrl |= (1ul << 4);
}

void SYSCTRL_ClearPVDInt(void)
{
    APB_SYSCTRL->AnaCtrl |= (1ul << 11);
}

void SYSCTRL_EnablePDRInt(uint8_t enable)
{
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 1, 16);
    if (0 == enable) return;
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), 0x1, 4);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 0, 16);
    APB_SYSCTRL->AnaCtrl |= (1ul << 3);
}

void SYSCTRL_ClearPDRInt(void)
{
    APB_SYSCTRL->AnaCtrl |= (1ul << 10);
}

void SYSCTRL_SetLDOOutputFlash(SYSCTRL_LDOOutputFlash level)
{
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x8), level & 0xf, 4, 11);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x8), 1, 31);
}

void SYSCTRL_SetLDOOutput(SYSCTRL_LDOOutputCore level)
{
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x0), level & 0xf, 4, 1);
}

void SYSCTRL_SetBuckDCDCOutput(SYSCTRL_BuckDCDCOutput level)
{
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x8), level & 0xf, 4, 27);
}

void SYSCTRL_EnableBuckDCDC(uint8_t enable)
{
    set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x0), enable & 1, 0);
}

void SYSCTRL_SetLDOOutputRF(SYSCTRL_LDOOutputRF level)
{
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0xc), level & 0x1f, 5, 6);
}

void SYSCTRL_SetAdcVrefOutput(SYSCTRL_AdcVrefOutput level)
{
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0xc), level & 0x1f, 5, 11);
}

void SYSCTRL_WaitForLDO(void)
{
}

void SYSCTRL_USBPhyConfig(uint8_t enable, uint8_t pull_sel)
{
    if(enable)
    {
        io_write(AON2_CTRL_BASE + 0x174, 1ul | (1 << pull_sel) | (7 << 4));
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

void SYSCTRL_EnableWakeupSourceDetection(void)
{
    set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x1A8), 1, 16);
}

uint8_t SYSCTRL_GetLastWakeupSource(SYSCTRL_WakeupSource_t *source)
{
    source->other = 0;
    source->gpio = APB_SYSCTRL->SysIoWkSource;
    uint32_t a = APB_SYSCTRL->SysIoStatus >> 16;
    if ((source->gpio == 0) && (a == 0))
    {
        a = io_read(AON2_CTRL_BASE + 0x140);
        switch (a & 3)
        {
        case 0:
            return 0;
        case 1:
            source->other |= SYSCTRL_WAKEUP_SOURCE_AUTO;
            break;
        case 3:
            source->other |= SYSCTRL_WAKEUP_SOURCE_AUTO; // fall through
        case 2:
            a >>= 2;
            source->gpio = a & 1;
            source->gpio |= (a & (0x3 << 1)) << (5 - 1);
            source->gpio |= (a & (0x7 << 3)) << (21 - 3);
            source->gpio |= ((uint64_t)(a & (0x3 << 5))) << (36 - 5);
            break;
        default:
            break;
        }

        return 1;
    }

    source->gpio |= ((uint64_t)(a & 0x3f)) << 32;
    if (a & (1 << (6 + 0)))
        source->other |= SYSCTRL_WAKEUP_SOURCE_RTC_ALARM;
    if (a & (1 << (6 + 1)))
        source->other |= SYSCTRL_WAKEUP_SOURCE_AUTO;
    if (a & (1 << (6 + 2)))
        source->other |= SYSCTRL_WAKEUP_SOURCE_COMPARATOR;

    return 1;
}

void SYSCTRL_EnablePcapMode(const uint8_t channel_index, uint8_t enable)
{
    set_reg_bit((volatile uint32_t *)(APB_SYSCTRL_BASE + 0x74), enable & 0x1, channel_index);
}

void SYSCTRL_SelectMemoryBlocks(uint32_t block_map)
{
    uint32_t masked = block_map & 0x1f;
    set_reg_bits((volatile uint32_t *)(AON2_CTRL_BASE + 0x04), (~masked) & 0x1f, 5, 0);
    set_reg_bits((volatile uint32_t *)(AON2_CTRL_BASE + 0x14), masked, 5, 16);
    set_reg_bits((volatile uint32_t *)(AON2_CTRL_BASE + 0x14), masked, 5, 8);
}

static void SYSCTRL_CacheFlush(const uintptr_t base)
{
    *(volatile uint32_t *)(base + 0x58) =  (1UL<<31) | 0x4;
}

static void SYSCTRL_CacheControl0(const uintptr_t base, SYSCTRL_CacheMemCtrl ctrl, uint8_t bit_offset)
{
    if (SYSCTRL_MEM_BLOCK_AS_CACHE != ctrl)
        set_reg_bit((volatile uint32_t *)(base), 0, 1);

    set_reg_bits(&APB_SYSCTRL->SysCtrl, ctrl, 1, bit_offset);

    if (SYSCTRL_MEM_BLOCK_AS_CACHE == ctrl)
    {
        SYSCTRL_CacheFlush(base);
        set_reg_bit((volatile uint32_t *)(base), 1, 1);
    }
}

void SYSCTRL_DCacheControl(SYSCTRL_CacheMemCtrl d_cache)
{
    #define DC_BASE 0x40141000

    SYSCTRL_CacheControl0(DC_BASE, d_cache, 0);
}

void SYSCTRL_ICacheControl(SYSCTRL_CacheMemCtrl i_cache)
{
    #define IC_BASE 0x40140000

    SYSCTRL_CacheControl0(IC_BASE, i_cache, 1);
}

void SYSCTRL_ICacheFlush(void)
{
    SYSCTRL_CacheFlush(IC_BASE);
}

void SYSCTRL_CacheControl(SYSCTRL_CacheMemCtrl i_cache, SYSCTRL_CacheMemCtrl d_cache)
{
    SYSCTRL_ICacheControl(i_cache);
    SYSCTRL_DCacheControl(d_cache);
}

__attribute__((weak)) const factory_calib_data_t *flash_get_factory_calib_data(void)
{
    // add `eflash.c` to the project!
    return 0;
}

int SYSCTRL_Init(void)
{
    uint32_t i;
    const factory_calib_data_t *p = flash_get_factory_calib_data();
    if (!p) return 1;

    if(p->band_gap<0x7f)
        set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x8), p->band_gap, 7, 4);
    set_reg_bits(APB_SYSCTRL->CguCfg + 7, 750, 12, 20);

    for (i = 0; i < sizeof(p->vaon) / sizeof(p->vaon[0]); i++)
    {
        if (p->vaon[i] >= 1300)
            return 4;
        if (p->vaon[i] >= 1190)
        {
            set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x0), i, 4, 28);
            break;
        }
    }

    if (i >= sizeof(p->vaon) / sizeof(p->vaon[0])) return 2;

    for (i = 1; i < sizeof(p->vcore) / sizeof(p->vcore[0]); i++)
    {
        if(p->vcore[i] > 3100)
            return 5;
        if (p->vcore[i] > 1180)
        {
            SYSCTRL_SetLDOOutput((SYSCTRL_LDOOutputCore)(i - 1));
            break;
        }
    }

    if (i >= sizeof(p->vcore) / sizeof(p->vcore[0])) return 3;

    return 0;
}

void SYSCTRL_Reset(void)
{
    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_WDT);
    TMR_WatchDogEnable3(WDT_INTTIME_INTERVAL_2MS, WDT_RSTTIME_INTERVAL_4MS, 0);
    while(1);
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
static volatile uint32_t g_slow_rc_clk;
#define AON1_REG5       ((volatile uint32_t *)(AON1_CTRL_BASE + 0x18))

static void set_reg_bits(volatile uint32_t *reg, uint32_t v, uint8_t bit_width, uint8_t bit_offset)
{
    uint32_t mask = ((1 << bit_width) - 1) << bit_offset;
    v &=(1<<bit_width)-1;
    *reg = (*reg & ~mask) | (v << bit_offset);
}

static void set_reg_bit(volatile uint32_t *reg, uint8_t v, uint8_t bit_offset)
{
    if (v)
        *reg |= 1 << bit_offset;
    else
        *reg &= ~(1 << bit_offset);
}

static uint32_t get_safe_divider(uint32_t reg, int bit_offset, int bit_width)
{
    uint32_t v = reg >> bit_offset;
    uint32_t mask = (1 << bit_width) - 1;
    v &= mask;
    return v > 0 ? v : 1;
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
    case SYSCTRL_ITEM_APB_WDT       :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 1);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 17);
        break;
    case SYSCTRL_ITEM_APB_PWM       :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 5);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 10);
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
    case SYSCTRL_ITEM_APB_DMA       :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 12);
        set_reg_bit(APB_SYSCTRL->CguCfg + 2, v, 0);
        break;
    case SYSCTRL_ITEM_AHB_SPI0      :
        set_reg_bit(APB_SYSCTRL->CguCfg + 2, v, 12);
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
    case SYSCTRL_ITEM_APB_SysCtrl   :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 0);
        break;
    case SYSCTRL_ITEM_APB_PinCtrl   :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 6);
        break;
    case SYSCTRL_ITEM_APB_USB:
        set_reg_bit(&APB_SYSCTRL->USBCfg, v, 5);
        set_reg_bit(&APB_SYSCTRL->USBCfg, v, 7);
        break;
    case SYSCTRL_ITEM_APB_ASDM       :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 28);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 21);
        break;
    case SYSCTRL_ITEM_APB_RTIMER0    :
    case SYSCTRL_ITEM_APB_RTIMER1    :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 4);
        set_reg_bit(APB_SYSCTRL->CguCfg + 5, v, 2);
        break;
    case SYSCTRL_ITEM_APB_PTE        :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 26);
        break;
    case SYSCTRL_ITEM_APB_GPIOTE     :
        set_reg_bit(APB_SYSCTRL->CguCfg + 3, v, 27);
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
        if (items & (1u << i))
            SYSCTRL_SetClkGate(i);
}

int SYSCTRL_SelectUsedDmaItems(uint32_t items)
{
    int i = 0;
    uint32_t value = 0;
    int cnt = 0;
    for (i = 0; i <= SYSCTRL_DMA_ASDM_RX; i++)
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
    for (i = SYSCTRL_DMA_UART0_TX; i < SYSCTRL_DMA_LAST; i++)
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

    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x38), 0x0, 2, 12);
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), 0x3, 2, 4);

    if (0 == enable) return;

    if (threshold > SYSCTRL_PDR_END)
    {
        threshold -= SYSCTRL_PDR_END + 1;
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x38), 0x1, 12);
        set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x30),  threshold, 3, 18);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), 0x0, 5);
    }
    else
    {
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x38), 0x1, 13);
        set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x30), threshold, 3, 24);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), 0x0, 4);
    }
}

void SYSCTRL_EnablePVDInt(uint8_t enable, uint8_t polarity, uint8_t level)
{
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x38), 0x0, 12);
    if (0 == enable) return;
    level -= SYSCTRL_PDR_END + 1;
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x30), level, 3, 18);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), 0x1, 5);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x38), 0x1, 12);
    set_reg_bit(&APB_SYSCTRL->AnaCtrl, polarity, 2);
    APB_SYSCTRL->AnaCtrl |= (1ul << 4);
}

void SYSCTRL_ClearPVDInt(void)
{
    APB_SYSCTRL->AnaCtrl |= (1ul << 11);
}

void SYSCTRL_EnablePDRInt(uint8_t enable, uint8_t level)
{
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x38), 0x0, 13);
    if (0 == enable) return;
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x30), level, 3, 24);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), 0x1, 4);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x38), 0x1, 13);
    APB_SYSCTRL->AnaCtrl |= (1ul << 3);
}

void SYSCTRL_ClearPDRInt(void)
{
    APB_SYSCTRL->AnaCtrl |= (1ul << 10);
}

void SYSCTRL_ClearClkGateMulti(uint32_t items)
{
    SYSCTRL_Item i;
    for (i = (SYSCTRL_Item)0; i < SYSCTRL_ITEM_NUMBER; i++)
        if (items & (1u << i))
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
    case SYSCTRL_ITEM_APB_WDT       :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 11);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 18);
        break;
    case SYSCTRL_ITEM_APB_PWM       :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 15);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 12);
        break;
    case SYSCTRL_ITEM_APB_QDEC      :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 19);
        set_reg_bit(&APB_SYSCTRL->QdecCfg, v, 14);
        break;
    case SYSCTRL_ITEM_APB_KeyScan   :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 20);
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
    case SYSCTRL_ITEM_APB_SysCtrl   :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 10);
        break;
    case SYSCTRL_ITEM_APB_PinCtrl   :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 16);
        break;
    case SYSCTRL_ITEM_APB_ASDM  :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 21);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 27);
        break;
    case SYSCTRL_ITEM_APB_RTIMER0   :
    case SYSCTRL_ITEM_APB_RTIMER1   :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 0, v, 14);
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 4);
        break;
    case SYSCTRL_ITEM_APB_PTE      :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 28);
        break;
    case SYSCTRL_ITEM_APB_GPIOTE   :
        set_reg_bit(APB_SYSCTRL->RstuCfg + 1, v, 29);
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

void SYSCTRL_ResetAllBlocks(void)
{
    APB_SYSCTRL->RstuCfg[1] &= 0x21;
}

void SYSCTRL_EnablePcapMode(const uint8_t channel_index, uint8_t enable)
{
    set_reg_bit((volatile uint32_t *)(APB_SYSCTRL_BASE + 0x70), enable & 0x1, channel_index);
}

void SYSCTRL_SelectQDECClk(SYSCTRL_ClkMode mode, uint16_t div)
{
    set_reg_bit(&APB_SYSCTRL->QdecCfg, mode&1, 15);
    set_reg_bits(&APB_SYSCTRL->QdecCfg, div, 10, 1);
    set_reg_bit(&APB_SYSCTRL->QdecCfg, 1, 11);
}

uint32_t SYSCTRL_GetSlowClk(void)
{
    uint8_t mode = (*(volatile uint32_t *)(AON1_CTRL_BASE + 0x10) >> 8) & 1;
    if (mode)
    {
        return OSC_CLK_FREQ;
    }
    return g_slow_rc_clk;
}

uint32_t SYSCTRL_AutoTuneSlowRC(SYSCTRL_SlowRCClkMode value)
{
    uint32_t tune;
    uint32_t pll_reg_vale = *(volatile uint32_t *)(AON1_CTRL_BASE + 0x28);
    uint32_t reg5 = *AON1_REG5;
    uint32_t aon1_int = *(volatile uint32_t *)(AON1_CTRL_BASE + 0x1c);
    uint32_t clk_src = (*(volatile uint32_t *)(AON1_CTRL_BASE + 0x10)) & 0x100;

    g_slow_rc_clk = (uint32_t)value * 8000000;

    SYSCTRL_SelectHClk(SYSCTRL_CLK_SLOW);
    SYSCTRL_SelectFlashClk(SYSCTRL_CLK_SLOW);
    set_reg_bit((volatile uint32_t*)(AON1_CTRL_BASE + 0x10), 1, 8);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x28), 1, 0);
    while ((APB_SYSCTRL->PllCtrl & (1u << 30)) == 0);
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x28), 6 | ((value*4)<<6) | (1<<14), 20, 1);
    *(volatile uint32_t*)(APB_SYSCTRL_BASE + 0x1e0) = 0x271 | (0x7<<10);
    set_reg_bits((volatile uint32_t*)(APB_SYSCTRL_BASE + 0x230), 0x4e2, 16, 12);
    set_reg_bit((volatile uint32_t*)(AON1_CTRL_BASE + 0x3c), 1, 19);
    set_reg_bit((volatile uint32_t*)(AON1_CTRL_BASE + 0x38), 0, 19);
    set_reg_bit((volatile uint32_t*)(APB_SYSCTRL_BASE + 0x230), 1, 30);
    while ((((*(volatile uint32_t*)(APB_SYSCTRL_BASE + 0x230))>>28)&0x1) == 0);
    tune = (*(volatile uint32_t*)(APB_SYSCTRL_BASE + 0x230)) & 0xfff;
    set_reg_bit((volatile uint32_t*)(APB_SYSCTRL_BASE + 0x230), 0, 30);
    set_reg_bits((volatile uint32_t*)(AON1_CTRL_BASE + 0x3c), tune, 12, 4);
    set_reg_bit((volatile uint32_t*)(AON1_CTRL_BASE + 0x38), 1, 19);
    set_reg_bit((volatile uint32_t*)(AON1_CTRL_BASE + 0x3c), 0, 19);
    *(volatile uint32_t *)(AON1_CTRL_BASE + 0x28) = pll_reg_vale;
    *AON1_REG5 = reg5;
    *(volatile uint32_t *)(AON1_CTRL_BASE + 0x1c) = aon1_int;
    *(volatile uint32_t *)(AON1_CTRL_BASE + 0x10) |= clk_src;
    return tune;
}

void SYSCTRL_TuneSlowRC(uint32_t tune)
{
    set_reg_bits((volatile uint32_t*)(AON1_CTRL_BASE + 0x3c), tune, 12, 4);
}

void SYSCTRL_SelectSpiClkDiv(spi_port_t port, SYSCTRL_ClkMode mode, uint8_t div)
{
    switch (port)
    {
    case SPI_PORT_0:
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, mode == 0 ? 0 : 1, 21);
        set_reg_bits(APB_SYSCTRL->CguCfg, div, 4, 20);
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 1, 30);
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

void SYSCTRL_SelectTimerClk(timer_port_t port, uint8_t div, pre_clk_source_t source)
{
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, (source == SOURCE_32K_CLK) ? 1 : 0, 15 + port * 2);
    if (source == SOURCE_32K_CLK)
        return;
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, (source == SOURCE_SLOW_CLK) ? 0 : 1, 16);
    if (div)
    {
        set_reg_bits(&APB_SYSCTRL->CguCfg8, div&0xf, 4, 20);
        set_reg_bit(&APB_SYSCTRL->CguCfg8, 1, 24);
    }
}

void SYSCTRL_SelectPWMClk(uint8_t div, pre_clk_source_t source)
{
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, (source == SOURCE_32K_CLK) ? 1 : 0, 23);
    if (source == SOURCE_32K_CLK)
        return;
    set_reg_bit(&APB_SYSCTRL->CguCfg8, (source == SOURCE_SLOW_CLK) ? 0 : 1, 15);
    if (div)
    {
        set_reg_bits(&APB_SYSCTRL->CguCfg8, div&0xf, 4, 27);
        set_reg_bit(&APB_SYSCTRL->CguCfg8, 1, 31);
    }
}

int SYSCTRL_ConfigPLLClk(uint32_t div_pre, uint32_t loop, uint32_t div_output)
{
    div_pre     &= 0x3f;
    loop        &= 0xff;
    div_output  &= 0x3f;

    uint32_t ref = SYSCTRL_GetSlowClk() / 1000000 / div_pre;
    if (ref < 2) return 1;
    uint32_t freq = (uint64_t)SYSCTRL_GetSlowClk() * loop / 1000000 / div_pre;
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

    const uint32_t reg = AON1_CTRL_BASE + 0x28;
    uint32_t t = io_read(reg);
    t &= ~(0xfffffeul);
    t |= (div_pre << 1) | (loop << 7) | (div_output << 15) | (vco << 21);
    io_write(reg, t);
    return 0;
}

void SYSCTRL_EnablePLL(uint8_t enable)
{

    uint8_t enabled = io_read(AON1_CTRL_BASE + 0x28) & 1;
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x28), enable, 0);
    if ((0 == enabled) && enable)
        while ((APB_SYSCTRL->PllCtrl & (1u << 30)) == 0);
}

uint32_t SYSCTRL_GetPLLClk()
{
    uint32_t pll_ctrl = io_read(AON1_CTRL_BASE + 0x28);
    if (pll_ctrl & 1ul)
    {
        uint32_t div = ((pll_ctrl >> 1) & 0x3f) * ((pll_ctrl >> 15) & 0x3f);
        return (uint32_t)((uint64_t)SYSCTRL_GetSlowClk() * ((pll_ctrl >> 7) & 0xff) / div);
    }

    return 0;
}

void SYSCTRL_EnableSlowRC(uint8_t enable, SYSCTRL_SlowRCClkMode mode)
{
    #define AON1_PMU3       ((volatile uint32_t *)(AON1_CTRL_BASE + 0x3c))

    if (enable)
    {
        SYSCTRL_AutoTuneSlowRC(mode);
        set_reg_bit(AON1_PMU3, 1, 19);
    }
    else
    {
        set_reg_bit(AON1_PMU3, 0, 19);
    }
}

static void enabled_div_hclk_update_act(uint8_t enable)
{
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), enable, 13);
}

static void enabled_div_flash_update_act(uint8_t enable)
{
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), enable, 14);
}

void SYSCTRL_SelectHClk(SYSCTRL_ClkMode mode)
{
    if (mode >= SYSCTRL_CLK_PLL_DIV_1)
    {
        set_reg_bits(AON1_REG5, mode, 4, 20);
        set_reg_bit(AON1_REG5, 1, 28);
        enabled_div_hclk_update_act(1);
    }
    else
    {
        enabled_div_hclk_update_act(0);
    }

    set_reg_bit(AON1_REG5, mode == 0 ? 0 : 1, 30);
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

void SYSCTRL_SelectFlashClk(SYSCTRL_ClkMode mode)
{
    if (mode >= SYSCTRL_CLK_PLL_DIV_1)
    {
        set_reg_bits(AON1_REG5, mode, 4, 24);
        set_reg_bit(AON1_REG5, 1, 29);
        enabled_div_flash_update_act(1);
    }
    else
    {
        enabled_div_flash_update_act(0);
    }
    set_reg_bit(AON1_REG5, mode == 0 ? 0 : 1, 31);
}

uint8_t SYSCTRL_GetPClkDiv(void)
{
    return get_safe_divider((uint32_t)&APB_SYSCTRL->CguCfg[0], 4, 4);
}

uint32_t SYSCTRL_GetPClk()
{
    return SYSCTRL_GetHClk() / get_safe_divider((uint32_t)&APB_SYSCTRL->CguCfg[0], 4, 4);
}

void SYSCTRL_SelectTypeAClk(SYSCTRL_Item item, SYSCTRL_ClkMode mode)
{
    switch (item)
    {
    case SYSCTRL_ITEM_APB_PWM:
        set_reg_bit(&APB_SYSCTRL->CguCfg8, mode, 15);
        break;
    case SYSCTRL_ITEM_APB_ADC:
        set_reg_bit(&APB_SYSCTRL->CguCfg8, mode, 17);
        break;
    case SYSCTRL_ITEM_APB_TMR0:
    case SYSCTRL_ITEM_APB_TMR1:
        set_reg_bit(&APB_SYSCTRL->CguCfg8, mode, 19);
        break;
    default:
        break;
    }
}

void SYSCTRL_SelectCPU32k(SYSCTRL_CPU32kMode mode)
{
    uint8_t enable = (mode == SYSCTRL_CPU_32k_CLK_EXT) ? 1 : 0;
    set_reg_bit((uint32_t*)AON1_CTRL_BASE, enable, 7);
    set_reg_bit((uint32_t*)AON1_CTRL_BASE, enable, 5);
}

uint32_t SYSCTRL_GetCLK32k(void)
{
    return RTC_CLK_FREQ;
}

void SYSCTRL_SetPClkDiv(uint8_t div)
{
    set_reg_bits(APB_SYSCTRL->CguCfg, div, 4, 4);
    set_reg_bit(APB_SYSCTRL->CguCfg, 1, 29);
}

void SYSCTRL_SetFastPreDiv(uint8_t div)
{
    if(div >= SYSCTRL_CLK_FAST_PER_DIV1)
        div = div - SYSCTRL_CLK_FAST_PER_DIV1 + 1;
    set_reg_bits(APB_SYSCTRL->CguCfg, div, 4, 12);
    set_reg_bit(APB_SYSCTRL->CguCfg, 1, 31);
}

void SYSCTRL_SelectKeyScanClk(SYSCTRL_ClkMode mode)
{
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, (mode == 0) ? 1 : 0, 13);
    if (mode >= SYSCTRL_CLK_SLOW_DIV_1)
    {
        set_reg_bits(APB_SYSCTRL->CguCfg, mode, 4, 24);
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 1, 31);
    }
}

uint32_t SYSCTRL_GetHClk(void)
{
    if (*AON1_REG5 & (1ul << 30))
        return SYSCTRL_GetPLLClk() / get_safe_divider(*AON1_REG5, 20, 4);

    return SYSCTRL_GetSlowClk();
}

uint32_t SYSCTRL_GetFastPreCLK(void)
{
    return SYSCTRL_GetPLLClk() / get_safe_divider(APB_SYSCTRL->CguCfg[0], 12, 4);
}

uint32_t SYSCTRL_GetFlashClk()
{
    if (*AON1_REG5 & (1ul << 31))
        return SYSCTRL_GetPLLClk() / get_safe_divider(*AON1_REG5, 24, 4);

    return SYSCTRL_GetSlowClk();
}

uint32_t SYSCTRL_GetClk(SYSCTRL_Item item)
{
    switch (item)
    {
    case SYSCTRL_ITEM_APB_TMR0:
    case SYSCTRL_ITEM_APB_TMR1:
        if (APB_SYSCTRL->CguCfg[1] & (1 << (15 + (item - SYSCTRL_ITEM_APB_TMR0)*2)))
            return SYSCTRL_GetCLK32k();
        if(APB_SYSCTRL->CguCfg[1] & (1<<16))
            return SYSCTRL_GetPLLClk() / get_safe_divider((uint32_t)APB_SYSCTRL->CguCfg8, 20, 4);
        else
            return SYSCTRL_GetSlowClk() / get_safe_divider((uint32_t)APB_SYSCTRL->CguCfg8, 20, 4);

    case SYSCTRL_ITEM_APB_PWM:
        if ((APB_SYSCTRL->CguCfg[1] & (1 << 23)))
            return SYSCTRL_GetCLK32k();
        if (APB_SYSCTRL->CguCfg8 & (1 << 15))
            return SYSCTRL_GetPLLClk() / get_safe_divider((uint32_t)APB_SYSCTRL->CguCfg8, 27, 4);
        return SYSCTRL_GetSlowClk() / get_safe_divider((uint32_t)APB_SYSCTRL->CguCfg8, 27, 4);
    case SYSCTRL_ITEM_APB_KeyScan:
        if (APB_SYSCTRL->CguCfg[1] & (1 << 13))
            return SYSCTRL_GetCLK32k();
        return SYSCTRL_GetSlowClk() / get_safe_divider((uint32_t)APB_SYSCTRL->CguCfg[0], 24, 4);
    case SYSCTRL_ITEM_AHB_SPI0:
        if (APB_SYSCTRL->CguCfg[1] & (1 << 21))
            return SYSCTRL_GetPLLClk() / get_safe_divider((uint32_t)APB_SYSCTRL->CguCfg[0], 20, 4);
        return SYSCTRL_GetSlowClk() / get_safe_divider((uint32_t)APB_SYSCTRL->CguCfg[0], 20, 4);
    case SYSCTRL_ITEM_APB_SPI1:
        if (APB_SYSCTRL->CguCfg[1] & (1 << 22))
            return SYSCTRL_GetFastPreCLK();
        return SYSCTRL_GetSlowClk();
    case SYSCTRL_ITEM_APB_I2S:
        if (APB_SYSCTRL->CguCfg[1] & (1 << 23))
            return SYSCTRL_GetPLLClk() / get_safe_divider((uint32_t)APB_SYSCTRL->CguCfg[1], 6, 4);
        return SYSCTRL_GetSlowClk();
    case SYSCTRL_ITEM_APB_UART0:
    case SYSCTRL_ITEM_APB_UART1:
        if (APB_SYSCTRL->CguCfg[1] & (1 << (19 + item - SYSCTRL_ITEM_APB_UART0)))
            return SYSCTRL_GetFastPreCLK();
        return SYSCTRL_GetSlowClk();
    case SYSCTRL_ITEM_APB_ADC:
        if (APB_SYSCTRL->CguCfg8 & (1 << 17))
            return SYSCTRL_GetAdcClkDiv();
        return SYSCTRL_GetSlowClk();
    case SYSCTRL_ITEM_APB_USB:
        return SYSCTRL_GetPLLClk() / get_safe_divider((uint32_t)APB_SYSCTRL->USBCfg, 0, 4);
    case SYSCTRL_ITEM_APB_QDEC:
        if (APB_SYSCTRL->QdecCfg & (1 << 15))
            return SYSCTRL_GetFastPreCLK() / get_safe_divider((uint32_t)APB_SYSCTRL->QdecCfg, 1, 10);
        return SYSCTRL_GetSlowClk() / get_safe_divider((uint32_t)APB_SYSCTRL->QdecCfg, 1, 10);
    case SYSCTRL_ITEM_APB_I2C0:
    case SYSCTRL_ITEM_APB_RTIMER0:
    case SYSCTRL_ITEM_APB_RTIMER1:
        return SYSCTRL_GetPClk();
    default:
        return SYSCTRL_GetSlowClk();
    }
}

void SYSCTRL_SetAdcClkDiv(uint8_t denom)
{
    APB_SYSCTRL->CguCfg8 &= ~0x1fff;
    APB_SYSCTRL->CguCfg8 |= 0x1000 | 0x40 | (denom & 0x3f);
}

uint32_t SYSCTRL_GetAdcClkDiv(void)
{
    uint16_t denom = APB_SYSCTRL->CguCfg8 & 0x3f;
    uint16_t num = (APB_SYSCTRL->CguCfg8 >> 6) & 0x3f;
    return SYSCTRL_GetSlowClk() * num / denom;
}

uint8_t SYSCTRL_MemoryRetentionCtrl(uint8_t mode, uint32_t block_map)
{
    if (mode == 0)
    {
        block_map &= 0x30;
        if (block_map)
        {
            set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 0,13);
            set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 0,18);
            set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), block_map&SYSCTRL_SYS_BLOCK00, 13);
            set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), block_map&SYSCTRL_SYS_BLOCK01, 18);
        }
        else
            return 1;
    }
    else
    {
        block_map &= 0x7f;
        set_reg_bits((volatile uint32_t *)(AON2_CTRL_BASE + 0x4), 0, 7, 12);
        set_reg_bits((volatile uint32_t *)(AON2_CTRL_BASE + 0x4), block_map, 7, 12);
    }
    return 0;
}

static void SYSCTRL_CacheFlush(const uintptr_t base)
{
    *(volatile uint32_t *)(base + 0x58) =  (1UL<<31) | 0x4;
}

static void SYSCTRL_CacheControl0(const uintptr_t base, SYSCTRL_CacheMemCtrl ctrl, uint8_t bit_offset)
{
    if (SYSCTRL_MEM_BLOCK_AS_CACHE != ctrl)
        set_reg_bit((volatile uint32_t *)(base), 0, 1);

    set_reg_bits(&APB_SYSCTRL->SysCtrl, ctrl, 1, bit_offset);

    if (SYSCTRL_MEM_BLOCK_AS_CACHE == ctrl)
    {
        SYSCTRL_CacheFlush(base);
        set_reg_bit((volatile uint32_t *)(base), 1, 1);
    }
}

void SYSCTRL_ICacheControl(SYSCTRL_CacheMemCtrl i_cache)
{
    #define IC_BASE 0x40140000

    SYSCTRL_CacheControl0(IC_BASE, i_cache, 1);
}

void SYSCTRL_ICacheFlush(void)
{
    SYSCTRL_CacheFlush(IC_BASE);
}

uint32_t SYSCTRL_RC2MCalib(uint32_t clc)
{
    static volatile uint32_t pcap[2];
    uint32_t diff;

    uint32_t freq;
    volatile DMA_Descriptor descriptor __attribute__((aligned(8)));

    *(volatile uint32_t *)(AON1_CTRL_BASE + 0x3C) &= ~(0xfful<<24);
    *(volatile uint32_t *)(AON1_CTRL_BASE + 0x3C) |= (clc&0xff)<<24;
    APB_PWM->Channels[0].Ctrl0 = (0x6<<7)|(0x1<<21)|(0x1<<20);
    APB_PWM->PCAPChannels[0].Ctrl0 |= 0x1;
    APB_PWM->CapCntEn = 0x1;

    descriptor.Ctrl = 0x0|(0x0<<4)|(0x8<<8)|(0x2<<12)|(0x2<<14)|(0x0<<16)|(0x1<<17)|(0x2<<18)|(0x2<<21)|(0x1<<24) ;
    descriptor.SrcAddr = (uint32_t)(&APB_PWM->PCAPChannels[0].Ctrl1);
    descriptor.DstAddr = (uint32_t)&pcap[1];
    descriptor.TranSize = 1000*2-1;
    descriptor.Next = 0;

    APB_DMA->Channels[0].Descriptor.Ctrl = 0x0|(0x8<<8)|(0x2<<14)|(0x0<<16)|(0x1<<17)|(0x2<<18)|(0x2<<21)|(0x1<<24) ;
    APB_DMA->Channels[0].Descriptor.SrcAddr = (uint32_t)(&APB_PWM->PCAPChannels[0].Ctrl1);
    APB_DMA->Channels[0].Descriptor.DstAddr = (uint32_t)pcap;
    APB_DMA->Channels[0].Descriptor.TranSize = 2;
    APB_DMA->Channels[0].Descriptor.Next = (DMA_Descriptor*)&descriptor;
    __DSB();
    APB_DMA->Channels[0].Descriptor.Ctrl |= 0x1;
    APB_PWM->Channels[0].Ctrl0 |= 0x1<<6;

    while (!(APB_DMA->IntStatus & (0x1<<16)));
    APB_DMA->IntStatus = 0xffffffff;
    __DSB();
    while ((APB_DMA->IntStatus & (0x1<<16)));

    diff = pcap[1] - pcap[0];
    freq = (1000UL * 24000UL) / diff;

    SYSCTRL_ResetBlock(SYSCTRL_ITEM_APB_PWM);
    SYSCTRL_ReleaseBlock(SYSCTRL_ITEM_APB_PWM);
    return (uint32_t)(freq*1000);
}

uint32_t SYSCTRL_RC2MTune(uint32_t freq)
{
    uint32_t start;
    uint32_t end;
    uint32_t mid;

    uint32_t min;
    uint32_t max;

    uint32_t Freq;

    uint8_t enabled = io_read(AON1_CTRL_BASE + 0x28) & 1;
    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_DMA);
    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_PWM);
    set_reg_bit((volatile uint32_t *)(APB_SYSCTRL_BASE + 0x70), 1, 0);
    set_reg_bit((volatile uint32_t *)(APB_SYSCTRL_BASE + 0x70), 1, 3);

    APB_SYSCTRL->DmaCtrl[1] = 0x8 | (0x9<<4);

    min = 0;
    max = 0xff;

    start = min;
    end = max;

    while(start < end){
        mid = (start + end) / 2;
        Freq = SYSCTRL_RC2MCalib(mid);
        if(Freq == freq){
            break;
        }
        else if(Freq < freq){
            start = mid + 1;
        }
        else{
            end = mid - 1;
        }
    }
    if(end < min){
        end = min;
    }
    if(start > max){
        start = max;
    }

    Freq = SYSCTRL_RC2MCalib(end);
    if(Freq > freq){
        Freq = SYSCTRL_RC2MCalib(start);
    }

    APB_PWM->Channels[0].Ctrl0 &= ~(0x1<<20);
    APB_PWM->PCAPChannels[0].Ctrl0 = 0;
    set_reg_bit((volatile uint32_t *)(APB_SYSCTRL_BASE + 0x70), 0, 0);
    set_reg_bit((volatile uint32_t *)(APB_SYSCTRL_BASE + 0x70), 0, 3);
    APB_SYSCTRL->DmaCtrl[1] = 0x76543210;
    SYSCTRL_SetClkGate(SYSCTRL_ITEM_APB_DMA);
    SYSCTRL_SetClkGate(SYSCTRL_ITEM_APB_PWM);

    return Freq;
}

__attribute__((weak)) int Vcore_calib(void)
{
    // add `eflash.c` to the project!
    return -1;
}

int SYSCTRL_Init(void)
{
    typedef void    (*rom_PowerOnSeq)(uint8_t XOMode, uint8_t XOModeFast, uint8_t SeqFastMode);
    typedef void    (*rom_PowerDownSeq)(void);
    typedef void    (*rom_PLLinUse)(uint8_t PLLEn, uint8_t XOMode, uint8_t XOModeFast, uint8_t SeqFastMode);
    typedef void       (*rom_PowerDownSeqPatch             )(void);

    #define ROM_PLLinUse    ((rom_PLLinUse)(0x00000e21))
    #define ROM_PowerOnSeq          ((rom_PowerOnSeq)(0x0000101d))
    #define ROM_PowerDownSeq        ((rom_PowerDownSeq)(0x00000f05))
    #define ROM_PowerDownSeqPatch   ((rom_PowerDownSeqPatch)(0x00001001))
    SYSCTRL_RC2MTune(1500000);
    ROM_PowerOnSeq(1, 1, 1);
    ROM_PowerDownSeq();
    ROM_PowerDownSeqPatch();

    if (io_read(AON1_CTRL_BASE + 0x28) & 1)
        ROM_PLLinUse(1, 1, 1, 1);
    else
        ROM_PLLinUse(0, 1, 1, 1);

    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), 0x1d, 6, 16);
    set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x4),0,18);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x14),0,26);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10),0,10);
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10),0,7);

    Vcore_calib();
    return 0;
}

void SYSCTRL_SelectI2sClkDiv(pre_clk_source_t source, uint8_t div)
{
    set_reg_bit(APB_SYSCTRL->CguCfg + 1, source == SOURCE_SLOW_CLK ? 0 : 1, 11);
    if (div)
    {
        set_reg_bits(APB_SYSCTRL->CguCfg + 1, div, 4, 6);
        set_reg_bit(APB_SYSCTRL->CguCfg + 1, 1, 10);
    }
}

void SYSCTRL_UpdateAsdmClk(uint32_t div)
{
    set_reg_bits((volatile uint32_t *)(APB_SYSCTRL_BASE + 0x21c), div, 5, 5);
    set_reg_bit((volatile uint32_t *)(APB_SYSCTRL_BASE + 0x21c), 1, 10);
}

void SYSCTRL_UpdateQdecClk(uint32_t div)
{
    set_reg_bits((volatile uint32_t *)(APB_SYSCTRL_BASE + 0x21c), div, 4, 0);
    set_reg_bit((volatile uint32_t *)(APB_SYSCTRL_BASE + 0x21c), 1, 4);
}

void SYSCTRL_SelectUSBClk(SYSCTRL_ClkMode mode)
{
    set_reg_bits(&APB_SYSCTRL->USBCfg, mode, 4, 0);
    set_reg_bit(&APB_SYSCTRL->USBCfg, 1, 4);
}

void SYSCTRL_SetBuckDCDCOutput(SYSCTRL_BuckDCDCOutput level)
{
    set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x34), level, 6, 14);
}

void SYSCTRL_EnableBuckDCDC(uint8_t enable)
{
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x18), enable, 1);
}

void SYSCTRL_EnableDCDCMode(uint8_t mode)
{
    if (mode)
    {
        set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x20), 50, 6, 0);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x18),  1, 1);
        set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x4),  1, 19);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x18),  0, 2);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x18),  1, 0);
        while ((*(volatile uint32_t *)(APB_SYSCTRL_BASE + 0x234)&0x40) == 0);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x18),  0, 3);
    }
    else
    {
        set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE + 0x20), 1, 6, 0);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x18),  1, 2);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x18),  0, 0);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x18),  1, 3);
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x18),  0, 1);
    }
}

void SYSCTRL_USBPhyConfig(uint8_t enable, uint8_t pull_sel)
{
    if(enable)
    {
        set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE), 1ul | (1 << pull_sel) | (4 << 4), 9, 17);
    }
    else
    {
        set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE), 0, 9, 17);
    }
}

uint8_t SYSCTRL_GetLastWakeupSource(SYSCTRL_WakeupSource_t *source)
{
    source->other = 0;
    source->gpio = APB_SYSCTRL->SysIoWkSource;
    uint32_t a = APB_SYSCTRL->SysIoStatus >> 16;
    if ((source->gpio == 0) && (a == 0))
    {
        a = io_read(AON2_CTRL_BASE + 0x74);
        switch (a & 3)
        {
        case 0:
            return 0;
        case 1:
            source->other |= SYSCTRL_WAKEUP_SOURCE_AUTO;
            break;
        case 3:
            source->other |= SYSCTRL_WAKEUP_SOURCE_AUTO; // fall through
        case 2:
            a >>= 2;
            source->gpio = a & 1;
            source->gpio |= (a & (0x3 << 1)) << (5 - 1);
            source->gpio |= (a & (0x7 << 3)) << (21 - 3);
            source->gpio |= ((uint64_t)(a & (0x3 << 5))) << (36 - 5);
            break;
        default:
            break;
        }

        return 1;
    }

    source->gpio |= ((uint64_t)(a & 0x0f)) << 32;
    if (a & (1 << 4))
        source->other |= SYSCTRL_WAKEUP_SOURCE_AUTO;

    return 1;
}

void SYSCTRL_EnableWakeupSourceDetection(void)
{
    set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x84), 1, 16);
}

void SYSCTRL_Reset(void)
{
    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_WDT);
    TMR_WatchDogEnable3(WDT_INTTIME_INTERVAL_2MS, WDT_RSTTIME_INTERVAL_4MS, 0);
    while(1);
}

void SYSCTRL_EnableClockOutput(uint8_t enable, uint16_t denom)
{
    if (enable)
    {
        io_write(APB_SYSCTRL_BASE + 0x1e0, (denom & 0x3ff) | (7 << 10));
    }
    else
    {
        io_write(APB_SYSCTRL_BASE + 0x1e0, 1);
    }
}

void SYSCTRL_EnableInternalVref(uint8_t enable)
{
    set_reg_bit((volatile uint32_t*)(AON1_CTRL_BASE+0X18), !enable, 16);
}

void SYSCTRL_EnableAsdmVrefOutput(uint8_t enable)
{
    set_reg_bit((volatile uint32_t*)(APB_SYSCTRL_BASE + 0x234), enable, 7);
}

void SYSCTRL_SetAdcVrefSel(uint8_t val)
{
    if (val)
    {
        set_reg_bits((volatile uint32_t*)(APB_SYSCTRL_BASE + 0x234), val, 4, 9);
        set_reg_bit((volatile uint32_t*)(APB_SYSCTRL_BASE + 0x234), 0, 8);
    }
    else
        set_reg_bit((volatile uint32_t*)(APB_SYSCTRL_BASE + 0x234), 1, 8);
}

void SYSCTRL_EnableResetSource(uint8_t enable)
{
    set_reg_bit((volatile uint32_t*)(APB_SYSCTRL_BASE + 0x218), enable, 0);
}

SYSCTRL_ResetSource SYSCTRL_GetResetSource(void)
{
    uint32_t val;
    val = *(volatile uint32_t*)(APB_SYSCTRL_BASE + 0x218);

    return (SYSCTRL_ResetSource)((val>>1)&0x3f);
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)

static void set_reg_bits(volatile uint32_t *reg, uint32_t v, uint8_t bit_width, uint8_t bit_offset)
{
    uint32_t field = bit_width == 32 ? UINT32_MAX : (1u << bit_width) - 1u;
    uint32_t mask = field << bit_offset;
    *reg = (*reg & ~mask) | ((v & field) << bit_offset);
}

static void set_reg_bit(volatile uint32_t *reg, uint8_t v, uint8_t bit_offset)
{
    uint32_t mask = 1u << bit_offset;
    *reg = (*reg & ~mask) | (v ? mask : 0u);
}

static int get_safe_divider(uint32_t reg, int bit_offset, int bit_width)
{
    uint32_t value = (reg >> bit_offset) & ((1u << bit_width) - 1u);
    return value ? (int)value : 1;
}

static void ing26_apb_gate(unsigned int bit, uint8_t enable)
{
    set_reg_bit(APB1_SYSCTRL->CguCfg + 4, 1, bit);
    set_reg_bit(APB1_SYSCTRL->CguCfg + 3, enable, bit);
}

static void ing26_function_gate(unsigned int bit, uint8_t enable)
{
    set_reg_bit(APB1_SYSCTRL->CguCfg + 6, 1, bit);
    set_reg_bit(APB1_SYSCTRL->CguCfg + 5, enable, bit);
}

static void ing26_ahb_gate(unsigned int bit, uint8_t enable)
{
    set_reg_bit(APB1_SYSCTRL->CguCfg + 2, 1, bit + 15);
    set_reg_bit(APB1_SYSCTRL->CguCfg + 2, enable, bit);
}

static void SYSCTRL_ClkGateCtrl(SYSCTRL_ClkGateItem item, uint8_t v)
{
    switch (item)
    {
    case SYSCTRL_ITEM_APB1_GPIO0:
        ing26_apb_gate(21, v);
        ing26_function_gate(18, (APB1_SYSCTRL->CguCfg[3] & ((1u << 21) | (1u << 22))) != 0);
        break;
    case SYSCTRL_ITEM_APB1_GPIO1:
        ing26_apb_gate(22, v);
        ing26_function_gate(18, (APB1_SYSCTRL->CguCfg[3] & ((1u << 21) | (1u << 22))) != 0);
        break;
    case SYSCTRL_ITEM_APB1_TMR0      :
        ing26_apb_gate(2, v);
        ing26_function_gate(0, v);
        break;
    case SYSCTRL_ITEM_APB1_TMR1      :
        ing26_apb_gate(3, v);
        ing26_function_gate(1, v);
        break;
    case SYSCTRL_ITEM_APB1_WDT       :
        ing26_apb_gate(1, v);
        ing26_function_gate(17, v);
        break;
    case SYSCTRL_ITEM_APB2_PWM       :
        ing26_apb_gate(5, v);
        ing26_function_gate(10, v);
        break;
    case SYSCTRL_ITEM_APB2_QDEC      :
        ing26_apb_gate(9, v);
        set_reg_bit(&APB1_SYSCTRL->QdecCfg, 1, 13);
        set_reg_bit(&APB1_SYSCTRL->QdecCfg, v, 12);
        break;
    case SYSCTRL_ITEM_APB2_KeyScan   :
        ing26_apb_gate(10, v);
        ing26_function_gate(19, v);
        ing26_function_gate(20, v);
        break;
    case SYSCTRL_ITEM_APB1_DMA0       :
        ing26_apb_gate(12, v);
        ing26_ahb_gate(0, v);
        break;
    case SYSCTRL_ITEM_APB1_SPI1      :
        ing26_apb_gate(14, v);
        ing26_function_gate(7, v);
        break;
    case SYSCTRL_ITEM_APB1_ADC       :
        ing26_apb_gate(15, v);
        ing26_function_gate(12, v);
        break;
    case SYSCTRL_ITEM_APB2_I2S       :
        ing26_apb_gate(16, v);
        ing26_function_gate(8, v);
        break;
    case SYSCTRL_ITEM_APB1_UART0     :
        ing26_apb_gate(17, v);
        ing26_function_gate(4, v);
        break;
    case SYSCTRL_ITEM_APB1_UART1     :
        ing26_apb_gate(18, v);
        ing26_function_gate(5, v);
        break;
    case SYSCTRL_ITEM_APB1_I2C0      :
        ing26_apb_gate(19, v);
        break;
    case SYSCTRL_ITEM_APB1_SysCtrl   :
        ing26_apb_gate(0, v);
        break;
    case SYSCTRL_ITEM_APB1_PinCtrl   :
        ing26_apb_gate(6, v);
        break;
    case SYSCTRL_ITEM_AHB_USB:
        set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x54), v, 0);
        set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x54), v, 7);
        break;
    case SYSCTRL_ITEM_APB2_ASDM       :
        ing26_apb_gate(28, v);
        ing26_function_gate(21, v);
        break;
    case SYSCTRL_ITEM_APB1_RTIMER2    :
        ing26_apb_gate(4, v);
        ing26_function_gate(2, v);
        break;
    case SYSCTRL_ITEM_APB1_RTIMER3    :
        ing26_apb_gate(4, v);
        ing26_function_gate(2, v);
        break;
    case SYSCTRL_ITEM_APB1_SPI2    :
        ing26_ahb_gate(13, v);
        ing26_apb_gate(29, v);
        break;
    case SYSCTRL_ITEM_APB1_PTE    :
        ing26_apb_gate(26, v);
        break;
    case SYSCTRL_ITEM_APB2_SPI0    :
        ing26_ahb_gate(12, v);
        ing26_apb_gate(13, v);
        break;
    case SYSCTRL_ITEM_APB2_DMA1    :
        ing26_ahb_gate(14, v);
        ing26_apb_gate(30, v);
        break;
    case SYSCTRL_ITEM_APB2_GPIOTE :
        ing26_apb_gate(27, v);
        break;
    case SYSCTRL_ITEM_APB2_SDIO  :
        ing26_function_gate(9, v);
        ing26_ahb_gate(9, v);
        ing26_apb_gate(11, v);
        break;
    case SYSCTRL_ITEM_APB1_HUART0  :
        ing26_apb_gate(17, v);
        break;
    case SYSCTRL_ITEM_APB1_HUART1 :
        ing26_apb_gate(23, v);
        break;
    case SYSCTRL_ITEM_APB2_HUART2   :
        ing26_apb_gate(25, v);
        break;
    case SYSCTRL_ITEM_AHB_LPUART :
        set_reg_bit((__IO uint32_t *)(AON2_CTRL_BASE + 0x2c), v, 18);
        set_reg_bit((__IO uint32_t *)(AON2_CTRL_BASE + 0x2c), v, 7);
        break;
    case SYSCTRL_ITEM_AHB_LPQDEC :
        set_reg_bit((__IO uint32_t *)(AON2_CTRL_BASE + 0x2c), v, 23);
        set_reg_bit((__IO uint32_t *)(AON2_CTRL_BASE + 0x2c), v, 9);
        set_reg_bit((__IO uint32_t *)(AON2_CTRL_BASE + 0x2c), v, 17);
        break;
    case SYSCTRL_ITEM_AHB_LPTIMER :
        set_reg_bit((__IO uint32_t *)(AON2_CTRL_BASE + 0x2c), v, 11);
        set_reg_bit((__IO uint32_t *)(AON2_CTRL_BASE + 0x2c), v, 22);
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

void SYSCTRL_SetClkGateMulti(uint64_t items)
{
    unsigned int i;
    for (i = 0; i < SYSCTRL_ITEM_NUMBER; ++i)
        if (items & (UINT64_C(1) << i))
            SYSCTRL_SetClkGate((SYSCTRL_Item)i);
}

static uint32_t ing26_dma_items;

int SYSCTRL_SelectUsedDmaItems(uint32_t items)
{
    uint32_t regs[3] = {0, 0, 0};
    unsigned int group, item, slot;
    if (items & ((1u << 15) | (1u << RESERVED) | (3u << 30))) return -1;
    for (group = 0; group < 2; ++group)
    {
        slot = group * 8u;
        for (item = group ? SYSCTRL_DMA_UART0_TX : 0;
             item < (unsigned int)(group ? SYSCTRL_DMA_LAST : SYSCTRL_DMA_UART0_TX); ++item)
        {
            unsigned int reg, shift;
            if (!(items & (1u << item))) continue;
            if (slot == (group + 1u) * 8u) return group ? -2 : -1;
            reg = slot < 6 ? 0 : slot < 12 ? 1 : 2;
            shift = (slot < 6 ? slot : slot < 12 ? slot - 6 : slot - 12) * 5u;
            regs[reg] |= (item - (group ? SYSCTRL_DMA_UART0_TX : 0)) << shift;
            ++slot;
        }
    }
    io_write(APB1_SYSCTRL_BASE + 0x30, regs[0]);
    io_write(APB1_SYSCTRL_BASE + 0x34, regs[1]);
    io_write(APB1_SYSCTRL_BASE + 0x38, regs[2]);
    ing26_dma_items = items;
    return 0;
}

int SYSCTRL_GetDmaId(SYSCTRL_DMA item)
{
    unsigned int slot, start;
    if ((unsigned int)item >= SYSCTRL_DMA_LAST || !(ing26_dma_items & (1u << item))) return -1;
    start = item >= SYSCTRL_DMA_UART0_TX ? 8 : 0;
    for (slot = start; slot < start + 8; ++slot)
    {
        unsigned int reg = slot < 6 ? 0 : slot < 12 ? 1 : 2;
        unsigned int shift = (slot < 6 ? slot : slot < 12 ? slot - 6 : slot - 12) * 5;
        if (((io_read(APB1_SYSCTRL_BASE + 0x30 + reg * 4) >> shift) & 31u)
            == (uint32_t)(item - (start ? SYSCTRL_DMA_UART0_TX : 0))) return slot;
    }
    return -1;
}

void SYSCTRL_ClearClkGateMulti(uint64_t items)
{
    unsigned int i;
    for (i = 0; i < SYSCTRL_ITEM_NUMBER; ++i)
        if (items & (UINT64_C(1) << i))
            SYSCTRL_ClearClkGate((SYSCTRL_Item)i);
}

static void SYSCTRL_ResetBlockCtrl(SYSCTRL_ResetItem item, uint8_t v)
{
    switch (item)
    {
    case SYSCTRL_ClkGate_APB1_GPIO0:
        set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x84), v, 25);
        break;
    case SYSCTRL_ClkGate_APB1_GPIO1:
        set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x84), v, 26);
        break;
    case SYSCTRL_ClkGate_APB1_TMR0      :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 12);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 2);
        break;
    case SYSCTRL_ClkGate_APB1_TMR1      :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 13);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 3);
        break;
    case SYSCTRL_ClkGate_APB1_WDT       :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 11);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 18);
        break;
    case SYSCTRL_ClkGate_APB2_PWM       :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 15);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 12);
        break;
    case SYSCTRL_ClkGate_APB2_QDEC      :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 19);
        set_reg_bit(&APB1_SYSCTRL->QdecCfg, v, 14);
        break;
    case SYSCTRL_ClkGate_APB2_KEYSCAN   :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 26);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 20);
        break;
    case SYSCTRL_ClkGate_APB1_DMA0       :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 22);
        break;
    case SYSCTRL_ClkGate_APB2_SPI0      :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 23);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 8);
        break;
    case SYSCTRL_ClkGate_APB1_SPI1      :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 24);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 9);
        break;
    case SYSCTRL_ClkGate_APB1_ADC       :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 25);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 14);
        break;
    case SYSCTRL_ClkGate_APB2_I2S       :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 26);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 10);
        break;
    case SYSCTRL_ClkGate_APB1_UART0     :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 27);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 6);
        break;
    case SYSCTRL_ClkGate_APB1_UART1     :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 28);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 7);
        break;
    case SYSCTRL_ClkGate_APB1_I2C0      :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 29);
        break;
    case SYSCTRL_ClkGate_APB1_SYSCTRL   :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 10);
        break;
    case SYSCTRL_ClkGate_APB1_PINCTRL   :
        set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x84), v, 24);
        break;
    case SYSCTRL_ClkGate_APB2_ASDM  :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 21);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 27);
        break;
    case SYSCTRL_ClkGate_APB1_RTIMER2   :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 14);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 4);
        break;
    case SYSCTRL_ClkGate_APB1_RTIMER3   :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 14);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 4);
        break;
    case SYSCTRL_ClkGate_APB1_SPI2    :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 13);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 15);
        break;
    case SYSCTRL_ClkGate_APB1_PTE    :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 28);
        break;
    case SYSCTRL_ClkGate_APB2_DMA1    :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 17);
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 20);
        break;
    case SYSCTRL_ClkGate_APB2_GPIOTE    :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 29);
        break;
    case SYSCTRL_ClkGate_APB1_HUART0    :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 18);
        break;
    case SYSCTRL_ClkGate_APB1_HUART1    :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 30);
        break;
    case SYSCTRL_ClkGate_APB2_SDIO  :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 1, v, 1);
        break;
    case SYSCTRL_ClkGate_APB2_HUART2    :
        set_reg_bit(APB1_SYSCTRL->RstuCfg + 0, v, 8);
        break;
    case SYSCTRL_ClkGate_AHB_USB:
        set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x54), v, 1);
        set_reg_bit((volatile uint32_t *)(AON2_CTRL_BASE + 0x54), v, 6);
        break;
    case SYSCTRL_ClkGate_AHB_LPQDEC     :
        set_reg_bit((__IO uint32_t *)(AON2_CTRL_BASE + 0x2c), v, 9);
        set_reg_bit((__IO uint32_t *)(AON2_CTRL_BASE + 0x2c), v, 17);
        break;
    case SYSCTRL_ClkGate_AHB_LPUART     :
        set_reg_bit((__IO uint32_t *)(AON2_CTRL_BASE + 0x2c), v, 7);
        break;
    case SYSCTRL_ClkGate_AHB_LPTIMER     :
        set_reg_bit((__IO uint32_t *)(AON2_CTRL_BASE + 0x2c), v, 11);
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

void SYSCTRL_EnablePcapMode(const uint8_t channel_index, uint8_t enable)
{
    set_reg_bit((volatile uint32_t *)(APB1_SYSCTRL_BASE + 0x70), enable & 0x1, channel_index);
}

void SYSCTRL_SelectQDECClk(SYSCTRL_ClkMode mode, uint16_t div)
{
    set_reg_bit(&APB1_SYSCTRL->QdecCfg, mode, 15);
    set_reg_bits(&APB1_SYSCTRL->QdecCfg, div, 10, 1);
    set_reg_bit(&APB1_SYSCTRL->QdecCfg, 1, 11);
}

uint32_t SYSCTRL_GetSlowClk(void)
{
    return (io_read(AON1_CTRL_BASE + 0x10) & (1u << 8)) ? OSC_CLK_FREQ : SYSCTRL_RC_FAST_HZ;
}

int SYSCTRL_ConfigPLLClk(uint32_t div_pre, uint32_t loop, uint32_t div_output)
{
    uint32_t freq, vco, value;
    if (!div_pre || div_pre > 63u || !loop || loop > 255u || !div_output || div_output > 63u)
        return 1;
    if (SYSCTRL_GetSlowClk() / div_pre < 2000000u)
        return 1;
    freq = (uint32_t)((uint64_t)SYSCTRL_GetSlowClk() * loop / div_pre / 1000000u);
    if (freq >= 460 && freq <= 600) vco = 7;
    else if (freq >= 380 && freq <= 520) vco = 6;
    else if (freq >= 300 && freq <= 440) vco = 5;
    else if (freq >= 220 && freq <= 360) vco = 4;
    else if (freq >= 160 && freq <= 280) vco = 3;
    else if (freq >= 100 && freq <= 200) vco = 2;
    else if (freq >= 60 && freq <= 120) vco = 1;
    else return 2;
    value = io_read(AON1_CTRL_BASE + 0x28);
    value = (value & ~0x00fffffeu) | (div_pre << 1) | (loop << 7)
          | (div_output << 15) | (vco << 21);
    io_write(AON1_CTRL_BASE + 0x28, value);
    return 0;
}

int SYSCTRL_EnablePLLChecked(uint8_t enable, uint32_t max_polls)
{
    set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x28), enable, 0);
    if (!enable) return 0;
    while (max_polls--)
        if (APB1_SYSCTRL->PllCtrl & (1u << 30)) return 0;
    return -1;
}

void SYSCTRL_EnablePLL(uint8_t enable)
{
    (void)SYSCTRL_EnablePLLChecked(enable, 1000000u);
}

uint32_t SYSCTRL_GetPLLClk(void)
{
    uint32_t pll = io_read(AON1_CTRL_BASE + 0x28);
    uint32_t pre = (pll >> 1) & 63u, out = (pll >> 15) & 63u;
    if (!(pll & 1u) || !pre || !out) return 0;
    return (uint32_t)((uint64_t)SYSCTRL_GetSlowClk() * ((pll >> 7) & 255u) / pre / out);
}

void SYSCTRL_EnableSlowRC(uint8_t enable, SYSCTRL_SlowRCClkMode mode)
{
    #define AON1_PMU3       ((volatile uint32_t *)(AON1_CTRL_BASE + 0x3c))

    if (enable)
    {
        // TODO:
    }
    else
    {
        set_reg_bit(AON1_PMU3, 0, 19);
    }
}

#define ING26_CLK_CFG ((volatile uint32_t *)(AON1_CTRL_BASE + 0x4c))
#define ING26_CLK_PULSES ((1u << 12) | (1u << 13) | (1u << 22))

static uint32_t ing26_pll_div_clk(void)
{
    return SYSCTRL_GetPLLClk() / get_safe_divider(*ING26_CLK_CFG, 17, 5);
}

static uint32_t ing26_per_clk(void)
{
    return (io_read(APB1_SYSCTRL_BASE + 0x21c) & (1u << 23))
         ? ing26_pll_div_clk() : SYSCTRL_GetPLLClk();
}

uint32_t SYSCTRL_GetFastPreCLK(void)
{
    uint32_t source = (io_read(APB1_SYSCTRL_BASE + 0x21c) & (1u << 21))
                    ? ing26_pll_div_clk() : SYSCTRL_GetPLLClk();
    return source / get_safe_divider(APB1_SYSCTRL->CguCfg[1], 7, 4);
}

static int ing26_select_main_clock(SYSCTRL_ClkMode mode, uint8_t offset, uint8_t width,
                                   uint8_t source_bit, uint8_t update_bit, uint32_t max_polls)
{
    uint32_t old_div = get_safe_divider(*ING26_CLK_CFG, offset, width);
    uint32_t div = mode ? (uint32_t)mode : 1u;
    uint32_t mask = ((1u << width) - 1u) << offset;
    uint32_t value;
    uint8_t valid_shift = source_bit == 14 ? 24 : 26;
    uint8_t settled = 0;
    if ((unsigned int)mode > 15u) return -2;
    if (mode && SYSCTRL_EnablePLLChecked(1, max_polls)) return -1;
    if (div >= old_div)
    {
        value = *ING26_CLK_CFG & ~(ING26_CLK_PULSES | mask);
        *ING26_CLK_CFG = value | (div << offset) | (1u << update_bit);
    }
    value = *ING26_CLK_CFG & ~(ING26_CLK_PULSES | (1u << source_bit));
    *ING26_CLK_CFG = value | (mode ? (1u << source_bit) : 0u);
    while (max_polls--)
    {
        if (((io_read(APB1_SYSCTRL_BASE + 0x1c8) >> valid_shift) & 3u) == (mode ? 2u : 1u))
        {
            settled = 1;
            break;
        }
    }
    if (!settled) return -1;
    if (div < old_div)
    {
        value = *ING26_CLK_CFG & ~(ING26_CLK_PULSES | mask);
        *ING26_CLK_CFG = value | (div << offset) | (1u << update_bit);
    }
    return 0;
}

int SYSCTRL_SelectHClkChecked(SYSCTRL_ClkMode mode, uint32_t max_polls)
{
    int result = ing26_select_main_clock(mode, 0, 8, 14, 12, max_polls);
    if (!result) set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), !!mode, 13);
    return result;
}

void SYSCTRL_SelectHClk(SYSCTRL_ClkMode mode)
{
    (void)SYSCTRL_SelectHClkChecked(mode, 1000000u);
}

int SYSCTRL_SelectFlashClkChecked(SYSCTRL_ClkMode mode, uint32_t max_polls)
{
    int result = ing26_select_main_clock(mode, 8, 4, 15, 13, max_polls);
    if (!result) set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x1c), !!mode, 14);
    return result;
}

void SYSCTRL_SelectFlashClk(SYSCTRL_ClkMode mode)
{
    (void)SYSCTRL_SelectFlashClkChecked(mode, 1000000u);
}

uint32_t SYSCTRL_GetHClk(void)
{
    uint32_t config = *ING26_CLK_CFG;
    uint32_t source = (config & (1u << 14))
                    ? ((config & (1u << 16)) ? ing26_pll_div_clk() : SYSCTRL_GetPLLClk())
                    : SYSCTRL_GetSlowClk();
    return source / get_safe_divider(config, 0, 8);
}

uint32_t SYSCTRL_GetFlashClk(void)
{
    uint32_t config = *ING26_CLK_CFG;
    uint32_t source = (config & (1u << 15)) ? ing26_per_clk() : SYSCTRL_GetSlowClk();
    return source / get_safe_divider(config, 8, 4);
}

void SYSCTRL_SetPClkDiv(uint8_t div)
{
    uint32_t value;
    if (!div || div > 31u) return;
    value = APB1_SYSCTRL->CguCfg[1] & ~((1u << 5) | (1u << 11) | (7u << 29) | 31u);
    APB1_SYSCTRL->CguCfg[1] = value | div | (1u << 5);
}

uint8_t SYSCTRL_GetPClkDiv(void)
{
    return (uint8_t)get_safe_divider(APB1_SYSCTRL->CguCfg[1], 0, 5);
}

uint32_t SYSCTRL_GetPClk(void)
{
    return SYSCTRL_GetHClk() / SYSCTRL_GetPClkDiv();
}

uint32_t SYSCTRL_GetPClk2(void)
{
    return SYSCTRL_GetHClk() / get_safe_divider(APB1_SYSCTRL->CguCfg[0], 5, 5);
}

void SYSCTRL_SelectSlowClk(SYSCTRL_SlowClkMode mode)
{
    if ((unsigned int)mode <= 1u)
        set_reg_bit((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), (uint8_t)mode, 8);
}

int SYSCTRL_GetCLK32k(void) { return 32768; }
int SYSCTRL_GetCPU32k(void) { return SYSCTRL_GetCLK32k(); }

void SYSCTRL_SetAdcClkDiv(uint8_t denom)
{
    uint32_t value;
    if (denom == 0 || denom > 63u)
        return;
    value = APB1_SYSCTRL->CguCfg8;
    /* Do not replay update pulses for unrelated dividers in this register. */
    value &= ~((1u << 12) | (1u << 13) | (1u << 16) | (1u << 25) | (1u << 31) | 0x3fu);
    APB1_SYSCTRL->CguCfg8 = value | denom | (1u << 13);
}

uint32_t SYSCTRL_GetAdcClkDiv(void)
{
    uint32_t denom = APB1_SYSCTRL->CguCfg8 & 0x3fu;
    return denom ? SYSCTRL_GetSlowClk() / denom : 0;
}

uint32_t SYSCTRL_GetClk(SYSCTRL_Item item)
{
    uint32_t cfg1 = APB1_SYSCTRL->CguCfg[1], cfg8 = APB1_SYSCTRL->CguCfg8;
    uint32_t slow = SYSCTRL_GetSlowClk();
    uint32_t source;
    switch (item)
    {
    case SYSCTRL_ITEM_APB1_ADC: return SYSCTRL_GetAdcClkDiv();
    case SYSCTRL_ITEM_APB1_TMR0:
    case SYSCTRL_ITEM_APB1_TMR1:
        if (cfg1 & (1u << (item == SYSCTRL_ITEM_APB1_TMR0 ? 15 : 17)))
            return SYSCTRL_GetCLK32k();
        source = (cfg1 & (1u << 16)) ? ing26_per_clk() : slow;
        return source / get_safe_divider(cfg8, 17, 8);
    case SYSCTRL_ITEM_APB1_RTIMER2:
    case SYSCTRL_ITEM_APB1_RTIMER3:
        source = (cfg1 >> 12) & 3u;
        if (source >= 2) return SYSCTRL_GetCLK32k();
        return (source ? SYSCTRL_GetFastPreCLK() : slow)
             / get_safe_divider(APB1_SYSCTRL->CguCfg[0], 15, 5);
    case SYSCTRL_ITEM_APB2_PWM:
        if (!(cfg1 & (1u << 23))) return SYSCTRL_GetCLK32k();
        return ((cfg8 & (1u << 15)) ? ing26_per_clk() : slow)
             / get_safe_divider(cfg8, 26, 5);
    case SYSCTRL_ITEM_APB2_KeyScan:
        if (cfg1 & (1u << 14)) return SYSCTRL_GetCLK32k();
        return slow / get_safe_divider(APB1_SYSCTRL->CguCfg[0], 25, 5);
    case SYSCTRL_ITEM_APB2_SPI0:
        return ((cfg1 & (1u << 21)) ? ing26_per_clk() : slow)
             / get_safe_divider(APB1_SYSCTRL->CguCfg[0], 20, 5);
    case SYSCTRL_ITEM_APB1_SPI2:
        return ((cfg1 & (1u << 18)) ? ing26_per_clk() : slow)
             / get_safe_divider(APB1_SYSCTRL->CguCfg[0], 0, 5);
    case SYSCTRL_ITEM_APB1_SPI1:
        return (cfg1 & (1u << 22)) ? SYSCTRL_GetFastPreCLK() : slow;
    case SYSCTRL_ITEM_APB1_UART0:
    case SYSCTRL_ITEM_APB1_UART1:
        return (cfg1 & (1u << (item == SYSCTRL_ITEM_APB1_UART0 ? 19 : 20)))
             ? SYSCTRL_GetFastPreCLK() : slow;
    case SYSCTRL_ITEM_APB2_QDEC:
        return ((APB1_SYSCTRL->QdecCfg & (1u << 15)) ? SYSCTRL_GetFastPreCLK() : slow)
             / get_safe_divider(APB1_SYSCTRL->QdecCfg, 1, 10);
    case SYSCTRL_ITEM_APB2_I2S:
        return (cfg1 & (1u << 6))
             ? ing26_per_clk() / get_safe_divider(cfg8, 6, 6) : slow;
    case SYSCTRL_ITEM_APB2_ASDM:
        source = (io_read(APB1_SYSCTRL_BASE + 0x21c) & (1u << 20))
               ? ing26_pll_div_clk() : SYSCTRL_GetPLLClk();
        return source / get_safe_divider(io_read(APB1_SYSCTRL_BASE + 0x21c), 5, 6);
    case SYSCTRL_ITEM_APB1_WDT:
    case SYSCTRL_ITEM_APB1_GPIO0:
    case SYSCTRL_ITEM_APB1_GPIO1:
        return SYSCTRL_GetCLK32k();
    case SYSCTRL_ITEM_AHB_LPUART:
    case SYSCTRL_ITEM_AHB_LPTIMER:
    case SYSCTRL_ITEM_AHB_LPQDEC:
        source = io_read(AON2_CTRL_BASE + 0x2c);
        source = (source >> (item == SYSCTRL_ITEM_AHB_LPUART ? 0 : item == SYSCTRL_ITEM_AHB_LPTIMER ? 2 : 24)) & 3u;
        return source == 0 ? (uint32_t)SYSCTRL_GetCLK32k() : source == 1 ? slow : SYSCTRL_GetHClk() / get_safe_divider(APB1_SYSCTRL->CguCfg[0], 10, 5);
    case SYSCTRL_ITEM_AHB_USB:
        if (io_read(AON2_CTRL_BASE + 0x54) & (1u << 10)) return SYSCTRL_GetCLK32k();
        source = io_read(AON2_CTRL_BASE + 0x2c);
        switch (source >> 30)
        {
        case 0: source = slow; break;
        case 1: source = SYSCTRL_RC_FAST_HZ; break;
        case 2: source = SYSCTRL_GetPLLClk(); break;
        default: source = ing26_pll_div_clk(); break;
        }
        return source / get_safe_divider(io_read(AON2_CTRL_BASE + 0x2c), 26, 4);
    case SYSCTRL_ITEM_APB1_HUART0:
    case SYSCTRL_ITEM_APB1_HUART1:
    case SYSCTRL_ITEM_APB2_HUART2:
        return SYSCTRL_GetPClk2();
    default:
        return (unsigned int)item < SYSCTRL_ITEM_NUMBER ? SYSCTRL_GetPClk() : 0;
    }
}

void SYSCTRL_SelectMemoryBlocks(uint32_t block_map)
{
    uint32_t masked = block_map & 0x3f;
    set_reg_bits((volatile uint32_t *)(AON2_CTRL_BASE + 0x04), masked, 6, 12);
}

int SYSCTRL_Init(void)
{
    // TODO:
    return 0;
}

void SYSCTRL_SelectI2sClk(SYSCTRL_ClkMode mode)
{
    uint32_t value;
    if ((unsigned int)mode > 15u) return;
    if (mode)
    {
        value = APB1_SYSCTRL->CguCfg8 & ~((1u << 12) | (1u << 13) | (1u << 16)
              | (1u << 25) | (1u << 31) | (63u << 6));
        APB1_SYSCTRL->CguCfg8 = value | ((uint32_t)mode << 6);
    }
    value = APB1_SYSCTRL->CguCfg[1] & ~((1u << 5) | (1u << 11) | (7u << 29) | (1u << 6));
    APB1_SYSCTRL->CguCfg[1] = value | (mode ? (1u << 6) : 0u) | (1u << 11);
}

void SYSCTRL_SelectLpTimerClk(SYSCTRL_LpClkMode mode)
{
    if ((unsigned int)mode <= SYSCTRL_PCLK)
        set_reg_bits((volatile uint32_t *)(AON2_CTRL_BASE + 0x2c), mode, 2, 2);
}

void SYSCTRL_SelectLpUartClk(SYSCTRL_LpClkMode mode)
{
    if ((unsigned int)mode <= SYSCTRL_PCLK)
        set_reg_bits((volatile uint32_t *)(AON2_CTRL_BASE + 0x2c), mode, 2, 0);
}

void SYSCTRL_SelectLpQdecClk(SYSCTRL_LpClkMode mode)
{
    if ((unsigned int)mode <= SYSCTRL_PCLK)
        set_reg_bits((volatile uint32_t *)(AON2_CTRL_BASE + 0x2c), mode, 2, 24);
}

void SYSCTRL_UpdateAsdmClk(uint32_t div)
{
    uint32_t value;
    if (!div || div > 63u) return;
    value = io_read(APB1_SYSCTRL_BASE + 0x21c) & ~((63u << 5) | (1u << 11) | (1u << 28));
    io_write(APB1_SYSCTRL_BASE + 0x21c, value | (div << 5) | (1u << 11));
}

void SYSCTRL_SelectUSBClk(SYSCTRL_ClkMode mode)
{
    uint32_t value;
    if ((unsigned int)mode > 15u) return;
    value = io_read(AON2_CTRL_BASE + 0x2c) & ~((3u << 30) | (15u << 26) | (1u << 4));
    if (mode) value |= ((uint32_t)mode << 26) | (2u << 30);
    else value |= 1u << 26;
    io_write(AON2_CTRL_BASE + 0x2c, value);
    io_write(AON2_CTRL_BASE + 0x2c, value | (1u << 4));
}

void SYSCTRL_USBPhyConfig(uint8_t enable, uint8_t pull_sel)
{
    if(enable)
    {
        /* PHY[6:4] maps to AON1[23:21]; retain this family's drive code 4. */
        set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE), 1ul | (1 << pull_sel) | (4 << 4), 9, 17);
    }
    else
    {
        set_reg_bits((volatile uint32_t *)(AON1_CTRL_BASE), 0, 9, 17);
    }
}
void SYSCTRL_ResetAllBlocks(void)
{
    unsigned int i;
    for (i = 0; i < SYSCTRL_ITEM_NUMBER; ++i)
        if (i != SYSCTRL_ITEM_APB1_SysCtrl)
            SYSCTRL_ResetBlock((SYSCTRL_ResetItem)i);
    SYSCTRL_ResetBlock(SYSCTRL_ITEM_APB1_SysCtrl);
}

void SYSCTRL_SelectPWMClk(SYSCTRL_ClkMode mode)
{
    uint32_t value;
    if ((unsigned int)mode > 15u) return;
    value = APB1_SYSCTRL->CguCfg8 & ~((1u << 12) | (1u << 13) | (1u << 16)
          | (1u << 25) | (1u << 31) | (31u << 26) | (1u << 15));
    if (mode) value |= (uint32_t)mode << 26;
    APB1_SYSCTRL->CguCfg8 = value | (1u << 31);
    set_reg_bit(APB1_SYSCTRL->CguCfg + 1, !!mode, 23);
}

void SYSCTRL_SelectKeyScanClk(SYSCTRL_ClkMode mode)
{
    uint32_t value;
    if ((unsigned int)mode > 15u) return;
    value = APB1_SYSCTRL->CguCfg[0] & ~((3u << 30) | (31u << 25));
    APB1_SYSCTRL->CguCfg[0] = value | ((mode ? (uint32_t)mode : 1u) << 25);
    value = APB1_SYSCTRL->CguCfg[1] & ~((1u << 5) | (1u << 11) | (7u << 29) | (1u << 14));
    APB1_SYSCTRL->CguCfg[1] = value | (mode ? 0u : (1u << 14)) | (1u << 31);
}

void SYSCTRL_SelectTypeAClk(SYSCTRL_Item item, SYSCTRL_ClkMode mode)
{
    /* ING926 SAR ADC always uses its dedicated divider; no legacy ADC mux. */
    if (item == SYSCTRL_ITEM_APB1_ADC && mode == SYSCTRL_CLK_SLOW)
        SYSCTRL_SetAdcClkDiv(1);
}

#endif

void SYSCTRL_DelayCycles(uint32_t freq, uint32_t cycles)
{
    uint32_t sys = SYSCTRL_GetHClk();
    uint32_t cnt = (uint64_t)sys * cycles / freq;
    cnt = (cnt + 2) / 3;    // 3 instructions for the loop at minimum
    if (cnt < 1) return;
    while (--cnt) __NOP();
}
