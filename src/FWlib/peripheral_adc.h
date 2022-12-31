#ifndef _PERIPHERAL_ADC_H
#define _PERIPHERAL_ADC_H

#include <stdint.h>

#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define ADC_CLK_16      0
#define ADC_CLK_32      1
#define ADC_CLK_64      2
#define ADC_CLK_128     3

#define ADC_CLK_EN      0x4

// total 8 ADC channels
#define ADC_CHANNEL_NUM     8

/** \brief  Structure type to access the ADC
 */
typedef struct
{
    volatile uint32_t ctrl0;
    volatile uint32_t ctrl1;
    volatile uint32_t ctrl2;
    volatile uint32_t ctrl3;
    volatile uint32_t ctrl4;
}  ADC_Type;

/** \brief  Structure type to access the ADC power control
 */
typedef struct
{
    volatile uint32_t pon;
    volatile uint32_t act;
}  ADC_Pwr_Ctrl_Type;

/** \brief ADC power control
 * flag: 1: power on
 *       0: power off
 */
void ADC_PowerCtrl(const uint8_t flag);

/** \brief ADC global enable/disable
 * flag: 1: power on
 *       0: power off
 */
void ADC_Enable(const uint8_t flag);

/** \brief ADC reset
 */
void ADC_Reset(void);

/** \brief Set ADC clock selection
 *  Note that: remeber to include ADC_CLK_EN.
 */
void ADC_SetClkSel(const uint8_t clk_sel);

#define ADC_MODE_SINGLE     1
#define ADC_MODE_LOOP       0
void ADC_SetMode(const uint8_t mode);

// the number of clock circles between two calculation at loop mode
void ADC_SetLoopDelay(const uint32_t delay);

void ADC_EnableChannel(const uint8_t channel_id, const uint8_t flag);
void ADC_ClearChannelDataValid(const uint8_t channel_id);
uint8_t ADC_IsChannelDataValid(const uint8_t channel_id);
uint16_t ADC_ReadChannelData(const uint8_t channel_id);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

typedef enum {
    CALIBRATION_MODE,
    CONVERSION_MODE,
} SADC_adcMode;

typedef enum {
    CALRPT_32       = 0x0,
    CALRPT_1        = 0x1,
    CALRPT_256      = 0x2,
    CALRPT_1024     = 0x3,
} SADC_adcCalrpt;

typedef enum {
    CONTINUES_MODE,
    SINGLE_MODE,
} SADC_adcCtrlMode;

typedef enum {
    ADC_CH_0 = 0,
    ADC_CH_1,
    ADC_CH_2,
    ADC_CH_3,
    ADC_CH_4,
    ADC_CH_5,
    ADC_CH_6,
    ADC_CH_7,
    ADC_CH_8,
    ADC_CH_9,
    ADC_CH_10,
    ADC_CH_11,
} SADC_channelId;

typedef enum {
    SINGLE_END_MODE,
    DIFFERENTAIL_MODE,
} SADC_adcIputMode;

typedef enum {
    PGA_GAIN_1       = 0x0,
    PGA_GAIN_2       = 0x1,
    PGA_GAIN_4       = 0x2,
    PGA_GAIN_8       = 0x3,
    PGA_GAIN_16      = 0x4,
    PGA_GAIN_32      = 0x5,
    PGA_GAIN_64      = 0x6,
    PGA_GAIN_128     = 0x7,
    PGA_NO_USE       = PGA_GAIN_1,
} SADC_adcPgaGain;

typedef enum {
    SADC_CFG_0      = 0x0,
    SADC_CFG_1      = 0x4,
    SADC_CFG_2      = 0x8,
    SADC_DATA       = 0x0c,
    SADC_STATUS     = 0x10,
    SADC_INT_MAKS   = 0x30,
    SADC_INT        = 0x34,
} SADC_adcReg;

typedef struct
{
    float vref_P;
    float vref_N;
    float(*cb)(uint16_t);
} SADC_adcCal_t;

/**
 * @brief Enable ADC control signal
 *
 * @param[in]                null
 * @return                   null
 */
void ADC_EnableCtrlSignal(void);

/**
 * @brief Reset ADC control signal
 *
 * @param[in]                null
 * @return                   null
 */
void ADC_ResetCtrlSignal(void);

/**
 * @brief Set ADC mode
 *
 * @param[in] mode           ADC mode, see 'SADC_adcMode'
 * @return                   null
 */
void ADC_SetAdcMode(SADC_adcMode mode);

/**
 * @brief Get ADC mode
 *
 * @param[in]                null
 * @return                   mode
 */
SADC_adcMode ADC_GetAdcMode(void);

/**
 * @brief Set ADC control mode
 *
 * @param[in] mode           ADC control mode, see 'SADC_adcCtrlMode'
 * @return                   null
 */
void ADC_SetAdcCtrlMode(SADC_adcCtrlMode mode);

/**
 * @brief Enable ADC channel
 *
 * @param[in] ch             ADC channel ID, see 'SADC_channelId'
 * @param[in] enable         enable/disable channel
 * @return                   null
 */
void ADC_EnableChannel(SADC_channelId ch, uint8_t enable);

/**
 * @brief Enable ADC interrupt
 *
 * @param[in] enable         enable/disable ADC interrupt
 * @return                   null
 */
void ADC_IntEnable(uint8_t enable);

/**
 * @brief Set ADC interrupt trigger number
 *
 * @param[in] num            ADC interrupt trigger number(0-15)
 * @return                   null
 */
void ADC_SetIntTrig(uint8_t num);

/**
 * @brief Enable ADC dma transmition request
 *
 * @param[in] enable         enable/disable ADC dma transmition request
 * @return                   null
 */
void ADC_DmaEnable(uint8_t enable);

/**
 * @brief Set ADC dma transmition trigger number
 *
 * @param[in] num            ADC dma transmition trigger number(0-15)
 * @return                   null
 */
void ADC_SetDmaTrig(uint8_t num);

/**
 * @brief clear ADC FIFO
 *
 * @param[in]                null
 * @return                   null
 */
void ADC_ClrFifo(void);

/**
 * @brief Get ADC-FIFO's empty status
 *
 * @param[in]                null
 * @return FIFO's status     0:not empty,1:empty
 */
uint8_t ADC_GetFifoEmpty(void);

/**
 * @brief Get ADC busy status
 *
 * @param[in]                null
 * @return                   1:busy, 0:not work
 */
uint8_t ADC_GetBusyStatus(void);

/**
 * @brief Set ADC input mode
 *
 * @param[in] mode           ADC input mode, see 'SADC_adcIputMode'
 * @return                   null
 */
void ADC_SetInputMode(SADC_adcIputMode mode);

/**
 * @brief Set pga gain
 *
 * @param[in] gain           pga gain, see 'SADC_adcPgaGain'
 * @return                   null
 */
void ADC_PgaGainSet(SADC_adcPgaGain gain);

/**
 * @brief Get pga gain
 *
 * @param[in]                null
 * @return                   pga gain, see 'SADC_adcPgaGain'
 */
SADC_adcPgaGain ADC_PgaGainGet(void);

/**
 * @brief Enable pga
 *
 * @param[in] enable         enable/disable pga
 * @return                   null
 */
void ADC_PgaEnable(uint8_t enable);

/**
 * @brief Get pga work status
 *
 * @param[in]                null
 * @return                   1:pga enabled, 0:pga disabled
 */
uint8_t ADC_GetPgaStatus(void);

/**
 * @brief Set ADC loop delay
 *
 * @param[in] delay          ADC loop delay(0-0xffffffff)
 * @return                   null
 */
void ADC_SetLoopDelay(uint32_t delay);

/**
 * @brief Get ADC interrupt-bit's value
 *
 * @param[in]                null
 * @return                   1:interrupt, 0:no interrupt
 */
uint8_t ADC_GetIntStatus(void);

/**
 * @brief Read ADC FIFO data(should work with ADC_GetDataChannel & ADC_GetData)
 *
 * @param[in]                null
 * @return                   data channel & ADC data
 */
uint32_t ADC_PopFifoData(void);

/**
 * @brief To get channel Id of the data read by ADC_PopFifoData
 *
 * @param[in] data           data read by ADC_PopFifoData
 * @return                   channel id, see 'SADC_channelId'
 */
SADC_channelId ADC_GetDataChannel(uint32_t data);

/**
 * @brief To get ADC data of the data read by ADC_PopFifoData
 *
 * @param[in] data           data read by ADC_PopFifoData
 * @return                   ADC data
 */
uint16_t ADC_GetData(uint32_t data);

/**
 * @brief Read ADC data in specified channel
 * @note Using ADC_PopFifoData to get the whole data, and get that data's
 * channel id and ADC data by ADC_GetDataChannel & ADC_GetData is recommended.
 * 
 * @param[in] channel_id     channel ID
 * @return                   ADC data
 */
uint16_t ADC_ReadChannelData(const uint8_t channel_id);

/**
 * @brief start ADC
 *
 * @param[in] start          start/stop ADC
 * @return                   null
 */
void ADC_Start(uint8_t start);

/**
 * @brief Calibrate VREFP
 *
 * @param[in]                null
 * @return                   null
 */
void ADC_VrefCalibration(void);

/**
 * @brief Get voltage by ADC data
 * @note Should Calibrate VREFP by ADC_VrefCalibration first.
 *
 * @param[in] data           ADC data
 * @return                   voltage
 */
float ADC_GetVol(uint16_t data);

/**
 * @brief Reset ADC configuration
 *
 * @param[in]                null
 * @return                   null
 */
void ADC_Reset(void);

/**
 * @brief Close ADC
 *
 * @param[in]                null
 * @return                   null
 */
void ADC_AdcClose(void);

/**
 * @brief ADC calibration standard configuration interface
 *
 * @param[in] mode           ADC input mode, see 'SADC_adcIputMode'
 * @return                   null
 */
void ADC_Calibration(SADC_adcIputMode mode);

/**
 * @brief ADC conversion standard configuration interface
 *
 * @param[in] ctrlMode       ADC control mode, see 'SADC_adcCtrlMode'
 * @param[in] pgaGain        pga gain, see 'SADC_adcPgaGain'
 * @param[in] pgaEnable      enable/diable pga
 * @param[in] ch             ADC channel id, see 'SADC_channelId'
 * @param[in] enNum          ADC interrupt trigger number(0-15)
 * @param[in] dmaEnNum       DMA transmition require trigger number(0-15)
 * @param[in] inputMode      ADC input mode, see 'SADC_adcIputMode'
 * @param[in] loopDelay      ADC loop delay(0-0xffffffff)
 * @return                   null
 */
void ADC_ConvCfg(SADC_adcCtrlMode ctrlMode, 
                 SADC_adcPgaGain pgaGain,
                 uint8_t pgaEnable,
                 SADC_channelId ch, 
                 uint8_t enNum, 
                 uint8_t dmaEnNum, 
                 SADC_adcIputMode inputMode, 
                 uint32_t loopDelay);

#endif

#ifdef __cplusplus
}
#endif

#endif
