#include "peripheral_qdec.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
static void QDEC_RegClr(QDEC_channelId ch, QDEC_qdecReg reg, uint8_t s, uint32_t b)
{
    QDEC_REG_CLR(ch, reg, b, s);
}
void QDEC_RegWrBit(QDEC_channelId ch, QDEC_qdecReg reg, uint32_t v, uint8_t s)
{
    QDEC_REG_WR(ch, reg, v, s);
}
void QDEC_RegWrBits(QDEC_channelId ch, QDEC_qdecReg reg, uint32_t v, uint8_t s, uint8_t b)
{
    QDEC_REG_CLR(ch, reg, b, s);
    QDEC_REG_WR(ch, reg, v, s);
}
static uint32_t QDEC_RegRd(QDEC_channelId ch, QDEC_qdecReg reg, uint8_t s, uint8_t b)
{
    return QDEC_REG_RD(ch, reg, b, s);
}

void QDEC_EnableQdecDiv(QDEC_indexCfg div, uint8_t enable)
{
    APB_SYSCTRL->QdecCfg &= ~(1 << 20);
    if (enable) {
        APB_SYSCTRL->QdecCfg &= ~(QDEC_MK_MASK(3) << 16);
        APB_SYSCTRL->QdecCfg |= div << 16;
        APB_SYSCTRL->QdecCfg |= enable << 20;
    }
}

void QDEC_TcCfg(QDEC_channelId ch, uint16_t tc_ra, uint16_t tc_rb, uint16_t tc_rc)
{
    if (ch > QDEC_CH_2) 
        return;
    QDEC_RegWrBits(ch, QDEC_CH_WRITE_A, tc_ra, 0, 16);
    QDEC_RegWrBits(ch, QDEC_CH_WRITE_B, tc_rb, 0, 16);
    QDEC_RegWrBits(ch, QDEC_CH_WRITE_C, tc_rc, 0, 16);
}

void QDEC_QdecCfg(QDEC_channelId ch, uint16_t tc_ra, uint16_t tc_rb, uint16_t tc_rc, 
    uint8_t fliter, uint8_t miss)
{
    QDEC_RegWrBit(QDEC_CH_0, QDEC_BMR, 1, 16);
    QDEC_RegWrBits(ch, QDEC_CH_MODE, 1, 16, 2);
    QDEC_RegWrBits(ch, QDEC_CH_MODE, 1, 18, 2);
    QDEC_RegWrBits(ch, QDEC_CH_MODE, 2, 20, 2);
    QDEC_TcCfg(ch, tc_ra, tc_rb, tc_rc);
    QDEC_RegWrBit(QDEC_CH_0, QDEC_BMR, 1, 9);
    QDEC_RegWrBit(QDEC_CH_0, QDEC_BMR, 1, 8);
    QDEC_RegWrBit(QDEC_CH_0, QDEC_BMR, 1, 18);
    QDEC_RegWrBits(QDEC_CH_0, QDEC_BMR, fliter, 20, 6);
//    QDEC_RegWrBits(QDEC_CH_0, QDEC_BMR, miss, 26, 4);
    
    QDEC_RegWrBits(ch, QDEC_CH_INT_EN, 0x20, 0, 10);
}

uint16_t QDEC_ReadData(QDEC_channelId ch)
{
    return QDEC_RegRd(ch, QDEC_CH_WRITE_A, 0, 16);
}

void QDEC_ChannelEnable(QDEC_channelId ch, uint8_t enable)
{
    if (enable) 
        QDEC_RegWrBits(ch, QDEC_CH_CTRL, 5, 0, 3);
    else
        QDEC_RegClr(ch, QDEC_CH_CTRL, 0, 3);
}

#endif

