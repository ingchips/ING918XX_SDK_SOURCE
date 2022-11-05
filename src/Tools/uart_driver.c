#include <string.h>
#include "uart_driver.h"
#include "peripheral_uart.h"

#include "port_gen_os_driver.h"
#include "platform_api.h"

#define GEN_OS          ((const gen_os_driver_t *)platform_get_gen_os_driver())

#ifndef UART_BUFF_SIZE
#define UART_BUFF_SIZE         (1024)  // must be 2^n
#endif

#define UART_BUFF_SIZE_MASK    (UART_BUFF_SIZE - 1)

typedef struct
{
    uint8_t           buffer[UART_BUFF_SIZE];
    uint16_t          write_next;
    uint16_t          read_next;
    gen_handle_t      handle;
    gen_handle_t      tx_sem;
    UART_TypeDef     *port;
    f_uart_rx_byte    rx_byte_cb;
    void             *user_data;
} uart_driver_ctx_t;

static uart_driver_ctx_t ctx = {0};

void uart_driver_task(void *data)
{
    uart_driver_ctx_t *ctx = (uart_driver_ctx_t *)data;
    for (;;)
    {
wait:
        if (GEN_OS->event_wait(ctx->tx_sem) != 0) continue;

        while (ctx->read_next != ctx->write_next)
        {
            if (!apUART_Check_TXFIFO_FULL(ctx->port))
            {
                UART_SendData(ctx->port, ctx->buffer[ctx->read_next]);
                ctx->read_next = (ctx->read_next + 1) & UART_BUFF_SIZE_MASK;
            }
            else
                goto wait;
        }
    }
}

void driver_trigger_output(uart_driver_ctx_t *ctx)
{
    GEN_OS->event_set(ctx->tx_sem);
}

uint32_t uart_driver_isr(void *user_data)
{
    uint32_t status;

    while(1)
    {
        status = apUART_Get_all_raw_int_stat(ctx.port);
        if (status == 0)
            break;

        ctx.port->IntClear = status;

        // rx int
        if (status & (1 << bsUART_RECEIVE_INTENAB))
        {
            while (apUART_Check_RXFIFO_EMPTY(ctx.port) != 1)
            {
                uint8_t b = ctx.port->DataRead;
                ctx.rx_byte_cb(ctx.user_data, b);
            }
        }

        // tx int
        if (status & (1 << bsUART_TRANSMIT_INTENAB))
            driver_trigger_output(&ctx);
    }
    return 0;
}

void driver_flush_tx()
{
    while (ctx.read_next != ctx.write_next)
    {
        while (!apUART_Check_TXFIFO_FULL(ctx.port))
        {
            UART_SendData(ctx.port, ctx.buffer[ctx.read_next]);
            ctx.read_next = (ctx.read_next + 1) & UART_BUFF_SIZE_MASK;
        }
    }
}

int uart_add_buffer(uart_driver_ctx_t *ctx, const void *buffer, int size, int start)
{
    int remain = UART_BUFF_SIZE - start;
    if (remain >= size)
    {
        memcpy(ctx->buffer + start, buffer, size);
        return start + size;
    }
    else
    {
        memcpy(ctx->buffer + start, buffer, remain);
        size -= remain;
        memcpy(ctx->buffer, (const uint8_t *)buffer + remain, size);
        return size;
    }
}

uint32_t driver_append_tx_data(const void *data, int len)
{
    uint16_t next;
    int16_t free_size;
    uint8_t use_mutex = !IS_IN_INTERRUPT();

    if (use_mutex)
        GEN_OS->enter_critical();

    next = ctx.write_next;
    free_size = ctx.read_next - ctx.write_next;
    if (free_size <= 0) free_size += UART_BUFF_SIZE;
    if (free_size > 0) free_size--;

    if (len > free_size)
    {
        if (use_mutex)
            GEN_OS->leave_critical();
        driver_trigger_output(&ctx);
        return 1;
    }

    next = uart_add_buffer(&ctx, data, len, next) & UART_BUFF_SIZE_MASK;

    ctx.write_next = next;

    if (use_mutex)
        GEN_OS->leave_critical();

    driver_trigger_output(&ctx);
    return 0;
}

void uart_driver_init(UART_TypeDef *port, void *user_data, f_uart_rx_byte rx_byte_cb)
{
    ctx.tx_sem = GEN_OS->event_create();
    ctx.port = port;
    ctx.rx_byte_cb = rx_byte_cb;
    ctx.user_data = user_data;

    GEN_OS->task_create("uart",
                        uart_driver_task,
                        &ctx,
                        512,
                        GEN_TASK_PRIORITY_LOW);
}
