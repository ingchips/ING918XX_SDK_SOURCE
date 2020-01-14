#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>

#include "cm32gpm3.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#define ADC              ((ADC_Type          *)(SYSCTRL_BASE + 0x30))
#define ADC_Pwr_Ctrl     ((ADC_Pwr_Ctrl_Type *)(APB_RTC_BASE + 0x90))

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

#ifdef __cplusplus
}
#endif

#endif
