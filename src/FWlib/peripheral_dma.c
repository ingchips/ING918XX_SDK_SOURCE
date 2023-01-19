#include "peripheral_dma.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

#define bsDMA_DST_REQ_SEL        4
#define bsDMA_SRC_REQ_SEL        8
#define bsDMA_DST_ADDR_CTRL      12
#define bsDMA_SRC_ADDR_CTRL      14
#define bsDMA_DST_MODE           16
#define bsDMA_SRC_MODE           17
#define bsDMA_DST_WIDTH          18
#define bsDMA_SRC_WIDTH          21

typedef enum
{
    DMA_WIDTH_BYTE = 0,
    DMA_WIDTH_16_BITS = 1,
    DMA_WIDTH_32_BITS = 2,
    DMA_WIDTH_64_BITS = 3,
} DMA_TransferWidth;

int DMA_PrepareMem2Mem(DMA_Descriptor *pDesc,
                                 void *dst, void *src, int size,
                                 DMA_AddressControl dst_addr_ctrl, DMA_AddressControl src_addr_ctrl,
                                 uint32_t options)
{
    uint32_t v = ((uint32_t)dst & 3) | ((uint32_t)src & 3) | (size & 3);

    pDesc->Ctrl = options
                 | ((uint32_t)dst_addr_ctrl << bsDMA_DST_ADDR_CTRL)
                 | ((uint32_t)src_addr_ctrl << bsDMA_SRC_ADDR_CTRL)
                 | ((v ? DMA_WIDTH_BYTE : DMA_WIDTH_32_BITS) << bsDMA_DST_WIDTH)
                 | ((v ? DMA_WIDTH_BYTE : DMA_WIDTH_32_BITS) << bsDMA_SRC_WIDTH);
    pDesc->TranSize = v ? size : size / sizeof(uint32_t);
    pDesc->DstAddr = (uint32_t)dst;
    pDesc->SrcAddr = (uint32_t)src;
    return 0;
}

static int DMG_GetSPIDMAWidth(SSP_TypeDef *SSP)
{
    switch ((SSP->TransFmt >> 8) & 0x1f)
    {
    case 7:
        return DMA_WIDTH_BYTE;
    case 15:
        return DMA_WIDTH_16_BITS;
    default:
        return DMA_WIDTH_32_BITS;
    }
}

static DMA_TransferWidth DMA_GetPeripheralWidth(SYSCTRL_DMA src)
{
    switch (src)
    {
    case SYSCTRL_DMA_UART0_RX:
    case SYSCTRL_DMA_UART1_RX:
    case SYSCTRL_DMA_I2C0:
    case SYSCTRL_DMA_UART0_TX:
    case SYSCTRL_DMA_UART1_TX:
    case SYSCTRL_DMA_I2C1:
        return DMA_WIDTH_BYTE;

    case SYSCTRL_DMA_SPI0_TX:
    case SYSCTRL_DMA_SPI0_RX:
        return DMG_GetSPIDMAWidth(AHB_SSP0);
    case SYSCTRL_DMA_SPI1_TX:
    case SYSCTRL_DMA_SPI1_RX:
        return DMG_GetSPIDMAWidth(APB_SSP1);
    case SYSCTRL_DMA_I2S_RX:
    case SYSCTRL_DMA_PDM:
    case SYSCTRL_DMA_ADC:
    case SYSCTRL_DMA_I2S_TX:
    case SYSCTRL_DMA_PWM0:
    case SYSCTRL_DMA_PWM1:
    case SYSCTRL_DMA_PWM2:
    case SYSCTRL_DMA_KeyScan:
    case SYSCTRL_DMA_QDEC0:
    case SYSCTRL_DMA_QDEC1:
    case SYSCTRL_DMA_QDEC2:
        return DMA_WIDTH_32_BITS;
    default:
        return DMA_WIDTH_32_BITS;
    }
}

static volatile void *DMA_GetPeripheralDataAddr(SYSCTRL_DMA src)
{
    switch (src)
    {
    case SYSCTRL_DMA_UART0_RX:
        return &APB_UART0->DataRead;
    case SYSCTRL_DMA_UART1_RX:
        return &APB_UART1->DataRead;
    case SYSCTRL_DMA_SPI0_TX:
        return &AHB_SSP0->Data;
    case SYSCTRL_DMA_SPI1_TX:
        return &APB_SSP1->Data;
    case SYSCTRL_DMA_I2C0:
        return &APB_I2C0->Data;
    case SYSCTRL_DMA_QDEC0:
        return &APB_QDEC->channels[0].channel_write_a;
    case SYSCTRL_DMA_QDEC1:
        return 0;
    case SYSCTRL_DMA_I2S_RX:
        return &APB_I2S->RX;
    case SYSCTRL_DMA_PDM:
        return &APB_PDM->Data;
    case SYSCTRL_DMA_ADC:
        return &APB_SADC->sadc_data;

    case SYSCTRL_DMA_UART0_TX:
        return &APB_UART0->DataRead;
    case SYSCTRL_DMA_UART1_TX:
        return &APB_UART1->DataRead;
    case SYSCTRL_DMA_SPI0_RX:
        return &AHB_SSP0->Data;
    case SYSCTRL_DMA_SPI1_RX:
        return &APB_SSP1->Data;
    case SYSCTRL_DMA_I2C1:
        return &APB_I2C1->Data;
    case SYSCTRL_DMA_QDEC2:
        return 0;
    case SYSCTRL_DMA_KeyScan:
        return 0;
    case SYSCTRL_DMA_I2S_TX:
        return &APB_I2S->TX;
    case SYSCTRL_DMA_PWM0:
        return &APB_PWM->Channels[0].DmaData;
    case SYSCTRL_DMA_PWM1:
        return &APB_PWM->Channels[1].DmaData;
    case SYSCTRL_DMA_PWM2:
        return &APB_PWM->Channels[2].DmaData;

    default:
        return 0;
    }
}

int DMA_PreparePeripheral2Mem(DMA_Descriptor *pDesc,
                                        uint32_t *dst, SYSCTRL_DMA src, int size,
                                        DMA_AddressControl dst_addr_ctrl,
                                        uint32_t options)
{
    int req = SYSCTRL_GetDmaId(src);
    int width = DMA_GetPeripheralWidth(src);
    uint32_t peri_addr = (uint32_t)DMA_GetPeripheralDataAddr(src);
    uint32_t v = ((uint32_t)dst & 3) | (size & 3);

    if ((req < 0) | (width < 0) | (peri_addr == 0)) return -1;

    pDesc->Ctrl = options
                 | ((uint32_t)1 << bsDMA_SRC_MODE)
                 | ((uint32_t)req << bsDMA_SRC_REQ_SEL)
                 | ((uint32_t)DMA_ADDRESS_FIXED << bsDMA_SRC_ADDR_CTRL)
                 | ((uint32_t)width << bsDMA_SRC_WIDTH)
                 | ((uint32_t)dst_addr_ctrl << bsDMA_DST_ADDR_CTRL)
                 | ((v ? DMA_WIDTH_BYTE : DMA_WIDTH_32_BITS) << bsDMA_DST_WIDTH);
    pDesc->TranSize = size / (1 << width);
    pDesc->DstAddr = (uint32_t)dst;
    pDesc->SrcAddr = (uint32_t)peri_addr;

    return 0;
}

int DMA_PrepareMem2Peripheral(DMA_Descriptor *pDesc,
                                        SYSCTRL_DMA dst, uint32_t *src, int size,
                                        DMA_AddressControl src_addr_ctrl,
                                        uint32_t options)
{
    int req = SYSCTRL_GetDmaId(dst);
    int width = DMA_GetPeripheralWidth(dst);
    uint32_t peri_addr = (uint32_t)DMA_GetPeripheralDataAddr(dst);
    uint32_t v = ((uint32_t)src & 3) | (size & 3);

    if ((req < 0) | (width < 0) | (peri_addr == 0)) return -1;
    if((DMA_WIDTH_32_BITS == width) && (size & 3)) return -2;

    pDesc->Ctrl = options
                 | ((uint32_t)src_addr_ctrl << bsDMA_SRC_ADDR_CTRL)
                 | ((v ? DMA_WIDTH_BYTE : DMA_WIDTH_32_BITS) << bsDMA_SRC_WIDTH)
                 | ((uint32_t)1 << bsDMA_DST_MODE)
                 | ((uint32_t)req << bsDMA_DST_REQ_SEL)
                 | ((uint32_t)DMA_ADDRESS_FIXED << bsDMA_DST_ADDR_CTRL)
                 | ((uint32_t)width << bsDMA_DST_WIDTH);
    pDesc->TranSize = v ? size : size / sizeof(uint32_t);
    pDesc->DstAddr = (uint32_t)peri_addr;
    pDesc->SrcAddr = (uint32_t)src;

    return 0;
}

int DMA_PreparePeripheral2Peripheral(DMA_Descriptor *pDesc,
                                               SYSCTRL_DMA dst, SYSCTRL_DMA src, int size,
                                               uint32_t options)
{
    int dst_req = SYSCTRL_GetDmaId(dst);
    int dst_width = DMA_GetPeripheralWidth(dst);
    uint32_t dst_addr = (uint32_t)DMA_GetPeripheralDataAddr(dst);

    int src_req = SYSCTRL_GetDmaId(src);
    int src_width = DMA_GetPeripheralWidth(src);
    uint32_t src_addr = (uint32_t)DMA_GetPeripheralDataAddr(src);

    if ((dst_req < 0) | (dst_width < 0) | (dst_addr == 0)) return -1;
    if ((src_req < 0) | (src_width < 0) | (src_addr == 0)) return -2;

    if((DMA_WIDTH_32_BITS == dst_width) && (size & 3)) return -3;

    pDesc->Ctrl = options
                 | ((uint32_t)1 << bsDMA_DST_MODE)
                 | ((uint32_t)dst_req << bsDMA_DST_REQ_SEL)
                 | ((uint32_t)DMA_ADDRESS_FIXED << bsDMA_DST_ADDR_CTRL)
                 | ((uint32_t)dst_width << bsDMA_DST_WIDTH)
                 | ((uint32_t)1 << bsDMA_SRC_MODE)
                 | ((uint32_t)src_req << bsDMA_SRC_REQ_SEL)
                 | ((uint32_t)DMA_ADDRESS_FIXED << bsDMA_SRC_ADDR_CTRL)
                 | ((uint32_t)src_width << bsDMA_SRC_WIDTH);
    pDesc->TranSize = src_width == DMA_WIDTH_BYTE ? size : size / (1 << src_width);
    pDesc->DstAddr = (uint32_t)dst_addr;
    pDesc->SrcAddr = (uint32_t)src_addr;

    return 0;
}

void DMA_Reset(uint8_t reset)
{
    if (reset)
        APB_DMA->Ctrl |= 1;
    else
        APB_DMA->Ctrl &= ~(uint32_t)1;
}

void DMA_EnableChannel(int channel_id, DMA_Descriptor *first)
{
    uint32_t t = first->Ctrl;
    first->Ctrl &= ~(uint32_t)1;
    APB_DMA->Channels[channel_id].Descriptor = *first;
    first->Ctrl = t;
    APB_DMA->Channels[channel_id].Descriptor.Ctrl |= 1;
}

void DMA_AbortChannel(int channel_id)
{
    APB_DMA->Abort |= (1 << channel_id);
}

uint32_t DMA_GetChannelIntState(int channel_id)
{
    uint32_t v = APB_DMA->IntStatus;
    uint32_t r = (v >> channel_id) & 1;
    r |= (v >> (8 + channel_id - 1)) & 2;
    r |= (v >> (16 + channel_id - 2)) & 4;
    return r;
}

void DMA_ClearChannelIntState(int channel_id, uint32_t state)
{
    uint32_t v = state & DMA_IRQ_ERROR ? (1 << channel_id) : 0;
    v |= state & DMA_IRQ_ABORT ? (1 << (8 + channel_id)) : 0;
    v |= state & DMA_IRQ_TC ? (1 << (16 + channel_id)) : 0;
    APB_DMA->IntStatus = v;
}

int DMA_MemCopy(int channel_id, void *dst, void *src, int size)
{
    DMA_Descriptor descriptor;
    uint32_t state;
    descriptor.Next = (DMA_Descriptor *)0;
    DMA_PrepareMem2Mem(&descriptor, dst, src, size, DMA_ADDRESS_INC, DMA_ADDRESS_INC, 0);
    DMA_EnableChannel(channel_id, &descriptor);

    while ((state = DMA_GetChannelIntState(channel_id)) == 0) ;
    DMA_ClearChannelIntState(channel_id, state);

    return state & (DMA_IRQ_ERROR | DMA_IRQ_ABORT) ? 1 : 0;
}

#endif
