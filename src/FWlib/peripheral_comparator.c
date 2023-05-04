/*
** COPYRIGHT (c) 2023 by INGCHIPS
*/

#include "peripheral_comparator.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

static void COMPARATOR_reg_write_bits(volatile uint32_t *reg, uint8_t offset, uint8_t bits_width, const uint32_t data)
{
    uint32_t mask;

    mask = 0xFFFFFFFF;
    mask = mask >> (32 - bits_width);

    *reg = (*reg & (~(mask << offset))) | ((data & mask) << offset);

    return;
}

static uint32_t COMPARATOR_reg_read_bits(volatile uint32_t *reg, uint8_t offset, uint8_t bits_width)
{
    uint32_t data;
    uint32_t mask;

    mask = 0xFFFFFFFF;
    mask = mask >> (32 - bits_width);

    data = ((*reg) >> offset) & mask;
    return data;
}

#define AON2_CTRL_COMPARATOR_CONFIG (AON2_CTRL_BASE + 0x178)

void COMPARATOR_SetComparatorConfig(uint16_t config)
{
    uint8_t offset = 0;
    uint8_t bits_width = 16;

    COMPARATOR_reg_write_bits((volatile uint32_t *)AON2_CTRL_COMPARATOR_CONFIG, offset, bits_width, config);
    return;
}

uint8_t COMPARATOR_GetComparatorResult(void)
{
    uint8_t offset = 16;
    uint8_t bits_width = 1;
    uint8_t ret;

    ret = COMPARATOR_reg_read_bits((volatile uint32_t *)AON2_CTRL_COMPARATOR_CONFIG, offset, bits_width);
    return ret;
}

#define COMPARATOR_VINN_RESERVED_CHANNEL 0xff
#define COMPARATOR_VINN_IN_CHIP_VREF     0xf1

const static uint8_t vinp_map_to_gpio[COMPARATOR_VINP_NUMBER] =
{
    GIO_GPIO_4,
    GIO_GPIO_18,
    GIO_GPIO_20,
    GIO_GPIO_26,
    GIO_GPIO_27,
    GIO_GPIO_28,
    GIO_GPIO_1,
    GIO_GPIO_2,
};

const static uint8_t vinn_map_to_gpio[COMPARATOR_VINN_NUMBER] =
{
    GIO_GPIO_3,
    GIO_GPIO_19,
    GIO_GPIO_32,
    GIO_GPIO_33,
    GIO_GPIO_15,
    COMPARATOR_VINN_RESERVED_CHANNEL,
    COMPARATOR_VINN_RESERVED_CHANNEL,
    COMPARATOR_VINN_IN_CHIP_VREF,
};

void COMPARATOR_Initialize(const COMPARATOR_SetStateStruct* cmp_set)
{
    uint16_t cmp_config;

    if (cmp_set == 0) {
        return;
    }

    COMPARATOR_InitializePins(cmp_set);

    cmp_config = ((cmp_set->en              << 0 ) |
                  (cmp_set->v_in_p          << 1 ) |
                  (cmp_set->hysteresis      << 4 ) |
                  (cmp_set->vinn_division   << 5 ) |
                  (cmp_set->work_mode       << 9)  |
                  (cmp_set->output_polarity << 11) |
                  (cmp_set->v_in_n          << 12));
    COMPARATOR_SetComparatorConfig(cmp_config);

    return;
}

void COMPARATOR_InitializePins(const COMPARATOR_SetStateStruct* cmp_set)
{
    uint8_t vinp_gpio;
    uint8_t vinn_gpio;

    vinp_gpio = vinp_map_to_gpio[cmp_set->v_in_p];
    vinn_gpio = vinn_map_to_gpio[cmp_set->v_in_n];

    if (vinp_gpio < GIO_GPIO_NUMBER) {
        PINCTRL_EnableAnalog(vinp_gpio);
    }

    if (vinn_gpio < GIO_GPIO_NUMBER) {
        PINCTRL_EnableAnalog(vinn_gpio);
    }

    return;
}

#define AON2_CTRL_SYS_WAKEUP_POL1  (AON2_CTRL_BASE + 0x124)
#define AON2_CTRL_SYS_WAKEUP_EN1   (AON2_CTRL_BASE + 0x12C)
#define AON2_CTRL_SYS_WAKEUP_MODE2 (AON2_CTRL_BASE + 0x138)

int COMPARATOR_SetDeepSleepWakeupSource(uint8_t enable, COMPARATOR_LEVEL_WAKEUP_t level, COMPARATOR_WAKEUP_MODE_t wakeup_mode)
{
    uint8_t offset;
    uint8_t bits_width;
    uint32_t en;

    offset = 8;
    bits_width = 1;
    COMPARATOR_reg_write_bits((volatile uint32_t *)AON2_CTRL_SYS_WAKEUP_POL1, offset, bits_width, level);

    offset = 8;
    bits_width = 1;
    en = (enable == 0) ? 0: 1;
    COMPARATOR_reg_write_bits((volatile uint32_t *)AON2_CTRL_SYS_WAKEUP_EN1, offset, bits_width, en);

    offset = 16;
    bits_width = 2;
    COMPARATOR_reg_write_bits((volatile uint32_t *)AON2_CTRL_SYS_WAKEUP_MODE2, offset, bits_width, wakeup_mode);

    return 0;
}

#define SYSCTRL_ANA_CTRL_W1C_BITS 10 // 10 ~ 16 bits are interrupt flag, write 1 clear!!!
int COMPARATOR_SetInterrupt(COMPARATOR_INTERRUPT_MODE_t int_mode)
{
    uint8_t offset = 6;
    uint8_t bits_width = 2;
    uint32_t reg_data;

    reg_data = COMPARATOR_reg_read_bits(&APB_SYSCTRL->AnaCtrl, 0, SYSCTRL_ANA_CTRL_W1C_BITS);
    COMPARATOR_reg_write_bits(&reg_data, offset, bits_width, int_mode);
    COMPARATOR_reg_write_bits(&APB_SYSCTRL->AnaCtrl, 0, 32, reg_data);
    return 0;
}

COMPARATOR_INTERRUPT_MODE_t COMPARATOR_GetIntStatusAndClear(void)
{
    uint8_t offset = 13;
    uint8_t bits_width = 2;
    COMPARATOR_INTERRUPT_MODE_t ret;
    uint32_t reg_data;

    ret = COMPARATOR_reg_read_bits(&APB_SYSCTRL->AnaCtrl, offset, bits_width);
    reg_data = COMPARATOR_reg_read_bits(&APB_SYSCTRL->AnaCtrl, 0, SYSCTRL_ANA_CTRL_W1C_BITS);
    COMPARATOR_reg_write_bits(&reg_data, offset, bits_width, ret);
    COMPARATOR_reg_write_bits(&APB_SYSCTRL->AnaCtrl, 0, 32, reg_data);
    return ret;
}

#endif

