#include <stdlib.h>
#include "i2s_emu.h"
#include "platform_api.h"

#include "FreeRTOS.h"
#include "task.h"

#ifdef I2S_DBG

#define SPI_MIC_CLK         GIO_GPIO_10
#define SPI_MIC_MOSI        GIO_GPIO_11
#define SPI_MIC_CS          GIO_GPIO_13

#endif

static uint32_t ClkFreq = 1;

#include "eflash.inc"

#define w32(a,b) *(volatile uint32_t*)(a) = (uint32_t)(b)
#define r32(a)   *(volatile uint32_t*)(a)
    
static int spi_word_cnt = 0;
static int sample_cnt = 0;
static f_pcm_sample_cb sample_cb;

static void SPI_Init(SSP_TypeDef * SSP_Ptr)
{
    apSSP_sDeviceControlBlock Param; 

    apSSP_Initialize(SSP_Ptr);

    /* Set Device Parameters */
    Param.ClockRate        = 0x0;   // sspclkout = sspclk/(ClockPrescale*(ClockRate+1))
    Param.ClockPrescale    = 0x2e;  // Must be an even number from 2 to 254
    Param.eSCLKPolarity    = apSSP_SCLKPOLARITY_IDLELOW;
    Param.eFrameFormat     = apSSP_FRAMEFORMAT_TISYNCHRONOUS;
    Param.eDataSize        = apSSP_DATASIZE_16BITS;
    Param.eLoopBackMode    = apSSP_LOOPBACKOFF;
    Param.eMasterSlaveMode = apSSP_MASTER;
    Param.eSlaveOutput     = apSSP_SLAVEOUTPUTDISABLED;
    apSSP_DeviceParametersSet(SSP_Ptr, &Param);
    SSP_Ptr->IntMask = (1 << bsSSP_MASK_RXINTENABLE);
}

static uint32_t spi_isr(void *user_data)
{
    int i;
    while (apSSP_TxFifoNotFull(MIC_PORT))
        apSSP_WriteFIFO(MIC_PORT, 0xff00);
    
    for (i = 0; i < 2; i++)
    {
        pcm_sample_t sample;
        sample = (pcm_sample_t)apSSP_ReadFIFO(MIC_PORT);
        if (spi_word_cnt == 0)
        {
            sample_cnt = sample_cnt < 53 ? sample_cnt + 1 : 0;
            if (sample_cnt != 0)
                sample_cb(sample);
        }

        spi_word_cnt = (spi_word_cnt + 1) & 0x3;        
    }
    
    while (apSSP_TxFifoNotFull(MIC_PORT))
        apSSP_WriteFIFO(MIC_PORT, 0xff00);
    
    return 0;
}

static uint32_t spi_trigger_timer_isr(void *user_data)
{
    apSSP_DeviceEnable(MIC_PORT);
    EflashCacheEna();

    TMR_IntClr(INTERNAL_TIMER);
    TMR_Disable(INTERNAL_TIMER);     

    return 0;
}

void i2s_emu_setup(f_pcm_sample_cb f, int sample_rate)
{
    sample_cb = f;
    SYSCTRL_ClearClkGateMulti(    (1 << SYSCTRL_ClkGate_AHB_SPI0)
                                | (1 << SYSCTRL_ClkGate_APB_PinCtrl)
                                | (1 << SYSCTRL_ClkGate_APB_TMR1)
                                | (1 << SYSCTRL_ClkGate_APB_GPIO)
                                | (1 << SYSCTRL_ClkGate_APB_PWM));
#ifdef I2S_DBG
    PINCTRL_SetPadMux(SPI_MIC_MOSI, IO_SOURCE_SPI0_DO);
    PINCTRL_SetPadMux(SPI_MIC_CLK, IO_SOURCE_SPI0_CLK);
    PINCTRL_SetPadMux(SPI_MIC_CS, IO_SOURCE_SPI0_SSN);
#endif

    PINCTRL_SelSpiDiIn(SPI_PORT_0, SPI_MIC_MISO);
    
    PINCTRL_SetPadMux(BCLK_PIN, IO_SOURCE_GENERAL);
    PINCTRL_SetGeneralPadMode(BCLK_PIN, IO_MODE_PWM, 0, 0);
    PINCTRL_SetPadMux(WS_PIN, IO_SOURCE_GENERAL);  
    PINCTRL_SetGeneralPadMode(WS_PIN, IO_MODE_PWM, 1, 0);

    NVIC_SetPriority((IRQn_Type)14, 2);
        
    PWM_SetPeraThreshold(0, 0x17);
    PWM_SetPeraThreshold(1, 0x5c0);
    PWM_SetHighThreshold(0, 0, 0xb);
    PWM_SetHighThreshold(1, 0, 0x2e0);
    PWM_SetMode(0, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
    PWM_SetMode(1, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
    PWM_SetMultiDutyCycleCtrl(0, 0);
    PWM_SetMultiDutyCycleCtrl(0, 0);
    
    TMR_Disable(INTERNAL_TIMER);
    TMR_SetOpMode(INTERNAL_TIMER, TMR_CTL_OP_MODE_ONESHOT);
    TMR_SetCMP(INTERNAL_TIMER, 0x538);
    TMR_IntEnable(INTERNAL_TIMER);

    platform_set_irq_callback(PLATFORM_CB_IRQ_SPI0, spi_isr, NULL);
    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, spi_trigger_timer_isr, NULL);
}

void i2s_emu_start(void)
{    
    apSSP_DeviceDisable(MIC_PORT);
    SPI_Init(MIC_PORT);
    while (apSSP_TxFifoNotFull(MIC_PORT))
        apSSP_WriteFIFO(MIC_PORT, 0xff00);
    while (apSSP_RxFifoNotEmpty(MIC_PORT))
        apSSP_ReadFIFO(MIC_PORT);
    
    TMR_Disable(INTERNAL_TIMER);
    TMR_Reload(INTERNAL_TIMER);

    EflashCacheBypass();
    PWM_EnableMulti(3, 1);
    TMR_Enable(INTERNAL_TIMER);

    spi_word_cnt = 0;
    sample_cnt = 0;
}

void i2s_emu_stop(void)
{
    apSSP_DeviceDisable(MIC_PORT);
    PWM_EnableMulti(3, 0);
}
