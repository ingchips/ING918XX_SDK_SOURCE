#include "peripheral_qdec.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916 || INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)
#define QDEC_LEFT_SHIFT(v, s)        ((v) << (s))
#define QDEC_RIGHT_SHIFT(v, s)       ((v) >> (s))
#define QDEC_MK_MASK(b)              ((QDEC_LEFT_SHIFT(1, b)) - (1))
#define QDEC_REG_VAL(reg)            ((*((volatile uint32_t *)((APB_QDEC_BASE) + (reg)))))
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

static void QDEC_SetRegBits(volatile uint32_t *reg, uint32_t v, uint8_t bit_width, uint8_t bit_offset)
{
    uint32_t mask = ((1 << bit_width) - 1) << bit_offset;
    *reg = (*reg & ~mask) | (v << bit_offset);
}

static void QDEC_SetRegBit(volatile uint32_t *reg, uint8_t v, uint8_t bit_offset)
{
    uint32_t mask = 1 << bit_offset;
    *reg = (*reg & ~mask) | (v << bit_offset);
}


void QDEC_ChModeCfg(QDEC_CHX Channel, QDEC_ModuCfg ModeCfg)
{
    switch (ModeCfg)

    {
    case QDEC_TIMER:
//        *(uint32_t*)0x040009004UL |= (1<<15); 
        QDEC_SetRegBit(&APB_QDEC->channels[Channel].channel_mode, 1, 15);//Wave
        QDEC_SetRegBit(&APB_QDEC->bmr, 0, 8);//Qdec        break;
    case QDEC_PWM:
        QDEC_SetRegBit(&APB_QDEC->channels[Channel].channel_mode, 1, 15);//Wave
        QDEC_SetRegBit(&APB_QDEC->bmr, 0, 8);//QdecEn
        break;
    case QDEC_PCM:
        QDEC_SetRegBit(&APB_QDEC->channels[Channel].channel_mode, 0, 15);//Wave
        QDEC_SetRegBit(&APB_QDEC->bmr, 0, 8);//QdecEn
        break;
    case QDEC_QDEC:
        QDEC_SetRegBit(&APB_QDEC->channels[Channel].channel_mode, 0, 15);//Wave
        QDEC_SetRegBit(&APB_QDEC->bmr, 1, 8);//QdecEn
        break;
    default:
        break;
    }
}

void QDEC_TmrCfg(QDEC_CHX Channel, QDEC_TMR_UP_MODE TmrUpMode,QDEC_TMR_RELOAD_MODE TmrReloadMode)
{
    if(TmrUpMode == QDEC_TMR_STOP)
        QDEC_SetChxCpcStopEn(Channel, 1);
    else
        QDEC_SetChxCpcStopEn(Channel, 0);
    if(TmrReloadMode == QDEC_TMR_RELOAD_UP_VALE)
        QDEC_SetChxCpcTrg(Channel, 1);
    else
        QDEC_SetChxCpcTrg(Channel, 0);
}

void QDEC_SetChxCpcTrg(QDEC_CHX Channel, uint8_t val)
{
    QDEC_SetRegBit(&APB_QDEC->channels[Channel].channel_mode, val, 14);
}

void QDEC_SetChxCpcStopEn(QDEC_CHX Channel, uint8_t val)
{
    QDEC_SetRegBit(&APB_QDEC->channels[Channel].channel_mode, val, 6);
}

void QDEC_ExternalEventEdgeSet(QDEC_CHX Channel, QDEC_ExTrigger edge)
{
    QDEC_SetRegBits(&APB_QDEC->channels[Channel].channel_mode, (uint8_t)edge, 2, 8);
}

void QDEC_SetEtrg(QDEC_CHX Channel, uint8_t val)
{
    QDEC_SetRegBits(&APB_QDEC->channels[Channel].channel_mode, val, 2, 10);
}

void QDEC_SetEtrgEn(QDEC_CHX Channel, uint8_t enable)
{
    QDEC_SetRegBit(&APB_QDEC->channels[Channel].channel_mode, enable, 12);
}

void QDEC_SetCHxTmrCntA(QDEC_CHX Channel, uint16_t val)
{
    APB_QDEC->channels[Channel].channel_write_a = val;
}

void QDEC_SetCHxTmrCntB(QDEC_CHX Channel, uint16_t val)
{
    APB_QDEC->channels[Channel].channel_write_b = val;
}

void QDEC_SetCHxTmrCntC(QDEC_CHX Channel, uint16_t val)
{
    APB_QDEC->channels[Channel].channel_write_c = val;
}

void QDEC_SetChxIntEn(QDEC_CHX Channel, uint8_t enable, uint16_t items)
{
    if(enable)
        APB_QDEC->channels[Channel].channel_int_en = items;
    else
        APB_QDEC->channels[Channel].channel_int_dis = items;
}

uint32_t QDEC_GetChxIntEn(QDEC_CHX Channel)
{
    return APB_QDEC->channels[Channel].channel_tiob0_rd;
}

uint32_t QDEC_GetChxIntStateClr(QDEC_CHX Channel)
{
    return APB_QDEC->channels[Channel].channel_int_rd;
}

void QDEC_EnableChannel(QDEC_CHX Channel, uint8_t enable)
{
    if(enable)
        QDEC_SetRegBits(&APB_QDEC->channels[Channel].channel_ctrl, 5, 3, 0);
    else
        QDEC_SetRegBits(&APB_QDEC->channels[Channel].channel_ctrl, 3, 2, 1);
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

