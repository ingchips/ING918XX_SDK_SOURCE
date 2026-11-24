#include "ingsoc.h"

/* 原来只判断 __ARMCC_VERSION >= 6000000，导致 GCC/clang 走 else 分支
 * 拿到 RVDS（ARMCC 专用）移植层而编译失败。改为"ARMCC 5 及更早才用 RVDS"，
 * 其余（ARMCC 6+、GCC、clang）都用 GCC 移植层。 */
#if !defined(__ARMCC_VERSION) || (__ARMCC_VERSION >= 6000000)
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    #include "../FreeRTOS/portable/GCC/ARM_CM3/port.c"
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #include "../FreeRTOS/portable/GCC/ARM_CM4F/port.c"
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
    #include "../FreeRTOS/portable/GCC/ARM_CM3/port.c"
#else
    #error unknown or unsupported chip family
#endif
#else
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    #include "../FreeRTOS/portable/RVDS/ARM_CM3/port.c"
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #include "../FreeRTOS/portable/RVDS/ARM_CM4F/port.c"
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
    #include "../FreeRTOS/portable/RVDS/ARM_CM3/port.c"
#else
    #error unknown or unsupported chip family
#endif
#endif

