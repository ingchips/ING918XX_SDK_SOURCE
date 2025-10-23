#include "ingsoc.h"

#include "app_cfg.h"
#include "platform_api.h"

#if (SAMPLING_RATE != 16000)
#error only 16kHz is supported
#endif

#if(INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
#include "peripheral_asdm.h"
#endif

// Note: MIC is on the RIGHT channel.

static uint32_t sample_counter = 0;

#if(INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

static uint32_t cb_isr(void *user_data)
{
    uint32_t DataGet;
//    platform_printf("irq\n");

    while (ASDM_GetFifoCount(APB_ASDM))
    {
        DataGet = ASDM_GetOutData(APB_ASDM);
        sample_counter++;
        if (sample_counter & 1)
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
