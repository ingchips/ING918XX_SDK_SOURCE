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

typedef void (*ADC_VrefCaliCb)(void);

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

#define ADC_FIFO_DEPTH      16

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
    PGA_PARA_0 = 0,
    PGA_PARA_1,
    PGA_PARA_2,
    PGA_PARA_3,
    PGA_PARA_4,
    PGA_PARA_5,
} SADC_pgaPara;

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
    float (*cb)(const uint16_t);
} SADC_adcCal_t;

typedef struct
{
    uint32_t k;
    uint32_t Coseq;
} SADC_ftChPara_t;
typedef struct
{
    uint32_t Vp;
    uint16_t V12Data;
    uint16_t (*f)(const SADC_ftChPara_t *, const uint32_t);
    SADC_ftChPara_t chParaSin[8];
    SADC_ftChPara_t chParaDiff[4];
    SADC_ftChPara_t chParaSinNoPga[12];
    SADC_ftChPara_t chParaDiffNoPga[4];
} SADC_ftCali_t;

/**
 * @brief Enable ADC control signal
 */
void ADC_EnableCtrlSignal(void);

/**
 * @brief Reset ADC control signal
 */
void ADC_ResetCtrlSignal(void);

/**
 * @brief Set ADC mode
 *
 * @param[in] mode           ADC mode, see 'SADC_adcMode'
 */
void ADC_SetAdcMode(SADC_adcMode mode);

/**
 * @brief Get ADC mode
 *
 * @return                   mode
 */
SADC_adcMode ADC_GetAdcMode(void);

/**
 * @brief Set ADC control mode
 *
 * @param[in] mode           ADC control mode, see 'SADC_adcCtrlMode'
 */
void ADC_SetAdcCtrlMode(SADC_adcCtrlMode mode);

/**
 * @brief Enable ADC channel
 *
 * @param[in] ch             ADC channel ID, see 'SADC_channelId'
 * @param[in] enable         enable/disable channel
 */
void ADC_EnableChannel(SADC_channelId ch, uint8_t enable);

/**
 * @brief Disable all ADC channels
 */
void ADC_DisableAllChannels(void);

/**
 * @brief Enable ADC interrupt
 *
 * @param[in] enable         enable/disable ADC interrupt
 */
void ADC_IntEnable(uint8_t enable);

/**
 * @brief Set ADC interrupt trigger number
 *
 * @param[in] num            ADC interrupt trigger number(0-15)
 */
void ADC_SetIntTrig(uint8_t num);

/**
 * @brief Enable ADC dma transmition request
 *
 * @param[in] enable         enable/disable ADC dma transmition request
 */
void ADC_DmaEnable(uint8_t enable);

/**
 * @brief Set ADC dma transmition trigger number
 *
 * @param[in] num            ADC dma transmition trigger number(0-15)
 */
void ADC_SetDmaTrig(uint8_t num);

/**
 * @brief clear ADC FIFO
 */
void ADC_ClrFifo(void);

/**
 * @brief Get ADC-FIFO's empty status
 *
 * @return FIFO's status     0:not empty,1:empty
 */
uint8_t ADC_GetFifoEmpty(void);

/**
 * @brief Get ADC busy status
 *
 * @return                   1:busy, 0:not work
 */
uint8_t ADC_GetBusyStatus(void);

/**
 * @brief Set ADC input mode
 *
 * @param[in] mode           ADC input mode, see 'SADC_adcIputMode'
 */
void ADC_SetInputMode(SADC_adcIputMode mode);

/**
 * @brief Get ADC input mode
 *
 * @return                   ADC input mode, see 'SADC_adcIputMode'
*/
SADC_adcIputMode ADC_GetInputMode(void);

/**
 * @brief Set pga-para
 *
 * @param[in] para           pga-para, see 'SADC_pgaPara'
 * @note The conversion relation between pga-para and input voltage is:
 * 1.For differential input:
 * Vmin、Vmax stand for the minimum and maximum values of the gap between input
 * voltage VINP and VINN.
 * |pga-para| Vmin   | Vmax  |
 * | [000]  | -Vp    | Vp    |
 * | [001]  | -Vp/2  | Vp/2  |
 * | [010]  | -Vp/4  | Vp/4  |
 * | [011]  | -Vp/8  | Vp/8  |
 * | [100]  | -Vp/16 | Vp/16 |
 * | [101]  | -Vp/32 | Vp/32 |
 * 2.For single-ended input:
 * Vmin、Vmax stand for the minimum and maximum values of input voltage.
 * |pga-para| Vmin    | Vmax    |
 * | [000]  | 0       | Vp      |
 * | [001]  | 0       | Vp      |
 * | [010]  | Vp/4    | 3Vp/4   |
 * | [011]  | 3Vp/8   | 5Vp/8   |
 * | [100]  | 7Vp/16  | 9Vp/16  |
 * | [101]  | 15Vp/32 | 17Vp/32 |
 * other pga-para values are invalid
 *
 * Please get a good pga-para as parameter and call 'ADC_PgaParaSet' to set it.
 * So how to get a good pga-para? Please see below.
 * Steps to get a good pga-para:
 * 1.Please measure the wave range of input vlotage when it's in single-ended mode,
 * or the gap of VINP and VINN when it's in differential mode;
 * 2.Compare your range with the range between Vmin and Vmax in every row of those
 * table in the above, and let yours included in the second one;
 * 3.Do step-2 repeatedly with every row and find the biggest pga-para, is the best one.
 *
 * When PGA is disabled:
 * 1.pga-para=0 in differential mode, pga-para=1 in single-ended mode;
 * 2.Call ADC_PgaParaSet is useless, but it into effect when enable PGA next time.
 */
void ADC_PgaParaSet(SADC_pgaPara para);

/**
 * @brief Get pga-para
 *
 * @return                   pga-para, see 'SADC_pgaPara'
 */
SADC_pgaPara ADC_PgaParaGet(void);

/**
 * @brief Enable pga
 *
 * @param[in] enable         enable/disable pga
 */
void ADC_PgaEnable(uint8_t enable);

/**
 * @brief Get pga work status
 *
 * @return                   1:pga enabled, 0:pga disabled
 */
uint8_t ADC_GetPgaStatus(void);

/**
 * @brief Set ADC loop delay
 *
 * @param[in] delay          ADC loop delay(0-0xffffffff)
 */
void ADC_SetLoopDelay(uint32_t delay);

/**
 * @brief Get ADC interrupt-bit's value
 *
 * @return                   1:interrupt, 0:no interrupt
 */
uint8_t ADC_GetIntStatus(void);

/**
 * @brief Read ADC FIFO data(should work with ADC_GetDataChannel & ADC_GetData)
 *
 * @return                   data channel & ADC data
 */
uint32_t ADC_PopFifoData(void);

/**
 * @brief To get channel Id of the data read by ADC_PopFifoData
 *
 * @param[in] data           data read by ADC_PopFifoData
 * @return                   channel id, see 'SADC_channelId'
 */
SADC_channelId ADC_GetDataChannel(const uint32_t data);

/**
 * @brief To get ADC data of the data read by ADC_PopFifoData
 * @note The data obtained by this api is the raw data of the registers,
 *       which has not been processed in any way and may be biased.
 *
 * @param[in] data           data read by ADC_PopFifoData
 * @return                   ADC data
 */
uint16_t ADC_GetRawData(const uint32_t data);

/**
 * @brief To get ADC data of the data read by ADC_PopFifoData
 * @note The data obtained by this api has been calibrated by ft-data.
 *
 * @param[in] data           data read by ADC_PopFifoData
 * @return                   ADC data
 */
uint16_t ADC_GetData(const uint32_t data);

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
 * @brief Get ADC-Channel's enabled status
 * Example:
 * 1.single-mode with CH0/CH4/CH6 are enabled, it returns 0x51.
 * 2.differential-mode with CH1 is enabled, it returns 0x2.
 *
 * @param[in] channel_id     ADC input mode, see 'SADC_adcIputMode'
 * @return                   ADC-Channel's enabled status
 */
uint16_t ADC_GetEnabledChannels(void);

/**
 * @brief start ADC
 *
 * @param[in] start          start/stop ADC
 */
void ADC_Start(uint8_t start);

/**
 * @brief To Calibrate VREFP
 * @note Should call 'ADC_ftInit' first or it dosen't work.
 */
void ADC_VrefCalibration(void);

/**
 * @brief Initialization of ADC FT-Calibration
 * @note Should call this function before do ADC conversion or get voltage value.
 * And this function should be used again if 'ADC_AdcClose' is called.
 *
 * This function uses `flash_prepare_factory_data()`.
 */
void ADC_ftInit(void);

/**
 * @brief Initialization of ADC FT-Calibration(use static memory allocation)
 * @note Should call this function before do ADC conversion or get voltage value.
 * And this function should be used again if 'ADC_AdcClose' is called.
 *
 * This function uses `flash_prepare_factory_data()`.
 *
 * Calibration parameters (ftCali_ptr) must use global or static variables to allocate memory space.
 *
 * @param[in] ftCali_ptr     Calibrate parameter struct address.
 */
void ADC_ftInitCali(SADC_ftCali_t *ftCali_ptr);

/**
 * @brief Get voltage by ADC data
 * @note Should Calibrate VREFP by ADC_VrefCalibration first.
 *
 * @param[in] data           ADC data
 * @return                   voltage
 */
float ADC_GetVol(const uint16_t data);

/**
 * @brief Reset ADC configuration
 */
void ADC_Reset(void);

/**
 * @brief Close ADC
 * @note The api must ensure that the calibration parameters are initialized by ADC_ftInit before use.
 */
void ADC_AdcClose(void);

/**
 * @brief Close ADC
 * @note If the initialization API uses ADC_ftInitCali, exit the ADC should use this API.
 */
void ADC_AdcCloseCali(void);

/**
 * @brief ADC calibration standard configuration interface
 *
 * @param[in] mode           ADC input mode, see 'SADC_adcIputMode'
 */
void ADC_Calibration(SADC_adcIputMode mode);

/**
 * @brief ADC conversion standard configuration interface
 *
 * @param[in] ctrlMode       ADC control mode, see 'SADC_adcCtrlMode'
 * @param[in] pgaPara        pga-para, see 'SADC_pgaPara'
 * @param[in] pgaEnable      enable/diable pga
 * @param[in] ch             ADC channel id, see 'SADC_channelId'
 * @param[in] enNum          ADC interrupt trigger number(0-15)
 * @param[in] dmaEnNum       DMA transmition require trigger number(0-15)
 * @param[in] inputMode      ADC input mode, see 'SADC_adcIputMode'
 * @param[in] loopDelay      ADC loop delay(0-0xffffffff)
 */
void ADC_ConvCfg(SADC_adcCtrlMode ctrlMode,
                 SADC_pgaPara pgaPara,
                 uint8_t pgaEnable,
                 SADC_channelId ch,
                 uint8_t enNum,
                 uint8_t dmaEnNum,
                 SADC_adcIputMode inputMode,
                 uint32_t loopDelay);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

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
    PGA_PARA_0 = 0,
    PGA_PARA_1,
    PGA_PARA_2,
    PGA_PARA_3,
    PGA_PARA_4,
    PGA_PARA_5,
} SADC_pgaPara;

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
    float (*cb)(const uint16_t);
} SADC_adcCal_t;

typedef struct
{
    uint32_t k;
    uint32_t Coseq;
} SADC_ftChPara_t;
typedef struct
{
    uint32_t Vp;
    uint16_t V12Data;
    uint16_t (*f)(const SADC_ftChPara_t *, const uint32_t);
    SADC_ftChPara_t chParaSin[8];
    SADC_ftChPara_t chParaDiff[4];
    SADC_ftChPara_t chParaSinNoPga[12];
    SADC_ftChPara_t chParaDiffNoPga[4];
} SADC_ftCali_t;

/**
 * @brief Enable ADC control signal
 */
void ADC_EnableCtrlSignal(void);

/**
 * @brief Reset ADC control signal
 */
void ADC_ResetCtrlSignal(void);

/**
 * @brief Set ADC mode
 *
 * @param[in] mode           ADC mode, see 'SADC_adcMode'
 */
void ADC_SetAdcMode(SADC_adcMode mode);

/**
 * @brief Get ADC mode
 *
 * @return                   mode
 */
SADC_adcMode ADC_GetAdcMode(void);

/**
 * @brief Set ADC control mode
 *
 * @param[in] mode           ADC control mode, see 'SADC_adcCtrlMode'
 */
void ADC_SetAdcCtrlMode(SADC_adcCtrlMode mode);

/**
 * @brief Enable ADC channel
 *
 * @param[in] ch             ADC channel ID, see 'SADC_channelId'
 * @param[in] enable         enable/disable channel
 */
void ADC_EnableChannel(SADC_channelId ch, uint8_t enable);

/**
 * @brief Disable all ADC channels
 */
void ADC_DisableAllChannels(void);

/**
 * @brief Enable ADC interrupt
 *
 * @param[in] enable         enable/disable ADC interrupt
 */
void ADC_IntEnable(uint8_t enable);

/**
 * @brief Set ADC interrupt trigger number
 *
 * @param[in] num            ADC interrupt trigger number(0-15)
 */
void ADC_SetIntTrig(uint8_t num);

/**
 * @brief Enable ADC dma transmition request
 *
 * @param[in] enable         enable/disable ADC dma transmition request
 */
void ADC_DmaEnable(uint8_t enable);

/**
 * @brief Set ADC dma transmition trigger number
 *
 * @param[in] num            ADC dma transmition trigger number(0-15)
 */
void ADC_SetDmaTrig(uint8_t num);

/**
 * @brief clear ADC FIFO
 */
void ADC_ClrFifo(void);

/**
 * @brief Get ADC-FIFO's empty status
 *
 * @return FIFO's status     0:not empty,1:empty
 */
uint8_t ADC_GetFifoEmpty(void);

/**
 * @brief Get ADC busy status
 *
 * @return                   1:busy, 0:not work
 */
uint8_t ADC_GetBusyStatus(void);

/**
 * @brief Set ADC input mode
 *
 * @param[in] mode           ADC input mode, see 'SADC_adcIputMode'
 */
void ADC_SetInputMode(SADC_adcIputMode mode);

/**
 * @brief Get ADC input mode
 *
 * @return                   ADC input mode, see 'SADC_adcIputMode'
*/
SADC_adcIputMode ADC_GetInputMode(void);

/**
 * @brief Set pga-para
 *
 * @param[in] para           pga-para, see 'SADC_pgaPara'
 * @note The conversion relation between pga-para and input voltage is:
 * 1.For differential input:
 * Vmin、Vmax stand for the minimum and maximum values of the gap between input
 * voltage VINP and VINN.
 * |pga-para| Vmin   | Vmax  |
 * | [000]  | -Vp    | Vp    |
 * | [001]  | -Vp/2  | Vp/2  |
 * | [010]  | -Vp/4  | Vp/4  |
 * | [011]  | -Vp/8  | Vp/8  |
 * | [100]  | -Vp/16 | Vp/16 |
 * | [101]  | -Vp/32 | Vp/32 |
 * 2.For single-ended input:
 * Vmin、Vmax stand for the minimum and maximum values of input voltage.
 * |pga-para| Vmin    | Vmax    |
 * | [000]  | 0       | Vp      |
 * | [001]  | 0       | Vp      |
 * | [010]  | Vp/4    | 3Vp/4   |
 * | [011]  | 3Vp/8   | 5Vp/8   |
 * | [100]  | 7Vp/16  | 9Vp/16  |
 * | [101]  | 15Vp/32 | 17Vp/32 |
 * other pga-para values are invalid
 *
 * Please get a good pga-para as parameter and call 'ADC_PgaParaSet' to set it.
 * So how to get a good pga-para? Please see below.
 * Steps to get a good pga-para:
 * 1.Please measure the wave range of input vlotage when it's in single-ended mode,
 * or the gap of VINP and VINN when it's in differential mode;
 * 2.Compare your range with the range between Vmin and Vmax in every row of those
 * table in the above, and let yours included in the second one;
 * 3.Do step-2 repeatedly with every row and find the biggest pga-para, is the best one.
 *
 * When PGA is disabled:
 * 1.pga-para=0 in differential mode, pga-para=1 in single-ended mode;
 * 2.Call ADC_PgaParaSet is useless, but it into effect when enable PGA next time.
 */
void ADC_PgaParaSet(SADC_pgaPara para);

/**
 * @brief Get pga-para
 *
 * @return                   pga-para, see 'SADC_pgaPara'
 */
SADC_pgaPara ADC_PgaParaGet(void);

/**
 * @brief Enable pga
 *
 * @param[in] enable         enable/disable pga
 */
void ADC_PgaEnable(uint8_t enable);

/**
 * @brief Get pga work status
 *
 * @return                   1:pga enabled, 0:pga disabled
 */
uint8_t ADC_GetPgaStatus(void);

/**
 * @brief Set ADC loop delay
 *
 * @param[in] delay          ADC loop delay(0-0xffffffff)
 */
void ADC_SetLoopDelay(uint32_t delay);

/**
 * @brief Get ADC interrupt-bit's value
 *
 * @return                   1:interrupt, 0:no interrupt
 */
uint8_t ADC_GetIntStatus(void);

/**
 * @brief Read ADC FIFO data(should work with ADC_GetDataChannel & ADC_GetData)
 *
 * @return                   data channel & ADC data
 */
uint32_t ADC_PopFifoData(void);

/**
 * @brief To get channel Id of the data read by ADC_PopFifoData
 *
 * @param[in] data           data read by ADC_PopFifoData
 * @return                   channel id, see 'SADC_channelId'
 */
SADC_channelId ADC_GetDataChannel(const uint32_t data);

/**
 * @brief To get ADC data of the data read by ADC_PopFifoData
 *
 * @param[in] data           data read by ADC_PopFifoData
 * @return                   ADC data
 */
uint16_t ADC_GetData(const uint32_t data);

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
 * @brief Get ADC-Channel's enabled status
 * Example:
 * 1.single-mode with CH0/CH4/CH6 are enabled, it returns 0x51.
 * 2.differential-mode with CH1 is enabled, it returns 0x2.
 *
 * @param[in] channel_id     ADC input mode, see 'SADC_adcIputMode'
 * @return                   ADC-Channel's enabled status
 */
uint16_t ADC_GetEnabledChannels(void);

/**
 * @brief start ADC
 *
 * @param[in] start          start/stop ADC
 */
void ADC_Start(uint8_t start);

/**
 * @brief To Calibrate VREFP
 * @note Should call 'ADC_ftInit' first or it dosen't work.
 */
void ADC_VrefCalibration(void);

/**
 * @brief Initialization of ADC FT-Calibration
 * @note Should call this function before do ADC conversion or get voltage value.
 * And this function should be used again if 'ADC_AdcClose' is called.
 *
 * This function uses `flash_prepare_factory_data()`.
 */
void ADC_ftInit(void);

/**
 * @brief Initialization of ADC FT-Calibration(use static memory allocation)
 * @note Should call this function before do ADC conversion or get voltage value.
 * And this function should be used again if 'ADC_AdcClose' is called.
 *
 * This function uses `flash_prepare_factory_data()`.
 *
 * Calibration parameters (ftCali_ptr) must use global or static variables to allocate memory space.
 *
 * @param[in] ftCali_ptr     Calibrate parameter struct address.
 */
void ADC_ftInitCali(SADC_ftCali_t *ftCali_ptr);

/**
 * @brief Get voltage by ADC data
 * @note Should Calibrate VREFP by ADC_VrefCalibration first.
 *
 * @param[in] data           ADC data
 * @return                   voltage
 */
float ADC_GetVol(const uint16_t data);

/**
 * @brief Reset ADC configuration
 */
void ADC_Reset(void);

/**
 * @brief Close ADC
 */
void ADC_AdcClose(void);

/**
 * @brief ADC calibration standard configuration interface
 *
 * @param[in] mode           ADC input mode, see 'SADC_adcIputMode'
 */
void ADC_Calibration(SADC_adcIputMode mode);

/**
 * @brief ADC conversion standard configuration interface
 *
 * @param[in] ctrlMode       ADC control mode, see 'SADC_adcCtrlMode'
 * @param[in] pgaPara        pga-para, see 'SADC_pgaPara'
 * @param[in] pgaEnable      enable/diable pga
 * @param[in] ch             ADC channel id, see 'SADC_channelId'
 * @param[in] enNum          ADC interrupt trigger number(0-15)
 * @param[in] dmaEnNum       DMA transmition require trigger number(0-15)
 * @param[in] inputMode      ADC input mode, see 'SADC_adcIputMode'
 * @param[in] loopDelay      ADC loop delay(0-0xffffffff)
 */
void ADC_ConvCfg(SADC_adcCtrlMode ctrlMode,
                 SADC_pgaPara pgaPara,
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
