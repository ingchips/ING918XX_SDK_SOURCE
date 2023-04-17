#include "peripheral_pte.h"

void PTE_ClearInt(SYSCTRL_PTE_CHANNEL_ID ch)
{
    APB_PTE->Channels[ch].Int = 1;
}

uint8_t PTE_GetIntState(SYSCTRL_PTE_CHANNEL_ID ch)
{
    return (APB_PTE->Channels[ch].Int);
}

void PTE_EnableChannel(SYSCTRL_PTE_CHANNEL_ID ch)
{
    APB_PTE->Channels[ch].En = 1;
}

void PTE_DisableChannel(SYSCTRL_PTE_CHANNEL_ID ch)
{
    APB_PTE->Channels[ch].En = 0;
}

void PTE_SetChannelIntMask(SYSCTRL_PTE_CHANNEL_ID ch, uint32_t options)
{
    uint32_t *m;
    uint32_t Cm;

    if (options) 
        APB_PTE->Channels[ch].IntMask = 1;
    else {
        m = (uint32_t *)&(APB_PTE->Channels[ch]);
        Cm = PTE_MAKE_CHANNEL_INT_MASK_OPTION(!options);
        *m &= Cm;
    }
}

void PTE_IrqProc(SYSCTRL_PTE_CHANNEL_ID ch)
{
    PTE_SetChannelIntMask(ch, 0);
}

void PTE_OutPeripheralContinueProc(SYSCTRL_PTE_CHANNEL_ID ch)
{
    PTE_ClearInt(ch);
}

void PTE_OutPeripheralEndProc(SYSCTRL_PTE_CHANNEL_ID ch)
{
    PTE_ClearInt(ch);
    PTE_DisableChannel(ch);
}

void PTE_ChannelClose(SYSCTRL_PTE_CHANNEL_ID ch)
{
    PTE_ClearInt(ch);
    PTE_DisableChannel(ch);
    PTE_SetChannelIntMask(ch, 0);
}

int PTE_ConnectPeripheral(SYSCTRL_PTE_CHANNEL_ID ch, 
                          SYSCTRL_PTE_SRC_INT src, 
                          SYSCTRL_PTE_DST_EN dst)
{
    if (ch  >  SYSCTRL_PTE_CHANNEL_3   ||
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
    PTE_SetChannelIntMask(ch, 1);
    PTE_EnableChannel(ch);

    return 0;
}

