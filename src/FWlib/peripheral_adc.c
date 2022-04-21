#include "peripheral_adc.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define SET_BIT(V, n)       ((V) |  (1 << (n)))
#define CLEAR_BIT(V, n)     ((V) & ~(1 << (n)))
#define CHANGE_BIT(V, n, b) (CLEAR_BIT(V, n) | ((b) << (n)))

#define ADC_POW_MASK    0x3

#define ADC              ((ADC_Type          *)(SYSCTRL_BASE + 0x30))
#define ADC_Pwr_Ctrl     ((ADC_Pwr_Ctrl_Type *)(APB_RTC_BASE + 0x90))

uint8_t adc_get_power_state(void)
{
    return ((*(volatile uint32_t *)(APB_RTC_BASE + 0x88)) >> 3) & ADC_POW_MASK;
}

void ADC_PowerCtrl(const uint8_t flag)
{
#define ADC_PON         0x3
#define ADC_PON_STATE   ADC_PON

#define ADC_POFF              ((~ADC_PON) & ADC_POW_MASK)
#define ADC_POFF_STATE        ((~ADC_PON_STATE) & ADC_POW_MASK)

    if (flag)
    {
        ADC_Pwr_Ctrl->act = (ADC_Pwr_Ctrl->act & (~ADC_POW_MASK)) | ADC_PON;
        while (adc_get_power_state() != ADC_PON_STATE) ;
        ADC_Pwr_Ctrl->act = CLEAR_BIT(ADC_Pwr_Ctrl->act, 2);
        ADC->ctrl0 &= ~((0xff<<3) | (0x3<<11) | (0x3<<13) | (0x3<<26));
        ADC->ctrl0 |= (0x1<<11) | (0x1<<13) | (0x2<<26);    // ibias, iota, inres
    }
    else
    {
        ADC_Pwr_Ctrl->act = (ADC_Pwr_Ctrl->act & (~ADC_POW_MASK)) | ADC_POFF;
        while (adc_get_power_state() != ADC_POFF_STATE) ;
        ADC_Pwr_Ctrl->act = SET_BIT(ADC_Pwr_Ctrl->act, 2);
    }
}

void ADC_Enable(const uint8_t flag)
{
    ADC->ctrl0 = CHANGE_BIT(ADC->ctrl0, 15, flag);
}

void ADC_Reset()
{
    ADC->ctrl0 = CLEAR_BIT(ADC->ctrl0, 24);
    ADC->ctrl0 = SET_BIT(ADC->ctrl0, 24);
}

void ADC_SetClkSel(const uint8_t clk_sel)
{
    ADC->ctrl0 = (ADC->ctrl0 & ~0x7) | (clk_sel & 0x7);
}

void ADC_SetMode(const uint8_t mode)
{
    ADC->ctrl0 = CHANGE_BIT(ADC->ctrl0, 25, mode);
}

// the number of clock circles between two calculation at loop mode
void ADC_SetLoopDelay(const uint32_t delay)
{
    ADC->ctrl4 = delay;
}

void ADC_EnableChannel(const uint8_t channel_id, const uint8_t flag)
{
    ADC->ctrl0 = CHANGE_BIT(ADC->ctrl0, 3 + channel_id, flag);
}

void ADC_ClearChannelDataValid(const uint8_t channel_id)
{
    ADC->ctrl1 = SET_BIT(ADC->ctrl1, channel_id);
    while (ADC_IsChannelDataValid(channel_id)) ;
    ADC->ctrl1 = CLEAR_BIT(ADC->ctrl1, channel_id);
}

uint8_t ADC_IsChannelDataValid(const uint8_t channel_id)
{
    return (ADC->ctrl0 >> (16 + channel_id)) & 0x1;
}

uint16_t ADC_ReadChannelData(const uint8_t channel_id)
{
    uint32_t v = 0;
    switch (channel_id)
    {
    case 0:
        v = ADC->ctrl1 >> 8;
        break;
    case 1:
        v = ADC->ctrl1 >> 18;
        break;
    case 2:
        v = ADC->ctrl2;
        break;
    case 3:
        v = ADC->ctrl2 >> 10;
        break;
    case 4:
        v = ADC->ctrl2 >> 20;
        break;
    case 5:
        v = ADC->ctrl3;
        break;
    case 6:
        v = ADC->ctrl3 >> 10;
        break;
    case 7:
        v = ADC->ctrl3 >> 20;
        break;
    }
    return v & 0x3FF; // // 10bit ADC
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#endif
