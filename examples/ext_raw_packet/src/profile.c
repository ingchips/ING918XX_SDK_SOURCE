#include "ingsoc.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#define RX_DELAY                300
#else
#define RX_DELAY                600
#endif

#ifdef APP_TESTER

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#error APP_TESTER is not supported on ING918
#endif

#include "profile_iqcmd.c"

#include "uart_console.c"

#else

#include "profile_demo.c"

static void console_rx_data(const char *d, uint8_t len)
{
}

#endif

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
