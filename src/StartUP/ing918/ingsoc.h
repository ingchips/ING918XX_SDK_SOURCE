#ifndef INGSOC_H
#define INGSOC_H

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
} IRQn_Type;

typedef enum
{
    PLATFORM_CB_IRQ_RTC,
    PLATFORM_CB_IRQ_TIMER0,
    PLATFORM_CB_IRQ_TIMER1,
    PLATFORM_CB_IRQ_TIMER2,
    PLATFORM_CB_IRQ_GPIO,
    PLATFORM_CB_IRQ_SPI0,
    PLATFORM_CB_IRQ_SPI1,
    PLATFORM_CB_IRQ_UART0,
    PLATFORM_CB_IRQ_UART1,
    PLATFORM_CB_IRQ_I2C0,
    PLATFORM_CB_IRQ_I2C1,

    PLATFORM_CB_IRQ_MAX
} platform_irq_callback_type_t;

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the Cortex-M4 Processor and Core Peripherals  ------- */
#define __CM3_REV                 0x0201    /*!< Core revision r2p1                              */
#define __MPU_PRESENT             1         /*!< MPU present or not                              */
#define __NVIC_PRIO_BITS          3         /*!< Number of Bits used for Priority Levels         */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used    */

#include <core_cm3.h>                       /* Processor and core peripherals                    */

#include "../__ingsoc.h"

#define INGCHIPS_FAMILY                 INGCHIPS_FAMILY_918

/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

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

// System Control
typedef struct{
    __IO uint32_t WRITE;
    __IO uint32_t SET  ;
    __IO uint32_t CLR  ;
    __IO uint32_t MODIFY;
} SYSCTRL_RegDef;

typedef struct{
    __IO uint32_t Reversed;
    __IO uint32_t SYSCTRL_ClkGate;
    __IO uint32_t RstSet;
    __IO uint32_t Ctrl0;
    __IO uint32_t RtcClkDet;
    __IO uint32_t CacheCtrl;
    __IO uint32_t CacheHitCtrl0;
    __IO uint32_t CacheHitCtrl1;
    __IO uint32_t CacheHitCtrl2;
    __IO uint32_t CacheHitCtrl3;
    __IO uint32_t CacheHitCtrl4;
    __IO uint32_t EflashModeCtrl;
} SYSCTRL_TypeDef;

/******************************************************************************/
/*                         memory map                                         */
/******************************************************************************/

#define ROM_BASE           ((uint32_t)0x00000000UL)
#define FLASH_BASE         ((uint32_t)0x00004000UL)
#define SYS_MEM_BASE       ((uint32_t)0x20000000UL)

#define APB_BASE           ((uint32_t)0x40000000UL)
#define AHB_SPI0_BASE      ((uint32_t)0x40060000UL) // SPI0 site in AHB BUS
#define SYSCTRL_BASE       ((uint32_t)0x40070000UL)
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

// system control
#define AHB_SYSCTRL  ((SYSCTRL_TypeDef *)  SYSCTRL_BASE)

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

// Test if in interrupt mode
#define IS_IN_INTERRUPT() ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0)

// Test if debugger is attached
#define IS_DEBUGGER_ATTACHED() (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)

#ifdef USE_STDPERIPH_DRIVER
    #include "peripheral_uart.h"
    #include "peripheral_gpio.h"
    #include "peripheral_sysctrl.h"
    #include "peripheral_i2c.h"
    #include "peripheral_pinctrl.h"
    #include "peripheral_rtc.h"
    #include "peripheral_ssp.h"
    #include "peripheral_timer.h"
    #include "peripheral_adc.h"
    #include "peripheral_pwm.h"
#endif

// Clock Freq Define
#ifndef TARGET_FPGA
#define PLL_CLK_FREQ  48000000UL   // 48MHz, AHB Clock
#define OSC_CLK_FREQ  24000000UL   // 24MHz, Apb Clock
#else
#define PLL_CLK_FREQ  32000000UL   // 32MHz, AHB Clock
#define OSC_CLK_FREQ  16000000UL   // 16MHz, Apb Clock
#endif

#define RTC_CLK_FREQ  32768

#ifdef __cplusplus
  }
#endif

#endif



