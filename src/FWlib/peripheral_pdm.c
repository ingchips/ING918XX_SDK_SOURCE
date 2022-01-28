#include "peripheral_pdm.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

void PDM_Config(PDM_TypeDef *base, uint8_t left_en, uint8_t right_en,
                uint8_t hpf_en,
                uint8_t i2s_slave_mode, uint8_t i2s_slave_bitwidth)
{
    base->Cfg &= ~((1 << 22) | (1 << 23) | (1 << 28) | (1 << 24) | (1 << 25));
    base->Cfg |= ((uint32_t)left_en << 22) | ((uint32_t)right_en << 23)
                 | ((uint32_t)hpf_en << 28)
                 | ((uint32_t)i2s_slave_mode << 24) | ((uint32_t)i2s_slave_bitwidth << 25);
}

void PDM_Start(PDM_TypeDef *base, uint8_t enable)
{
    if (enable)
        base->Cfg |= 1 << 29;
    else
        base->Cfg &= ~(1 << 29);
}

void PDM_DmaEnable(PDM_TypeDef *base, uint8_t trig_cfg, uint8_t enable)
{
    uint32_t mask = base->Cfg & ~(0x107);
    mask |= trig_cfg & 0x7;
    mask |= enable << 8;
    base->Cfg = mask;
}

#endif
