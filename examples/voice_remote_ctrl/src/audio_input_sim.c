#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>

#include "app_cfg.h"

#include "audio_encoder.h"

#if (SAMPLING_RATE != 16000)
#error "only 16kHz is supported"
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
    #define NUM_OF_SAMPLES  57190
#else
    #define NUM_OF_SAMPLES  30501
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    #define SAMPLES_LOCATION        0x00044000
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #define SAMPLES_LOCATION        0x02041000
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
    #define SAMPLES_LOCATION        0x0202e000
#else
    #error unknown or unsupport chip family
#endif

static const pcm_sample_t *pcm = (const pcm_sample_t *)SAMPLES_LOCATION;

static uint16_t pcm_index = 0;

uint32_t audio_sample_isr(void *user_data)
{
    pcm_sample_t sample = 0;
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    TMR_IntClr(APB_TMR1);
#elif ((INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) || (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20))
    TMR_IntClr(APB_TMR1, 0, 0x1);
#endif

    if (pcm_index < NUM_OF_SAMPLES)
    {
        sample = pcm[pcm_index++];
    }
    else
    {
        sample = pcm[0];
        pcm_index = 1;
    }

    audio_rx_sample(sample);

    return 0;
}

void audio_input_setup(void)
{
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_TMR1));

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / SAMPLING_RATE);
    TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
    TMR_Reload(APB_TMR1);
    TMR_IntEnable(APB_TMR1);
#elif ((INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) || (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20))
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    SYSCTRL_SelectTimerClk(TMR_PORT_1, SYSCTRL_CLK_SLOW_DIV_4);
#else
    SYSCTRL_SelectTimerClk(TMR_PORT_1, 4, SOURCE_SLOW_CLK);
#endif    
    TMR_SetOpMode(APB_TMR1, 0, TMR_CTL_OP_MODE_32BIT_TIMER_x1, TMR_CLK_MODE_EXTERNAL, 0);
    TMR_SetReload(APB_TMR1, 0, OSC_CLK_FREQ / 4 / SAMPLING_RATE);
    TMR_IntEnable(APB_TMR1, 0, 0x1);
#endif

    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, audio_sample_isr, NULL);
}

void audio_input_start(void)
{
    pcm_index = 0;
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    TMR_Enable(APB_TMR1);sdfsdf
#elif ((INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) || (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20))
    TMR_Enable(APB_TMR1, 0, 0x1);
#endif
}

void audio_input_stop(void)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    TMR_Disable(APB_TMR1);
#elif ((INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) || (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20))
    TMR_Enable(APB_TMR1, 0, 0x0);
#endif
}
