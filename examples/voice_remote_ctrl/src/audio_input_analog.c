#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>

#include "app_cfg.h"

#include "audio_encoder.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

uint32_t audio_sample_isr(void *user_data)
{
    pcm_sample_t sample;

    TMR_IntClr(APB_TMR1);

    sample = ADC_ReadChannelData(ADC_CHANNEL_ID) - 512;

    audio_rx_sample(sample);

    return 0;
}

void audio_input_setup(void)
{
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_TMR1));

    // timer 0 can be used as watchdog, so we use timer 1.
    // setup timer 1 to sampling rate
    TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / SAMPLING_RATE);
    TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
    TMR_Reload(APB_TMR1);
    TMR_IntEnable(APB_TMR1);

    ADC_PowerCtrl(1);
    ADC_Reset();
    ADC_SetClkSel(ADC_CLK_EN | ADC_CLK_16);
    ADC_SetMode(ADC_MODE_LOOP);
    ADC_EnableChannel(ADC_CHANNEL_ID, 1);
    ADC_Enable(1);

    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, audio_sample_isr, NULL);
}

void audio_input_start(void)
{
    TMR_Enable(APB_TMR1);
}

void audio_input_stop(void)
{
    TMR_Disable(APB_TMR1);
}
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#include "pingpong.h"

#define ADC_CHANNEL             ADC_CH_0
#define ADC_CLK_MHZ             6
#define ADC_CHANNEL_NUM         1
#define LOOP_DELAY(c, s, ch)    ((((c) * (1000000)) / (s)) - (((16) * (ch)) + (5)))
#define DMA_CHANNEL             0
static DMA_PingPong_t PingPong;

static uint32_t DMA_cb_isr(void *user_data)
{
    uint32_t state = DMA_GetChannelIntState(DMA_CHANNEL);
    DMA_ClearChannelIntState(DMA_CHANNEL, state);

    uint32_t *buff = DMA_PingPongIntProc(&PingPong, DMA_CHANNEL);
    uint32_t tranSize = DMA_PingPongGetTransSize(&PingPong);
    for (uint32_t i = 0; i < tranSize; ++i) {
        if (ADC_GetDataChannel(buff[i]) != ADC_CHANNEL) continue;
        uint16_t sample = ADC_GetData(buff[i]);
        audio_rx_sample((pcm_sample_t)(sample - 0x2000));
    }
    return 0;
}

SADC_ftCali_t AdcCaliData;
void audio_input_setup(void)
{
    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_ADC);
    SYSCTRL_SetAdcClkDiv(24 / ADC_CLK_MHZ);
    SYSCTRL_ReleaseBlock(SYSCTRL_ITEM_APB_ADC);
    ADC_Reset();
    ADC_Calibration(DIFFERENTAIL_MODE);
    ADC_ftInitCali(&AdcCaliData);
    ADC_ConvCfg(CONTINUES_MODE, PGA_PARA_4, 1, ADC_CHANNEL, 0, 8, DIFFERENTAIL_MODE, 
                LOOP_DELAY(ADC_CLK_MHZ, SAMPLING_RATE, ADC_CHANNEL_NUM));

    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ITEM_APB_DMA));
    SYSCTRL_SelectUsedDmaItems(1 << 9);
    DMA_PingPongSetup(&PingPong, SYSCTRL_DMA_ADC, 80, 8);
    platform_set_irq_callback(PLATFORM_CB_IRQ_DMA, DMA_cb_isr, 0);
}

void audio_input_start(void)
{
    DMA_PingPongEnable(&PingPong, DMA_CHANNEL);
    ADC_Start(1);
}

void audio_input_stop(void)
{
    DMA_PingPongDisable(&PingPong, DMA_CHANNEL);
    ADC_Start(0);
}


#elif(INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

#if (SAMPLING_RATE != 16000)
#error only 16kHz is supported
#endif

#if(INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
#include "peripheral_asdm.h"
#endif

static uint32_t cb_isr(void *user_data)
{
    uint32_t DataGet;

    while (ASDM_GetFifoCount(APB_ASDM))
    {
        DataGet = ASDM_GetOutData(APB_ASDM);

        audio_rx_sample((pcm_sample_t)(DataGet));
        audio_rx_sample((pcm_sample_t)(DataGet >> 16));
    }

    return 0;
}

void AsdmInitGpio(void)
{
    PINCTRL_EnableAnalog(GIO_GPIO_11); // vref pad
    PINCTRL_EnableAnalog(GIO_GPIO_12); // mic bias pad
    PINCTRL_EnableAnalog(GIO_GPIO_13); // mic in p
    PINCTRL_EnableAnalog(GIO_GPIO_14); // mic in n
}

ASDM_ConfigTypeDef AsdmConfig = {
    .Agc_config = 0,
    .Asdm_Mode = ASDM_AMIC,
    .Agc_mode = ASDM_AgcVoice,
    .Sample_rate = ASDM_SR_16k,
    .Fifo_Enable = 1,
    .volume = 0x3fff,
    .FifoIntMask = ASDM_RX_FLG_EN,
};

void audio_input_setup(void)
{
    int ret;
    
    SYSCTRL_ConfigPLLClk(6, 128, 2);// PLL = 307.2MHz

    AsdmInitGpio();

    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_ASDM);
    SYSCTRL_SetAdcVrefSel(0x1);
    SYSCTRL_EnableAsdmVrefOutput(0);
    SYSCTRL_EnableInternalVref(1);
    ret = ASDM_Config(APB_ASDM, &AsdmConfig);
    if (ret)
        return;
    platform_set_irq_callback(PLATFORM_CB_IRQ_ASDM, cb_isr, 0);
}

void audio_input_start(void)
{
    sample_counter = 1;
    ASDM_Enable(APB_ASDM,1);
}

void audio_input_stop(void)
{
    ASDM_Enable(APB_ASDM,0);
}
#endif
