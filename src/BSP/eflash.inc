
#include "stdint.h"   
#include "eflash.h"    
 
extern uint32_t ClkFreq; //0:16M 1:24M

static void Delay10US (uint32_t  ulTime)
{
    uint32_t i;
		uint32_t cnt;
		cnt = (ClkFreq==1) ? 119 : 79;
    while (ulTime--) {
        for (i = 0; i < cnt; i++);
    }
}

static void DelayMS (uint32_t  ulTime)
{
    uint32_t i;
		uint32_t cnt;
		cnt = (ClkFreq==1) ? 11892 : 7930;
    while (ulTime--) {
        for (i = 0; i < cnt; i++);
    }
}

static void EraseEFlashMas(void)
{
		*(uint32_t *)0xc4004 = 0x44650000;
		DelayMS(1);
		while((*(uint32_t volatile*)0x4007002c>>1) & 0x1);
}

static void EraseEFlashPage(uint32_t PageIdx)
{
		*(uint32_t *)0xc4004 = 0x84650000+PageIdx;
		DelayMS(1);
		while((*(uint32_t volatile*)0x4007002c>>1) & 0x1);
}

static void EflashBaseTime(void)
{
		*(uint32_t *)0xc4008 = 0x19000;
}

static void EflashRepair(void)
{
#ifndef FPGA_SIM
		uint32_t rdata;
		rdata = *(uint32_t *)0xc4000;
		*(uint32_t *)0xc4000 = rdata | 0x1;
		while((*(uint32_t volatile*)0xc4000) & 0x1);
#endif
}

static void EflashCacheBypass(void)
{
		*(uint32_t *)0x40041000 = 0x2;
}

static void EflashCacheEna(void)
{
		*(uint32_t *)0x40041000 = 0x0;
}

static void EflashCacheFlush(void)
{
		*(uint32_t *)0x40041000 = 0x1;
		while((*(uint32_t volatile*)0x4007000c)>>4 & 0x1);
}

static void EflashProgramEnable(void)
{
		*(uint32_t *)0xc400c = 0x2;
}

static void EflashProgramDisable(void)
{
#ifndef FPGA_SIM
		*(uint32_t *)0xc400c = 0x0;
#endif
}

static void EflashProgram(uint32_t addr, uint32_t data)
{
		*(uint32_t *)addr = data;
		Delay10US(1);
		while((*(uint32_t volatile*)0x4007002c) & 0x1);
}
