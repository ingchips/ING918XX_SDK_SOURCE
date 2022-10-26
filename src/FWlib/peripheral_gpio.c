#include "peripheral_gpio.h"

static void GIO_MaskedWrite(volatile uint32_t *reg, const uint8_t index, const uint8_t v)
{
    uint32_t mask = 1 << index;
    *reg = (*reg & (~mask)) | (v << index);
}

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define GPIO_PIN_IE0  ((__IO uint32_t *)(APB_PINC_BASE + 0x00))
#define GPIO_PIN_PE0  ((__IO uint32_t *)(APB_PINC_BASE + 0x10))
#define GPIO_PIN_PS0  ((__IO uint32_t *)(APB_PINC_BASE + 0x18))
#define GPIO_PIN_DS00 ((__IO uint32_t *)(APB_PINC_BASE + 0x28))
#define GPIO_PIN_DS01 ((__IO uint32_t *)(APB_PINC_BASE + 0x2c))

void GIO_SetDirection(const GIO_Index_t io_index, const GIO_Direction_t dir)
{
    if (dir != GIO_DIR_NONE)
    {
        GIO_MaskedWrite(GPIO_OEB, io_index, dir == GIO_DIR_INPUT ? 0 : 1);
        GIO_MaskedWrite(GPIO_PIN_IE0, io_index, dir == GIO_DIR_OUTPUT ? 1 : 0);
    }
    else
    {
        GIO_MaskedWrite(GPIO_OEB, io_index, 0);
        GIO_MaskedWrite(GPIO_PIN_IE0, io_index, 1);
    }
}

GIO_Direction_t GIO_GetDirection(const GIO_Index_t io_index)
{
    return (GIO_Direction_t)((*GPIO_OEB >> io_index) & 1);
}

void GIO_WriteValue(const GIO_Index_t io_index, const uint8_t bit)
{
    GIO_MaskedWrite(GPIO_DO, io_index, bit);
}

uint8_t GIO_ReadValue(const GIO_Index_t io_index)
{
    return (*GPIO_DI >> io_index) & 1;
}

void GIO_ConfigIntSource(const GIO_Index_t io_index, const uint8_t enable, const GIO_IntTriggerType_t type)
{
    GIO_MaskedWrite(GPIO_LV, io_index, type);
    GIO_MaskedWrite(GPIO_NE, io_index, enable & 1);
    GIO_MaskedWrite(GPIO_PE, io_index, enable >> 1);
}

uint8_t GIO_GetIntStatus(const GIO_Index_t io_index)
{
    return (*GPIO_IS >> io_index) & 1;
}

void GIO_ClearIntStatus(const GIO_Index_t io_index)
{
    GIO_MaskedWrite(GPIO_IS, io_index, 0);
}


void GIO_ClearBits(void)
{
    uint32_t tmp1 = (*GPIO_PIN_LED)|0x1;
    uint32_t tmp2 = (*GPIO_PIN_LED)&(~0x1);

    (*GPIO_PIN_LED) = tmp1;
    (*GPIO_PIN_LED) = tmp2;
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define DEF_GIO_AND_PIN(io_index)       GIO_TypeDef *pDef = io_index >= GIO_GPIO_18 ? APB_GPIO1 : APB_GPIO0; int index = io_index >= GIO_GPIO_18 ? io_index - GIO_GPIO_18 : io_index;

void GIO_SetDirection(const GIO_Index_t io_index, const GIO_Direction_t dir)
{
    DEF_GIO_AND_PIN(io_index);
    if (dir != GIO_DIR_NONE)
    {
        GIO_MaskedWrite(&pDef->ChDir, index, dir == GIO_DIR_INPUT ? 0 : 1);
        GIO_MaskedWrite(&pDef->IOIE, index, dir == GIO_DIR_INPUT ? 1 : 0);
    }
    else
    {
        GIO_MaskedWrite(&pDef->ChDir, index, 0);
        GIO_MaskedWrite(&pDef->IOIE, index, 0);
    }
}

GIO_Direction_t GIO_GetDirection(const GIO_Index_t io_index)
{
    DEF_GIO_AND_PIN(io_index);
    GIO_Direction_t r = pDef->ChDir & (1 << index) ? GIO_DIR_OUTPUT : GIO_DIR_INPUT;
    if ((GIO_DIR_INPUT == r) && ((pDef->IOIE & (1 << index)) == 0))
        r = GIO_DIR_NONE;
    return r;
}

void GIO_WriteValue(const GIO_Index_t io_index, const uint8_t bit)
{
    DEF_GIO_AND_PIN(io_index);
    if (bit)
        pDef->DoutSet |= 1 << index;
    else
        pDef->DoutClear |= 1 << index;
}

uint8_t GIO_ReadValue(const GIO_Index_t io_index)
{
    DEF_GIO_AND_PIN(io_index);
    return (pDef->DataIn >> index) & 1;
}

static uint8_t map_int_mode(const uint8_t enable, const GIO_IntTriggerType_t type)
{
    switch (type)
    {
    case GIO_INT_EDGE:
        switch (enable)
        {
        case GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE:
            return 5;
        case GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE:
            return 6;
        case GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE:
            return 7;
        }
        break;

    case GIO_INT_LOGIC:
        switch (enable)
        {
        case GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE:
            return 3;
        case GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE:
            return 2;
        }
        break;
    }

    return 0;
}

void GIO_ConfigIntSource(const GIO_Index_t io_index, const uint8_t enable, const GIO_IntTriggerType_t type)
{
    DEF_GIO_AND_PIN(io_index);
    uint8_t mode = map_int_mode(enable, type);
    if (0 == mode)
    {
        GIO_MaskedWrite(&pDef->IntEn, index, 0);
        return;
    }
    int reg_index = index / 8;
    int offset = (index & 0x7) * 4;
    pDef->IntMod[reg_index] &= ~(0x7 << offset);
    pDef->IntMod[reg_index] |= mode << offset;
    GIO_MaskedWrite(&pDef->IntEn, index, 1);
}

uint8_t GIO_GetIntStatus(const GIO_Index_t io_index)
{
    DEF_GIO_AND_PIN(io_index);
    return (pDef->IntStatus >> index) & 1;
}

void GIO_ClearIntStatus(const GIO_Index_t io_index)
{
    DEF_GIO_AND_PIN(io_index);
    pDef->IntStatus |= 1 << index;
}

void GIO_DebounceCtrl(uint8_t group_mask, uint8_t clk_pre_scale, GIO_DbClk_t clk)
{
    uint32_t value = ((uint32_t)clk) << 31 | clk_pre_scale;
    if (group_mask & 1) APB_GPIO0->DeBounceCtrl = value;
    if (group_mask & 2) APB_GPIO1->DeBounceCtrl = value;
}

void GIO_DebounceEn(const GIO_Index_t io_index, uint8_t enable)
{
    DEF_GIO_AND_PIN(io_index);
    GIO_MaskedWrite(&pDef->DeBounceEn, index, enable);
}

#define MASK_18_BITS ((1 << 18) - 1)

void GIO_WriteAll(const uint64_t value)
{
    APB_GPIO0->DataOut = value & MASK_18_BITS;
    APB_GPIO1->DataOut = (value >> 18) & MASK_18_BITS;
}

uint64_t GIO_ReadAll(void)
{
    return (((uint64_t)APB_GPIO1->DataIn) << 18) | APB_GPIO0->DataIn;
}

uint64_t GIO_GetAllIntStatus(void)
{
    return (((uint64_t)APB_GPIO1->IntStatus) << 18) | APB_GPIO0->IntStatus;
}

void GIO_ClearAllIntStatus(void)
{
    APB_GPIO0->IntStatus = (uint32_t)-1;
    APB_GPIO1->IntStatus = (uint32_t)-1;
}

#endif
