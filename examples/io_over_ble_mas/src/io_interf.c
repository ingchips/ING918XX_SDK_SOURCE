#include "io_interf.h"
#include "cm32gpm3.h"
#include "platform_api.h"

#include <string.h>

#define dbg_printf printf

extern const pair_config_t *pair_config;
int8_t send_data(const uint8_t *data, int len);

#if (IO_TYPE == IO_TYPE_UART)
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
        send_data(recv_buff, recv_cnt);
        recv_cnt = 0;
        last_crlf = 0;
    }
    
    if (IS_CRLF(c) && last_crlf)
        return;
    
    last_crlf = IS_CRLF(c);
    
    recv_buff[recv_cnt++] = c;
    
    if (last_crlf)
    {
        send_data(recv_buff, recv_cnt);
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
            while (apUART_Check_RXFIFO_EMPRY(APB_UART0) != 1)
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

#endif

#if (IO_TYPE == IO_TYPE_L2)

#include "io_l2.c"

#endif
