#include "peripheral_ir.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

void IR_Send_Data(IR_REG_H* IR_BASE, IR_Send_Data_Input_Params *Param_p )
{
    uint32_t cycle_low = ((IR_CLK_DEFAULT_HZ / Param_p->carrier_freq_hz)) / IR_CARRIER_DUTY_CYCLE_FACTOR;
    uint32_t cycle_high = ((IR_CLK_DEFAULT_HZ / Param_p->carrier_freq_hz)) - cycle_low;
    
    IR_BASE->ir_carry_config.r = (cycle_high << 9)|cycle_low;
    
    IR_BASE->ir_time_1 = Param_p->lead_code_low_time_us;
    IR_BASE->ir_time_2 = Param_p->lead_code_repeat_high_time_us;
    IR_BASE->ir_time_3 = Param_p->lead_code_high_time_us;
    
    IR_BASE->ir_ctrl.r |= ((Param_p->bit_0_time_us/IR_BIT_TIME_FACTOR) & 0x7f) << 8;
    IR_BASE->ir_ctrl.r |= ((Param_p->bit_1_time_us/IR_BIT_TIME_FACTOR) & 0x7f) << 16;
    IR_BASE->ir_ctrl.r |= ((Param_p->bit_0_1_low_time_us/IR_BIT_TIME_FACTOR) & 0x7f) << 24;
    
    IR_BASE->ir_ctrl.r |= 0x1 << 23;//txrx_mode
    
    IR_BASE->ir_tx_code.r |= ((Param_p->datacode << 16)|(Param_p->usercode));//datacode | usercode
    
    IR_BASE->ir_tx_config.r |= (0x1 << 0);//tx_start_set
}

void IR_Send_Repeat(IR_REG_H* IR_BASE)
{
    IR_BASE->ir_ctrl.r |= (0x1 << 15);//repeat_mode
    IR_BASE->ir_tx_config.r |= (0x1 << 0);//tx_start_set
}

void IR_Send_NEC(IR_REG_H* IR_BASE)
{
  IR_Send_Data_Input_Params Param;
  
  Param.carrier_freq_hz = 38000;
  Param.lead_code_low_time_us = 9000;
  Param.lead_code_repeat_high_time_us = 2250;
  Param.lead_code_high_time_us = 4500;

  Param.bit_0_time_us = 1120;
  Param.bit_1_time_us = 2250;
  Param.bit_0_1_low_time_us = 560;

  Param.datacode = 0x1234;
  Param.usercode = 0xabcd;
  
  IR_Send_Data(IR_BASE, &Param);
}

#endif
