#include "ingsoc.h"

#include "app_cfg.h"
#include "platform_api.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

// Note: MIC is on the RIGHT channel.
static uint32_t sample_counter = 0;

static uint32_t cb_isr(void *user_data)
{
    uint32_t state = I2S_GetIntState(APB_I2S);
    I2S_ClearIntState(APB_I2S, state);

    int i = I2S_GetRxFIFOCount(APB_I2S);

    while (i)
    {
        uint32_t sample = I2S_PopRxFIFO(APB_I2S);
        sample_counter++;
        i--;
        if (sample_counter & 1)
            audio_rx_sample((pcm_sample_t)(sample >> 8));
    }

    return 0;
}

void audio_input_setup(void)
{
    // PDM @ 3MHz
    SYSCTRL_SetAdcClkDiv(8);
    SYSCTRL_SelectTypeAClk(SYSCTRL_ITEM_APB_PDM, SYSCTRL_CLK_ADC_DIV);

    PINCTRL_SetPadMux(PDM_PIN_MCLK, IO_SOURCE_PDM_DMIC_MCLK);
    PINCTRL_SetPadMux(PDM_PIN_IN, IO_SOURCE_PDM_DMIC_IN);
    PINCTRL_SelPdmIn(PDM_PIN_IN);

    // I2S data form PDM
    // I2S @ OSC_CLK_FREQ / (4 * 5 * 75) = 16000
    I2S_DataFromPDM(1);
    I2S_ConfigClk(APB_I2S, 5, 75);
    I2S_ConfigIRQ(APB_I2S, 0, 1, 0, 10);
    I2S_DMAEnable(APB_I2S, 0, 0);
    I2S_Config(APB_I2S, I2S_ROLE_MASTER, I2S_MODE_STANDARD, 0, 1, 0, 1, 24);

    platform_set_irq_callback(PLATFORM_CB_IRQ_I2S, cb_isr, 0);

    PDM_Config(APB_PDM, 0, 1, 1, 1, 0);
}

void audio_input_start(void)
{
    sample_counter = 1;
    I2S_ClearRxFIFO(APB_I2S);
    PDM_Start(APB_PDM, 1);
    I2S_Enable(APB_I2S, 0, 1);
}

void audio_input_stop(void)
{
    PDM_Start(APB_PDM, 0);
    I2S_Enable(APB_I2S, 0, 0);
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
        audio_rx_sample((pcm_sample_t)(rr[cnt] & 0xff));
        audio_rx_sample((pcm_sample_t)(rr[cnt] >> 16));
        cnt++;
    }

    return 0;
}

void pdm_io_init(void)
{
    SYSCTRL_ClearClkGate(SYSCTRL_ClkGate_APB_GPIO0);//gpio0
    SYSCTRL_ClearClkGate(SYSCTRL_ClkGate_APB_PinCtrl);
    PINCTRL_SelAsdm(GIO_GPIO_9,GIO_GPIO_10);
}

ASDM_ConfigTypeDef AsdmConfig = {
    .Agc_config = 0,
    .Asdm_Mode = ASDM_PDM,
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

    pdm_io_init();

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

#else

#error "audio input pdm not support"

#endif
