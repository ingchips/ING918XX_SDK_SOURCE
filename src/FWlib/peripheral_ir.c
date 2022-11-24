#include "peripheral_ir.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916


void IR_CtrlEnable(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_ctrl |= (1 << bsIR_IR_CTRL_IR_EN);
}

void IR_CtrlSetIrMode(IR_TypeDef* IR_BASE, IR_IrMode_e mode)
{
  IR_BASE->ir_ctrl &= (~(BW2M(bwIR_IR_CTRL_IR_MODE) << bsIR_IR_CTRL_IR_MODE));
  IR_BASE->ir_ctrl |= (mode << bsIR_IR_CTRL_IR_MODE);
}

void IR_CtrlSetIrEndDetectEn(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_ctrl |= (1 << bsIR_IR_CTRL_IR_END_DETECT_EN);
}

void IR_CtrlSetIrIntEn(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_ctrl |= (1 << bsIR_IR_CTRL_IR_INT_EN);
}

void IR_CtrlSetIrIntVerifyEn(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_ctrl |= (1 << bsIR_IR_CTRL_IR_INT_VERIFY_EN);
}

void IR_CtrlIrUsercodeVerify(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_ctrl |= (1 << bsIR_IR_CTRL_IR_USERCODE_VERIFY);
}

void IR_CtrlIrDatacodeVerify(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_ctrl |= (1 << bsIR_IR_CTRL_IR_DATACODE_VERIFY);
}

void IR_CtrlIrSetBitTime1(IR_TypeDef* IR_BASE, uint8_t time)
{
  IR_BASE->ir_ctrl &= (~(BW2M(bwIR_IR_CTRL_BIT_TIME_1) << bsIR_IR_CTRL_BIT_TIME_1));
  IR_BASE->ir_ctrl |= (time << bsIR_IR_CTRL_BIT_TIME_1);
}

void IR_CtrlIrTxRepeatMode(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_ctrl |= (1 << bsIR_IR_CTRL_TX_REPEAT_MODE);
}
void IR_CleanIrTxRepeatMode(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_ctrl &= (~(1 << bsIR_IR_CTRL_TX_REPEAT_MODE));
}
void IR_CtrlIrSetBitTime2(IR_TypeDef* IR_BASE, uint8_t time)
{
  IR_BASE->ir_ctrl &= (~(BW2M(bwIR_IR_CTRL_BIT_TIME_2) << bsIR_IR_CTRL_BIT_TIME_2));
  IR_BASE->ir_ctrl |= (time << bsIR_IR_CTRL_BIT_TIME_2);
}

void IR_CtrlSetTxRxMode(IR_TypeDef* IR_BASE, IR_TxRxMode_e mode)
{
  IR_BASE->ir_ctrl &= (~(BW2M(bwIR_IR_CTRL_TXRX_MODE) << bsIR_IR_CTRL_TXRX_MODE));
  IR_BASE->ir_ctrl |= (mode << bsIR_IR_CTRL_TXRX_MODE);
}

void IR_CtrlIrSetIrBitCycle(IR_TypeDef* IR_BASE, uint8_t cycle)
{
  IR_BASE->ir_ctrl &= (~(BW2M(bwIR_IR_CTRL_IR_BIT_CYCLE) << bsIR_IR_CTRL_IR_BIT_CYCLE));
  IR_BASE->ir_ctrl |= (cycle << bsIR_IR_CTRL_IR_BIT_CYCLE);
}


void IR_TxConfigTxStart(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_tx_config |= (1 << bsIR_IR_TX_CONFIG_TX_START);
}

void IR_TxConfigIrTxPol(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_tx_config |= (1 << bsIR_IR_TX_CONFIG_IR_TX_POL);
}

void IR_TxConfigCarrierCntClr(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_tx_config |= (1 << bsIR_IR_TX_CONFIG_CARRIER_CNT_CLR);
}

void IR_TxConfigIrIntEn(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_tx_config |= (1 << bsIR_IR_TX_CONFIG_IR_INT_EN);
}


void IR_CarryConfigSetIrCarryLow(IR_TypeDef* IR_BASE, uint16_t val)
{
  IR_BASE->ir_carry_config &= (~(BW2M(bwIR_IR_CARRY_CONFIG_IR_CARRY_LOW) << bsIR_IR_CARRY_CONFIG_IR_CARRY_LOW));
  IR_BASE->ir_carry_config |= (val << bsIR_IR_CARRY_CONFIG_IR_CARRY_LOW);
}

void IR_CarryConfigSetIrCarryHigh(IR_TypeDef* IR_BASE, uint16_t val)
{
  IR_BASE->ir_carry_config &= (~(BW2M(bwIR_IR_CARRY_CONFIG_IR_CARRY_HIGH) << bsIR_IR_CARRY_CONFIG_IR_CARRY_HIGH));
  IR_BASE->ir_carry_config |= (val << bsIR_IR_CARRY_CONFIG_IR_CARRY_HIGH);
}


void IR_TimeSetIrTime1(IR_TypeDef* IR_BASE, uint16_t time)
{
  IR_BASE->ir_time_1 &= (~(BW2M(bwIR_IR_TIME_IR_TIME_S) << bsIR_IR_TIME_IR_TIME_S));
  IR_BASE->ir_time_1 |= (time << bsIR_IR_TIME_IR_TIME_S);
}

void IR_TimeSetIrTime2(IR_TypeDef* IR_BASE, uint16_t time)
{
  IR_BASE->ir_time_2 &= (~(BW2M(bwIR_IR_TIME_IR_TIME_S) << bsIR_IR_TIME_IR_TIME_S));
  IR_BASE->ir_time_2 |= (time << bsIR_IR_TIME_IR_TIME_S);
}

void IR_TimeSetIrTime3(IR_TypeDef* IR_BASE, uint16_t time)
{
  IR_BASE->ir_time_3 &= (~(BW2M(bwIR_IR_TIME_IR_TIME_S) << bsIR_IR_TIME_IR_TIME_S));
  IR_BASE->ir_time_3 |= (time << bsIR_IR_TIME_IR_TIME_S);
}

void IR_TimeSetIrTime4(IR_TypeDef* IR_BASE, uint16_t time)
{
  IR_BASE->ir_time_4 &= (~(BW2M(bwIR_IR_TIME_IR_TIME_S) << bsIR_IR_TIME_IR_TIME_S));
  IR_BASE->ir_time_4 |= (time << bsIR_IR_TIME_IR_TIME_S);
}

void IR_TimeSetIrTime5(IR_TypeDef* IR_BASE, uint16_t time)
{
  IR_BASE->ir_time_5 &= (~(BW2M(bwIR_IR_TIME_IR_TIME_S) << bsIR_IR_TIME_IR_TIME_S));
  IR_BASE->ir_time_5 |= (time << bsIR_IR_TIME_IR_TIME_S);
}

uint16_t IR_RxCodeGetIrRxUsercode(IR_TypeDef* IR_BASE)
{
    return (uint16_t)((IR_BASE->ir_rx_code >> bsIR_IR_RX_CODE_IR_RX_USERCODE) & BW2M(bwIR_IR_RX_CODE_IR_RX_USERCODE));
}

uint16_t IR_RxCodeGetIrRxDatacode(IR_TypeDef* IR_BASE)
{  
    return (uint16_t)((IR_BASE->ir_rx_code >> bsIR_IR_RX_CODE_IR_RX_DATACODE) & BW2M(bwIR_IR_RX_CODE_IR_RX_DATACODE));
}

void IR_TxCodeSetIrTxUsercode(IR_TypeDef* IR_BASE, uint16_t val)
{
  IR_BASE->ir_tx_code &= (~(BW2M(bwIR_IR_TX_CODE_IR_TX_USERCODE) << bsIR_IR_TX_CODE_IR_TX_USERCODE));
  IR_BASE->ir_tx_code |= (val << bsIR_IR_TX_CODE_IR_TX_USERCODE);
}

void IR_TxCodeSetIrTxDatacode(IR_TypeDef* IR_BASE, uint16_t val)
{
  IR_BASE->ir_tx_code &= (~(BW2M(bwIR_IR_TX_CODE_IR_TX_DATACODE) << bsIR_IR_TX_CODE_IR_TX_DATACODE));
  IR_BASE->ir_tx_code |= (val << bsIR_IR_TX_CODE_IR_TX_DATACODE);
}

uint8_t IR_FsmGetIrReceivedOk(IR_TypeDef* IR_BASE)
{
  return (uint8_t) ((IR_BASE->ir_fsm >> bsIR_IR_FSM_IR_RECEIVED_OK) & BW2M(bwIR_IR_FSM_IR_RECEIVED_OK));
}

uint8_t IR_FsmGetIrUsercodeVerify(IR_TypeDef* IR_BASE)
{
  return (uint8_t) ((IR_BASE->ir_fsm >> bsIR_IR_FSM_IR_USERCODE_VERIFY) & BW2M(bwIR_IR_FSM_IR_USERCODE_VERIFY));
}

uint8_t IR_FsmGetIrDatacodeVerify(IR_TypeDef* IR_BASE)
{
  return (uint8_t) ((IR_BASE->ir_fsm >> bsIR_IR_FSM_IR_DATACODE_VERIFY) & BW2M(bwIR_IR_FSM_IR_DATACODE_VERIFY));
}

uint8_t IR_FsmGetIrRepeat(IR_TypeDef* IR_BASE)
{
  return (uint8_t) ((IR_BASE->ir_fsm >> bsIR_IR_FSM_IR_REPEAT) & BW2M(bwIR_IR_FSM_IR_REPEAT));
}

uint8_t IR_FsmGetIrTransmitOk(IR_TypeDef* IR_BASE)
{
  return (uint8_t) ((IR_BASE->ir_fsm >> bsIR_IR_FSM_IR_TRANSMIT_OK) & BW2M(bwIR_IR_FSM_IR_TRANSMIT_OK));
}

uint8_t IR_FsmGetIrTxRepeat(IR_TypeDef* IR_BASE)
{
  return (uint8_t) ((IR_BASE->ir_fsm >> bsIR_IR_FSM_IR_TX_REPEAT) & BW2M(bwIR_IR_FSM_IR_TX_REPEAT));
}

void IR_FsmClearIrInt(IR_TypeDef* IR_BASE)
{
  IR_BASE->ir_fsm |= (1 << bsIR_IR_FSM_IR_RECEIVED_OK) | (1 << bsIR_IR_FSM_IR_REPEAT) 
                      | (1 << bsIR_IR_FSM_IR_TRANSMIT_OK) | (1 << bsIR_IR_FSM_IR_TX_REPEAT);
}

#endif
