#ifndef PERPHERAL_ASDM_H_
#define PERPHERAL_ASDM_H_
// #include <stdint.h>
#include "ingsoc.h"

/**
 * @brief Fifo int mask
 */
typedef enum {
    ASDM_RX_FLG_EN           = (1 << 16),
    ASDM_FIFO_FULL_EN        = (1 << 17),
    ASDM_FIFO_EMPTY_EN       = (1 << 18),
}ASDM_FifoMask;

/**
 * @brief Asdm sample rate
 */
typedef enum {
    ASDM_DMIC_64_DOWN_SAMPLE = 0,
    ASDM_DMIC_128_DOWN_SAMPLE = 1,
}ASDM_DmicSample;

/**
 * @brief Asdm sample frequence
 */
typedef enum {
    ASDM_SR_48k = 1,
    ASDM_SR_44k = (1<<1),
    ASDM_SR_32k = (1<<2),
    ASDM_SR_24k = (1<<3),
    ASDM_SR_22k = (1<<4),
    ASDM_SR_16k = (1<<5),
    ASDM_SR_12k = (1<<6),
    ASDM_SR_11k = (1<<7),
    ASDM_SR_8k  = (1<<8),
} ASDM_SampleRatek;

/**
 * @brief Asdm init struct
 */
typedef struct 
{
    ASDM_DmicSample Dmic_sample;
    ASDM_SampleRatek Sample_rate;
    uint8_t INT_mask;
    uint8_t Fifo_trig_num;
}ASDM_PdmControlTypeDef;

/**
 * @brief Config ASDM PDM module
 *
 * @param base              ASDM base address
 * @param pParam            ASDM init struct
 * @return                  None
 * @example ASDM_PDM_Config(APB_ASDM, &pParam)
 */
void ASDM_PDM_Config(ASDM_TypeDef *base, ASDM_PdmControlTypeDef* pParam);
/**
 * @brief PDM module enable or disable 
 *
 * @param base              ASDM base address
 * @param enable            1 Enable 0 disable
 * @return                  None
 * @example ASDM_Enable(APB_ASDM, 1)
 */
void PDM_Enable(ASDM_TypeDef *base, uint8_t enable);
/**
 * @brief sample the asdm analog output data at the ASDM_CLK edge
 *
 * @param base              ASDM base address
 * @param edge              0 posedge 1 negedge
 * @return                  None
 * @example data edge is negedge ASDM_SetEdgeSample(APB_ASDM, 1)
 */
void ASDM_SetEdgeSample(ASDM_TypeDef *base, uint8_t edge);
/**
 * @brief High pass filter enable or disable
 *
 * @param base              ASDM base address
 * @param enable            1 Enable 0 disable
 * @return                  None
 * @example ASDM_SetHighPassEn(APB_ASDM, 1)
 */
void ASDM_SetHighPassEn(ASDM_TypeDef *base, uint8_t enable);
/**
 * @brief High pass filter error enable or disable
 * 
 * @param base              ASDM base address
 * @param enable            1 Enable 0 disable
 * @return                  None
 * @example ASDM_HighPassErrorEn(APB_ASDM, 1)
 */
void ASDM_HighPassErrorEn(ASDM_TypeDef *base, uint8_t enable);
/**
 * @brief inverter the input data from analog  or DMIC
 * 
 * @praam base              ASDM base address
 * @param enable            1 Enable 0 disable
 * @return                  None
 * @example ASDM_InvertInput(APB_ASDM, 1)
 */ 
void ASDM_InvertInput(ASDM_TypeDef *base, uint8_t enable);
/**
 * @brief selet the input data from analog  or DMIC
 * 
 * @param base              ASDM base address
 * @param enable            0 AMIC 1 DMIC
 * @return                  None
 * @example ASDM_SelMic(APB_ASDM, 1)
 */ 
void ASDM_SelMic(ASDM_TypeDef *base, uint8_t enable);
/**
 * @brief DMIC downsample rate select signal
 * 
 * @param base              ASDM base address
 * @param enable            0: 64 downsample rate 1: 128 downsample rate
 * @return                  None
 * @example ASDM_SelDownSample(APB_ASDM, 1)
 * NOTE: only active when ASDM_SelMic is dmic
 */ 
void ASDM_SelDownSample(ASDM_TypeDef *base, uint8_t enable);
/**
 * @brief Updata the samping rate
 * 
 * @param base              ASDM base address
 * @param enable            combination of bits whose positions are listed in `ASDM_SampleRatek`
 * @return                  None
 * @example ASDM_UpdateSr(APB_ASDM, ASDM_SR_48k)
 */ 
void ASDM_UpdateSr(ASDM_TypeDef *base, ASDM_SampleRatek div);
/**
 * @brief ADSM reset (reset the reg data and mem data.)
 * 
 * @param base              ASDM base address
 * @return                  None
 * @example ASDM_Reset(APB_ASDM)
 */
void ASDM_Reset(ASDM_TypeDef *base);
/**
 * @brief ADSM mute enable or disable
 * 
 * @param base              ASDM base address
 * @param enable            1 Enable 0 disable
 * @return                  None
 * @example ASDM_MuteEn(APB_ASDM,1)
 */
void ASDM_MuteEn(ASDM_TypeDef *base, uint8_t enable);
/**
 * @brief volume fadein/fadeout function when update the volume value.
 * 
 * @param base              ASDM base address
 * @param enable            1 disable  volume fadein/fadeout function when update the volume value.
 *                          0  enable the  fadein/fadeout function.
 * @return                  None
 * @example ASDM_FadeInOutEn(APB_ASDM,1)
 * NOTE: the volume will change immediately to the vol_l setting or 0 if mute_l set to 1
 *       the volume will change step by step and step value is vol_step[7:0].
 */
void ASDM_FadeInOutEn(ASDM_TypeDef *base, uint8_t enable);
/**
 * @brief volume step control reg
 * 
 * @param base              ASDM base address
 * @param step              Setup the volume step value
 * @return                  None
 * @example ASDM_SetVolStep(APB_ASDM,100)
 * NOTE: 
 *  when asdm_vol_step[7:0] is larger than 0, the mute/unmute or volume change 
 *  is step by step and the step value is vol_step[7:0].
 *  mute time: (asdmx_vol_x/asdmx_vol_step)/Fs, Fs is sample rate of ASDMx
 */
void ASDM_SetVolStep(ASDM_TypeDef *base, uint32_t step);
/**
 * @brief ASDM L's volume zed state
 * 
 * @param base              ASDM base address
 * @return                  1: ASDM L's volume is zero (truly muted)
 *                          0: ASDM L's volume is not zero (not muted)
 */
uint8_t ADSM_VolZeroState(ASDM_TypeDef *base);
/**
 * @brief The volume control for the ASDM
 * 
 * @param base              ASDM base address
 * @param vol               vol data(0-0x3fff )
 *                          0x0 -72.25db 0x1 -66.23db 0x2 -66.23db 0x800 -6db 0x1000 0db 0x3fff 12db
 * @return                  None
 * @example ASDM_SetVol(APB_ASDM,0x3fff)
 */
void ASDM_SetVol(ASDM_TypeDef *base, uint32_t vol);
/**
 * @brief ADSM get out data
 * 
 * @param base              ASDM base address
 * @return                  Dmicc 16bit data (data_next[15:0],data[15:0])
 */
uint32_t ASDM_GetOutData(ASDM_TypeDef *base);
/**
 * @brief Set ASDM high pass filter coefficient
 * 
 * @param base              ASDM base address
 * @param data              data 0x1-0xfff
 * @return                  None
 * @example ASDM_MuteEn(APB_ASDM,0xffe)
 * NOTE:    data format Q(0.12) unsigned
 *          H(z) = (1-z^-1)/(1- hpf_coef *z^-1)
 *          this register is can't be set when asdm_dig_en is 1
 */
void ASDM_SetHighPassFilter(ASDM_TypeDef *base, uint32_t data);
/**
 * @brief ADSM read data flag
 * 
 * @param base              ASDM base address
 * @return                  None
 */
uint8_t ADSM_GetReadFlag(ASDM_TypeDef *base);
/**
 * @brief ADSM clear over flow error
 * 
 * @param base              ASDM base address
 * @return                  None
 * @example ASDM_ClrOverflowError(APB_ASDM)
 */
void ASDM_ClrOverflowError(ASDM_TypeDef *base);
/**
 * @brief Disable and enable asdm agc filter
 * 
 * @param base              ASDM base address
 * @param enable            1 Enable 0 disable
 * @return                  None
 * @example ASDM_AGCEn(APB_ASDM,1)
 */
void ASDM_AGCEn(ASDM_TypeDef *base, uint8_t enable);
/**
 * @brief ASDM configure agc max level
 * 
 * @param base              ASDM base address
 * @param level             config the max level as:  -3 - asdm_max_level  (dB)
 *                          asdm_max_level = -3 - max level ;
 * @return                  None
 * @example ASDM_SetAGCMaxLevel(APB_ASDM,7)
 * NOTE: max level range: -3 ~ -34 dB. default is 01001, -12 dB.
 */
void ASDM_SetAGCMaxLevel(ASDM_TypeDef *base, uint32_t level);
/**
 * @brief asdm_frame_time is PCM numbers in each frame
 * 
 * @param base              ASDM base address
 * @param time              set frame time as (asdm_frame_time+1) ms
 *                          unit is ms, range is 1 ~ 4096 ms
 * @return                  None
 * @example ASDM_SetAGCFrameTime(APB_ASDM,0x1f)
 */
void ASDM_SetAGCFrameTime(ASDM_TypeDef *base, uint32_t time);
/**
 * @brief set noise threshold
 * 
 * @param base              ASDM base address
 * @param noise             asdm_noise_th = (90 + noise threshold level)/2 
 * @return                  None
 * @example ASDM_SetAGCNoiseThresh(APB_ASDM,0xf)
 */
void ASDM_SetAGCNoiseThresh(ASDM_TypeDef *base, uint32_t noise);
/**
 * @brief Set noise mode
 * 
 * @param base              ASDM base address
 * @param enable            0:  The ADC out data level is used to compare the noise threshold
 *                          1: the ADC input data level is used to compare the noise threshold
 * @return None
 * @example ASDM_SetAGCNoiseMode(APB_ASDM,1)
 * NOTE:the ADC input data level is equated as ADC out data level sub the PGA gain
 */
void ASDM_SetAGCNoiseMode(ASDM_TypeDef *base, uint8_t mode);
/**
 * @brief disable or enable agc noise gate
 * 
 * @param base              ASDM base address
 * @param enable            1 Enable 0 disable
 * @return                  None
 * @example ASDM_AGCNoiseGateEn(APB_ASDM,1)
 */
void ASDM_AGCNoiseGateEn(ASDM_TypeDef *base, uint8_t enable);
/**
 * @brief noise gate active mode
 * 
 * @param base              ASDM base address
 * @param enable            0:  the PCM data is mute/unmute when cross zero
 *                          1:  the PCM data is mute/unmute immediately
 * @return                  None
 * @example ASDM_SetAGCGateMode(APB_ASDM,1)
 * NOTE: only active when ASDM_AGCNoiseGateEn is enabled
 */
void ASDM_SetAGCGateMode(ASDM_TypeDef *base, uint8_t mode);
/**
 * @brief program the amount of delay before the AGC algorithm begins to mute the PCM out
 *        data when input data level is lower than noise threshold level. 
 * 
 * @param base              ASDM base address
 * @param enable            Set the noise hold time as asdm_noise_hold * frame_time.
 *                          asdm_hold = noise_hold_time/frame_time .
 * @return                  None
 * @example ASDM_NoiseHold(APB_ASDM,3)
 */
void ASDM_NoiseHold(ASDM_TypeDef *base, uint32_t hold);
/**
 * @brief get ADSM PCM out mute state
 * 
 * @param base              ASDM base address
 * @return                  pcm out mute state
 */
uint8_t ASDM_SetAGCMute(ASDM_TypeDef *base);
/**
 * @brief get fifo full state
 * 
 * @param base              ASDM base address
 * @return                  fifo full state
 */
uint8_t ASDM_GetFifoFullState(ASDM_TypeDef *base);
/**
 * @brief get fifo empty state
 * 
 * @param base              ASDM base address
 * @return                  fifo empty state
 */
uint8_t ASDM_GetFifoEmptyState(ASDM_TypeDef *base);
/**
 * @brief clear fifo data
 * 
 * @param base              ASDM base address
 * @return                  None
 * @example ASDM_clrFifo(APB_ASDM)
 */
void ASDM_clrFifo(ASDM_TypeDef *base);
/**
 * @brief Eanble or disable FIFO
 * @param base              ASDM base address
 * @param enable            1 Enable 0 disable
 * @return                  None
 * @example ASDM_FifoEn(APB_ASDM,1)
 */
void ASDM_FifoEn(ASDM_TypeDef *base, uint8_t enable);
/**
 * @brief get fifo data count
 * 
 * @param base              ASDM base address
 * @return                  fifo data count
 */
uint8_t ASDM_GetFifoCount(ASDM_TypeDef *base);
/**
 * @brief set fifo depth
 * 
 * @param base              ASDM base address
 * @param depth             range from 0 to 3 or 7 which depends on the rtl macros
 * @return                  None
 * @example ASDM_SetFifoDepth(APB_ASDM,7)
 */
void ASDM_SetFifoDepth(ASDM_TypeDef *base, uint8_t depth);
/**
 * @brief Set FIFO interrupt mask
 * 
 * @param base              ASDM base address
 * @param enable            combination of bits whose positions are listed in `ASDM_FifoMask`
 * @return                  None
 * @example ASDM_SetFifoInt(APB_ASDM,ASDM_FIFO_FULL_EN|ASDM_FIFO_EMPTY_EN)
 */
void ASDM_SetFifoInt(ASDM_TypeDef *base, uint8_t items);
/**
 * @brief set FIFO trigger depth
 * 
 * @param base              ASDM base address
 * @param trig              fifo dma ack trigger, range from 0 to 3 or 7 which depends on the rtl macros
 * @return                  None
 * @example ASDM_SetFifoTrig(APB_ASDM,3)
 */
void ASDM_SetFifoTrig(ASDM_TypeDef *base, uint8_t trig);

#endif
