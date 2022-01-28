
#ifndef __PERIPHERAL_IR_H__
#define __PERIPHERAL_IR_H__


#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"


#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916


typedef union
{
    volatile uint32_t r;
    struct
    {
      volatile uint32_t ir_en                  : 1 ;
      volatile uint32_t ir_mode                : 2 ;
      volatile uint32_t ir_end_detect_en       : 1 ;
      volatile uint32_t ir_int_en              : 1 ;
      volatile uint32_t ir_int_verify_en       : 1 ;
      volatile uint32_t ir_usercode_verify     : 1 ;
      volatile uint32_t ir_datacode_verify     : 1 ;
      volatile uint32_t bit_time_1             : 7 ;
      volatile uint32_t tx_repeat_mode         : 1 ;
      volatile uint32_t bit_time_2             : 7 ;
      volatile uint32_t txrx_mode              : 1 ;
      volatile uint32_t ir_bit_cycle           : 7 ;
      volatile uint32_t unused                 : 1 ;
    }f;
}IR_REG_IR_Ctrl;

typedef union
{
    volatile uint32_t r;
    struct
    {
      volatile uint32_t tx_start            : 1 ;
      volatile uint32_t ir_tx_pol           : 1 ;
      volatile uint32_t ir_debug_sel        : 1 ;
      volatile uint32_t carrier_cnt_clr     : 1 ;
      volatile uint32_t ir_int_en           : 1 ;
      volatile uint32_t unused              : 27 ;
    }f;
}IR_REG_IR_Tx_Config;

typedef union
{
    volatile uint32_t r;
    struct
    {
      volatile uint32_t ir_carry_low             : 9 ;
      volatile uint32_t ir_carry_high            : 9 ;
      volatile uint32_t unused                   : 14 ;
    }f;
}IR_REG_IR_Carry_Config;

typedef union
{
    volatile uint32_t r;
    struct
    {
      volatile uint32_t ir_rx_usercode             : 16 ;
      volatile uint32_t ir_rx_datacode             : 16 ;
    }f;
}IR_REG_IR_Rx_Code;

typedef union
{
    volatile uint32_t r;
    struct
    {
      volatile uint32_t ir_tx_usercode             : 16 ;
      volatile uint32_t ir_tx_datacode             : 16 ;
    }f;
}IR_REG_IR_Tx_Code;

typedef struct
{
  volatile IR_REG_IR_Ctrl                                     ir_ctrl           ; //0x0
  volatile IR_REG_IR_Tx_Config                                ir_tx_config           ; //0x4
  volatile IR_REG_IR_Carry_Config                             ir_carry_config           ; //0x8
  volatile uint32_t                                            ir_time_1           ; //0xC
  volatile uint32_t                                            ir_time_2            ; //0x10
  volatile uint32_t                                            ir_time_3            ; //0x14
  volatile uint32_t                                            ir_time_4            ; //0x18
  volatile uint32_t                                            ir_time_5           ; //0x1C
  volatile IR_REG_IR_Rx_Code                                   ir_rx_code       ; //0x20
  volatile IR_REG_IR_Tx_Code                                   ir_tx_code       ; //0x24
  volatile uint32_t                                            ir_fsm            ; //0x28
}IR_REG_H;

#define IR_CLK_DEFAULT_HZ (24000000)
#define IR_CARRIER_DUTY_CYCLE_FACTOR (3)
#define IR_BIT_TIME_FACTOR (30)

typedef struct
{
  uint32_t carrier_freq_hz;
  
  uint16_t lead_code_low_time_us;
  uint16_t lead_code_repeat_high_time_us;
  uint16_t lead_code_high_time_us;
  
  uint16_t bit_0_time_us;
  uint16_t bit_1_time_us;
  uint16_t bit_0_1_low_time_us;
  
  uint16_t datacode;
  uint16_t usercode;
  
}IR_Send_Data_Input_Params;

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

