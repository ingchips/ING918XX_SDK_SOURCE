#include "log_util.h"
#include "platform_api.h"
#include "peripheral_rtc.h"
#include "str_util.h"

static char str_buf[30];

const char *log_rtc_timestamp(void)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    return fmt_rtc_timestamp(str_buf, RTC_Current());
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    return fmt_us_timestamp(str_buf, platform_get_us_time());
#else
    #error unknown or unsupported chip family
#endif
}
