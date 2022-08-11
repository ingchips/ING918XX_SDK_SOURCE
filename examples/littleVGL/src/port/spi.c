#include "spi.h"
#include "ingsoc.h"

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
void SPI_Delay(uint8_t t)
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