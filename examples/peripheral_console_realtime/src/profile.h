#ifndef _PROFILESTASK_H_
#define _PROFILESTASK_H_

#include <stdint.h>

uint32_t setup_profile(void *data, void *user_data);

#if (INGCHIPS_FAMILY != INGCHIPS_FAMILY_918)
    #ifdef USE_POWER_LIB
        #error USE_POWER_LIB is only allowed on ING918
    #endif
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #ifdef OPT_RAM_CODE
        #define ADDITIONAL_ATTRIBUTE    __attribute__((section(".ram_code")))
    #else
        #define ADDITIONAL_ATTRIBUTE
    #endif
#endif

#endif


