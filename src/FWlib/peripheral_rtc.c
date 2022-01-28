#include "ingsoc.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

void RTC_Enable(const uint8_t flag)
{
    volatile uint32_t * reg = (volatile uint32_t *)(APB_RTC_BASE + 0x130);
    const uint32_t mask = 1 << 3;
    *reg = flag == 0 ? *reg & ~mask : *reg | mask;
}

static uint32_t RTC_ReadStable(volatile uint32_t * reg)
{
    uint32_t r = *reg;
    while ((r != *reg) || (r != *reg))
        r = *reg;
    return r;
}

uint32_t RTC_CurrentHigh(void)
{
    volatile uint32_t * reg = (volatile uint32_t *)(APB_RTC_BASE + 0x130);
    uint32_t r = RTC_ReadStable(reg);
    return (r >> 7) & 0xffff;
}

uint32_t RTC_Current(void)
{
    volatile uint32_t * reg = (volatile uint32_t *)(APB_RTC_BASE + 0x168);
    return RTC_ReadStable(reg);
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

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

void RTC_Enable(uint8_t enable)
{
    if (enable)
        APB_RTC->Ctrl |= 1;
    else
        APB_RTC->Ctrl &= 0xfffffffe;
}

uint16_t RTC_GetTime(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
    uint32_t t = APB_RTC->Cntr;
    *hour = (t >> 12) & 0x1f;
    *minute = (t >> 6) & 0x3f;
    *second = t & 0x3f;
    return t >> 17;
}

void RTC_ModifyTime(uint16_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    APB_RTC->Cntr = ((uint32_t)day << 17)
                    | ((uint32_t)hour << 12)
                    | ((uint32_t)minute << 6)
                    | ((uint32_t)second);
}

int RTC_IsModificationDone(void)
{
    return APB_RTC->Ctrl & (1 << 16);
}

void RTC_ConfigAlarm(uint8_t hour, uint8_t minute, uint8_t second)
{
    APB_RTC->Alarm = ((uint32_t)hour << 12)
                    | ((uint32_t)minute << 6)
                    | ((uint32_t)second);
}

#define _RTC_INT_MASK   (0x3ful << 2)

void RTC_EnableIRQ(uint32_t mask)
{
    APB_RTC->Ctrl &= ~_RTC_INT_MASK;
    APB_RTC->Ctrl |= (mask & _RTC_INT_MASK);
}

uint32_t RTC_GetIntState(void)
{
    return APB_RTC->St & _RTC_INT_MASK;
}

void RTC_ClearIntState(uint32_t state)
{
    APB_RTC->St |= state &  _RTC_INT_MASK;
}

#endif
