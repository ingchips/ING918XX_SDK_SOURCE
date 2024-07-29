#ifndef PERPHERAL_ASDM_H_
#define PERPHERAL_ASDM_H_
// #include <stdint.h>
#include "ingsoc.h"


void PDM_Enable(ASDM_TypeDef *base, uint8_t enable);
void ASDM_SetEdgeSample(ASDM_TypeDef *base, uint8_t edge);
void ASDM_SetHighPassEn(ASDM_TypeDef *base, uint8_t enable);
void ASDM_HighPassErrorEn(ASDM_TypeDef *base, uint8_t enable);
void ASDM_InvertInput(ASDM_TypeDef *base, uint8_t enable);
void ASDM_SelMic(ASDM_TypeDef *base, uint8_t enable);
void ASDM_SelDownSample(ASDM_TypeDef *base, uint8_t enable);
void ASDM_UpdateSr(ASDM_TypeDef *base, uint32_t div);
void ASDM_Reset(ASDM_TypeDef *base);
void ASDM_MuteEn(ASDM_TypeDef *base, uint8_t enable);
void ASDM_FadeInOutEn(ASDM_TypeDef *base, uint8_t enable);
void ASDM_SetVolStep(ASDM_TypeDef *base, uint32_t step);
uint8_t ADSM_VolZeroState(ASDM_TypeDef *base);
void ASDM_SetVol(ASDM_TypeDef *base, uint32_t vol);
uint32_t ASDM_GetOutData(ASDM_TypeDef *base);
void ASDM_SetHighPassFilter(ASDM_TypeDef *base, uint32_t data);
uint8_t ADSM_GetReadFlag(ASDM_TypeDef *base);
void ASDM_ClrOverflowError(ASDM_TypeDef *base);
void ASDM_AGCEn(ASDM_TypeDef *base, uint8_t enable);
void ASDM_SetAGCMaxLevel(ASDM_TypeDef *base, uint32_t level);
void ASDM_SetAGCFrameTime(ASDM_TypeDef *base, uint32_t time);
void ASDM_SetAGCNoiseThresh(ASDM_TypeDef *base, uint32_t noise);
void ASDM_SetAGCNoiseMode(ASDM_TypeDef *base, uint8_t mode);
void ASDM_AGCNoiseGateEn(ASDM_TypeDef *base, uint8_t enable);
void ASDM_SetAGCGateMode(ASDM_TypeDef *base, uint8_t mode);
void ASDM_NoiseHold(ASDM_TypeDef *base, uint8_t enable);
void ASDM_SetAGCMute(ASDM_TypeDef *base, uint8_t enable);
uint8_t ASDM_GetFifoFullState(ASDM_TypeDef *base);
uint8_t ASDM_GetFifoEmptyState(ASDM_TypeDef *base);
void ASDM_clrFifo(ASDM_TypeDef *base);
void ASDM_FifoEn(ASDM_TypeDef *base, uint8_t enable);
uint8_t ASDM_GetFifoCount(ASDM_TypeDef *base);
void ASDM_SetFifoDepth(ASDM_TypeDef *base, uint8_t depth);
void ASDM_SetFifoInt(ASDM_TypeDef *base, uint8_t items);
void ASDM_SetFifoTrig(ASDM_TypeDef *base, uint8_t trig);

#endif
