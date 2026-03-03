#include "ingsoc.h"
#include "platform_api.h"
#include "peripheral_pwm.h"
#include <stdio.h>
#include "trace.h"
#include "btstack_event.h"
#include "mesh_profile.h"
#include "app_config.h"
#include "port_gen_os_driver.h"

#ifdef ENABLE_BUTTON_TEST
#include "BUTTON_TEST.h"
#endif

//-----------------------------------------------------
// uart config (using default PIN)
#define PRINT_UART          APB_UART0
#define PRINT_UART_BAUD     115200
#ifndef PIN_LED
#define PIN_LED             GIO_GPIO_17
#endif
#ifndef LED_LEVEL_ON
#define LED_LEVEL_ON        1           // high level => LED ON
#endif
#ifndef LED_LEVEL_OFF
#define LED_LEVEL_OFF       0           // low  level => LED OFF
#endif
//-----------------------------------------------------

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
    while (apUART_Check_TXFIFO_FULL(PRINT_UART) == 1);
    UART_SendData(PRINT_UART, (uint8_t)*c);
    return 0;
}

int fputc(int ch, FILE *f)
{
    cb_putc((char *)&ch, NULL);
    return ch;
}

void __aeabi_assert(const char *a ,const char* b, int c)
{
    platform_printf("assert:%s,%s,%d\n", a, b, c);
    for (;;);
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

void led_set_state(uint8_t led_on)
{
    GIO_WriteValue(PIN_LED, led_on != 0 ? LED_LEVEL_ON : LED_LEVEL_OFF);
}

void setup_peripherals(void)
{
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO0) |
                                (1 << SYSCTRL_ClkGate_APB_GPIO1) |
                                (1 << SYSCTRL_ClkGate_APB_PinCtrl));

    config_uart(OSC_CLK_FREQ, PRINT_UART_BAUD);

    // setup GPIOs for keys
    PINCTRL_SetPadMux(PIN_LED, IO_SOURCE_GPIO);
    GIO_SetDirection(PIN_LED, GIO_DIR_OUTPUT);
    GIO_WriteValue(PIN_LED, LED_LEVEL_OFF);

#ifdef ENABLE_BUTTON_TEST
    button_test_init();
#endif
}

trace_rtt_t trace_ctx = {0};
int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    platform_config(PLATFORM_CFG_RT_CLK_ACC, 200);

    setup_peripherals();

    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ASSERTION, (f_platform_evt_cb)cb_assertion, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    trace_rtt_init(&trace_ctx);
    platform_set_evt_callback(PLATFORM_CB_EVT_TRACE, (f_platform_evt_cb)cb_trace_rtt, &trace_ctx);
    platform_config(PLATFORM_CFG_TRACE_MASK, 0xfff);

    return (int)os_impl_get_driver();
}

