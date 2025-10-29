#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>

#include "audio_service.h"
#include "kb_scan.h"

#include "app_cfg.h"
#include "log.h"

#define PRINT_PORT    APB_UART0

uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    platform_printf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n"
                    "R12: 0x%08X\n",
                    info->pc, info->lr, info->psr,
                    info->r0, info->r1, info->r2, info->r3, info->r12);
    for (;;);
}

uint32_t cb_assertion(assertion_info_t *info, void *_)
{
    platform_printf("[ASSERTION] @ %s:%d\n",
                    info->file_name,
                    info->line_no);
    for (;;);
}

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

    PINCTRL_DisableAllInputs();

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO)
                              | (1 << SYSCTRL_ClkGate_APB_TMR2)
                              | (1 << SYSCTRL_ClkGate_APB_PinCtrl));
#if (BOARD == BOARD_REM)
    kb_init();
    // setup timer 2: 20Hz
    TMR_SetCMP(APB_TMR2, TMR_CLK_FREQ / 20);
    TMR_SetOpMode(APB_TMR2, TMR_CTL_OP_MODE_WRAPPING);
    TMR_Reload(APB_TMR2);
    TMR_IntEnable(APB_TMR2);

    // gpio 1
    PINCTRL_SetPadMux(GIO_GPIO_1, IO_SOURCE_GENERAL);
    GIO_SetDirection(GIO_GPIO_1, GIO_DIR_OUTPUT);
    PINCTRL_SetPadPwmSel(GIO_GPIO_1, 1);
#endif
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ITEM_APB_GPIO0)
                              | (1 << SYSCTRL_ITEM_APB_GPIO1)
                              | (1 << SYSCTRL_ITEM_APB_TMR2)
                              | (1 << SYSCTRL_ITEM_APB_PinCtrl));
#endif
}

uint32_t on_deep_sleep_wakeup(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    setup_peripherals();
    return 0;
}

uint32_t query_deep_sleep_allowed(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    // TODO: return 0 if deep sleep is not allowed now; else deep sleep is allowed
    return 0;
}

#ifdef MIC_DBG
uint16_t buffer[12600];
int index = 0;

uint32_t dbg_audio_sample_isr(void *user_data)
{
    TMR_IntClr(APB_TMR1);

    buffer[index++] = ADC_ReadChannelData(0) - 512;

    if (index >= sizeof(buffer) / sizeof(buffer[0]))
        for (;;);

    return 0;
}
#endif

#if (BOARD == BOARD_REM)
uint32_t kb_scan_isr(void *user_data)
{
    TMR_IntClr(APB_TMR2);
    kb_update();
    return 0;
}

void kb_state_changed(const uint32_t old_state, const uint32_t new_state)
{
    int i;
    uint32_t changed = old_state ^ new_state;
    for (i = 0; i < 16; i++)
    {
        if (changed & (1 << i))
        {
            platform_printf("key %d: %s\n", i, new_state & (1 << i) ? "down" : "up");
        }
    }
}

void kb_keep_pressed(const uint32_t keys)
{
    int i;
    for (i = 0; i < 16; i++)
    {
        if (keys & (1 << i))
            platform_printf("key keep pressed: %d\n", i);
    }
}

#else

void kb_state_changed(const uint32_t old_state, const uint32_t new_state) {}

void kb_keep_pressed(const uint32_t keys) {}

#endif

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    SYSCTRL_Init();

    setup_peripherals();

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
    // PLL might be adjusted to a lower frequency. So a larger delay compensation
    // is reserved.
    platform_config(PLATFORM_CFG_LL_DELAY_COMPENSATION, 1200);
#endif

    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ASSERTION, (f_platform_evt_cb)cb_assertion, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup deep sleep handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);
    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

#if (BOARD == BOARD_REM)
    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER2, kb_scan_isr, NULL);
    TMR_Enable(APB_TMR2);
#endif

    return 0;
}
