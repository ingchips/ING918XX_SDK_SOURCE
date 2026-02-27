#ifndef __INGSOC_H
#define __INGSOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define INGCHIPS_FAMILY_918             0
#define INGCHIPS_FAMILY_916             1
#define INGCHIPS_FAMILY_20              2

#define PLATFORM_BUNDLE_VARIANT_TYPICAL                 0
#define PLATFORM_BUNDLE_VARIANT_NOOS_TYPICAL            1
#define PLATFORM_BUNDLE_VARIANT_EXP                     2
#define PLATFORM_BUNDLE_VARIANT_NOOS_EXP                3
#define PLATFORM_BUNDLE_VARIANT_EXTENSION               4
#define PLATFORM_BUNDLE_VARIANT_NOOS_EXTENSION          5
#define PLATFORM_BUNDLE_VARIANT_MASS_CONN               6
#define PLATFORM_BUNDLE_VARIANT_NOOS_MASS_CONN          7
#define PLATFORM_BUNDLE_VARIANT_MINI                    8
#define PLATFORM_BUNDLE_VARIANT_NOOS_MINI               9
#define PLATFORM_BUNDLE_VARIANT_ROM                     10

// register access
#define io_write_b(a,d)  (*(volatile uint8_t*)(a)=(d))
#define io_read_b(a)     (*(volatile uint8_t*)(a))
#define io_write(a,d)    (*(volatile uint32_t*)(a)=(d))
#define io_read(a)       (*(volatile uint32_t*)(a))

// Bits Width change to Mask Bits
#define BW2M(a)          ((1u << (a)) -1)

#ifndef __NOP
#define __NOP()             __asm("nop")
#endif

#ifdef __cplusplus
}
#endif

#endif
