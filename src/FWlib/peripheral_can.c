#include "peripheral_can.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
#define CAN_CTRL0_STABLE 0xc8bce0f8u
int CAN_SetTiming(CAN_TypeDef *base, uint32_t nominal, uint32_t data)
{
    if (!base || !(base->Ctrl0 & 0x80u) || (nominal & ~0xff7f7fffu) || (data & ~0xff0f0f1fu)) return -1;
    base->NominalTiming = nominal;
    base->DataTiming = data;
    return 0;
}
void CAN_Enable(CAN_TypeDef *base, uint8_t enable)
{
    uint32_t ctrl = base->Ctrl0 & CAN_CTRL0_STABLE;
    base->Ctrl0 = enable ? ctrl & ~0x80u : ctrl | 0x80u;
}
int CAN_SetFilter(CAN_TypeDef *base, uint8_t index, uint32_t code, uint32_t mask)
{
    uint32_t select;
    if (!base || index >= 16 || !(base->Ctrl0 & 0x80u)) return -1;
    select = base->FilterSelect & ~0x2fu;
    base->FilterSelect = select | index;
    base->FilterCode = code;
    base->FilterSelect = select | index | 0x20u;
    base->FilterCode = mask;
    base->FilterSelect = (select | (1u << (16u + index))) & ~0x2fu;
    return 0;
}
uint32_t CAN_GetStatus(CAN_TypeDef *base) { return base->Ctrl0; }
int CAN_TransmitSlot(CAN_TypeDef *base, const uint32_t words[18])
{
    uint32_t ctrl; unsigned int i;
    if (!base || !words) return -1;
    ctrl = base->Ctrl0;
    if (ctrl & (0x80u | 0x1000u | 2u)) return 1;
    ctrl = (ctrl & CAN_CTRL0_STABLE) & ~0x8000u;
    base->Ctrl0 = ctrl;
    for (i = 0; i < 18; i++) base->TxBuffer[i] = words[i];
    base->Ctrl0 = ctrl | 0x1000u;
    return 0;
}
int CAN_ReceiveSlot(CAN_TypeDef *base, uint32_t words[20])
{
    uint32_t ctrl; unsigned int i;
    if (!base || !words) return -1;
    ctrl = base->Ctrl0;
    if (!(ctrl & (3u << 24))) return 1;
    for (i = 0; i < 20; i++) words[i] = base->RxBuffer[i];
    base->Ctrl0 = (ctrl & CAN_CTRL0_STABLE) | (1u << 28);
    return 0;
}

#endif
