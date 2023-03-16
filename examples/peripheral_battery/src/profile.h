#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>

#define AVE_NUM 5

/**
 * @brief To get the ADC data in average
 * @note It returns average value in continue mode, equals to 'ADC_GetData' in single mode.
 * To avoid get fluctuant single data, is a good way to configure ADC in continue mode with 'AVE_NUM' 
 * data per trigger, call this function 'AVE_NUM' times with raw-data to get a data more accurate.
 * 
 * @param[in] data           data read by ADC_PopFifoData
 * @return                   ADC data
 */
uint16_t ADC_GetAveData(uint32_t data);

/**
 * @brief Disable all ADC average data memory
 * @note Call this function to free all ADC average data memory. And you need to rebuild
 * them before next calling of function 'ADC_Start(1)'.
 * You should have got all data in ADC average data memory or you don't need them, for they
 * will be discarded after calling 'ADC_AveDisable'.
 */
void ADC_AveDisable(void);

uint32_t setup_profile(void *data, void *user_data);

#endif


