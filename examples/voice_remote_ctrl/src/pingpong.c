#include "pingpong.h"

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
uint8_t DMA_PingPongSetup(DMA_PingPong_t *PingPong, 
                          SYSCTRL_DMA srcDev, 
                          uint32_t transWordsNum, 
                          uint8_t burstNum)
{
    if (transWordsNum % burstNum) transWordsNum -= transWordsNum % burstNum;
    uint8_t ret;
    uint8_t b = FindBurstNum(burstNum);
    uint16_t bits = transWordsNum * sizeof(uint32_t);
    PingPong->ping = malloc(bits);
    PingPong->pong = malloc(bits);
    PingPong->descriptor_pp[0].Next = &PingPong->descriptor_pp[1];
    PingPong->descriptor_pp[1].Next = &PingPong->descriptor_pp[0];
    ret = DMA_PreparePeripheral2Mem(&PingPong->descriptor_pp[PING_WRITE],
                              (uint32_t *)PingPong->ping,
                              srcDev,
                              bits,
                              DMA_ADDRESS_INC,
                              0
                              | b << 24
                              | 1 << DMA_DESC_OPTION_BIT_INTERRUPT_EACH_DESC);
    ret += DMA_PreparePeripheral2Mem(&PingPong->descriptor_pp[PONG_WRITE],
                              (uint32_t *)PingPong->pong,
                              srcDev,
                              bits,
                              DMA_ADDRESS_INC,
                              0
                              | b << 24
                              | 1 << DMA_DESC_OPTION_BIT_INTERRUPT_EACH_DESC);
    PingPong->status = PING_WRITE;
    return ret;
}

uint32_t * DMA_PingPongIntProc(DMA_PingPong_t *PingPong, uint8_t ch)
{
    PingPong->status ^= 1;
    DMA_EnableChannel(ch, &PingPong->descriptor_pp[PingPong->status]);
    uint32_t *rr = (PingPong->status == PING_WRITE) ? (PingPong->pong) : (PingPong->ping);
    return rr;
}

uint32_t DMA_PingPongGetTransSize(DMA_PingPong_t *PingPong)
{
    return PingPong->descriptor_pp[0].TranSize;
}

void DMA_PingPongEnable(DMA_PingPong_t *PingPong, uint8_t ch)
{
    DMA_Reset(1);
    DMA_EnableChannel(ch, &PingPong->descriptor_pp[PingPong->status]);
}

void DMA_PingPongDisable(DMA_PingPong_t *PingPong, uint8_t ch)
{
    DMA_AbortChannel(ch);
    DMA_Reset(1);
    free(PingPong->ping);
    free(PingPong->pong);
}

#endif
