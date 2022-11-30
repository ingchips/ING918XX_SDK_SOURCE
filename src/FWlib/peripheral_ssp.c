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
void apSSP_Initialize (SSP_TypeDef *SPI_BASE)
{
    SPI_BASE->Ctrl |= (1 << bsSPI_CTRL_SPIRST);
    while((SPI_BASE->Ctrl >> bsSPI_CTRL_SPIRST) & bwSPI_CTRL_SPIRST);
    apSSP_ResetTxFifo(SPI_BASE);
    apSSP_ResetRxFifo(SPI_BASE);
}

void apSSP_DeviceParametersSet(SSP_TypeDef *SPI_BASE, apSSP_sDeviceControlBlock *pParam)
{

    apSSP_Initialize(SPI_BASE);

    /* Set Device Parameters */
    SPI_BASE->TransFmt =  (SPI_DATAMERGE_DISABLE     << bsSPI_TRANSFMT_DATAMERGE |
                          pParam->eSCLKPolarity     << bsSPI_TRANSFMT_CPOL |
                          pParam->eSCLKPhase        << bsSPI_TRANSFMT_CPHA |
                          pParam->eLsbMsbOrder      << bsSPI_TRANSFMT_LSB |
                          pParam->eDataSize         << bsSPI_TRANSFMT_DATALEN |
                          pParam->eMasterSlaveMode  << bsSPI_TRANSFMT_SLVMODE |
                          pParam->eAddrLen          << bsSPI_TRANSFMT_ADDRLEN);

    SPI_BASE->TransCtrl = (pParam->eReadWriteMode         << bsSPI_TRANSCTRL_TRANSMODE |
                          pParam->eQuadMode               << bsSPI_TRANSCTRL_DUALQUAD |
                          ((pParam->eWriteTransCnt)-1)    << bsSPI_TRANSCTRL_WRTRANCNT |
                          ((pParam->eReadTransCnt)-1)     << bsSPI_TRANSCTRL_RDTRANCNT |
                          pParam->eAddrEn                 << bsSPI_TRANSCTRL_ADDREN |
                          pParam->eCmdEn                  << bsSPI_TRANSCTRL_CMDEN |
                          pParam->SlaveDataOnly           << bsSPI_TRANSCTRL_SLVDATAONLY);
  
    apSSP_SetTxThres(SPI_BASE, pParam->TxThres);
    apSSP_SetRxThres(SPI_BASE, pParam->RxThres);
    apSSP_SetTimingSclkDiv(SPI_BASE, pParam->eSclkDiv);
    apSSP_IntEnable(SPI_BASE, pParam->eInterruptMask);

}
/*====================================================================*/
void apSSP_WriteFIFO(SSP_TypeDef *SPI_BASE, uint32_t Data)
{
    SPI_BASE->Data = Data;
}

/*====================================================================*/
void apSSP_WriteCmd(SSP_TypeDef *SPI_BASE, uint32_t Addr, uint32_t Cmd)
{
    SPI_BASE->Addr = Addr;
    //Writing operations on this register will trigger SPI transfers
    SPI_BASE->Cmd = Cmd;
}

/*====================================================================*/
void apSSP_ReadFIFO(SSP_TypeDef *SPI_BASE, uint32_t *Data)
{
    *Data = SPI_BASE->Data;
}

/*====================================================================*/
uint32_t apSSP_ReadCommand(SSP_TypeDef *SPI_BASE)
{
    return SPI_BASE->Cmd;
}

/*====================================================================*/
void apSSP_SetTransferFormat(SSP_TypeDef *SPI_BASE, uint32_t val, uint32_t shift, uint32_t width)
{
    SPI_BASE->TransFmt &= (~(BW2M(width) << shift));
    SPI_BASE->TransFmt |= (val << shift);
}

/*====================================================================*/
void apSSP_SetTransferControl(SSP_TypeDef *SPI_BASE, uint32_t val, uint32_t shift, uint32_t width)
{
    SPI_BASE->TransCtrl &= (~(BW2M(width) << shift));
    SPI_BASE->TransCtrl |= (val << shift);
}

/*====================================================================*/
void apSSP_SetTransferControlWrTranCnt(SSP_TypeDef *SPI_BASE, uint32_t val)
{
    SPI_BASE->TransCtrl &= (~(BW2M(bwSPI_TRANSCTRL_WRTRANCNT) << bsSPI_TRANSCTRL_WRTRANCNT));
    SPI_BASE->TransCtrl |= ((val-1) << bsSPI_TRANSCTRL_WRTRANCNT);
}

void apSSP_SetTransferControlRdTranCnt(SSP_TypeDef *SPI_BASE, uint32_t val)
{
    SPI_BASE->TransCtrl &= (~(BW2M(bwSPI_TRANSCTRL_RDTRANCNT) << bsSPI_TRANSCTRL_RDTRANCNT));
    SPI_BASE->TransCtrl |= ((val-1) << bsSPI_TRANSCTRL_RDTRANCNT);
}

/*====================================================================*/
uint8_t apSSP_GetSPIActiveStatus(SSP_TypeDef *SPI_BASE)
{
    return (uint8_t)((SPI_BASE->Status >> bsSPI_STATUS_SPIACTIVE) & BW2M(bwSPI_STATUS_SPIACTIVE));
}

/*====================================================================*/
uint32_t apSSP_GetIntRawStatus(SSP_TypeDef *SPI_BASE)
{
    return SPI_BASE->IntrSt;
}

void apSSP_ClearIntStatus(SSP_TypeDef *SPI_BASE, uint32_t val)
{
    SPI_BASE->IntrSt |= val;
}

/*====================================================================*/
void apSSP_IntEnable(SSP_TypeDef *SPI_BASE, uint32_t mask)
{
    SPI_BASE->IntrEn |= mask;
}

void apSSP_IntDisable(SSP_TypeDef *SPI_BASE, uint32_t mask)
{
    SPI_BASE->IntrEn &= ~mask;
}

/*====================================================================*/
// enable spi tx fifo dma
void apSSP_SetTxDmaEn(SSP_TypeDef *SPI_BASE, uint8_t en)
{
    SPI_BASE->Ctrl &= (~(0x1 << bsSPI_CTRL_TXDMAEN));
    SPI_BASE->Ctrl |= (en << bsSPI_CTRL_TXDMAEN);
}

void apSSP_SetRxDmaEn(SSP_TypeDef *SPI_BASE, uint8_t en)
{
    SPI_BASE->Ctrl &= (~(0x1 << bsSPI_CTRL_RXDMAEN));
    SPI_BASE->Ctrl |= (en << bsSPI_CTRL_RXDMAEN);
}

/*====================================================================*/
void apSSP_SetTxThres(SSP_TypeDef *SPI_BASE, uint8_t thres)
{
    SPI_BASE->Ctrl &= (~(BW2M(bwSPI_CTRL_TXTHRES) << bsSPI_CTRL_TXTHRES));
    SPI_BASE->Ctrl |= (thres << bsSPI_CTRL_TXTHRES);
}

/*====================================================================*/
void apSSP_SetRxThres(SSP_TypeDef *SPI_BASE, uint8_t thres)
{
    SPI_BASE->Ctrl &= (~(BW2M(bwSPI_CTRL_RXTHRES) << bsSPI_CTRL_RXTHRES));
    SPI_BASE->Ctrl |= (thres << bsSPI_CTRL_RXTHRES);
}

/*====================================================================*/
void apSSP_ResetTxFifo(SSP_TypeDef *SPI_BASE)
{
    SPI_BASE->Ctrl |= (1UL << bsSPI_CTRL_TXFIFORST);
    //It is automatically cleared to 0 after the reset operation completes.
    while((SPI_BASE->Ctrl >> bsSPI_CTRL_TXFIFORST) & BW2M(bwSPI_CTRL_TXFIFORST));
}

/*====================================================================*/
void apSSP_ResetRxFifo(SSP_TypeDef *SPI_BASE)
{
    SPI_BASE->Ctrl |= (1UL << bsSPI_CTRL_RXFIFORST);
    //It is automatically cleared to 0 after the reset operation completes.
    while((SPI_BASE->Ctrl >> bsSPI_CTRL_RXFIFORST) & BW2M(bwSPI_CTRL_RXFIFORST));
}

uint32_t apSSP_GetSpiStatus(SSP_TypeDef *SPI_BASE)
{
    return (SPI_BASE->Status );
}

/*====================================================================*/
uint8_t apSSP_RxFifoFull(SSP_TypeDef *SPI_BASE)
{
    return (uint8_t) ((SPI_BASE->Status >> bsSPI_STATUS_RXFULL) & BW2M(bwSPI_STATUS_RXFULL));
}

/*====================================================================*/
uint8_t apSSP_RxFifoEmpty(SSP_TypeDef *SPI_BASE)
{
    return (uint8_t) ((SPI_BASE->Status >> bsSPI_STATUS_RXEMPTY) & BW2M(bwSPI_STATUS_RXEMPTY));
}

/*====================================================================*/
uint8_t apSSP_TxFifoFull(SSP_TypeDef *SPI_BASE)
{
    return (uint8_t) ((SPI_BASE->Status >> bsSPI_STATUS_TXFULL) & BW2M(bwSPI_STATUS_TXFULL));
}

/*====================================================================*/
uint8_t apSSP_TxFifoEmpty(SSP_TypeDef *SPI_BASE)
{
    return (uint8_t) ((SPI_BASE->Status >> bsSPI_STATUS_TXEMPTY) & BW2M(bwSPI_STATUS_TXEMPTY));
}

/*====================================================================*/
uint8_t apSSP_GetDataNumInTxFifo(SSP_TypeDef *SPI_BASE)
{
    return (uint8_t) ( ((SPI_BASE->Status >> bsSPI_STATUS_TXNUML) & BW2M(bwSPI_STATUS_TXNUML)) |
                       (((SPI_BASE->Status >> bsSPI_STATUS_TXNUMH) & BW2M(bwSPI_STATUS_TXNUMH)) << bwSPI_STATUS_TXNUML) );
}

/*====================================================================*/
uint8_t apSSP_GetDataNumInRxFifo(SSP_TypeDef *SPI_BASE)
{
    return (uint8_t) ( ((SPI_BASE->Status >> bsSPI_STATUS_RXNUML) & BW2M(bwSPI_STATUS_RXNUML)) |
                       (((SPI_BASE->Status >> bsSPI_STATUS_RXNUMH) & BW2M(bwSPI_STATUS_TXNUMH)) << bwSPI_STATUS_RXNUML) );
}

/*====================================================================*/
void apSSP_SetTimingSclkDiv(SSP_TypeDef *SPI_BASE, uint8_t sclk_div)
{
    SPI_BASE->Timing &= (~BW2M(bwSPI_TIMING_SCLK_DIV));
    SPI_BASE->Timing |= (sclk_div << bsSPI_TIMING_SCLK_DIV);
}

/*====================================================================*/
void apSSP_SetTimingCs2Sclk(SSP_TypeDef *SPI_BASE, uint8_t cs2sclk)
{
    SPI_BASE->Timing &= (~(BW2M(bwSPI_TIMING_CS2SCLK) << bsSPI_TIMING_CS2SCLK));
    SPI_BASE->Timing |= (cs2sclk << bsSPI_TIMING_CS2SCLK);
}

/*====================================================================*/
uint8_t apSSP_GetTxFifoDepthWords(SSP_TypeDef *SPI_BASE)
{
    return SPI_FIFO_DEPTH;//words
}

uint8_t apSSP_GetRxFifoDepthWords(SSP_TypeDef *SPI_BASE)
{
    return SPI_FIFO_DEPTH;//words
}

/*====================================================================*/
uint16_t apSSP_GetSlaveTxDataCnt(SSP_TypeDef *SPI_BASE)
{
    return ( ((SPI_BASE->SlvDataCnt >> bsSPI_SLAVE_DATA_COUNT_WRITE_CNT) & BW2M(bwSPI_SLAVE_DATA_COUNT_WRITE_CNT)) );
}

uint16_t apSSP_GetSlaveRxDataCnt(SSP_TypeDef *SPI_BASE)
{
    return ( ((SPI_BASE->SlvDataCnt >> bsSPI_SLAVE_DATA_COUNT_READ_CNT) & BW2M(bwSPI_SLAVE_DATA_COUNT_READ_CNT)) );
}

#endif
