
/*
** COPYRIGHT (c) 2023 by INGCHIPS
*/

#include "peripheral_keyscan.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define GROUP_A 0
#define GROUP_B 1
static uint8_t gpio_retention_group[GIO_GPIO_NUMBER] = {
    GROUP_A, // GIO_GPIO_0
    GROUP_B, // GIO_GPIO_1
    GROUP_B, // GIO_GPIO_2
    GROUP_B, // GIO_GPIO_3
    GROUP_B, // GIO_GPIO_4
    GROUP_A, // GIO_GPIO_5
    GROUP_A, // GIO_GPIO_6
    GROUP_B, // GIO_GPIO_7
    GROUP_B, // GIO_GPIO_8
    GROUP_B, // GIO_GPIO_9
    GROUP_B, // GIO_GPIO_10
    GROUP_B, // GIO_GPIO_11
    GROUP_B, // GIO_GPIO_12
    GROUP_B, // GIO_GPIO_13
    GROUP_B, // GIO_GPIO_14
    GROUP_B, // GIO_GPIO_15
    GROUP_B, // GIO_GPIO_16
    GROUP_B, // GIO_GPIO_17
    GROUP_B, // GIO_GPIO_18
    GROUP_B, // GIO_GPIO_19
    GROUP_B, // GIO_GPIO_20
    GROUP_A, // GIO_GPIO_21
    GROUP_A, // GIO_GPIO_22
    GROUP_A, // GIO_GPIO_23
    GROUP_B, // GIO_GPIO_24
    GROUP_B, // GIO_GPIO_25
    GROUP_B, // GIO_GPIO_26
    GROUP_B, // GIO_GPIO_27
    GROUP_B, // GIO_GPIO_28
    GROUP_B, // GIO_GPIO_29
    GROUP_B, // GIO_GPIO_30
    GROUP_B, // GIO_GPIO_31
    GROUP_B, // GIO_GPIO_32
    GROUP_B, // GIO_GPIO_33
    GROUP_B, // GIO_GPIO_34
    GROUP_B, // GIO_GPIO_35
    GROUP_A, // GIO_GPIO_36
    GROUP_A, // GIO_GPIO_37
    GROUP_B, // GIO_GPIO_38
    GROUP_B, // GIO_GPIO_39
    GROUP_B, // GIO_GPIO_40
    GROUP_B, // GIO_GPIO_41
};

static uint8_t keyscan_bsp_check_state_para(const KEYSCAN_SetStateStruct* keyscan_set)
{
    if (keyscan_set == 0) {
        return -1;
    }

    if ((keyscan_set->row == 0) || (keyscan_set->col == 0) || (keyscan_set->ctx == 0)) {
        return -1;
    }

    return 0;
}

void keyscan_bsp_enable_wakeup_before_sleep(const KEYSCAN_SetStateStruct* keyscan_set)
{
    int i;
    uint8_t enable_retention_group_a = 0;
    uint8_t enable_retention_group_b = 0;

    if (keyscan_bsp_check_state_para(keyscan_set) != 0) {
        return;
    }

    KEYSCAN_SetScannerEn(0);
    for (i = 0; i < keyscan_set->row_num; i++) {
        PINCTRL_SetPadMux(keyscan_set->row[i].gpio, IO_SOURCE_GPIO);
        GIO_SetDirection(keyscan_set->row[i].gpio, GIO_DIR_OUTPUT);
        GIO_WriteValue(keyscan_set->row[i].gpio, 1);
        if (gpio_retention_group[keyscan_set->row[i].gpio] == GROUP_A) {
            enable_retention_group_a = 1;
        } else {
            enable_retention_group_b = 1;
        }
    }

    for (i = 0; i < keyscan_set->col_num; i++) {
        PINCTRL_SetPadMux(keyscan_set->col[i].gpio, IO_SOURCE_GPIO);
        PINCTRL_Pull(keyscan_set->col[i].gpio, PINCTRL_PULL_DOWN);
        GIO_SetDirection(keyscan_set->col[i].gpio, GIO_DIR_INPUT);
        GIO_ConfigIntSource(keyscan_set->col[i].gpio, GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE, GIO_INT_EDGE);
        GIO_EnableDeepSleepWakeupSource(keyscan_set->col[i].gpio, 1, 1, PINCTRL_PULL_DOWN);
    }

    if (enable_retention_group_a) {
        GIO_EnableRetentionGroupA(1);
    }

    if (enable_retention_group_b) {
        GIO_EnableRetentionGroupB(1);
    }

    return;
}

void keyscan_bsp_disable_wakeup_after_sleep(const KEYSCAN_SetStateStruct* keyscan_set)
{
    int i;
    uint8_t disable_retention_group_a = 0;
    uint8_t disable_retention_group_b = 0;

    if (keyscan_bsp_check_state_para(keyscan_set) != 0) {
        return;
    }

    for (i = 0; i < keyscan_set->row_num; i++) {
        if (gpio_retention_group[keyscan_set->row[i].gpio] == GROUP_A) {
            disable_retention_group_a = 1;
        } else {
            disable_retention_group_b = 1;
        }
    }

    if (disable_retention_group_a) {
        GIO_EnableRetentionGroupA(0);
    }

    if (disable_retention_group_b) {
        GIO_EnableRetentionGroupB(0);
    }

    return;
}

#endif

