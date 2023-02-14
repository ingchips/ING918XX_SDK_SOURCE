#include "peripheral_qdec.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#define QDEC_LEFT_SHIFT(v, s)        ((v) << (s))
#define QDEC_RIGHT_SHIFT(v, s)       ((v) >> (s))
#define QDEC_MK_MASK(b)              ((QDEC_LEFT_SHIFT(1, b)) - (1))
#define QDEC_REG_VAL(reg)            ((*((uint32_t *)((APB_QDEC_BASE) + (reg)))))
#define QDEC_REG_WR(reg, v, s)       ((QDEC_REG_VAL(reg)) |= (QDEC_LEFT_SHIFT(v, s)))
#define QDEC_REG_RD(reg, b, s)       ((QDEC_RIGHT_SHIFT((QDEC_REG_VAL(reg)), s)) & QDEC_MK_MASK(b))
#define QDEC_REG_CLR(reg, b, s)      ((QDEC_REG_VAL(reg)) &= (~(QDEC_LEFT_SHIFT(QDEC_MK_MASK(b), s))))

static void QDEC_RegClr(QDEC_qdecReg reg, uint8_t s, uint32_t b)
{
    QDEC_REG_CLR(reg, b, s);
}

void QDEC_RegWrBit(QDEC_qdecReg reg, uint32_t v, uint8_t s)
{
    QDEC_REG_WR(reg, v, s);
}

void QDEC_RegWrBits(QDEC_qdecReg reg, uint32_t v, uint8_t s, uint8_t b)
{
    QDEC_REG_CLR(reg, b, s);
    QDEC_REG_WR(reg, v, s);
}

static uint32_t QDEC_RegRd(QDEC_qdecReg reg, uint8_t s, uint8_t b)
{
    return QDEC_REG_RD(reg, b, s);
}

void QDEC_EnableQdecDiv(QDEC_indexCfg div)
{
    APB_SYSCTRL->QdecCfg |= 1 << 19;
    APB_SYSCTRL->QdecCfg &= ~(QDEC_MK_MASK(3) << 16);
    APB_SYSCTRL->QdecCfg |= div << 16;
    APB_SYSCTRL->QdecCfg |= 1 << 20;
}

void QDEC_QdecCfg(uint8_t fliter, uint8_t miss)
{
    QDEC_Reset();
    QDEC_RegWrBit(QDEC_BMR, 1, 16);
    QDEC_RegWrBits(QDEC_CH_MODE, 1, 16, 2);
    QDEC_RegWrBits(QDEC_CH_MODE, 1, 18, 2);
    QDEC_RegWrBits(QDEC_CH_MODE, 2, 20, 2);

    QDEC_RegWrBits(QDEC_CH_WRITE_A, 0xf , 0, 16);
    QDEC_RegWrBits(QDEC_CH_WRITE_B, 0x12, 0, 16);
    QDEC_RegWrBits(QDEC_CH_WRITE_C, 0x22, 0, 16);
    
    QDEC_RegWrBit(QDEC_BMR, 1, 9);
    QDEC_RegWrBit(QDEC_BMR, 1, 8);
    QDEC_RegWrBit(QDEC_BMR, 1, 18);
    QDEC_RegWrBits(QDEC_BMR, fliter, 20, 6);
    QDEC_RegWrBit(QDEC_BMR, 1, 19);
    QDEC_RegWrBits(QDEC_BMR, miss, 26, 4);
    
    QDEC_RegWrBits(QDEC_CH_INT_EN, 0x20, 0, 10);
}

uint16_t QDEC_GetData(void)
{
    return QDEC_RegRd(QDEC_CH_WRITE_A, 0, 16);
}

uint8_t QDEC_GetDirection(void)
{
    return QDEC_RegRd(QDEC_STATUS_SEL, 8, 1);
}

void QDEC_Reset(void)
{
    QDEC_RegWrBit(QDEC_BCR, 1, 8);
}

void QDEC_ChannelEnable(uint8_t enable)
{
    if (enable) 
        QDEC_RegWrBits(QDEC_CH_CTRL, 5, 0, 3);
    else
        QDEC_RegClr(QDEC_CH_CTRL, 0, 3);
}

void QDEC_IntClear(void)
{
    QDEC_RegRd(QDEC_CH_TIOB0_RD, 0, 10);
}

#endif

