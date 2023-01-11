#include "peripheral_i2s.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

void I2S_Config(I2S_TypeDef *base, i2s_role_t role, i2s_mode_t mode,
                uint8_t mono,
                uint8_t lrclk_polar, uint8_t txbclk_polar, uint8_t rxbclk_polar,
                uint8_t data_width)
{
    uint32_t mask = 0x7 << 2;
    base->ModeConfig &= ~mask;
    base->ModeConfig |=  (mode << 2) | ((uint32_t)mono << 4);
    mask = 0x1f0f;
    base->Config &= ~mask;
    data_width = data_width <= 16 ? 0 : data_width - 1;
    base->Config |=   (role << 0)
                        | (lrclk_polar << 1)
                        | (txbclk_polar << 2)
                        | (rxbclk_polar << 3)
                        | ((uint32_t)data_width << 8);
}

void I2S_DMAEnable(I2S_TypeDef *base, uint8_t tx_en, uint8_t rx_en)
{
    base->Config &= ~(0x3 << 13);
    base->Config |= (tx_en << 13) | (rx_en << 14);
}

void I2S_Enable(I2S_TypeDef *base, uint8_t tx_en, uint8_t rx_en)
{
    uint32_t mask = 0x3;
    base->ModeConfig &= ~mask;
    base->ModeConfig |= (tx_en << 0) | (rx_en << 1);
}

void I2S_ConfigClk(I2S_TypeDef *base, uint8_t b_div, uint8_t lr_div)
{
    base->ClkDiv = ((uint32_t)(b_div - 1) << 8) | (lr_div - 1);
}

void I2S_ConfigIRQ(I2S_TypeDef *base, uint8_t tx_irq_en, uint8_t rx_irq_en,
                   uint8_t tx_irq_trigger_level, uint8_t rx_irq_trigger_level)
{
    uint32_t mask = 0x30;
    base->Config &= ~mask;
    base->Config |= (tx_irq_en << 4) | (rx_irq_en << 5);
    base->Trig = ((uint32_t)rx_irq_trigger_level << 8) | tx_irq_trigger_level;
}

uint32_t I2S_PopRxFIFO(I2S_TypeDef *base)
{
    return base->RX;
}

void I2S_PushTxFIFO(I2S_TypeDef *base, uint32_t value)
{
    base->TX = value;
}

void I2S_ClearRxFIFO(I2S_TypeDef *base)
{
    base->Config |= 1 << 7;
}

void I2S_ClearTxFIFO(I2S_TypeDef *base)
{
    base->Config |= 1 << 6;
}

uint32_t I2S_GetIntState(I2S_TypeDef *base)
{
    return base->Status & 0x3;
}

void I2S_ClearIntState(I2S_TypeDef *base, uint32_t state)
{
    base->Status = state;
}

int I2S_GetTxFIFOCount(I2S_TypeDef *base)
{
    return base->FifoStatus & 0x1f;
}

int I2S_GetRxFIFOCount(I2S_TypeDef *base)
{
    return (base->FifoStatus >> 8) & 0x1f;
}

void I2S_DataFromPDM(uint8_t enable)
{
    APB_SYSCTRL->PdmI2sCtrl = enable;
}

#endif
