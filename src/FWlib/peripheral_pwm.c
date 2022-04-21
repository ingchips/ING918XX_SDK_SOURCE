#include "peripheral_pwm.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

static void PWM_SetRegBit(uint32_t addr_offset, const uint8_t channel_index, const uint8_t v, const uint8_t bit_width)
{
    volatile uint32_t *reg = (volatile uint32_t *)(PWM_REG_BASE + addr_offset);
    uint32_t mask = ((1 << bit_width) - 1) << (channel_index * bit_width);
    *reg = (*reg & ~mask) | (v << (channel_index * bit_width));
}

void PWM_Enable(const uint8_t channel_index, const uint8_t enable)
{
    PWM_SetRegBit(0x0C, channel_index, enable, 1);
}

void PWM_EnableMulti(uint32_t channel_mask, const uint8_t enable)
{
    volatile uint32_t *reg = (volatile uint32_t *)(PWM_REG_BASE + 0x0C);
    if (enable)
        *reg |= channel_mask;
    else
        *reg &= ~channel_mask;
}

// Note: when mask = 1, output is masked as 0.
void PWM_SetMask(const uint8_t channel_index, const uint8_t mask_a, const uint8_t mask_b)
{
    PWM_SetRegBit(0x00, channel_index, (mask_b << 1) | mask_a, 2);
}

void PWM_SetMode(const uint8_t channel_index, const PWM_WordMode_t mode)
{
    PWM_SetRegBit(0x14, channel_index, mode, 3);
}

// PMW Halt Mode
// When the register HALT_ENABLE is set to 1, PWM will controlled by HALT_CONFIG registers.
// The PWM_OUT_A will output the value of HALT_CONFIG[0], and the PWM_OUT_B will output the value of HALT_CONFIG[1]
void PWM_HaltCtrlEnable(const uint8_t channel_index, const uint8_t enable)
{
    PWM_SetRegBit(0x04, channel_index, enable, 1);
}

void PWM_HaltCtrlCfg(const uint8_t channel_index, const uint8_t out_a, const uint8_t out_b)
{
    PWM_SetRegBit(0x08, channel_index, (out_b << 1) | out_a, 2);
}

// The PWM waveform can be set to multi-duty-cycle if it's not in single step mode or halt mode
// comp_num is in [0..3], which means [1..4] duty-cycles are used (\ref PWM_SetHighThreshold)
void PWM_SetMultiDutyCycleCtrl(const uint8_t channel_index, const uint8_t comp_num)
{
    PWM_SetRegBit(0x10, channel_index, comp_num, 2);
}

#define REG_GROUP_THRESHOLD(index) ((volatile uint32_t *)(PWM_REG_BASE + 0x20 + 0x40 * channel_index))

void PWM_SetPeraThreshold(const uint8_t channel_index, const uint32_t threshold)
{
    volatile uint32_t *reg = REG_GROUP_THRESHOLD(channel_index);
    *reg = threshold;
}

void PWM_SetDiedZoneThreshold(const uint8_t channel_index, const uint32_t threshold)
{
    volatile uint32_t *reg = REG_GROUP_THRESHOLD(channel_index) + 1;
    *reg = threshold;
}

void PWM_SetHighThreshold(const uint8_t channel_index, const uint8_t multi_duty_index, const uint32_t threshold)
{
    volatile uint32_t *reg = REG_GROUP_THRESHOLD(channel_index) + 2 + multi_duty_index;
    *reg = threshold;
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

static void PWM_SetRegBit(uint32_t addr_offset, const uint8_t offset, const uint8_t v, const uint8_t bit_width)
{
    volatile uint32_t *reg = (volatile uint32_t *)(APB_PWM_BASE + addr_offset);
    uint32_t mask = ((1 << bit_width) - 1) << offset;
    *reg = (*reg & ~mask) | (v << offset);
}

void PWM_Enable(const uint8_t channel_index, const uint8_t enable)
{
    PWM_SetRegBit(0x00, 5, enable & 1, 1);
}

void PWM_SetMask(const uint8_t channel_index, const uint8_t mask_a, const uint8_t mask_b)
{
    PWM_SetRegBit(0x00, 0, (mask_b << 1) | mask_a, 2);
}

void PWM_SetInvertOutput(const uint8_t channel_index, const uint8_t inv_a, const uint8_t inv_b)
{
    PWM_SetRegBit(0x00, 9, (inv_b << 1) | inv_a, 2);
}

void PWM_SetMode(const uint8_t channel_index, const PWM_WordMode_t mode)
{
    PWM_SetRegBit(0x00, 6, mode, 3);
}

void PWM_HaltCtrlEnable(const uint8_t channel_index, const uint8_t enable)
{
    PWM_SetRegBit(0x00, 2, enable, 1);
}

void PWM_HaltCtrlCfg(const uint8_t channel_index, const uint8_t out_a, const uint8_t out_b)
{
    PWM_SetRegBit(0x00, 3, (out_b << 1) | out_a, 2);
}

void PWM_SetPeraThreshold(const uint8_t channel_index, const uint32_t threshold)
{
    APB_PWM->PeraTh = threshold;
}

void PWM_SetDiedZoneThreshold(const uint8_t channel_index, const uint32_t threshold)
{
    APB_PWM->DZoneTh = threshold;
}

void PWM_SetHighThreshold(const uint8_t channel_index, const uint8_t multi_duty_index, const uint32_t threshold)
{
    APB_PWM->HighTh = threshold;
}

void PWM_DmaEnable(const uint8_t channel_index, uint8_t trig_cfg, uint8_t enable)
{
    uint32_t mask = APB_PWM->Ctrl0 & ~(0x1ful << 18);
    mask |= (enable ? 0x3 : 0x0) << 18;
    mask |= (trig_cfg & 0x7) << 20;
    APB_PWM->Ctrl0 = mask;
}

#endif

void PWM_SetupSimple(const uint8_t channel_index, const uint32_t frequency, const uint16_t on_duty)
{
    uint32_t pera = PWM_CLOCK_FREQ / frequency;
    uint32_t high = pera > 1000 ? pera / 100 * (100 - on_duty) : pera * (100 - on_duty) / 100;
    PWM_HaltCtrlEnable(channel_index, 1);
    PWM_Enable(channel_index, 0);
    PWM_SetPeraThreshold(channel_index, pera);
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PWM_SetMultiDutyCycleCtrl(channel_index, 0);        // do not use multi duty cycles
#endif
    PWM_SetHighThreshold(channel_index, 0, high);
    PWM_SetMode(channel_index, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
    PWM_SetMask(channel_index, 0, 0);
    PWM_Enable(channel_index, 1);
    PWM_HaltCtrlEnable(channel_index, 0);
}
