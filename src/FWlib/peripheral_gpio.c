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

uint8_t GIO_ReadOutputValue(const GIO_Index_t io_index)
{
    return (*GPIO_DO >> io_index) & 1;
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

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define START_OF_GPIO1          GIO_GPIO_21
#define MASK_GPIO0_BITS         ((1 << 21) - 1)

#define DEF_GIO_AND_PIN(io_index)       GIO_TypeDef *pDef = io_index >= START_OF_GPIO1 ? APB_GPIO1 : APB_GPIO0; int index = io_index >= START_OF_GPIO1 ? io_index - START_OF_GPIO1 : io_index;

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

uint8_t GIO_ReadOutputValue(const GIO_Index_t io_index)
{
    DEF_GIO_AND_PIN(io_index);
    return (pDef->DataOut >> index) & 1;
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

void GIO_WriteAll(const uint64_t value)
{
    APB_GPIO0->DataOut = value & MASK_GPIO0_BITS;
    APB_GPIO1->DataOut = (value >> GIO_GPIO_21) & MASK_GPIO0_BITS;
}

uint64_t GIO_ReadAll(void)
{
    return (((uint64_t)APB_GPIO1->DataIn) << GIO_GPIO_21) | APB_GPIO0->DataIn;
}

uint64_t GIO_GetAllIntStatus(void)
{
    return (((uint64_t)APB_GPIO1->IntStatus) << GIO_GPIO_21) | APB_GPIO0->IntStatus;
}

void GIO_ClearAllIntStatus(void)
{
    APB_GPIO0->IntStatus = (uint32_t)-1;
    APB_GPIO1->IntStatus = (uint32_t)-1;
}

void GIO_EnableRetentionGroupA(uint8_t enable)
{
    #define AON1_REG4       (AON1_CTRL_BASE + 0x10)
    #define AON2_STATUS0    (AON2_CTRL_BASE + 0x100)

    GIO_MaskedWrite((volatile uint32_t *)AON1_REG4, 7, enable);
    while (((io_read(AON2_STATUS0) >> 19) & 1) != enable);
}

#define AON2_SLEEP_CTRL     (AON2_CTRL_BASE + 0x1A8)

#define nop(n) do { int i = n; while (i--) __NOP(); } while (0)

void GIO_EnableRetentionGroupB(uint8_t enable)
{
    if (enable)
    {
        GIO_MaskedWrite((volatile uint32_t *)AON2_SLEEP_CTRL, 14, 1);
        nop(10);
        GIO_MaskedWrite((volatile uint32_t *)AON2_SLEEP_CTRL, 15, 1);
        nop(10);
    }
    else
    {
        GIO_MaskedWrite((volatile uint32_t *)AON2_SLEEP_CTRL, 15, 0);
        nop(10);
        GIO_MaskedWrite((volatile uint32_t *)AON2_SLEEP_CTRL, 14, 0);
        nop(10);
    }
}

void GIO_EnableHighZGroupB(uint8_t enable)
{
    if (enable)
    {
        GIO_MaskedWrite((volatile uint32_t *)AON2_SLEEP_CTRL, 15, 1);
        nop(10);
        GIO_MaskedWrite((volatile uint32_t *)AON2_SLEEP_CTRL, 14, 1);
        nop(10);
    }
    else
    {
        GIO_MaskedWrite((volatile uint32_t *)AON2_SLEEP_CTRL, 14, 0);
        nop(10);
        GIO_MaskedWrite((volatile uint32_t *)AON2_SLEEP_CTRL, 15, 0);
        nop(10);
    }
}

int GIO_EnableDeepSleepWakeupSource(GIO_Index_t io_index, uint8_t enable,
        uint8_t level, pinctrl_pull_mode_t pull)
{
    if ((io_index <= 17)
     || (21 <= io_index) && (io_index <= 25)
     || (29 <= io_index) && (io_index <= 37))
    {
        if (io_index <= 31)
            GIO_MaskedWrite((volatile uint32_t *)(AON2_CTRL_BASE + 0x128), io_index, enable);
        else
            GIO_MaskedWrite((volatile uint32_t *)(AON2_CTRL_BASE + 0x12c), io_index - 32, enable);

        if (io_index <= 31)
            GIO_MaskedWrite((volatile uint32_t *)(AON2_CTRL_BASE + 0x120), io_index, level ? 0 : 1);
        else
            GIO_MaskedWrite((volatile uint32_t *)(AON2_CTRL_BASE + 0x124), io_index - 32, level ? 0 : 1);
    }
    else
        return -1;

    if ((1 <= io_index) && (io_index <= 4)
     || (7 <= io_index) && (io_index <= 17)
     || (29 <= io_index) && (io_index <= 35))
    {
        if (io_index <= 31)
            GIO_MaskedWrite((volatile uint32_t *)(AON2_CTRL_BASE + 0x60), io_index, enable);
        else
            GIO_MaskedWrite((volatile uint32_t *)(AON2_CTRL_BASE + 0x64), io_index - 32, enable);

        if (enable == 0) return 0;

        if (pull != PINCTRL_PULL_DISABLE)
        {
            if (io_index <= 31)
            {
                GIO_MaskedWrite((volatile uint32_t *)(AON2_CTRL_BASE + 0x68), io_index, 1);
                GIO_MaskedWrite((volatile uint32_t *)(AON2_CTRL_BASE + 0x70), io_index, pull == PINCTRL_PULL_UP ? 1 : 0);
            }
            else
            {
                GIO_MaskedWrite((volatile uint32_t *)(AON2_CTRL_BASE + 0x6c), io_index - 32, 1);
                GIO_MaskedWrite((volatile uint32_t *)(AON2_CTRL_BASE + 0x74), io_index - 32, pull == PINCTRL_PULL_UP ? 1 : 0);
            }
        }
    }
    return 0;
}

void GIO_EnableDeeperSleepWakeupSourceGroupA(uint8_t enable, uint8_t level)
{
    GIO_MaskedWrite((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 9, (level & 1) ^ 1);
    GIO_MaskedWrite((volatile uint32_t *)(AON1_CTRL_BASE + 0x10), 10, enable);
}

void GIO_SetBits(const uint64_t index_mask)
{
    APB_GPIO0->DoutSet |= index_mask & 0x1fffff;
    APB_GPIO1->DoutSet |= index_mask >> 21;
}

void GIO_ClearBits(const uint64_t index_mask)
{
    APB_GPIO0->DoutClear |= index_mask & 0x1fffff;
    APB_GPIO1->DoutClear |= index_mask >> 21;
}

void GIO_ToggleBits(const uint64_t index_mask)
{
    APB_GPIO0->DataOut ^= index_mask & 0x1fffff;
    APB_GPIO1->DataOut ^= index_mask >> 21;
}

#endif
