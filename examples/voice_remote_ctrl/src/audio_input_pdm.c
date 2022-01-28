#include "ingsoc.h"

#include "app_cfg.h"
#include "platform_api.h"

#if (SAMPLING_RATE != 16000)
#error only 16kHz is supported
#endif

#if (INGCHIPS_FAMILY != INGCHIPS_FAMILY_916)
#error only ING916xx is supported
#endif

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
    SYSCTRL_SetAdcClkDiv(8, 1);
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
