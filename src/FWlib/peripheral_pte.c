#include "peripheral_pte.h"

void PTE_ClearInt(SYSCTRL_PTE_CHENNEL_ID ch)
{
    APB_PTE->Channels[ch].Int = 1;
}

uint8_t PTE_GetIntState(SYSCTRL_PTE_CHENNEL_ID ch)
{
    return (APB_PTE->Channels[ch].Int);
}

void PTE_EnableChennel(SYSCTRL_PTE_CHENNEL_ID ch)
{
    APB_PTE->Channels[ch].En = 1;
}

void PTE_DisableChennel(SYSCTRL_PTE_CHENNEL_ID ch)
{
    APB_PTE->Channels[ch].En = 0;
}

void PTE_SetChennelIntMask(SYSCTRL_PTE_CHENNEL_ID ch, uint32_t options)
{
    uint32_t *m;
    uint32_t Cm;

    if (options) 
        APB_PTE->Channels[ch].IntMask = 1;
    else {
        m = (uint32_t *)&(APB_PTE->Channels[ch]);
        Cm = PTE_MAKE_CHENNAL_INT_MASK_OPTION(!options);
        *m &= Cm;
    }
}

void PTE_IrqProcess(SYSCTRL_PTE_CHENNEL_ID ch)
{
    PTE_SetChennelIntMask(ch, 0);
}

void PTE_OutPeripheralContinueProcess(SYSCTRL_PTE_CHENNEL_ID ch)
{
    PTE_ClearInt(ch);
}

void PTE_OutPeripheralEndProcess(SYSCTRL_PTE_CHENNEL_ID ch)
{
    PTE_ClearInt(ch);
    PTE_DisableChennel(ch);
}

void PTE_ChennelClose(SYSCTRL_PTE_CHENNEL_ID ch)
{
    PTE_ClearInt(ch);
    PTE_DisableChennel(ch);
    PTE_SetChennelIntMask(ch, 0);
}

int PTE_ConnectPeripheral(SYSCTRL_PTE_CHENNEL_ID ch, 
                          SYSCTRL_PTE_SRC_INT src, 
                          SYSCTRL_PTE_DST_EN dst)
{
    if (ch  >  SYSCTRL_PTE_CHENNEL_4   ||
        src >= SYSCTRL_PTE_SRC_INT_MAX ||
        dst >= SYSCTRL_PTE_DST_EN_MAX) {
        return -1;
    }
    uint32_t Sb = PTE_MAKE_MASK_SIZE_OPTION(SYSCTRL_PTE_SRC_INT_MAX);
    uint32_t Db = PTE_MAKE_MASK_SIZE_OPTION(SYSCTRL_PTE_DST_EN_MAX);

    PTE_ClearInt(ch);
    APB_PTE->Channels[ch].InMask  &= (~Sb);
    APB_PTE->Channels[ch].OutMask &= (~Db);
    APB_PTE->Channels[ch].InMask  |= (1 << src);
    APB_PTE->Channels[ch].OutMask |= (1 << dst);
    PTE_SetChennelIntMask(ch, 1);
    PTE_EnableChennel(ch);

    return 0;
}

