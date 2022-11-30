#define  OPTIONAL_RF_CLK
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include <stdio.h>
#include "adc_cali.h"

#define PRINT_PORT    APB_UART0

uint32_t cb_putc(char *c, void *dummy)
{
    while (apUART_Check_TXFIFO_FULL(PRINT_PORT) == 1);
    UART_SendData(PRINT_PORT, (uint8_t)*c);
    return 0;
}

int fputc(int ch, FILE *f)
{
    cb_putc((char *)&ch, NULL);
    return ch;
}

void config_uart(uint32_t freq, uint32_t baud)
{
    UART_sStateStruct config;

    config.word_length       = UART_WLEN_8_BITS;
    config.parity            = UART_PARITY_NOT_CHECK;
    config.fifo_enable       = 1;
    config.two_stop_bits     = 0;
    config.receive_en        = 1;
    config.transmit_en       = 1;
    config.UART_en           = 1;
    config.cts_en            = 0;
    config.rts_en            = 0;
    config.rxfifo_waterlevel = 1;
    config.txfifo_waterlevel = 1;
    config.ClockFrequency    = freq;
    config.BaudRate          = baud;

    apUART_Initialize(PRINT_PORT, &config, 0);
}

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_TMR1));

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    // setup timer 1: 0.5s (2Hz)
    TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / 2);
    TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
    TMR_IntEnable(APB_TMR1);
    TMR_Reload(APB_TMR1);
    TMR_Enable(APB_TMR1);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    // setup channel 0 timer 1: 0.5s (2Hz)
    SYSCTRL_SelectTimerClk(TMR_PORT_1, SYSCTRL_CLK_32k);
    TMR_SetOpMode(APB_TMR1, 0, TMR_CTL_OP_MODE_32BIT_TIMER_x1, TMR_CLK_MODE_EXTERNAL, 0);
    TMR_SetReload(APB_TMR1, 0, TMR_GetClk(APB_TMR1, 0) / 2);
    TMR_Enable(APB_TMR1, 0, 0xf);
    TMR_IntEnable(APB_TMR1, 0, 0xf);
#ifndef SIMULATION        
    // setup ADC
    ADC_ClkCfg(SADC_CLK_6M);
    ADC_Calibration(SINGLE_END_MODE);
#endif
#else
    #error unknown or unsupported chip family
#endif

}

uint32_t timer_isr(void *user_data);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
void ADC_ClearChannelDataValid(const uint8_t channel_id) { }
#endif

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    setup_peripherals();

    //platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);

    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, timer_isr, NULL);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    adc_prepare_calibration();
#endif

    return 0;
}



