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

void ASDM_PDM_Config(ASDM_TypeDef *base, ASDM_PdmControlTypeDef* pParam)
{
    ASDM_SetEdgeSample(base,1);
    ASDM_SetHighPassEn(base,1);
    ASDM_HighPassErrorEn(base,0);
    ASDM_InvertInput(base,1);
    ASDM_SelMic(base,1);
    ASDM_SelDownSample(base,pParam->Dmic_sample);
    
    ASDM_UpdateSr(base, pParam->Sample_rate);
    ASDM_MuteEn(base,0);
    ASDM_FadeInOutEn(base,0);
    ASDM_SetVolStep(base, 3);

    ASDM_SetVol(base, 0x3fff);

    ASDM_SetHighPassFilter(base, 0xFF);

    ASDM_AGCEn(base,1);
    ASDM_SetAGCMaxLevel(base, 1<<4);

    ASDM_SetAGCFrameTime(base, 8);

    ASDM_SetAGCNoiseThresh(base, 0xf);
    ASDM_SetAGCNoiseMode(base, 0);
    ASDM_AGCNoiseGateEn(base, 1);
    ASDM_SetAGCGateMode(base, 0);
    ASDM_NoiseHold(base, 0x7);

    
    ASDM_FifoEn(base, 1);
    ASDM_SetFifoDepth(APB_ASDM,3);
    ASDM_IntMask(base, pParam->INT_mask);
}

void ASDM_AdcMicConfig(ASDM_TypeDef *base, ASDM_PdmControlTypeDef* pParam)
{
    ASDM_SetEdgeSample(base,1);
    ASDM_SetHighPassEn(base,1);
    ASDM_HighPassErrorEn(base,0);
    ASDM_InvertInput(base,0);
    ASDM_SelMic(base,0);
    ASDM_SelDownSample(base,0);

    ASDM_UpdateSr(base, pParam->Sample_rate);
    ASDM_MuteEn(base,0);
    ASDM_FadeInOutEn(base,0);
    ASDM_SetVolStep(base, 0);

    ASDM_SetVol(base, 0x3fff);

    ASDM_SetHighPassFilter(base, 0xFF);

    ASDM_AGCEn(base,1);
    ASDM_SetAGCMaxLevel(base, 1<<4);

    ASDM_SetAGCFrameTime(base, 1);

    ASDM_SetAGCNoiseThresh(base, 0xf);
    ASDM_SetAGCNoiseMode(base, 0);
    ASDM_AGCNoiseGateEn(base, 1);
    ASDM_SetAGCGateMode(base, 0);
    ASDM_NoiseHold(base, 0x7);

    // amic set
    base->asdm_ana_ctrl0 = 1|7<<1|7<<4|7<<7|7<<10|0<<13;
    base->pga_l_ctrl = 0x3|7<<2|1<<6|1<<9;
    base->pga_l_vol_ctrl = 0;

    base->micbias_ctrl = 1|3<<1|1<<3;
    ASDM_MicBiasEn(base, 1);

    ASDM_FifoEn(base, 1);
    ASDM_IntMask(base, pParam->INT_mask);
}


void PDM_Enable(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_ctrl, enable, 0);
    ASDM_SetRegBit(&base->asdm_ctrl, enable, 1);
}

void ASDM_SetEdgeSample(ASDM_TypeDef *base, uint8_t edge)
{
    ASDM_SetRegBit(&base->asdm_ctrl, edge, 2);
}

void ASDM_SetHighPassEn(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_ctrl, enable, 3);
}

void  ASDM_HighPassErrorEn(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_ctrl, enable, 4);
}

void ASDM_InvertInput(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_ctrl, enable, 5);
}

void ASDM_SelMic(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_ctrl, enable, 6);
}

void ASDM_SelDownSample(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_ctrl, enable, 7);
}

void ASDM_UpdateSr(ASDM_TypeDef *base, ASDM_SampleRatek div)
{
    ASDM_SetRegBits(&base->asdm_sr, div, 9, 0);
    ASDM_SetRegBit(&base->asdm_sr, 1, 9);
}

void ASDM_Reset(ASDM_TypeDef *base)
{
    ASDM_SetRegBit(&base->asdm_clr, 1, 0);
}

void ASDM_MuteEn(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_mute_ctrl, enable, 0);
}

void ASDM_FadeInOutEn(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_mute_ctrl, enable, 1);
}

void ASDM_SetVolStep(ASDM_TypeDef *base, uint32_t step)
{
    ASDM_SetRegBits(&base->asdm_mute_ctrl, step, 8, 2);
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

void ASDM_SetHighPassFilter(ASDM_TypeDef *base, uint32_t data)
{
    ASDM_SetRegBits(&base->asdm_hpf_coef, data, 12, 0);
}

uint8_t ADSM_GetReadFlag(ASDM_TypeDef *base)
{
    return base->asdm_rx_ctrl & 0x1;
}

void ASDM_ClrOverflowError(ASDM_TypeDef *base)
{
    ASDM_SetRegBit(&base->asdm_rx_ctrl, 1, 9);
}

void ASDM_AGCEn(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_agc_ctrl0, enable, 0);
}

void ASDM_SetAGCMaxLevel(ASDM_TypeDef *base, uint32_t level)
{
    ASDM_SetRegBits(&base->asdm_agc_ctrl0, level, 5, 1);
}

void ASDM_SetAGCFrameTime(ASDM_TypeDef *base, uint32_t time)
{
    ASDM_SetRegBits(&base->asdm_agc_ctrl1, time, 12, 0);
}

void ASDM_SetAGCNoiseThresh(ASDM_TypeDef *base, uint32_t noise)
{
    ASDM_SetRegBits(&base->asdm_agc_ctrl2, noise, 5, 0);
}

void ASDM_SetAGCNoiseMode(ASDM_TypeDef *base, uint8_t mode)
{
    ASDM_SetRegBit(&base->asdm_agc_ctrl2, mode, 5);
}

void ASDM_AGCNoiseGateEn(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->asdm_agc_ctrl2, enable, 6);
}

void ASDM_SetAGCGateMode(ASDM_TypeDef *base, uint8_t mode)
{
    ASDM_SetRegBit(&base->asdm_agc_ctrl2, mode, 7);
}

void ASDM_NoiseHold(ASDM_TypeDef *base, uint32_t hold)
{
    ASDM_SetRegBits(&base->asdm_agc_ctrl2, hold, 5, 8);
}

uint8_t ASDM_GetAGCMute(ASDM_TypeDef *base)
{
    return base->asdm_agc_st&0x1;
}

void ASDM_SetPgaVolume(ASDM_TypeDef *base, uint32_t volume)
{
    ASDM_SetRegBits(&base->pga_l_vol_ctrl, volume, 5, 0);
}

void ASDM_MicBiasEn(ASDM_TypeDef *base, uint8_t enable)
{
    ASDM_SetRegBit(&base->micbias_ctrl, enable,1);
}

uint8_t ASDM_GetIntStatus(ASDM_TypeDef *base)
{
    return base->fifo_addr & 0x7;
}

void ASDM_clrFifo(ASDM_TypeDef *base)
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

void ASDM_SetFifoDepth(ASDM_TypeDef *base, uint8_t depth)
{
    ASDM_SetRegBits(&base->fifo_addr, depth, 3, 12);
}

void ASDM_IntMask(ASDM_TypeDef *base, uint8_t mask)
{
    ASDM_SetRegBits(&base->fifo_addr, mask, 3, 16);
}

void ASDM_SetDMATrig(ASDM_TypeDef *base, uint8_t trig)
{
    ASDM_SetRegBits(&base->fifo_addr, trig, 3, 19);
}
#endif
