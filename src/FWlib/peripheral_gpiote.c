#include "peripheral_gpiote.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
int GPIOTE_Config(GPIOTE_TypeDef *base, uint8_t channel, uint8_t mode, uint8_t pin, uint8_t initial)
{
    if (!base || channel >= 4 || pin >= 56 || (mode != 0 && mode != 1 && mode != 3)) return -1;
    base->reg_gpiote_config[channel] = mode | ((uint32_t)pin << 8) | ((uint32_t)(initial != 0) << 24);
    return 0;
}
uint32_t GPIOTE_GetConfig(GPIOTE_TypeDef *base, uint8_t channel)
{
    return (base && channel < 4) ? base->reg_gpiote_config[channel] : 0;
}

#endif
