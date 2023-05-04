#ifndef INGSOC_H
#define INGSOC_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum IRQn
{
    /* -------------------  Cortex-M4 Processor Exceptions Numbers  ------------------- */
    NonMaskableInt_IRQn           = -14,      /*!<  2 Non Maskable Interrupt          */
    HardFault_IRQn                = -13,      /*!<  3 HardFault Interrupt             */
    MemoryManagement_IRQn         = -12,      /*!<  4 Memory Management Interrupt     */
    BusFault_IRQn                 = -11,      /*!<  5 Bus Fault Interrupt             */
    UsageFault_IRQn               = -10,      /*!<  6 Usage Fault Interrupt           */
    SVCall_IRQn                   =  -5,      /*!< 11 SV Call Interrupt               */
    DebugMonitor_IRQn             =  -4,      /*!< 12 Debug Monitor Interrupt         */
    PendSV_IRQn                   =  -2,      /*!< 14 Pend SV Interrupt               */
    SysTick_IRQn                  =  -1,      /*!< 15 System Tick Interrupt           */
} IRQn_Type;

typedef enum
{
    PLATFORM_CB_IRQ_RTC,
    PLATFORM_CB_IRQ_GPIO0,
    PLATFORM_CB_IRQ_GPIO1,
    PLATFORM_CB_IRQ_TIMER0,
    PLATFORM_CB_IRQ_TIMER1,
    PLATFORM_CB_IRQ_TIMER2,
    PLATFORM_CB_IRQ_WDT,
    PLATFORM_CB_IRQ_PDM,
    PLATFORM_CB_IRQ_AHPSPI,
    PLATFORM_CB_IRQ_SPI0 = PLATFORM_CB_IRQ_AHPSPI,
    PLATFORM_CB_IRQ_APBSPI,
    PLATFORM_CB_IRQ_SPI1 = PLATFORM_CB_IRQ_APBSPI,
    PLATFORM_CB_IRQ_SADC,
    PLATFORM_CB_IRQ_I2S,
    PLATFORM_CB_IRQ_UART0,
    PLATFORM_CB_IRQ_UART1,
    PLATFORM_CB_IRQ_I2C0,
    PLATFORM_CB_IRQ_I2C1,
    PLATFORM_CB_IRQ_DMA,
    PLATFORM_CB_IRQ_KEYSCAN,
    PLATFORM_CB_IRQ_IR_INT,
    PLATFORM_CB_IRQ_IR_WAKEUP,
    PLATFORM_CB_IRQ_PCAP0,
    PLATFORM_CB_IRQ_PCAP1,
    PLATFORM_CB_IRQ_PCAP2,
    PLATFORM_CB_IRQ_QDEC0,
    PLATFORM_CB_IRQ_QDEC1,
    PLATFORM_CB_IRQ_QDEC2,
    PLATFORM_CB_IRQ_USB,
    PLATFORM_CB_IRQ_LPC_POS,
    PLATFORM_CB_IRQ_LPC_NEG,
    PLATFORM_CB_IRQ_PMU_PVD,
    PLATFORM_CB_IRQ_PMU_PDR,
    PLATFORM_CB_IRQ_PTE0,
    PLATFORM_CB_IRQ_PTE1,
    PLATFORM_CB_IRQ_PTE2,
    PLATFORM_CB_IRQ_PTE3,
    PLATFORM_CB_IRQ_SLOWRC_TUNE,
    PLATFORM_CB_IRQ_32K_CNT,

    PLATFORM_CB_IRQ_MAX
} platform_irq_callback_type_t;

// WARNING: Defined for compatibility with ING918xx. Do not use this.
#define PLATFORM_CB_IRQ_GPIO        PLATFORM_CB_IRQ_GPIO0

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* Configuration of the Cortex-M# Processor and Core Peripherals */
#define __CM4_REV                 0x0001    /*!< Core Revision r0p1                               */
#define __NVIC_PRIO_BITS          5         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __FPU_PRESENT             1         /*!< FPU present or not                               */

#include <core_cm4.h>                       /* Processor and core peripherals                    */
#include <stdint.h>

#include "../__ingsoc.h"

#define INGCHIPS_FAMILY                 INGCHIPS_FAMILY_916

/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

typedef struct
{
    __IO uint32_t OUT_CTRL[7];      // 0x00
         uint32_t Reserved0[9];
    __IO uint32_t IN_CTRL[11];      // 0x40
         uint32_t Reserved1[5];
    __IO uint32_t PE_CTRL[2];       // 0x80
    __IO uint32_t PS_CTRL[2];       // 0x88
    __IO uint32_t Reserved2[28];
    __IO uint32_t IS_CTRL[2];       // 0x100
    __IO uint32_t DR_CTRL[3];       // 0x108
    __IO uint32_t SPI_CFG0;         // 0x114
    __IO uint32_t SPI_CFG1;         // 0x118
    __IO uint32_t SYS_BOND_CFG;     // 0x11c
} PINCTRL_TypeDef;

typedef struct
{
    __IO uint32_t Reserved[4];
    __IO uint32_t Cfg;                          //0x10
    __IO uint32_t IntEn;                        //0x14
    __IO uint32_t Status;                       //0x18
    __IO uint32_t Addr;                         //0x1C
    __IO uint32_t Data;                         //0x20
    __IO uint32_t Ctrl;                         //0x24
    __IO uint32_t Cmd;                          //0x28
    __IO uint32_t Setup;                        //0x2C
    __IO uint32_t TPM;                          //0x30
} I2C_TypeDef;

//uart
typedef struct
{
    __IO uint32_t  DataRead;                    // 0x00
    __IO uint32_t  StatusClear;                 // 0x04
    __IO uint32_t  Reserved1;                   // 0x08
    __IO uint32_t  Reserved2;                   // 0x0C
    __IO uint32_t  Reserved3;                   // 0x10
    __IO uint32_t  Reserved4;                   // 0x14
    __IO uint32_t  Flag;                        // 0x18
    __IO uint32_t  Reserved5;                   // 0x1C
    __IO uint32_t  LowPower;                    // 0x20
    __IO uint32_t  IntBaudDivisor;              // 0x24
    __IO uint32_t  FractBaudDivisor;            // 0x28
    __IO uint32_t  LineCon_H;                   // 0x2C
    __IO uint32_t  Control;                     // 0x30
    __IO uint32_t  FifoSelect;                  // 0x34
    __IO uint32_t  IntMask;                     // 0x38
    __IO uint32_t  IntRaw;                      // 0x3C
    __IO uint32_t  Interrupt;                   // 0x40  // Masked Interrupt
    __IO uint32_t  IntClear;                    // 0x44
    __IO uint32_t  DmaCon;                      // 0x48
} UART_TypeDef;

typedef struct
{
    __IO uint32_t Ctrl;
    __IO uint32_t Reload;
    __IO uint32_t Counter;
    __IO uint32_t Reserved;
} TMR_CH_TypeDef;

// timer
#define CHANNEL_NUMBER_PER_TMR                    2           // each TMR has `CHANNEL_NUMBER_PER_TMR` channels
typedef struct
{
    __IO uint32_t Reserved[4];
    __IO uint32_t Cfg;                  // 0x10
    __IO uint32_t IntEn;                // 0x14
    __IO uint32_t IntStatus;            // 0x18
    __IO uint32_t ChEn;                 // 0x1c
    TMR_CH_TypeDef Channels[CHANNEL_NUMBER_PER_TMR];   // 0x20
} TMR_TypeDef;

// watchdog timer
typedef struct
{
    __IO uint32_t Reserved[4];
    __IO uint32_t Ctrl;                 // 0x10
    __IO uint32_t Reset;                // 0x14
    __IO uint32_t WrEn;                 // 0x18
    __IO uint32_t St;                   // 0x1c
} WDT_TypeDef;


typedef struct
{
    __IO uint32_t Ctrl0;                // 0x00
    __IO uint32_t Ctrl1;                // 0x04
    __IO uint32_t Reserved[2];
    __IO uint32_t PeraTh;               // 0x10
    __IO uint32_t HighTh;               // 0x14
    __IO uint32_t DZoneTh;              // 0x18
    __IO uint32_t DmaData;              // 0x1c
} PWM_ChannelDef;

typedef struct
{
    __IO uint32_t Ctrl0;                // 0x00
    __IO uint32_t Ctrl1;                // 0x04
    __IO uint32_t Reserved[2];
} PCAP_ChannelDef;

// dedicated PWM
typedef struct
{
    PWM_ChannelDef Channels[3];         // 0x00
    PCAP_ChannelDef PCAPChannels[3];    // 0x60
    __IO uint32_t CapCntEn;             // 0x90
    __IO uint32_t CapCounter;           // 0x94
} PWM_TypeDef;

// I2S
typedef struct
{
    __IO uint32_t ModeConfig;           // 0x00
    __IO uint32_t ClkDiv;               // 0x04
    __IO uint32_t Config;               // 0x08
    __IO uint32_t TX;                   // 0x0c
    __IO uint32_t RX;                   // 0x10
    __IO uint32_t Trig;                 // 0x14
    __IO uint32_t Status;               // 0x18
    __IO uint32_t FifoStatus;           // 0x1c
} I2S_TypeDef;

// PDM
typedef struct
{
    __IO uint32_t Cfg;                  // 0x00
    __IO uint32_t DcCfg;                // 0x04
    __IO uint32_t Reserved[7];          // 0x08
    __IO uint32_t Data;                 // 0x24
} PDM_TypeDef;

// GPIO
typedef struct
{
    __IO uint32_t Reserved[4];
    __IO uint32_t Cfg;                  // 0x10
    __IO uint32_t Reserved2[3];
    __IO uint32_t DataIn;               // 0x20
    __IO uint32_t DataOut;              // 0x24
    __IO uint32_t ChDir;                // 0x28
    __IO uint32_t DoutClear;            // 0x2c
    __IO uint32_t DoutSet;              // 0x30
    __IO uint32_t IOIE;                 // 0x34
    __IO uint32_t Reserved3[2];         // 0x38
    __IO uint32_t PullEn;               // 0x40
    __IO uint32_t PullType;             // 0x44
    __IO uint32_t Reserved4[2];         // 0x48
    __IO uint32_t IntEn;                // 0x50
    __IO uint32_t IntMod[3];            // 0x54
    __IO uint32_t Reserved5;
    __IO uint32_t IntStatus;            // 0x64
    __IO uint32_t Reserved6[2];
    __IO uint32_t DeBounceEn;           // 0x70
    __IO uint32_t DeBounceCtrl;         // 0x74
} GIO_TypeDef;

// RTC
typedef struct
{
    __IO uint32_t Reserved[4];
    __IO uint32_t Cntr;                 // 0x10
    __IO uint32_t Alarm;                // 0x14
    __IO uint32_t Ctrl;                 // 0x18
    __IO uint32_t St;                   // 0x1c
    __IO uint32_t Trim;                 // 0x20
    __IO uint32_t SEC_CFG;              // 0x24
} RTC_TypeDef;

typedef struct tagDMA_Descriptor
{
    uint32_t Ctrl;                  // +0x00
    uint32_t TranSize;              // +0x04
    uint32_t SrcAddr;               // +0x08
    uint32_t Reserved1;
    uint32_t DstAddr;               // +0x10
    uint32_t Reserved2;
    struct tagDMA_Descriptor *Next; // +0x18    (8-bytes aligned address)
} DMA_Descriptor;

typedef struct
{
    DMA_Descriptor Descriptor;      // +0x00
    uint32_t Reserved3;             // +0x1c
} DMA_ChannelReg;

// DMA
typedef struct{
    __IO uint32_t Reserved1[8];
    __IO uint32_t Ctrl;                 // 0x20
    __IO uint32_t Abort;                // 0x24
    __IO uint32_t Reserved2[2];
    __IO uint32_t IntStatus;            // 0x30
    __IO uint32_t En;                   // 0x34
    __IO uint32_t Reserved3[2];
    DMA_ChannelReg Channels[8];         // 0x40
} DMA_TypeDef;

// System Control
typedef struct{
    __IO uint32_t CguCfg[8];           // 0x00
    __IO uint32_t RstuCfg[2];          // 0x20
    __IO uint32_t SysCtrl;             // 0x28
    __IO uint32_t CguCfg8;             // 0x2c
    __IO uint32_t DmaCtrl[2];          // 0x30
    __IO uint32_t Reserved3[2];
    __IO uint32_t PllCtrl;             // 0x40
    __IO uint32_t AnaCtrl;             // 0x44
    __IO uint32_t Reserved1[2];
    __IO uint32_t PdmI2sCtrl;          // 0x50
    __IO uint32_t QdecCfg;             // 0x54
    __IO uint32_t CguCfg9;             // 0x58
    __IO uint32_t Reserved2[1];
    __IO uint32_t SysIoStatus;         // 0x60
    __IO uint32_t SysIoWkSource;       // 0x64
    __IO uint32_t SysIoInStatus;       // 0x68
    __IO uint32_t Reserved4[5];
    __IO uint32_t USBCfg;              // 0x80
} SYSCTRL_TypeDef;

// PTE
typedef struct
{
    __IO uint32_t En         :1;            // +0x00
    __IO uint32_t Int        :1;
    __IO uint32_t InMask     :24;
    __IO uint32_t IntMask    :1;
    __IO uint32_t Reserved1  :5;
    __IO uint32_t OutMask    :16;           // +0x04
    __IO uint32_t Reserved2  :16;
} PTE_ChannelCtrlReg;

typedef struct{
    __IO PTE_ChannelCtrlReg Channels[4];    // 0x1a0
} PTE_TypeDef;

typedef struct
{
    __IO uint32_t Reserved0[4];       //0x0
    __IO uint32_t TransFmt;           //0x10
    __IO uint32_t DirectIO;           //0x14
    __IO uint32_t Reserved1[2];       //0x18
    __IO uint32_t TransCtrl;          //0x20
    __IO uint32_t Cmd;                //0x24
    __IO uint32_t Addr;               //0x28
    __IO uint32_t Data;               //0x2C
    __IO uint32_t Ctrl;               //0x30
    __IO uint32_t Status;             //0x34
    __IO uint32_t IntrEn;             //0x38
    __IO uint32_t IntrSt;             //0x3C
    __IO uint32_t Timing;             //0x40
    __IO uint32_t Reserved2[3];       //0x44
    __IO uint32_t MemCtrl;            //0x50
    __IO uint32_t Reserved3[3];       //0x54
    __IO uint32_t SlvSt;              //0x60
    __IO uint32_t SlvDataCnt;         //0x64
    __IO uint32_t Reserved4[5];       //0x68
    __IO uint32_t Config;             //0x7C
} SSP_TypeDef;

typedef struct
{
    __IO uint32_t Efuse_cfg0;         //0x0
    __IO uint32_t Efuse_cfg1;         //0x4
    __IO uint32_t Efuse_cfg2;         //0x8
    __IO uint32_t Efuse_cfg3;         //0xC
    __IO uint32_t Reserved0[3];       //0x10
    __IO uint32_t Efuse_cfg4;         //0x1C
    __IO uint32_t Efuse_dly_cfg0;     //0x20
    __IO uint32_t Efuse_dly_cfg1;     //0x24
    __IO uint32_t Reserved1[2];       //0x28
    __IO uint32_t Efuse_status;       //0x30
    __IO uint32_t Reserved2[3];       //0x34
    __IO uint32_t Efuse_rdata[4];     //0x40
} EFUSE_TypeDef;

typedef struct
{
    __IO uint32_t ir_ctrl;                //0x0
    __IO uint32_t ir_tx_config;           //0x4
    __IO uint32_t ir_carry_config;        //0x8
    __IO uint32_t ir_time_1;              //0xC
    __IO uint32_t ir_time_2;              //0x10
    __IO uint32_t ir_time_3;              //0x14
    __IO uint32_t ir_time_4;              //0x18
    __IO uint32_t ir_time_5;              //0x1C
    __IO uint32_t ir_rx_code;             //0x20
    __IO uint32_t ir_tx_code;             //0x24
    __IO uint32_t ir_fsm;                 //0x28
} IR_TypeDef;

typedef struct{
    __IO uint32_t sadc_cfg[3];            // 0x0
    __IO uint32_t sadc_data;              // 0x0c
    __IO uint32_t sadc_status;            // 0x10
    __IO uint32_t Reserved[7];            // 0x14
    __IO uint32_t sadc_int_mask;          // 0x30
    __IO uint32_t sadc_int;               // 0x34
} SADC_TypeDef;

//QDEC
typedef struct
{
    __IO uint32_t channel_ctrl          ; // 0x00
    __IO uint32_t channel_mode          ; // 0x04
    __IO uint32_t channel_step          ; // 0x08
    __IO uint32_t channel_read_rab      ; // 0x0c
    __IO uint32_t channel_read_tc       ; // 0x10
    __IO uint32_t channel_write_a       ; // 0x14
    __IO uint32_t channel_write_b       ; // 0x18
    __IO uint32_t channel_write_c       ; // 0x1c
    __IO uint32_t channel_tiob0_rd      ; // 0x20
    __IO uint32_t channel_int_en        ; // 0x24
    __IO uint32_t channel_int_dis       ; // 0x28
    __IO uint32_t channel_int_rd        ; // 0x2c
    __IO uint32_t Reserved[4]           ; // 0x30
} QDEC_ChannelCtrlReg;
typedef struct{
    __IO QDEC_ChannelCtrlReg channels[3]; // 0x0
    __IO uint32_t bcr;                    // 0xc0
    __IO uint32_t bmr;                    // 0xc4
    __IO uint32_t qdec_inten;             // 0xc8
    __IO uint32_t qdec_intdis;            // 0xcc
    __IO uint32_t qdec_inten_rd;          // 0xd0
    __IO uint32_t qdec_status_sel;        // 0xd4
    __IO uint32_t pwm_fault;              // 0xd8
    __IO uint32_t Reserved;               // 0xdc
    __IO uint32_t dummy;                  // 0xe0
    __IO uint32_t wpmode;                 // 0xe4
    __IO uint32_t Reserved1;              // 0xe8
    __IO uint32_t addrsize;               // 0xec
    __IO uint32_t name1;                  // 0xf0
    __IO uint32_t name2;                  // 0xf4
    __IO uint32_t FEATURES;               // 0xf8
} QDEC_TypeDef;

typedef struct{
    __IO uint32_t      DICtrlx;     //0x900 + i*20
    __IO uint8_t      _NOT_USED_25[4];
    __IO uint32_t      DIIntx;      //0x908 + i*20
    __IO uint8_t      _NOT_USED_26[4];
    __IO uint32_t      DISizex;     //0x910 + i*20
    __IO uint32_t      DIDmax;      //0x914 + i*20
    __IO uint32_t      DIFifox;     //0x918 + i*20
    __IO uint32_t      DIDmaxx;     //0x91C + i*20
}USB_DIEPReg;

typedef struct{
    __IO uint32_t      DOCtrlx;     //0xB00 + i*20
    __IO uint8_t      _NOT_USED_39[4];//
    __IO uint32_t      DOIntx;      //0xB08 + i*20
    __IO uint8_t      _NOT_USED_40[4];//
    __IO uint32_t      DOSizex;     //0xB10 + i*20
    __IO uint32_t      DODmax;      //0xB14 + i*20
    __IO uint8_t      _NOT_USED_41[4];//
    __IO uint32_t      DODmaxx;     //0xB1C + i*20
}USB_DOEPReg;

typedef struct
{
    __IO uint32_t      _NOT_USED_0;
    __IO uint32_t      UsbOTGIntStat;   //0x0004
    __IO uint32_t      UsbAConfig;      //0x0008
    __IO uint32_t      UsbConfig;       //0x000C
    __IO uint32_t      UsbRControl;     //0x0010
    __IO uint32_t      UsbIntStat;      //0x0014
    __IO uint32_t      UsbIntMask;      //0x0018
    __IO uint32_t      _NOT_USED_1[2];
    __IO uint32_t      UsbRFifoSize;    //0x0024
    __IO uint32_t      UsbTFifoSize;    //0x0028
    __IO uint32_t      _NOT_USED_2[8];
    __IO uint32_t      UsbHConfig3;     //0x004C
    __IO uint32_t      _NOT_USED_3[45];
    __IO uint32_t      UsbIFifo[5];     //0x0104
    __IO uint32_t      _NOT_USED_4[442];
    __IO uint32_t      UsbDConfig;      //0x0800
    __IO uint32_t      UsbDControl;     //0x0804
    __IO uint32_t      _NOT_USED_5[2];
    __IO uint32_t      UsbDIMask;       //0x0810
    __IO uint32_t      UsbDOMask;       //0x0814
    __IO uint32_t      UsbDInt;         //0x0818
    __IO uint32_t      UsbDIntMask;     //0x081C
    __IO uint32_t      _NOT_USED_51[4];
    __IO uint32_t      UsbDThreCtrl;    //0x0830
    __IO uint32_t      _NOT_USED_6[51];
    __IO uint32_t      UsbDICtrl0;      //0x0900
    __IO uint8_t       _NOT_USED_23[4]; //0x0904
    __IO uint32_t      UsbDIInt0;       //0x0908
    __IO uint8_t       _NOT_USED_24[4]; //0x090C
    __IO uint32_t      UsbDISize0;      //0x0910
    __IO uint32_t      UsbDIDma0;       //0x0914
    __IO uint32_t      UsbDIFifo0;      //0x0918
    __IO uint32_t      UsbDIDmax0;      //0x091C
    USB_DIEPReg        UsbDIxConfig[5]; //0x0920
    __IO uint8_t       _NOT_USED_35[320];//0x09C0
    __IO uint32_t      UsbDOCtrl0;      //0x0B00
    __IO uint8_t       _NOT_USED_36[4]; //0x0B04
    __IO uint32_t      UsbDOInt0;       //0x0B08
    __IO uint8_t       _NOT_USED_37[4]; //0x0B0C
    __IO uint32_t      UsbDOSize0;      //0x0B10
    __IO uint32_t      UsbDODma0;       //0x0B14
    __IO uint8_t       _NOT_USED_38[4]; //0x0B18
    __IO uint32_t      UsbDODmax0;      //0x0B1C
    USB_DOEPReg        UsbDOxConfig[5]; //0x0B20
    __IO uint8_t       _NOT_USED_54[576];//0x0BC0
    __IO uint32_t      UsbPCConfig;     //0x0E00
} USB_TypeDef;

typedef struct
{
    __IO uint32_t      key_scanner_ctrl0; //0x0000
    __IO uint32_t      key_scanner_ctrl1; //0x0004
    __IO uint32_t      key_row_mask_ctrl; //0x0008
    __IO uint32_t      key_col_mask_ctrl; //0x000C
    __IO uint32_t      key_int_en;        //0x0010
    __IO uint32_t      key_int;           //0x0014
    __IO uint32_t      fifo_status;       //0x0018
    __IO uint32_t      key_data;          //0x001C
    __IO uint32_t      key_trig;          //0x0020
    __IO uint32_t      key_err_cfg;       //0x0024
} KEYSCAN_TypeDef;

/******************************************************************************/
/*                         memory map                                         */
/******************************************************************************/

#define ROM_BASE           ((uint32_t)0x00000000UL)
#define FLASH_BASE         ((uint32_t)0x02000000UL)
#define AHB_QSPI_MEM_BASE  ((uint32_t)0x04000000UL)
#define SYS_MEM_BASE       ((uint32_t)0x20000000UL)

#define APB_BASE           ((uint32_t)0x40000000UL)
#define APB_SYSCTRL_BASE   (APB_BASE + 0x00000)
#define APB_WDT_BASE       (APB_BASE + 0x01000)
#define APB_TMR0_BASE      (APB_BASE + 0x02000)
#define APB_TMR1_BASE      (APB_BASE + 0x03000)
#define APB_TMR2_BASE      (APB_BASE + 0x04000)
#define APB_PWM_BASE       (APB_BASE + 0x05000)
#define APB_IOMUX_BASE     (APB_BASE + 0x06000)
#define APB_PDM_BASE       (APB_BASE + 0x08000)
#define APB_QDEC_BASE      (APB_BASE + 0x09000)
#define APB_KEYSCAN_BASE   (APB_BASE + 0x0A000)
#define APB_IR_BASE        (APB_BASE + 0x0B000)
#define APB_DMACFG_BASE    (APB_BASE + 0x0C000)
#define APB_SPI_BASE       (APB_BASE + 0x0E000)
#define APB_SARADC_BASE    (APB_BASE + 0x0F000)
#define APB_I2S_BASE       (APB_BASE + 0x10000)
#define APB_UART0_BASE     (APB_BASE + 0x11000)
#define APB_UART1_BASE     (APB_BASE + 0x12000)
#define APB_I2C0_BASE      (APB_BASE + 0x13000)
#define APB_I2C1_BASE      (APB_BASE + 0x14000)
#define APB_GPIO0_BASE     (APB_BASE + 0x15000)
#define APB_GPIO1_BASE     (APB_BASE + 0x16000)
#define APB_EFUSE_BASE     (APB_BASE + 0x17000)

#define AON_APB_BASE       ((uint32_t)0x40100000UL)
#define AON2_CTRL_BASE     (AON_APB_BASE + 0x0000)
#define AON_RTC_BASE       (AON_APB_BASE + 0x1000)
#define AON1_CTRL_BASE     (AON_APB_BASE + 0x2000)

#define AHB_QSPI_BASE      ((uint32_t)0x40160000UL)
#define AHB_USB_BASE       ((uint32_t)0x40180000UL)
#define APB_PINC_BASE      APB_IOMUX_BASE

#define APB_SYSCTRL        ((SYSCTRL_TypeDef *)APB_SYSCTRL_BASE)
#define APB_PTE            ((PTE_TypeDef *)(APB_SYSCTRL_BASE + 0x1a0))
#define APB_WDT            ((WDT_TypeDef *)APB_WDT_BASE)
#define APB_TMR0           ((TMR_TypeDef *)APB_TMR0_BASE)
#define APB_TMR1           ((TMR_TypeDef *)APB_TMR1_BASE)
#define APB_TMR2           ((TMR_TypeDef *)APB_TMR2_BASE)
#define APB_PWM            ((PWM_TypeDef *)APB_PWM_BASE)
#define APB_I2S            ((I2S_TypeDef *)APB_I2S_BASE)
#define APB_SADC           ((SADC_TypeDef *)APB_SARADC_BASE)
#define APB_PDM            ((PDM_TypeDef *)APB_PDM_BASE)
#define APB_QDEC           ((QDEC_TypeDef *)APB_QDEC_BASE)
#define APB_PINCTRL        ((PINCTRL_TypeDef *)APB_PINC_BASE)
#define APB_UART0          ((UART_TypeDef *)APB_UART0_BASE)
#define APB_UART1          ((UART_TypeDef *)APB_UART1_BASE)
#define APB_GPIO0          ((GIO_TypeDef *)APB_GPIO0_BASE)
#define APB_GPIO1          ((GIO_TypeDef *)APB_GPIO1_BASE)
#define APB_I2C0           ((I2C_TypeDef *)APB_I2C0_BASE)
#define APB_I2C1           ((I2C_TypeDef *)APB_I2C1_BASE)
#define AHB_SSP0           ((SSP_TypeDef *)AHB_QSPI_BASE)
#define APB_SSP1           ((SSP_TypeDef *)APB_SPI_BASE)
#define APB_DMA            ((DMA_TypeDef *)APB_DMACFG_BASE)
#define APB_RTC            ((RTC_TypeDef *)AON_RTC_BASE)
#define APB_EFUSE          ((EFUSE_TypeDef *)APB_EFUSE_BASE)
#define APB_IR             ((IR_TypeDef *)APB_IR_BASE)
#define AHB_USB            ((USB_TypeDef *)AHB_USB_BASE)
#define APB_KEYSCAN        ((KEYSCAN_TypeDef *)APB_KEYSCAN_BASE)

#define APB_SPI            APB_SSP1
#define AHB_QSPI           AHB_SSP0

// Test if in interrupt mode
#define IS_IN_INTERRUPT() ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0)

// Test if debugger is attached
#define IS_DEBUGGER_ATTACHED() (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)

#ifdef USE_STDPERIPH_DRIVER
    #include "peripheral_pinctrl.h"
    #include "peripheral_adc.h"
    #include "peripheral_comparator.h"
    #include "peripheral_dma.h"
    #include "peripheral_efuse.h"
    #include "peripheral_gpio.h"
    #include "peripheral_i2c.h"
    #include "peripheral_i2s.h"
    #include "peripheral_ir.h"
    #include "peripheral_pdm.h"
    #include "peripheral_pte.h"
    #include "peripheral_pwm.h"
    #include "peripheral_qdec.h"
    #include "peripheral_rtc.h"
    #include "peripheral_ssp.h"
    #include "peripheral_sysctrl.h"
    #include "peripheral_timer.h"
    #include "peripheral_uart.h"
    #include "peripheral_usb.h"
#endif

#define OSC_CLK_FREQ  24000000UL

#define RTC_CLK_FREQ  32768

#ifdef __cplusplus
}
#endif

#endif



