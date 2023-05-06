#define OPTIONAL_RF_CLK
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "kv_storage.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

#include "eflash.h"
#include <string.h>

#include "board.h"

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

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#define DB_FLASH_ADDRESS  0x42000
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#define DB_FLASH_ADDRESS  0x2042000
#else
#error unknown or unsupported chip family
#endif

#define KB_KEY_1        GIO_GPIO_1
#define KB_KEY_2        GIO_GPIO_5
#define KB_KEY_3        GIO_GPIO_7

#define KEY_MASK        ((1 << KB_KEY_1) | (1 << KB_KEY_2) | (1 << KB_KEY_3))

#define LED_PIN         GIO_GPIO_9

void connection_changed(int connected)
{
    GIO_WriteValue(LED_PIN, connected ? 0 : 1);
}

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO0)
                              | (1 << SYSCTRL_ClkGate_APB_PinCtrl));

    // setup GPIOs for keys
    PINCTRL_DisableAllInputs();
    setup_keys();

    // LED
    PINCTRL_SetPadMux(LED_PIN, IO_SOURCE_GPIO);
    GIO_SetDirection(LED_PIN, GIO_DIR_OUTPUT);
    connection_changed(0);
}

extern void key_pressed(uint32_t keys_mask);

uint32_t gpio_isr(void *user_data)
{
    uint32_t current = ~GIO_ReadAll();
    uint32_t mask = 0;

    // report which keys are pressed
    if (current & (1 << KB_KEY_1))
        mask |= 1;
    if (current & (1 << KB_KEY_2))
        mask |= 2;
    if (current & (1 << KB_KEY_3))
    {
        uint32_t v = 0;
        printf("Erase bonding data\n");
        program_flash(DB_FLASH_ADDRESS, (uint8_t *)&v, sizeof(v));
        printf("... restart\n");
        NVIC_SystemReset();
        while (1) ;
    }

    if (mask)
      key_pressed(mask);

    GIO_ClearAllIntStatus();
    return 0;
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

int db_write_to_flash(const void *db, const int size)
{
    program_flash(DB_FLASH_ADDRESS, (const uint8_t *)db, size);
    return KV_OK;
}

int db_read_from_flash(void *db, const int max_size)
{
    memcpy(db, (void *)DB_FLASH_ADDRESS, max_size);
    return KV_OK;
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(1);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);

    // setup handlers
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ASSERTION, (f_platform_evt_cb)cb_assertion, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP, on_deep_sleep_wakeup, NULL);
    platform_set_evt_callback(PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED, query_deep_sleep_allowed, NULL);
    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);

    setup_peripherals();
    kv_init(db_write_to_flash, db_read_from_flash);

    printf("Usage:\n"
           "Key 1: Accept call\n"
           "Key 2: Reject call\n"
           "Key 3: Clear bonding\n");

    return 0;
}



