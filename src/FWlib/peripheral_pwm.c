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

void PWM_SetMode(const uint8_t channel_index, const PWM_WorkMode_t mode)
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

static void PWM_SetRegBit(const uint8_t channel_index, uint32_t addr_offset, const uint8_t offset, const uint8_t v, const uint8_t bit_width)
{
    volatile uint32_t *reg = (volatile uint32_t *)((uintptr_t)(&APB_PWM->Channels[channel_index]) + addr_offset);
    uint32_t mask = ((1 << bit_width) - 1) << offset;
    *reg = (*reg & ~mask) | (v << offset);
}

void PWM_Enable(const uint8_t channel_index, const uint8_t enable)
{
    PWM_SetRegBit(channel_index, 0x00, 6, enable & 1, 1);
}

void PWM_SetMask(const uint8_t channel_index, const uint8_t mask_a, const uint8_t mask_b)
{
    PWM_SetRegBit(channel_index, 0x00, 0, (mask_b << 1) | mask_a, 2);
}

void PWM_SetInvertOutput(const uint8_t channel_index, const uint8_t inv_a, const uint8_t inv_b)
{
    PWM_SetRegBit(channel_index, 0x00, 10, (inv_b << 1) | inv_a, 2);
}

void PWM_SetMode(const uint8_t channel_index, const PWM_WorkMode_t mode)
{
    PWM_SetRegBit(channel_index, 0x00, 7, mode, 3);
}

void PWM_HaltCtrlEnable2(const uint8_t channel_index, const uint8_t enable_a, const uint8_t enable_b)
{
    PWM_SetRegBit(channel_index, 0x00, 2, (enable_b << 1) | enable_a, 2);
}

void PWM_HaltCtrlEnable(const uint8_t channel_index, const uint8_t enable)
{
    PWM_HaltCtrlEnable2(channel_index, enable, enable);
}

void PWM_HaltCtrlCfg(const uint8_t channel_index, const uint8_t out_a, const uint8_t out_b)
{
    PWM_SetRegBit(channel_index, 0x00, 4, (out_b << 1) | out_a, 2);
}

void PWM_SetPeraThreshold(const uint8_t channel_index, const uint32_t threshold)
{
    APB_PWM->Channels[channel_index].PeraTh = threshold;
}

void PWM_SetDiedZoneThreshold(const uint8_t channel_index, const uint32_t threshold)
{
    APB_PWM->Channels[channel_index].DZoneTh = threshold;
}

void PWM_SetHighThreshold(const uint8_t channel_index, const uint8_t multi_duty_index, const uint32_t threshold)
{
    APB_PWM->Channels[channel_index].HighTh = threshold;
}

void PWM_DmaEnable(const uint8_t channel_index, uint8_t trig_cfg, uint8_t enable)
{
    uint32_t mask = APB_PWM->Channels[channel_index].Ctrl0 & ~(0x1ful << 20);
    mask |= (enable ? 0x1 : 0x0) << 20;
    mask |= (trig_cfg & 0x7) << 21;
    APB_PWM->Channels[channel_index].Ctrl0 = mask;
}

void PCAP_Enable(const uint8_t channel_index)
{
    PWM_SetMode(channel_index, (PWM_WorkMode_t)6);
    PWM_Enable(channel_index, 1);
}

void PCAP_EnableEvents(const uint8_t channel_index,
                       uint8_t events_on_0,
                       uint8_t events_on_1)
{
    APB_PWM->PCAPChannels[channel_index].Ctrl0 = events_on_0 | (events_on_1 << 2);
}

uint32_t PCAP_ReadData(const uint8_t channel_index)
{
    return APB_PWM->PCAPChannels[channel_index].Ctrl1;
}

void PCAP_CounterEnable(uint8_t enable)
{
    APB_PWM->CapCntEn = enable;
}

uint32_t PCAP_ReadCounter(void)
{
    return APB_PWM->CapCounter;
}

void PWM_FifoTriggerEnable(const uint8_t channel_index, uint8_t enable, uint32_t mask)
{
    if(enable)
    {
        APB_PWM->Channels[channel_index].Ctrl0 |= mask;
    }
    else
    {
        APB_PWM->Channels[channel_index].Ctrl0 &= ~(mask);
    }
}

uint32_t PWM_GetFifoStatus(const uint8_t channel_index)
{
    return(APB_PWM->Channels[channel_index].Ctrl1);
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

void PWM_SetupSingle(const uint8_t channel_index, const uint32_t pulse_width)
{
    uint32_t pera = PWM_CLOCK_FREQ / (1000000000 / pulse_width);
    PWM_Enable(channel_index, 0);
    PWM_SetPeraThreshold(channel_index, pera);
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PWM_SetMultiDutyCycleCtrl(channel_index, 0);        // do not use multi duty cycles
#endif
    PWM_SetHighThreshold(channel_index, 0, 0);
    PWM_SetMode(channel_index, PWM_WORK_MODE_SINGLE_WITHOUT_DIED_ZONE);
    PWM_SetMask(channel_index, 0, 0);
    PWM_Enable(channel_index, 1);
}
