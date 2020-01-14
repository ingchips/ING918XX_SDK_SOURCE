#include <stdint.h>
#include <stdio.h>
#include <string.h>

const char *float_ieee_11073_val_to_repr(uint32_t val)
{
    static char r[50];
#define FLOAT_VALUE_INFINITY_PLUS           0x007FFFFEu
#define FLOAT_VALUE_NAN                     0x007FFFFFu
#define FLOAT_VALUE_NRES                    0x00800000u
#define FLOAT_VALUE_RFU                     0x00800001u
#define FLOAT_VALUE_INFINITY_MINUS          0x00800002u

    if (val >= 0x007FFFFE && val <= 0x00800002)
    {
        switch (val)
        {
            case FLOAT_VALUE_INFINITY_PLUS:
                return "+INFINITY";
            case FLOAT_VALUE_NAN:
                return "NaN";
            case FLOAT_VALUE_NRES:
                return "NRes";
            case FLOAT_VALUE_RFU:
                return "RFU";
            case FLOAT_VALUE_INFINITY_MINUS:
                return "-INFINITY";
            default:
                return "BAD ieee_11073_val";
        }
    }

    int exponent = (int8_t)(val >> 24);
    int mantissa = (int)(val & 0x7FFFFF);

    if ((val & 0x00800000) != 0)
        mantissa -= 0x00800000;

    if (mantissa == 0) return "0";

    sprintf(r, "%d", mantissa);
    if (exponent > 0)
    {
        int i = strlen(r);
        while (exponent > 0)
        {
            r[i] = '0';
            i++;
            exponent--;
        }
        r[i] = '\0';
    }
    else if (exponent < 0)
    {
        int len = strlen(r);
        exponent = -exponent;
        if (exponent < len)
        {
            int dp = len - exponent;
            memmove(r + dp + 1, r + dp, exponent + 1);
            r[dp] = '.';
        }
        else
        {
            int z = exponent - len;
            memmove(r, r + 2 + exponent, len + 1);
            r[0] = '0'; r[1] = '.';
            while (z > 0)
            {
                r[1 + z] = '0';
                z--;
            }
        }
    }

    return r;
}
