#include "profile.h"
#include "cm32gpm3.h"
#include "platform_api.h"
#include "kv_storage.h"
#include "btstack_util.h"
#include "eflash.h"
#include <string.h>
#include <stdio.h>

#include "USBKeyboard.h"

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

#define KB_KEY_1        GIO_GPIO_1
#define KB_KEY_2        GIO_GPIO_5
#define KB_KEY_3        GIO_GPIO_7

#define KEY_MASK        ((1 << KB_KEY_1) | (1 << KB_KEY_2) | (1 << KB_KEY_3))

void setup_peripherals(void)
{
    config_uart(OSC_CLK_FREQ, 115200);
    
    // setup GPIOs for keys
    PINCTRL_DisableAllInputs();
    PINCTRL_SetPadMux(KB_KEY_1, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(KB_KEY_2, IO_SOURCE_GENERAL);
    PINCTRL_SetPadMux(KB_KEY_3, IO_SOURCE_GENERAL);
    GIO_SetDirection(KB_KEY_1, GIO_DIR_INPUT);
    GIO_SetDirection(KB_KEY_2, GIO_DIR_INPUT);
    GIO_SetDirection(KB_KEY_3, GIO_DIR_INPUT);
    GIO_ConfigIntSource(KB_KEY_1, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
    GIO_ConfigIntSource(KB_KEY_2, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
    GIO_ConfigIntSource(KB_KEY_3, GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                        GIO_INT_EDGE);
}

extern kb_report_t report;
extern void kb_state_changed(void);

uint32_t gpio_isr(void *user_data)
{   
    uint32_t current = ~GIO_ReadAll();
    int8_t i = 0;
    
    // report which keys are pressed
    if (current & (1 << KB_KEY_1))
        report.codes[i++] = KEY_1;
    if (current & (1 << KB_KEY_2))
        report.codes[i++] = KEY_2;
    if (current & (1 << KB_KEY_3))
        report.codes[i++] = KEY_3;
    while (i < sizeof(report.codes))
        report.codes[i++] = 0;        
    kb_state_changed();
    
    printf("%02X-%02X-%02X\n", report.codes[0], report.codes[1], report.codes[2]);

    GIO_ClearAllIntStatus();
    return 0;
}

#define DB_FLASH_ADDRESS  0x42000
int db_write_to_flash(const void *db, const int size)
{
    platform_printf("write to flash, size = %d\n", size);
    printf_hexdump(db, size);
    program_flash(DB_FLASH_ADDRESS, (const uint8_t *)db, size);
    return KV_OK;
}

int read_from_flash(void *db, const int max_size)
{
    memcpy(db, (void *)DB_FLASH_ADDRESS, max_size);
    return KV_OK;
}

int app_main()
{
    // If there are *three* crystals on board, *uncomment* below line.
    // Otherwise, below line should be kept commented out.
    // platform_set_rf_clk_source(0);

    platform_set_irq_callback(PLATFORM_CB_IRQ_GPIO, gpio_isr, NULL);
    setup_peripherals();

    sysSetPublicDeviceAddr((uint8_t *)0x2a100);
    // platform_config(PLATFORM_CFG_LOG_HCI, PLATFORM_CFG_ENABLE);
    
    platform_set_evt_callback(PLATFORM_CB_EVT_PUTC, (f_platform_evt_cb)cb_putc, NULL);    
    platform_set_evt_callback(PLATFORM_CB_EVT_HARD_FAULT, (f_platform_evt_cb)cb_hard_fault, NULL);

    platform_set_evt_callback(PLATFORM_CB_EVT_PROFILE_INIT, setup_profile, NULL);
    
    kv_init(db_write_to_flash, read_from_flash);
    return 0;
}

