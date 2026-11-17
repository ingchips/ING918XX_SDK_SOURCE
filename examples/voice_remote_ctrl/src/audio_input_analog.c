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
#define PP_LEN                  80
static DMA_PingPong_t PingPong;
#if !DMA_PINGPONG_USE_MALLOC
static uint32_t dma_buffers[2][PP_LEN];
#endif

static uint32_t DMA_cb_isr(void *user_data)
{
    void *completed_buffer;
    if (DMA_PingPongIntProc(&PingPong, &completed_buffer) != 1)
        return 0;
    const uint32_t *buff = (const uint32_t *)completed_buffer;
    uint32_t tranSize = DMA_PingPongGetBufferBytes(&PingPong) / sizeof(uint32_t);
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
    if (DMA_PingPongSetup(&PingPong, SYSCTRL_DMA_ADC, PP_LEN * sizeof(uint32_t),
                         DMA_BURST_SIZE_8) != 0)
        return;
#if !DMA_PINGPONG_USE_MALLOC
    if (DMA_PingPongSetBuffers(&PingPong, dma_buffers[0], dma_buffers[1],
                              sizeof(dma_buffers[0])) != 0)
        return;
#endif
    platform_set_irq_callback(PLATFORM_CB_IRQ_DMA, DMA_cb_isr, 0);
}

void audio_input_start(void)
{
    if (DMA_PingPongEnable(&PingPong, DMA_CHANNEL) != 0)
        return;
    ADC_Start(1);
}

void audio_input_stop(void)
{
    ADC_Start(0);
    (void)DMA_PingPongDisable(&PingPong);
}

#elif(INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

#if (SAMPLING_RATE != 16000)
#error only 16kHz is supported
#endif

#include "peripheral_asdm.h"
#include "pingpong.h"

#define TRIGGER_NUMBER  1
#define CHANNEL_ID  0
#define PP_LEN  80
static DMA_PingPong_t PingPong;
#if !DMA_PINGPONG_USE_MALLOC
static uint32_t dma_buffers[2][PP_LEN];
#endif

static uint32_t DMA_cb_isr(void *user_data)
{
    uint32_t cnt = 0;
    void *completed_buffer;
    if (DMA_PingPongIntProc(&PingPong, &completed_buffer) != 1)
        return 0;
    const uint32_t *rr = (const uint32_t *)completed_buffer;
    uint32_t transSize = DMA_PingPongGetBufferBytes(&PingPong) / sizeof(uint32_t);
    while (transSize--) {
        audio_rx_sample((pcm_sample_t)(rr[cnt] & 0xffff));
        audio_rx_sample((pcm_sample_t)(rr[cnt] >> 16));
        cnt++;
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
    .FifoIntMask = 0,
    .Fifo_DmaTrigNum = TRIGGER_NUMBER,
};

void audio_input_setup(void)
{
    int ret;

    SYSCTRL_ConfigPLLClk(6, 128, 2);// PLL = 307.2MHz

    AsdmInitGpio();

    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_ASDM);
    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_DMA);
    SYSCTRL_SelectUsedDmaItems((1<<SYSCTRL_DMA_ASDM_RX)|(1<<SYSCTRL_DMA_UART1_TX)|(1<<SYSCTRL_DMA_UART0_TX)|(1<<SYSCTRL_DMA_SDADC_RX));
    SYSCTRL_SetAdcVrefSel(0x1);
    SYSCTRL_EnableAsdmVrefOutput(0);
    SYSCTRL_EnableInternalVref(1);
    ret = ASDM_Config(APB_ASDM, &AsdmConfig);
    if (ret)
        return;
    if (DMA_PingPongSetup(&PingPong, SYSCTRL_DMA_SDADC_RX, PP_LEN * sizeof(uint32_t),
                         DMA_BURST_SIZE_1) != 0)
        return;
#if !DMA_PINGPONG_USE_MALLOC
    if (DMA_PingPongSetBuffers(&PingPong, dma_buffers[0], dma_buffers[1],
                              sizeof(dma_buffers[0])) != 0)
        return;
#endif
    platform_set_irq_callback(PLATFORM_CB_IRQ_DMA, DMA_cb_isr, 0);
}

void audio_input_start(void)
{
    if (DMA_PingPongEnable(&PingPong, CHANNEL_ID) != 0)
        return;
    ASDM_Enable(APB_ASDM,1);
}

void audio_input_stop(void)
{
    ASDM_Enable(APB_ASDM, 0);
    (void)DMA_PingPongDisable(&PingPong);
}
#endif
