
#ifndef __CM32GPM3_PWRC_H__
#define __CM32GPM3_PWRC_H__


#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "cm32gpm3.h"

//--------------
// TPO
//
#define bwPWRC_TPO                      8

void PWRC_Set_TPO(uint8_t tpo);
uint8_t PWRC_Get_TPO(void);

//--------------
// TPD
//
#define bwPWRC_TPD                      8

void PWRC_Set_TPD(uint8_t tpd);
uint8_t PWRC_Get_TPD(void);

//--------------
// TAO
//
#define bwPWRC_TAO                      4

void PWRC_Set_TAO(uint8_t tao);
uint8_t PWRC_Get_TAO(void);

//--------------
// TAD
//
#define bwPWRC_TAD                      4

void PWRC_Set_TAD(uint8_t tad);
uint8_t PWRC_Get_TAD(void);

//--------------
// TTO
//
#define bwPWRC_TTO                      4

void PWRC_Set_TTO(uint8_t tto);
uint8_t PWRC_Get_TTO(void);

//--------------
// TTD
//
#define bwPWRC_TTD                      4

void PWRC_Set_TTD(uint8_t ttd);
uint8_t PWRC_Get_TTD(void);

//--------------
// DP_SLP
//
#define bwPWRC_DP_SLP                   1

void PWRC_Set_DP_SLP(void);
uint8_t PWRC_Get_DP_SLP(void);

//--------------
// WRM_BOOT
//
#define bwPWRC_WRM_BOOT                 2

#define PWRC_WRM_BOOT_COLD_BOOT         0
#define PWRC_WRM_BOOT_WAKEUP_HAPPENED   1
#define PWRC_WRM_BOOT_SOFTWARE_RST      2
#define PWRC_WRM_BOOT_WATCHDOG_RST      3

uint8_t PWRC_Get_WRM_BOOT(void);

//--------------
// AE
//
#define bwPWRC_AE                       1

void PWRC_Open_AE(void);
void PWRC_Close_AE(void);
uint8_t PWRC_Get_AE(void);

//--------------
// TE
//
#define bwPWRC_TE                       1

void PWRC_Open_TE(void);
void PWRC_Close_TE(void);
uint8_t PWRC_Get_TE(void);

//--------------
// AE_STAT
//
#define bwPWRC_AE_STAT                  1

uint8_t PWRC_Get_AE_STAT(void);

//--------------
// TE_STAT
//
#define bwPWRC_TE_STAT                  1

uint8_t PWRC_Get_TE_STAT(void);

//--------------
// UART_EN
//
#define bwPWRC_UART_EN                  1

void PWRC_Open_UART_EN(void);
void PWRC_Close_UART_EN(void);
uint8_t PWRC_Get_UART_EN(void);

//--------------
// RTC_M0_EN
//
#define bwPWRC_RTC_M0_EN                1

void PWRC_Open_RTC_M0_EN(void);
void PWRC_Close_RTC_M0_EN(void);
uint8_t PWRC_Get_RTC_M0_EN(void);

//--------------
// RTC_M1_EN
//
#define bwPWRC_RTC_M1_EN                1

void PWRC_Open_RTC_M1_EN(void);
void PWRC_Close_RTC_M1_EN(void);
uint8_t PWRC_Get_RTC_M1_EN(void);

//--------------
// RTC_EXINT_CTRL
//
#define bwPWRC_RTC_EXINT_CTRL             6

#define bsPWRC_EXINT_CTRL_INT_EN          0
#define bwPWRC_EXINT_CTRL_INT_EN                 1

// GPIO                                       
#define bsPWRC_EXINT_CTRL_GIO_OE          1
#define bwPWRC_EXINT_CTRL_GIO_OE                 1
#define bsPWRC_EXINT_CTRL_GIO_O           2
#define bwPWRC_EXINT_CTRL_GIO_O                  1
#define bsPWRC_EXINT_CTRL_GIO_I           3
#define bwPWRC_EXINT_CTRL_GIO_I                  1
 
// PAD Func Sel                                    
#define bsPWRC_EXINT_CTRL_PIN_CTRL        4
#define bwPWRC_EXINT_CTRL_PIN_CTRL               2

#define EXINT_CTRL_PIN_AS_INT         0
#define EXINT_CTRL_PIN_AS_GIO         1
#define EXINT_CTRL_PIN_AS_DBG         2

void PWRC_Set_EXINT_CTRL(uint8_t ctrl);
uint8_t PWRC_Get_EXINT_CTRL(void);

//
void PWRC_Set_EXINT_AS_INT(void);
void PWRC_Set_EXINT_AS_GIO(void);
void PWRC_Set_EXINT_AS_DBG(void);
void PWRC_Set_EXINT_GIO_OUTPUT(void);
void PWRC_Set_EXINT_GIO_INPUT(void);
void PWRC_Set_EXINT_GIO_OUTPUT_HIGH(void);
void PWRC_Set_EXINT_GIO_OUTPUT_LOW(void);
void PWRC_Set_EXINT_INT_ENABLE(void);
void PWRC_Set_EXINT_INT_DISABLE(void);


//--------------
// RTC_M0_HP
//
#define bwPWRC_RTC_M0_HP                1

void PWRC_Clear_RTC_M0_HP(void);
uint8_t PWRC_Get_RTC_M0_HP(void);

//--------------
// RTC_M1_HP
//
#define bwPWRC_RTC_M1_HP                1

void PWRC_Clear_RTC_M1_HP(void);
uint8_t PWRC_Get_RTC_M1_HP(void);

//--------------
// EXINT_HP
//
#define bwPWRC_EXINT_HP                 1

void PWRC_Clear_EXINT_HP(void);
uint8_t PWRC_Get_EXINT_HP(void);

//--------------
// EXINT_TP
//
#define bwPWRC_EXINT_TP                 3

#define PWRC_EXINT_TP_LOW_LEVEL         1
#define PWRC_EXINT_TP_HIGH_LEVEL        0
#define PWRC_EXINT_TP_POSI_EDGE         2
#define PWRC_EXINT_TP_NEGE_EDGE         3
#define PWRC_EXINT_TP_BOTH_EDGE         4

void PWRC_Set_EXINT_TP(uint8_t tp);
uint8_t PWRC_Get_EXINT_TP(void);

//-------------
// EXINT_CL
//
#define bwPWRC_EXINT_SYS_INT_CL         1

void PWRC_Set_EXINT_SYS_INT_CL(void);

//-------------
// TM
//
#define bwPWRC_TM                       2

uint8_t PWRC_Get_TM(void);

//-------------
// NVREG0~7
//
#define bwPWRC_NVREG                    32

void PWRC_Set_NVREG(uint8_t num, uint32_t data);
uint32_t PWRC_Get_NVREG(uint8_t num);






#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

