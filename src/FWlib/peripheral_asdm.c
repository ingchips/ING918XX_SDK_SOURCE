#include "peripheral_asdm.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
static void ASDM_SetRegBits(volatile uint32_t *reg, uint32_t v, uint8_t bit_width, uint8_t bit_offset)
{
    uint32_t mask = (1 << bit_width) - 1;
    *reg &= ~(mask<<bit_offset);
    v = v & mask;
    *reg |= v << bit_offset;
}

static void ASDM_SetRegBit(volatile uint32_t *reg, uint8_t v, uint8_t bit_offset)
{
    if (v)
        *reg |= 1 << bit_offset;
    else
        *reg &= ~(1 << bit_offset);
}

int ASDM_Config(ASDM_TypeDef *base, ASDM_ConfigTypeDef* pParam)
{
    int ret;
    base->asdm_ctrl = 0xc | (pParam->Asdm_Mode << 6);
    ret = ASDM_SetSampleRate(base, pParam->Sample_rate, pParam->Agc_mode);
    if (ret)
        return ret;
    base->asdm_mute_ctrl = 0xc;
    base->asdm_vol_ctrl = pParam->volume;
    ret = ASDM_SetAgcMode(base, pParam->Agc_mode, pParam->Agc_config);
    if (ret)
        return ret;
    base->asdm_ana_ctrl0 = 0x1fff;
    base->asdm_pga_en_ibias = 0x1;
    base->pga_l_ctrl = 0x343;
    base->micbias_ctrl = 0xf;
    base->fifo_addr = 0x4|(pParam->Fifo_Enable<<3)|(pParam->FifoIntMask <<16)|(pParam->Fifo_DmaTrigNum<<19) ;
    return 0;
}

void ASDM_Enable(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_ctrl, enable, 0);
    ASDM_SetRegBit(&base->asdm_ctrl, enable, 1);
}

void ASDM_SetEdgeSample(ASDM_TypeDef *base, uint8_t edge)
{
    ASDM_SetRegBit(&base->asdm_ctrl, edge, 2);
}

void ASDM_InvertInput(ASDM_TypeDef *base, uint8_t invert)
{
    ASDM_SetRegBit(&base->asdm_ctrl, invert, 5);
}

void ASDM_SelMic(ASDM_TypeDef *base, uint8_t sel)
{
    ASDM_SetRegBit(&base->asdm_ctrl, sel, 6);
}

void ASDM_SelDownSample(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_ctrl, enable, 7);
}

int ASDM_SetSampleRate(ASDM_TypeDef *base, ASDM_SampleRate sample, ASDM_AgcMode mode)
{
    float div;
    uint32_t rounded, i;
    uint16_t hpf_arr[18] = {
        4017, 4079,
        4010, 4078,
        3978, 4072,
        3941, 4064,
        3928, 4061,
        3868, 4048,
        3797, 4032,
        3773, 4027,
        3664, 4001,
    };

    uint32_t pll_clk = SYSCTRL_GetPLLClk();
    if (pll_clk > 390000000)
        return -1;
    if (!(mode & 0x3))
        mode = ASDM_AgcVoice;
    switch (sample)
    {
        case ASDM_SR_8k:
        case ASDM_SR_12k:
        case ASDM_SR_16k:
        case ASDM_SR_24k:
        case ASDM_SR_32k:
        case ASDM_SR_48k:
            div = ((float)(pll_clk/1000000ul)) / 12.288f;
            rounded = (int)(div + 0.5f);
            break;
        case ASDM_SR_44k:
        case ASDM_SR_22k:
        case ASDM_SR_11k:
            div = ((float)(pll_clk/1000000ul)) / 11.2896f;
            rounded = (int)(div + 0.5f);
            break;
        default:
            return -1;
    }
    for (i=0; i<9; i++)
    {
        if ((1<<i) & sample)
        {
            ASDM_SetRegBits(&base->asdm_hpf_coef, hpf_arr[i*2+(mode-1)], 12, 0);
            break;
        }
    }
    SYSCTRL_UpdateAsdmClk(rounded);
    ASDM_SetRegBits(&base->asdm_sr, sample, 9, 0);
    ASDM_SetRegBit(&base->asdm_sr, 1, 9);

    return 0;
}

void ASDM_SetHpfCoef(ASDM_TypeDef *base, uint32_t coef)
{
    ASDM_SetRegBits(&base->asdm_hpf_coef, coef, 12, 0);
}

int ASDM_SetAgcMode(ASDM_TypeDef *base, ASDM_AgcMode mode, ASDM_AgcConfigTypeDef *config)
{
    uint32_t max_level, frame_time, noise_th, noise_hold, noise_en, agc_gate;
    if (mode == ASDM_AgcCustom)
    {
        if (config == 0)
            return -1;
        max_level = config->Agc_MaxLevel;
        frame_time = config->Agc_NoiseFramTime;
        noise_th = config->Agc_NoiseThreshold;
        noise_hold = config->Agc_NoiseHold;
        noise_en = config->Agc_NoiseEn;
        agc_gate = config->Agc_PgaGate;
    }
    else if (mode)
    {
        max_level = 0x20;
        frame_time = 1;
        noise_th = 0xf;
        noise_hold = 0x7;
        agc_gate = 0xc;
        noise_en = 1;
        if (config)
        {
            config->Agc_MaxLevel = max_level;
            config->Agc_NoiseFramTime = frame_time;
            config->Agc_NoiseThreshold = noise_th;
            config->Agc_NoiseHold = noise_hold;
            config->Agc_NoiseEn = noise_en;
        }
    }
    else
    {
        base->asdm_agc_ctrl0 = 0;
        return 0;
    }
    base->pga_l_vol_ctrl = agc_gate;
    base->asdm_agc_ctrl0 = 1 | (max_level<<1);
    base->asdm_agc_ctrl1 = frame_time;
    base->asdm_agc_ctrl2 = noise_th | (noise_hold<<8) | (noise_en<<6);

    return 0;
    // TODO
}

void ASDM_Reset(ASDM_TypeDef *base)
{
    ASDM_SetRegBit(&base->asdm_clr, 1, 0);
}

void ASDM_MuteEn(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_mute_ctrl, enable, 0);
}

uint8_t ADSM_VolZeroState(ASDM_TypeDef *base)
{
    return (base->asdm_mute_ctrl >> 10) & 0x1;
}

void ASDM_SetVol(ASDM_TypeDef *base, uint32_t vol)
{
    ASDM_SetRegBits(&base->asdm_vol_ctrl, vol, 14, 0);
}

uint32_t ASDM_GetOutData(ASDM_TypeDef *base)
{
    return base->asdm_dout;
}

uint8_t ADSM_GetReadFlag(ASDM_TypeDef *base)
{
    return base->asdm_rx_ctrl & 0x1;
}

void ASDM_ClrOverflowError(ASDM_TypeDef *base)
{
    ASDM_SetRegBit(&base->asdm_rx_ctrl, 1, 1);
}

uint8_t ASDM_GetAGCMute(ASDM_TypeDef *base)
{
    return base->asdm_agc_st&0x1;
}

void ASDM_SetPgaGain(ASDM_TypeDef *base, uint32_t gain)
{
    ASDM_SetRegBits(&base->pga_l_vol_ctrl, gain, 5, 0);
}

void ASDM_ClrFifo(ASDM_TypeDef *base)
{
    ASDM_SetRegBit(&base->fifo_addr, 1, 2);
}

void ASDM_FifoEn(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->fifo_addr, enable, 3);
}

uint8_t ASDM_GetFifoCount(ASDM_TypeDef *base)
{
    return (base->fifo_addr >> 8) & 0x07ul;
}

void ASDM_IntMask(ASDM_TypeDef *base, uint8_t mask)
{
    ASDM_SetRegBits(&base->fifo_addr, mask, 3, 16);
}

uint8_t ASDM_GetIntStatus(ASDM_TypeDef *base)
{
    return base->fifo_addr & 0x7;
}

void ASDM_SetDMATrig(ASDM_TypeDef *base, uint8_t trig)
{
    ASDM_SetRegBits(&base->fifo_addr, trig, 3, 19);
}
#endif
