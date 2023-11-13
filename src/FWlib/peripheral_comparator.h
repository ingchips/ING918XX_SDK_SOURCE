
/*
** COPYRIGHT (c) 2023 by INGCHIPS
*/

#ifndef __PERIPHERAL_COMPARATOR_H__
#define __PERIPHERAL_COMPARATOR_H__

#include "ingsoc.h"

#ifdef __cplusplus
    extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

typedef enum
{
    COMPARATOR_VINP0_GPIO4 = 0,
    COMPARATOR_VINP1_GPIO18   ,
    COMPARATOR_VINP2_GPIO20   ,
    COMPARATOR_VINP3_GPIO26   ,
    COMPARATOR_VINP4_GPIO27   ,
    COMPARATOR_VINP5_GPIO28   ,
    COMPARATOR_VINP6_GPIO1    ,
    COMPARATOR_VINP7_GPIO2    ,
    COMPARATOR_VINP_NUMBER    ,
} COMPARATOR_Vinp_t;

typedef enum
{
    COMPARATOR_VINN0_GPIO3 = 0,
    COMPARATOR_VINN1_GPIO19   ,
    COMPARATOR_VINN2_GPIO32   ,
    COMPARATOR_VINN3_GPIO33   ,
    COMPARATOR_VINN4_GPIO15   ,
    COMPARATOR_VINN7_VREF  = 7,
    COMPARATOR_VINN_NUMBER    ,
} COMPARATOR_Vinn_t;

typedef enum
{
    COMPARATOR_DISABLE = 0,
    COMPARATOR_ENABLE     ,
} COMPARATOR_Enable_t;

typedef enum
{
    COMPARATOR_NO_HYSTERRESIS = 0,
    COMPARATOR_30mV_HYSTERRESIS  ,
} COMPARATOR_HysteresisValtage_t;

typedef enum
{
    COMPARATOR_1_16 = 0,
    COMPARATOR_2_16    ,
    COMPARATOR_3_16    ,
    COMPARATOR_4_16    ,
    COMPARATOR_5_16    ,
    COMPARATOR_6_16    ,
    COMPARATOR_7_16    ,
    COMPARATOR_8_16    ,
    COMPARATOR_9_16    ,
    COMPARATOR_10_16   ,
    COMPARATOR_11_16   ,
    COMPARATOR_12_16   ,
    COMPARATOR_13_16   ,
    COMPARATOR_14_16   ,
    COMPARATOR_15_16   ,
    COMPARATOR_16_16   ,
} COMPARATOR_VinnDivision_t;

typedef enum
{
    COMPARATOR_ULTRA_LOW_POWER = 0,
    COMPARATOR_LOW_POWER          ,
    COMPARATOR_MIDDLE_POWER       ,
    COMPARATOR_HIGH_POWER         ,
} COMPARATOR_WorkMode_t;

typedef enum
{
    COMPARATOR_OUT1_P_GREATE_N = 0,
    COMPARATOR_OUT1_N_GREATE_P    ,
} COMPARATOR_OutputPolarity_t;

typedef struct {
    COMPARATOR_Vinp_t v_in_p;
    COMPARATOR_Vinn_t v_in_n;
    COMPARATOR_Enable_t en;
    COMPARATOR_HysteresisValtage_t hysteresis;
    COMPARATOR_VinnDivision_t vinn_division;
    COMPARATOR_WorkMode_t work_mode;
    COMPARATOR_OutputPolarity_t output_polarity;
} COMPARATOR_SetStateStruct;

typedef enum
{
    COMPARATOR_HIGH_LEVEL_WAKEUP = 0,
    COMPARATOR_LOW_LEVEL_WAKEUP     ,
} COMPARATOR_LEVEL_WAKEUP_t;

typedef enum
{
    COMPARATOR_LEVEL_WAKEUP_MODE            = 0,
    COMPARATOR_POSEDGE_WAKEUP_MODE             ,
    COMPARATOR_NEGEDGE_WAKEUP_MODE             ,
    COMPARATOR_POSEDGE_AND_NEGEDGE_WAKEUP_MODE ,
} COMPARATOR_WAKEUP_MODE_t;

typedef enum
{
    COMPARATOR_DISABLE_INT_MODE          = 0,
    COMPARATOR_NEGEDGE_INT_MODE             ,
    COMPARATOR_POSEDGE_INT_MODE             ,
    COMPARATOR_POSEDGE_AND_NEGEDGE_INT_MODE ,
} COMPARATOR_INTERRUPT_MODE_t;

/**
 * @brief Initialize comparator module
 *
 * IO pins are also initialzed here.
 * 
 * Note: Comparator's internal configurations are kept during power saving,
 * so, this function do not need to be called after wake up.
 *
 * @param[in] cmp_set           Initial parameter struct
 */
void COMPARATOR_Initialize(const COMPARATOR_SetStateStruct* cmp_set);

/**
 * @brief Initialize IO pins for comparator module
 *
 * Note: Unlike `COMPARATOR_Initialize()`, this function may need to be 
 *       called after each wake up. See also the retention functionality of GPIO.
 *
 * @param[in] cmp_set           Initial parameter struct
 */
void COMPARATOR_InitializePins(const COMPARATOR_SetStateStruct* cmp_set);

/**
 * @brief Get comparator result
 *
 * @return                      comparator result
 */
uint8_t COMPARATOR_GetComparatorResult(void);

/**
 * @brief Set comparator as wakeup source from DEEP SLEEP mode
 *
 * @param[in] enable            0:disable, 1:enable
 * @param[in] level             COMPARATOR_LEVEL_WAKEUP_t
 * @param[in] wakeup_mode       COMPARATOR_WAKEUP_MODE_t
 * @return                      0:OK, other:error
 */
int COMPARATOR_SetDeepSleepWakeupSource(uint8_t enable,
                                        COMPARATOR_LEVEL_WAKEUP_t level,
                                        COMPARATOR_WAKEUP_MODE_t wakeup_mode);

/**
 * @brief Set comparator as interrupt source
 *
 * @param[in] int_mode          COMPARATOR_INTERRUPT_MODE_t
 * @return                      0:OK, other:error
 */
int COMPARATOR_SetInterrupt(COMPARATOR_INTERRUPT_MODE_t int_mode);

/**
 * @brief Get comparator interrupt status and clear interrupt flag
 *
 * @return                      interrupt status
 */
COMPARATOR_INTERRUPT_MODE_t COMPARATOR_GetIntStatusAndClear(void);

#endif

#ifdef __cplusplus
    }
#endif

#endif
