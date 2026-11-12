#include "peripheral_etimer.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)

/* Do not acknowledge W1C flags or retrigger read/reload handshakes in an RMW. */
static void ETMR_Update(ETMR_TypeDef *base, uint32_t mask, uint32_t value)
{
    uint32_t ctrl = base->CTL & ~0x8c2u;
    base->CTL = (ctrl & ~mask) | (value & mask);
}

uint32_t ETMR_GetCNT(ETMR_TypeDef* pTMR)
{
    return pTMR->CNT;
}

void ETMR_ReadLock(ETMR_TypeDef* pTMR)
{
    ETMR_Update(pTMR, 1u << bsETMR_CTL_READ_LOCK, 1u << bsETMR_CTL_READ_LOCK);
}

uint32_t ETMR_GetReadReady(ETMR_TypeDef* pTMR)
{
    return (pTMR->CTL & (1 << bsETMR_CTL_READ_LOCK));
}

void ETMR_SetReload(ETMR_TypeDef* pTMR, uint8_t enable)
{
    if (enable) ETMR_Update(pTMR, 1u << bsETMR_CTL_RELOAD, 1u << bsETMR_CTL_RELOAD);
    else ETMR_Update(pTMR, 1u << bsETMR_CTL_RELOAD, 0);
}

//-----------
// TMR_CMP
//
void ETMR_SetValue(ETMR_TypeDef* pTMR, uint32_t value)
{
    pTMR->RELOAD = value;
}

void ETMR_Cmd(ETMR_TypeDef* pTMR, uint8_t enable)
{
    if (enable) ETMR_Update(pTMR, 1u << bsETMR_CTL_TMR_EN, 1u << bsETMR_CTL_TMR_EN);
    else ETMR_Update(pTMR, 1u << bsETMR_CTL_TMR_EN, 0);
}

void ETMR_SetMode(ETMR_TypeDef* pTMR, uint8_t mode)
{
    if (mode > 1) return;
    ETMR_Update(pTMR, 3u << bsETMR_CTL_MODE, (uint32_t)mode << bsETMR_CTL_MODE);
}

void ETMR_IntCmd(ETMR_TypeDef* pTMR, uint8_t IntIT, uint8_t enable)
{
    if (enable) ETMR_Update(pTMR, IntIT & 0x30u, IntIT & 0x30u);
    else ETMR_Update(pTMR, IntIT & 0x30u, 0);
}

void ETMR_IntClr(ETMR_TypeDef* pTMR, uint8_t IntIT)
{
    ETMR_Update(pTMR, 0xc0u, IntIT & 0xc0u);
}

uint8_t ETMR_GetStatus(ETMR_TypeDef* pTMR, uint8_t IntIT)
{
    return  (pTMR->CTL & IntIT);
}

void ETMR_HaltCmd(ETMR_TypeDef* pTMR, uint8_t enable)
{
    if (enable) ETMR_Update(pTMR, 1u << bsETMR_CTL_HALT_EN, 1u << bsETMR_CTL_HALT_EN);
    else ETMR_Update(pTMR, 1u << bsETMR_CTL_HALT_EN, 0);
}

void ETMR_LockCmd(ETMR_TypeDef* pTMR, uint8_t enable)
{
    if (enable) ETMR_Update(pTMR, 1u << bsETMR_CTL_LOCK, 1u << bsETMR_CTL_LOCK);
    else ETMR_Update(pTMR, 1u << bsETMR_CTL_LOCK, 0);
}

uint8_t ETMR_GetHit(ETMR_TypeDef* pTMR)
{
    return (pTMR->CTL >> bsETMR_CTL_HIT_STATUS) & 1u;
}

#endif
