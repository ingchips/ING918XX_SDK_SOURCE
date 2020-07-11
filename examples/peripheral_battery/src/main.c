#define  OPTIONAL_RF_CLK
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include <stdio.h>

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
    
    // setup timer 1: 1sec timer
	TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / 2);
	TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
	TMR_IntEnable(APB_TMR1);
    TMR_Reload(APB_TMR1);
	TMR_Enable(APB_TMR1);
}

uint32_t cfg_rf(void *_, void * __)
{
#define reg(x)      ((volatile uint32_t *)(x))
#define ADDR 0x96
    uint16_t v;
    *reg(0x40090408) = (1 << 8) | ADDR;
    while (*reg(0x40090408) & (1 << 8)) ;
    v = *reg(0x4009040c) & 0xfffe;
    v = 0x200;
    *reg(0x4009040c) = v << 16;
    *reg(0x40090408) = (1 << 9) | ADDR;
    while (*reg(0x40090408) & (1 << 9)) ;
    return 0;
}

uint32_t timer_isr(void *user_data);

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    setup_peripherals();
    
    sysSetPublicDeviceAddr((uint8_t *)(0x44000));
    
    //platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);

    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    
    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, timer_isr, NULL);
    
    platform_set_evt_callback(PLATFORM_CB_EVT_LLE_INIT, cfg_rf, NULL);
    return 0;
}



