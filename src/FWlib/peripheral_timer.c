#include "peripheral_timer.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

// TMR_CTL
#define bsTMR_CTL_TMR_EN                    0            // timer enable
#define bwTMR_CTL_TMR_EN                    1
#define bsTMR_CTL_RELOAD                    1            // when set, timer counter clear zero.
#define bwTMR_CTL_RELOAD                    1
#define bsTMR_CTL_OP_MODE                   2
#define bwTMR_CTL_OP_MODE                   1

#define bsTMR_CTL_INT_EN                    4            // int enable
#define bwTMR_CTL_INT_EN                    1
#define bsTMR_CTL_WatchDog_EN               5            // watchdog enable, timer 0 only
#define bwTMR_CTL_WatchDog_EN               1
#define bsTMR_CTL_INT_STATUS                6            // read as int status, write as int clear
#define bwTMR_CTL_INT_STATUS                1
#define bsTMR_CTL_WatchDog_STATUS           7            // watchdog status, timer 0 only
#define bwTMR_CTL_WatchDog_STATUS           1

#define bsTMR_CTL_HALTENA                   15           // hardware do not support for all three timer 0, 1, 2.
#define bwTMR_CTL_HALTENA                   1

uint32_t TMR_GetCNT(TMR_TypeDef *pTMR)
{
	return pTMR->CNT;
}

void TMR_Reload(TMR_TypeDef *pTMR)
{
    pTMR->CTL |= 1 << bsTMR_CTL_RELOAD;
}

//-----------
// TMR_CMP
//
void TMR_SetCMP(TMR_TypeDef *pTMR, uint32_t value)
{
	pTMR->CMP = value;
}

uint32_t TMR_GetCMP(TMR_TypeDef *pTMR)
{
	return pTMR->CMP;
}

//
void TMR_Enable(TMR_TypeDef *pTMR)
{
	pTMR->CTL |= 1 << bsTMR_CTL_TMR_EN;
}

void TMR_Disable(TMR_TypeDef *pTMR)
{
	pTMR->CTL &= ~(1 << bsTMR_CTL_TMR_EN);
}

//
void TMR_SetOpMode(TMR_TypeDef *pTMR, uint8_t mode)
{
    #define mask (2 << bsTMR_CTL_OP_MODE)
    pTMR->CTL = (pTMR->CTL & ~mask) | (mode << bsTMR_CTL_OP_MODE);
}

//
void TMR_IntEnable(TMR_TypeDef *pTMR)
{
	pTMR->CTL |= 1 << bsTMR_CTL_INT_EN;
}

void TMR_IntDisable(TMR_TypeDef *pTMR)
{
	pTMR->CTL &= ~(1 << bsTMR_CTL_INT_EN);
}

//
void TMR_WatchDogEnable(uint32_t timeout)
{
    APB_TMR0->CTL = (1 << bsTMR_CTL_RELOAD);
    APB_TMR0->CMP = timeout;
	APB_TMR0->CTL |= (1 << bsTMR_CTL_TMR_EN) | (1 << bsTMR_CTL_WatchDog_EN);
    TMR0_LOCK();
}

void TMR_WatchDogDisable(void)
{
    TMR0_UNLOCK();
	APB_TMR0->CTL &= ~(1 << bsTMR_CTL_WatchDog_EN);
}

//
uint8_t TMR_IntHappened(TMR_TypeDef *pTMR)
{
	return ( (pTMR->CTL >> bsTMR_CTL_INT_STATUS) & BW2M(bsTMR_CTL_INT_STATUS) );
}

void TMR_IntClr(TMR_TypeDef *pTMR)
{
	pTMR->CTL |= 1 << bsTMR_CTL_INT_STATUS;
}

//-----------
// TMR_LOCK
//
void TMR0_LOCK() // timer 0 only
{
	APB_TMR0->LOCK = 1;
}

void TMR0_UNLOCK() // timer 0 only
{
	APB_TMR0->LOCK = 0xDEADFACE;
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

void TMR_SetOpMode(TMR_TypeDef *pTMR, uint8_t ch_id, uint8_t op_mode, uint8_t clk_mode, uint8_t pwm_park_value)
{
    pTMR->Channels[ch_id].Ctrl = (op_mode & 0x7) | (clk_mode << 3) | (pwm_park_value << 4);
}

uint32_t TMR_GetClk(TMR_TypeDef *pTMR, uint8_t ch_id)
{
    SYSCTRL_Item item = SYSCTRL_ITEM_APB_TMR0;
    if (APB_TMR1 == pTMR) item = SYSCTRL_ITEM_APB_TMR1;
    else if (APB_TMR2 == pTMR) item = SYSCTRL_ITEM_APB_TMR2;
    else;
    return pTMR->Channels[ch_id].Ctrl & 8 ? SYSCTRL_GetPClk() : SYSCTRL_GetClk(item);
}

void TMR_SetReload(TMR_TypeDef *pTMR, uint8_t ch_id, uint32_t value)
{
    pTMR->Channels[ch_id].Reload = value;
}

void TMR_Enable(TMR_TypeDef *pTMR, uint8_t ch_id, uint8_t mask)
{
    pTMR->ChEn &= ~(0xf << (ch_id * 4));
    pTMR->ChEn |= (mask & 0xf) << (ch_id * 4);
}

void TMR_IntEnable(TMR_TypeDef *pTMR, uint8_t ch_id, uint8_t mask)
{
    pTMR->IntEn &= ~(0xf << (ch_id * 4));
    pTMR->IntEn |= (mask & 0xf) << (ch_id * 4);
}

uint32_t TMR_GetCNT(TMR_TypeDef *pTMR, uint8_t ch_id)
{
    return pTMR->Channels[ch_id].Counter;
}

uint32_t TMR_GetCMP(TMR_TypeDef *pTMR, uint8_t ch_id)
{
    return pTMR->Channels[ch_id].Reload;
}

void TMR_IntClr(TMR_TypeDef *pTMR, uint8_t ch_id, uint8_t mask)
{
    pTMR->IntStatus = (mask & 0xf) << (ch_id * 4);
}

uint8_t TMR_IntHappened(TMR_TypeDef *pTMR, uint8_t ch_id)
{
    return (pTMR->IntStatus >> (ch_id * 4)) & 0xf;
}

void TMR_PauseEnable(TMR_TypeDef *pTMR, uint8_t enable)
{
    uint8_t bit_offset = pTMR == APB_TMR0 ? 3 :
                            pTMR == APB_TMR1 ? 4 : 5;
    volatile uint32_t * reg = (volatile uint32_t *)(APB_SYSCTRL_BASE + 0x28);
    uint32_t mask = 1 << bit_offset;

    *reg = (*reg & ~mask) | (enable << bit_offset);
}

#define WDT_UNLOCK()    APB_WDT->WrEn = 0x5aa5

void TMR_WatchDogEnable3(wdt_inttime_interval_t int_timeout, wdt_rsttime_interval_t rst_timeout, uint8_t enable_int)
{
    WDT_UNLOCK();
    APB_WDT->Ctrl = (rst_timeout << 8) | (int_timeout << 4) | (enable_int ? 0xd : 0x9);

    #define AON1_BOOT   (volatile uint32_t *)(AON1_CTRL_BASE + 0x14)
    *AON1_BOOT |= 1u << 28;
}

void TMR_WatchDogDisable(void)
{
    WDT_UNLOCK();
    APB_WDT->Ctrl = 0;
    *AON1_BOOT &= ~(1u << 28);
}

void TMR_WatchDogRestart()
{
    WDT_UNLOCK();
    APB_WDT->Reset = 0xcafe;
}

void TMR_WatchDogClearInt(void)
{
    APB_WDT->St = 1;
}
#endif
