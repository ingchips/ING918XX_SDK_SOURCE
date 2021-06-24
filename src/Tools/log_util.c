#include "log_util.h"
#include <stdio.h>
#include "platform_api.h"
#include "peripheral_rtc.h"
#include "str_util.h"

static char str_buf[30];

const char *log_rtc_timestamp(void)
{
    return fmt_rtc_timestamp(str_buf, RTC_Current());
}
