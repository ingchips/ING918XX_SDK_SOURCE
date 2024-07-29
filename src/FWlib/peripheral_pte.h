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

typedef enum
{
    PTE_CHANNAL_0 = 0,
    PTE_CHANNAL_1,
    PTE_CHANNAL_2,
    PTE_CHANNAL_3,
    PTE_CHANNAL_4,
    PTE_CHANNAL_5,
    PTE_CHANNAL_6,
    PTE_CHANNAL_7,
    PTE_CHANNAL_8,
    PTE_CHANNAL_9,
    PTE_CHANNAL_10,
    PTE_CHANNAL_11,
    PTE_CHANNAL_12,
    PTE_CHANNAL_13,
    PTE_CHANNAL_14,
    PTE_CHANNAL_15,
}PTE_Channal;

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

typedef enum
{
    PTEC_CHANNEL_GROUP_0 = 0,
    PTEC_CHANNEL_GROUP_1,
    PTEC_CHANNEL_GROUP_2,
    PTEC_CHANNEL_GROUP_3,
}PTEC_ChannelGroup;

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif    /* __cplusplus */

#endif

#endif

