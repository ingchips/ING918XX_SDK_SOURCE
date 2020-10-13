
#include "peripheral_ssp.h"

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
void apSSP_DMAModeSet(SSP_TypeDef * SSP_Ptr, apSSP_eDMAMode DMAMode)
{
    /* Set DMA enable bit */
    switch (DMAMode)
    {
        case apSSP_DMA_TX_ON:
        	  SSP_Ptr->DMAControlReg |= 1 << bsSSP_DMA_TRANSMIT_ENABLE;
            break;
        case apSSP_DMA_TX_OFF:
        	  SSP_Ptr->DMAControlReg &= ~(1 << bsSSP_DMA_TRANSMIT_ENABLE);
            break;
        case apSSP_DMA_RX_ON:
        	  SSP_Ptr->DMAControlReg |= 1 << bsSSP_DMA_RECEIVE_ENABLE;
            break;
        case apSSP_DMA_RX_OFF:
        	  SSP_Ptr->DMAControlReg &= ~(1 << bsSSP_DMA_RECEIVE_ENABLE);
            break;
    }
}

/*====================================================================*/
uint32_t apSSP_DMAAddressGet(uint32_t BaseAddr) // AHB_SPI0_BASE or APB_SPI1_BASE
{
    return BaseAddr + 0x8;
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
