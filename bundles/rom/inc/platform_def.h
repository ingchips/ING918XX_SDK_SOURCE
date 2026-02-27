#ifndef _platform_def_h
#define _platform_def_h
#include "ingsoc.h"

#define PLATFORM_BUNDLE_VARIANT     PLATFORM_BUNDLE_VARIANT_ROM

#define PLATFORM_IN_ROM             1

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

    #define   SYS_MEMORY_SIZE_KiB   48
    #define SHARE_MEMORY_SIZE_KiB    8

#else
    #error unknown or unsupported chip family
#endif

#endif
