#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>

#include "audio_service.h"
#include "kb_scan.h"

#include "blink.h"

#include "../../hid_keyboard/src/USBKeyboard.h"
#include "kb_service.h"
#include "app_cfg.h"

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

#define KB_KEY_1        GIO_GPIO_1
#define KB_KEY_2        GIO_GPIO_5
#define KB_KEY_3        GIO_GPIO_7
#define KB_KEY_4        GIO_GPIO_4

const uint8_t key_ids[4] = {KB_KEY_1, KB_KEY_2, KB_KEY_3, KB_KEY_4};

#define KEY_MASK        ((1 << KB_KEY_1) | (1 << KB_KEY_2) | (1 << KB_KEY_3) | (1 << KB_KEY_4))

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);

    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO)
                              | (1 << SYSCTRL_ClkGate_APB_TMR1) | (1 << SYSCTRL_ClkGate_APB_TMR2)
                              | (1 << SYSCTRL_ClkGate_APB_PinCtrl));
    PINCTRL_DisableAllInputs();

    // timer 0 can be used as watchdog, so we use timer 1.
    // setup timer 1 to sampling rate
    TMR_SetCMP(APB_TMR1, TMR_CLK_FREQ / (16000 * OVER_SAMPLING));
	TMR_SetOpMode(APB_TMR1, TMR_CTL_OP_MODE_WRAPPING);
    TMR_Reload(APB_TMR1);
    TMR_IntEnable(APB_TMR1);

    ADC_PowerCtrl(1);
    ADC_Reset();
    ADC_SetClkSel(ADC_CLK_EN | ADC_CLK_16);
    ADC_SetMode(ADC_MODE_LOOP);
    ADC_EnableChannel(ADC_CHANNEL_ID, 1);
    ADC_Enable(1);

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
#elif (APP_TYPE != APP_ING)
    // setup GPIOs for keys
    #define SET_UP_KEY(key)     \
        PINCTRL_SetPadMux(key, IO_SOURCE_GENERAL);  \
        GIO_SetDirection(key, GIO_DIR_INPUT);       \
        GIO_ConfigIntSource(key, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE, GIO_INT_EDGE)

    SET_UP_KEY(KB_KEY_1);
    SET_UP_KEY(KB_KEY_2);
    SET_UP_KEY(KB_KEY_3);
    SET_UP_KEY(KB_KEY_4);
#endif
}

void app_state_changed(const app_state_t state)
{
#if (BOARD == BOARD_REM)
    switch (state)
    {
    case APP_PAIRING:
        blink_style(0, BLINK_FAST);
        break;
    case APP_WAIT_CONN:
        blink_style(0, BLINK_SLOW);
        break;
    case APP_CONN:
        blink_style(0, BLINK_ON);
        break;
    }
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

extern kb_report_trigger_send(void);
extern my_kb_report_t report;

#include "keycodes.h"

const uint8_t key_map[] =
{
    AKEYCODE_RIGHT_BRACKET, // 0x48, //	0 (J2)
    AKEYCODE_BUTTON_R2,     // 0x69, //	1 (J6)
    AKEYCODE_CAPS_LOCK,     // 0x73, //	2 (J10)
    AKEYCODE_BUTTON_R1,     // 0x67, //	3 (J14)
    AKEYCODE_PAGE_UP,       // 0x5c, //	4 (J3)
    AKEYCODE_BUTTON_THUMBL, // 0x6a, //	5 (J7)
    AKEYCODE_MEDIA_PAUSE,   // 0x7f, //	6 (J11)
    AKEYCODE_PAGE_DOWN,     // 0x5d, //	7 (J15)
    AKEYCODE_MEDIA_FAST_FORWARD,    // 0x5A, //	8 (J4)
    AKEYCODE_SOFT_LEFT,     // 0x01, //	9 (J8)
    AKEYCODE_LEFT_BRACKET,  // 0x47, //	10(J12)
    AKEYCODE_MUTE,          // 0x5b, //	11(J16)
    AKEYCODE_CLEAR,         // 0x1c, //	12(J5)
    AKEYCODE_BUTTON_START,  // 0x6c, //	13(J9)
    AKEYCODE_MEDIA_CLOSE,   // 0x80, //	14(J13)
    AKEYCODE_CTRL_RIGHT     // 0x72, // 15(J17)
};

void kb_state_changed(const uint32_t old_state, const uint32_t new_state)
{
    int i;
    uint32_t changed = old_state ^ new_state;
    report.keycode = 0;
    for (i = 0; i < 16; i++)
    {
        if (changed & (1 << i))
        {
            // printf("key %d: %s\n", i, new_state & (1 << i) ? "down" : "up");
            if (new_state & (1 << i))
            {
                report.keycode = key_map[i];
                break;
            }
        }
    }

    kb_report_trigger_send();
}

extern void start_paring(void);

void kb_keep_pressed(const uint32_t keys)
{
#define PARING_KEY  ((1 << 6) | (1 << 9))
#define TALK_KEY    (1 << 12)
    if ((keys & PARING_KEY) == PARING_KEY)
    {
        platform_printf("pairing...\n");
        start_paring();
    }
    if ((keys & TALK_KEY) == TALK_KEY)
    {
        platform_printf("talking...\n");
        start_talking();
    }
}

#else

void kb_state_changed(const uint32_t old_state, const uint32_t new_state) {}

void kb_keep_pressed(const uint32_t keys) {}

#ifdef HAS_KB
extern void kb_report_trigger_send(void);
extern my_kb_report_t report;

uint32_t gpio_isr(void *user_data)
{
    uint32_t current = ~GIO_ReadAll();
    int8_t i = 0;
    report.k_map = 0;

    // report which keys are pressed
    for (i = 0; i < sizeof(key_ids) / sizeof(key_ids[0]); i++)
    {
        if (current & (1 << key_ids[i]))
            report.k_map |= (1 << i);
    }

    kb_report_trigger_send();

    GIO_ClearAllIntStatus();
    return 0;
};

#endif
#endif

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

#if (APP_TYPE == APP_MIBOXS)
    {
        const static uint8_t pub_addr[] = {0x14, 0xAB, 0x56, 0x90, 0xA8, 0x8A};
        sysSetPublicDeviceAddr(pub_addr);
    }
#endif
    
    setup_peripherals();

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup deep sleep handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);
    // setup putc handle
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);

    audio_init();

    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER1, audio_sample_isr, NULL);
#if (BOARD == BOARD_REM)
    platform_set_irq_callback(PLATFORM_CB_IRQ_TIMER2, kb_scan_isr, NULL);
    TMR_Enable(APB_TMR2);
#elif (defined HAS_KB)
    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);
#endif

    return 0;
}
