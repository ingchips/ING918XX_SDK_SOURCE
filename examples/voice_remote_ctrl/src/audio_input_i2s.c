#include "ingsoc.h"

#include "app_cfg.h"
#include "platform_api.h"

#if (SAMPLING_RATE != 16000)
#error only 16kHz is supported
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

// Note: MIC is on the RIGHT channel.
// Tested on INMP441.

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
    PINCTRL_SetPadMux(I2S_PIN_BCLK, IO_SOURCE_I2S_BCLK_OUT);
    PINCTRL_SetPadMux(I2S_PIN_IN, IO_SOURCE_I2S_DATA_IN);
    PINCTRL_SelI2sIn(IO_NOT_A_PIN, IO_NOT_A_PIN, I2S_PIN_IN);
    PINCTRL_SetPadMux(I2S_PIN_LRCLK, IO_SOURCE_I2S_LRCLK_OUT);
    PINCTRL_Pull(IO_SOURCE_I2S_DATA_IN, PINCTRL_PULL_DOWN);

    SYSCTRL_ConfigPLLClk(6, 128, 2);                    // PLL = 256MHz
    SYSCTRL_SelectHClk(SYSCTRL_CLK_PLL_DIV_1 + 1);      // MCU @ 128MHz
    SYSCTRL_SelectI2sClk(SYSCTRL_CLK_PLL_DIV_1 + 4);    // I2S Clk @ 51.2MHz
    I2S_ConfigClk(APB_I2S, 25, 32);                     // F_bclk = 1.024MHz
    I2S_ConfigIRQ(APB_I2S, 0, 1, 0, 10);
    I2S_DMAEnable(APB_I2S, 0, 0);    
    I2S_Config(APB_I2S, I2S_ROLE_MASTER, I2S_MODE_STANDARD, 0, 1, 0, 1, 24);
    
    platform_set_irq_callback(PLATFORM_CB_IRQ_I2S, cb_isr, 0);
}

void audio_input_start(void)
{
    sample_counter = 1;
    I2S_ClearRxFIFO(APB_I2S);
    I2S_Enable(APB_I2S, 0, 1);
}

void audio_input_stop(void)
{
    I2S_Enable(APB_I2S, 0, 0);
}

#else
#error only ING916xx is supported
#endif

