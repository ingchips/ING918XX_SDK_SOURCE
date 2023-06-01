
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

/* same depth for both RX FIFO and TX FIFO */
#define SPI_FIFO_DEPTH (8)//8 words

/* ----------------------------------------------------------
 * Description:
 * Bit shifts and widths for Transfer Format Register "TransFmt"
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

/* SPI Master/Slave mode selection 0x0: Master mode 0x1: Slave mode */
typedef enum
{
  SPI_SLVMODE_MASTER_MODE = 0,
  SPI_SLVMODE_SLAVE_MODE = 1
}SPI_TransFmt_SlvMode_e;

/* Transfer data with the least significant bit first */
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
  SPI_ADDRLEN_4_BYTES = 3
}SPI_TransFmt_AddrLen_e;

/* The length of each data unit in bits
  The actual bit number of a data unit is (DataLen + 1) */
typedef enum
{
  SPI_DATALEN_8_BITS = 0x7,
  SPI_DATALEN_16_BITS = 0xf,
  SPI_DATALEN_32_BITS = 0x1f
}SPI_TransFmt_DataLen_e;

/* ----------------------------------------------------------
 * Description:
 * Bit shifts and widths for Transfer Control Register "TransCtrl"
 */
#define bsSPI_TRANSCTRL_RDTRANCNT         0
#define bsSPI_TRANSCTRL_DUMMYCNT          9
#define bsSPI_TRANSCTRL_WRTRANCNT         12
/* SPI_TransCtrl_DualQuad_e */
#define bsSPI_TRANSCTRL_DUALQUAD          22
/* SPI_TransCtrl_TransMode_e */
#define bsSPI_TRANSCTRL_TRANSMODE         24
#define bsSPI_TRANSCTRL_ADDRFMT           28
#define bsSPI_TRANSCTRL_ADDREN            29
#define bsSPI_TRANSCTRL_CMDEN             30
#define bsSPI_TRANSCTRL_SLVDATAONLY       31

#define bwSPI_TRANSCTRL_DUMMYCNT          2
#define bwSPI_TRANSCTRL_RDTRANCNT         9
#define bwSPI_TRANSCTRL_WRTRANCNT         9
#define bwSPI_TRANSCTRL_DUALQUAD          2
#define bwSPI_TRANSCTRL_TRANSMODE         4
#define bwSPI_TRANSCTRL_ADDRFMT           1
#define bwSPI_TRANSCTRL_ADDREN            1
#define bwSPI_TRANSCTRL_CMDEN             1

#define SPI_TRANSCTRL_RDTRANCNT_X(x) ((x && 0x1FF) - 1)
#define SPI_TRANSCTRL_WRTRANCNT_X(x) ((x && 0x1FF) - 1)

/* bit width 9 bit */
typedef uint16_t SPI_TransCtrl_TransCnt;

/* SPI data phase format */
typedef enum
{
  SPI_DUALQUAD_REGULAR_MODE = 0,
  SPI_DUALQUAD_DUAL_IO_MODE = 1,
  SPI_DUALQUAD_QUAD_IO_MODE = 2
}SPI_TransCtrl_DualQuad_e;

/* Transfer mode */
typedef enum
{
  SPI_TRANSMODE_WRITE_READ_SAME_TIME = 0,
  SPI_TRANSMODE_WRITE_ONLY = 1,
  SPI_TRANSMODE_READ_ONLY = 2,
  SPI_TRANSMODE_WRITE_READ = 3,
  SPI_TRANSMODE_READ_WRITE = 4,
  SPI_TRANSMODE_WRITE_DUMMY_READ = 5,
  SPI_TRANSMODE_READ_DUMMY_WRITE = 6,
  SPI_TRANSMODE_DUMMY_WRITE = 8,
  SPI_TRANSMODE_DUMMY_READ = 9
}SPI_TransCtrl_TransMode_e;

/* SPI address phase enable (Master mode only) 0x0: Disable the address phase
0x1: Enable the address phase */
typedef enum
{
  SPI_ADDREN_DISABLE = 0,
  SPI_ADDREN_ENABLE = 1
}SPI_TransCtrl_AddrEn_e;

/* SPI command phase enable (Master mode only) 0x0: Disable the command phase
0x1: Enable the command phase */
typedef enum
{
  SPI_CMDEN_DISABLE = 0,
  SPI_CMDEN_ENABLE = 1
}SPI_TransCtrl_CmdEn_e;

/* Data-only mode (slave mode only) 0x0: Disable the data-only mode 0x1: Enable the data-only mode */
typedef enum
{
  SPI_SLVDATAONLY_DISABLE = 0,
  SPI_SLVDATAONLY_ENABLE = 1
}SPI_TransCtrl_SlvDataOnly_e;

/* SPI_ADDRFMT_SINGLE_MODE: regular mode, addr is transfered only on MOSI with SPI_TransFmt_AddrLen_e cycle
   SPI_ADDRFMT_QUAD_MODE: addr is transfered as quad mode */
typedef enum
{
  SPI_ADDRFMT_SINGLE_MODE = 0,
  SPI_ADDRFMT_QUAD_MODE = 1
}SPI_TransCtrl_AddrFmt_e;

/* ----------------------------------------------------------
 * Description:
 * Bit shifts and widths for Control Register "Ctrl"
 */

/* SPI reset
Write 1 to reset. It is automatically cleared to 0 after the reset operation completes. */
#define bsSPI_CTRL_SPIRST           0
/* Receive FIFO reset
Write 1 to reset. It is automatically cleared to 0 after the reset operation completes. */
#define bsSPI_CTRL_RXFIFORST        1
/* Transmit FIFO reset
Write 1 to reset. It is automatically cleared to 0 after the reset operation completes. */
#define bsSPI_CTRL_TXFIFORST        2
/* DMA enable */
#define bsSPI_CTRL_RXDMAEN          3
#define bsSPI_CTRL_TXDMAEN          4
/* Receive (RX) FIFO Threshold
The RXFIFOInt interrupt generate when the RX data count is more than or equal to the RX FIFO threshold. */
#define bsSPI_CTRL_RXTHRES          8
/* Transmit (TX) FIFO Threshold
The TXFIFOInt interrupt generate when the TX data count is less than or equal to the TX FIFO threshold. */
#define bsSPI_CTRL_TXTHRES          16

/* bit width */
#define bwSPI_CTRL_SPIRST           1
#define bwSPI_CTRL_RXFIFORST        1
#define bwSPI_CTRL_TXFIFORST        1
#define bwSPI_CTRL_RXTHRES          8
#define bwSPI_CTRL_TXTHRES          8

typedef uint8_t SPI_ControlRxThres;
typedef uint8_t SPI_ControlTxThres;
/* ----------------------------------------------------------
 * Description:
 * Bit shifts and widths for Int Register "IntrEn"
 */
 
/* Enable the SPI Receive FIFO Threshold interrupt. Control whether interrupts are triggered when the valid entries are greater than or equal to the RX FIFO
threshold. */
#define bsSPI_INTREN_RXFIFOINTEN    2
/* Enable the SPI Transmit FIFO Threshold interrupt. Control whether interrupts are triggered when the valid entries are less than or equal to the TX FIFO
threshold. */
#define bsSPI_INTREN_TXFIFOINTEN    3
/* Enable the End of SPI Transfer interrupt.
Control whether interrupts are triggered when SPI transfers end. */
#define bsSPI_INTREN_ENDINTEN       4
/* Enable the Slave Command Interrupt.
Control whether interrupts are triggered whenever slave commands are received.
(Slave mode only) */
#define bsSPI_INTREN_SLVCMDEN       5

typedef uint8_t SPI_InterruptEnableMask;/* bit mask combined from above interrupt bit */

/* ----------------------------------------------------------
 * Description:
 * Bit shifts and widths for Status register "Status"
 */
/* SPI register programming is in progress.
In master mode, SPIActive becomes 1 after the SPI command register is written and becomes 0 after the transfer is finished.
In slave mode, SPIActive becomes 1 after the SPI CS signal is asserted and becomes 0 after the SPI CS signal is deasserted. */
#define bsSPI_STATUS_SPIACTIVE           0
/* Receive FIFO Empty flag */
#define bsSPI_STATUS_RXEMPTY             14
/* Receive FIFO Full flag */
#define bsSPI_STATUS_RXFULL              15
/* Transmit FIFO Empty flag */
#define bsSPI_STATUS_TXEMPTY             22
/* Transmit FIFO Full flag */
#define bsSPI_STATUS_TXFULL              23
/* Number of valid entries in the Transmit FIFO[5:0], bit width is bwSPI_STATUS_TXNUML */
#define bsSPI_STATUS_TXNUML              16
/* Number of valid entries in the Transmit FIFO[7:6], bit width is bwSPI_STATUS_TXNUMH */
#define bsSPI_STATUS_TXNUMH              28
/* Number of valid entries in the Receive FIFO[5:0] */
#define bsSPI_STATUS_RXNUML              8
/* Number of valid entries in the Receive FIFO[7:6] */
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

/* ----------------------------------------------------------
 * Description:
 * Bit shifts and widths for Configuration register
 */
 /* Depth of TX/RX FIFO */
#define bsSPI_CONFIG_RXFIFOSIZE          0
#define bsSPI_CONFIG_TXFIFOSIZE          4

#define bwSPI_CONFIG_RXFIFOSIZE          4
#define bwSPI_CONFIG_TXFIFOSIZE          4

/* ----------------------------------------------------------
 * Description:
 * Bit shifts and widths for Timing register "Timing"
 */

#define bsSPI_TIMING_SCLK_DIV            0
/* The minimum time between the edges of SPI CS and the edges of SCLK. see bwSPI_TIMING_CS2SCLK for bit width*/
#define bsSPI_TIMING_CS2SCLK             12

#define bwSPI_TIMING_SCLK_DIV            8
#define bwSPI_TIMING_CS2SCLK             2

/* several options of spi clock */

/* default clk config for spi0 and spi1
   for default, spi interface clock is 24M, use "spi interface clock / (2 * (eSclkDiv + 1))" for calculation 
   for example, "eSclkDiv == 1" means 24M/(2*(1+1)) = 6M(spi clk speed)*/
#define SPI_INTERFACETIMINGSCLKDIV_DEFAULT_6M    (1)
#define SPI_INTERFACETIMINGSCLKDIV_DEFAULT_4M    (2)
#define SPI_INTERFACETIMINGSCLKDIV_DEFAULT_3M    (3)
#define SPI_INTERFACETIMINGSCLKDIV_DEFAULT_2M4   (4)
#define SPI_INTERFACETIMINGSCLKDIV_DEFAULT_2M    (5)

/* high speed SPI1 clk config 
   1. SPI1 use HCLK, use SYSCTRL_SelectHClk() and SYSCTRL_SelectSpiClk() to increase spi interface clock
   for instance, below config would setup a spi interface clock for 84M(if pll clock is 336M), use SYSCTRL_GetClk() to confirm
    SYSCTRL_SelectHClk(SYSCTRL_CLK_PLL_DIV_1+3);//setup hclk, 336/4 = 84M
    SYSCTRL_SelectSpiClk(SPI_PORT_1,SYSCTRL_CLK_HCLK);//switch spi clock to hclk
   this is only an example, use API "SYSCTRL_GetClk(SYSCTRL_ITEM_APB_SPI1)" to check the real spi interface clock
   2. again, use "spi interface clock / (2 * (eSclkDiv + 1))" for calculation 
   for example, "eSclkDiv == 1" means 84M/(2*(1+1)) = 21M(spi clk speed)
#define SPI_INTERFACETIMINGSCLKDIV_SPI1_21M    (1)
#define SPI_INTERFACETIMINGSCLKDIV_SPI1_14M    (2)
*/

/* high speed SPI0 clk config 
   1. for SPI0, use SYSCTRL_GetPLLClk() to check the source clock, then use below API to config spi interface clock
   //for say, pll clock is 336M, then below api would generate a spi interface clock of 336/4 = 84M
   SYSCTRL_SelectSpiClk(SPI_PORT_0,SYSCTRL_CLK_PLL_DIV_1+3);
   2. again, use "spi interface clock / (2 * (eSclkDiv + 1))" for calculation 
   for example, "eSclkDiv == 1" means 84M/(2*(1+1)) = 21M(spi clk speed)
#define SPI_INTERFACETIMINGSCLKDIV_SPI0_21M    (1)
#define SPI_INTERFACETIMINGSCLKDIV_SPI0_14M    (2)
*/

typedef uint8_t  SPI_InterfaceTimingSclkDiv;// spi interface clock / (2 * (eSclkDiv + 1))

/* ----------------------------------------------------------
 * Description:
 * Bit shifts and widths for Configuration register
 */
 /* Depth of TX/RX FIFO */

#define bsSPI_SLAVE_DATA_COUNT_READ_CNT           0
#define bsSPI_SLAVE_DATA_COUNT_WRITE_CNT          16

#define bwSPI_SLAVE_DATA_COUNT_READ_CNT           10
#define bwSPI_SLAVE_DATA_COUNT_WRITE_CNT          10

/* ----------------------------------------------------------
 * Description:
 * SPI memory access control register
 */

#define bsSPI_MEMORY_ACCESS_CONTROL_RD_CMD           0
#define bwSPI_MEMORY_ACCESS_CONTROL_RD_CMD           4

typedef enum
{
  SPI_MEMRD_CMD_03 = 0 ,//read command 0x03 + 3bytes address(regular mode) + data (regular mode)
  SPI_MEMRD_CMD_0B = 1 ,//read command 0x0B + 3bytes address(regular mode) + 1byte dummy + data (regular mode)
  SPI_MEMRD_CMD_3B = 2 ,//read command 0x3B + 3bytes address(regular mode) + 1byte dummy + data (dual mode)
  SPI_MEMRD_CMD_6B = 3 ,//read command 0x6B + 3bytes address(regular mode) + 1byte dummy + data (quad mode)
  SPI_MEMRD_CMD_BB = 4 ,//read command 0xBB + 3bytes + 1byte 0 address(dual mode) + data (dual mode)
  SPI_MEMRD_CMD_EB = 5 ,//read command 0xEB + 3bytes + 1byte 0 address(quad mode) + 2bytes dummy + data (quad mode)
  SPI_MEMRD_CMD_13 = 8 ,//read command 0x13 + 4bytes address(regular mode) + data (regular mode)
  SPI_MEMRD_CMD_0C = 9 ,//read command 0x0C + 4bytes address(regular mode) + 1byte dummy + data (regular mode)
  SPI_MEMRD_CMD_3C = 10,//read command 0x3C + 4bytes address(regular mode) + 1byte dummy + data (dual mode)
  SPI_MEMRD_CMD_6C = 11,//read command 0x6C + 4bytes address(regular mode) + 1byte dummy + data (quad mode)
  SPI_MEMRD_CMD_BC = 12,//read command 0xBC + 4bytes + 1byte 0 address(dual mode) + data (dual mode)
  SPI_MEMRD_CMD_EC = 13,//read command 0xEC + 4bytes + 1byte 0 address(quad mode) + 2bytes dummy + data (quad mode)
}apSSP_sDeviceMemRdCmd;

/*
 * Description:
 * The data structure to hold parameters to control SSP device.
 */
typedef struct apSSP_xDeviceControlBlock
{
  SPI_InterfaceTimingSclkDiv   eSclkDiv; /* Clock rate divisor */
  SPI_TransFmt_CPHA_e          eSCLKPhase;/* odd or even edge, used to for spi mode */
  SPI_TransFmt_CPOL_e          eSCLKPolarity;/* idle low or idle high, used to for spi mode */
  /* Transfer data with the least significant bit first 0x0: Most significant bit first 0x1: Least significant bit first */
  SPI_TransFmt_LSB_e           eLsbMsbOrder;
  /* The length of each data unit in bits The actual bit number of a data unit is (DataLen + 1) */
  SPI_TransFmt_DataLen_e       eDataSize;
  /* SPI Master/Slave mode selection 0x0: Master mode 0x1: Slave mode */
  SPI_TransFmt_SlvMode_e       eMasterSlaveMode;
  /* Transfer mode 0x0: Write and read at the same time 0x1: Write only 0x2: Read only */
  SPI_TransCtrl_TransMode_e    eReadWriteMode;
  /* SPI data phase format 0x0: Regular (Single) mode 0x2: Quad I/O mode */
  SPI_TransCtrl_DualQuad_e     eQuadMode;
  /* Transfer count for write/read data, eg. WrTranCnt indicates the number of units of data to be transmitted */
  SPI_TransCtrl_TransCnt       eWriteTransCnt;/* 9 bit width */
  SPI_TransCtrl_TransCnt       eReadTransCnt;/* 9 bit width */
  /* SPI address phase enable (Master mode only) 0x0: Disable the address phase 0x1: Enable the address phase */
  SPI_TransCtrl_AddrEn_e       eAddrEn;
  /* SPI command phase enable (Master mode only) 0x0: Disable the command phase 0x1: Enable the command phase */
  SPI_TransCtrl_CmdEn_e        eCmdEn;
  /* check bit definition of Int Register */
  SPI_InterruptEnableMask      eInterruptMask;
  /* Transmit (TX) FIFO Threshold, Interrupt will be triggered if TX data cnt less than TxThres*/
  SPI_ControlTxThres           TxThres;
  /* Receive (RX) FIFO Threshold, Interrupt will be triggered if RX data cnt exceed RxThres*/
  SPI_ControlRxThres           RxThres;
  /* Data-only mode (slave mode only) 0x0: Disable the data-only mode 0x1: Enable the data-only mode */
  SPI_TransCtrl_SlvDataOnly_e  SlaveDataOnly;
  SPI_TransFmt_AddrLen_e       eAddrLen;
} apSSP_sDeviceControlBlock;

/**
 * @brief Setup SPI module
 *
 * @param[in] SPI_BASE              base address
 * @param[in] pParam                various config items, see definition of apSSP_sDeviceControlBlock
 */
void apSSP_DeviceParametersSet(SSP_TypeDef *SPI_BASE, apSSP_sDeviceControlBlock *pParam);

/**
 * @brief Get SPI active status, 1 represents SPI transfer is still ongoing
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
 * @brief Set SPI configuration, register value for "TransFmt" reg, see apSSP_DeviceParametersSet for reference
 *        eg. apSSP_SetTransferFormat(APB_SSP1, 1, bsSPI_TRANSFMT_CPHA, bwSPI_TRANSFMT_CPHA);
 * @param[in] SPI_BASE              base address
 */
void apSSP_SetTransferFormat(SSP_TypeDef *SPI_BASE, uint32_t val, uint32_t shift, uint32_t width);

/**
 * @brief Set SPI configuration, register value for "TransCtrl" reg, see apSSP_DeviceParametersSet for reference
 *        eg. apSSP_SetTransferControl(APB_SSP1, 1, bsSPI_TRANSCTRL_RDTRANCNT, bwSPI_TRANSCTRL_RDTRANCNT);
 * @param[in] SPI_BASE              base address
 */
void apSSP_SetTransferControl(SSP_TypeDef *SPI_BASE, uint32_t val, uint32_t shift, uint32_t width);

/**
 * @brief Set SPI configuration, see apSSP_DeviceParametersSet for reference
 *
 * @param[in] SPI_BASE              base address
 * @param[in] val                   write/read cnt number for one spi transfer(CS down and up)
 */
void apSSP_SetTransferControlWrTranCnt(SSP_TypeDef *SPI_BASE, uint32_t val);
void apSSP_SetTransferControlRdTranCnt(SSP_TypeDef *SPI_BASE, uint32_t val);
/**
 * @brief Enable SPI Int, see apSSP_DeviceParametersSet for reference
 *
 * @param[in] SPI_BASE              base address
 * @param[in] mask                  see description of reg "IntrEn"
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
 * @brief Get SPI Int status
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
void apSSP_WriteFIFO(SSP_TypeDef *SPI_BASE, uint32_t Data);

/**
 * @brief Get FIFO data
 *
 * @param[in] SPI_BASE              base address
 */
void apSSP_ReadFIFO(SSP_TypeDef *SPI_BASE, uint32_t *Data);

/**
 * @brief Get data in command register
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

/**
 * @brief Set fifo threshold for fifo interrupt
 *
 * @param[in] SPI_BASE              base address
 * @param[in] thres                 bit width is 8 bit
 */
void apSSP_SetTxThres(SSP_TypeDef *SPI_BASE, uint8_t thres);
void apSSP_SetRxThres(SSP_TypeDef *SPI_BASE, uint8_t thres);

/**
 * @brief Enable spi dma
 *
 * @param[in] SPI_BASE              base address
 * @param[in] en                    enable: 1; disable: 0
 */
void apSSP_SetTxDmaEn(SSP_TypeDef *SPI_BASE, uint8_t en);
void apSSP_SetRxDmaEn(SSP_TypeDef *SPI_BASE, uint8_t en);

/**
 * @brief Only for slave, to get data cnt info
 *
 * @param[in] SPI_BASE              base address
 */
uint16_t apSSP_GetSlaveTxDataCnt(SSP_TypeDef *SPI_BASE);
uint16_t apSSP_GetSlaveRxDataCnt(SSP_TypeDef *SPI_BASE);

/**
 * @brief Get FIFO depth, return is fixed 8 words
 *
 * @param[in] SPI_BASE              base address
 */
uint8_t apSSP_GetTxFifoDepthWords(SSP_TypeDef *SPI_BASE);
uint8_t apSSP_GetRxFifoDepthWords(SSP_TypeDef *SPI_BASE);

/**
 * @brief Set dummy cnt, only applies to mode 5,6,8,9 of SPI_TransCtrl_TransMode_e
 *
 * @param[in] SPI_BASE              base address
 * @param[in] cnt                   the final dummy cycle = (cnt+1)/(data len / io width)
 *                                  data len: pParam.eDataSize, io width: 1(regular),4(quad)
 */
void apSSP_SetTransferControlDummyCnt(SSP_TypeDef *SPI_BASE, uint8_t cnt);

/**
 * @brief Set addr format, can only be one of SPI_TransCtrl_AddrFmt_e, only applies to master
 *
 * @param[in] SPI_BASE              base address
 * @param[in] fmt                   default is SPI_ADDRFMT_SINGLE_MODE
 */
void apSSP_SetTransferControlAddrFmt(SSP_TypeDef *SPI_BASE, SPI_TransCtrl_AddrFmt_e fmt);

/**
 * @brief Set memory directly read cmd
 *
 * @param[in] SPI_BASE              base address, only apply to SPI0
 * @param[in] cmd                   refer to apSSP_sDeviceMemRdCmd for different cmd format
 */
void apSSP_SetMemAccessCmd(SSP_TypeDef *SPI_BASE, apSSP_sDeviceMemRdCmd cmd);
#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

