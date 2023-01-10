#ifndef __APP_DEBUG_H
#define __APP_DEBUG_H

#include "btstack_util.h"
#include "app_config.h"

// Avoid complaints of unused arguments when log levels are disabled.
static inline void __app_log_unused(const char *format, ...) {};

// printf.
#define app_printf(format, ...)             printf(format, ## __VA_ARGS__)

// info.
#ifdef APP_LOG_INFO_EN
#define app_log_info(format, ...)           app_printf(format "\n",  ## __VA_ARGS__)
#define app_log_info_hexdump(data, size)    printf_hexdump(data, size)
#else
#define app_log_info(...)                   __app_log_unused(__VA_ARGS__)
#define app_log_info_hexdump(data, size)
#endif

// debug.
#ifdef APP_LOG_DEBUG_EN
#define app_log_debug(format, ...)          app_printf(format "\n",  ## __VA_ARGS__)
#define app_log_debug_hexdump(data, size)   printf_hexdump(data, size)
#else
#define app_log_debug(...)                  __app_log_unused(__VA_ARGS__)
#define app_log_debug_hexdump(data, size)
#endif

// error.
#ifdef APP_LOG_ERROR_EN
#define app_log_error(format, ...)          app_printf(format "\n",  ## __VA_ARGS__)
#define app_log_error_hexdump(data, size)   printf_hexdump(data, size)
#else
#define app_log_error(...)                  __app_log_unused(__VA_ARGS__)
#define app_log_error_hexdump(data, size)
#endif

// assert.
#ifdef APP_ASSERT_EN
    #include "platform_api.h"
    #ifndef app_assert
    #define app_assert(condition)         if (condition) {} else { platform_raise_assertion(__FILE__, __LINE__);  }    
    #endif
#else
    #define app_assert(condition)         {}
#endif


#endif // __APP_DEBUG_H
