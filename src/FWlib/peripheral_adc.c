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
#include "platform_api.h"
#include <stdlib.h>
#include <string.h>
#include "eflash.h"
#define ADC_LEFT_SHIFT(v, s)            ((v) << (s))
#define ADC_RIGHT_SHIFT(v, s)           ((v) >> (s))
#define ADC_MK_MASK(b)                  ((ADC_LEFT_SHIFT(1, b)) - (1))
#define ADC_REG_VAL(reg)                ((*((volatile uint32_t *)((APB_SARADC_BASE) + (reg)))))
#define REG_CLR(reg, b, s)              ((ADC_REG_VAL(reg)) & (~(ADC_LEFT_SHIFT(ADC_MK_MASK(b), s))))
#define REG_OR(v, s)                    ((ADC_REG_VAL(reg)) | (ADC_LEFT_SHIFT(v, s)))
#define ADC_REG_CLR(reg, b, s)          ((ADC_REG_VAL(reg)) = (REG_CLR(reg, b, s)))
#define ADC_REG_WR(reg, v, s)           ((ADC_REG_VAL(reg)) = (REG_OR(v, s)))
#define ADC_REG_WR_BITS(reg, v, s, b)   ((ADC_REG_VAL(reg)) = ((REG_CLR(reg, b, s)) | (ADC_LEFT_SHIFT(v, s))))
#define ADC_REG_RD(reg, b, s)           ((ADC_RIGHT_SHIFT((ADC_REG_VAL(reg)), s)) & ADC_MK_MASK(b))

static SADC_ftCali_t *ftCali;
static SADC_adcCal_t ADC_adcCal;

#define DEF_WEAK_FUNC(prototype)    __attribute__((weak)) prototype { platform_raise_assertion("missing eflash.c", __LINE__); return 0; }

DEF_WEAK_FUNC(int flash_prepare_factory_data(void))
DEF_WEAK_FUNC(const factory_calib_data_t *flash_get_factory_calib_data(void))
DEF_WEAK_FUNC(const void *flash_get_adc_calib_data(void))
DEF_WEAK_FUNC(uint32_t read_flash_security(uint32_t addr))

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
    ADC_REG_WR_BITS(reg, v, s, b);
}
static uint32_t ADC_RegRd(SADC_adcReg reg, uint8_t s, uint8_t b)
{
    return ADC_REG_RD(reg, b, s);
}

static float ADC_CalWithPgaSingle(const uint16_t data)
{
    return ADC_adcCal.vref_P * data / 16384.f;
}
static float ADC_CalWithPgaDiff(const uint16_t data)
{
    return ADC_adcCal.vref_P * data / 16384.f - ADC_adcCal.vref_P * 0.5f;
}
static void ADC_CbRegister(void)
{
    if (ADC_GetInputMode())
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
    ADC_RegClr(SADC_CFG_0, 10, 1);
    if (ADC_GetInputMode()) {
        if (ch > ADC_CH_3) return;
        if (ch & 0x1) {
            if (enable) {
                ADC_RegWr(SADC_CFG_0, 1, 10);
                ADC_RegWrBits(SADC_CFG_0, (ch << 1), 12, 4);
                ADC_RegWrBits(SADC_CFG_2, 1, 3, 12);
            } else {
                ADC_RegClr(SADC_CFG_0, 12, 4);
                ADC_RegClr(SADC_CFG_2, 3, 12);
            }
        } else {
            if (enable) {
                ADC_RegClr(SADC_CFG_2, 5, 10);
                if (ch)
                    ADC_RegWr(SADC_CFG_2, 1, 4);
                else
                    ADC_RegWr(SADC_CFG_2, 1, 3);
            } else {
                if (ch)
                    ADC_RegClr(SADC_CFG_2, 4, 1);
                else
                    ADC_RegClr(SADC_CFG_2, 3, 1);
            }
        }
    } else {
        ADC_RegClr(SADC_CFG_0, 12, 4);
        if (enable)
            ADC_RegWr(SADC_CFG_2, 1, (ch + 3));
        else
            ADC_RegClr(SADC_CFG_2, (ch + 3), 1);
    }
}

uint16_t ADC_GetEnabledChannels(void)
{
    if (ADC_GetInputMode()) {
        if (ADC_RegRd(SADC_CFG_0, 10, 1))
            return 1 << (ADC_RegRd(SADC_CFG_0, 12, 4) >> 1);
        else
            return (ADC_RegRd(SADC_CFG_2, 4, 1) << 2) | ADC_RegRd(SADC_CFG_2, 3, 1);
    }
    return ADC_RegRd(SADC_CFG_2, 3, 12);
}

void ADC_DisableAllChannels(void)
{
    ADC_RegClr(SADC_CFG_2, 3, 12);
    ADC_RegClr(SADC_CFG_0, 12, 4);
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
SADC_adcIputMode ADC_GetInputMode(void)
{
    return (SADC_adcIputMode)ADC_RegRd(SADC_CFG_0, 8, 1);
}

void ADC_PgaGainSet(SADC_adcPgaGain gain)
{
    if (gain > PGA_GAIN_128) return;
    ADC_RegWrBits(SADC_CFG_0, gain, 2, 3);
}
uint32_t ADC_PgaGainGet(void)
{
    if (ADC_GetPgaStatus())
        return (uint32_t)(1 << ADC_RegRd(SADC_CFG_0, 2, 3));
    else
        return (uint32_t)(1 << (ADC_GetInputMode() ^ 1));
}
void ADC_PgaEnable(uint8_t enable)
{
    ADC_RegClr(SADC_CFG_0, 1, 1);
    if (enable)
        ADC_RegWr(SADC_CFG_0, 1, 1);
}

uint8_t ADC_GetPgaStatus(void)
{
    return ADC_RegRd(SADC_CFG_0, 1, 1);
}

void ADC_SetLoopDelay(uint32_t delay)
{
    ADC_RegWrBits(SADC_CFG_1, delay, 0, 32);
    if (ADC_GetInputMode() && ADC_RegRd(SADC_CFG_0, 10, 1))
        ADC_RegWr(SADC_CFG_0, 3, 22);
    else
        ADC_RegClr(SADC_CFG_0, 22, 2);
}

uint8_t ADC_GetIntStatus(void)
{
    return ADC_RegRd(SADC_INT, 3, 1);
}

uint32_t ADC_PopFifoData(void)
{
    return ADC_RegRd(SADC_DATA, 0, 18);
}
SADC_channelId ADC_GetDataChannel(const uint32_t data)
{
    if (ADC_GetInputMode()) {
        if (ADC_RegRd(SADC_CFG_0, 10, 1))
            return (SADC_channelId)(ADC_RegRd(SADC_CFG_0, 12, 4) >> 1);
        return (SADC_channelId)((ADC_RIGHT_SHIFT(data, 14) & ADC_MK_MASK(4)) >> 1);
    }
    return (SADC_channelId)(ADC_RIGHT_SHIFT(data, 14) & ADC_MK_MASK(4));
}
uint16_t ADC_GetData(const uint32_t data)
{
    if (!ftCali || !ftCali->f) return (data & ADC_MK_MASK(14));
    SADC_channelId ch = ADC_GetDataChannel(data);
    if (ch > ADC_CH_11) return (data & ADC_MK_MASK(14));
    SADC_ftChPara_t *chPara = 0;
    if (ADC_GetPgaStatus()) {
        if ((ch <= ADC_CH_7)) {
            if (ADC_GetInputMode()) {
                if (ch <= ADC_CH_3)
                    chPara = &(ftCali->chParaDiff[ch]);
            } else
                chPara = &(ftCali->chParaSin[ch]);
        }
    } else {
        if (ADC_GetInputMode()) {
            if (ch <= ADC_CH_3)
                chPara = &(ftCali->chParaDiffNoPga[ch]);
        } else {
            if (ch != ADC_CH_9)
                chPara = &(ftCali->chParaSinNoPga[ch]);
        }
    }
    if (chPara)
        return ftCali->f(chPara, data & ADC_MK_MASK(14));
    return (data & ADC_MK_MASK(14));
}
uint16_t ADC_ReadChannelData(const uint8_t channel_id)
{
    uint32_t data = ADC_PopFifoData();
    if (ADC_GetDataChannel(data) == channel_id)
        return ADC_GetData(data);
    return 0;
}

float ADC_GetVol(const uint16_t data)
{
    if (!ADC_adcCal.cb || !ADC_adcCal.vref_P)
        return 0.f;
    if (data > ADC_MK_MASK(14))
        return 0.f;
    return ADC_adcCal.cb(data);
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
    if (!SYSCTRL_GetClk(SYSCTRL_ITEM_APB_ADC)) return;
    if (!ftCali) return;
    ADC_DisableAllChannels();
    ADC_ClrFifo();
    ADC_ConvCfg(CONTINUES_MODE, PGA_GAIN_2, 1, ADC_CH_9, 15, 0,
        SINGLE_END_MODE, SYSCTRL_GetClk(SYSCTRL_ITEM_APB_ADC) / 63000);
    ADC_Start(1);
    while (!ADC_GetIntStatus());
    ADC_Start(0);
    uint8_t i, j;
    uint32_t data, cnt = 0;
    uint16_t array[14] = {0};
    ADC_PopFifoData();
    while (!ADC_GetFifoEmpty()) {
        array[cnt] = ADC_GetData(ADC_PopFifoData());
        cnt++;
    }
    for (i = 0; i < cnt; i++) {
        for (j = i; j < cnt; j++) {
            if (array[j] > array[i]) {
                data = array[i];
                array[i] = array[j];
                array[j] = data;
            }
        }
    }
    data = 0;
    cnt--;
    for (i = 1; i < cnt; i++) {
        data += array[i];
    }
    data /= (cnt - 1);
    ADC_VrefRegister(ftCali->V12Data * ftCali->Vp / data * 0.00001f , 0.f);
    ADC_EnableChannel(ADC_CH_9, 0);
    ADC_IntEnable(0);
}

static uint16_t ADC_FtCal(const SADC_ftChPara_t *chPara, const uint32_t data)
{
    if (data >= 8192)
        return 18192 - chPara->Coseq + (data - 8192) * 200000 / (chPara->k * ADC_PgaGainGet());
    else
        return 18192 - chPara->Coseq - (8192 - data) * 200000 / (chPara->k * ADC_PgaGainGet());
}

typedef struct {
    uint8_t readFlg;
    uint8_t ver;
    uint32_t flg;
    const uint16_t *p_adcCali;
    uint32_t Cin1;
    uint32_t Cin2;
    uint32_t Cout1;
    uint32_t Cout2;
    uint32_t Cout1Addr;
    uint32_t Cout2Addr;
    uint32_t V_cal[4];
} ADC_ftCalPara_t;
static void ADC_FtCoutSet(ADC_ftCalPara_t *calPara)
{
    #define READ_FLASH_SECURITY_LSB(addr)   ((read_flash_security(addr)) & (ADC_MK_MASK(16)))
    #define READ_FLASH_SECURITY_MSB(addr)   (((read_flash_security(addr)) >> (16)) & (ADC_MK_MASK(16)))
    #define READ_ADC_CALI_POINTER(num)      (calPara->p_adcCali[num])
    #define ADC_CALI_POINTER_NUM_CAL(addr)  (((addr) - (0x2000)) >> (1))
    if (calPara->readFlg)
        calPara->Cout1 = READ_FLASH_SECURITY_LSB(calPara->Cout1Addr);
    else
        calPara->Cout1 = READ_ADC_CALI_POINTER(ADC_CALI_POINTER_NUM_CAL(calPara->Cout1Addr));
    if (calPara->Cout1Addr == calPara->Cout2Addr) {
        if (calPara->readFlg)
            calPara->Cout2 = READ_FLASH_SECURITY_MSB(calPara->Cout2Addr);
        else
            calPara->Cout2 = READ_ADC_CALI_POINTER(ADC_CALI_POINTER_NUM_CAL(calPara->Cout2Addr) + 1);
    } else {
        if (calPara->readFlg)
            calPara->Cout2 = READ_FLASH_SECURITY_LSB(calPara->Cout2Addr);
        else
            calPara->Cout2 = READ_ADC_CALI_POINTER(ADC_CALI_POINTER_NUM_CAL(calPara->Cout2Addr));
    }
}
static void ADC_FtParaCal(SADC_ftChPara_t *p, const ADC_ftCalPara_t *calPara)
{
    #define P(n)  (calPara->n)
    p->k = (P(Cout2) - P(Cout1)) * 100000 / (P(Cin2) - P(Cin1));
    p->Coseq = 18192 - (P(Cin1) * (P(Cout2) - 8192) + P(Cin2) * (8192 - P(Cout1))) / (P(Cout2) - P(Cout1));
}
void ADC_ftInit(void)
{
    if (ftCali) return;
    uint8_t i;
    uint8_t ret = flash_prepare_factory_data();
    const factory_calib_data_t *p_factoryCali = flash_get_factory_calib_data();
    ADC_ftCalPara_t ftCalPara = {0};
    ftCalPara.p_adcCali = (const uint16_t *)flash_get_adc_calib_data();
    if (ret || !p_factoryCali || !ftCalPara.p_adcCali)
        ftCalPara.readFlg = 1;
    ftCali = malloc(sizeof(SADC_ftCali_t));
    memset(ftCali, 0, sizeof(SADC_ftCali_t));

    if (ftCalPara.readFlg)
        ftCalPara.flg = read_flash_security(0x1170);
    else
        ftCalPara.flg = p_factoryCali->adc_calib_ver;
    ftCalPara.ver = ftCalPara.flg & ADC_MK_MASK(16);
    ftCalPara.flg = (ftCalPara.flg >> 16) & ADC_MK_MASK(16);
    if (ftCalPara.flg == 0xadc0) {
        if (ftCalPara.readFlg) {
            ftCali->Vp = read_flash_security(0x1174);
            for (i = 0; i < 4; ++i) {
                ftCalPara.V_cal[i] = read_flash_security(0x1178 + (i << 2));
            }
        } else {
            const uint32_t *p = (const uint32_t *)&p_factoryCali->slow_rc[0];
            ftCali->Vp = p[0];
            memcpy(ftCalPara.V_cal, p, 16);
        }
    } else if (ftCalPara.flg == 0xadcf) {
        ftCali->Vp = 330000;
        ftCalPara.V_cal[0] = 30000;
        ftCalPara.V_cal[1] = 310000;
        if (ftCalPara.ver == 2) {
            ftCalPara.V_cal[2] = 30000;
            ftCalPara.V_cal[3] = 310000;
        } else {
            ftCalPara.V_cal[2] = 90000;
            ftCalPara.V_cal[3] = 230000;
        }
    } else {
        ftCali->f = 0;
        return;
    }

    uint32_t mbg;
    if (ftCalPara.readFlg)
        mbg = read_flash_security(0x1100);
    else
        mbg = p_factoryCali->band_gap;
    if (mbg < 0xffffffff)
        *(volatile uint32_t *)0x40102008 = *(volatile uint32_t *)0x40102008 & (~(0x3f << 4)) | (mbg & ADC_MK_MASK(6)) << 4;
    ftCalPara.Cin1 = ftCalPara.V_cal[0] * 16384 / ftCali->Vp;
    ftCalPara.Cin2 = ftCalPara.V_cal[1] / 10 * 16384 / (ftCali->Vp / 10);
    for (i = 0; i < 8; ++i) {
        switch (ftCalPara.ver) {
        case 1:
        case 2:
            ftCalPara.Cout1Addr = 0x2000 + (i << 2);
            ftCalPara.Cout2Addr = ftCalPara.Cout1Addr;
            break;
        default:
            ftCalPara.Cout1Addr = 0x2002 + (i << 5);
            ftCalPara.Cout2Addr = 0x201e + (i << 5);
            break;
        }
        ADC_FtCoutSet(&ftCalPara);
        ADC_FtParaCal(&ftCali->chParaSin[i], &ftCalPara);
        if (ftCalPara.ver == 2) {
            ftCalPara.Cout1Addr = 0x2120 + (i << 2);
            ftCalPara.Cout2Addr = ftCalPara.Cout1Addr;
            ADC_FtCoutSet(&ftCalPara);
            ADC_FtParaCal(&ftCali->chParaSinNoPga[i], &ftCalPara);
        }
    }
    if (ftCalPara.ver == 2) {
        for (i = 0; i < 4; ++i) {
            if (i == 1) continue;
            if (i == 0)
                ftCalPara.Cout1Addr = 0x2140;
            else
                ftCalPara.Cout1Addr = 0x2140 + ((i - 1) << 2);
            ftCalPara.Cout2Addr = ftCalPara.Cout1Addr;
            ADC_FtCoutSet(&ftCalPara);
            ADC_FtParaCal(&ftCali->chParaSinNoPga[i + 8], &ftCalPara);
        }
    }
    ftCalPara.Cin2 = (ftCalPara.V_cal[3] - ftCalPara.V_cal[2]) / 10 * 16384 / (ftCali->Vp / 10) + 8192;
    ftCalPara.Cin1 = 16384 - ftCalPara.Cin2;
    for (i = 0; i < 4; ++i) {
        switch (ftCalPara.ver) {
        case 1:
        case 2:
            ftCalPara.Cout1Addr = 0x2020 + (i << 2);
            ftCalPara.Cout2Addr = ftCalPara.Cout1Addr;
            break;
        default:
            ftCalPara.Cout1Addr = 0x2100 + (i << 4);
            ftCalPara.Cout2Addr = 0x210e + (i << 4);;
            break;
        }
        ADC_FtCoutSet(&ftCalPara);
        ADC_FtParaCal(&ftCali->chParaDiff[i], &ftCalPara);
        if (ftCalPara.ver == 2) {
            ftCalPara.Cout1Addr = 0x214c + (i << 2);
            ftCalPara.Cout2Addr = ftCalPara.Cout1Addr;
            ADC_FtCoutSet(&ftCalPara);
            ADC_FtParaCal(&ftCali->chParaDiffNoPga[i], &ftCalPara);
        }
    }
    if (ftCalPara.readFlg)
        ftCali->V12Data = read_flash_security(0x1144) & ADC_MK_MASK(16);
    else
        ftCali->V12Data = p_factoryCali->v12_adc[0];
    if (ftCalPara.ver == 1)
        ftCali->V12Data -= 14;
    ftCali->f = ADC_FtCal;
    ADC_VrefRegister(ftCali->Vp * 0.00001f, 0.f);
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
    ADC_Reset();
    free(ftCali);
    ftCali = 0;
}

void ADC_Start(uint8_t start)
{
    if (start) {
        ADC_RegWr(SADC_CFG_2, 1, 2);
        if (ADC_RegRd(SADC_CFG_0, 10, 1) && ADC_RegRd(SADC_CFG_0, 12, 4))
            ADC_RegWr(SADC_CFG_0, 1, 11);
    } else {
        ADC_RegClr(SADC_CFG_2, 2, 1);
        ADC_RegClr(SADC_CFG_0, 11, 1);
        while (ADC_GetBusyStatus());
    }
}

void ADC_Calibration(SADC_adcIputMode mode)
{
    ADC_RegClr(SADC_CFG_0, 9, 1);
    ADC_SetAdcMode(CALIBRATION_MODE);
    ADC_SetAdcCtrlMode(SINGLE_MODE);
    ADC_SetInputMode(mode);
    ADC_EnableChannel(ADC_CH_0, 1);
    ADC_SetIntTrig(1);
    ADC_SetCalrpt(CALRPT_32);
    ADC_IntEnable(1);
    ADC_EnableCtrlSignal();
    ADC_RegWr(SADC_CFG_0, 1, 17);
    ADC_RegWrBits(SADC_CFG_0, 4, 18, 4);
    ADC_Start(1);
    while (!ADC_GetIntStatus());
    ADC_Start(0);
    ADC_IntEnable(0);
    ADC_SetAdcMode(CONVERSION_MODE);
    ADC_EnableChannel(ADC_CH_0, 0);
    ADC_RegClr(SADC_CFG_0, 17, 1);
    ADC_ClrFifo();
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
    ADC_SetInputMode(inputMode);
    ADC_EnableChannel(ch, 1);
    if (enNum) {
        ADC_IntEnable(1);
        ADC_SetIntTrig(enNum);
    } else if (dmaEnNum) {
        ADC_DmaEnable(1);
        ADC_SetDmaTrig(dmaEnNum);
    }
    ADC_SetLoopDelay(loopDelay);
    ADC_EnableCtrlSignal();
}

#endif
