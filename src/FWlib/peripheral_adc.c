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
#define ADC_LEFT_SHIFT(v, s)    ((v) << (s))
#define ADC_RIGHT_SHIFT(v, s)   ((v) >> (s))
#define ADC_MK_MASK(b)          ((ADC_LEFT_SHIFT(1, b)) - (1))
#define ADC_REG_VAL(reg)        ((*((uint32_t *)((APB_SARADC_BASE) + (reg)))))
#define ADC_REG_WR(reg, v, s)   ((ADC_REG_VAL(reg)) |= (ADC_LEFT_SHIFT(v, s)))
#define ADC_REG_RD(reg, b, s)   ((ADC_RIGHT_SHIFT((ADC_REG_VAL(reg)), s)) & ADC_MK_MASK(b))
#define ADC_REG_CLR(reg, b, s)  ((ADC_REG_VAL(reg)) &= (~(ADC_LEFT_SHIFT(ADC_MK_MASK(b), s))))

static SADC_adcCal_t ADC_adcCal;

static void ADC_RegClr(SADC_adcReg reg, uint8_t s, uint32_t b)
{
    ADC_REG_CLR(reg, b, s);
}
static void ADC_RegWr(SADC_adcReg reg, uint32_t v, uint8_t s)
{
    ADC_REG_WR(reg, v, s);
}
static void ADC_RegWrBits(SADC_adcReg reg, uint32_t v, uint8_t s, uint8_t b)
{
    ADC_REG_CLR(reg, b, s);
    ADC_REG_WR(reg, v, s);
}
static uint32_t ADC_RegRd(SADC_adcReg reg, uint8_t s, uint8_t b)
{
    return ADC_REG_RD(reg, b, s);
}

static float ADC_CalWithPgaSingle(uint16_t data)
{
    float gain = (float)ADC_PgaGainGet();
    return ADC_adcCal.vref_P * 0.5f + ((2 * ADC_adcCal.vref_P * (data - 8192.f)) / (16384.f * gain));
}
static float ADC_CalWithPgaDiff(uint16_t data)
{
    float gain = (float)ADC_PgaGainGet();
    return ((data - 8192.f) * 2.f * ADC_adcCal.vref_P) / (16384.f * gain);
}
static void ADC_CbRegister(void)
{
    if (ADC_RegRd(SADC_CFG_0, 8, 1))
        ADC_adcCal.cb = ADC_CalWithPgaDiff;
    else
        ADC_adcCal.cb = ADC_CalWithPgaSingle;
}

void ADC_EnableCtrlSignal(void)
{
    ADC_RegWr(SADC_CFG_0, 1, 9);
    ADC_RegWr(SADC_CFG_0, 1, 5);
}
void ADC_ResetCtrlSignal(void)
{
    ADC_RegClr(SADC_CFG_0, 5, 1);
    ADC_RegClr(SADC_CFG_0, 9, 1);
}

void ADC_SetAdcMode(SADC_adcMode mode)
{
    ADC_RegClr(SADC_CFG_0, 0, 1);
    if (mode)
        ADC_RegWr(SADC_CFG_0, mode, 0);
}
SADC_adcMode ADC_GetAdcMode(void)
{
    return (SADC_adcMode)ADC_RegRd(SADC_CFG_0, 0, 1);
}

static void ADC_SetCalrpt(SADC_adcCalrpt value)
{
    switch (value) {
    case CALRPT_32  :
    case CALRPT_1   :
    case CALRPT_256 :
    case CALRPT_1024:
        ADC_RegWrBits(SADC_CFG_0, value, 6, 2);
    }
}

void ADC_SetAdcCtrlMode(SADC_adcCtrlMode mode)
{
    ADC_RegClr(SADC_CFG_2, 0, 1);
    if (mode)
        ADC_RegWr(SADC_CFG_2, mode, 0);
}

void ADC_EnableChannel(SADC_channelId ch, uint8_t enable)
{
    if (ch > ADC_CH_11) return;
    if (enable)
        ADC_RegWr(SADC_CFG_2, 1, (ch + 3));
    else
        ADC_RegClr(SADC_CFG_2, (ch + 3), 1);
}

void ADC_IntEnable(uint8_t enable)
{
    ADC_RegClr(SADC_INT_MAKS, 3, 1);
    if (enable)
        ADC_RegWr(SADC_INT_MAKS, 1, 3);
}
void ADC_SetIntTrig(uint8_t num)
{
    if (num > 0xf)
        return;
    ADC_RegWrBits(SADC_CFG_2, num, 20, 4);
    ADC_DmaEnable(0);
}
void ADC_DmaEnable(uint8_t enable)
{
    ADC_RegClr(SADC_CFG_2, 15, 1);
    if (enable)
        ADC_RegWr(SADC_CFG_2, 1, 15);
}
void ADC_SetDmaTrig(uint8_t num)
{
    if (num > 0xf)
        return;
    ADC_RegWrBits(SADC_CFG_2, num, 16, 4);
    ADC_IntEnable(0);
}

void ADC_ClrFifo(void)
{
    ADC_RegWr(SADC_STATUS, 1, 22);
}

uint8_t ADC_GetFifoEmpty(void)
{
    return ADC_RegRd(SADC_INT, 0, 1);
}

uint8_t ADC_GetBusyStatus(void)
{
    return ADC_RegRd(SADC_STATUS, 23, 1);
}

void ADC_SetInputMode(SADC_adcIputMode mode)
{
    ADC_RegClr(SADC_CFG_0, 8, 1);
    if (mode)
        ADC_RegWr(SADC_CFG_0, mode, 8);
    if (ADC_adcCal.cb)
        ADC_CbRegister();
}

void ADC_PgaGainSet(SADC_adcPgaGain gain)
{
    if (gain > PGA_GAIN_128) return;
    ADC_RegWrBits(SADC_CFG_0, gain, 2, 3);
}
SADC_adcPgaGain ADC_PgaGainGet(void)
{
    return (SADC_adcPgaGain)(1 << ADC_RegRd(SADC_CFG_0, 2, 3));
}
void ADC_PgaEnable(uint8_t enable)
{
    ADC_RegClr(SADC_CFG_0, 1, 1);
    // if (enable)
    ADC_RegWr(SADC_CFG_0, 1, 1);
}

uint8_t ADC_GetPgaStatus(void)
{
    return ADC_RegRd(SADC_CFG_0, 1, 1);
}

void ADC_SetLoopDelay(uint32_t delay)
{
    ADC_RegWrBits(SADC_CFG_1, delay, 0, 32);
}

uint8_t ADC_GetIntStatus(void)
{
    return ADC_RegRd(SADC_INT, 3, 1);
}

uint32_t ADC_PopFifoData(void)
{
    return ADC_RegRd(SADC_DATA, 0, 18);
}
SADC_channelId ADC_GetDataChannel(uint32_t data)
{
    return (SADC_channelId)(ADC_RIGHT_SHIFT(data, 14) & ADC_MK_MASK(4));
}
uint16_t ADC_GetData(uint32_t data)
{
    return (data & ADC_MK_MASK(14));
}
uint16_t ADC_ReadChannelData(const uint8_t channel_id)
{
    uint32_t data = ADC_PopFifoData();
    if (ADC_GetDataChannel(data) == channel_id)
        return ADC_GetData(data);
    return 0;
}

void ADC_Start(uint8_t start)
{
    if (start)
        ADC_RegWr(SADC_CFG_2, 1, 2);
    else
        ADC_RegClr(SADC_CFG_2, 2, 1);
}

static void ADC_VrefRegister(float VP, float VN)
{
    if (VP < VN) return;
    ADC_adcCal.vref_P = VP;
    ADC_adcCal.vref_N = VN;
    ADC_CbRegister();
}

void ADC_VrefCalibration(void)
{
    ADC_ConvCfg(SINGLE_MODE, PGA_GAIN_2, 1, ADC_CH_9, 1, 0, SINGLE_END_MODE, 0);
    ADC_Start(1);
    while (!ADC_GetIntStatus());
    uint32_t data = ADC_PopFifoData();
    ADC_VrefRegister(19659.6f / (float)ADC_GetData(data), 0.f);
    ADC_EnableChannel(ADC_CH_9, 0);
    ADC_Start(0);
}

float ADC_GetVol(uint16_t data)
{
    if (!ADC_adcCal.cb || !ADC_adcCal.vref_P)
        return 0;
    float vol = ADC_adcCal.cb(data);
    if (vol > ADC_adcCal.vref_P) return ADC_adcCal.vref_P;
    if (vol < ADC_adcCal.vref_N) return ADC_adcCal.vref_N;
    return vol;
}

void ADC_Reset(void)
{
    ADC_RegWrBits(SADC_CFG_0, 2 << 6, 0, 32);
    ADC_RegWrBits(SADC_CFG_1, 0x100, 0, 32);
    ADC_RegWrBits(SADC_CFG_2, 4 << 16 | 4 << 20 | 0xa << 24, 0, 32);
    ADC_ClrFifo();
    ADC_RegClr(SADC_STATUS  , 0, 32);
    ADC_RegClr(SADC_INT_MAKS, 0, 32);
}

void ADC_AdcClose(void)
{
    ADC_Start(0);
    ADC_IntEnable(0);
    while (ADC_GetBusyStatus());
    ADC_ResetCtrlSignal();
    ADC_DmaEnable(0);
    ADC_ClrFifo();
}

void ADC_Calibration(SADC_adcIputMode mode)
{
    ADC_SetAdcMode(CALIBRATION_MODE);
    ADC_SetAdcCtrlMode(SINGLE_MODE);
    ADC_EnableChannel(ADC_CH_0, 1);
    ADC_SetIntTrig(1);
    ADC_SetCalrpt(CALRPT_32);
    ADC_SetInputMode(mode);
    ADC_IntEnable(1);
    ADC_EnableCtrlSignal();
    ADC_RegWr(SADC_CFG_0, 1, 17);
    ADC_RegWrBits(SADC_CFG_0, 4, 18, 4);
    ADC_Start(1);
    while (!ADC_GetIntStatus());
    ADC_Start(0);
    ADC_ResetCtrlSignal();
    ADC_IntEnable(0);
    ADC_ClrFifo();
    ADC_SetAdcMode(CONVERSION_MODE);
}

void ADC_ConvCfg(SADC_adcCtrlMode ctrlMode, 
                 SADC_adcPgaGain pgaGain,
                 uint8_t pgaEnable,
                 SADC_channelId ch, 
                 uint8_t enNum, 
                 uint8_t dmaEnNum, 
                 SADC_adcIputMode inputMode, 
                 uint32_t loopDelay)
{
    ADC_SetAdcMode(CONVERSION_MODE);
    ADC_SetAdcCtrlMode(ctrlMode);
    ADC_PgaGainSet(pgaGain);
    ADC_PgaEnable(pgaEnable);
    ADC_EnableChannel(ch, 1);
    if (enNum) {
        ADC_IntEnable(1);
        ADC_SetIntTrig(enNum);
    } else if (dmaEnNum) {
        ADC_DmaEnable(1);
        ADC_SetDmaTrig(dmaEnNum);
    }
    ADC_SetInputMode(inputMode);
    ADC_SetLoopDelay(loopDelay);
    ADC_EnableCtrlSignal();
}

#endif
