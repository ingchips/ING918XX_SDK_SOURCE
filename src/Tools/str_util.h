#ifndef _str_util_h
#define _str_util_h

#include <stdint.h>

#ifdef _cplusplus
extern "C" {
#endif

typedef void (*f_print_string)(const char *str);

/**
 ****************************************************************************************
 * @brief Print hex table
 *
 * @param   value           input bytes
 * @param   len             input length
 * @param   print           customer string printing function
 ****************************************************************************************
 */
void print_hex_table(const uint8_t *value, int len, f_print_string print);

/**
 ****************************************************************************************
 * @brief Base64 encoding
 *
 * @param[in]   data            input bytes
 * @param[in]   data_len        input length
 * @param[out]  res             string buffer to contain result
 * @param[in]   buffer_size     size of string buffer (if not large enough, res = "")
 * @return                      string buffer
 ****************************************************************************************
 */
char *base64_encode(const uint8_t *data, int data_len,
                    char *res, int buffer_size);

/**
 ****************************************************************************************
 * @brief Format RTC value into HH:MM:SS.milli_sec,micro_sec
 *
 * @param[in]   str         buffer to hode the string
 * @param[in]   rtc_value   RTC value (lower 32bit)
 * @return                  str
 ****************************************************************************************
 */
const char *fmt_rtc_timestamp(char *str, uint32_t rtc_value);

/**
 ****************************************************************************************
 * @brief Format RTC value into HH:MM:SS.milli_sec,micro_sec
 *
 * @param[in]   str         buffer to hode the string
 * @param[in]   rtc_value   RTC value  (full 48bit) 
 * @return                  str
 ****************************************************************************************
 */
const char *fmt_rtc_timestamp_full(char *str, uint64_t rtc_value);

/**
 ****************************************************************************************
 * @brief Format time stamp in micro second into HH:MM:SS.milli_sec,micro_sec
 *
 * @param[in]   str         buffer to hode the string
 * @param[in]   us_time     time stamp in micro second (us)
 * @return                  str
 ****************************************************************************************
 */
const char *fmt_us_timestamp(char *str, uint64_t us_time);

#ifdef _cplusplus
}
#endif

#endif

