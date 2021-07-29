#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "kv_storage.h"
#include "btstack_util.h"
#include "eflash.h"
#include "trace.h"
#include <string.h>
#include <stdio.h>
#include "blink.h"

#include "../../peripheral_console/src/key_detector.h"

#define PRINT_UART    APB_UART0

uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    platform_printf("HARDFAULT:\n"
           "PC : 0x%08X\n"
           "LR : 0x%08X\n"
           "PSR: 0x%08X\n"
           "R0 : 0x%08X\n"
           "R1 : 0x%08X\n"
           "R2 : 0x%08X\n"
           "P3 : 0x%08X\n"
           "R12: 0x%08X\n",
           info->pc, info->lr, info->psr, info->r0, info->r1, info->r2,
           info->r3, info->r12);
    for (;;);
}

uint32_t cb_putc(char *c, void *dummy)
{
    while (apUART_Check_TXFIFO_FULL(PRINT_UART) == 1);
    UART_SendData(PRINT_UART, (uint8_t)*c);
    return 0;
}

int fputc(int ch, FILE *f)
{
    cb_putc((char *)&ch, NULL);
    return ch;
}

void config_uart(uint32_t freq, uint32_t baud)
{
    UART_sStateStruct UART_0;

    UART_0.word_length       = UART_WLEN_8_BITS;
    UART_0.parity            = UART_PARITY_NOT_CHECK;
    UART_0.fifo_enable       = 1;
    UART_0.two_stop_bits     = 0;   //used 2 stop bit ,    0
    UART_0.receive_en        = 1;
    UART_0.transmit_en       = 1;
    UART_0.UART_en           = 1;
    UART_0.cts_en            = 0;
    UART_0.rts_en            = 0;
    UART_0.rxfifo_waterlevel = 1;   //UART_FIFO_ONE_SECOND;
    UART_0.txfifo_waterlevel = 1;   //UART_FIFO_ONE_SECOND;
    UART_0.ClockFrequency    = freq;
    UART_0.BaudRate          = baud;

    apUART_Initialize(PRINT_UART, &UART_0, 0);
}

uint32_t gpio_isr(void *user_data)
{
    GIO_ClearAllIntStatus();
    key_detector_start_on_demand();
    return 0;
}

#define LED_PIN     GIO_GPIO_6
#define LED_PWM_CH  0

void show_app_state(enum app_state state)
{
    switch (state)
    {
        case APP_PAIRING:
            blink_style(LED_PWM_CH, BLINK_FAST);
            break;
        case APP_ADV:
            blink_style(LED_PWM_CH, BLINK_NORMAL);
            break;
        case APP_CONN:
            blink_style(LED_PWM_CH, BLINK_ON);
            break;
        default:
            blink_style(LED_PWM_CH, BLINK_OFF);
            break;
    }
}

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);

    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO)
                              | (1 << SYSCTRL_ClkGate_APB_TMR1)
                              | (1 << SYSCTRL_ClkGate_APB_PinCtrl)
                              | (1 << SYSCTRL_ClkGate_APB_PWM));

    // setup timer 1: 1Hz
    TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / 10);
    TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
    TMR_IntEnable(APB_TMR1);
    TMR_Reload(APB_TMR1);
    TMR_Enable(APB_TMR1);

    PINCTRL_SetPadMux(KEY_PIN, IO_SOURCE_GENERAL);
    GIO_SetDirection(KEY_PIN, GIO_DIR_INPUT);
    PINCTRL_Pull(KEY_PIN, PINCTRL_PULL_UP);
    GIO_ConfigIntSource(KEY_PIN, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE, GIO_INT_EDGE);
    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);

    PINCTRL_SetPadMux(LED_PIN, IO_SOURCE_GENERAL);
    PINCTRL_SetGeneralPadMode(LED_PIN, IO_MODE_PWM, LED_PWM_CH, 1);
}

uint32_t timer_isr(void *user_data)
{
    TMR_IntClr(APB_TMR1);
    app_timer_callback();
    return 0;
}

#define DB_FLASH_ADDRESS  0x42000
int db_write_to_flash(const void *db, const int size)
{
    platform_printf("write to flash, size = %d\n", size);
    program_flash(DB_FLASH_ADDRESS, (const uint8_t *)db, size);
    return KV_OK;
}

int read_from_flash(void *db, const int max_size)
{
    memcpy(db, (void *)DB_FLASH_ADDRESS, max_size);
    return KV_OK;
}

extern void on_key_event(key_press_event_t evt);

trace_rtt_t trace_ctx = {0};

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, timer_isr, NULL);
    setup_peripherals();

    // platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);
    
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);    
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    
    kv_init(db_write_to_flash, read_from_flash);

    key_detect_init(on_key_event);

    trace_rtt_init(&trace_ctx);
    platform_set_evt_callback(PLATFORM_CB_EVT_TRACE, (f_platform_evt_cb)cb_trace_rtt, &trace_ctx);
    platform_config(PLATFORM_CFG_TRACE_MASK, 0xff);

    return 0;
}

