#include <string.h>
#include "trace.h"
#include "peripheral_uart.h"

void trace_task(trace_info_t *ctx)
{
    for (;;)
    {
wait:
        xSemaphoreTake(ctx->tx_sem, portMAX_DELAY);

        while (ctx->read_next != ctx->write_next)
        {
            if (!apUART_Check_TXFIFO_FULL(ctx->port))
            {
                UART_SendData(ctx->port, ctx->buffer[ctx->read_next]);
                ctx->read_next = (ctx->read_next + 1) & TRACE_BUFF_SIZE_MASK;
            }
            else
                goto wait;
        }
    }
}

void trace_flush(trace_info_t *ctx)
{
    while (ctx->read_next != ctx->write_next)
    {
        while (!apUART_Check_TXFIFO_FULL(ctx->port))
        {
            UART_SendData(ctx->port, ctx->buffer[ctx->read_next]);
            ctx->read_next = (ctx->read_next + 1) & TRACE_BUFF_SIZE_MASK;
        }
    }
}

int trace_add_buffer(trace_info_t *ctx, const uint8_t *buffer, int size, int start)
{
    int remain = TRACE_BUFF_SIZE - start;
    if (remain >= size)
    {
        memcpy(ctx->buffer + start, buffer, size);
        return start + size;
    }
    else
    {
        memcpy(ctx->buffer + start, buffer, remain);
        size -= remain;
        memcpy(ctx->buffer, buffer + remain, size);
        return size;
    }
}

void trace_trigger_output(trace_info_t *ctx)
{
    if (IS_IN_INTERRUPT())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(ctx->tx_sem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
        xSemaphoreGive(ctx->tx_sem);
}

uint32_t cb_trace(const platform_evt_trace_t *trace, trace_info_t *ctx)
{
    uint16_t next;
    int16_t free_size;
    uint8_t use_mutex = !IS_IN_INTERRUPT();
    
    if (use_mutex)
        xSemaphoreTake(ctx->mutex, portMAX_DELAY);
    
    next = ctx->write_next;
    free_size = ctx->read_next - ctx->write_next;
    if (free_size <= 0) free_size += TRACE_BUFF_SIZE;
    if (free_size > 0) free_size--;

    if ((trace->len1 + trace->len2) > free_size)
    {
        if (use_mutex)
            xSemaphoreGive(ctx->mutex);
        trace_trigger_output(ctx);
        return 0;
    }
    
    next = trace_add_buffer(ctx, (const uint8_t *)trace->data1, trace->len1, next) & TRACE_BUFF_SIZE_MASK;
    next = trace_add_buffer(ctx, (const uint8_t *)trace->data2, trace->len2, next) & TRACE_BUFF_SIZE_MASK;
    
    ctx->write_next = next;
    
    if (use_mutex) xSemaphoreGive(ctx->mutex);

    trace_trigger_output(ctx);
    return 0;
}

void trace_init(trace_info_t *ctx)
{
    ctx->tx_sem = xSemaphoreCreateBinary();
    ctx->mutex = xSemaphoreCreateMutex();
    xTaskCreate((TaskFunction_t)trace_task,
               "trace",
               configMINIMAL_STACK_SIZE,
               ctx,
               (configMAX_PRIORITIES - 1),
               &ctx->handle);
}

uint32_t trace_uart_isr(trace_info_t *ctx)
{
    ctx->port->IntClear = ctx->port->Interrupt;
    trace_trigger_output(ctx);
    return 0;
}

