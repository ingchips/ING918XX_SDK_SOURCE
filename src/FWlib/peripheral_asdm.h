#ifndef PERPHERAL_ASDM_H_
#define PERPHERAL_ASDM_H_

#include "ingsoc.h"
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

/**
 * @brief Fifo int mask
 *
 * @note
 *      ASDM_RX_FLG_EN : RX FIFO flag interrupt, auto clear.
 *      ASDM_FIFO_FULL_EN : RX FIFO full interrupt. auto clear.
 *      ASDM_FIFO_EMPTY_EN : RX FIFO empty interrupt. auto clear.
 */
typedef enum {
    ASDM_RX_FLG_EN           = 1,
    ASDM_FIFO_FULL_EN        = 1<<1,
    ASDM_FIFO_EMPTY_EN       = 1<<2,
}ASDM_FifoMask;

/**
 * @brief Asdm sample rate
 * @note Only used for dmic.
 *      DMIC typically uses 64x sampling;
 */
typedef enum {
    ASDM_DMIC_64_DOWN_SAMPLE = 0,
    ASDM_DMIC_128_DOWN_SAMPLE = 1,
}ASDM_DmicSample;

/**
 * @bref Enumeration of typical audio sample rates
 * @note ASDM_SR_44k, ASDM_SR_22k, ASDM_SR_11k
 * actually represent 44.1kHz, 22.05kHz, 11.025kHz respectively
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
} ASDM_SampleRate;

/**
 * @bref Enumeration of typical AGC tuning modes for ASDM
 *
 * @note
 * - ASDM_AgcOff:      Completely disables AGC (no audio processing)
 * - ASDM_AgcVoice:    Optimized for voice calls (adjusts AGC params + HPF)
 * - ASDM_AgcMusic:    Optimized for music playback (adjusts AGC params + HPF)
 * - ASDM_AgcCustom:   Manual mode (user must provide AGC params + HPF settings)
 */
typedef enum
{
    ASDM_AgcOff = 0,
    ASDM_AgcVoice,
    ASDM_AgcMusic,
    ASDM_AgcCustom,
} ASDM_AgcMode;

/**
 * @bref Structure for configuring AGC settings for ASDM
 *
 * @note
 * Agc_MaxLevel: width [0:4] Configures the max level of the AGC output.
 *                  0x0 is the minimum level, 0x1F is the maximum level.
 *                  asdm_max_level = -3 - max level
 *                  max level range: -3 ~ -34 dB,
 * Agc_NoiseThreshold: width [0:4] Configures the noise threshold of the AGC.
 *                  00000 :  -90dB
 *                  11111 :  -28dB
 *                  2dB step
 *                  Agc_NoiseThreshold = (90 + noise threshold level)/2
 * Agc_NoiseEn: width [1] Configures the noise enable of the AGC.
 *                  0 : noise disable
 *                  1 : noise enable
 * Agc_NoiseHold: width [0:4] Configures the noise hold of the AGC.
 *                  Set the noise hold time as Agc_NoiseHold * frame_time.
 *                  asdm_hold = noise_hold_time/frame_time .
 * Agc_NoiseFramTime: width [0:11] Configures the noise frame time of the AGC.
 *                  asdm_frame_time is PCM numbers in each frame
 *                  set frame time as (asdm_frame_time+1) ms
 *                  unit is ms, range is 1 ~ 4096 ms
 * Agc_PgaGate: width [0:5] Configures the PGA gate of the AGC.
 */
typedef struct
{
    uint8_t Agc_MaxLevel;
    uint8_t Agc_NoiseThreshold;
    uint8_t Agc_NoiseEn;
    uint8_t Agc_NoiseHold;
    uint16_t Agc_NoiseFramTime;
    uint16_t Agc_PgaGate;
} ASDM_AgcConfigTypeDef;

/**
 * @brief ASDM configuration structure definition
 *
 * @note
 * volume: width [0:13] Configures the volume of the ASDM.
 *          The volume control for the ASDM. format : (2.12) unsigned data.
 *          example: 0x0:0, 0x1:-72.25dB 0x2: -66.23dB .... 0x1000: 0dB ... 0x3fff 12dB
 * Asdm_Mode: width [1] Configures the mode of the ASDM. 0: amic 1: dmic
 *
 * Sample_rate: width [0:8] Configures the sample rate of the ASDM. enumeration value is defined in `ASDM_SampleRate`.
 * @note Requires external sampling clock source of either:
 *       - 12.288 MHz
 *       - 11.2896 MHz
 * @recommend Set PLL output to 384 MHz to ensure accurate sample rates
 * @warning Deviation may cause sampling rate inaccuracy
 *
 * Agc_mode: width [1] Configures the AGC mode of the ASDM. enumeration value is defined in `ASDM_AgcMode`.
 * Agc_config: Configures the AGC configuration of the ASDM. pointer to `ASDM_AgcConfigTypeDef`.
 *              if Agc_mode is 0, this parameter is invalid.
 *
 *              if Agc_mode is ASDM_AgcCustom, this parameter is required.
 *              User must pass Agc_config struct pointer to configure AGC parameters
 *
 *              Other mode: get_config Optional pointer to store preset configs (may be NULL)
 *              - NULL: Use preset parameters (for typical modes)
 *              - Valid pointer: Get custom config.
 *              WARNING: Wild pointers will cause undefined behavior.
 * Fifo_Enable: 1: enable  Set true to enable FIFO (data buffering),
 *              0: false to bypass. Disabling causes overflow data loss.
 *              fifo depth 8.
 *
 * Fifo_DmaTrigNum : width [0:2] Configures the DMA trigger number for FIFO. range [0:7]
 *
 * FifoIntMask: width [0:1] Configures the FIFO interrupt mask. enumeration value is defined in `ASDM_FifoMask`.
 *
 */
typedef struct
{
    uint32_t volume;
    uint8_t Asdm_Mode;
    ASDM_SampleRate Sample_rate;
    ASDM_AgcMode Agc_mode;
    ASDM_AgcConfigTypeDef *Agc_config;
    uint8_t Fifo_Enable;
    uint8_t Fifo_DmaTrigNum;
    uint8_t FifoIntMask;
} ASDM_ConfigTypeDef;

/**
 * @brief Configure ASDM
 *
 * @param [in] base               ASDM base address
 * @param [in] pParam             ASDM configuration structure pointer, described by ASDM_ConfigTypeDef.
 * @return [out]                  None
 */
int ASDM_Config(ASDM_TypeDef *base, ASDM_ConfigTypeDef* pParam);

/**
 * @brief enable or disable ASDM
 *
 * @param [in] base              ASDM base address
 * @param [in] enable            1 Enable 0 disable
 * @return [out]                  None
 */
void ASDM_Enable(ASDM_TypeDef *base, uint8_t enable);

/**
 * @brief sample the asdm analog output data at the ASDM_CLK edge
 *
 * @param [in] base              ASDM base address
 * @param [in] edge              0 posedge 1 negedge
 * @return [out]                  None
 * @example data edge is negedge ASDM_SetEdgeSample(APB_ASDM, 1)
 */
void ASDM_SetEdgeSample(ASDM_TypeDef *base, uint8_t edge);

/**
 * @brief set the ASDM output data at the ASDM_CLK edge
 * @param [in] base ASDM base address
 * @param [in] invert    1: inverter the input data from analog or DMIC
 *                       0: not inverter
 * @return [out]                  None
 */
void ASDM_InvertInput(ASDM_TypeDef *base, uint8_t invert);

/**
 * @brief selet the input data from analog  or DMIC
 *
 * @param [in] base              ASDM base address
 * @param [in] sel               0 AMIC 1 DMIC
 * @return [out]                  None
 * @example ASDM_SelMic(APB_ASDM, 1)
 */
void ASDM_SelMic(ASDM_TypeDef *base, uint8_t sel);

/**
 * @brief DMIC downsample rate select signal
 *
 * @param [in] base              ASDM base address
 * @param [in] enable            0: 64 downsample rate 1: 128 downsample rate
 * @return [out]                  None
 * @example ASDM_SelDownSample(APB_ASDM, 1)
 * @note only active when asdm mode is dmic
 */
void ASDM_SelDownSample(ASDM_TypeDef *base, uint8_t enable);

/**
 * @brief Updata the samping rate
 *
 * @param [in] base              ASDM base address
 * @param [in] sample            input sample rate from ASDM_SampleRate enum
 * @param [in] mode              input agc mode from `ASDM_AgcMode` enum.
 *
 * @note Requires external sampling clock source of either:
 *       - 12.288 MHz
 *       - 11.2896 MHz
 * @recommend Set PLL output to 384 MHz to ensure accurate sample rates
 * @warning Deviation may cause sampling rate inaccuracy
 *
 * @return [out]                 -1 error: pll clk is too high. must be lower than 390M
 *                               0 success
 * @example ASDM_UpdateSr(APB_ASDM, ASDM_SR_48k)
 */
int ASDM_SetSampleRate(ASDM_TypeDef *base, ASDM_SampleRate sample, ASDM_AgcMode mode);

/**
 * @brief High Pass Filter (HPF) Digital Implementation
 *
 * Transfer Function:
 *    Y[z] = ((1 - Z^-1) / (1 - hpf*Z^-1)) * X[z]
 *
 * Difference Equation (time domain):
 *    y[n] = x[n] - x[n-1] + hpf * y[n-1]
 *
 * Normalized Cutoff Calculation:
 *    hpf = 1 / (1 + (2π*fc)/fs)
 *    - fc : Cutoff frequency (Hz)
 *    - fs : Sampling rate (Hz)
 *
 * @param [in] base              ASDM base address
 * @param [in] coef              input HPF coefficient
*                               Register Format:
 *                                     Q(0.12) fixed-point representation (12-bit fractional)
 * @return [out]                 0 success
 */
void ASDM_SetHpfCoef(ASDM_TypeDef *base, uint32_t coef);

/**
 *
 * @param base ASDM base address
 * @param mode input agc mode from `ASDM_AgcMode` enum.
 * @param config struct ASDM_AgcConfigTypeDef
 * @return [out] 0 success
 *               -1 error: pointer is NULL
 *
 * config:      Configures the AGC configuration of the ASDM. pointer to `ASDM_AgcConfigTypeDef`.
 *              if Agc_mode is 0, this parameter is invalid.
 *
 *              if Agc_mode is ASDM_AgcCustom, this parameter is required.
 *              User must pass Agc_config struct pointer to configure AGC parameters
 *
 *              Other mode: get_config Optional pointer to store preset configs (may be NULL)
 *              - NULL: Use preset parameters (for typical modes)
 *              - Valid pointer: Get custom config.
 * WARNING: Wild pointers will cause undefined behavior.
 */
int ASDM_SetAgcMode(ASDM_TypeDef *base, ASDM_AgcMode mode, ASDM_AgcConfigTypeDef *config);

/**
 * @brief ADSM reset (reset the reg data and mem data.)
 *
 * @param [in] base              ASDM base address
 * @return [out]                  None
 * @example ASDM_Reset(APB_ASDM)
 */
void ASDM_Reset(ASDM_TypeDef *base);

/**
 * @brief ADSM mute enable or disable
 *
 * @param [in] base              ASDM base address
 * @param [in] enable            1 Enable 0 disable
 * @return [out]                  None
 * @example ASDM_MuteEn(APB_ASDM,1)
 */
void ASDM_MuteEn(ASDM_TypeDef *base, uint8_t enable);

/**
 * @brief ASDM PGA gain set
 *
 * @param base ASDM base address
 * @param gain Set PGA gain value
 */
void ASDM_SetPgaGain(ASDM_TypeDef *base, uint32_t gain);

/**
 * @brief ASDM L's volume zed state
 *
 * @param [in] base              ASDM base address
 * @return [out]                  1: ASDM L's volume is zero (truly muted)
 *                                0: ASDM L's volume is not zero (not muted)
 */
uint8_t ADSM_VolZeroState(ASDM_TypeDef *base);

/**
* @brief ASDM Volume Control (Sample Gain)
 *
 * @param [in] base              ASDM base address
 * @param [in] vol               vol data(0-0x3fff )
*                               The volume control for the ASDM. format : Q(2.12) unsigned data.
*                               fixed-point representation (14-bit fractional)
 * @example                     0x0:0, 0x1:-72.25dB 0x2: -66.23dB .... 0x1000: 0dB ... 0x3fff 12dB
 *
 * @return [out]                  None
 * @example ASDM_SetVol(APB_ASDM,0x3fff)
 */
void ASDM_SetVol(ASDM_TypeDef *base, uint32_t vol);

/**
 * @brief ADSM get out data
 *
 * @param [in] base              ASDM base address
 * @return [out]                  Dmic 16bit data (data_next[15:0],data[15:0])
 */
uint32_t ASDM_GetOutData(ASDM_TypeDef *base);

/**
 * @brief ADSM read data flag,Buffer data input notification flag.
 *
 * @param [in] base              ASDM base address
 * @return [out]                  1:Buffer data input.
 *                                0:No buffer data input
 */
uint8_t ADSM_GetReadFlag(ASDM_TypeDef *base);

/**
 * @brief ADSM clear over flow error
 *
 * @param [in] base              ASDM base address
 * @return [out]                  None
 * @example ASDM_ClrOverflowError(APB_ASDM)
 */
void ASDM_ClrOverflowError(ASDM_TypeDef *base);

/**
 * @brief get ADSM out mute state
 *
 * @param [in] base              ASDM base address
 * @return [out]                 1: agc out mute state
 *                               0: agc out unmute state
 */
uint8_t ASDM_GetAGCMute(ASDM_TypeDef *base);

/**
 * @brief clear fifo data, fifo pointer reset to 0
 *
 * @param [in] base              ASDM base address
 * @return [out]                  None
 * @example ASDM_ClrFifo(APB_ASDM)
 */
void ASDM_ClrFifo(ASDM_TypeDef *base);

/**
 * @brief Eanble or disable FIFO
 * @param [in] base              ASDM base address
 * @param [in] enable            1 Enable 0 disable
 * @return [out]                  None
 * @example ASDM_FifoEn(APB_ASDM,1)
 */
void ASDM_FifoEn(ASDM_TypeDef *base, uint8_t enable);

/**
 * @brief get fifo data count
 *
 * @param [in] base              ASDM base address
 * @return [out]                  fifo data count
 */
uint8_t ASDM_GetFifoCount(ASDM_TypeDef *base);

/**
 * @brief Set FIFO interrupt mask
 *
 * @param [in] base              ASDM base address
 * @param [in] mask              combination of bits whose positions are listed in `ASDM_FifoMask`
 * @return [out]                  None
 * @code ASDM_IntMask(APB_ASDM,ASDM_FIFO_FULL_EN|ASDM_FIFO_EMPTY_EN)
 */
void ASDM_IntMask(ASDM_TypeDef *base, uint8_t mask);

/**
 * @brief get FIFO interrupt status
 *
 * @param base ASDM base address
 * @return ASDM interrupt status
 */
uint8_t ASDM_GetIntStatus(ASDM_TypeDef *base);

/**
 * @brief set FIFO trigger depth
 *
 * @note the trigger depth is 0 to 7, the actual trigger depth is trigger depth + 1,
 * Only used when the fifo is enabled. Only supported DMA trigger.
 *
 * @param [in] base              ASDM base address
 * @param [in] trig              fifo dma ack trigger, range from 0 to 3 or 7 which depends on the rtl macros
 * @return [out]                  None
 * @example ASDM_SetDMATrig(APB_ASDM,3)
 */
void ASDM_SetDMATrig(ASDM_TypeDef *base, uint8_t trig);

#endif


#endif
