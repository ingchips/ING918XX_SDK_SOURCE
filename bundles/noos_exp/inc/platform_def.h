#ifndef _platform_def_h
#define _platform_def_h
#include "ingsoc.h"

#define PLATFORM_BUNDLE_VARIANT     PLATFORM_BUNDLE_VARIANT_NOOS_EXP

#if   (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

    #define SHARE_MEMORY_SIZE_KiB   64

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

    #define   SYS_MEMORY_SIZE_KiB   32
    #define SHARE_MEMORY_SIZE_KiB   32

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

    #define   SYS_MEMORY_SIZE_KiB   40
    #define SHARE_MEMORY_SIZE_KiB   16

#else
    #error unknown or unsupported chip family
#endif

#endif
