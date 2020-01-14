#include "service_ibeacon.h"
#include "platform_api.h"

#include <math.h>

void print_uuid(uint8_t *uuid)
{
    platform_printf("{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}", 
                    uuid[0], uuid[1], uuid[2], uuid[3],
                    uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9],
                    uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
}

double estimate_distance(int8_t ref_power, int8_t rssi)
{
    return pow(10, (ref_power - rssi) / 20.0);
}

