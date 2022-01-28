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


typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t REV_MINOR                        : 4 ;
        volatile uint32_t REV_MAJOR                        : 4 ;
        volatile uint32_t PRODUCT_ID                       : 24;
    }f;
}DMA_REG_IdRev;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t unused0                          : 31;
        volatile uint32_t COMP                             : 1 ;
    }f;
}DMA_REG_IdMisc;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t ChannelNum                       : 4 ;
        volatile uint32_t FIFODepth                        : 6 ;
        volatile uint32_t ReqNum                           : 5 ;
        volatile uint32_t BusNum                           : 1 ;
        volatile uint32_t CoreNum                          : 1 ;
        volatile uint32_t AddrWidth                        : 7 ;
        volatile uint32_t DataWidth                        : 2 ;
        volatile uint32_t unused1                          : 4 ;
        volatile uint32_t ReqSync                          : 1 ;
        volatile uint32_t ChainXfr                         : 1 ;
    }f;
}DMA_REG_DMACfg;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t Reset                            : 1 ;
        volatile uint32_t unused2                          : 31;
    }f;
}DMA_REG_DMACtrl;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t ChAbort                          : 8 ;
        volatile uint32_t unused3                          : 24;
    }f;
}DMA_REG_ChAbort;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t Error                            : 8 ;
        volatile uint32_t Abort                            : 8 ;
        volatile uint32_t TC                               : 8 ;
        volatile uint32_t unused4                          : 8 ;
    }f;
}DMA_REG_IntStatus;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t ChEN                             : 8 ;
        volatile uint32_t unused5                          : 24;
    }f;
}DMA_REG_ChEN;

#define DMA_REG_CHCTRL_ENABLE (0)

typedef enum
{
  DMA_DSTADDRCTRL_INCREMENT_ADDRESS = 0,
  DMA_DSTADDRCTRL_DECREMENT_ADDRESS,
  DMA_DSTADDRCTRL_FIXED_ADDRESS
}DMA_REG_ChCtrl_DstAddrCtrl;

typedef enum
{
  DMA_SRCADDRCTRL_INCREMENT_ADDRESS = 0,
  DMA_SRCADDRCTRL_DECREMENT_ADDRESS,
  DMA_SRCADDRCTRL_FIXED_ADDRESS
}DMA_REG_ChCtrl_SrcAddrCtrl;

typedef enum
{
  DMA_DSTWIDTH_BYTE_TRANSFER = 0,
  DMA_DSTWIDTH_HALF_WORD_TRANSFER,
  DMA_DSTWIDTH_WORD_TRANSFER,
  DMA_DSTWIDTH_DOUBLE_WORD_TRANSFER
}DMA_REG_ChCtrl_DstWidth;

typedef enum
{
  DMA_SRCWIDTH_BYTE_TRANSFER = 0,
  DMA_SRCWIDTH_HALF_WORD_TRANSFER,
  DMA_SRCWIDTH_WORD_TRANSFER,
  DMA_SRCWIDTH_DOUBLE_WORD_TRANSFER
}DMA_REG_ChCtrl_SrcWidth;

typedef enum
{
  DMA_SRCBURSTSIZE_1_TRANSFER = 0,
  DMA_SRCBURSTSIZE_2_TRANSFER,
  DMA_SRCBURSTSIZE_4_TRANSFER,
  DMA_SRCBURSTSIZE_8_TRANSFER,
  DMA_SRCBURSTSIZE_16_TRANSFER,
  DMA_SRCBURSTSIZE_32_TRANSFER,
  DMA_SRCBURSTSIZE_64_TRANSFER,
  DMA_SRCBURSTSIZE_128_TRANSFER
}DMA_REG_ChCtrl_SrcBurstSize;

typedef enum
{
  DMA_EN_INT_VALID_AT_END_OF_CHAIN_TRANSITION = 0,
  DMA_EN_INT_VALID_AT_END_OF_BURST_TRANS
}DMA_REG_ChCtrl_En_Int;

typedef enum
{
  DMA_PRIORITY_LOWER_PRIORITY = 0,
  DMA_PRIORITY_HIGHER_PRIORITY
}DMA_REG_ChCtrl_Priority;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t Enable                           : 1 ;
        volatile uint32_t IntTCMask                        : 1 ;
        volatile uint32_t IntErrMask                       : 1 ;
        volatile uint32_t IntAbtMask                       : 1 ;
        volatile uint32_t DstReqSel                        : 4 ;
        volatile uint32_t SrcReqSel                        : 4 ;
        volatile uint32_t DstAddrCtrl                      : 2 ;
        volatile uint32_t SrcAddrCtrl                      : 2 ;
        volatile uint32_t DstMode                          : 1 ;
        volatile uint32_t SrcMode                          : 1 ;
        volatile uint32_t DstWidth                         : 3 ;
        volatile uint32_t SrcWidth                         : 3 ;
        volatile uint32_t SrcBurstSize                     : 4 ;
        volatile uint32_t en_int_chain_single              : 1 ;
        volatile uint32_t Priority                         : 1 ;
        volatile uint32_t unused6                          : 2 ;
    }f;
}DMA_REG_ChCtrl;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t TranSize                         : 32;
    }f;
}DMA_REG_ChTranSize;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t SrcAddr                          : 32;
    }f;
}DMA_REG_ChSrcAddr;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t DstAddr                          : 32;
    }f;
}DMA_REG_ChDstAddr;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t unused7                          : 3 ;
        volatile uint32_t LLPointer                        : 29;
    }f;
}DMA_REG_ChLLPointer;


typedef struct
{
  volatile DMA_REG_ChCtrl                                             ChCtrl                       ; //0x40
  volatile DMA_REG_ChTranSize                                         ChTranSize                   ; //0x44
  volatile DMA_REG_ChSrcAddr                                          ChSrcAddr                    ; //0x48
  volatile uint32_t                                                    null_reg9                     ; //0x4C
  volatile DMA_REG_ChDstAddr                                          ChDstAddr                    ; //0x50
  volatile uint32_t                                                    null_reg10                    ; //0x54
  volatile DMA_REG_ChLLPointer                                        ChLLPointer                  ; //0x58
  volatile uint32_t                                                    null_reg11                    ; //0x5C
}DMA_REG_ChCtrl_Reg;

#define DMA_REG_CH_NUM (8)
typedef struct
{
  volatile DMA_REG_IdRev                                               IdRev                         ; //0x0
  volatile DMA_REG_IdMisc                                              IdMisc                        ; //0x4
  volatile uint32_t                                                    null_reg0                     ; //0x8
  volatile uint32_t                                                    null_reg1                     ; //0xC
  volatile DMA_REG_DMACfg                                              DMACfg                        ; //0x10
  volatile uint32_t                                                    null_reg2                     ; //0x14
  volatile uint32_t                                                    null_reg3                     ; //0x18
  volatile uint32_t                                                    null_reg4                     ; //0x1C
  volatile DMA_REG_DMACtrl                                             DMACtrl                       ; //0x20
  volatile DMA_REG_ChAbort                                             ChAbort                       ; //0x24
  volatile uint32_t                                                    null_reg5                     ; //0x28
  volatile uint32_t                                                    null_reg6                     ; //0x2C
  volatile DMA_REG_IntStatus                                           IntStatus                     ; //0x30
  volatile DMA_REG_ChEN                                                ChEN                          ; //0x34
  volatile uint32_t                                                    null_reg7                     ; //0x38
  volatile uint32_t                                                    null_reg8                     ; //0x3C
  DMA_REG_ChCtrl_Reg                                                   ChCtrlList[DMA_REG_CH_NUM]    ; // 0x40+n*0x20
}DMA_REG_H;

void DMA_Data_Transfer(DMA_REG_H* DMA_BASE, DMA_REG_ChCtrl_Reg Params[], uint8_t len)
{
    uint8_t i;
    for(i = 0; i < len; i++)
    {
      if(DMA_REG_CH_NUM <= len) break;

      DMA_BASE->ChCtrlList[i].ChCtrl.r = Params[i].ChCtrl.r | (0 << DMA_REG_CHCTRL_ENABLE);
      DMA_BASE->ChCtrlList[i].ChSrcAddr.r = Params[i].ChSrcAddr.r;
      DMA_BASE->ChCtrlList[i].ChDstAddr.r = Params[i].ChDstAddr.r;
      DMA_BASE->ChCtrlList[i].ChTranSize.r = Params[i].ChTranSize.r;

      DMA_BASE->ChCtrlList[i].ChCtrl.r |= (1 << DMA_REG_CHCTRL_ENABLE);
    }

}


void DMA_Data_Transfer_Without_Chain_Transfer(DMA_REG_H* DMA_BASE, uint32_t Src, uint32_t Dst, uint32_t Size)
{
  DMA_REG_ChCtrl_Reg Params[1];

  Params[0].ChCtrl.f.Enable              = 0x0;
  Params[0].ChCtrl.f.IntTCMask           = 0x0;
  Params[0].ChCtrl.f.IntErrMask          = 0x0;
  Params[0].ChCtrl.f.IntAbtMask          = 0x0;
  Params[0].ChCtrl.f.DstReqSel           = 0x0;
  Params[0].ChCtrl.f.SrcReqSel           = 0x0;
  Params[0].ChCtrl.f.DstAddrCtrl         = DMA_DSTADDRCTRL_INCREMENT_ADDRESS;
  Params[0].ChCtrl.f.SrcAddrCtrl         = DMA_SRCADDRCTRL_INCREMENT_ADDRESS;
  Params[0].ChCtrl.f.DstMode             = 0x0;
  Params[0].ChCtrl.f.SrcMode             = 0x0;
  Params[0].ChCtrl.f.DstWidth            = DMA_DSTWIDTH_BYTE_TRANSFER;
  Params[0].ChCtrl.f.SrcWidth            = DMA_SRCWIDTH_BYTE_TRANSFER;
  Params[0].ChCtrl.f.SrcBurstSize        = DMA_SRCBURSTSIZE_1_TRANSFER;
  Params[0].ChCtrl.f.en_int_chain_single = DMA_EN_INT_VALID_AT_END_OF_CHAIN_TRANSITION;
  Params[0].ChCtrl.f.Priority            = DMA_PRIORITY_HIGHER_PRIORITY;

  Params[0].ChSrcAddr.r = Src;
  Params[0].ChDstAddr.r = Dst;
  Params[0].ChTranSize.r = Size;

  DMA_Data_Transfer(DMA_BASE, Params, 1);
}

int DMA_PrepareRAM2RAM(DMA_Descriptor *pDesc,
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

static int DMA_GetPeripheralWidth(SYSCTRL_DMA src)
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
    case SYSCTRL_DMA_SPI1_TX:
    case SYSCTRL_DMA_I2S_RX:
    case SYSCTRL_DMA_PDM:
    case SYSCTRL_DMA_ADC:
    case SYSCTRL_DMA_SPI0_RX:
    case SYSCTRL_DMA_SPI1_RX:
    case SYSCTRL_DMA_I2S_TX:
    case SYSCTRL_DMA_PWM:
    case SYSCTRL_DMA_KeyScan:
    case SYSCTRL_DMA_QDEC0:
    case SYSCTRL_DMA_QDEC1:
    case SYSCTRL_DMA_QDEC2:
        return DMA_WIDTH_32_BITS;
    default:
        return -1;
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
        return 0;
    case SYSCTRL_DMA_QDEC1:
        return 0;
    case SYSCTRL_DMA_I2S_RX:
        return &APB_I2S->RX;
    case SYSCTRL_DMA_PDM:
        return &APB_PDM->Data;
    case SYSCTRL_DMA_ADC:
        return 0;

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
        return 0;
    case SYSCTRL_DMA_PWM:
        return &APB_PWM->DmaData;

    default:
        return 0;
    }
}

int DMA_PreparePeripheral2RAM(DMA_Descriptor *pDesc,
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
    pDesc->TranSize = width == DMA_WIDTH_BYTE ? size : size / sizeof(uint32_t);
    pDesc->DstAddr = (uint32_t)dst;
    pDesc->SrcAddr = (uint32_t)peri_addr;

    return 0;
}

int DMA_PrepareRAM2Peripheral(DMA_Descriptor *pDesc,
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
    pDesc->TranSize = src_width == DMA_WIDTH_BYTE ? size : size / sizeof(uint32_t);
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
    first->Ctrl &= ~(uint32_t)1;
    APB_DMA->Channels[channel_id].Descriptor = *first;
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
    DMA_PrepareRAM2RAM(&descriptor, dst, src, size, DMA_ADDRESS_INC, DMA_ADDRESS_INC, 0);
    DMA_EnableChannel(channel_id, &descriptor);

    while ((state = DMA_GetChannelIntState(channel_id)) == 0) ;
    DMA_ClearChannelIntState(channel_id, state);

    return state & (DMA_IRQ_ERROR | DMA_IRQ_ABORT) ? 1 : 0;
}

#endif
