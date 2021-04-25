#ifndef _str_util_h
#define _str_util_h

#include <stdint.h>

#ifdef _cplusplus
extern "C" {
#endif

typedef void (*f_print_string)(const char *str);

void print_hex_table(const uint8_t *value, int len, f_print_string print);

char *base64_encode(const uint8_t *data, int data_len,
                    char *res, int buffer_size);

#ifdef _cplusplus
}
#endif

#endif

