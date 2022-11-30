#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "profile.h"
#include "container.h"
#include "rom_tools.h"
#include "eflash.h"

#include "board.h"

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

void config_uart(uint32_t freq, uint32_t baud, uint32_t int_mask)
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

    apUART_Initialize(PRINT_PORT, &config, int_mask);
}

#define ARRAY_LEN(x)    (sizeof(x)/sizeof(x[0]))

#if (BOARD_ID == BOARD_ING91881B_02_02_05)

const static uint8_t led_pins[] = {
    GIO_GPIO_19, GIO_GPIO_18, GIO_GPIO_17, GIO_GPIO_16,
    GIO_GPIO_13, GIO_GPIO_12, GIO_GPIO_9, GIO_GPIO_6,
};

const static uint8_t key_pins[] = {
    IO_NOT_A_PIN, GIO_GPIO_1, GIO_GPIO_5, GIO_GPIO_7,
    GIO_GPIO_4
};

#define LED_ON          0
#define LED_OFF         1

#define KEY_DOWN        0

#define BUZZ_PIN        GIO_GPIO_8
#define IIC_SCL_PIN     GIO_GPIO_10
#define IIC_SDA_PIN     GIO_GPIO_11

#elif (BOARD_ID == BOARD_ING91881B_02_02_04)

const static uint8_t led_pins[] = {
    GIO_GPIO_19, GIO_GPIO_18, GIO_GPIO_17, GIO_GPIO_16,
    GIO_GPIO_9, GIO_GPIO_6,
};

const static uint8_t key_pins[] = {
    IO_NOT_A_PIN, GIO_GPIO_1, GIO_GPIO_5, GIO_GPIO_7,
    GIO_GPIO_4
};

#define LED_ON          0
#define LED_OFF         1

#define KEY_DOWN        0

#define BUZZ_PIN        GIO_GPIO_8
#define IIC_SCL_PIN     GIO_GPIO_14
#define IIC_SDA_PIN     GIO_GPIO_15

#else
    #error unknown BOARD_ID
#endif

extern void on_key_changed(int i, int value);

uint32_t gpio_isr(void *user_data)
{
    int i;
    uint32_t current = GIO_ReadAll();
    uint32_t status = GIO_GetAllIntStatus();
    GIO_ClearAllIntStatus();

    for (i = 0; i < ARRAY_LEN(key_pins); i++)
    {
        if (key_pins[i] == IO_NOT_A_PIN) continue;
        if (status & (1 << key_pins[i]))
            on_key_changed(i, ((current >> key_pins[i]) & 1) == KEY_DOWN ? 1 : 0);
    }

    return 0;
}

void led_ctrl(int id, int state)
{
    if ((0 <= id) && (id < ARRAY_LEN(led_pins)) && (led_pins[id] != IO_NOT_A_PIN))
    {
        GIO_WriteValue((GIO_Index_t)led_pins[id], state ? LED_ON : LED_OFF);
    }
}

void led_toggle(int id)
{
    static uint32_t state = 0;
    if ((0 <= id) && (id < ARRAY_LEN(led_pins)) && (led_pins[id] != IO_NOT_A_PIN))
    {
        uint32_t mask = 1ul << led_pins[id];
        GIO_WriteValue((GIO_Index_t)led_pins[id], state & mask ? LED_ON : LED_OFF);
        state ^= mask;
    }
}

void buzz(int freq)
{
    set_buzzer_freq(freq);
}

uint8_t f_to_u8(float v)
{
    int r = (int)(v * 255);
    if (r < 0) return 0;
    else if (r >= 255) return 255;
    else return r;
}

float hue2rgb(float p, float q, float t)
{
    if(t < 0) t += 1;
    if(t > 1) t -= 1;
    if(t < 1/6.) return p + (q - p) * 6 * t;
    if(t < 1/2.) return q;
    if(t < 2/3.) return p + (q - p) * (2/3. - t) * 6;
    return p;
}

#define HUE2RGB(p, q, t)    f_to_u8(hue2rgb(p, q, t))

void hsl_to_rgb(float H, float S, float L,
                uint8_t *R, uint8_t *G, uint8_t *B)
{
    float q, p;

    if (0.0 == S)
    {
        *R = L;
        *G = L;
        *B = L;
        return;
    }

    q = L < 0.5 ? L * (1 + S) : L + S - L * S;
    p = 2 * L - q;
    *R = HUE2RGB(p, q, H + 1/3.);
    *G = HUE2RGB(p, q, H);
    *B = HUE2RGB(p, q, H - 1/3.);
}

void set_led_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    __disable_irq();
#if (BOARD_ID == BOARD_ID_020205)
    set_rgb_led_color(r, g, b);
#else
    set_rgb_led_color(r, b, g);
#endif
    __enable_irq();
}

void set_led_hsl(float H, float S, float L)
{
    uint8_t r, g, b;
    hsl_to_rgb(H, S, L, &r, &g, &b);
    set_led_rgb(r, g, b);
}

uint32_t uart_isr(void *user_data)
{
    __disable_irq();
    PRINT_PORT->IntClear = apUART_Get_all_raw_int_stat(PRINT_PORT);
    while (!apUART_Check_RXFIFO_EMPTY(PRINT_PORT))
    {
        volatile uint32_t trash = PRINT_PORT->DataRead;
        (void)trash;
    }
    config_uart(OSC_CLK_FREQ, 115200, 0);

    typedef  void (*pFunction)(void);
    __disable_irq();
    #define BOOT_ADDR 0x44000
    {
        int i;
        SysTick->CTRL = 0;

        SYSCTRL_WriteBlockRst(0);
        SYSCTRL_WriteBlockRst(0x3ffffful);

        __set_MSP(*(uint32_t *)(BOOT_ADDR));
        ((pFunction)(*(uint32_t *) (BOOT_ADDR + 4)))();
    }

    return 0;
}

void setup_peripherals(void)
{
    int i;
    SYSCTRL_ClearClkGateMulti(0
                            | (1 << SYSCTRL_ClkGate_APB_TMR0)
                            | (1 << SYSCTRL_ClkGate_APB_GPIO)
                            | (1 << SYSCTRL_ClkGate_APB_PinCtrl)
                            | (1 << SYSCTRL_ClkGate_APB_PWM)
                            | (1 << SYSCTRL_ClkGate_APB_I2C0));
    config_uart(OSC_CLK_FREQ, 115200, 1 << bsUART_RECEIVE_INTENAB);

    for (i = 0; i < ARRAY_LEN(led_pins); i++)
    {
        if (led_pins[i] == IO_NOT_A_PIN) continue;
        PINCTRL_SetPadMux(led_pins[i], IO_SOURCE_GPIO);
        GIO_SetDirection((GIO_Index_t)led_pins[i], GIO_DIR_OUTPUT);
        GIO_WriteValue((GIO_Index_t)led_pins[i], LED_OFF);
    }

    setup_keys();

    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);
    setup_rgb_led();
    set_rgb_led_color(0, 0, 0);

    setup_buzzer();
    buzz(0);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PINCTRL_SetPadMux(IIC_SCL_PIN, IO_SOURCE_I2C0_SCL_O);
    PINCTRL_SetPadMux(IIC_SDA_PIN, IO_SOURCE_I2C0_SDO);
    PINCTRL_SelI2cSclIn(I2C_PORT, IIC_SCL_PIN);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    PINCTRL_SetPadMux(IIC_SCL_PIN, IO_SOURCE_I2C0_SCL_OUT);
    PINCTRL_SetPadMux(IIC_SDA_PIN, IO_SOURCE_I2C0_SDA_OUT);
    PINCTRL_SelI2cIn(I2C_PORT, IIC_SCL_PIN, IIC_SDA_PIN);
#else
    #error unknown or unsupported chip family
#endif

    platform_set_irq_callback(PLATFORM_CB_IRQ_UART0, uart_isr, NULL);
}

float read_adc(int channel)
{
    ADC_Reset();
    ADC_PowerCtrl(1);

    ADC_SetClkSel(ADC_CLK_EN | ADC_CLK_128);
    ADC_SetMode(ADC_MODE_LOOP);
    ADC_EnableChannel(channel == 0 ? 1 : 0, 1);
    ADC_EnableChannel(channel, 1);
    ADC_Enable(1);

    while (ADC_IsChannelDataValid(channel) == 0) ;
    uint16_t voltage = ADC_ReadChannelData(channel);

    ADC_ClearChannelDataValid(channel);
    while (ADC_IsChannelDataValid(channel) == 0) ;
    voltage = ADC_ReadChannelData(channel);

    ADC_Enable(0);
    ADC_PowerCtrl(0);

    return voltage * (3.3 / 1024.);
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
    return 0;
}
