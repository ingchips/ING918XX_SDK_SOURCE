#include "peripheral_trng.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
int TRNG_Config(TRNG_TypeDef *base, uint8_t rate, uint8_t post_process, uint32_t bit_select)
{
    if (!base || (bit_select & ~0x7ffffu)) return -1;
    base->Config = ((uint32_t)rate << 8) | ((uint32_t)(post_process != 0) << 1) | 8u;
    base->BitSel = bit_select;
    base->DataCtrl = 2u;
    return 0;
}
void TRNG_Enable(TRNG_TypeDef *base, uint8_t enable)
{
    if (enable) base->Config |= 1u; else base->Config &= ~1u;
}
void TRNG_IntEnable(TRNG_TypeDef *base, uint8_t enable)
{
    if (enable) base->Config &= ~8u; else base->Config |= 8u;
}
void TRNG_ClearInt(TRNG_TypeDef *base)
{
    base->Config |= 4u;
    base->Config &= ~4u;
}
int TRNG_Read(TRNG_TypeDef *base, uint32_t *data)
{
    if (!base || !data || (base->DataCtrl & 3u) != 2u) return -1;
    if (!(base->DataCtrl & 4u)) return 1;
    *data = base->Data;
    base->DataCtrl &= ~4u;
    return 0;
}

#endif
