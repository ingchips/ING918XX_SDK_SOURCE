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

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)


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
}PTE_Channal;

/**
 * @brief PTE Module Evevt and task
 */
typedef enum
{
    PTE_EVENT_QDEC_TMR_STOP = 0,
    PTE_TASK_QDEC_TMR_STOP,
    PTE_TASK_QDEC_TMR_START,

    PTE_EVENT_DMA_TRANS_CMPL = 0,

    PTE_EVENT_GPIO_INT = 0,
    PTR_TASK_GPIO_OUT,
    PTE_TASK_GPIO_SET,
    PTE_TASK_GPIO_CLR,

    PTE_EVENT_TIMER_CHX_TMRX = 0,
    PTE_TASK_TIMER_CHX_TMRX_ENABLE,
    PTE_TASK_TIMER_CHX_TMRX_DISABLE,

    PTE_EVENT_SPI_TRANS_END = 0,
    PTE_TASK_SPI_INITIATE,
    PTE_TASK_SPI_TERMINATE,

    PTE_EVENT_IIC_TRANS_CMPL = 0,
    PTE_TASK_IIC_ISSUE_TRANS,

    PTE_EVENT_ADC_SMP_DONE = 0,
    PTE_EVENT_ADC_SMP_STOP,
    PTE_TASK_ADC_SINGLE_START,
    PTE_TASK_ADC_LOOP_SMP_START,
    PTE_TASK_ADC_LOOP_SMP_STOP,

    PTE_EVENT_UART_TX_FIFO_TRIGE = 0,
    PTE_EVENT_UART_RX_FIFO_TRIGE,
    PTE_TASK_UART_TX_ENABLE,
    PTE_TASK_UART_TX_DISABLE,
    PTE_TASK_UART_RX_ENABLE,
    PTE_TASK_UART_RX_DISABLE,

    PTE_TASK_I2S_TX_FIFO_TRIGE_ENABLE = 0,
    PTE_TASK_I2S_RX_FIFO_TRIGE_ENABLE,
    PTE_TASK_I2S_TX_WR_RST_MEM_DISABLE,
    PTE_TASK_I2S_RX_WR_RST_MEM_DISABLE,

    PTE_EVENT_PWM_CX_PWM_STOP = 0,
    PTE_EVENT_PWM_CX_FIFO_TRIG,
    PTE_EVENT_PWM_CX_PCM_STOP,
    PTE_TASK_PWM_CX_ONE_PWM,
    PTE_TASK_PWM_CX_PWM_START,
    PTE_TASK_PWM_CX_PWM_DMA_START,
    PTE_TASK_PWM_CX_PWM_STOP,
    PTE_TASK_PWM_CX_IR_START,
    PTE_TASK_PWM_CX_PCM_START,
    PTE_TASK_PWM_CX_PCM_STOP,

    PTE_EVENT_ASDM_RX_STOP = 0,
    PTE_TASK_ASDM_CONVERSION_START,
    PTE_TASK_ASDM_CONVERSION_STOP,
}PTE_ModuleTaskEvt;

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
 * @param items             combination of bits whose positions are listed in `PTE_Channal`
 * @return                  None
 * @example Enable channel0 and channel1 PTE_ChxEnable((1<<PTE_CHANNEL_0) | (1<<PTE_CHANNEL_1))
 */
void PTE_ChxEnable(uint32_t items);
/**
 * @brief PTE channel disable
 *
 * @param items             combination of bits whose positions are listed in `PTE_Channal`
 * @return                  None
 * @example Disable channel0 and channel1 PTE_ChxEnable((1<<PTE_CHANNEL_0) | (1<<PTE_CHANNEL_1))
 */
void PTE_ChxDisable(uint32_t items);
/**
 * @brief Get PTE channel enable state
 *
 * @param None
 * @return  combination of bits whose positions are listed in `PTE_Channal`
 */
uint32_t PTE_ChxGetEnableState(void);
/**
 * @brief Configure task or event parameters of the PTE peripheral module
 *
 * @param SetPTEModule      Set PTE peripheral module. Must be one of `PTE_Module`
 * @param SetTaskChannal    Set PTE task channel. Must be one of `PTE_Channal`
 * @param PTETaskEvt       Set PTE task event. Must be one of `PTE_ModuleTaskEvt`
 * @param TaskEvtSer       Set PTE task event serial number.(The module serial number can be viewed in the PTE_ModuleTaskEvt comment)
 * @return                 0 : failed, 1 : success
 * @example Set Qdec module task config,channal 0,task is QDEC_TMR_STOP,serial number is 1
 *          PTE_SetModuleTaskEventConfig(PTE_MODULE_QDEC, PTE_CHANNEL_0, PTE_TASK_QDEC_TMR_STOP, 1)
 */
uint8_t PTE_SetModuleTaskEventConfig(PTE_Module SetPTEModule, PTE_Channal SetTaskChannal, PTE_ModuleTaskEvt PTETaskEvt, uint32_t TaskEvtSer);
/**
 * @brief Ctrl task or event of the PTE peripheral module
 *
 * @param SetPTEModule      Set PTE peripheral module. Must be one of `PTE_Module`
 * @param PTETaskEvt       Set PTE task event. Must be one of `PTE_ModuleTaskEvt`
 * @param enable           Set PTE task event enable or disable.(1:enable,0:disable)
 * @param TaskEvtSer       Set PTE task event serial number.(The module serial number can be viewed in the PTE_ModuleTaskEvt comment)
 * @return                 0 : failed, 1 : success
 * @example Ctrl Qdec module task enable,task is QDEC_TMR_STOP,serial number is 1
 *          PTE_TaskEnable(PTE_MODULE_QDEC, PTE_TASK_QDEC_TMR_STOP, 1,0)
 */
uint8_t PTE_TaskEnable(PTE_Module SetPTEModule, PTE_ModuleTaskEvt PTETaskEvt, uint8_t enable, uint32_t TaskEvtSer);
/**
 * @brief Trigger task of the peripheral module PTE
 *
 * @param SetPTEModule      Set PTE peripheral module. Must be one of `PTE_Module`
 * @param PTETaskEvt       Set PTE task event. Must be one of `PTE_ModuleTaskEvt`
 * @param TaskEvtSer       Set PTE task event serial number.(The module serial number can be viewed in the PTE_ModuleTaskEvt comment)
 * @return                 0 : failed, 1 : success
 * @example Trigger Qdec module task,task is QDEC_TMR_STOP,serial number is 1
 *          PTE_TriggerTask(PTE_MODULE_QDEC, PTE_TASK_QDEC_TMR_STOP, 1)
 */
uint8_t PTE_TriggerTask(PTE_Module SetPTEModule, PTE_ModuleTaskEvt PTETaskEvt, uint32_t TaskEvtSer);
/**
 * @brief Enable PTE channel group
 *
 * @param PTEC_ChannelGroup      PTE group. Must be one of `PTEC_ChannelGroup`
 * @return                 None
 * @example Enbale PTE channel group 0
 *          PTE_SetTaskChxGroupEN(PTEC_CHANNEL_GROUP_0)
 */
void PTE_SetTaskChxGroupEN(PTEC_ChannelGroup SetChannelGroup);
/**
 * @brief Disable PTE channel group
 *
 * @param PTEC_ChannelGroup      PTE group. Must be one of `PTEC_ChannelGroup`
 * @return                 None
 * @example Disable PTE channel group 0
 *          PTE_SetTaskChxGroupDis(PTEC_CHANNEL_GROUP_0)
 */
void PTE_SetTaskChxGroupDis(PTEC_ChannelGroup SetChannelGroup);
/**
 * @brief Configure PTE channel group enable subscription
 *
 * @param PTEC_ChannelGroup      PTE group. Must be one of `PTEC_ChannelGroup`
 * @param SetSubChannel          combination of bits whose positions are listed in `PTEC_ChannelGroup`
 * @return                 None
 * @example Cfg PTE channel group 0,subscription of TASKS_CHGX_EN Group1
 *          PTE_SetTaskChxGroupEnConfig(PTEC_CHANNEL_GROUP_0,(1<<PTEC_CHANNEL_GROUP_1))
 */
void PTE_SetTaskChxGroupEnConfig(PTEC_ChannelGroup SetChannelGroup,uint32_t SetSubChannel);
/**
 * @brief Configure PTE channel group disable subscription
 *
 * @param PTEC_ChannelGroup      PTE group. Must be one of `PTEC_ChannelGroup`
 * @param SetSubChannel          combination of bits whose positions are listed in `PTEC_ChannelGroup`
 * @return                 None
 * @example Cfg PTE channel group 0,subscription of TASKS_CHGX_DIS Group1
 *          PTE_SetTaskChxGroupDisConfig(PTEC_CHANNEL_GROUP_0,(1<<PTEC_CHANNEL_GROUP_1))
 */
void PTE_SetTaskChxGroupDisConfig(PTEC_ChannelGroup SetChannelGroup,uint32_t SetSubChannel);
/**
 * @brief Configure PTE channel group map
 *
 * @param PTEC_ChannelGroup      PTE group. Must be one of `PTEC_ChannelGroup`
 * @param SetSubChannel          combination of bits whose positions are listed in `PTE_Channal`
 * @return                 None
 * 
 * @note A group only can map up to 4 channels
 * 
 * @example Map PTE channel group 0, PTE_CHANNEL_0 to PTE_CHANNEL_3 is a group
 *          PTE_SetTaskChxGroupMap(PTEC_CHANNEL_GROUP_0,(1<<PTE_CHANNEL_0)|(1<<PTE_CHANNEL_1)|(1<<PTE_CHANNEL_2)|(1<<PTE_CHANNEL_3)
 */
void PTE_SetTaskChxGroupMap(PTEC_ChannelGroup SetChannelGroup,uint32_t SetGroupMap);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif    /* __cplusplus */

#endif

#endif

