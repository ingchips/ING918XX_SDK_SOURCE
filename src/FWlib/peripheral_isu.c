#include "peripheral_isu.h"
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
static int ISU_ValidRegister(ISU_Register reg)
{
    switch (reg) {
        case ISU_CLOCK_CTRL:
        case ISU_TRNG_CTRL:
        case ISU_ENTROPY_SRC:
        case ISU_ENTROPY_SAMPLE:
        case ISU_POST_PROCESS:
        case ISU_EVALUATION:
        case ISU_TRNG_STAT:
        case ISU_TRNG_INTR:
        case ISU_TRNG_INTR_MASK:
        case ISU_RN_POOL_FIFO:
        case ISU_RN_POOL_STAT:
        case ISU_ACA_STATUS:
        case ISU_SRAM_WADDR:
        case ISU_SRAM_WDATA:
        case ISU_SRAM_RADDR:
        case ISU_SRAM_RDATA:
            return 1;
        default:
            return 0;
    }
}
int ISU_ReadRegister(ISU_Register reg, uint32_t *value)
{
    if (!value || !ISU_ValidRegister(reg)) return -1;
    *value = io_read(APB1_ISU_BASE + (uint32_t)reg);
    return 0;
}
int ISU_WriteRegister(ISU_Register reg, uint32_t value)
{
    if (!ISU_ValidRegister(reg)) return -1;
    io_write(APB1_ISU_BASE + (uint32_t)reg, value);
    return 0;
}
#endif
