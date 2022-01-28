
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

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t RevMinor                         : 4 ;
        volatile uint32_t RevMajor                         : 8 ;
        volatile uint32_t ID                               : 20;
    }f;
}SPI_REG_IdRev;


#define SPI_REG_TRANSFMT_CPHA       (0)
#define SPI_REG_TRANSFMT_CPOL       (1)
#define SPI_REG_TRANSFMT_LSB        (3)
#define SPI_REG_TRANSFMT_MOSIBIDIR  (4)
#define SPI_REG_TRANSFMT_DATAMERGE  (7)
#define SPI_REG_TRANSFMT_DATALEN    (8)
#define SPI_REG_TRANSFMT_ADDRLEN    (16)

typedef enum
{
  SPI_CPHA_ODD_SCLK_EDGES = 0,
  SPI_CPHA_EVEN_SCLK_EDGES = 1
}SPI_REG_TransFmt_CPHA_e;

typedef enum
{
  SPI_CPOL_SCLK_LOW_IN_IDLE_STATES = 0,
  SPI_CPOL_SCLK_HIGH_IN_IDLE_STATES = 1
}SPI_REG_TransFmt_CPOL_e;

typedef enum
{
  SPI_SLVMODE_MASTER_MODE = 0,
  SPI_SLVMODE_SLAVE_MODE = 1
}SPI_REG_TransFmt_SlvMode_e;

typedef enum
{
  SPI_LSB_MOST_SIGNIFICANT_BIT_FIRST = 0,
  SPI_LSB_LEAST_SIGNIFICANT_BIT_FIRST = 1
}SPI_REG_TransFmt_LSB_e;

typedef enum
{
  SPI_MOSIBIDIR_UNI_DIRECTIONAL = 0,
  SPI_MOSIBIDIR_BI_DIRECTIONAL = 1
}SPI_REG_TransFmt_MOSIBiDir_e;

typedef enum
{
  SPI_DATAMERGE_DISABLE = 0,
  SPI_DATAMERGE_ENABLE = 1
}SPI_REG_TransFmt_DataMerge_e;

#define SPI_REG_TRANSFMT_DATALEN_X(x) ((x && 0x1F) - 1)

typedef enum
{
  SPI_ADDRLEN_1_BYTE = 0,
  SPI_ADDRLEN_2_BYTES = 1,
  SPI_ADDRLEN_3_BYTES = 2,
  SPI_ADDRLEN_4_BYTES = 3,
}SPI_REG_TransFmt_AddrLen_e;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t CPHA                             : 1 ;
        volatile uint32_t CPOL                             : 1 ;
        volatile uint32_t SlvMode                          : 1 ;
        volatile uint32_t LSB                              : 1 ;
        volatile uint32_t MOSIBiDir                        : 1 ;
        volatile uint32_t unused0                          : 2 ;
        volatile uint32_t DataMerge                        : 1 ;
        volatile uint32_t DataLen                          : 5 ;
        volatile uint32_t unused1                          : 3 ;
        volatile uint32_t AddrLen                          : 2 ;
        volatile uint32_t unused2                          : 14;
    }f;
}SPI_REG_TransFmt;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t CS_I                             : 1 ;
        volatile uint32_t SCLK_I                           : 1 ;
        volatile uint32_t MOSI_I                           : 1 ;
        volatile uint32_t MISO_I                           : 1 ;
        volatile uint32_t WP_I                             : 1 ;
        volatile uint32_t HOLD_I                           : 1 ;
        volatile uint32_t unused3                          : 2 ;
        volatile uint32_t CS_O                             : 1 ;
        volatile uint32_t SCLK_O                           : 1 ;
        volatile uint32_t MOSI_O                           : 1 ;
        volatile uint32_t MISO_O                           : 1 ;
        volatile uint32_t WP_O                             : 1 ;
        volatile uint32_t HOLD_O                           : 1 ;
        volatile uint32_t unused4                          : 2 ;
        volatile uint32_t CS_OE                            : 1 ;
        volatile uint32_t SCLK_OE                          : 1 ;
        volatile uint32_t MOSI_OE                          : 1 ;
        volatile uint32_t MISO_OE                          : 1 ;
        volatile uint32_t WP_OE                            : 1 ;
        volatile uint32_t HOLD_OE                          : 1 ;
        volatile uint32_t unused5                          : 2 ;
        volatile uint32_t DirectIOEn                       : 1 ;
        volatile uint32_t unused6                          : 7 ;
    }f;
}SPI_REG_DirectIO;

#define SPI_REG_TRANSCTRL_RDTRANCNT (0)
#define SPI_REG_TRANSCTRL_WRTRANCNT (12)
#define SPI_REG_TRANSCTRL_DUALQUAD  (22)
#define SPI_REG_TRANSCTRL_TRANSMODE (24)
#define SPI_REG_TRANSCTRL_ADDREN    (29)
#define SPI_REG_TRANSCTRL_CMDEN     (30)

#define SPI_REG_TRANSCTRL_RDTRANCNT_X(x) ((x && 0x1FF) - 1)
#define SPI_REG_TRANSCTRL_WRTRANCNT_X(x) ((x && 0x1FF) - 1)

typedef enum
{
  SPI_DUALQUAD_REGULAR_MODE = 0,
  SPI_DUALQUAD_DUAL_IO_MODE = 1,
  SPI_DUALQUAD_QUAD_IO_MODE = 2
}SPI_REG_TransCtrl_DualQuad_e;

typedef enum
{
  SPI_TRANSMODE_WRITE_READ_SAME_TIME = 0,
  SPI_TRANSMODE_WRITE_ONLY = 1,
  SPI_TRANSMODE_READ_ONLY = 2
}SPI_REG_TransCtrl_TransMode_e;

typedef enum
{
  SPI_ADDREN_DISABLE = 0,
  SPI_ADDREN_ENABLE = 1
}SPI_REG_TransCtrl_AddrEn_e;

typedef enum
{
  SPI_CMDEN_DISABLE = 0,
  SPI_CMDEN_ENABLE = 1
}SPI_REG_TransCtrl_CmdEn_e;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t RdTranCnt                        : 9 ;
        volatile uint32_t DummyCnt                         : 2 ;
        volatile uint32_t TokenValue                       : 1 ;
        volatile uint32_t WrTranCnt                        : 9 ;
        volatile uint32_t TokenEn                          : 1 ;
        volatile uint32_t DualQuad                         : 2 ;
        volatile uint32_t TransMode                        : 4 ;
        volatile uint32_t AddrFmt                          : 1 ;
        volatile uint32_t AddrEn                           : 1 ;
        volatile uint32_t CmdEn                            : 1 ;
        volatile uint32_t SlvDataOnly                      : 1 ;
    }f;
}SPI_REG_TransCtrl;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t CMD                              : 8 ;
        volatile uint32_t unused7                          : 24;
    }f;
}SPI_REG_Cmd;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t ADDR                             : 32;
    }f;
}SPI_REG_Addr;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t DATA                             : 32;
    }f;
}SPI_REG_Data;

#define SPI_REG_CTRL_SPIRST (0)
#define SPI_REG_CTRL_RXFIFORST (1)
#define SPI_REG_CTRL_TXFIFORST (2)
#define SPI_REG_CTRL_RXTHRES (8)
#define SPI_REG_CTRL_TXTHRES (16)

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t SPIRST                           : 1 ;
        volatile uint32_t RXFIFORST                        : 1 ;
        volatile uint32_t TXFIFORST                        : 1 ;
        volatile uint32_t RXDMAEN                          : 1 ;
        volatile uint32_t TXDMAEN                          : 1 ;
        volatile uint32_t unused8                          : 3 ;
        volatile uint32_t RXTHRES                          : 8 ;
        volatile uint32_t TXTHRES                          : 8 ;
        volatile uint32_t unused9                          : 8 ;
    }f;
}SPI_REG_Ctrl;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t SPIActive                        : 1 ;
        volatile uint32_t unused10                         : 7 ;
        volatile uint32_t RXNUMLo                          : 6 ;
        volatile uint32_t RXEMPTY                          : 1 ;
        volatile uint32_t RXFULL                           : 1 ;
        volatile uint32_t TXNUMLo                          : 6 ;
        volatile uint32_t TXEMPTY                          : 1 ;
        volatile uint32_t TXFULL                           : 1 ;
        volatile uint32_t RXNUMHi                          : 2 ;
        volatile uint32_t unused11                         : 2 ;
        volatile uint32_t TXNUMHi                          : 2 ;
        volatile uint32_t unused12                         : 2 ;
    }f;
}SPI_REG_Status;

#define SPI_REG_INTREN_RXFIFOINTEN (2)
#define SPI_REG_INTREN_TXFIFOINTEN (2)
#define SPI_REG_INTREN_ENDINTEN (4)
#define SPI_REG_INTREN_SLVCMDEN (5)

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t RXFIFOORIntEn                    : 1 ;
        volatile uint32_t TXFIFOURIntEn                    : 1 ;
        volatile uint32_t RXFIFOIntEn                      : 1 ;
        volatile uint32_t TXFIFOIntEn                      : 1 ;
        volatile uint32_t EndIntEn                         : 1 ;
        volatile uint32_t SlvCmdEn                         : 1 ;
        volatile uint32_t unused13                         : 26;
    }f;
}SPI_REG_IntrEn;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t RXFIFOORInt                      : 1 ;
        volatile uint32_t TXFIFOURInt                      : 1 ;
        volatile uint32_t RXFIFOInt                        : 1 ;
        volatile uint32_t TXFIFOInt                        : 1 ;
        volatile uint32_t EndInt                           : 1 ;
        volatile uint32_t SlvCmdInt                        : 1 ;
        volatile uint32_t unused14                         : 26;
    }f;
}SPI_REG_IntrSt;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t SCLK_DIV                         : 8 ;
        volatile uint32_t CSHT                             : 4 ;
        volatile uint32_t CS2SCLK                          : 2 ;
        volatile uint32_t unused15                         : 18;
    }f;
}SPI_REG_Timing;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t MemRdCmd                         : 4 ;
        volatile uint32_t unused16                         : 4 ;
        volatile uint32_t MemCtrlChg                       : 1 ;
        volatile uint32_t unused17                         : 23;
    }f;
}SPI_REG_MemCtrl;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t USR_Status                       : 16;
        volatile uint32_t Ready                            : 1 ;
        volatile uint32_t OverRun                          : 1 ;
        volatile uint32_t UnderRun                         : 1 ;
        volatile uint32_t unused18                         : 13;
    }f;
}SPI_REG_SlvSt;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t RCnt                             : 10;
        volatile uint32_t unused19                         : 6 ;
        volatile uint32_t WCnt                             : 10;
        volatile uint32_t unused20                         : 6 ;
    }f;
}SPI_REG_SlvDataCnt;

typedef union
{
    volatile uint32_t r;
    struct
    {
        volatile uint32_t RxFIFOSize                       : 4 ;
        volatile uint32_t TxFIFOSize                       : 4 ;
        volatile uint32_t DualSPI                          : 1 ;
        volatile uint32_t QuadSPI                          : 1 ;
        volatile uint32_t unused21                         : 1 ;
        volatile uint32_t DirectIO                         : 1 ;
        volatile uint32_t AHBMem                           : 1 ;
        volatile uint32_t EILMMem                          : 1 ;
        volatile uint32_t Slave                            : 1 ;
        volatile uint32_t unused22                         : 17;
    }f;
}SPI_REG_Config;

typedef struct
{
  volatile SPI_REG_IdRev                          IdRev                         ; //0x0
  volatile uint32_t                                  null_reg0                     ; //0x4
  volatile uint32_t                                  null_reg1                     ; //0x8
  volatile uint32_t                                  null_reg2                     ; //0xC
  volatile SPI_REG_TransFmt                       TransFmt                      ; //0x10
  volatile SPI_REG_DirectIO                       DirectIO                      ; //0x14
  volatile uint32_t                                  null_reg3                     ; //0x18
  volatile uint32_t                                  null_reg4                     ; //0x1C
  volatile SPI_REG_TransCtrl                      TransCtrl                     ; //0x20
  volatile SPI_REG_Cmd                            Cmd                           ; //0x24
  volatile SPI_REG_Addr                           Addr                          ; //0x28
  volatile SPI_REG_Data                           Data                          ; //0x2C
  volatile SPI_REG_Ctrl                           Ctrl                          ; //0x30
  volatile SPI_REG_Status                         Status                        ; //0x34
  volatile SPI_REG_IntrEn                         IntrEn                        ; //0x38
  volatile SPI_REG_IntrSt                         IntrSt                        ; //0x3C
  volatile SPI_REG_Timing                         Timing                        ; //0x40
  volatile uint32_t                                  null_reg5                     ; //0x44
  volatile uint32_t                                  null_reg6                     ; //0x48
  volatile uint32_t                                  null_reg7                     ; //0x4C
  volatile SPI_REG_MemCtrl                        MemCtrl                       ; //0x50
  volatile uint32_t                                  null_reg8                     ; //0x54
  volatile uint32_t                                  null_reg9                     ; //0x58
  volatile uint32_t                                  null_reg10                    ; //0x5C
  volatile SPI_REG_SlvSt                          SlvSt                         ; //0x60
  volatile SPI_REG_SlvDataCnt                     SlvDataCnt                    ; //0x64
  volatile uint32_t                                  null_reg11                    ; //0x68
  volatile uint32_t                                  null_reg12                    ; //0x6C
  volatile uint32_t                                  null_reg13                    ; //0x70
  volatile uint32_t                                  null_reg14                    ; //0x74
  volatile uint32_t                                  null_reg15                    ; //0x78
  volatile SPI_REG_Config                         Config                        ; //0x7C
}SPI_REG_H;


/*
 * Description:
 * The data structure to hold parameters to control SPI device.
 */
typedef struct
{
    SPI_REG_TransFmt_SlvMode_e       SlvMode;
    // master related control
    SPI_REG_TransFmt_AddrLen_e       AddressLength;
    uint16_t                         DataLength;
    SPI_REG_TransFmt_DataMerge_e     DataMerge;
    SPI_REG_TransFmt_CPOL_e          CPOL;
    SPI_REG_TransFmt_CPHA_e          CPHA;
    SPI_REG_TransFmt_LSB_e           LSB;
    SPI_REG_TransFmt_MOSIBiDir_e     MOSIBiDir;
    
    uint16_t                          WrTranCnt;
    uint16_t                          RdTranCnt;
    SPI_REG_TransCtrl_TransMode_e     TransMode;
    SPI_REG_TransCtrl_DualQuad_e      DualQuad;
    SPI_REG_TransCtrl_CmdEn_e         PhaseCmd;
    SPI_REG_TransCtrl_AddrEn_e        PhaseAddr;
  
    // slave related control
    uint16_t                          RXTHRES;
    uint16_t                          TXTHRES;

} apSSP_sDeviceControlBlock;


#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

