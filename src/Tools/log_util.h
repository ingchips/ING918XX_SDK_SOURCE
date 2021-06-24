#ifndef _log_util_h
#define _log_util_h

#ifndef PRINTF
#define PRINTF platform_printf
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_LEVEL_INFO      0
#define LOG_LEVEL_WARN      1
#define LOG_LEVEL_ERROR     2
#define LOG_LEVEL_MSG       3

const char *log_rtc_timestamp(void);

#ifdef __cplusplus
}
#endif

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#define LOG_PRINT(TAG, ...)  do { PRINTF("[%s] %s: ", log_rtc_timestamp(), TAG); PRINTF(__VA_ARGS__); PRINTF("\n"); } while (0)

#if (LOG_LEVEL <= LOG_LEVEL_MSG)
#define LOG_MSG(...)   LOG_PRINT("MSG", __VA_ARGS__)
#else
#define LOG_MSG(...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define LOG_ERROR(...) LOG_PRINT("ERROR", __VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_WARN)
#define LOG_WARN(...) LOG_PRINT("WARN", __VA_ARGS__)
#else
#define LOG_WARN(...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
#define LOG_INFO(...) LOG_PRINT("INFO", __VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#endif
