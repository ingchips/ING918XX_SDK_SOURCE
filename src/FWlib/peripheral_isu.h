#ifndef PERIPHERAL_ISU_H
#define PERIPHERAL_ISU_H
#include "ingsoc.h"
#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
/* Register-level access only: no undocumented crypto/TRNG control bits inferred. */
typedef enum {
    ISU_CLOCK_CTRL=0, ISU_TRNG_CTRL=0x200, ISU_ENTROPY_SRC=0x204,
    ISU_ENTROPY_SAMPLE=0x208, ISU_POST_PROCESS=0x20c, ISU_EVALUATION=0x210,
    ISU_TRNG_STAT=0x214, ISU_TRNG_INTR=0x218, ISU_TRNG_INTR_MASK=0x21c,
    ISU_RN_POOL_FIFO=0x280, ISU_RN_POOL_STAT=0x284,
    ISU_ACA_STATUS=0x312c, ISU_SRAM_WADDR=0x3130, ISU_SRAM_WDATA=0x3134,
    ISU_SRAM_RADDR=0x3138, ISU_SRAM_RDATA=0x313c
} ISU_Register;
int ISU_ReadRegister(ISU_Register reg, uint32_t *value);
int ISU_WriteRegister(ISU_Register reg, uint32_t value);
#endif /* INGCHIPS_FAMILY_26 */

#ifdef __cplusplus
}
#endif

#endif
