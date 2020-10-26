#include <stdint.h>

char *base64_encode(const uint8_t *data, int data_len,
                    char *res, int buffer_size)
{
    int len = (data_len + 2) / 3 * 4;
    int i, j;

    const char *base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if (len > buffer_size - 1)
    {
        res[0] = '\0';
        return res;
    }

    res[len]='\0';

    for(i = 0, j = 0; i < len - 2; j += 3, i += 4)
    {
        res[i + 0] = base64_table[data[j] >> 2];
        res[i + 1] = base64_table[(data[j] & 0x3) << 4 | (data[j + 1] >> 4)];
        res[i + 2] = base64_table[(data[j + 1] & 0xf) << 2 | (data[j + 2] >> 6)];
        res[i + 3] = base64_table[data[j + 2] & 0x3f];
    }

    switch (data_len % 3)
    {
        case 1:
            res[i - 2] = '=';
            res[i - 1] = '=';
            break;
        case 2:
            res[i - 1] = '=';
            break;
    }

    return res;
}
