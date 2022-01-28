#include "peripheral_ssp.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_918
/*
 * Description:
 * Bit shifts and widths for Control register 0 (SSPCR0)
 */
#define bsSSP_CONTROL0_CLOCKRATE        8
#define bwSSP_CONTROL0_CLOCKRATE        0xff //8
#define bsSSP_CONTROL0_SCLKPHASE        7
#define bwSSP_CONTROL0_SCLKPHASE        1
#define bsSSP_CONTROL0_SCLKPOLARITY     6
#define bwSSP_CONTROL0_SCLKPOLARITY     1
#define bsSSP_CONTROL0_FRAMEFORMAT      4
#define bwSSP_CONTROL0_FRAMEFORMAT      0x3 //2
#define bsSSP_CONTROL0_DATASIZE         0
#define bwSSP_CONTROL0_DATASIZE         0xf //4

/*
 * Description:
 * Bit shifts and widths for Control register 1 (SSPCR1)
 */
#define bsSSP_CONTROL1_SLAVEOUTPUT      3
#define bwSSP_CONTROL1_SLAVEOUTPUT      1
#define bsSSP_CONTROL1_MASTERSLAVEMODE  2
#define bwSSP_CONTROL1_MASTERSLAVEMODE  1
#define bsSSP_CONTROL1_SSPENABLE        1
#define bwSSP_CONTROL1_SSPENABLE        1
#define bsSSP_CONTROL1_LOOPBACK         0
#define bwSSP_CONTROL1_LOOPBACK         1

/*
 * Description:
 * Bit shifts and widths for Status register
 */
#define bsSSP_STATUS_BUSY           4
#define bwSSP_STATUS_BUSY           1
#define bsSSP_STATUS_RXFULL         3
#define bwSSP_STATUS_RXFULL         1
#define bsSSP_STATUS_RXNOTEMPTY     2
#define bwSSP_STATUS_RXNOTEMPTY     1
#define bsSSP_STATUS_TXNOTFULL      1
#define bwSSP_STATUS_TXNOTFULL      1
#define bsSSP_STATUS_TXEMPTY        0
#define bwSSP_STATUS_TXEMPTY        1

/*
 * Description:
 * Bit shifts and widths for Clock Prescale register
 */
#define bsSSP_CLOCKPRESCALE_CLOCKDIVISOR    0
#define bwSSP_CLOCKPRESCALE_CLOCKDIVISOR    0xff //8

/*
 * Description:
 * Bit shifts and widths for Interrupt Enables in Mask register
 */
#define bsSSP_MASK_TXINTENABLE         3
#define bwSSP_MASK_TXINTENABLE         1
#define bsSSP_MASK_RXINTENABLE         2
#define bwSSP_MASK_RXINTENABLE         1
#define bsSSP_MASK_RTMINTENABLE        1
#define bwSSP_MASK_RTMINTENABLE        1
#define bsSSP_MASK_RORINTENABLE        0
#define bwSSP_MASK_RORINTENABLE        1

/*
 * Description:
 * Bit shifts and widths for Raw and Masked Interrupt Status register
 */

#define bsSSP_INTERRUPTID_TXINT         3
#define bwSSP_INTERRUPTID_TXINT         1
#define bsSSP_INTERRUPTID_RXINT         2
#define bwSSP_INTERRUPTID_RXINT         1
#define bsSSP_INTERRUPTID_RTMINT        1
#define bwSSP_INTERRUPTID_RTMINT        1
#define bsSSP_INTERRUPTID_RORINT        0
#define bwSSP_INTERRUPTID_RORINT        1


/*
 * Descripton:
 * Bit shifts and widths for interrupt clear register
 */
#define bsSSP_INTERRUPTCLEAR_RTMINT        1
#define bwSSP_INTERRUPTCLEAR_RTMINT        1
#define bsSSP_INTERRUPTCLEAR_RORINT        0
#define bwSSP_INTERRUPTCLEAR_RORINT        1


/*
 * Descripton:
 * Bit shifts and widths for DMA Control Register
 */
#define bsSSP_DMA_TRANSMIT_ENABLE        1
#define bwSSP_DMA_TRANSMIT_ENABLE        1
#define bsSSP_DMA_RECEIVE_ENABLE         0
#define bwSSP_DMA_RECEIVE_ENABLE         1

/*====================================================================*/
void apSSP_Initialize (SSP_TypeDef * SSP_Ptr)
{
    /* Set SSP to a known state (as after Reset), disable interrupts */
    SSP_Ptr->ControlRegister1 = \
    SSP_Ptr->ControlRegister0 = \
    SSP_Ptr->ClockPreScale    = 0;

    /* Clear transmit & receive interrupt bits */
    SSP_Ptr->IntMask = 0;
}

/*====================================================================*/
void apSSP_DeviceParametersSet(SSP_TypeDef * SSP_Ptr, apSSP_sDeviceControlBlock *pParam)
{
    /* Write master/slave, slave output and loop back mode bits */
    SSP_Ptr->ControlRegister1 = ( pParam->eLoopBackMode    << bsSSP_CONTROL1_LOOPBACK        ) |
                                ( pParam->eMasterSlaveMode << bsSSP_CONTROL1_MASTERSLAVEMODE ) |
                                ( pParam->eSlaveOutput     << bsSSP_CONTROL1_SLAVEOUTPUT     );

    /* Assemble bit value from various parameters and write to register
     * Writing all 16 bits so do not need to read value in first
     */
    SSP_Ptr->ControlRegister0 = ( pParam->eDataSize        << bsSSP_CONTROL0_DATASIZE        ) |
                                ( pParam->eFrameFormat     << bsSSP_CONTROL0_FRAMEFORMAT     ) |
                                ( pParam->eSCLKPolarity    << bsSSP_CONTROL0_SCLKPOLARITY    ) |
                                ( pParam->eSCLKPhase       << bsSSP_CONTROL0_SCLKPHASE       ) |
                                ( pParam->ClockRate        << bsSSP_CONTROL0_CLOCKRATE       );

    /* Write clock prescale divisor
     * Writing all 16 bits so do not need to read value in first
     */
    SSP_Ptr->ClockPreScale =     pParam->ClockPrescale    << bsSSP_CLOCKPRESCALE_CLOCKDIVISOR;
}

/*====================================================================*/
void apSSP_DeviceParametersGet(SSP_TypeDef * SSP_Ptr, apSSP_sDeviceControlBlock *pParam)
{
    /* Get various data fields from control register 0 */
    pParam->ClockRate         = (uint8_t)                ((SSP_Ptr->ControlRegister0 >> bsSSP_CONTROL0_CLOCKRATE      ) & bwSSP_CONTROL0_CLOCKRATE      );
    pParam->eSCLKPhase        = (apSSP_eSCLKPhase)       ((SSP_Ptr->ControlRegister0 >> bsSSP_CONTROL0_SCLKPHASE      ) & bwSSP_CONTROL0_SCLKPHASE      );
    pParam->eSCLKPolarity     = (apSSP_eSCLKPolarity)    ((SSP_Ptr->ControlRegister0 >> bsSSP_CONTROL0_SCLKPOLARITY   ) & bwSSP_CONTROL0_SCLKPOLARITY   );
    pParam->eFrameFormat      = (apSSP_eFrameFormat)     ((SSP_Ptr->ControlRegister0 >> bsSSP_CONTROL0_FRAMEFORMAT    ) & bwSSP_CONTROL0_FRAMEFORMAT    );
    pParam->eDataSize         = (apSSP_eDataSize)        ((SSP_Ptr->ControlRegister0 >> bsSSP_CONTROL0_DATASIZE       ) & bwSSP_CONTROL0_DATASIZE       );

    /* Get various data fields from control register 1 */
    pParam->eLoopBackMode     = (apSSP_eLoopBackMode)    ((SSP_Ptr->ControlRegister1 >> bsSSP_CONTROL1_LOOPBACK       ) & bwSSP_CONTROL1_LOOPBACK       );
    pParam->eMasterSlaveMode  = (apSSP_eMasterSlaveMode) ((SSP_Ptr->ControlRegister1 >> bsSSP_CONTROL1_MASTERSLAVEMODE) & bwSSP_CONTROL1_MASTERSLAVEMODE);
    pParam->eSlaveOutput      = (apSSP_eSlaveOutput)     ((SSP_Ptr->ControlRegister1 >> bsSSP_CONTROL1_SLAVEOUTPUT    ) & bwSSP_CONTROL1_SLAVEOUTPUT    );

    /* Get clock prescale divisor data field */
    pParam->ClockPrescale     = (uint8_t)                ( SSP_Ptr->ClockPreScale    >> bsSSP_CLOCKPRESCALE_CLOCKDIVISOR) & bwSSP_CLOCKPRESCALE_CLOCKDIVISOR;
}

/*====================================================================*/
void apSSP_DeviceEnable(SSP_TypeDef * SSP_Ptr)
{
    SSP_Ptr->ControlRegister1 |= 1 << bsSSP_CONTROL1_SSPENABLE;
}

/*====================================================================*/
void apSSP_DeviceDisable(SSP_TypeDef * SSP_Ptr)
{
    /* Clear SSP enable bit (SSE) */
    SSP_Ptr->ControlRegister1 &= ~(1 << bsSSP_CONTROL1_SSPENABLE);
}

/*====================================================================*/
void apSSP_DeviceReceiveEnable(SSP_TypeDef * SSP_Ptr)
{
    SSP_Ptr->IntMask |= (1 << bsSSP_MASK_RXINTENABLE ) |
                        (1 << bsSSP_MASK_RORINTENABLE) |
                        (1 << bsSSP_MASK_RTMINTENABLE);
}

/*====================================================================*/
void apSSP_DeviceReceiveOverrunEnable(SSP_TypeDef * SSP_Ptr)
{
    SSP_Ptr->IntMask |= (1 << bsSSP_MASK_RORINTENABLE);
}

/*====================================================================*/
void apSSP_DeviceReceiveDisable(SSP_TypeDef * SSP_Ptr)
{
    SSP_Ptr->IntMask &= ~ ((1 << bsSSP_MASK_RXINTENABLE ) |
                          (1 << bsSSP_MASK_RORINTENABLE) |
                          (1 << bsSSP_MASK_RTMINTENABLE  ));
}

/*====================================================================*/
void apSSP_DeviceTransmitEnable(SSP_TypeDef * SSP_Ptr)
{
    SSP_Ptr->IntMask |= 1 << bsSSP_MASK_TXINTENABLE;
}

/*====================================================================*/
void apSSP_DeviceTransmitDisable(SSP_TypeDef * SSP_Ptr)
{
    SSP_Ptr->IntMask &= ~(1 << bsSSP_MASK_TXINTENABLE);
}

/*====================================================================*/
void apSSP_DeviceReceiveClear(SSP_TypeDef * SSP_Ptr)
{
    volatile uint16_t DummyData;

    /* Repeat while there's data in the SSP Receive FIFO */
    while( (SSP_Ptr->StatusRegister >> bsSSP_STATUS_RXNOTEMPTY) &  bwSSP_STATUS_RXNOTEMPTY )
    {
        /* Read data, throw it away */
        DummyData = (uint16_t) SSP_Ptr->DataRegister;
    }
}

/*====================================================================*/
uint8_t apSSP_DeviceBusyGet(SSP_TypeDef * SSP_Ptr)
{
    return (uint8_t) ((SSP_Ptr->StatusRegister >> bsSSP_STATUS_BUSY) & bwSSP_STATUS_BUSY);
}

/*====================================================================*/
void apSSP_WriteFIFO(SSP_TypeDef * SSP_Ptr, uint16_t data)
{
	// send data
	SSP_Ptr->DataRegister = data;
}

/*====================================================================*/
uint16_t apSSP_ReadFIFO(SSP_TypeDef * SSP_Ptr)
{
	uint16_t data;
	data = SSP_Ptr->DataRegister;
	return data;
}

/*====================================================================*/
uint8_t apSSP_RxFifoFull(SSP_TypeDef * SSP_Ptr)
{
	return (uint8_t) ((SSP_Ptr->StatusRegister >> bsSSP_STATUS_RXFULL) & bwSSP_STATUS_RXFULL);
}

/*====================================================================*/
uint8_t apSSP_RxFifoNotEmpty(SSP_TypeDef * SSP_Ptr)
{
	return (uint8_t) ((SSP_Ptr->StatusRegister >> bsSSP_STATUS_RXNOTEMPTY) & bwSSP_STATUS_RXNOTEMPTY);
}

/*====================================================================*/
uint8_t apSSP_TxFifoNotFull(SSP_TypeDef * SSP_Ptr)
{
	return (uint8_t) ((SSP_Ptr->StatusRegister >> bsSSP_STATUS_TXNOTFULL) & bwSSP_STATUS_TXNOTFULL);
}

/*====================================================================*/
uint8_t apSSP_TxFifoEmpty(SSP_TypeDef * SSP_Ptr)
{
	return (uint8_t) ((SSP_Ptr->StatusRegister >> bsSSP_STATUS_TXEMPTY) & bwSSP_STATUS_TXEMPTY);
}

void apSSP_ClearInt(SSP_TypeDef * SSP_Ptr, uint32_t bits)
{
    SSP_Ptr->IntClearRegister = bits;
}

uint32_t apSSP_GetIntRawStatus(SSP_TypeDef * SSP_Ptr)
{
    return SSP_Ptr->IntRawStatus;
}

#endif

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916
/*====================================================================*/
void apSSP_Initialize (SPI_REG_H *SPI_Ptr)
{
    //disable all int
    SPI_Ptr->IntrEn.r = 0;
    SPI_Ptr->TransCtrl.r = 0;
    
    //SPI reset
    SPI_Ptr->Ctrl.r |= (1 << SPI_REG_CTRL_SPIRST);
    
    //1. Read TX/RX FIFO depth in the Configuration Register
    //SPI_Ptr->Config.f.RxFIFOSize
    //SPI_Ptr->Config.f.TxFIFOSize
}

/*====================================================================*/
void apSSP_DeviceParametersSet(SPI_REG_H *SPI_Ptr, apSSP_sDeviceControlBlock *pParam)
{
    //Wait for the previous SPI transfer to finish
    while(SPI_Ptr->Status.f.SPIActive == 1);
    
    if(pParam->SlvMode == SPI_SLVMODE_MASTER_MODE)
    {
      SPI_Ptr->TransFmt.r = ( (pParam->DataMerge << SPI_REG_TRANSFMT_DATAMERGE) |
                              (pParam->CPOL << SPI_REG_TRANSFMT_CPOL) |
                              (pParam->CPHA << SPI_REG_TRANSFMT_CPHA) |
                              (pParam->LSB << SPI_REG_TRANSFMT_LSB) |
                              (pParam->MOSIBiDir << SPI_REG_TRANSFMT_MOSIBIDIR) |
                              (pParam->DataLength << SPI_REG_TRANSFMT_DATALEN) |
                              (pParam->AddressLength << SPI_REG_TRANSFMT_ADDRLEN) );

      SPI_Ptr->TransCtrl.r = (  (pParam->TransMode << SPI_REG_TRANSCTRL_TRANSMODE) |
                                (pParam->DualQuad << SPI_REG_TRANSCTRL_DUALQUAD) |
                                (pParam->WrTranCnt << SPI_REG_TRANSCTRL_WRTRANCNT) |
                                (pParam->RdTranCnt << SPI_REG_TRANSCTRL_RDTRANCNT) |
                                (pParam->PhaseCmd << SPI_REG_TRANSCTRL_CMDEN) |
                                (pParam->PhaseAddr << SPI_REG_TRANSCTRL_ADDREN) );
                                
      //Enable the End of SPI Transfer interrupt.
      SPI_Ptr->IntrEn.r = (0x1 << SPI_REG_INTREN_ENDINTEN);
    }
    else
    {
      SPI_Ptr->Ctrl.r = ((0x1 << SPI_REG_CTRL_RXFIFORST) |
                         (0x1 << SPI_REG_CTRL_TXFIFORST));
      while(SPI_Ptr->Ctrl.f.RXFIFORST == 1);
      while(SPI_Ptr->Ctrl.f.TXFIFORST == 1);
      
      SPI_Ptr->Ctrl.r |= ((pParam->RXTHRES << SPI_REG_CTRL_RXTHRES)|
                          (pParam->TXTHRES << SPI_REG_CTRL_RXTHRES));
      
      SPI_Ptr->IntrEn.r = ( (0x1 << SPI_REG_INTREN_ENDINTEN) |
                            (0x1 << SPI_REG_INTREN_RXFIFOINTEN) |
                            (0x1 << SPI_REG_INTREN_TXFIFOINTEN) |
                            (0x1 << SPI_REG_INTREN_SLVCMDEN));
    }
    
}

/*====================================================================*/
void apSSP_WriteFIFO(SPI_REG_H *SPI_Ptr, uint32_t Addr, uint32_t Cmd, uint32_t Data[], uint16_t Len)
{
    uint16_t i;
    for(i = 0; i < Len; i++){
        //Transmit FIFO Empty flag
        while(SPI_Ptr->Status.f.TXFULL == 0x1);
        //For writes, data is enqueued to the TX FIFO
        SPI_Ptr->Data.r = Data[i];
    }

    //SPI Address (Master mode only)
    SPI_Ptr->Addr.r = Addr;
    //SPI Command
    //This register must be written with a dummy value to start a SPI transfer even when the command phase is not enabled
    SPI_Ptr->Cmd.r = Cmd;
    
    //Wait the EndInt interrupt by checking the EndInt bit of the SPI Interrupt Status Register (0x3C).
    //Write one to clear the EndInt bit of the SPI Interrupt Status Register (0x3C).
    
}

/*====================================================================*/
void apSSP_DeviceReceiveEnable(SPI_REG_H *SPI_Ptr, uint32_t Addr, uint32_t Cmd)
{
    //SPI Address (Master mode only)
    SPI_Ptr->Addr.r = Addr;
    //SPI Command
    //This register must be written with a dummy value to start a SPI transfer even when the command phase is not enabled
    SPI_Ptr->Cmd.r = Cmd;
    
    //Wait the EndInt interrupt by checking the EndInt bit of the SPI Interrupt Status Register (0x3C).
    //Write one to clear the EndInt bit of the SPI Interrupt Status Register (0x3C).
}

/*====================================================================*/
void apSSP_ReadFIFO(SPI_REG_H *SPI_Ptr, uint32_t Data[], uint16_t Len)
{
    uint16_t i;
    for(i = 0; i < SPI_Ptr->Status.f.RXNUMLo; i++){
        //Receive FIFO Empty flag
        while(SPI_Ptr->Status.f.RXEMPTY == 0x1);
        //For reads, data is read and dequeued from the RX FIFO
        if(i < Len) {Data[i] = SPI_Ptr->Data.r;}
    }

}

/*====================================================================*/
uint32_t apSSP_GetIntRawStatus(SPI_REG_H *SPI_Ptr)
{
    return SPI_Ptr->IntrSt.r;
}


#endif
