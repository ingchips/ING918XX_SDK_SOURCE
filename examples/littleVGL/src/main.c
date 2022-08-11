#include <stdio.h>
#include <string.h>
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include "spi.h"
#include "peripheral_gpio.h"
#include "lcd_config.h"
#include "lvgl.h"
#include "lv_api_map.h"
#include "lv_port_disp.h"
//#include "lv_example_widgets.h"

static uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    platform_printf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n"
                    "R12: 0x%08X\n",
                    info->pc, info->lr, info->psr,
                    info->r0, info->r1, info->r2, info->r3, info->r12);
    for (;;);
}

static uint32_t cb_assertion(assertion_info_t *info, void *_)
{
    platform_printf("[ASSERTION] @ %s:%d\n",
                    info->file_name,
                    info->line_no);
    for (;;);
}

static uint32_t cb_heap_out_of_mem(uint32_t tag, void *_)
{
    platform_printf("[OOM] @ %d\n", tag);
    for (;;);
}

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
    SYSCTRL_ClearClkGateMulti(0
                            | (1 << SYSCTRL_ClkGate_APB_TMR0)
                            | (1 << SYSCTRL_ClkGate_APB_TMR1)
							| (1 << SYSCTRL_ClkGate_APB_GPIO)
							| (1 << SYSCTRL_ClkGate_AHB_SPI0)
							| (1 << SYSCTRL_ClkGate_APB_PinCtrl)

	);
    config_uart(OSC_CLK_FREQ, 115200);

    if (!IS_DEBUGGER_ATTACHED())
    {
        // Watchdog will timeout after 20sec
        TMR_WatchDogEnable(TMR_CLK_FREQ * 10);
    }

	// IO setup
	PINCTRL_SetPadMux(SPI_LCD_BL, IO_SOURCE_GENERAL);
	PINCTRL_SetPadMux(SPI_LCD_DC, IO_SOURCE_GENERAL);
	PINCTRL_SetPadMux(SPI_LCD_RST, IO_SOURCE_GENERAL);
	PINCTRL_SetPadMux(SPI_LCD_CS, IO_SOURCE_GENERAL);
	PINCTRL_SetPadMux(SPI_LCD_MOSI, IO_SOURCE_SPI0_DO);
	PINCTRL_SetPadMux(SPI_LCD_SCLK, IO_SOURCE_SPI0_CLK);

	GIO_SetDirection(SPI_LCD_BL, GIO_DIR_OUTPUT);
	GIO_SetDirection(SPI_LCD_RST, GIO_DIR_OUTPUT);
	GIO_SetDirection(SPI_LCD_CS, GIO_DIR_OUTPUT);
	GIO_SetDirection(SPI_LCD_DC, GIO_DIR_OUTPUT);

	SPI_Init();

	TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / 1000);//1k hz , 1ms
	TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
	TMR_IntEnable(APB_TMR1);
	TMR_Reload(APB_TMR1);
	TMR_Enable(APB_TMR1);
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

static void watchdog_task(void *pdata)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(9000));
        if (IS_DEBUGGER_ATTACHED()) continue;
        TMR_WatchDogRestart();
    }
}

trace_rtt_t trace_ctx = {0};

uint32_t timer1_isr(void *user_data)
{
    BaseType_t xHigherPriorityTaskWoke = pdFALSE;
    TMR_IntClr(APB_TMR1);
	lv_tick_inc(1);//lvgl 1ms heart beat
    return 0;
}

static void lvgl_task(void *pdata)
{
    extern void lv_example_chart_2(void);

    lv_init();
	lv_port_disp_init();
	//show a chart here
	lv_example_chart_2();
    
    for (;;)
    {
    	lv_task_handler();
        vTaskDelay(2);
    }
}

int app_main()
{
    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ASSERTION, (f_platform_evt_cb)cb_assertion, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_HEAP_OOM, (f_platform_evt_cb)cb_heap_out_of_mem, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    setup_peripherals();

    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, timer1_isr, NULL);	

	xTaskCreate(watchdog_task,
           "w",
           configMINIMAL_STACK_SIZE,
           NULL,
           (configMAX_PRIORITIES - 1),
           NULL);

	xTaskCreate(lvgl_task,
               "b",
               1200,
               NULL,
               (configMAX_PRIORITIES - 2),
               NULL);
    trace_rtt_init(&trace_ctx);
    platform_set_evt_callback(PLATFORM_CB_EVT_TRACE, (f_platform_evt_cb)cb_trace_rtt, &trace_ctx);
    // TODO: config trace mask
    platform_config(PLATFORM_CFG_TRACE_MASK, 0);
    return 0;
}

