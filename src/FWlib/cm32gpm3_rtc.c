#include "cm32gpm3.h"

void RTC_Enable(const uint8_t flag)
{
    volatile uint32_t * reg = (volatile uint32_t *)(APB_RTC_BASE + 0x130);
    const uint32_t mask = 1 << 3;
    *reg = flag == 0 ? *reg & ~mask : *reg | mask;
}

uint32_t RTC_CurrentHigh(void)
{
    volatile uint32_t * reg = (volatile uint32_t *)(APB_RTC_BASE + 0x130);
    uint32_t r = *reg;
    return (r >> 7) & 0xffff;
}

uint32_t RTC_Current(void)
{
    volatile uint32_t * reg = (volatile uint32_t *)(APB_RTC_BASE + 0x168);
    return *reg;
}

uint64_t RTC_CurrentFull(void)
{
    union
    {
        struct
        {
            uint32_t low;
            uint32_t high;
        } parts;
        uint64_t v;
    } r;
    uint32_t t;
    r.parts.low = RTC_Current();
    r.parts.high = RTC_CurrentHigh();
    t = RTC_Current();
    if (t < r.parts.low)
    {
        r.parts.high = RTC_CurrentHigh();
        r.parts.low = t;
    }
    return r.v;
}

void RTC_SetNextIntOffset(const uint32_t offset)
{
    volatile uint32_t *clr = (volatile uint32_t *)(APB_RTC_BASE + 0x124);
    volatile uint32_t *off = (volatile uint32_t *)(APB_RTC_BASE + 0x10c);
    *clr = 0;
    *off = offset;
}

void RTC_ClearInt(void)
{
    volatile uint32_t * reg = (volatile uint32_t *)(APB_RTC_BASE + 0x130);
    *reg = *reg | 1;
    while ((*reg & (1 << 5)) != 0);
    *reg = *reg & ~1;
}
