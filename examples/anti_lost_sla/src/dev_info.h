#ifndef _DEV_INFO_H
#define _DEV_INFO_H

#include <stdint.h>
#include "bluetooth.h"

typedef struct
{
    bd_addr_t slave;
    bd_addr_t master;
} dev_info_t;

extern const dev_info_t dev_info;

#endif
