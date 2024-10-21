#include "ingsoc.h"
#include "peripheral_rtc.h"

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

#ifndef SOFTWARE_RTC_DHMS

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
    return APB_RTC->St & (1 << 16);
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
    APB_RTC->St = state &  _RTC_INT_MASK;
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

#define bsRTC_TRIM_DIR      7

#define _RTC_TRIM_MASK   (0x9ful)

void RTC_DayCntTrim(uint8_t trim, rtc_trim_direction_t dir)
{
    uint8_t v = trim | (dir << bsRTC_TRIM_DIR);
    APB_RTC->Trim &= ~_RTC_TRIM_MASK << bsRTC_DAY_TRIM;
    APB_RTC->Trim |= (v & _RTC_TRIM_MASK) << bsRTC_DAY_TRIM;
}

void RTC_HourCntTrim(uint8_t trim, rtc_trim_direction_t dir)
{
    uint8_t v = trim | (dir << bsRTC_TRIM_DIR);
    APB_RTC->Trim &= ~_RTC_TRIM_MASK << bsRTC_HOUR_TRIM;
    APB_RTC->Trim |= (v & _RTC_TRIM_MASK) << bsRTC_HOUR_TRIM;
}

void RTC_MinCntTrim(uint8_t trim, rtc_trim_direction_t dir)
{
    uint8_t v = trim | (dir << bsRTC_TRIM_DIR);
    APB_RTC->Trim &= ~_RTC_TRIM_MASK << bsRTC_MIN_TRIM;
    APB_RTC->Trim |= (v & _RTC_TRIM_MASK) << bsRTC_MIN_TRIM;
}

void RTC_SecCntTrim(uint8_t trim, rtc_trim_direction_t dir)
{
    uint8_t v = trim | (dir << bsRTC_TRIM_DIR);
    APB_RTC->Trim &= ~_RTC_TRIM_MASK << bsRTC_SEC_TRIM;
    APB_RTC->Trim |= (v & _RTC_TRIM_MASK) << bsRTC_SEC_TRIM;
}

void RTC_HSecCntTrim(uint16_t trim)
{
    APB_RTC->SEC_CFG = trim & 0x3fff;
}

void RTC_SetAllTrimValue(uint8_t day_trim, uint8_t hour_trim, uint8_t min_trim,uint8_t sec_trim)
{
    uint32_t v = ((uint32_t)(day_trim & _RTC_TRIM_MASK) << bsRTC_DAY_TRIM)
                | ((uint32_t)(hour_trim & _RTC_TRIM_MASK) << bsRTC_HOUR_TRIM)
                | ((uint32_t)(min_trim & _RTC_TRIM_MASK) << bsRTC_MIN_TRIM)
                | ((uint32_t)(sec_trim & _RTC_TRIM_MASK) << bsRTC_SEC_TRIM);
    APB_RTC->Trim = v;
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

static int multiply_and_remainder(int value, int mult, uint32_t divider, int *quotient)
{
    int sign = value > 0 ? 1 : -1;
    int64_t t = value;
    t *= mult;
    *quotient = (int)(t / divider);
    int remainder = t - *quotient * divider;

    if ((remainder >= (int)(divider / 2))
        || (remainder <= - (int)(divider / 2)))
    {
        *quotient += sign;
        remainder -= sign * divider;
    }

    return remainder;
}

static uint8_t convert_to_rtc_trim(int trim)
{
    uint8_t r;
    if (trim >= 0)
    {
        r = trim <= 0x1f ? (uint8_t)trim : 0x1f;
        r |= RTC_TRIM_SLOW_DOWN << bsRTC_TRIM_DIR;
    }
    else
    {
        trim *= -1;
        r = trim <= 0x1f ? (uint8_t)trim : 0x1f;
        r |= RTC_TRIM_SPEED_UP << bsRTC_TRIM_DIR;
    }
    return r;
}

int RTC_Trim(uint32_t cali_value)
{
    int freq_hz;
    int remainder;

    remainder = multiply_and_remainder(65536, 1000000, cali_value, &freq_hz);

    int day_trim;
    int hour_trim;
    int min_trim;
    int sec_trim;

    #define IDEAL_FREQ  32768

    // try to improve: SEC_CFG is half second
    if ((freq_hz <= IDEAL_FREQ) && (freq_hz & 1))
    {
        freq_hz--;
        remainder += cali_value;
    }

    if (freq_hz <= IDEAL_FREQ)
    {
        uint16_t v = (freq_hz / 2) - 1;
        APB_RTC->SEC_CFG = v;
        sec_trim = 0;
    }
    else if (freq_hz <= IDEAL_FREQ + 0x1f)
    {
        sec_trim = (freq_hz - IDEAL_FREQ) | (RTC_TRIM_SLOW_DOWN << bsRTC_TRIM_DIR);
    }
    else
        return 1;

    remainder = multiply_and_remainder(remainder, 60, cali_value, &min_trim);
    remainder = multiply_and_remainder(remainder, 60, cali_value, &hour_trim);
    remainder = multiply_and_remainder(remainder, 24, cali_value, &day_trim);

    RTC_SetAllTrimValue(convert_to_rtc_trim(day_trim),
                        convert_to_rtc_trim(hour_trim),
                        convert_to_rtc_trim(min_trim),
                        convert_to_rtc_trim(sec_trim));

    return 0;
}

#else
void RTC_Enable(uint8_t enable)
{
}
#endif

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

#ifdef SOFTWARE_RTC_DHMS

#include "platform_api.h"

struct software_rtc
{
    int64_t             hms_offset;
    f_platform_irq_cb   cb;
    void *              user_data;
    uint32_t            last_timer_alarm;
    uint32_t            last_timer_day;
    uint32_t            last_timer_hour;
    uint32_t            last_timer_minute;
    uint32_t            last_timer_second;
    uint32_t            last_timer_half_sec;
    uint16_t            irq_state;
    uint16_t            irq_mask;
} software_rtc = {0};

#define US625_PER_SECOND       (1000000 / 625)
#define US625_PER_HALF_SECOND  (US625_PER_SECOND / 2)
#define US625_PER_MINUTE       (US625_PER_SECOND * 60)
#define US625_PER_HOUR         (US625_PER_MINUTE * 60)
#define US625_PER_DAY          (US625_PER_HOUR * 24)

#define DAY_COUNTER_MASK        0x1f

static void invoke_isr(uint16_t flag)
{
    software_rtc.irq_state |= flag;
    if ((software_rtc.irq_state & software_rtc.irq_mask)
        && (software_rtc.cb))
        software_rtc.cb(software_rtc.user_data);
}

static void config_next_timer(uint16_t flag, uint32_t delay, void (*cb)(void),
                              uint32_t *last_timer)
{
    if (software_rtc.irq_mask & flag)
    {
        *last_timer += delay;
        platform_set_abs_timer(cb, *last_timer);
    }
}

#define TIMER_BODY(n1, n2)  do { \
    config_next_timer(RTC_IRQ_ ## n1, US625_PER_ ## n1, timer_cb_ ## n2, &software_rtc.last_timer_ ## n2); \
    invoke_isr(RTC_IRQ_ ## n1); } while (0)


#define DEF_TIMER_CB(n1, n2)            \
    static void timer_cb_ ## n2(void)   \
    {                                   \
        TIMER_BODY(n1, n2);             \
    }

DEF_TIMER_CB(DAY, day)
DEF_TIMER_CB(HOUR, hour)
DEF_TIMER_CB(MINUTE, minute)
DEF_TIMER_CB(SECOND, second)
DEF_TIMER_CB(HALF_SECOND, half_sec)

static void timer_cb_alarm(void)
{
    software_rtc.last_timer_alarm += US625_PER_DAY;
    platform_set_abs_timer(timer_cb_alarm, software_rtc.last_timer_alarm);
    invoke_isr(RTC_IRQ_ALARM);
}

int64_t get_full_625_counter(void)
{
    return (int64_t)platform_get_us_time() / 625;
}

uint16_t RTC_GetTime(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
    int64_t t = get_full_625_counter() + software_rtc.hms_offset;
    t /= US625_PER_SECOND;
    *second = t % 60;
    t /= 60;
    *minute = t % 60;
    t /= 60;
    *hour = t % 24;
    return (t / 24) & DAY_COUNTER_MASK;
}

void RTC_ModifyTime(uint16_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    int64_t t = get_full_625_counter();
    int64_t k = day & DAY_COUNTER_MASK;
    k *= 24;
    k += hour;
    k *= 60;
    k += minute;
    k *= 60;
    k += second;
    k *= US625_PER_SECOND;
    software_rtc.hms_offset = k - t;
    RTC_EnableIRQ(software_rtc.irq_mask);
}

int RTC_IsModificationDone(void)
{
    return 1;
}

void RTC_ConfigAlarm(uint8_t hour, uint8_t minute, uint8_t second)
{
    int64_t t = get_full_625_counter() + software_rtc.hms_offset;
    int64_t start = t / US625_PER_DAY * US625_PER_DAY;
    uint32_t k = hour;
    k *= 60;
    k += minute;
    k *= 60;
    k += second;
    k *= US625_PER_SECOND;
    start += k;
    if (start < t) start += US625_PER_DAY;
    software_rtc.last_timer_alarm = platform_get_timer_counter() + (uint32_t)(start - t);
    platform_set_abs_timer(timer_cb_alarm, software_rtc.last_timer_alarm);
}

void RTC_EnableIRQ(uint32_t mask)
{
    software_rtc.irq_mask = (uint16_t)mask;
    int64_t t = get_full_625_counter() + software_rtc.hms_offset;
    if (software_rtc.irq_mask & RTC_IRQ_DAY)
    {
        int64_t start = t / US625_PER_DAY * US625_PER_DAY;
        start += US625_PER_DAY;
        software_rtc.last_timer_day = start - software_rtc.hms_offset + 1;
        platform_set_abs_timer(timer_cb_day, software_rtc.last_timer_day);
    }
    else
        platform_delete_timer(timer_cb_day);

    if (software_rtc.irq_mask & RTC_IRQ_HOUR)
    {
        int64_t start = t / US625_PER_HOUR * US625_PER_HOUR;
        start += US625_PER_HOUR;
        software_rtc.last_timer_hour = start - software_rtc.hms_offset + 1;
        platform_set_abs_timer(timer_cb_hour, software_rtc.last_timer_hour);
    }
    else
        platform_delete_timer(timer_cb_hour);

    if (software_rtc.irq_mask & RTC_IRQ_MINUTE)
    {
        int64_t start = t / US625_PER_MINUTE * US625_PER_MINUTE;
        start += US625_PER_MINUTE;
        software_rtc.last_timer_minute = start - software_rtc.hms_offset + 1;
        platform_set_abs_timer(timer_cb_minute, software_rtc.last_timer_minute);
    }
    else
        platform_delete_timer(timer_cb_minute);

    if (software_rtc.irq_mask & RTC_IRQ_SECOND)
    {
        int64_t start = t / US625_PER_SECOND * US625_PER_SECOND;
        start += US625_PER_SECOND;
        software_rtc.last_timer_second = start - software_rtc.hms_offset + 1;
        platform_set_abs_timer(timer_cb_second, software_rtc.last_timer_second);
    }
    else
        platform_delete_timer(timer_cb_second);

    if (software_rtc.irq_mask & RTC_IRQ_HALF_SECOND)
    {
        int64_t start = t / US625_PER_HALF_SECOND * US625_PER_HALF_SECOND;
        start += US625_PER_HALF_SECOND;
        software_rtc.last_timer_half_sec = start - software_rtc.hms_offset + 1;
        platform_set_abs_timer(timer_cb_half_sec, software_rtc.last_timer_half_sec);
    }
    else
        platform_delete_timer(timer_cb_half_sec);
}

uint32_t RTC_GetIntState(void)
{
    return software_rtc.irq_state;
}

void RTC_ClearIntState(uint32_t state)
{
    software_rtc.irq_state &= ~(uint16_t)state;
}

void RTC_SoftSetISR(uint32_t (*irq_cb)(void *user_data), void *user_data)
{
    software_rtc.cb = irq_cb;
    software_rtc.user_data = user_data;
}

#endif
