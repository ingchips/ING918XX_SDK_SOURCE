#define PRINT_PORT    APB_UART0

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

#ifdef DETECT_KEY

extern void on_key_event(key_press_event_t evt);

uint32_t gpio_isr(void *user_data)
{
    GIO_ClearAllIntStatus();
    key_detector_start_on_demand();
    return 0;
}
#endif

#define PIN_BUZZER 8
#define PIN_WAKEUP 0        // For ING916xx: GPIO 0 belongs to Group A

void setup_peripherals(void)
{
   // SYSCTRL_SetClkGateMulti((1 << SYSCTRL_ClkGate_APB_UART0));
    SYSCTRL_ClearClkGateMulti(0
#ifdef USE_WATCHDOG
                              | (1 << SYSCTRL_ClkGate_APB_WDT)
#endif
#if (defined DETECT_KEY) || (LISTEN_TO_POWER_SAVING)
                              | (1 << SYSCTRL_ClkGate_APB_GPIO0)
                              | (1 << SYSCTRL_ClkGate_APB_PinCtrl)
#endif
    );

#ifdef DETECT_KEY
    PINCTRL_SetPadMux(KEY_PIN, IO_SOURCE_GPIO);
    GIO_SetDirection(KEY_PIN, GIO_DIR_INPUT);
    PINCTRL_Pull(KEY_PIN, PINCTRL_PULL_DOWN);
    GIO_ConfigIntSource(KEY_PIN, GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE, GIO_INT_EDGE);
    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    config_uart(OSC_CLK_FREQ, 115200);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    config_uart(SYSCTRL_GetClk(SYSCTRL_ITEM_APB_UART0), 115200);

#ifdef DETECT_KEY
    // GPIO 0 (also connected to KEY) emulating EXT_INT as in ING918XX
    GIO_EnableRetentionGroupA(0);
    PINCTRL_SetPadMux(PIN_WAKEUP, IO_SOURCE_GPIO);
    GIO_SetDirection((GIO_Index_t)PIN_WAKEUP, GIO_DIR_INPUT);
    PINCTRL_Pull(PIN_WAKEUP, PINCTRL_PULL_DOWN);
#endif
#else
    #error unknown or unsupported chip family
#endif

#ifdef LISTEN_TO_POWER_SAVING
    PINCTRL_SetPadMux(PIN_BUZZER, IO_SOURCE_GPIO);
    GIO_SetDirection((GIO_Index_t)PIN_BUZZER, GIO_DIR_OUTPUT);
    GIO_WriteValue((GIO_Index_t)PIN_BUZZER, 1);
#endif

#ifdef USE_WATCHDOG
    // Watchdog will timeout after 10sec
    TMR_WatchDogEnable(TMR_CLK_FREQ * 5);
#endif
}

uint32_t on_deep_sleep_wakeup(const platform_wakeup_call_info_t *info, void *user_data)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#ifdef USE_POWER_LIB
    power_ctrl_deep_sleep_wakeup();
#endif
    setup_peripherals();
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    if (PLATFORM_WAKEUP_REASON_NORMAL == info->reason)
        setup_peripherals();
    else
        GIO_EnableRetentionGroupA(0);
#endif
#ifdef DETECT_KEY
    key_detector_start_on_demand();
#endif
    return 1;
}

uint32_t query_deep_sleep_allowed(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    if (IS_DEBUGGER_ATTACHED())
        return 0;
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#ifdef USE_POWER_LIB
    power_ctrl_before_deep_sleep();
#endif
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    GIO_EnableRetentionGroupA(1);
#endif
    return PLATFORM_ALLOW_DEEP_SLEEP;
}

uint32_t idle_proc(void *dummy, void *user_data)
{
    __DSB();
    __WFI();
    __ISB();
	return 0;
}

#ifdef USE_TRACE
trace_rtt_t trace_ctx = {0};
#endif

const platform_evt_cb_table_t evt_cb_table =
{
    .callbacks = {
        [PLATFORM_CB_EVT_HARD_FAULT] = {
            .f = (f_platform_evt_cb)cb_hard_fault
        },
        [PLATFORM_CB_EVT_ASSERTION] = {
            .f = (f_platform_evt_cb)cb_assertion
        },
        [PLATFORM_CB_EVT_HEAP_OOM] = {
            .f = (f_platform_evt_cb)cb_heap_out_of_mem
        },
        [PLATFORM_CB_EVT_PUTC] = {
            .f = (f_platform_evt_cb)cb_putc
        },
        [PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP] = {
            .f = (f_platform_evt_cb)on_deep_sleep_wakeup
        },
        [PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED] = {
            .f = query_deep_sleep_allowed
        },
        [PLATFORM_CB_EVT_PROFILE_INIT] = {
            .f = setup_profile
        },
#ifdef USE_TRACE
        [PLATFORM_CB_EVT_TRACE] = {
            .f = (f_platform_evt_cb)cb_trace_rtt,
            .user_data = &trace_ctx
        },
#endif
#if ((INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) && (defined CUSTOMIZE_IDLE_PROC))
        [PLATFORM_CB_EVT_IDLE_PROC] = {
            .f = idle_proc
        },
#endif
    }
};

void _app_main()
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    #ifdef USE_POWER_LIB
        power_ctrl_init(POWER_CTRL_MODE_BALANCED);
    #endif
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#ifdef DETECT_KEY
    // configure it only once
    GIO_EnableDeepSleepWakeupSource(PIN_WAKEUP, 1, 1, PINCTRL_PULL_DOWN);
#endif
#endif

#ifdef USE_OSC32K
    platform_config(PLATFORM_CFG_OSC32K_EN, PLATFORM_CFG_ENABLE);
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    while (platform_read_info(PLATFORM_INFO_OSC32K_STATUS) == 0) ;
#endif
    platform_config(PLATFORM_CFG_32K_CLK, PLATFORM_32K_OSC);
    {
        int i;
        for (i = 0; i < 100 * 48; i++) __NOP();
    }
    platform_config(PLATFORM_CFG_RC32K_EN, PLATFORM_CFG_DISABLE);
#else
    platform_config(PLATFORM_CFG_OSC32K_EN, PLATFORM_CFG_DISABLE);
    platform_config(PLATFORM_CFG_32K_CLK_ACC, 500);
#endif

    // setup handlers
    platform_set_evt_callback_table(&evt_cb_table);

#ifdef DETECT_KEY
    key_detect_init(on_key_event);
#endif
    setup_peripherals();

#ifdef USE_TRACE
    trace_rtt_init(&trace_ctx);
    platform_config(PLATFORM_CFG_TRACE_MASK, 0x0);
#endif

    platform_config(PLATFORM_CFG_POWER_SAVING, PLATFORM_CFG_ENABLE);
}
