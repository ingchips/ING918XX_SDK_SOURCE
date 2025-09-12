#include "spi.h"

void SPI_Init(SSP_TypeDef * SSP_Ptr)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
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
#else
    #warning WIP
#endif
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
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    // send data
    apSSP_WriteFIFO(AHB_SSP0, data);
    // wait data send complete
    apSSP_DeviceEnable(AHB_SSP0);
    while(apSSP_DeviceBusyGet(AHB_SSP0));
    apSSP_DeviceDisable(AHB_SSP0);
    // clear dummy data
    apSSP_ReadFIFO(AHB_SSP0);
#else
    #warning WIP
#endif
}
  
