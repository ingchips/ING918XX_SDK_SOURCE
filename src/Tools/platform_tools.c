#include "platform_tools.h"

#include <stddef.h>
#include "ingsoc.h"

static int isr_stack_size = 0;
static uint32_t *isr_stack_top = NULL;

#define STACK_MAGIC 0xBAAAAAAD

static void fill_stack(uint32_t *top, const int size)
{
    int i;
    for (i = 0; i < size >> 2; i++)
    {
        *--top = STACK_MAGIC;
    }
}

static int stack_free_size(const uint32_t *top, const int size)
{
    int i;
    int r = 0;
    top -= size >> 2;
    for (i = 0; i < (size >> 2); i++)
    {
        if (*top++ == STACK_MAGIC)
            r += 4;
        else
            break;
    }
    return r;
}

int platform_instrument_isr_stack(platform_stack_status_t *status)
{
    int min_free = stack_free_size(isr_stack_top, isr_stack_size);
    uint32_t cur = __get_MSP();
    uint32_t top = (uint32_t)isr_stack_top;
    int bytes_free = top >= cur ? isr_stack_size - (int)(top - cur) : -1;
    if (status)
    {
        status->bytes_free = bytes_free;
        status->bytes_minimum_ever_free = min_free;
    }
    if (bytes_free > 0)
        fill_stack((uint32_t *)cur, bytes_free);
    return min_free;
}

void platform_start_instrument_isr_stack(void *top, int size)
{
    isr_stack_top = top;
    isr_stack_size = size;
    fill_stack((uint32_t *)top, size);
}
