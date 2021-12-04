#include <string.h>
#include <stdio.h>
#include "trace.h"
#include "peripheral_uart.h"

#include "SEGGER_RTT.c"
#include "eflash.h"

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

__attribute((weak)) int write_flash(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    return 0;
}

__attribute((weak)) int program_flash(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    platform_raise_assertion("trace.c: MISSING eflash.c", __LINE__);
    return 0;
}

static void flash_trace_aligned_append(trace_flash_t *ctx, const uint8_t *data, int len)
{
    while (len > 0)
    {
        uint32_t c;
        int empty = EFLASH_PAGE_SIZE - ctx->offset_in_page;
        if (empty == 0)
        {
            ctx->cur_page += EFLASH_PAGE_SIZE;
            if (ctx->cur_page >= ctx->start_addr + ctx->total_size)
                ctx->cur_page = ctx->start_addr;
            program_flash(ctx->cur_page, (uint8_t *)&ctx->page_cnt, sizeof(ctx->page_cnt));
            ctx->page_cnt++;
            ctx->offset_in_page = sizeof(ctx->page_cnt);
            empty = EFLASH_PAGE_SIZE - sizeof(ctx->page_cnt);
        };
        
        {
            c = len > empty ? empty : len;
            write_flash(ctx->cur_page + ctx->offset_in_page, data, c);
            ctx->offset_in_page += c;
        }
        
        data += c;
        len -= c;
    }
}

static void flash_trace_append(trace_flash_t *ctx, const uint8_t *data, int len)
{
    if (len < 1) return;
    if (ctx->frag_size)
    {
        int c = len >= 4 - ctx->frag_size ? 4 - ctx->frag_size : len;
        len -= c;
        memcpy(ctx->frag + ctx->frag_size, data, c);
        data += c;
        ctx->frag_size += c;
        if (ctx->frag_size == 4)
        {
            flash_trace_aligned_append(ctx, ctx->frag, 4);
            ctx->frag_size = 0;
        }
    }
    
    if (len < 1) return;
    
    int rounded = len & ~0x3ul;    
    flash_trace_aligned_append(ctx, data, rounded);
    
    if (len > rounded)
    {
        data += rounded;
        ctx->frag_size = len - rounded;
        memcpy(ctx->frag, data, ctx->frag_size);
    }
}

uint32_t cb_trace_flash(const platform_evt_trace_t *trace, trace_flash_t *ctx)
{
    if (ctx->enable == 0) return 0;

    uint8_t use_mutex = !IS_IN_INTERRUPT();

    if (use_mutex) xSemaphoreTake(ctx->mutex, portMAX_DELAY);
    
    flash_trace_append(ctx, trace->data1, trace->len1);
    flash_trace_append(ctx, trace->data2, trace->len2);
    
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

void trace_flash_init(trace_flash_t *ctx, uint32_t flash_start_addr, uint32_t total_size)
{
    ctx->mutex = xSemaphoreCreateMutex();
    ctx->enable = 0;
    ctx->offset_in_page = EFLASH_PAGE_SIZE;
    ctx->start_addr = flash_start_addr;
    ctx->total_size = total_size;
    ctx->frag_size = 0;
    ctx->cur_page =  flash_start_addr + total_size;
}

void trace_flash_enable(trace_flash_t *ctx, int enable)
{
    ctx->enable = enable;
}

void trace_flash_erase_all(trace_flash_t *ctx)
{
    uint32_t v = 0xfffffffful;
    uint32_t addr = ctx->start_addr;
    while (addr < ctx->start_addr + ctx->total_size)
    {
        program_flash(addr, (uint8_t *)&v, sizeof(v));
        addr += EFLASH_PAGE_SIZE;
    }
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
