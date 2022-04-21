
#ifndef __PERIPHERAL_SSP_H__
#define __PERIPHERAL_SSP_H__


#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_918
/*
 * Description:
 * This specifies the frame format options
 */
typedef enum apSSP_xFrameFormat
{
    apSSP_FRAMEFORMAT_MOTOROLASPI       = 0x0,       /* Motorola SPI frame format */
    apSSP_FRAMEFORMAT_TISYNCHRONOUS     = 0x1,       /* TI synchronous serial frame format */
    apSSP_FRAMEFORMAT_NATIONALMICROWIRE = 0x2        /* National Microwire frame format */
} apSSP_eFrameFormat;

/*
 * Description:
 * This specifies the data size select options
 */
typedef enum apSSP_xDataSize
{
    apSSP_DATASIZE_4BITS  = 0x3,     /*  4 bit data */
    apSSP_DATASIZE_5BITS  = 0x4,     /*  5 bit data */
    apSSP_DATASIZE_6BITS  = 0x5,     /*  6 bit data */
    apSSP_DATASIZE_7BITS  = 0x6,     /*  7 bit data */
    apSSP_DATASIZE_8BITS  = 0x7,     /*  8 bit data */
    apSSP_DATASIZE_9BITS  = 0x8,     /*  9 bit data */
    apSSP_DATASIZE_10BITS = 0x9,     /* 10 bit data */
    apSSP_DATASIZE_11BITS = 0xA,     /* 11 bit data */
    apSSP_DATASIZE_12BITS = 0xB,     /* 12 bit data */
    apSSP_DATASIZE_13BITS = 0xC,     /* 13 bit data */
    apSSP_DATASIZE_14BITS = 0xD,     /* 14 bit data */
    apSSP_DATASIZE_15BITS = 0xE,     /* 15 bit data */
    apSSP_DATASIZE_16BITS = 0xF      /* 16 bit data */
} apSSP_eDataSize;

/*
 * Description:
 * This specifies the SCLK phase options. It is only applicable to
 * Motorola SPI frame format.
 */
typedef enum apSSP_xSCLKPhase
{
    apSSP_SCLKPHASE_TRAILINGEDGE = 0,   /* Data changes on trailing edge of the clock (valid on leading edge) */
    apSSP_SCLKPHASE_LEADINGEDGE  = 1    /* Data changes on leading edge of the clock (valid on trailing edge) */
} apSSP_eSCLKPhase;

/*
 * Description:
 * This specifies the SCLK polarity options. It is only applicable to
 * Motorola SPI frame format.
 */
typedef enum apSSP_xSCLKPolarity
{
    apSSP_SCLKPOLARITY_IDLELOW   = 0,   /* Clock low when idle and pulsed high */
    apSSP_SCLKPOLARITY_IDLEHIGH  = 1    /* Clock high when idle and pulsed low */
} apSSP_eSCLKPolarity;

/*
 * Description:
 * This specifies the master/slave mode options
 */
typedef enum apSSP_xMasterSlaveMode
{
    apSSP_MASTER = 0,   /* Device is Master */
    apSSP_SLAVE  = 1    /* Device is Slave  */
} apSSP_eMasterSlaveMode;

/*
 * Description:
 * This specifies the slave output mode options
 */
typedef enum apSSP_xSlaveOutput
{
    apSSP_SLAVEOUTPUTENABLED  = 0,  /* SSP may drive output in slave mode */
    apSSP_SLAVEOUTPUTDISABLED = 1   /* SSP must not drive output in slave mode */
} apSSP_eSlaveOutput;

/*
 * Description:
 * This specifies the loop back mode options
 */
typedef enum apSSP_xLoopBackMode
{
    apSSP_LOOPBACKOFF = 0,      /* Loop back off, normal serial port operation */
    apSSP_LOOPBACKON  = 1       /* Loop back on, Output of transmit
                                 * serial shifter connected to input of
                                 * receive serial shifter internally
                                 */
} apSSP_eLoopBackMode;

/*
 * Description:
 * This specifies the DMA mode options.
 */
typedef enum apSSP_xDMAMode
{
    apSSP_DMA_TX_ON = 1,
    apSSP_DMA_TX_OFF,
    apSSP_DMA_RX_ON,
    apSSP_DMA_RX_OFF
} apSSP_eDMAMode;


/*
 * Description:
 * The data structure to hold parameters to control SSP device.
 */
typedef struct apSSP_xDeviceControlBlock
{
    uint8_t ClockPrescale;           /* Clock rate prescale divisor */
    uint8_t ClockRate;               /* Clock rate divisor */
    apSSP_eSCLKPhase eSCLKPhase;
    apSSP_eSCLKPolarity eSCLKPolarity;
    apSSP_eFrameFormat eFrameFormat;
    apSSP_eDataSize eDataSize;
    apSSP_eLoopBackMode eLoopBackMode;
    apSSP_eMasterSlaveMode eMasterSlaveMode;
    apSSP_eSlaveOutput eSlaveOutput;

} apSSP_sDeviceControlBlock;


//==============================================================================================//
void apSSP_Initialize (SSP_TypeDef * SSP_Ptr);

void apSSP_DeviceParametersSet(SSP_TypeDef * SSP_Ptr, apSSP_sDeviceControlBlock *pParam);

void apSSP_DeviceParametersGet(SSP_TypeDef * SSP_Ptr, apSSP_sDeviceControlBlock *pParam);

void apSSP_DeviceEnable(SSP_TypeDef * SSP_Ptr);

uint32_t apSSP_GetIntRawStatus(SSP_TypeDef * SSP_Ptr);
void apSSP_ClearInt(SSP_TypeDef * SSP_Ptr, uint32_t bits);

void apSSP_DeviceDisable(SSP_TypeDef * SSP_Ptr);

void apSSP_DeviceReceiveEnable(SSP_TypeDef * SSP_Ptr);

void apSSP_DeviceReceiveOverrunEnable(SSP_TypeDef * SSP_Ptr);

void apSSP_DeviceReceiveDisable(SSP_TypeDef * SSP_Ptr);

void apSSP_DeviceTransmitEnable(SSP_TypeDef * SSP_Ptr);

void apSSP_DeviceTransmitDisable(SSP_TypeDef * SSP_Ptr);

void apSSP_DeviceReceiveClear(SSP_TypeDef * SSP_Ptr);

uint8_t apSSP_DeviceBusyGet(SSP_TypeDef * SSP_Ptr);

void apSSP_WriteFIFO(SSP_TypeDef * SSP_Ptr, uint16_t data);

uint16_t apSSP_ReadFIFO(SSP_TypeDef * SSP_Ptr);

uint8_t apSSP_RxFifoFull(SSP_TypeDef * SSP_Ptr);

uint8_t apSSP_RxFifoNotEmpty(SSP_TypeDef * SSP_Ptr);

uint8_t apSSP_TxFifoNotFull(SSP_TypeDef * SSP_Ptr);

uint8_t apSSP_TxFifoEmpty(SSP_TypeDef * SSP_Ptr);

#endif

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

/*
 * Description:
 * Bit shifts and widths for Transfer Format Register
 */
#define bsSPI_TRANSFMT_CPHA                 0
#define bsSPI_TRANSFMT_CPOL                 1
#define bsSPI_TRANSFMT_SLVMODE              2
#define bsSPI_TRANSFMT_LSB                  3
#define bsSPI_TRANSFMT_MOSIBIDIR            4
#define bsSPI_TRANSFMT_DATAMERGE            7
#define bsSPI_TRANSFMT_DATALEN              8
#define bsSPI_TRANSFMT_ADDRLEN              16

#define bwSPI_TRANSFMT_CPHA                 1
#define bwSPI_TRANSFMT_CPOL                 1
#define bwSPI_TRANSFMT_SLVMODE              1
#define bwSPI_TRANSFMT_LSB                  1
#define bwSPI_TRANSFMT_MOSIBIDIR            1
#define bwSPI_TRANSFMT_DATAMERGE            1
#define bwSPI_TRANSFMT_DATALEN              5
#define bwSPI_TRANSFMT_ADDRLEN              2

typedef enum
{
  SPI_CPHA_ODD_SCLK_EDGES = 0,
  SPI_CPHA_EVEN_SCLK_EDGES = 1
}SPI_TransFmt_CPHA_e;

typedef enum
{
  SPI_CPOL_SCLK_LOW_IN_IDLE_STATES = 0,
  SPI_CPOL_SCLK_HIGH_IN_IDLE_STATES = 1
}SPI_TransFmt_CPOL_e;

typedef enum
{
  SPI_SLVMODE_MASTER_MODE = 0,
  SPI_SLVMODE_SLAVE_MODE = 1
}SPI_TransFmt_SlvMode_e;

typedef enum
{
  SPI_LSB_MOST_SIGNIFICANT_BIT_FIRST = 0,
  SPI_LSB_LEAST_SIGNIFICANT_BIT_FIRST = 1
}SPI_TransFmt_LSB_e;

typedef enum
{
  SPI_MOSIBIDIR_UNI_DIRECTIONAL = 0,
  SPI_MOSIBIDIR_BI_DIRECTIONAL = 1
}SPI_TransFmt_MOSIBiDir_e;

typedef enum
{
  SPI_DATAMERGE_DISABLE = 0,
  SPI_DATAMERGE_ENABLE = 1
}SPI_TransFmt_DataMerge_e;

#define SPI_REG_TRANSFMT_DATALEN_X(x) ((x && 0x1F) - 1)

typedef enum
{
  SPI_ADDRLEN_1_BYTE = 0,
  SPI_ADDRLEN_2_BYTES = 1,
  SPI_ADDRLEN_3_BYTES = 2,
  SPI_ADDRLEN_4_BYTES = 3,
}SPI_TransFmt_AddrLen_e;


/*
 * Description:
 * Bit shifts and widths for Transfer Control Register
 */
#define bsSPI_TRANSCTRL_RDTRANCNT         0
#define bsSPI_TRANSCTRL_WRTRANCNT         12
#define bsSPI_TRANSCTRL_DUALQUAD          22
#define bsSPI_TRANSCTRL_TRANSMODE         24
#define bsSPI_TRANSCTRL_ADDREN            29
#define bsSPI_TRANSCTRL_CMDEN             30
#define bsSPI_TRANSCTRL_SLVDATAONLY       31

#define bwSPI_TRANSCTRL_RDTRANCNT         9
#define bwSPI_TRANSCTRL_WRTRANCNT         9
#define bwSPI_TRANSCTRL_DUALQUAD          2
#define bwSPI_TRANSCTRL_TRANSMODE         4
#define bwSPI_TRANSCTRL_ADDREN            1
#define bwSPI_TRANSCTRL_CMDEN             1

#define SPI_TRANSCTRL_RDTRANCNT_X(x) ((x && 0x1FF) - 1)
#define SPI_TRANSCTRL_WRTRANCNT_X(x) ((x && 0x1FF) - 1)

typedef enum
{
  SPI_DUALQUAD_REGULAR_MODE = 0,
  SPI_DUALQUAD_DUAL_IO_MODE = 1,
  SPI_DUALQUAD_QUAD_IO_MODE = 2
}SPI_TransCtrl_DualQuad_e;

typedef enum
{
  SPI_TRANSMODE_WRITE_READ_SAME_TIME = 0,
  SPI_TRANSMODE_WRITE_ONLY = 1,
  SPI_TRANSMODE_READ_ONLY = 2
}SPI_TransCtrl_TransMode_e;

typedef enum
{
  SPI_ADDREN_DISABLE = 0,
  SPI_ADDREN_ENABLE = 1
}SPI_TransCtrl_AddrEn_e;

typedef enum
{
  SPI_CMDEN_DISABLE = 0,
  SPI_CMDEN_ENABLE = 1
}SPI_TransCtrl_CmdEn_e;

typedef enum
{
  SPI_SLVDATAONLY_DISABLE = 0,
  SPI_SLVDATAONLY_ENABLE = 1
}SPI_TransCtrl_SlvDataOnly_e;


/*
 * Description:
 * Bit shifts and widths for Control Register
 */

#define bsSPI_CTRL_SPIRST           0
#define bsSPI_CTRL_RXFIFORST        1
#define bsSPI_CTRL_TXFIFORST        2
#define bsSPI_CTRL_RXTHRES          8
#define bsSPI_CTRL_TXTHRES          16

#define bwSPI_CTRL_SPIRST           1
#define bwSPI_CTRL_RXFIFORST        1
#define bwSPI_CTRL_TXFIFORST        1
#define bwSPI_CTRL_RXTHRES          8
#define bwSPI_CTRL_TXTHRES          8


#define bsSPI_INTREN_RXFIFOINTEN    2
#define bsSPI_INTREN_TXFIFOINTEN    2
#define bsSPI_INTREN_ENDINTEN       4
#define bsSPI_INTREN_SLVCMDEN       5


/*
 * Description:
 * Bit shifts and widths for Status register
 */
#define bsSPI_STATUS_SPIACTIVE           0
#define bsSPI_STATUS_RXEMPTY             14
#define bsSPI_STATUS_RXFULL              15
#define bsSPI_STATUS_TXEMPTY             22
#define bsSPI_STATUS_TXFULL              23
#define bsSPI_STATUS_TXNUML              16
#define bsSPI_STATUS_TXNUMH              28
#define bsSPI_STATUS_RXNUML              8
#define bsSPI_STATUS_RXNUMH              24

#define bwSPI_STATUS_SPIACTIVE           1
#define bwSPI_STATUS_RXEMPTY             1
#define bwSPI_STATUS_RXFULL              1
#define bwSPI_STATUS_TXEMPTY             1
#define bwSPI_STATUS_TXFULL              1
#define bwSPI_STATUS_TXNUML              6
#define bwSPI_STATUS_TXNUMH              2
#define bwSPI_STATUS_RXNUML              6
#define bwSPI_STATUS_RXNUMH              2


/*
 * Description:
 * Bit shifts and widths for Timing register
 */
#define bsSPI_TIMING_SCLK_DIV            0
#define bsSPI_TIMING_CS2SCLK             12

#define bwSPI_TIMING_SCLK_DIV            8
#define bwSPI_TIMING_CS2SCLK             2

/**
 * @brief Get SPI active status
 *
 * @param[in] SPI_BASE              base address
 */
uint8_t apSSP_GetSPIActiveStatus(SSP_TypeDef *SPI_BASE);

/**
 * @brief Reset SPI module
 *
 * @param[in] SPI_BASE              base address
 */
void apSSP_Initialize (SSP_TypeDef *SPI_BASE);

/**
 * @brief Set SPI configuration
 *
 * @param[in] SPI_BASE              base address
 */
void apSSP_SetTransferFormat(SSP_TypeDef *SPI_BASE, uint32_t val);

/**
 * @brief Set SPI configuration
 *
 * @param[in] SPI_BASE              base address
 */
void apSSP_SetTransferControl(SSP_TypeDef *SPI_BASE, uint32_t val);

/**
 * @brief Enable SPI Int
 *
 * @param[in] SPI_BASE              base address
 */
void apSSP_IntEnable(SSP_TypeDef *SPI_BASE, uint32_t mask);

/**
 * @brief Get FIFO status
 *
 * @param[in] SPI_BASE              base address
 */
uint8_t apSSP_RxFifoFull(SSP_TypeDef *SPI_BASE);
uint8_t apSSP_RxFifoEmpty(SSP_TypeDef *SPI_BASE);
uint8_t apSSP_TxFifoFull(SSP_TypeDef *SPI_BASE);
uint8_t apSSP_TxFifoEmpty(SSP_TypeDef *SPI_BASE);

/**
 * @brief Reset FIFO
 *
 * @param[in] SPI_BASE              base address
 */
void apSSP_ResetTxFifo(SSP_TypeDef *SPI_BASE);
void apSSP_ResetRxFifo(SSP_TypeDef *SPI_BASE);

/**
 * @brief Get SPI Int
 *
 * @param[in] SPI_BASE              base address
 */
uint32_t apSSP_GetIntRawStatus(SSP_TypeDef *SPI_BASE);
void apSSP_ClearIntStatus(SSP_TypeDef *SPI_BASE, uint32_t val);

/**
 * @brief Write data to FIFO
 *
 * @param[in] SPI_BASE              base address
 */
void apSSP_WriteFIFO(SSP_TypeDef *SPI_BASE, uint32_t Data[], uint16_t Len);

/**
 * @brief Get FIFO data
 *
 * @param[in] SPI_BASE              base address
 */
void apSSP_ReadFIFO(SSP_TypeDef *SPI_BASE, uint32_t Data[], uint16_t Len);

/**
 * @brief Get data in command register
 *
 * @param[in] SPI_BASE              base address
 *//**
 * @brief Get FIFO data
 *
 * @param[in] SPI_BASE              base address
 */
uint32_t apSSP_ReadCommand(SSP_TypeDef *SPI_BASE);

/**
 * @brief Get Number of valid entries in the FIFO
 *
 * @param[in] SPI_BASE              base address
 */
uint8_t apSSP_GetDataNumInTxFifo(SSP_TypeDef *SPI_BASE);
uint8_t apSSP_GetDataNumInRxFifo(SSP_TypeDef *SPI_BASE);

/**
 * @brief Write Cmd and Address and Trigger the data transfer
 *
 * @param[in] SPI_BASE              base address
 */
void apSSP_WriteCmd(SSP_TypeDef *SPI_BASE, uint32_t Addr, uint32_t Cmd);

/**
 * @brief Set sclk div
 *
 * @param[in] SPI_BASE              base address
 * @param[in] SCLK_DIV              SCLK period = (SCLK_DIV+1)*2*(Period of the SPI clock source)
 */
void apSSP_SetTimingSclkDiv(SSP_TypeDef *SPI_BASE, uint8_t sclk_div);
void apSSP_SetTimingCs2Sclk(SSP_TypeDef *SPI_BASE, uint8_t cs2sclk);

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

