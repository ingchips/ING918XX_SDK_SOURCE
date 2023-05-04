#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    #if   defined ( __CC_ARM )
      #include "./portable/RVDS/ARM_CM3/portmacro.h"

    #elif defined ( __ICCARM__ )
      #include "./portable/IAR/ARM_CM3/portmacro.h"

    #else
      #include "./portable/GCC/ARM_CM3/portmacro.h"
    #endif
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #if   defined ( __CC_ARM )
      #include "./portable/RVDS/ARM_CM4F/portmacro.h"

    #elif defined ( __ICCARM__ )
      #include "./portable/IAR/ARM_CM4F/portmacro.h"

    #else
      #include "./portable/GCC/ARM_CM4F/portmacro.h"
    #endif
#endif

