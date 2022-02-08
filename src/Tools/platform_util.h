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

/**
 ****************************************************************************************
 * @brief Inspect platform binary
 *
 * @param[in]   binary_addr         address of the binary
 * @param[in]   family              family of the binary
 * @return                          information of the binary
 ****************************************************************************************
 */
const struct platform_info *platform_inspect2(uintptr_t binary_addr, int family);

/**
 ****************************************************************************************
 * @brief Inspect platform binary
 *
 * Note: It is assumed the platform binary is for the same family as the building target.
 * 
 * @param[in]   binary_addr         address of the binary
 * @return                          information of the binary
 ****************************************************************************************
 */
const struct platform_info *platform_inspect(uintptr_t binary_addr);


#ifdef __cplusplus
}
#endif

#endif
