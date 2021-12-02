#ifndef _PLATFORM_UTIL_H_
#define _PLATFORM_UTIL_H_

#include "platform_api.h"

#ifdef __cplusplus
extern "C" {
#endif

struct platform_info
{
    platform_ver_t version;
    uintptr_t app_load_addr;    
};

const struct platform_info *platform_inspect(uintptr_t binary_addr);

#ifdef __cplusplus
}
#endif

#endif
