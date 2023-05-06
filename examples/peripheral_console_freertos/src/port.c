#include "ingsoc.h"

#if (__ARMCC_VERSION >= 6000000)
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    #include "../FreeRTOS/portable/GCC/ARM_CM3/port.c"
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #include "../FreeRTOS/portable/GCC/ARM_CM4F/port.c" 
#else
    #error unknown or unsupported chip family
#endif
#else
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    #include "../FreeRTOS/portable/RVDS/ARM_CM3/port.c"
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #include "../FreeRTOS/portable/RVDS/ARM_CM4F/port.c"
#else
    #error unknown or unsupported chip family
#endif
#endif

