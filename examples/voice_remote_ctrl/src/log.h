#ifndef _LOG_H_
#define _LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifndef PRINTF
#define PRINTF platform_printf
#endif

// Defining log levels
#define LOG_LEVEL_DEBUG     0
#define LOG_LEVEL_INFO      1
#define LOG_LEVEL_WARNING   2
#define LOG_LEVEL_ERROR     3


// 定义日志输出宏
#define LOG_PRINTF(level, ...) do { \
    PRINTF("[%s] ", level == LOG_LEVEL_DEBUG ? "DEBUG" : \
                        level == LOG_LEVEL_INFO ? "INFO" : \
                                    level == LOG_LEVEL_WARNING ? "WARNING" : "ERROR");\
    PRINTF(__VA_ARGS__); \
    PRINTF("\n"); \
} while (0)

//6 Space character
#define LOG_PRINTF_TAB(level, ...) do { \
    char blank[] = "     ";\
    PRINTF("%s",blank);\
    LOG_PRINTF(level,__VA_ARGS__);\
} while (0)

//12 Space character
#define LOG_PRINTF_TAB2(level, ...) do { \
    char blank[] = "          ";\
    PRINTF("%s",blank);\
    LOG_PRINTF(level,__VA_ARGS__);\
} while (0)

//18 Space character
#define LOG_PRINTF_TAB3(level, ...) do { \
    char blank[] = "               ";\
    PRINTF("%s",blank);\
    LOG_PRINTF(level,__VA_ARGS__);\
} while (0)


#ifdef __cplusplus
}
#endif

#endif /* _LOG_H_ */
