#include "spi.h"
#include "ingsoc.h"



#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define SSP_Ptr    AHB_SSP0

#define DummyByte       0xA5
#define DummyWord       0xA5A5A5A5

#define W8_2(addr)      ((addr >> 16) & 0xFFU)
#define W8_1(addr)      ((addr >> 8) & 0xFFU)
#define W8_0(addr)      (addr& 0xFFU)

void SPI_Init(void)
{
    apSSP_sDeviceControlBlock param;

    apSSP_Initialize(SSP_Ptr);
    
    SYSCTRL_ResetBlock(SYSCTRL_Reset_AHB_SPI0);
    SYSCTRL_ReleaseBlock(SYSCTRL_Reset_AHB_SPI0);    

    /* Set Device Parameters */
    param.ClockRate        = 0;  // sspclkout = sspclk/(ClockPrescale*(ClockRate+1))
    param.ClockPrescale    = 2;  // Must be an even number from 2 to 254
    param.eSCLKPhase       = apSSP_SCLKPHASE_TRAILINGEDGE;
    param.eSCLKPolarity    = apSSP_SCLKPOLARITY_IDLELOW;
    param.eFrameFormat     = apSSP_FRAMEFORMAT_MOTOROLASPI;
    param.eDataSize        = apSSP_DATASIZE_8BITS;
    param.eLoopBackMode    = apSSP_LOOPBACKOFF;
    param.eMasterSlaveMode = apSSP_MASTER;
    param.eSlaveOutput     = apSSP_SLAVEOUTPUTDISABLED;
    apSSP_DeviceParametersSet(SSP_Ptr, &param);    
}

void SPI_Init16bit(void)
{
    apSSP_sDeviceControlBlock param;

    apSSP_Initialize(SSP_Ptr);
    
    SYSCTRL_ResetBlock(SYSCTRL_Reset_AHB_SPI0);
    SYSCTRL_ReleaseBlock(SYSCTRL_Reset_AHB_SPI0);    

    /* Set Device Parameters */
    param.ClockRate        = 0;  // sspclkout = sspclk/(ClockPrescale*(ClockRate+1))
    param.ClockPrescale    = 2;  // Must be an even number from 2 to 254
    param.eSCLKPhase       = apSSP_SCLKPHASE_TRAILINGEDGE;
    param.eSCLKPolarity    = apSSP_SCLKPOLARITY_IDLELOW;
    param.eFrameFormat     = apSSP_FRAMEFORMAT_MOTOROLASPI;
    param.eDataSize        = apSSP_DATASIZE_16BITS;
    param.eLoopBackMode    = apSSP_LOOPBACKOFF;
    param.eMasterSlaveMode = apSSP_MASTER;
    param.eSlaveOutput     = apSSP_SLAVEOUTPUTDISABLED;
    apSSP_DeviceParametersSet(SSP_Ptr, &param);
    
}

/*********************************************/
//
//********************************************/
void SPI_Delay(uint32_t t)
{ 
    for(t=255; t>0; t--) 
    {
        __NOP(); 
    }
}
 
//********************************************/
//
//
//
//********************************************/ 
void SPI_Write(uint8_t data)
{
    // send data
    apSSP_WriteFIFO(SSP_Ptr, data);
    // wait data send complete
    apSSP_DeviceEnable(SSP_Ptr);
    while(apSSP_DeviceBusyGet(AHB_SSP0));
    apSSP_DeviceDisable(SSP_Ptr);
    // clear dummy data
    apSSP_ReadFIFO(SSP_Ptr);
}

void SPI_Write_FIFO_16(const uint16_t *data, int len)
{
    apSSP_DeviceEnable(SSP_Ptr);
    for (; len > 0; len--)
    {
        while (!apSSP_TxFifoNotFull(SSP_Ptr));
        apSSP_WriteFIFO(SSP_Ptr, *data++);
    }
}

void SPI_Write_FIFO_8(const uint8_t *data, int len)
{
    apSSP_DeviceEnable(SSP_Ptr);
    for (; len > 0; len--)
    {
        while (!apSSP_TxFifoNotFull(SSP_Ptr));
        apSSP_WriteFIFO(SSP_Ptr, *data++);
    }
}

void SPI_Fill_FIFO_16(uint16_t data, int len)
{
    apSSP_DeviceEnable(SSP_Ptr);
    for (; len > 0; len--)
    {
        while (!apSSP_TxFifoNotFull(SSP_Ptr));
        apSSP_WriteFIFO(SSP_Ptr, data);
    }
}

void SPI_Fill_FIFO_8(uint8_t data, int len)
{
    apSSP_DeviceEnable(SSP_Ptr);
    for (; len > 0; len--)
    {
        while (!apSSP_TxFifoNotFull(SSP_Ptr));
        apSSP_WriteFIFO(SSP_Ptr, data);
    }
}

void SPI_WaitTxDone(void)
{
    while (!apSSP_TxFifoEmpty(SSP_Ptr)) ;
    SPI_Delay(10);
    apSSP_DeviceDisable(SSP_Ptr);
}
#elif ((INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) ||(INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)) 

#if DMA_EN
DMA_Descriptor descriptor_spi_tx __attribute__((aligned (8)));
#define SPI_TX_CHANNEL_ID  0

void spi_dma_tx_init(void)
{
    SYSCTRL_ClearClkGate(SYSCTRL_ITEM_APB_DMA);
    DMA_PrepareMem2Peripheral(&descriptor_spi_tx, SYSCTRL_DMA_SPI0_TX, (uint32_t*)0,
        512*sizeof(uint32_t), DMA_ADDRESS_INC, 0);
}
#endif

void SPI_Init(void)
{
    apSSP_sDeviceControlBlock pParam;

    pParam.eSclkDiv = SPI_INTERFACETIMINGSCLKDIV_DEFAULT_6M;
    pParam.eSCLKPolarity = SPI_CPOL_SCLK_LOW_IN_IDLE_STATES;
    pParam.eSCLKPhase = SPI_CPHA_ODD_SCLK_EDGES;
    pParam.eLsbMsbOrder = SPI_LSB_MOST_SIGNIFICANT_BIT_FIRST;
    pParam.eDataSize = SPI_DATALEN_8_BITS;
    pParam.eMasterSlaveMode = SPI_SLVMODE_MASTER_MODE;
    pParam.eReadWriteMode = SPI_TRANSMODE_WRITE_ONLY;
    pParam.eQuadMode = SPI_DUALQUAD_REGULAR_MODE;
    pParam.eWriteTransCnt = 0;
    pParam.eReadTransCnt = 0;
    pParam.eAddrEn = SPI_ADDREN_DISABLE;
    pParam.eCmdEn = SPI_CMDEN_DISABLE;
    pParam.RxThres = 0;
    pParam.TxThres = 0;
    pParam.SlaveDataOnly = SPI_SLVDATAONLY_ENABLE;
    pParam.eAddrLen = SPI_ADDRLEN_1_BYTE;
    pParam.eMOSI_Dir = SPI_MOSI_UNI_DIR_MODE;
    pParam.eInterruptMask = 0;

    apSSP_DeviceParametersSet(SPI_PORT, &pParam); 
    
    #if DMA_EN
    spi_dma_tx_init();
    // src width byte
    descriptor_spi_tx.Ctrl &= ~(0x7<<21);
    #endif
}

void SPI_Init16bit(void)
{
    apSSP_sDeviceControlBlock pParam;   

    pParam.eSclkDiv = SPI_INTERFACETIMINGSCLKDIV_DEFAULT_6M;
    pParam.eSCLKPolarity = SPI_CPOL_SCLK_LOW_IN_IDLE_STATES;
    pParam.eSCLKPhase = SPI_CPHA_ODD_SCLK_EDGES;
    pParam.eLsbMsbOrder = SPI_LSB_MOST_SIGNIFICANT_BIT_FIRST;
    pParam.eDataSize = SPI_DATALEN_16_BITS;
    pParam.eMasterSlaveMode = SPI_SLVMODE_MASTER_MODE;
    pParam.eReadWriteMode = SPI_TRANSMODE_WRITE_ONLY;
    pParam.eQuadMode = SPI_DUALQUAD_REGULAR_MODE;
    pParam.eWriteTransCnt = 0;
    pParam.eReadTransCnt = 0;
    pParam.eAddrEn = SPI_ADDREN_DISABLE;
    pParam.eCmdEn = SPI_CMDEN_DISABLE;
    pParam.RxThres = 0;
    pParam.TxThres = 0;
    pParam.SlaveDataOnly = SPI_SLVDATAONLY_ENABLE;
    pParam.eAddrLen = SPI_ADDRLEN_1_BYTE;
    pParam.eMOSI_Dir = SPI_MOSI_UNI_DIR_MODE;
    pParam.eInterruptMask = 0;

    apSSP_DeviceParametersSet(SPI_PORT, &pParam); 
    
    #if DMA_EN
    spi_dma_tx_init();
    // src width half word
    descriptor_spi_tx.Ctrl &= ~(0x7<<21);
    descriptor_spi_tx.Ctrl |= 0x1<<21;
    #endif
    
}

/*********************************************/
//
//********************************************/
void SPI_Delay(uint32_t t)
{ 
    
    for(; t>0; t--) 
    {
        for(uint32_t i=255; i>0; i--)
            __NOP(); 
    }
}

//********************************************/
//
//
//
//********************************************/ 
void SPI_Write(uint8_t data)
{
    apSSP_SetTransferControlWrTranCnt(SPI_PORT,1);
    // send data
    apSSP_WriteFIFO(SPI_PORT, data);
    // wait data send complete
    apSSP_WriteCmd(SPI_PORT, 0, 0);
    while (!apSSP_TxFifoEmpty(SPI_PORT)) ;
    while(apSSP_GetSPIActiveStatus(SPI_PORT));
}

#if DMA_EN
void SPI_Write_FIFO_16(const uint16_t *data, int len)
{
    const uint16_t *p = data;
    int remaining = len;
    uint32_t status;
    
    apSSP_SetTxDmaEn(SPI_PORT, 1);
    // source address control increment address
    descriptor_spi_tx.Ctrl &= ~(0x3<<14);

    while (remaining > 0)
    {
        int chunk = (remaining > MAX_SPI_TRANSFER_COUNT) ? MAX_SPI_TRANSFER_COUNT : remaining;

        apSSP_SetTransferControlWrTranCnt(SPI_PORT, chunk);
        descriptor_spi_tx.SrcAddr = (uint32_t)p;
        descriptor_spi_tx.TranSize = chunk;
        DMA_EnableChannel(SPI_TX_CHANNEL_ID, &descriptor_spi_tx);
        apSSP_WriteCmd(SPI_PORT, 0, 0);
        p += chunk;
        remaining -= chunk;
        while(!(DMA_GetChannelIntState(SPI_TX_CHANNEL_ID)&DMA_IRQ_TC));
        while (!apSSP_TxFifoEmpty(SPI_PORT)) ;
        while(apSSP_GetSPIActiveStatus(SPI_PORT));
        DMA_ClearChannelIntState(SPI_TX_CHANNEL_ID, DMA_GetChannelIntState(SPI_TX_CHANNEL_ID));
    }
    
    apSSP_SetTxDmaEn(SPI_PORT, 0);
    
}

void SPI_Write_FIFO_8(const uint8_t *data, int len)
{
    const uint8_t *p = data;
    int remaining = len;
    
    apSSP_SetTxDmaEn(SPI_PORT, 1);
    // source address control increment address
    descriptor_spi_tx.Ctrl &= ~(0x3<<14);

    while (remaining > 0)
    {
        int chunk = (remaining > MAX_SPI_TRANSFER_COUNT) ? MAX_SPI_TRANSFER_COUNT : remaining;

        apSSP_SetTransferControlWrTranCnt(SPI_PORT, chunk);
        descriptor_spi_tx.SrcAddr = (uint32_t)p;
        descriptor_spi_tx.TranSize = chunk;
        DMA_EnableChannel(SPI_TX_CHANNEL_ID, &descriptor_spi_tx);
        apSSP_WriteCmd(SPI_PORT, 0, 0);
        p += chunk;
        remaining -= chunk;
        while(!(DMA_GetChannelIntState(SPI_TX_CHANNEL_ID)&DMA_IRQ_TC));
        while (!apSSP_TxFifoEmpty(SPI_PORT)) ;
        while(apSSP_GetSPIActiveStatus(SPI_PORT));
        DMA_ClearChannelIntState(SPI_TX_CHANNEL_ID, DMA_GetChannelIntState(SPI_TX_CHANNEL_ID));
    }
    
    apSSP_SetTxDmaEn(SPI_PORT, 0);
    
}

void SPI_Fill_FIFO_16(uint16_t data, int len)
{
    int remaining = len;
    
    apSSP_SetTxDmaEn(SPI_PORT, 1);
    // source address control fixed address
    descriptor_spi_tx.Ctrl &= ~(0x3<<14);
    descriptor_spi_tx.Ctrl |= (0x2<<14);

    while (remaining > 0)
    {
        int chunk = (remaining > MAX_SPI_TRANSFER_COUNT) ? MAX_SPI_TRANSFER_COUNT : remaining;

        apSSP_SetTransferControlWrTranCnt(SPI_PORT, chunk);
        descriptor_spi_tx.SrcAddr = (uint32_t)&data;
        descriptor_spi_tx.TranSize = chunk;
        DMA_EnableChannel(SPI_TX_CHANNEL_ID, &descriptor_spi_tx);
        apSSP_WriteCmd(SPI_PORT, 0, 0);
        remaining -= chunk;
        while(!(DMA_GetChannelIntState(SPI_TX_CHANNEL_ID)&DMA_IRQ_TC));
        while (!apSSP_TxFifoEmpty(SPI_PORT)) ;
        while(apSSP_GetSPIActiveStatus(SPI_PORT));
        DMA_ClearChannelIntState(SPI_TX_CHANNEL_ID, DMA_GetChannelIntState(SPI_TX_CHANNEL_ID));
    }
    
    apSSP_SetTxDmaEn(SPI_PORT, 0);
    
}

void SPI_Fill_FIFO_8(uint8_t data, int len)
{
    int remaining = len;
    
    apSSP_SetTxDmaEn(SPI_PORT, 1);
    // source address control fixed address
    descriptor_spi_tx.Ctrl &= ~(0x3<<14);
    descriptor_spi_tx.Ctrl |= (0x2<<14);
    
    while (remaining > 0)
    {
        int chunk = (remaining > MAX_SPI_TRANSFER_COUNT) ? MAX_SPI_TRANSFER_COUNT : remaining;

        apSSP_SetTransferControlWrTranCnt(SPI_PORT, chunk);
        descriptor_spi_tx.SrcAddr = (uint32_t)&data;
        descriptor_spi_tx.TranSize = chunk;
        DMA_EnableChannel(SPI_TX_CHANNEL_ID, &descriptor_spi_tx);
        apSSP_WriteCmd(SPI_PORT, 0, 0);
        remaining -= chunk;
        while(!(DMA_GetChannelIntState(SPI_TX_CHANNEL_ID)&DMA_IRQ_TC));
        while (!apSSP_TxFifoEmpty(SPI_PORT)) ;
        while(apSSP_GetSPIActiveStatus(SPI_PORT));
        DMA_ClearChannelIntState(SPI_TX_CHANNEL_ID, DMA_GetChannelIntState(SPI_TX_CHANNEL_ID));
    }
    
    apSSP_SetTxDmaEn(SPI_PORT, 0);
    
}
#else

void SPI_Write_FIFO_16(const uint16_t *data, int len)
{
    const uint16_t *p = data;
    int remaining = len;

    while (remaining > 0)
    {
        int chunk = (remaining > MAX_SPI_TRANSFER_COUNT) ? MAX_SPI_TRANSFER_COUNT : remaining;

        apSSP_SetTransferControlWrTranCnt(SPI_PORT, chunk);
        apSSP_WriteCmd(SPI_PORT, 0, 0);
        for (int i = 0; i < chunk; i++)
        {
            while (apSSP_TxFifoFull(SPI_PORT));
            apSSP_WriteFIFO(SPI_PORT, *p++);
        }
        remaining -= chunk;
        while (!apSSP_TxFifoEmpty(SPI_PORT)) ;
        while(apSSP_GetSPIActiveStatus(SPI_PORT));
    }
    
}

void SPI_Write_FIFO_8(const uint8_t *data, int len)
{
    const uint8_t *p = data;
    int remaining = len;

    while (remaining > 0)
    {
        int chunk = (remaining > MAX_SPI_TRANSFER_COUNT) ? MAX_SPI_TRANSFER_COUNT : remaining;

        apSSP_SetTransferControlWrTranCnt(SPI_PORT, chunk);
        apSSP_WriteCmd(SPI_PORT, 0, 0);
        for (int i = 0; i < chunk; i++)
        {
            while (apSSP_TxFifoFull(SPI_PORT));
            apSSP_WriteFIFO(SPI_PORT, *p++);
        }
        remaining -= chunk;
        while (!apSSP_TxFifoEmpty(SPI_PORT)) ;
        while(apSSP_GetSPIActiveStatus(SPI_PORT));
    }
    
}

void SPI_Fill_FIFO_16(uint16_t data, int len)
{
    int remaining = len;

    while (remaining > 0)
    {
        int chunk = (remaining > MAX_SPI_TRANSFER_COUNT) ? MAX_SPI_TRANSFER_COUNT : remaining;

        apSSP_SetTransferControlWrTranCnt(SPI_PORT, chunk);
        apSSP_WriteCmd(SPI_PORT, 0, 0);
        for (int i = 0; i < chunk; i++)
        {
            while (apSSP_TxFifoFull(SPI_PORT));
            apSSP_WriteFIFO(SPI_PORT, data);
        }
        remaining -= chunk;
        while (!apSSP_TxFifoEmpty(SPI_PORT)) ;
        while(apSSP_GetSPIActiveStatus(SPI_PORT));
    }
    
}

void SPI_Fill_FIFO_8(uint8_t data, int len)
{
    int remaining = len;

    while (remaining > 0)
    {
        int chunk = (remaining > MAX_SPI_TRANSFER_COUNT) ? MAX_SPI_TRANSFER_COUNT : remaining;

        apSSP_SetTransferControlWrTranCnt(SPI_PORT, chunk);
        apSSP_WriteCmd(SPI_PORT, 0, 0);
        for (int i = 0; i < chunk; i++)
        {
            while (apSSP_TxFifoFull(SPI_PORT));
            apSSP_WriteFIFO(SPI_PORT, data);
        }
        remaining -= chunk;
        while (!apSSP_TxFifoEmpty(SPI_PORT)) ;
        while(apSSP_GetSPIActiveStatus(SPI_PORT));
    }
    
}
#endif

void SPI_WaitTxDone(void)
{
    while(apSSP_GetSPIActiveStatus(SPI_PORT));
}

#endif
