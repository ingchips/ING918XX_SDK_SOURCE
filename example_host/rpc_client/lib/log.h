#define LOG_LEVEL_DBG           1
#define LOG_LEVEL_INFO          2
#define LOG_LEVEL_WARN          3
#define LOG_LEVEL_ERROR         4

#ifndef LOG_LEVEL
#define LOG_LEVEL               LOG_LEVEL_INFO
#endif

#define FAIL_COLOR          "\033[31;1m"
#define PASS_COLOR          "\033[32;1m"
#define WARN_COLOR          "\033[33;1m"
#define COLOR_END           "\033[0m"

#ifdef __cplusplus
extern "C" {
#endif

void print_timestamp(void);

#ifdef __cplusplus
}
#endif

#define PRINT(...) do { print_timestamp(); printf(__VA_ARGS__); printf("\n"); } while (0)

#if (LOG_LEVEL <= LOG_LEVEL_DBG)
#define LOG_D(...)       PRINT("[DBG ] " __VA_ARGS__)
#else
#define LOG_D(...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
#define LOG_I(...)       PRINT("[INFO] " __VA_ARGS__)
#else
#define LOG_I(...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_WARN)
#define LOG_W(...)       PRINT("[" WARN_COLOR "WARN" COLOR_END "] " __VA_ARGS__)
#else
#define LOG_W(...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define LOG_E(...)       PRINT("[" FAIL_COLOR "ERR " COLOR_END "] " __VA_ARGS__)
#else
#define LOG_E(...)
#endif


#define LOG_PROG(...)      PRINT("[....] " __VA_ARGS__)
#define LOG_PASS(...)      PRINT("[" PASS_COLOR "PASS" COLOR_END "] " __VA_ARGS__)
#define LOG_FAIL(...)      PRINT("[" FAIL_COLOR "FAIL" COLOR_END "] " __VA_ARGS__)
#define LOG_OK(...)        PRINT("[" PASS_COLOR " OK " COLOR_END "] " __VA_ARGS__)

#define LOG_PLAIN(...)     PRINT(__VA_ARGS__)