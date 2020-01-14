#include "spi.h"

void SPI_Init(SSP_TypeDef * SSP_Ptr)
{
    apSSP_sDeviceControlBlock Param; 

    apSSP_Initialize(SSP_Ptr);

    /* Set Device Parameters */
    Param.ClockRate        = 0;  // sspclkout = sspclk/(ClockPrescale*(ClockRate+1))
    Param.ClockPrescale    = 2;  // Must be an even number from 2 to 254
    Param.eSCLKPhase       = apSSP_SCLKPHASE_TRAILINGEDGE;
    Param.eSCLKPolarity    = apSSP_SCLKPOLARITY_IDLELOW;
    Param.eFrameFormat     = apSSP_FRAMEFORMAT_MOTOROLASPI;
    Param.eDataSize        = apSSP_DATASIZE_8BITS;
    Param.eLoopBackMode    = apSSP_LOOPBACKOFF;
    Param.eMasterSlaveMode = apSSP_MASTER;
    Param.eSlaveOutput     = apSSP_SLAVEOUTPUTDISABLED;
    apSSP_DeviceParametersSet(SSP_Ptr, &Param);
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
    apSSP_WriteFIFO(AHB_SSP0, data);
    // wait data send complete
    apSSP_DeviceEnable(AHB_SSP0);
    while(apSSP_DeviceBusyGet(AHB_SSP0));
    apSSP_DeviceDisable(AHB_SSP0);
    // clear dummy data
    apSSP_ReadFIFO(AHB_SSP0);
}
  
