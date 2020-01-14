#include "cm32gpm3_timer.h"

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

uint32_t TMR_GetCMP(TMR_TypeDef *pTMR, uint32_t value)
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
}

void TMR_WatchDogDisable(void)
{
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
	APB_TMR0->LOCK = 0;
}
