#include "spi.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
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
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

void SPI_Init(SSP_TypeDef * SPI_BASE)
{
    uint16_t data_uint_bits = 8;//8bit width of each data unit
    uint16_t data_len = 1;//1 byte
    apSSP_Initialize(SPI_BASE);

    /* Set Device Parameters */
    apSSP_SetTransferFormat(SPI_BASE, SPI_DATAMERGE_DISABLE               << bsSPI_TRANSFMT_DATAMERGE |
                                      SPI_CPOL_SCLK_LOW_IN_IDLE_STATES    << bsSPI_TRANSFMT_CPOL |
                                      SPI_CPHA_EVEN_SCLK_EDGES            << bsSPI_TRANSFMT_CPHA |
                                      SPI_LSB_MOST_SIGNIFICANT_BIT_FIRST  << bsSPI_TRANSFMT_LSB |
                                      (data_uint_bits-1)                  << bsSPI_TRANSFMT_DATALEN |
                                      SPI_ADDRLEN_1_BYTE                  << bsSPI_TRANSFMT_ADDRLEN |
                                      SPI_SLVMODE_MASTER_MODE             << bsSPI_TRANSFMT_SLVMODE);

    apSSP_SetTransferControl(SPI_BASE, SPI_TRANSMODE_WRITE_ONLY           << bsSPI_TRANSCTRL_TRANSMODE |
                                      SPI_DUALQUAD_REGULAR_MODE           << bsSPI_TRANSCTRL_DUALQUAD |
                                      (data_len-1)                        << bsSPI_TRANSCTRL_WRTRANCNT |
                                      (data_len-1)                        << bsSPI_TRANSCTRL_RDTRANCNT |
                                      SPI_ADDREN_DISABLE                  << bsSPI_TRANSCTRL_ADDREN |
                                      SPI_CMDEN_DISABLE                   << bsSPI_TRANSCTRL_CMDEN);

    apSSP_IntEnable(SPI_BASE, 1 << bsSPI_INTREN_ENDINTEN);
}

void SPI_Delay(uint8_t t)
{
    for(t=255; t>0; t--)
    {
        __NOP();
    }
}

void SPI_Write( uint8_t data)
{
    // send data
    uint32_t d[1] = {data};
    apSSP_WriteFIFO(APB_SPI, d, 1);
    // write dummy addr and cmd to trigger the writing
    apSSP_WriteCmd(APB_SPI,0x00,0x00);
}
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
#error WIP
#endif
