#include "ingsoc.h"

static uint32_t RTC_ReadStable(volatile uint32_t * reg)
{
    uint32_t r = *reg;
    while ((r != *reg) || (r != *reg))
        r = *reg;
    return r;
}

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

void RTC_Enable(const uint8_t flag)
{
    volatile uint32_t * reg = (volatile uint32_t *)(APB_RTC_BASE + 0x130);
    const uint32_t mask = 1 << 3;
    *reg = flag == 0 ? *reg & ~mask : *reg | mask;
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

void RTC_EnableDeepSleepWakeupSource(uint8_t enable)
{
    uint32_t t = io_read(AON2_CTRL_BASE + 0x12c);
    if (enable) 
    {
        t |= 1 << 6;
        APB_RTC->Ctrl |= 1 << 1;
    }
    else
    {
        t &= ~(1u << 6);
        APB_RTC->Ctrl &= ~((uint32_t)0x1 << 1);
    }
    io_write(AON2_CTRL_BASE + 0x12c, t);
}

#define bsRTC_SEC_TRIM      0
#define bsRTC_MIN_TRIM      8
#define bsRTC_HOUR_TRIM     16
#define bsRTC_DAY_TRIM      24

#define _RTC_TRIM_MASK   (0x9ful)

void RTC_DayCntTrim(uint8_t trim, rtc_trim_direction_t dir)
{
    uint8_t v = trim | (dir << 7);
    APB_RTC->Trim &= ~_RTC_TRIM_MASK << bsRTC_DAY_TRIM;
    APB_RTC->Trim |= (v & _RTC_TRIM_MASK) << bsRTC_DAY_TRIM;
}

void RTC_HourCntTrim(uint8_t trim, rtc_trim_direction_t dir)
{
    uint8_t v = trim | (dir << 7);
    APB_RTC->Trim &= ~_RTC_TRIM_MASK << bsRTC_HOUR_TRIM;
    APB_RTC->Trim |= (v & _RTC_TRIM_MASK) << bsRTC_HOUR_TRIM;
}

void RTC_MinCntTrim(uint8_t trim, rtc_trim_direction_t dir)
{
    uint8_t v = trim | (dir << 7);
    APB_RTC->Trim &= ~_RTC_TRIM_MASK << bsRTC_MIN_TRIM;
    APB_RTC->Trim |= (v & _RTC_TRIM_MASK) << bsRTC_MIN_TRIM;
}

void RTC_SecCntTrim(uint8_t trim, rtc_trim_direction_t dir)
{
    uint8_t v = trim | (dir << 7);
    APB_RTC->Trim &= ~_RTC_TRIM_MASK << bsRTC_SEC_TRIM;
    APB_RTC->Trim |= (v & _RTC_TRIM_MASK) << bsRTC_SEC_TRIM;
}

void RTC_SetAllTrimValue(uint8_t day_trim, uint8_t hour_trim, uint8_t min_trim,uint8_t sec_trim)
{
    uint32_t v = ((uint32_t)(day_trim & _RTC_TRIM_MASK) << bsRTC_DAY_TRIM)
                | ((uint32_t)(hour_trim & _RTC_TRIM_MASK) << bsRTC_HOUR_TRIM)
                | ((uint32_t)(min_trim & _RTC_TRIM_MASK) << bsRTC_MIN_TRIM)
                | ((uint32_t)(sec_trim & _RTC_TRIM_MASK) << bsRTC_SEC_TRIM);
    APB_RTC->Trim &= 0;
    APB_RTC->Trim |= v;
}

uint8_t RTC_GetAllTrimValue(uint8_t *hour_trim, uint8_t *min_trim,uint8_t *sec_trim)
{
    uint32_t s = APB_RTC->Trim;
    *hour_trim = (s >> bsRTC_HOUR_TRIM) & _RTC_TRIM_MASK;
    *min_trim = (s >> bsRTC_MIN_TRIM) & _RTC_TRIM_MASK;
    *sec_trim = (s >> bsRTC_SEC_TRIM) & _RTC_TRIM_MASK;

    return (s >> bsRTC_DAY_TRIM) & _RTC_TRIM_MASK;
}

void RTC_ClearAllTrimValue(void)
{
    APB_RTC->Trim &= 0;
}

#define _RTC_CNT_MASK   0x7fful

uint32_t RTC_CurrentHigh(void)
{
    volatile uint32_t * reg = (volatile uint32_t *)(AON2_CTRL_BASE + 0xBC);
    uint32_t r = RTC_ReadStable(reg);
    return r & _RTC_CNT_MASK;
}

uint32_t RTC_Current(void)
{
    volatile uint32_t * reg = (volatile uint32_t *)(AON2_CTRL_BASE + 0xB8);
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

void RTC_EnableFreeRun(uint8_t enable)
{
    #define AON1_REG0   (volatile uint32_t *)AON1_CTRL_BASE 
    #define AON1_REG3   (volatile uint32_t *)(AON1_CTRL_BASE + 0XC) 

    if (enable)
    {
        *AON1_REG0 |= 1u << 3;
        *AON1_REG3 |= 1u << 4;
    }
    else
    {
        *AON1_REG0 &= ~(1u << 3);
        *AON1_REG3 &= ~(1u << 4);
    }
}
#endif
