#ifndef __PERIPHERAL_PTE_H__
#define __PERIPHERAL_PTE_H__

#ifdef    __cplusplus
extern "C" {    /* allow C++ to use these headers */
#endif    /* __cplusplus */

#include "ingsoc.h"
#include "peripheral_sysctrl.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

typedef enum
{
    SYSCTRL_PTE_CHANNEL_0     = 0,
    SYSCTRL_PTE_CHANNEL_1     = 1,
    SYSCTRL_PTE_CHANNEL_2     = 2,
    SYSCTRL_PTE_CHANNEL_3     = 3,
} SYSCTRL_PTE_CHANNEL_ID;

typedef enum
{
    SYSCTRL_PTE_I2C0_INT       = 0,
    SYSCTRL_PTE_I2C1_INT       = 1,
    SYSCTRL_PTE_SARADC_INT     = 2,
    SYSCTRL_PTE_I2S_INT        = 3,
    SYSCTRL_PTE_DMA_INT        = 4,
    SYSCTRL_PTE_IR_INT         = 5,
    SYSCTRL_PTE_KEYSCANNER_INT = 6,
    SYSCTRL_PTE_PWMC0_INT      = 7,
    SYSCTRL_PTE_PWMC1_INT      = 8,
    SYSCTRL_PTE_PWMC2_INT      = 9,
    SYSCTRL_PTE_TIMER0_INT     = 10,
    SYSCTRL_PTE_TIMER1_INT     = 11,
    SYSCTRL_PTE_TIMER2_INT     = 12,
    SYSCTRL_PTE_GPIO0_INT      = 13,
    SYSCTRL_PTE_GPIO1_INT      = 14,
    SYSCTRL_PTE_UART0_INT      = 15,
    SYSCTRL_PTE_UART1_INT      = 16,
    SYSCTRL_PTE_SPI0_INT       = 17,
    SYSCTRL_PTE_SPI1_INT       = 18,
    SYSCTRL_PTE_SPIFLASH       = 19,
    SYSCTRL_PTE_RCT_CNT        = 20,
    SYSCTRL_PTE_IR_WAKEUP      = 21,
    SYSCTRL_PTE_USB_INT        = 22,
    SYSCTRL_PTE_QDEC_INT       = 23,

    SYSCTRL_PTE_SRC_INT_MAX    = 24,
} SYSCTRL_PTE_SRC_INT;

typedef enum
{
    SYSCTRL_PTE_I2C0_EN        = 0,
    SYSCTRL_PTE_I2C1_EN        = 1,
    SYSCTRL_PTE_SARADC_EN      = 2,
    SYSCTRL_PTE_I2S_TX_EN      = 3,
    SYSCTRL_PTE_I2S_RX_EN      = 4,
    SYSCTRL_PTE_IR_EN          = 5,
    SYSCTRL_PTE_KEYSCANNER_EN  = 6,
    SYSCTRL_PTE_PWMC0_EN       = 7,
    SYSCTRL_PTE_PWMC1_EN       = 8,
    SYSCTRL_PTE_PWMC2_EN       = 9,
    SYSCTRL_PTE_TIMER0_CH0_EN  = 10,
    SYSCTRL_PTE_TIMER0_CH1_EN  = 11,
    SYSCTRL_PTE_TIMER1_CH0_EN  = 12,
    SYSCTRL_PTE_TIMER1_CH1_EN  = 13,
    SYSCTRL_PTE_TIMER2_CH0_EN  = 14,
    SYSCTRL_PTE_TIMER2_CH1_EN  = 15,

    SYSCTRL_PTE_DST_EN_MAX     = 16,
} SYSCTRL_PTE_DST_EN;

#define PTE_MAKE_MASK_SIZE_OPTION(size)                 (((1) << ((uint32_t)(size))) - (1))
#define PTE_MAKE_CHANNEL_INT_MASK_OPTION(options)       ((~((0x1) << (1))) & (~((options) << (26))))

/**
 * @brief Standard process of PTE's irq-function
 *
 * @param[in] ch             PTE channel ID
 */
void PTE_IrqProc(SYSCTRL_PTE_CHANNEL_ID ch);

/**
 * @brief Standard process of out peripheral's irq-function with channel continue enabled.
 *
 * @param[in] ch             PTE channel ID
 */
void PTE_OutPeripheralContinueProc(SYSCTRL_PTE_CHANNEL_ID ch);

/**
 * @brief Standard process of out peripheral's irq-function with channel disabled.
 *
 * @param[in] ch             PTE channel ID
 */
void PTE_OutPeripheralEndProc(SYSCTRL_PTE_CHANNEL_ID ch);

/**
 * @brief Close PTE channel
 *
 * @param[in] ch             PTE channel ID
 */
void PTE_ChannelClose(SYSCTRL_PTE_CHANNEL_ID ch);

/**
 * @brief Enable PTE channel
 *
 * @param[in] ch             PTE channel ID
 */
void PTE_EnableChannel(SYSCTRL_PTE_CHANNEL_ID ch);

/**
 * @brief Connect two peripherals with PTE
 *
 * @param[in] ch             PTE channel ID
 * @param[in] src            source peripheral
 * @param[in] dst            destination peripheral
 * @return                   0 if no error else non-0
 */
int PTE_ConnectPeripheral(SYSCTRL_PTE_CHANNEL_ID ch,
                          SYSCTRL_PTE_SRC_INT src,
                          SYSCTRL_PTE_DST_EN dst);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

/*
 * PTE EVENT enum
 */
typedef enum
{
    PTE_TASK_QDEC_CH0_START = 0,
    PTE_TASK_QDEC_CH0_STOP,
    PTE_TASK_QDEC_CH1_START,
    PTE_TASK_QDEC_CH1_STOP,
    PTE_TASK_QDEC_CH2_START,
    PTE_TASK_QDEC_CH2_STOP,

    PTE_TASK_GPIOTE_TOGGLE_CH0 = 0,
    PTE_TASK_GPIOTE_TOGGLE_CH1,
    PTE_TASK_GPIOTE_TOGGLE_CH2,
    PTE_TASK_GPIOTE_TOGGLE_CH3,
    PTE_TASK_GPIOTE_TOGGLE_CH4,
    PTE_TASK_GPIOTE_TOGGLE_CH5,
    PTE_TASK_GPIOTE_TOGGLE_CH6,
    PTE_TASK_GPIOTE_TOGGLE_CH7,
    PTE_TASK_GPIOTE_HIGH_CH0,
    PTE_TASK_GPIOTE_HIGH_CH1,
    PTE_TASK_GPIOTE_HIGH_CH2,
    PTE_TASK_GPIOTE_HIGH_CH3,
    PTE_TASK_GPIOTE_HIGH_CH4,
    PTE_TASK_GPIOTE_HIGH_CH5,
    PTE_TASK_GPIOTE_HIGH_CH6,
    PTE_TASK_GPIOTE_HIGH_CH7,
    PTE_TASK_GPIOTE_LOW_CH0,
    PTE_TASK_GPIOTE_LOW_CH1,
    PTE_TASK_GPIOTE_LOW_CH2,
    PTE_TASK_GPIOTE_LOW_CH3,
    PTE_TASK_GPIOTE_LOW_CH4,
    PTE_TASK_GPIOTE_LOW_CH5,
    PTE_TASK_GPIOTE_LOW_CH6,
    PTE_TASK_GPIOTE_LOW_CH7,

    PTE_TASK_TIMER_CH0_TM0_START = 0,
    PTE_TASK_TIMER_CH0_TM1_START,
    PTE_TASK_TIMER_CH0_TM2_START,
    PTE_TASK_TIMER_CH0_TM3_START,
    PTE_TASK_TIMER_CH1_TM0_START,
    PTE_TASK_TIMER_CH1_TM1_START,
    PTE_TASK_TIMER_CH1_TM2_START,
    PTE_TASK_TIMER_CH1_TM3_START,
    PTE_TASK_TIMER_CH0_TM0_STOP,
    PTE_TASK_TIMER_CH0_TM1_STOP,
    PTE_TASK_TIMER_CH0_TM2_STOP,
    PTE_TASK_TIMER_CH0_TM3_STOP,
    PTE_TASK_TIMER_CH1_TM0_STOP,
    PTE_TASK_TIMER_CH1_TM1_STOP,
    PTE_TASK_TIMER_CH1_TM2_STOP,
    PTE_TASK_TIMER_CH1_TM3_STOP,

    PTE_TASK_SPI_MASTER_DMA_TX_START = 0,
    PTE_TASK_SPI_MASTER_DMA_TX_STOP,

    PTE_TASK_I2C_MASTER_DMA_TX = 0,

    PTE_TASK_ADC_START = 0,
    PTE_TASK_ADC_LOOP,
    PTE_TASK_ADC_DMA_STOP_TX,

    PTE_TASK_UART_DMA_TX_START = 0,
    PTE_TASK_UART_DMA_TX_STOP,
    PTE_TASK_UART_DMA_RX_START,
    PTE_TASK_UART_DMA_RX_STOP,

    PTE_TASK_PWM_CH0_IR_START = 0,
    PTE_TASK_PWM_CH0_ONE_PULSE,
    PTE_TASK_PWM_CH0_PWM_START,
    PTE_TASK_PWM_CH0_PWM_DMA_START,
    PTE_TASK_PWM_CH0_PCM_START,
    PTE_TASK_PWM_CH0_PWM_STOP,
    PTE_TASK_PWM_CH0_PCM_STOP,
    PTE_TASK_PWM_CH1_IR_START,
    PTE_TASK_PWM_CH1_ONE_PULSE,
    PTE_TASK_PWM_CH1_PWM_START,
    PTE_TASK_PWM_CH1_PWM_DMA_START,
    PTE_TASK_PWM_CH1_PCM_START,
    PTE_TASK_PWM_CH1_PWM_STOP,
    PTE_TASK_PWM_CH1_PCM_STOP,
    PTE_TASK_PWM_CH2_IR_START,
    PTE_TASK_PWM_CH2_ONE_PULSE,
    PTE_TASK_PWM_CH2_PWM_START,
    PTE_TASK_PWM_CH2_PWM_DMA_START,
    PTE_TASK_PWM_CH2_PCM_START,
    PTE_TASK_PWM_CH2_PWM_STOP,
    PTE_TASK_PWM_CH2_PCM_STOP,

    PTE_TASK_I2S_DMA_TX_START = 0,
    PTE_TASK_I2S_DMA_RX_START,
    PTE_TASK_I2S_DMA_TX_STOP,
    PTE_TASK_I2S_DMA_RX_STOP,

    PTE_TASK_ASDM_RX_START = 0,
    PTE_TASK_ASDM_RX_STOP,
}PTE_Task;

/*
 * PTE EVENT enum
 */
typedef enum
{
    PTE_EVENT_QDEC_CH0_STOP_CPL = 0,
    PTE_EVENT_QDEC_CH1_STOP_CPL,
    PTE_EVENT_QDEC_CH2_STOP_CPL,

    PTE_EVENT_DMA_CH0_STOP_CPL = 0,
    PTE_EVENT_DMA_CH1_STOP_CPL,
    PTE_EVENT_DMA_CH2_STOP_CPL,
    PTE_EVENT_DMA_CH3_STOP_CPL,
    PTE_EVENT_DMA_CH4_STOP_CPL,
    PTE_EVENT_DMA_CH5_STOP_CPL,
    PTE_EVENT_DMA_CH6_STOP_CPL,
    PTE_EVENT_DMA_CH7_STOP_CPL,

    PTE_EVENT_GPIOTE_CH0_INPUT_INT = 0,
    PTE_EVENT_GPIOTE_CH1_INPUT_INT,
    PTE_EVENT_GPIOTE_CH2_INPUT_INT,
    PTE_EVENT_GPIOTE_CH3_INPUT_INT,
    PTE_EVENT_GPIOTE_CH4_INPUT_INT,
    PTE_EVENT_GPIOTE_CH5_INPUT_INT,
    PTE_EVENT_GPIOTE_CH6_INPUT_INT,
    PTE_EVENT_GPIOTE_CH7_INPUT_INT,

    PTE_EVENT_TIMER_CH0_TIM0_TIM_UP = 0,
    PTE_EVENT_TIMER_CH0_TIM1_TIM_UP,
    PTE_EVENT_TIMER_CH0_TIM2_TIM_UP,
    PTE_EVENT_TIMER_CH0_TIM3_TIM_UP,
    PTE_EVENT_TIMER_CH1_TIM0_TIM_UP,
    PTE_EVENT_TIMER_CH1_TIM1_TIM_UP,
    PTE_EVENT_TIMER_CH1_TIM2_TIM_UP,
    PTE_EVENT_TIMER_CH1_TIM3_TIM_UP,

    PTE_EVENT_SPI_TX_END = 0,

    PTE_EVENT_I2C_TX_CPL = 0,

    PTE_EVENT_ADC_DATA_CPL = 0,
    PTE_EVENT_ADC_DMA_STOP_TX,

    PTE_EVENT_UART_TX_INT = 0,
    PTE_EVENT_UART_RX_INT,

    PTE_EVENT_PWM_CH0_PWM_STOP = 0,
    PTE_EVENT_PWM_CH0_FIFO_TRG,
    PTE_EVENT_PWM_CH0_PCM_STOP,
    PTE_EVENT_PWM_CH1_PWM_STOP,
    PTE_EVENT_PWM_CH1_FIFO_TRG,
    PTE_EVENT_PWM_CH1_PCM_STOP,
    PTE_EVENT_PWM_CH2_PWM_STOP,
    PTE_EVENT_PWM_CH2_FIFO_TRG,
    PTE_EVENT_PWM_CH2_PCM_STOP,

    PTE_EVENT_ASDM_RX_DATA = 0,
}PTE_Event;

/**
 * @brief PTE can used modules
 *
 * Range: 0 ~ 14
 *
 * Deafault: PTE_QDEC_MODULE
 */
typedef enum
{
    PTE_QDEC_MODULE = 0,
    PTE_DMA_MODULE,
    PTE_GPIOTE_MODULE,
    PTE_TIMER0_MODULE,
    PTE_QSPI_MODULE,
    PTE_I2C_MODULE,
    PTE_ADC_MODULE,
    PTE_UART0_MODULE,
    PTE_UART1_MODULE,
    PTE_SPI_MODULE,
    PTE_PWMC_MODULE,
    PTE_TIMER1_MODULE,
    PTE_I2S_MODULE,
    PTE_ASDM_MODULE,
    PTE_MODULE_MAX,
}PTE_Module;

/**
 * @brief PTE Channel
 *
 * Range: 0 ~ 15
 *
 * Deafault: channel 0
 */
typedef enum
{
    PTE_CHANNEL_0 = 0,
    PTE_CHANNEL_1,
    PTE_CHANNEL_2,
    PTE_CHANNEL_3,
    PTE_CHANNEL_4,
    PTE_CHANNEL_5,
    PTE_CHANNEL_6,
    PTE_CHANNEL_7,
    PTE_CHANNEL_8,
    PTE_CHANNEL_9,
    PTE_CHANNEL_10,
    PTE_CHANNEL_11,
    PTE_CHANNEL_12,
    PTE_CHANNEL_13,
    PTE_CHANNEL_14,
    PTE_CHANNEL_15,
}PTE_Channel;

/**
 * @brief GPIOTE channel
 *
 * Range: 0 ~ 7
 */
typedef enum
{
    GPIOTE_CHANNEL_0 = 0,
    GPIOTE_CHANNEL_1,
    GPIOTE_CHANNEL_2,
    GPIOTE_CHANNEL_3,
    GPIOTE_CHANNEL_4,
    GPIOTE_CHANNEL_5,
    GPIOTE_CHANNEL_6,
    GPIOTE_CHANNEL_7,
} GPIOTE_Channel;

/**
 * @brief GPIOTE channel mode
 *
 * Range: [0, 3]
 *
 * Default: GPIOTE_CHANNEL_DISABLE
 */
typedef enum
{
    GPIOTE_CHANNEL_DISABLE = 0,
    GPIOTE_CHANNEL_EVENT = 1,
    GPIOTE_CHANNEL_TASK = 3,
} GPIOTE_Mode;

/**
 * @brief PTEC channel group
 *
 * Range: [0, 1]
 *
 * Default: ChannelGroup0
 */
typedef enum
{
    PTEC_CHANNEL_GROUP_0 = 0,
    PTEC_CHANNEL_GROUP_1,
    PTEC_CHANNEL_GROUP_2,
    PTEC_CHANNEL_GROUP_3,
}PTEC_ChannelGroup;

/**
 * @brief PTE channel enabled
 *
 * @param items             combination of bits whose positions are listed in `PTE_Channel`
 * @return                  None
 * @example Enable channel0 and channel1 PTE_ChxEnable((1<<PTE_CHANNEL_0) | (1<<PTE_CHANNEL_1))
 */
void PTE_ChxEnable(uint32_t items);

/**
 * @brief PTE channel disable
 *
 * @param items             combination of bits whose positions are listed in `PTE_Channel`
 * @return                  None
 * @example Disable channel0 and channel1 PTE_ChxEnable((1<<PTE_CHANNEL_0) | (1<<PTE_CHANNEL_1))
 */
void PTE_ChxDisable(uint32_t items);

/**
 * @brief Get PTE channel enable state
 *
 * @param None
 * @return  combination of bits whose positions are listed in `PTE_Channel`
 */
uint32_t PTE_ChxGetEnableState(void);

/**
 * @brief Trigger task of the peripheral module PTE
 *
 * @param module      Set PTE peripheral module. Must be one of `PTE_Module`
 * @param Task       Set PTE task event. Must be one of `PTE_Task`
 * @return                 0 : success, 1 : input arg error
 * @example Trigger Qdec module task,task is QDEC_TMR_STOP
 *          PTE_TriggerTask(PTE_MODULE_QDEC, PTE_TASK_QDEC_CH0_START)
 */
uint8_t PTE_TriggerTask(PTE_Module module, PTE_Task Task);

/**
 * @brief Enable PTE channel group, already send group task signal.
 *
 * @param SetChannelGroup      PTE group. Must be one of `PTEC_ChannelGroup`
 * @return                 None
 * @example Enbale PTE channel group 0
 *          PTE_SetTaskChxGroupEN(PTEC_CHANNEL_GROUP_0)
 */
void PTE_SetTaskChxGroupEN(PTEC_ChannelGroup SetChannelGroup);

/**
 * @brief Disable PTE channel group, stop group task signal.
 *
 * @param SetChannelGroup      PTE group. Must be one of `PTEC_ChannelGroup`
 * @return                 None
 * @example Disable PTE channel group 0
 *          PTE_SetTaskChxGroupDis(PTEC_CHANNEL_GROUP_0)
 */
void PTE_SetTaskChxGroupDis(PTEC_ChannelGroup SetChannelGroup);

/**
 * @brief Configure PTE channel group enable subscription
 *
 * @param SetChannelGroup      PTE group. Must be one of `PTEC_ChannelGroup`
 * @param SetSubChannel          combination of bits whose positions are listed in `PTEC_ChannelGroup`
 * @return                 None
 * @example Cfg PTE channel group 0,subscription of TASKS_CHGX_EN Group1
 *          PTE_SetTaskSubChxGroupEn(PTEC_CHANNEL_GROUP_0,(1<<PTEC_CHANNEL_GROUP_1))
 */
void PTE_SetTaskSubChxGroupEn(PTEC_ChannelGroup SetChannelGroup,PTE_Channel SetSubChannel);

/**
 * @brief Configure PTE channel group disable subscription
 *
 * @param SetChannelGroup      PTE group. Must be one of `PTEC_ChannelGroup`
 * @param SetSubChannel          combination of bits whose positions are listed in `PTEC_ChannelGroup`
 * @return                 None
 * @example Cfg PTE channel group 0,subscription of TASKS_CHGX_DIS Group1
 *          PTE_SetTaskSubChxGroupDis(PTEC_CHANNEL_GROUP_0,(1<<PTEC_CHANNEL_GROUP_1))
 */
void PTE_SetTaskSubChxGroupDis(PTEC_ChannelGroup SetChannelGroup,PTE_Channel SetSubChannel);

/**
 * @brief Configure PTE channel group map
 *
 * @param PTEC_ChannelGroup      PTE group. Must be one of `PTEC_ChannelGroup`
 * @param SetSubChannel          combination of bits whose positions are listed in `PTE_Channel`
 * @return                 None
 *
 * @note A group only can map up to 4 channels
 *
 * @example Map PTE channel group 0, PTE_CHANNEL_0 to PTE_CHANNEL_3 is a group
 *          PTE_SetTaskChxGroupMap(PTEC_CHANNEL_GROUP_0,(1<<PTE_CHANNEL_0)|(1<<PTE_CHANNEL_1)|(1<<PTE_CHANNEL_2)|(1<<PTE_CHANNEL_3)
 */
void PTE_SetTaskChxGroupMap(PTEC_ChannelGroup SetChannelGroup,uint32_t SetGroupMap);

/**
 * @brief Configure task parameters of the PTE peripheral module
 *
 * @param module      Set PTE peripheral module. Must be one of `PTE_Module`
 * @param Task       Set PTE task configuration. Must be one of `PTE_Task`
 * @param SetTaskChannel    Set PTE task channel. Must be one of `PTE_Channel`
 * @param enable      Set PTE task enable or disable.
 * @return                 other : failed, 0 : success
 * @example Set Qdec module task config,channel 0,task is PTE_TASK_QDEC_CH0_START,
 *          PTE_ConfigTask(PTE_MODULE_QDEC, PTE_TASK_QDEC_CH0_START, PTE_CHANNEL_0, 1)
 */
uint8_t PTE_ConfigTask(PTE_Module module, PTE_Task Task, PTE_Channel SetTaskChannel, uint8_t enable);

/**
 * @brief Configure task parameters of the PTE peripheral module
 *
 * @param module      Set PTE peripheral module. Must be one of `PTE_Module`
 * @param Event       Set PTE task configuration. Must be one of `PTE_Task`
 * @param SetEventChannel    Set PTE task channel. Must be one of `PTE_Channel`
 * @param enable      Set PTE task enable or disable.
 * @return                 other : failed, 0 : success
 * @example Set Qdec module task config,channel 0,event is PTE_EVENT_QDEC_CH0_STOP_CPL,
 *          PTE_ConfigEvent(PTE_MODULE_QDEC, PTE_EVENT_QDEC_CH0_STOP_CPL, PTE_CHANNEL_0, 1)
 */
uint8_t PTE_ConfigEvent(PTE_Module module, PTE_Event Event, PTE_Channel SetEventChannel, uint8_t enable);

/**
 * @brief Configure task or event parameters of the GPIOTE peripheral module.
 *
 * @param channel Set GPIOTE channel. Must be one of `GPIOTE_Channel`
 * @param mode Set GPIOTE mode. Must be one of `GPIOTE_Mode`.
 * @param pin IO pin number.
 * @param out_level GPIOTE normal output level.  0: low level, 1: high level
 */
void GPIOTE_Configuration(GPIOTE_Channel channel, GPIOTE_Mode mode, uint8_t pin, uint8_t out_level);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif    /* __cplusplus */

#endif

#endif

