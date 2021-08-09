#include <string.h>
#include <stdio.h>
#include "trace.h"
#include "peripheral_uart.h"

#include "SEGGER_RTT.c"

void trace_task(trace_uart_t *ctx)
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

void trace_flush(trace_uart_t *ctx)
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

int trace_add_buffer(trace_uart_t *ctx, const uint8_t *buffer, int size, int start)
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

void trace_trigger_output(trace_uart_t *ctx)
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

uint32_t cb_trace_uart(const platform_evt_trace_t *trace, trace_uart_t *ctx)
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

uint32_t cb_trace_rtt(const platform_evt_trace_t *trace, trace_rtt_t *ctx)
{
    int free_size;
    uint8_t use_mutex = !IS_IN_INTERRUPT();

    if (use_mutex) xSemaphoreTake(ctx->mutex, portMAX_DELAY);
    
    free_size = SEGGER_RTT_GetAvailWriteSpace(0);
    if (trace->len1 + trace->len2 < free_size)
    {
        SEGGER_RTT_Write(0, trace->data1, trace->len1);
        SEGGER_RTT_Write(0, trace->data2, trace->len2);
    }
    
    if (use_mutex) xSemaphoreGive(ctx->mutex);
    
    return 0;
}

void trace_uart_init(trace_uart_t *ctx)
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

void trace_rtt_init(trace_rtt_t *ctx)
{
    SEGGER_RTT_Init();
    ctx->mutex = xSemaphoreCreateMutex();
}

uint32_t trace_uart_isr(trace_uart_t *ctx)
{
    ctx->port->IntClear = ctx->port->Interrupt;
    trace_trigger_output(ctx);
    return 0;
}

static char nibble_to_char(int v)
{
    return v <= 9 ? v - 0 + '0' : v - 10 + 'A';
}

static void hex_dump_line(const uint8_t *buf, int size, char *str, f_trace_puts f_puts)
{
    int i;
    uint8_t sum = 0;
    char *s = str + 1;
    for (i = 0; i < size; i++)
    {
        s[0] = nibble_to_char(buf[i] >> 4);
        s[1] = nibble_to_char(buf[i] & 0xf);
        sum += buf[i];
        s += 2;
    }
    sum = 1 + ~sum;
    s[0] = nibble_to_char(sum >> 4);
    s[1] = nibble_to_char(sum & 0xf);
    s[2] = '\0';
    f_puts(str);
}

static void hex_dump(char *str, uint8_t *buf, f_trace_puts f_puts, uint32_t base, int size)
{
#define HEX_REC_SIZE   16
    int i;
    uint16_t offset = 0;
    
    *(uint32_t*)buf = 0x04000002;
    buf[4] = base >> 24;
    buf[5] = (base >> 16) & 0xff;
    hex_dump_line(buf, 6, str, f_puts);
    
    *(uint32_t*)buf = 0x00000010;
    for (i = 0; i < size * 1024 / HEX_REC_SIZE; i++)
    {
        buf[1] = offset >> 8;
        buf[2] = offset & 0xff;
        memcpy(buf + 4, (void *)base, HEX_REC_SIZE);
        base += HEX_REC_SIZE;
        offset += 16;
        hex_dump_line(buf, 4 + HEX_REC_SIZE, str, f_puts);
    }
}

void trace_full_dump(f_trace_puts f_puts, int size)
{
    static char str[46];
    static uint8_t buf[HEX_REC_SIZE + 4];    
    sprintf(str,    " PC: %08x", (uint32_t)trace_full_dump); f_puts(str);
    sprintf(str,    "MSP: %08x", __get_MSP()); f_puts(str);
    sprintf(str,    "PSP: %08x", __get_PSP()); f_puts(str);
    sprintf(str,    "CTL: %08x", __get_CONTROL()); f_puts(str);
    str[0] = ':';
    hex_dump(str, buf, f_puts, 0x20000000, size);
    hex_dump(str, buf, f_puts, 0x400A0000, size);
}
