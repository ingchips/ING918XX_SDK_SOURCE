

#ifndef CM32GPM3_H
#define CM32GPM3_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum IRQn
{
/* -------------------  Cortex-M3 Processor Exceptions Numbers  ------------------- */
  NonMaskableInt_IRQn           = -14,      /*!<  2 Non Maskable Interrupt          */
  HardFault_IRQn                = -13,      /*!<  3 HardFault Interrupt             */
  MemoryManagement_IRQn         = -12,      /*!<  4 Memory Management Interrupt     */
  BusFault_IRQn                 = -11,      /*!<  5 Bus Fault Interrupt             */
  UsageFault_IRQn               = -10,      /*!<  6 Usage Fault Interrupt           */
  SVCall_IRQn                   =  -5,      /*!< 11 SV Call Interrupt               */
  DebugMonitor_IRQn             =  -4,      /*!< 12 Debug Monitor Interrupt         */
  PendSV_IRQn                   =  -2,      /*!< 14 Pend SV Interrupt               */
  SysTick_IRQn                  =  -1,      /*!< 15 System Tick Interrupt           */

/******  CM32GPM3 specific Interrupt Numbers *********************************************************/
  n00_BB0_IRQn                  =  0 ,
  n01_BB1_IRQn                  =  1 ,
  n02_RTC_M0_IRQn               =  2 ,
  n03_RTC_M1_IRQn               =  3 ,
  n04_TMR0_IRQn                 =  4 ,
  n05_TMR1_IRQn                 =  5 ,
  n06_TMR2_IRQn                 =  6 ,
  n07_DMA_IRQn                  =  7 ,
  n08_EXINT_IRQn                =  8 ,
  n09_GPIO_IRQn                 =  9 ,
  n10_BB0_IRQn                  =  10,
  n11_BB1_IRQn                  =  11,
  n12_DMA_IRQn                  =  12,
  n13_TMR1_IRQn                 =  13,
  n14_SPI0_IRQn                 =  14,
  n15_SPI1_IRQn                 =  15,
  n16_URT0_IRQn                 =  16,
  n17_URT1_IRQn                 =  17,
  n18_I2C_IRQn                  =  18,
  n19_DMA_IRQn                  =  19,
  n20_BB0_IRQn                  =  20,
  n21_BB1_IRQn                  =  21,
  n22_TMR2_IRQn                 =  22,
  n23_SPI1_IRQn                 =  23,
  n24_EXINT_IRQn                =  24,
  n25_GPIO_IRQn                 =  25,
  n26_I2C_IRQn                  =  26,
  n27_URT0_IRQn                 =  27,
  n28_URT1_IRQn                 =  28,
} IRQn_Type;

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

#ifndef C2NIM

/* --------  Configuration of the Cortex-M4 Processor and Core Peripherals  ------- */
#define __CM3_REV                 0x0201    /*!< Core revision r2p1                              */
#define __MPU_PRESENT             1         /*!< MPU present or not                              */
#define __NVIC_PRIO_BITS          3         /*!< Number of Bits used for Priority Levels         */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used    */

#endif

#include <core_cm3.h>                       /* Processor and core peripherals                    */
#include <stdint.h>
/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

#ifdef C2NIM
#def __IO
#else

/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler type
#endif

#endif

// i2c
typedef struct{
    __IO uint32_t NRM  ;  // normal access, "AS IS" operation 
    __IO uint32_t SET  ;  // set    operation
    __IO uint32_t CLR  ;  // clear  operation
    __IO uint32_t TOG  ;  // toggle operation
} I2C_RegDef;

typedef struct
{
  I2C_RegDef     I2C_CTRL0       ; // 0x00
  I2C_RegDef     I2C_TIMING0     ;
  I2C_RegDef     I2C_TIMING1     ;
  I2C_RegDef     I2C_TIMING2     ;
  I2C_RegDef     I2C_CTRL1       ; // 0x40
  __IO uint32_t  I2C_STAT        ;
  __IO uint32_t  Reserved0       ;
  __IO uint32_t  Reserved1       ;
  __IO uint32_t  Reserved3       ;
  I2C_RegDef     I2C_QUEUECTRL   ;       
  __IO uint32_t  I2C_QUEUESTAT   ;       
  __IO uint32_t  Reserved4       ;
  __IO uint32_t  Reserved5       ;
  __IO uint32_t  Reserved6       ;
  I2C_RegDef     I2C_QUEUECMD    ; // 0x80    
  __IO uint32_t  I2C_QUEUEDATA   ; // 0x90
  __IO uint32_t  Reserved7       ;      
  __IO uint32_t  Reserved8       ;      
  __IO uint32_t  Reserved9       ;      
  __IO uint32_t  I2C_DATA        ; // 0xa0
  __IO uint32_t  Reserved10      ;      
  __IO uint32_t  Reserved11      ;      
  __IO uint32_t  Reserved12      ;      
  I2C_RegDef     I2C_DEBUG0      ; // 0xb0       
  I2C_RegDef     I2C_DEBUG1      ;    
  __IO uint32_t  I2C_VERSION     ; // 0xd0
  __IO uint32_t  Reserved13      ;      
  __IO uint32_t  Reserved14      ;      
  __IO uint32_t  Reserved15      ;      
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


// timer
typedef struct
{
  __IO uint32_t CNT;
  __IO uint32_t CMP;
  __IO uint32_t CTL;
  __IO uint32_t LOCK;
} TMR_TypeDef;


//gpio
typedef struct{
    __IO uint32_t WRITE; 
    __IO uint32_t SET  ;
    __IO uint32_t CLR  ;
    __IO uint32_t MODIFY;
} GIO_RegDef;

typedef struct
{
  __IO uint32_t DI;
  __IO uint32_t rev0;
  __IO uint32_t rev1;
  __IO uint32_t rev2;
  GIO_RegDef DO ;
  GIO_RegDef OEB;
  GIO_RegDef rev3;
  GIO_RegDef IS ;
  GIO_RegDef IL ;
  GIO_RegDef IE1;
  GIO_RegDef IE0;
} GIO_TypeDef;

// spi
typedef struct 
{
  __IO uint32_t ControlRegister0;           /* SSPCR0 Address SSP Base + 0x00 */
  __IO uint32_t ControlRegister1;           /* SSPCR1 Address SSP Base + 0x04 */
  __IO uint32_t DataRegister;               /* SSPDR Address SSP Base + 0x08 */
  __IO uint32_t StatusRegister;             /* SSPSR Address SSP Base + 0x0C */
  __IO uint32_t ClockPreScale;              /* SSPCPSR Address SSP Base + 0x10 */
  __IO uint32_t IntMask;                    /* SSPIMSC Interrupt mask set and clear register */
  __IO uint32_t IntRawStatus;               /* SSPRIS Raw interrupt status register */
  __IO uint32_t IntMaskedStatus;            /* SSPMIS Maked interrupt status register */
  __IO uint32_t IntClearRegister;           /* SSPICR Interrupt clear register */
  __IO uint32_t DMAControlReg;              /* SSPDMACR DMA control register */
} SSP_TypeDef;
 
// dma
typedef struct
{
	__IO uint32_t ChSrcAddr;
	__IO uint32_t ChDestAddr;
	__IO uint32_t ChLLI;
	__IO uint32_t ChControl;
	__IO uint32_t ChConfiguration;
	__IO uint32_t Reserved1;
  __IO uint32_t Reserved2;
  __IO uint32_t Reserved3;
  
} DMA_ChDef;

#define DMA_NUM_RSRVD_WRDS_BEFORE_CHANNELS      ( (0x100 - 0x038) >> 2 )
#define DMA_NUM_RSRVD_WRDS_BEFORE_PERIPHERAL_ID ( (0xfe0 - 0x1f4) >> 2 )

typedef struct
{
	  __IO uint32_t         IntStatus;              /* 0x000 */  // RO
    __IO uint32_t         IntTCStatus;                         // RO
    __IO uint32_t         IntTCClear;                          // WO
    __IO uint32_t         IntErrorStatus;                      // RO
    __IO uint32_t         IntErrorClear;          /* 0x010 */  // WO
    __IO uint32_t         RawIntTCStatus;                      // RO
    __IO uint32_t         RawIntErrorStatus;                   // RO
    __IO uint32_t         ActiveChannels;                      // RO, A bit is cleared on completion of the DMA transfer.

    __IO uint32_t         SoftBReq;               /* 0x020 */  // RW
    __IO uint32_t         SoftSReq;               /* 0x024 */  // RW
    __IO uint32_t         SoftLBReq;              /* 0x028 */  // RW
    __IO uint32_t         SoftSBReq;              /* 0x02C */  // RW
    
    __IO uint32_t         Configuration;          /* 0x030 */  // RW
    __IO uint32_t         Sync;                   /* 0x034 */  // RW

    __IO uint32_t         Reserved1[ DMA_NUM_RSRVD_WRDS_BEFORE_CHANNELS ];  // RW

    DMA_ChDef             ChReg[7];     /* 0x100 -  */

    __IO uint32_t         Reserved2[ DMA_NUM_RSRVD_WRDS_BEFORE_PERIPHERAL_ID ];

    __IO uint32_t         PeripheralId0;          /* 0xFE0 */
    __IO uint32_t         PeripheralId1;
    __IO uint32_t         PeripheralId2;
    __IO uint32_t         PeripheralId3;
    __IO uint32_t         CellId0;                /* 0xFF0 */
    __IO uint32_t         CellId1;
    __IO uint32_t         CellId2;
    __IO uint32_t         CellId3;
	
} DMA_TypeDef;

// System Control
typedef struct{
    __IO uint32_t WRITE; 
    __IO uint32_t SET  ;
    __IO uint32_t CLR  ;
    __IO uint32_t MODIFY;
} SYSCTRL_RegDef;


typedef struct{
    // 0x00
    __IO uint8_t  SYSCTRL_SysClkSwitch;
    __IO uint8_t  SYSCTRL_CWClkSwitch;
    __IO uint8_t  SYSCTRL_ApbClkSwitch; 
    __IO uint8_t  SYSCTRL_PPSClkSwitch; 

    // 0x04
    __IO uint8_t  SYSCTRL_SysClkDiv;
    __IO uint8_t  SYSCTRL_CWClkDiv;
    __IO uint8_t  SYSCTRL_ApbClkDiv; 
    __IO uint8_t  SYSCTRL_PPSClkDiv; 

    // 0x08
    __IO uint32_t Reversed1;

    // 0x0c
    __IO uint32_t Reversed2;

    // 0x10 - 0x14 - 0x18 - 0x1c
    SYSCTRL_RegDef SYSCTRL_ClkGate;
    
    // 0x20 - 0x24 - 0x28 - 0x2c
    SYSCTRL_RegDef SYSCTRL_BlockRst;

    // 0x30
    __IO uint32_t SYSCTRL_SoftwareRst;

    // 0x34, RO
    __IO uint32_t SYSCTRL_RtcClkCount;

    // 0x38
    __IO uint8_t SYSCTRL_PLLParamONE_N;
    __IO uint8_t SYSCTRL_PLLParamONE_M;
    __IO uint8_t SYSCTRL_PLLParamONE_OD;
    __IO uint8_t Reversed3;

    // 0x3c
    __IO uint8_t SYSCTRL_PLLParamTWO_RST;
    __IO uint8_t SYSCTRL_PLLParamTWO_BP;
    __IO uint8_t SYSCTRL_PLLParamTWO_LKDT;
    __IO uint8_t Reversed4;
} SYSCTRL_TypeDef;

// Pwrc
typedef struct{
    __IO uint32_t	TPO          ;  // 0x00
    __IO uint32_t	TPD          ;  // 0x04
    __IO uint32_t	TAO          ;  // 0x08
    __IO uint32_t	TAD          ;  // 0x0C
    __IO uint32_t	TTO          ;  // 0x10
    __IO uint32_t	TTD          ;  // 0x14
    __IO uint32_t	DP_SLP       ;  // 0x18
    __IO uint32_t	WRM_BOOT     ;  // 0x1C
    __IO uint32_t	AE           ;  // 0x20
    __IO uint32_t	AE_STAT      ;  // 0x24
    __IO uint32_t	TE           ;  // 0x28
    __IO uint32_t	TE_STAT      ;  // 0x2C
    __IO uint32_t	UART_EN      ;  // 0x30
    __IO uint32_t	RTC_M0_EN    ;  // 0x34
    __IO uint32_t	RTC_M0_HP    ;  // 0x38
    __IO uint32_t	RTC_M1_EN    ;  // 0x3C
    __IO uint32_t	RTC_M1_HP    ;  // 0x40
    __IO uint32_t	EXINT_CTRL   ;  // 0x44
    __IO uint32_t	EXINT_HP     ;  // 0x48
    __IO uint32_t	EXINT_TP     ;  // 0x4C
    __IO uint32_t EXINT_CL     ;  // 0x50
    __IO uint32_t Reserved1    ;  // 0x54
    __IO uint32_t Reserved2    ;  // 0x58
    __IO uint32_t Reserved3    ;  // 0x5C
    __IO uint32_t	TM           ;  // 0x60
    __IO uint32_t Reserved4    ;  // 0x64
    __IO uint32_t Reserved5    ;  // 0x68
    __IO uint32_t Reserved6    ;  // 0x6C
    __IO uint32_t	NVREG0       ;  // 0x70
    __IO uint32_t	NVREG1       ;
    __IO uint32_t	NVREG2       ;
    __IO uint32_t	NVREG3       ;
    __IO uint32_t	NVREG4       ;
    __IO uint32_t	NVREG5       ;
    __IO uint32_t	NVREG6       ;
    __IO uint32_t	NVREG7       ;
} PWRCCTRL_TypeDef;


// Apb Pin Control
typedef struct{
    __IO uint8_t  PadCtrl;
    __IO uint8_t  FuncSel;
    __IO uint8_t  Reserved1; 
    __IO uint8_t  Reserved2;
} PINC_RegDef;

typedef struct{
		PINC_RegDef RF_CLK     ; //  0
		                        
		PINC_RegDef RF_CHN0_4  ; //  1
		PINC_RegDef RF_CHN0_3  ; //  2
		PINC_RegDef RF_CHN0_2  ; //  3
		PINC_RegDef RF_CHN0_1  ; //  4
		PINC_RegDef RF_CHN0_0  ; //  5
		                           
		PINC_RegDef RF_CHN1_4  ; //  6
		PINC_RegDef RF_CHN1_3  ; //  7
		PINC_RegDef RF_CHN1_2  ; //  8
		PINC_RegDef RF_CHN1_1  ; //  9
		PINC_RegDef RF_CHN1_0  ; // 10
		                           
		PINC_RegDef RF_CHN2_4  ; // 11
		PINC_RegDef RF_CHN2_3  ; // 12
		PINC_RegDef RF_CHN2_2  ; // 13
		PINC_RegDef RF_CHN2_1  ; // 14
		PINC_RegDef RF_CHN2_0  ; // 15
		                          
		PINC_RegDef UART_RX0   ; // 16
		PINC_RegDef UART_TX0   ; // 17
		PINC_RegDef UART_RX1   ; // 18
		PINC_RegDef UART_TX1   ; // 19
		                          
		PINC_RegDef SCL        ; // 20
		PINC_RegDef SDA        ; // 21
		                         
		PINC_RegDef SS0        ; // 22
		                           
		PINC_RegDef SCLK1      ; // 23
		PINC_RegDef MOSI1      ; // 24
		PINC_RegDef MISO1      ; // 25
		PINC_RegDef SS1        ; // 26
		                          
		PINC_RegDef GPIO_0     ; // 27
		PINC_RegDef GPIO_1     ; // 28
		PINC_RegDef GPIO_2     ; // 29
		PINC_RegDef GPIO_3     ; // 30
		PINC_RegDef GPIO_4     ; // 31
		PINC_RegDef GPIO_5     ; // 32
		                         
		PINC_RegDef PPS        ; // 33
		                           
		PINC_RegDef WP         ; // 34
		PINC_RegDef HOLDN      ; // 35
		                        
		PINC_RegDef SCLK0      ; // 36
		PINC_RegDef MOSI0      ; // 37
		PINC_RegDef MISO0      ; // 38
} PINC_TypeDef;

#ifndef C2NIM

/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
  /* leave anonymous unions enabled */
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler type
#endif

#endif

/******************************************************************************/
/*                         memory map                                         */
/******************************************************************************/
// AHB


#define ROM_BASE           ((uint32_t)0x00000000UL)
#define ITCM_BASE          ((uint32_t)0x00010000UL)
#define SRAM_MIRROR_BASE   ((uint32_t)0x00060000UL)
#define SRAM_BASE          ((uint32_t)0x20000000UL)
#define ROM_MIRROR_BASE    ((uint32_t)0x21000000UL)
#define ITCM_MIRROR_BASE   ((uint32_t)0x21010000UL)

#define APB_BASE           ((uint32_t)0x40000000UL)
#define AHB_SPI0_BASE      ((uint32_t)0x40060000UL) // SPI0 site in AHB BUS
#define SYSCTRL_BASE       ((uint32_t)0x40070000UL)
#define DMA_BASE           ((uint32_t)0x40080000UL)
#define BB_REG_BASE        ((uint32_t)0x40090000UL)
#define PWM_REG_BASE       ((uint32_t)0x40011000UL)

// APB
#define APB_GIO_BASE       (APB_BASE          )//0x00000  // gpio low  32
#define APB_TMR0_BASE      (APB_BASE + (1<<12))//0x01000  // WatchDog
#define APB_TMR1_BASE      (APB_BASE + (2<<12))//0x02000
#define APB_TMR2_BASE      (APB_BASE + (3<<12))//0x03000
#define APB_PINC_BASE      (APB_BASE + (4<<12))//0x04000

#define APB_I2C0_BASE      (APB_BASE + (1<<16))//0x10000
#define APB_I2C1_BASE      (APB_BASE + 0x12000)
#define APB_SCI0_BASE      (APB_BASE + (2<<16))//0x20000
#define APB_SCI1_BASE      (APB_BASE + (2<<16) + (1<<12))//0x21000
#define APB_SPI1_BASE      (APB_BASE + (3<<16))//0x30000     // SPI1 site in APB BUS
#define APB_RTC_BASE       (APB_BASE + (4<<16))//0x40000     // RTCDomain RTC
#define APB_PWRC_BASE      APB_RTC_BASE

// dma
#define AHB_DMA      ((DMA_TypeDef *)  DMA_BASE) 

// system control
#define AHB_SYSCTRL  ((SYSCTRL_TypeDef *)  SYSCTRL_BASE)

#define AHB_SYSCTRL_ClkSwitch_ADDR     SYSCTRL_BASE
#define AHB_SYSCTRL_ClkDiv_ADDR        SYSCTRL_BASE + 0x04
#define AHB_SYSCTRL_PLLParamONE_ADDR   SYSCTRL_BASE + 0x38
#define AHB_SYSCTRL_PLLParamTWO_ADDR   SYSCTRL_BASE + 0x3c

// i2c
#define APB_I2C0     ((I2C_TypeDef *) APB_I2C0_BASE)
#define APB_I2C1     ((I2C_TypeDef *) APB_I2C1_BASE)

// uart
#define APB_UART0    ((UART_TypeDef *) APB_SCI0_BASE)
#define APB_UART1    ((UART_TypeDef *) APB_SCI1_BASE)
// timer
#define APB_TMR0     ((TMR_TypeDef *) APB_TMR0_BASE)
#define APB_TMR1     ((TMR_TypeDef *) APB_TMR1_BASE)
#define APB_TMR2     ((TMR_TypeDef *) APB_TMR2_BASE)
// spi
#define AHB_SSP0     ((SSP_TypeDef *)  AHB_SPI0_BASE) // Actually, APB_SPI0 site in AHB Bus.
#define APB_SSP1     ((SSP_TypeDef *)  APB_SPI1_BASE)
// pwrc
#define APB_PWRC     ((PWRCCTRL_TypeDef *)  APB_PWRC_BASE)
// update
#define APB_UPDATE   (APB_RTC_BASE + 0xF0)

// register access
#define io_write_b(a,d)  (*(__IO uint8_t*)(a)=(d))
#define io_read_b(a)     (*(__IO uint8_t*)(a))
#define io_write(a,d)    (*(__IO uint32_t*)(a)=(d))
#define io_read(a)       (*(__IO uint32_t*)(a))

// check RTC Domain Update regitser before Write other RTC Domain register
#define RtcDomainUpdating    io_read(APB_UPDATE)

// apb pin ctrl
#define APB_PINC     ((PINC_TypeDef *) APB_PINC_BASE) 

typedef enum
{
    I2C_PORT_0,
    I2C_PORT_1
} i2c_port_t;

typedef enum
{
    UART_PORT_0,
    UART_PORT_1
} uart_port_t;

typedef enum
{
    SPI_PORT_0,
    SPI_PORT_1
} spi_port_t;

#ifdef USE_STDPERIPH_DRIVER
    #include "cm32gpm3_uart.h"
    #include "cm32gpm3_gpio.h"
    #include "cm32gpm3_sysctrl.h"
    #include "cm32gpm3_i2c.h"
    #include "cm32gpm3_pinctrl.h"
    #include "cm32gpm3_pwrc.h"
    #include "cm32gpm3_rtc.h"    
    #include "cm32gpm3_ssp.h"
    #include "cm32gpm3_timer.h"
    #include "cm32gpm3_adc.h"
    #include "cm32gpm3_pwm.h"
#endif


// Bits Width change to Mask Bits
#define BW2M(a)          (1 << a) -1

// Clock Freq Define
#ifndef TARGET_FPGA
#define PLL_CLK_FREQ  48000000UL   // 30MHz, AHB Clock
#define OSC_CLK_FREQ  24000000UL   // 20MHz, Apb Clock
#else
#define PLL_CLK_FREQ  32000000UL   // 30MHz, AHB Clock
#define OSC_CLK_FREQ  16000000UL   // 20MHz, Apb Clock
#endif

#define RTC_CLK_FREQ  32768

#ifdef __cplusplus
  }
#endif

#endif



