#ifndef adc_cali_h
#define adc_cali_h

#include <stdint.h>

enum adc_cali_method
{
    ADC_CALI_METHOD_1 = 1,      // calibration is done in factory
    ADC_CALI_METHOD_2,          // calibration is done in factory
    ADC_CALI_METHOD_SELF        // self-calibration
};

enum adc_sample_mode
{
    ADC_SAMPLE_MODE_SLOW,       // for ADC_CLK_128
    ADC_SAMPLE_MODE_FAST,       // for ADC_CLK_16
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief Prepare ADC calibration
 *
 *  Note: if any calibration method has been done, then the method is selected;
 *        otherwise, do self-calibration, then reset.
 *
 * @return              Selected calibration method
 ****************************************************************************************
 */
enum adc_cali_method adc_prepare_calibration(void);

/**
 ****************************************************************************************
 * @brief Calibrate raw ADC readings
 *
 *  Note: `adc_prepare_calibration` must be called before using this.
 *
 * @param[in]  mode             sample mode
 * @param[in]  channel_id       ADC channel ID of the value
 * @param[in]  value            raw ADC reading
 * @return                      calibrated ADC value
 ****************************************************************************************
 */
uint16_t adc_calibrate(enum adc_sample_mode mode, uint8_t channel_id, uint16_t value);

#ifdef __cplusplus
}
#endif

#endif
