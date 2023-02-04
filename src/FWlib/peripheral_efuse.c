
#include "peripheral_efuse.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

/*
  example for efuse usage

  - write bit 3(total 128bits) to 1(default 0)
    EFUSE_UnLock(APB_EFUSE, EFUSE_UNLOCK_FLAG);
    EFUSE_WriteEfuseDataBitToOne(APB_EFUSE, 3);
    while(1 == EFUSE_GetStatusBusy(EFUSE_BASE));
    EFUSE_Lock(EFUSE_BASE);
    
  - read data
    EFUSE_SetRdFlag(APB_EFUSE);
    EFUSE_GetEfuseData(APB_EFUSE, 0)//bit 0  - 31
    EFUSE_GetEfuseData(APB_EFUSE, 1)//bit 32 - 63
    EFUSE_GetEfuseData(APB_EFUSE, 2)//bit 64 - 95
    EFUSE_GetEfuseData(APB_EFUSE, 3)//bit 96 - 127

*/

/*====================================================================*/
void EFUSE_SetCfg0(EFUSE_TypeDef* EFUSE_BASE, uint32_t val)
{
    EFUSE_BASE->Efuse_cfg0 |= val;
}

/*====================================================================*/
uint8_t EFUSE_GetStatusBusy(EFUSE_TypeDef* EFUSE_BASE)
{
    return (uint8_t)((EFUSE_BASE->Efuse_status >> bsEFUSE_STATUS_BUSY) & BW2M(bwEFUSE_STATUS_BUSY));
}

/*====================================================================*/
uint8_t EFUSE_GetStatusState(EFUSE_TypeDef* EFUSE_BASE)
{
    return (uint8_t)((EFUSE_BASE->Efuse_status >> bsEFUSE_STATUS_STATE) & BW2M(bwEFUSE_STATUS_STATE));
}

/*====================================================================*/
uint8_t EFUSE_GetDataValidFlag(EFUSE_TypeDef* EFUSE_BASE)
{
    return (uint8_t)((EFUSE_BASE->Efuse_cfg0 >> bsEFUSE_CFG0_DATA_VALID) & BW2M(bwEFUSE_CFG0_DATA_VALID));
}

/*====================================================================*/
void EFUSE_SetRdFlag(EFUSE_TypeDef* EFUSE_BASE)
{
    EFUSE_BASE->Efuse_cfg0 |= (0x1 << bsEFUSE_CFG0_RD_EFUSE);
}

/*====================================================================*/
uint32_t EFUSE_GetEfuseData(EFUSE_TypeDef* EFUSE_BASE, EFUSE_ProgramWordCnt index)
{
   uint32_t rdata = 0;
  
   if(index < EFUSE_REG_WORD_NUM)
   {
    rdata = (EFUSE_BASE->Efuse_rdata[index]);
   }
   return rdata;
}

/*====================================================================*/
void EFUSE_WriteEfuseDataBitToOne(EFUSE_TypeDef* EFUSE_BASE, uint8_t addr)
{
   EFUSE_BASE->Efuse_cfg0 &= (~(BW2M(bwEFUSE_CFG0_PROM_MODE) << bsEFUSE_CFG0_PROM_MODE));
   EFUSE_BASE->Efuse_cfg0 &= (~(BW2M(bwEFUSE_CFG0_PROM_ADDR) << bsEFUSE_CFG0_PROM_ADDR));
   EFUSE_BASE->Efuse_cfg0 |= addr << bsEFUSE_CFG0_PROM_ADDR;
   EFUSE_BASE->Efuse_cfg0 |= 0x1 << bsEFUSE_CFG0_PG_EFUSE;
}

/*====================================================================*/
void EFUSE_UnLock(EFUSE_TypeDef* EFUSE_BASE, uint16_t data)
{
   EFUSE_BASE->Efuse_cfg0 &= (~(BW2M(bwEFUSE_CFG0_EFUSE_LOCK) << bsEFUSE_CFG0_EFUSE_LOCK));
   EFUSE_BASE->Efuse_cfg0 |= (data << bsEFUSE_CFG0_EFUSE_LOCK);
}

/*====================================================================*/
void EFUSE_Lock(EFUSE_TypeDef* EFUSE_BASE)
{
   EFUSE_BASE->Efuse_cfg0 &= (~(BW2M(bwEFUSE_CFG0_EFUSE_LOCK) << bsEFUSE_CFG0_EFUSE_LOCK));
}

/*====================================================================*/
void EFUSE_WriteEfuseDataWord(EFUSE_TypeDef* EFUSE_BASE, EFUSE_ProgramWordCnt index, uint32_t data)
{
   EFUSE_BASE->Efuse_cfg0 &= (~(BW2M(bwEFUSE_CFG0_PROM_MODE) << bsEFUSE_CFG0_PROM_MODE));
   EFUSE_BASE->Efuse_cfg0 |= 0x1 << bsEFUSE_CFG0_PROM_MODE;
  
   switch (index)
   {
     case EFUSE_PROGRAMWORDCNT_0:
     {
       EFUSE_BASE->Efuse_cfg1 = data;
     }break;
     case EFUSE_PROGRAMWORDCNT_1:
     {
       EFUSE_BASE->Efuse_cfg2 = data;
     }break;
     case EFUSE_PROGRAMWORDCNT_2:
     {
       EFUSE_BASE->Efuse_cfg3 = data;
     }break;
     case EFUSE_PROGRAMWORDCNT_3:
     {
       EFUSE_BASE->Efuse_cfg4 = data;
     }break;
   }
   
   EFUSE_UnLock(EFUSE_BASE, EFUSE_UNLOCK_FLAG);
   EFUSE_BASE->Efuse_cfg0 |= 0x1 << bsEFUSE_CFG0_PG_EFUSE;

   while(1 == EFUSE_GetStatusBusy(EFUSE_BASE));
   EFUSE_Lock(EFUSE_BASE);
}


#endif
