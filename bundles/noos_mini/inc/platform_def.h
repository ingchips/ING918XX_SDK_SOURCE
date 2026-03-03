#ifndef _platform_def_h
#define _platform_def_h
#include "ingsoc.h"

#define PLATFORM_BUNDLE_VARIANT     PLATFORM_BUNDLE_VARIANT_NOOS_MINI

#if   (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

    #define SHARE_MEMORY_SIZE_KiB    8

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

    #define   SYS_MEMORY_SIZE_KiB   56
    #define SHARE_MEMORY_SIZE_KiB    8

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

    #define   SYS_MEMORY_SIZE_KiB   48
    #define SHARE_MEMORY_SIZE_KiB    8

#else
    #error unknown or unsupported chip family
#endif

#endif
