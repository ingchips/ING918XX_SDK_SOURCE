#ifndef __INGSOC_H
#define __INGSOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define INGCHIPS_FAMILY_918             0
#define INGCHIPS_FAMILY_916             1

// register access
#define io_write_b(a,d)  (*(volatile uint8_t*)(a)=(d))
#define io_read_b(a)     (*(volatile uint8_t*)(a))
#define io_write(a,d)    (*(volatile uint32_t*)(a)=(d))
#define io_read(a)       (*(volatile uint32_t*)(a))

// Bits Width change to Mask Bits
#define BW2M(a)          ((1 << (a)) -1)

#ifndef __NOP
#define __NOP()             __asm("nop")
#endif

#ifdef __cplusplus
}
#endif

#endif
