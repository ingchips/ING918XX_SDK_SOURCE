#include "pingpang.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

static uint8_t FindBurstNum(uint8_t burstNum)
{
    uint8_t i;
    for (i = 0; i <= 3; ++i) {
        if ((1 << i) == burstNum) {
            return i;
        }
    }
    return -1;
}
uint8_t DMA_PingPangSetup(DMA_PingPang_t *PingPang, 
                          SYSCTRL_DMA srcDev, 
                          uint32_t transWordsNum, 
                          uint8_t burstNum)
{
    if (transWordsNum % burstNum) transWordsNum -= transWordsNum % burstNum;
    uint8_t ret;
    uint8_t b = FindBurstNum(burstNum);
    uint16_t bits = transWordsNum * sizeof(uint32_t);
    PingPang->ping = malloc(bits);
    PingPang->pang = malloc(bits);
    PingPang->descriptor_pp[0].Next = &PingPang->descriptor_pp[1];
    PingPang->descriptor_pp[1].Next = &PingPang->descriptor_pp[0];
    ret = DMA_PreparePeripheral2Mem(&PingPang->descriptor_pp[PING_WRITE],
							  (uint32_t *)PingPang->ping,
							  srcDev,
							  bits,
							  DMA_ADDRESS_INC,
							  0
							  | b << 24
							  | 1 << DMA_DESC_OPTION_BIT_INTERRUPT_EACH_DESC);
    ret += DMA_PreparePeripheral2Mem(&PingPang->descriptor_pp[PANG_WRITE],
							  (uint32_t *)PingPang->pang,
							  srcDev,
							  bits,
							  DMA_ADDRESS_INC,
							  0
							  | b << 24
							  | 1 << DMA_DESC_OPTION_BIT_INTERRUPT_EACH_DESC);
    PingPang->status = PING_WRITE;
    return ret;
}

uint32_t * DMA_PingPangIntProc(DMA_PingPang_t *PingPang, uint8_t ch)
{
	PingPang->status ^= 1;
    DMA_EnableChannel(ch, &PingPang->descriptor_pp[PingPang->status]);
	uint32_t *rr = (PingPang->status == PING_WRITE) ? (PingPang->pang) : (PingPang->ping);
    return rr;
}

uint32_t DMA_PingPangGetTransSize(DMA_PingPang_t *PingPang)
{
    return PingPang->descriptor_pp[0].TranSize;
}

void DMA_PingPangEnable(DMA_PingPang_t *PingPang, uint8_t ch)
{
    DMA_Reset(1);
    DMA_EnableChannel(ch, &PingPang->descriptor_pp[PingPang->status]);
}

void DMA_PingPangDisable(DMA_PingPang_t *PingPang, uint8_t ch)
{
    DMA_AbortChannel(ch);
    DMA_Reset(1);
    free(PingPang->ping);
    free(PingPang->pang);
}

#endif
