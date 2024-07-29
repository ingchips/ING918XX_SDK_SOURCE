#include "ingsoc.h"

#include "app_cfg.h"
#include "platform_api.h"

#if (SAMPLING_RATE != 16000)
#error only 16kHz is supported
#endif

#if(INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
#include "peripheral_asdm.h"
#endif

// Note: MIC is on the RIGHT channel.

static uint32_t sample_counter = 0;

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

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
#elif(INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)

static uint32_t cb_isr(void *user_data)
{
//    uint32_t i;
//    i = ASDM_GetFifoCount(APB_ASDM);
    while (ASDM_GetFifoCount(APB_ASDM))
    {
        uint32_t sample = ASDM_GetOutData(APB_ASDM);
        sample_counter++;
//        i--;
        if (sample_counter & 1)
            audio_rx_sample((pcm_sample_t)(sample));
            audio_rx_sample((pcm_sample_t)(sample >> 16));
            
    }

    return 0;
}

void pdm_io_init(void)
{
    SYSCTRL_ClearClkGate(SYSCTRL_ClkGate_APB_GPIO0);//gpio0
    SYSCTRL_ClearClkGate(SYSCTRL_ClkGate_APB_PinCtrl);
    PINCTRL_SelAsdm(GIO_GPIO_9,GIO_GPIO_10);
}

void audio_input_setup(void)
{
    //pclk_in
    *((uint32_t*)&(APB_SYSCTRL->CguCfg[4])) |= (1<<28);//asdm_posedge_sample
    *((uint32_t*) 0x40102028) |= 1;
    
    //enable clk
    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_ASDM);
    //fclk clk
    //if use pull 30 - 12.801MHz, 34 - 11.295MHz 
    //fpga use 24m 1 12m
    //¸üÐÂÄ£¿éfclkÊ±ÖÓ10·ÖÆµ
    SYSCTRL_UpdateAsdmClk(9);
    ASDM_SetEdgeSample(APB_ASDM,1);
    ASDM_SetHighPassEn(APB_ASDM,1);
    ASDM_HighPassErrorEn(APB_ASDM,0);
    ASDM_InvertInput(APB_ASDM,1);
    ASDM_SelMic(APB_ASDM,1);
    ASDM_SelDownSample(APB_ASDM,1);
    
    ASDM_UpdateSr(APB_ASDM, 1<<7);//asdm_sr fs 12k 

    ASDM_MuteEn(APB_ASDM,0);//asdm_mute_l
    ASDM_FadeInOutEn(APB_ASDM,0);//asdm_fadedis_l
    ASDM_SetVolStep(APB_ASDM, 3);//asdm_vol_step

    ASDM_SetVol(APB_ASDM, 0x3fff);//asdm_vol_l

    ASDM_SetHighPassFilter(APB_ASDM, 0xffe);//asdm_hpf_coef

    ASDM_AGCEn(APB_ASDM,1);//asdm_agc_sel
    ASDM_SetAGCMaxLevel(APB_ASDM, 1<<4);//asdm_max_level

    ASDM_SetAGCFrameTime(APB_ASDM, 8);//asdm_frame_time

    ASDM_SetAGCNoiseThresh(APB_ASDM, 0xf);//asdm_noise_th
    ASDM_SetAGCNoiseMode(APB_ASDM, 0);//asdm_noise_mode
    ASDM_AGCNoiseGateEn(APB_ASDM, 1);//asdm_noise_gt_en
    ASDM_SetAGCGateMode(APB_ASDM, 0);//asdm_noise_gt_mode
    ASDM_NoiseHold(APB_ASDM, 0x7);//asdm_noise_hold

    ASDM_SetAGCMute(APB_ASDM, 0);//asdm_agc_mute

    
    ASDM_FifoEn(APB_ASDM, 1);//fifo enable
    ASDM_SetFifoInt(APB_ASDM, 2);
    ASDM_SetFifoTrig(APB_ASDM, 3);//fifo_trig
    
    pdm_io_init();
    platform_set_irq_callback(PLATFORM_CB_IRQ_ASDM, cb_isr, 0);
}

void audio_input_start(void)
{
    sample_counter = 1;
    PDM_Enable(APB_ASDM,1);
}

void audio_input_stop(void)
{
    PDM_Enable(APB_ASDM,0);
}
#endif
