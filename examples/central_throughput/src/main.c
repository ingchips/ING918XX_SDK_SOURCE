#define OPTIONAL_RF_CLK
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include <stdio.h>
#include "uart_console.h"
#include "rf_util.h"

#ifdef USE_DISPLAY
#include "oled_ssd1306.c"
#include "FreeRTOS.h"
#include "task.h"
#endif

uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    platform_printf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n"
                    "R12: 0x%08X\n",
                    info->pc, info->lr, info->psr, 
                    info->r0, info->r1, info->r2, info->r3, info->r12);
    for (;;);
}

#define PRINT_PORT    APB_UART0

#define KB_KEY_1      GIO_GPIO_6
#define KB_KEY_2      GIO_GPIO_5

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

    apUART_Initialize(PRINT_PORT, &config, 1 << bsUART_RECEIVE_INTENAB);
}

void hw_timer_restart(void)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    TMR_Reload(APB_TMR1);
    TMR_Enable(APB_TMR1);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    TMR_Enable(APB_TMR1, 0, 0xf);
#else
    #error unknown or unsupported chip family
#endif
}

void hw_timer_clear_int(void)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    TMR_IntClr(APB_TMR1);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    TMR_IntClr(APB_TMR1, 0, 0xf);
#else
    #error unknown or unsupported chip family
#endif
}

void hw_timer_stop(void)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    TMR_Disable(APB_TMR1);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    TMR_Enable(APB_TMR1, 0, 0);
#else
    #error unknown or unsupported chip family
#endif
}

void setup_peripherals(void)
{    
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_TMR1)
                            | (1 << SYSCTRL_ClkGate_APB_GPIO0));

    config_uart(OSC_CLK_FREQ, 115200);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ);
	TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
	TMR_IntEnable(APB_TMR1);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    TMR_SetOpMode(APB_TMR1, 0, TMR_CTL_OP_MODE_32BIT_TIMER_x1, TMR_CLK_MODE_APB, 0);
    TMR_SetReload(APB_TMR1, 0, TMR_GetClk(APB_TMR1, 0));
    TMR_IntEnable(APB_TMR1, 0, 0xf);
#else
    #error unknown or unsupported chip family
#endif

    PINCTRL_SetPadMux(KB_KEY_1, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(KB_KEY_2, IO_SOURCE_GENERAL);
    GIO_SetDirection(KB_KEY_1, GIO_DIR_INPUT);
    GIO_SetDirection(KB_KEY_2, GIO_DIR_INPUT);
    GIO_ConfigIntSource(KB_KEY_1, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE,
                        GIO_INT_EDGE);
    GIO_ConfigIntSource(KB_KEY_2, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE,
                        GIO_INT_EDGE);
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

uint32_t cb_lle_init(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    return 0;
}

uint32_t uart_isr(void *user_data)
{
    uint32_t status;

    while(1)
    {
        status = apUART_Get_all_raw_int_stat(APB_UART0);
        if (status == 0)
            break;

        APB_UART0->IntClear = status;

        // rx int
        if (status & (1 << bsUART_RECEIVE_INTENAB))
        {
            while (apUART_Check_RXFIFO_EMPTY(APB_UART0) != 1)
            {
                char c = APB_UART0->DataRead;
                console_rx_data(&c, 1);
            }
        }
    }
    return 0;
}

uint32_t timer_isr(void *user_data);
void cmd_help(const char *param);

#ifdef USE_DISPLAY

app_status_t app_status = {0};
static SemaphoreHandle_t sem_display = NULL;
static char str[100];

void app_state_updated(void)
{
    xSemaphoreGive(sem_display);
}

void sprint_addr(char *str, const uint8_t *addr)
{
    sprintf(str, "ADD %02X%02X%02X%02X%02X%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

static void display_reset(void)
{
    OLED_Clear();     
    OLED_ShowString(0, 0, "M->S:", 1, 16);
    OLED_ShowString(0, 2, "S->M:", 1, 16);
    OLED_ShowString(0, 5, "ING THROUGHPUT", 1, 12);
}

static void sprint_time(char *str, uint32_t sec)
{
    uint32_t m = sec / 60;
    sec -= m * 60;
    uint32_t h = m / 60;
    m -= h * 60;
    sprintf(str, "%02d:%02d:%02d", h, m, sec);
}

static void display_task(void *pdata)
{
    static uint64_t start_time;
    int conn = 0;
printf("%d\n", __LINE__);
    OLED_Init();
printf("%d\n", __LINE__);
    display_reset();
printf("%d\n", __LINE__);
    for (;;)
    {
        BaseType_t r = xSemaphoreTake(sem_display,  portMAX_DELAY);
        if (app_status.connected)
        {
            if (conn == 0)
            {
                sprint_addr(str, app_status.peer);
                OLED_ShowString(0, 6, str, 1, 12);
                start_time = platform_get_us_time();
                conn = 1;
            }
            printf("%d\n", __LINE__);
            if (app_status.m2s_bps > 0)
                sprintf(str, "%7.1fkbps", app_status.m2s_bps / 1000.);
            else
                sprintf(str, "%11s", "---");
            OLED_ShowString(5 * 8, 0, str, 0, 16);
            
            if (app_status.s2m_bps > 0)
                sprintf(str, "%7.1fkbps", app_status.s2m_bps / 1000.);
            else
                sprintf(str, "%11s", "---");
            OLED_ShowString(5 * 8, 2, str, 0, 16);
            
            {
                uint32_t now = platform_get_us_time();
                if (now > start_time)
                {
                    now -= start_time;
                    sprint_time(str, (uint32_t)(now / 1000000));
                    OLED_ShowString(0, 7, str, 1, 12);
                }
            }
        }
        else
        {
            if (conn)
                display_reset();
            conn = 0;
        }
    }
}

static void display_init(void)
{
    sem_display = xSemaphoreCreateBinary();

    xTaskCreate(display_task,
           "w",
           200,
           NULL,
           0,
           NULL);
}

#endif

uint32_t gpio_isr(void *user_data)
{
    extern void start_tpt(tpt_dir_t dir);
    extern void stop_tpt(tpt_dir_t dir);

    uint32_t current = ~GIO_ReadAll();

    // report which keys are pressed
    if (current & (1 << KB_KEY_1))
    {
        static int last = 0;
        if (last)
        {
            stop_tpt(DIR_M_TO_S);
            last = 0;
        }
        else
        {
            start_tpt(DIR_M_TO_S);
            last = 1;
        }
    }
    if (current & (1 << KB_KEY_2))
    {
        static int last = 0;
        if (last)
        {
            stop_tpt(DIR_S_TO_M);
            last = 0;
        }
        else
        {
            start_tpt(DIR_S_TO_M);
            last = 1;
        }
    }

    GIO_ClearAllIntStatus();
    return 0;
}

int app_main()
{
    /// RF power boost
    rf_enable_powerboost();

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);    
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_LLE_INIT, cb_lle_init, NULL);
    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);

    setup_peripherals();

    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, timer_isr, NULL);
    platform_set_irq_callback(PLATFORM_CB_IRQ_UART0, uart_isr, NULL);
    
    cmd_help(NULL);

#ifdef USE_DISPLAY
    display_init();
#endif

    return 0;
}
