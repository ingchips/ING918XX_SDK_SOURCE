#include "io_interf.h"
#include "ingsoc.h"
#include "platform_api.h"

#include <string.h>
#include <stdio.h>

#define dbg_printf printf

const pair_config_t pair_config =
{
    .master =
    {
        .ble_addr = { 0xE0, 0xEF, 0x85, 0x8F, 0x1A, 0xc2}
    },
    .slave =
    {
        .ble_addr = { 0xE0, 0xEF, 0x85, 0x8F, 0x1A, 0xc3}
    },
};

int8_t send_data(const uint8_t *data, int len, int flush);

#if (IO_TYPE == IO_TYPE_UART_STR)
extern uint32_t cb_putc(char *c, void *dummy);

void HANDLE_FUNC(const uint8_t *data, const int len)
{
    int l = len;
    while (l--)
    {
        cb_putc((char *)data, NULL);
        data++;
    }
}

static uint8_t recv_buff[100];
static int recv_cnt;

#define IS_CRLF(c)  ((10 == (c)) || (13 == (c)))

void recv_char(uint8_t c)
{
    static int last_crlf = 0;
    if (recv_cnt >= sizeof(recv_buff))
    {
        send_data(recv_buff, recv_cnt, 1);
        recv_cnt = 0;
        last_crlf = 0;
    }

    if (IS_CRLF(c) && last_crlf)
        return;

    last_crlf = IS_CRLF(c);

    recv_buff[recv_cnt++] = c;

    if (last_crlf)
    {
        send_data(recv_buff, recv_cnt, 1);
        recv_cnt = 0;
    }
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
                recv_char(APB_UART0->DataRead);
        }
    }
    return 0;
}

void io_interf_setup_peripherals()
{
}

void io_interf_init()
{
    platform_set_irq_callback(PLATFORM_CB_IRQ_UART0, uart_isr, NULL);
}

#elif (IO_TYPE == IO_TYPE_UART_BIN)

#include "uart_driver.h"

#define COMM_PORT           APB_UART1
#define PIN_COMM_RX         GIO_GPIO_8
#define PIN_COMM_TX         GIO_GPIO_7

void HANDLE_FUNC(const uint8_t *data, const int len)
{
    if (driver_append_tx_data(data, len))
        dbg_printf("data lost: %d\n", len);
}

void recv_char(void *user_data, uint8_t c)
{
    send_data(&c, 1, 0);
}

static void config_uart(uint32_t freq, uint32_t baud)
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
    config.rxfifo_waterlevel = 7;
    config.txfifo_waterlevel = 1;
    config.ClockFrequency    = freq;
    config.BaudRate          = baud;

    apUART_Initialize(COMM_PORT, &config, (1 << bsUART_RECEIVE_INTENAB) | (1 << bsUART_TRANSMIT_INTENAB));
}

void io_interf_setup_peripherals()
{
    SYSCTRL_ClearClkGateMulti(   (1 << SYSCTRL_ClkGate_APB_PinCtrl)
                              || (1 << SYSCTRL_ClkGate_APB_UART1));
    config_uart(OSC_CLK_FREQ, 921600);

    PINCTRL_SetPadMux(PIN_COMM_RX, IO_SOURCE_GENERAL);
    PINCTRL_SelUartRxdIn(UART_PORT_1, PIN_COMM_RX);
    PINCTRL_SetPadMux(PIN_COMM_TX, IO_SOURCE_UART1_TXD);
}

void io_interf_init()
{
    uart_driver_init(COMM_PORT, NULL, recv_char);
    platform_set_irq_callback(PLATFORM_CB_IRQ_UART1, uart_driver_isr, NULL);
}

#elif (IO_TYPE == IO_TYPE_LOOPBACK)

void HANDLE_FUNC(const uint8_t *data, const int len)
{
    send_data(data, len, 0);
}

void io_interf_setup_peripherals()
{
}

void io_interf_init()
{
}

#elif (IO_TYPE == IO_TYPE_USB_BIN)
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#include "peripheral_usb.c"
#include "usb_driver.c"

void HANDLE_FUNC(const uint8_t *data, const int len)
{
    if(bsp_usb_send_data(data, len))
        dbg_printf("data lost: %d\n", len);
}

void io_interf_push_data(uint8_t *data, uint32_t len)
{
    send_data(data, len, 0);
}

void io_interf_setup_peripherals(){}

void io_interf_init()
{
    bsp_usb_init();
}
#endif
#endif
