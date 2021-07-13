
#ifndef __PERIPHERAL_SSP_H__
#define __PERIPHERAL_SSP_H__


#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"

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

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

