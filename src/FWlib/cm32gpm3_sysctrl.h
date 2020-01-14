
#ifndef __CM32GPM3_SYSCTRL_H__
#define __CM32GPM3_SYSCTRL_H__


#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "cm32gpm3.h"

/*
 * Description:
 * Bit shifts and widths for SYSCTRL_ClkSwitch  0x00
 */
#define bsSYSCTRL_ClkSwitch_Sys                0
#define bwSYSCTRL_ClkSwitch_Sys                   1
#define bsSYSCTRL_ClkSwitch_CW                 8
#define bwSYSCTRL_ClkSwitch_CW                    1
#define bsSYSCTRL_ClkSwitch_Apb                16
#define bwSYSCTRL_ClkSwitch_Apb                   1
#define bsSYSCTRL_ClkSwitch_PPS                24
#define bwSYSCTRL_ClkSwitch_PPS                   1

/*
 * Description:
 * Bit shifts and widths for SYSCTRL_ClkDiv  0x04
 */
#define bsSYSCTRL_ClkDiv_Sys                     0
#define bwSYSCTRL_ClkDiv_Sys                        5
#define bsSYSCTRL_ClkDiv_CW                      8
#define bwSYSCTRL_ClkDiv_CW                         5
#define bsSYSCTRL_ClkDiv_Apb                    16
#define bwSYSCTRL_ClkDiv_Apb                        5
#define bsSYSCTRL_ClkDiv_PPS                    24
#define bwSYSCTRL_ClkDiv_PPS                        5

/*
 * Description:
 * Bit shifts and widths for SYSCTRL_ClkGate  0x10
 */
#define bsSYSCTRL_ClkGate_AHB_DMA                  0
#define bwSYSCTRL_ClkGate_AHB_DMA                    1 
#define bsSYSCTRL_ClkGate_AHB_BB                   1 
#define bwSYSCTRL_ClkGate_AHB_BB                     1 
#define bsSYSCTRL_ClkGate_AHB_SPI0                 2
#define bwSYSCTRL_ClkGate_AHB_SPI0                   1
#define bsSYSCTRL_ClkGate_APB_GPIO                 3
#define bwSYSCTRL_ClkGate_APB_GPIO                   1
#define bsSYSCTRL_ClkGate_APB_I2C                  4  
#define bwSYSCTRL_ClkGate_APB_I2C                    1  
#define bsSYSCTRL_ClkGate_APB_SPI1                 5
#define bwSYSCTRL_ClkGate_APB_SPI1                   1
#define bsSYSCTRL_ClkGate_APB_TMR0                 6
#define bwSYSCTRL_ClkGate_APB_TMR0                   1
#define bsSYSCTRL_ClkGate_APB_TMR1                 7
#define bwSYSCTRL_ClkGate_APB_TMR1                   1
#define bsSYSCTRL_ClkGate_APB_TMR2                 8
#define bwSYSCTRL_ClkGate_APB_TMR2                   1
#define bsSYSCTRL_ClkGate_APB_SCI0                 9
#define bwSYSCTRL_ClkGate_APB_SCI0                   1
#define bsSYSCTRL_ClkGate_APB_SCI1                10
#define bwSYSCTRL_ClkGate_APB_SCI1                   1
#define bsSYSCTRL_ClkGate_APB_ISOL                11
#define bwSYSCTRL_ClkGate_APB_ISOL                   1
#define bsSYSCTRL_ClkGate_RtcClkDect              12
#define bwSYSCTRL_ClkGate_RtcClkDect                 1
#define bsSYSCTRL_ClkGate_APB_PINC                13
#define bwSYSCTRL_ClkGate_APB_PINC                   1


/*
 * Description:
 * Bit shifts and widths for SYSCTRL_BlockRst  0x20
 */
#define bsSYSCTRL_BlockRst_AHB_DMA                  0
#define bwSYSCTRL_BlockRst_AHB_DMA                       1 
#define bsSYSCTRL_BlockRst_AHB_BB                   1    
#define bwSYSCTRL_BlockRst_AHB_BB                        1 
#define bsSYSCTRL_BlockRst_CW_BB                    2    
#define bwSYSCTRL_BlockRst_CW_BB                         1 
#define bsSYSCTRL_BlockRst_PPS_BB                   3    
#define bwSYSCTRL_BlockRst_PPS_BB                        1 
#define bsSYSCTRL_BlockRst_RTC_BB                   4    
#define bwSYSCTRL_BlockRst_RTC_BB                        1 
#define bsSYSCTRL_BlockRst_RF_BB                    5
#define bwSYSCTRL_BlockRst_RF_BB                         1
#define bsSYSCTRL_BlockRst_AHB_SPI0                 6    
#define bwSYSCTRL_BlockRst_AHB_SPI0                      1
#define bsSYSCTRL_BlockRst_APB_GPIO                 7    
#define bwSYSCTRL_BlockRst_APB_GPIO                      1
#define bsSYSCTRL_BlockRst_APB_I2C                  8     
#define bwSYSCTRL_BlockRst_APB_I2C                       1  
#define bsSYSCTRL_BlockRst_APB_SPI1                 9    
#define bwSYSCTRL_BlockRst_APB_SPI1                      1
#define bsSYSCTRL_BlockRst_APB_TMR0                 10   
#define bwSYSCTRL_BlockRst_APB_TMR0                      1
#define bsSYSCTRL_BlockRst_APB_TMR1                 11   
#define bwSYSCTRL_BlockRst_APB_TMR1                      1
#define bsSYSCTRL_BlockRst_APB_TMR2                 12   
#define bwSYSCTRL_BlockRst_APB_TMR2                      1
#define bsSYSCTRL_BlockRst_APB_SCI0                 13   
#define bwSYSCTRL_BlockRst_APB_SCI0                      1
#define bsSYSCTRL_BlockRst_APB_SCI1                 14   
#define bwSYSCTRL_BlockRst_APB_SCI1                      1
#define bsSYSCTRL_BlockRst_APB_ISOL                 15   
#define bwSYSCTRL_BlockRst_APB_ISOL                      1
#define bsSYSCTRL_BlockRst_APB_PINC                 16
#define bwSYSCTRL_BlockRst_APB_PINC                      1


/*
 * Description:
 * Bit shifts and widths for SYSCTRL_PLLParamONE  0x38
 */
#define bsSYSCTRL_PLLParamONE_N                     0
#define bwSYSCTRL_PLLParamONE_N                          4
#define bsSYSCTRL_PLLParamONE_M                     8
#define bwSYSCTRL_PLLParamONE_M                          8
#define bsSYSCTRL_PLLParamONE_OD                    16
#define bwSYSCTRL_PLLParamONE_OD                         2


 /*
 * Description:
 * Bit shifts and widths for SYSCTRL_PLLParamTWO  0x3C
 */
#define bsSYSCTRL_PLLParamTWO_RST                   0
#define bwSYSCTRL_PLLParamTWO_RST                        1
#define bsSYSCTRL_PLLParamTWO_BP                    8
#define bwSYSCTRL_PLLParamTWO_BP                         1
#define bsSYSCTRL_PLLParamTWO_LOCK                  16
#define bwSYSCTRL_PLLParamTWO_LOCK                       1




//------------------------------
// SYSCTRL_ClkSwitch
//
// set
void SYSCTRL_SetSysClkSwitch(void); 
void SYSCTRL_SetCWClkSwitch(void) ; 
void SYSCTRL_SetApbClkSwitch(void); 
void SYSCTRL_SetPPSClkSwitch(void); 

// clear
void SYSCTRL_ClearSysClkSwitch(void);
void SYSCTRL_ClearCWClkSwitch(void) ;
void SYSCTRL_ClearApbClkSwitch(void);
void SYSCTRL_ClearPPSClkSwitch(void);

// get
uint32_t SYSCTRL_GetClkSwitch(void);


//-----------------------------------------
// SYSCTRL_ClkDiv
//
// set
void SYSCTRL_SetSysClkDiv(uint8_t div) ;
void SYSCTRL_SetCWClkDiv(uint8_t div)  ;
void SYSCTRL_SetApbClkDiv(uint8_t div) ;
void SYSCTRL_SetPPSClkDiv(uint8_t div) ;

// get
uint32_t SYSCTRL_GetClkDiv(void);

//-----------------------------------------
// SYSCTRL_ClkGate
//
// set
void SYSCTRL_SetClkGate_AHB_DMA (void)      ; 
void SYSCTRL_SetClkGate_AHB_BB  (void)      ; 
void SYSCTRL_SetClkGate_AHB_SPI0(void)      ; 
void SYSCTRL_SetClkGate_APB_GPIO(void)      ; 
void SYSCTRL_SetClkGate_APB_I2C (void)      ; 
void SYSCTRL_SetClkGate_APB_SPI1(void)      ; 
void SYSCTRL_SetClkGate_APB_TMR0(void)      ; 
void SYSCTRL_SetClkGate_APB_TMR1(void)      ; 
void SYSCTRL_SetClkGate_APB_TMR2(void)      ; 
void SYSCTRL_SetClkGate_APB_SCI0(void)      ; 
void SYSCTRL_SetClkGate_APB_SCI1(void)      ; 
void SYSCTRL_SetClkGate_APB_ISOL(void)      ; 
void SYSCTRL_SetClkGate_APB_RtcClkDect(void); 
void SYSCTRL_SetClkGate_APB_PINC(void)      ; 


// clear
void SYSCTRL_ClearClkGate_AHB_DMA (void)      ; 
void SYSCTRL_ClearClkGate_AHB_BB  (void)      ;  
void SYSCTRL_ClearClkGate_AHB_SPI0(void)      ;  
void SYSCTRL_ClearClkGate_APB_GPIO(void)      ;  
void SYSCTRL_ClearClkGate_APB_I2C (void)      ;  
void SYSCTRL_ClearClkGate_APB_SPI1(void)      ;  
void SYSCTRL_ClearClkGate_APB_TMR0(void)      ;  
void SYSCTRL_ClearClkGate_APB_TMR1(void)      ; 
void SYSCTRL_ClearClkGate_APB_TMR2(void)      ; 
void SYSCTRL_ClearClkGate_APB_SCI0(void)      ; 
void SYSCTRL_ClearClkGate_APB_SCI1(void)      ; 
void SYSCTRL_ClearClkGate_APB_ISOL(void)      ; 
void SYSCTRL_ClearClkGate_APB_RtcClkDect(void); 
void SYSCTRL_ClearClkGate_APB_PINC(void)      ; 


// toggle 
void SYSCTRL_ToggleClkGate_AHB_DMA (void)      ; 
void SYSCTRL_ToggleClkGate_AHB_BB  (void)      ; 
void SYSCTRL_ToggleClkGate_AHB_SPI0(void)      ;  
void SYSCTRL_ToggleClkGate_APB_GPIO(void)      ;  
void SYSCTRL_ToggleClkGate_APB_I2C (void)      ; 
void SYSCTRL_ToggleClkGate_APB_SPI1(void)      ;  
void SYSCTRL_ToggleClkGate_APB_TMR0(void)      ;  
void SYSCTRL_ToggleClkGate_APB_TMR1(void)      ;  
void SYSCTRL_ToggleClkGate_APB_TMR2(void)      ;  
void SYSCTRL_ToggleClkGate_APB_SCI0(void)      ;  
void SYSCTRL_ToggleClkGate_APB_SCI1(void)      ;  
void SYSCTRL_ToggleClkGate_APB_ISOL(void)      ; 
void SYSCTRL_ToggleClkGate_APB_RtcClkDect(void); 
void SYSCTRL_ToggleClkGate_APB_PINC(void)      ; 


// write
void SYSCTRL_WriteClkGate(uint32_t data) ; 

// read 
uint32_t SYSCTRL_ReadClkGate(void); 


//-----------------------------------------
// SYSCTRL_BlockRst
//
// set
void SYSCTRL_SetBlockRst_AHB_DMA (void); 
void SYSCTRL_SetBlockRst_AHB_BB  (void); 
void SYSCTRL_SetBlockRst_CW_BB   (void); 
void SYSCTRL_SetBlockRst_PPS_BB  (void); 
void SYSCTRL_SetBlockRst_RTC_BB  (void); 
void SYSCTRL_SetBlockRst_RF_BB   (void);
void SYSCTRL_SetBlockRst_AHB_SPI0(void); 
void SYSCTRL_SetBlockRst_APB_GPIO(void); 
void SYSCTRL_SetBlockRst_APB_I2C (void); 
void SYSCTRL_SetBlockRst_APB_SPI1(void); 
void SYSCTRL_SetBlockRst_APB_TMR0(void); 
void SYSCTRL_SetBlockRst_APB_TMR1(void); 
void SYSCTRL_SetBlockRst_APB_TMR2(void); 
void SYSCTRL_SetBlockRst_APB_SCI0(void); 
void SYSCTRL_SetBlockRst_APB_SCI1(void); 
void SYSCTRL_SetBlockRst_APB_ISOL(void); 
void SYSCTRL_SetBlockRst_APB_PINC(void); 


// clear
void SYSCTRL_ClearBlockRst_AHB_DMA (void)  ;
void SYSCTRL_ClearBlockRst_AHB_BB  (void)  ;
void SYSCTRL_ClearBlockRst_CW_BB   (void)  ;
void SYSCTRL_ClearBlockRst_PPS_BB  (void)  ;
void SYSCTRL_ClearBlockRst_RTC_BB  (void)  ;
void SYSCTRL_ClearBlockRst_RF_BB   (void)  ;
void SYSCTRL_ClearBlockRst_AHB_SPI0(void)  ;
void SYSCTRL_ClearBlockRst_APB_GPIO(void)  ;
void SYSCTRL_ClearBlockRst_APB_I2C (void)  ;
void SYSCTRL_ClearBlockRst_APB_SPI1(void)  ;
void SYSCTRL_ClearBlockRst_APB_TMR0(void)  ;
void SYSCTRL_ClearBlockRst_APB_TMR1(void)  ;
void SYSCTRL_ClearBlockRst_APB_TMR2(void)  ;
void SYSCTRL_ClearBlockRst_APB_SCI0(void)  ;
void SYSCTRL_ClearBlockRst_APB_SCI1(void)  ;
void SYSCTRL_ClearBlockRst_APB_ISOL(void)  ;
void SYSCTRL_ClearBlockRst_APB_PINC(void)  ;
                                           
                                           
// toggle                                  
void SYSCTRL_ToggleBlockRst_AHB_DMA (void) ;
void SYSCTRL_ToggleBlockRst_AHB_BB  (void) ;
void SYSCTRL_ToggleBlockRst_CW_BB   (void) ;
void SYSCTRL_ToggleBlockRst_PPS_BB  (void) ;
void SYSCTRL_ToggleBlockRst_RTC_BB  (void) ;
void SYSCTRL_ToggleBlockRst_RF_BB   (void) ;
void SYSCTRL_ToggleBlockRst_AHB_SPI0(void) ;
void SYSCTRL_ToggleBlockRst_APB_GPIO(void) ;
void SYSCTRL_ToggleBlockRst_APB_I2C (void) ;
void SYSCTRL_ToggleBlockRst_APB_SPI1(void) ;
void SYSCTRL_ToggleBlockRst_APB_TMR0(void) ;
void SYSCTRL_ToggleBlockRst_APB_TMR1(void) ;
void SYSCTRL_ToggleBlockRst_APB_TMR2(void) ;
void SYSCTRL_ToggleBlockRst_APB_SCI0(void) ;
void SYSCTRL_ToggleBlockRst_APB_SCI1(void) ;
void SYSCTRL_ToggleBlockRst_APB_ISOL(void) ;
void SYSCTRL_ToggleBlockRst_APB_PINC(void) ;


// write
void SYSCTRL_WriteBlockRst(uint32_t data) ;

// read 
uint32_t SYSCTRL_ReadBlockRst(void) ;


//-----------------------------------------
// SYSCTRL_SoftwareRst
//
// set 
void SYSCTRL_SetSoftwareRst(void) ; 

// get
uint8_t SYSCTRL_GetSoftwareRst(void) ;

//-----------------------------------------
// SYSCTRL_RtcClkCount
//
// read 
uint32_t SYSCTRL_ReadRtcClkCount(void) ; 


//-----------------------------------------
// SYSCTRL_PLLParamONE
//
// set
void SYSCTRL_SetPLLONE_N(uint8_t   data);
void SYSCTRL_SetPLLONE_M(uint8_t   data); 
void SYSCTRL_SetPLLONE_OD(uint8_t  data); 

// get 
uint32_t SYSCTRL_GetPLLONE(void);

//-----------------------------------------
// SYSCTRL_PLLParamTWO
//
// set
void SYSCTRL_SetPLLTWO_RST(uint8_t data); 
void SYSCTRL_SetPLLTWO_BP(uint8_t  data); 

// get 
uint32_t SYSCTRL_GetPLLTWO(void);

uint8_t SYSCTRL_PLL_LOCKED(void);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

