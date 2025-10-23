#include "stdlib.h"
#include "ingsoc.h"

#include "app_cfg.h"
#include "platform_api.h"
#include "pingpong.h"

#if (SAMPLING_RATE != 16000)
#error only 16kHz is supported
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916 || INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

// Note: MIC is on the RIGHT channel.
// Tested on INMP441.
static uint32_t sample_counter = 1;

#define TRIGGER_NUMBER  8
#define CHANNEL_ID  0
#define PP_LEN  80

DMA_PingPong_t PingPong;

static uint32_t DMA_cb_isr(void *user_data)
{
    uint32_t state = DMA_GetChannelIntState(CHANNEL_ID);
    DMA_ClearChannelIntState(CHANNEL_ID, state);

    uint32_t *rr = DMA_PingPongIntProc(&PingPong, CHANNEL_ID);
    uint32_t i = sample_counter;
    uint32_t transSize = DMA_PingPongGetTransSize(&PingPong);
    while (i < transSize) {
        audio_rx_sample((pcm_sample_t)(rr[i] >> 8));
        i += 2;
    }

    return 0;
}

void audio_input_setup(void)
{
    PINCTRL_SetPadMux(I2S_PIN_BCLK, IO_SOURCE_I2S_BCLK_OUT);
    PINCTRL_SetPadMux(I2S_PIN_IN, IO_SOURCE_I2S_DATA_IN);
    PINCTRL_SelI2sIn(IO_NOT_A_PIN, IO_NOT_A_PIN, I2S_PIN_IN);
    PINCTRL_SetPadMux(I2S_PIN_LRCLK, IO_SOURCE_I2S_LRCLK_OUT);
    PINCTRL_Pull(IO_SOURCE_I2S_DATA_IN, PINCTRL_PULL_DOWN);

    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_DMA) |
                              (1 << SYSCTRL_ClkGate_APB_I2S));
    SYSCTRL_ConfigPLLClk(6, 128, 2); // PLL = 256MHz
    SYSCTRL_SelectHClk(SYSCTRL_CLK_PLL_DIV_2);      // MCU @ 128MHz
    SYSCTRL_SelectI2sClk(SYSCTRL_CLK_PLL_DIV_5);    // I2S Clk @ 51.2MHz
    I2S_ConfigClk(APB_I2S, 25, 32); // F_bclk = 1.024MHz
    I2S_ConfigIRQ(APB_I2S, 0, 1, 0, TRIGGER_NUMBER);
    I2S_DMAEnable(APB_I2S, 0, 0);
    I2S_Config(APB_I2S, I2S_ROLE_MASTER, I2S_MODE_STANDARD, 0, 0, 0, 1, 24);

    DMA_PingPongSetup(&PingPong, SYSCTRL_DMA_I2S_RX, PP_LEN, TRIGGER_NUMBER);
    platform_set_irq_callback(PLATFORM_CB_IRQ_DMA, DMA_cb_isr, 0);
}

void audio_input_start(void)
{
    DMA_PingPongEnable(&PingPong, CHANNEL_ID);
    I2S_ClearRxFIFO(APB_I2S);
    I2S_DMAEnable(APB_I2S, 1, 1);
    I2S_Enable(APB_I2S, 0, 1);
}

void audio_input_stop(void)
{
    DMA_PingPongDisable(&PingPong, CHANNEL_ID);
    I2S_Enable(APB_I2S, 0, 0);
    I2S_DMAEnable(APB_I2S, 0, 0);
}

#else
#error only ING916xx and ING20xx is supported
#endif
