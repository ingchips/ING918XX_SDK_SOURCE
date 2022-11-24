
#ifndef __PERIPHERAL_IR_H__
#define __PERIPHERAL_IR_H__


#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"


#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

/**
 ****************************************************************************************
 * reg of ir_ctrl
 */
#define bsIR_IR_CTRL_IR_EN                          0
#define bsIR_IR_CTRL_IR_MODE                        1
#define bsIR_IR_CTRL_IR_END_DETECT_EN               3
#define bsIR_IR_CTRL_IR_INT_EN                      4
#define bsIR_IR_CTRL_IR_INT_VERIFY_EN               5
#define bsIR_IR_CTRL_IR_USERCODE_VERIFY             6
#define bsIR_IR_CTRL_IR_DATACODE_VERIFY             7
#define bsIR_IR_CTRL_BIT_TIME_1                     8
#define bsIR_IR_CTRL_TX_REPEAT_MODE                 15
#define bsIR_IR_CTRL_BIT_TIME_2                     16
#define bsIR_IR_CTRL_TXRX_MODE                      23
#define bsIR_IR_CTRL_IR_BIT_CYCLE                   24

#define bwIR_IR_CTRL_IR_EN                          1
#define bwIR_IR_CTRL_IR_MODE                        2
#define bwIR_IR_CTRL_IR_END_DETECT_EN               1
#define bwIR_IR_CTRL_IR_INT_EN                      1
#define bwIR_IR_CTRL_IR_INT_VERIFY_EN               1
#define bwIR_IR_CTRL_IR_USERCODE_VERIFY             1
#define bwIR_IR_CTRL_IR_DATACODE_VERIFY             1
#define bwIR_IR_CTRL_BIT_TIME_1                     7
#define bwIR_IR_CTRL_TX_REPEAT_MODE                 1
#define bwIR_IR_CTRL_BIT_TIME_2                     7
#define bwIR_IR_CTRL_TXRX_MODE                      1
#define bwIR_IR_CTRL_IR_BIT_CYCLE                   7

typedef enum
{
  IR_IR_MODE_IR_NEC,
  IR_IR_MODE_IR_9012,
  IR_IR_MODE_IR_RC5
} IR_IrMode_e;

typedef enum
{
  IR_TXRX_MODE_RX_MODE,
  IR_TXRX_MODE_TX_MODE
} IR_TxRxMode_e;

/**
 ****************************************************************************************
 * reg of ir_tx_config
 */
#define bsIR_IR_TX_CONFIG_TX_START                          0
#define bsIR_IR_TX_CONFIG_IR_TX_POL                         1
#define bsIR_IR_TX_CONFIG_CARRIER_CNT_CLR                   3
#define bsIR_IR_TX_CONFIG_IR_INT_EN                         4

#define bwIR_IR_TX_CONFIG_TX_START                          1
#define bwIR_IR_TX_CONFIG_IR_TX_POL                         1
#define bwIR_IR_TX_CONFIG_CARRIER_CNT_CLR                   1
#define bwIR_IR_TX_CONFIG_IR_INT_EN                         1

/**
 ****************************************************************************************
 * reg of ir_carry_config
 */
#define bsIR_IR_CARRY_CONFIG_IR_CARRY_LOW                          0
#define bsIR_IR_CARRY_CONFIG_IR_CARRY_HIGH                         9

#define bwIR_IR_CARRY_CONFIG_IR_CARRY_LOW                          9
#define bwIR_IR_CARRY_CONFIG_IR_CARRY_HIGH                         9


/**
 ****************************************************************************************
 * reg of ir_time
 */
#define bsIR_IR_TIME_IR_TIME_S                         0
#define bwIR_IR_TIME_IR_TIME_S                         12


/**
 ****************************************************************************************
 * reg of ir_rx_code
 */
#define bsIR_IR_RX_CODE_IR_RX_USERCODE                         0
#define bsIR_IR_RX_CODE_IR_RX_DATACODE                         16

#define bwIR_IR_RX_CODE_IR_RX_USERCODE                         16
#define bwIR_IR_RX_CODE_IR_RX_DATACODE                         16

/**
 ****************************************************************************************
 * reg of ir_tx_code
 */
#define bsIR_IR_TX_CODE_IR_TX_USERCODE                         0
#define bsIR_IR_TX_CODE_IR_TX_DATACODE                         16

#define bwIR_IR_TX_CODE_IR_TX_USERCODE                         16
#define bwIR_IR_TX_CODE_IR_TX_DATACODE                         16

/**
 ****************************************************************************************
 * reg of ir_fsm
 */
#define bsIR_IR_FSM_IR_RECEIVED_OK                             0
#define bsIR_IR_FSM_IR_USERCODE_VERIFY                         1
#define bsIR_IR_FSM_IR_DATACODE_VERIFY                         2
#define bsIR_IR_FSM_IR_REPEAT                                  3
#define bsIR_IR_FSM_IR_TRANSMIT_OK                             4
#define bsIR_IR_FSM_IR_TX_REPEAT                               5

#define bwIR_IR_FSM_IR_RECEIVED_OK                             1
#define bwIR_IR_FSM_IR_USERCODE_VERIFY                         1
#define bwIR_IR_FSM_IR_DATACODE_VERIFY                         1
#define bwIR_IR_FSM_IR_REPEAT                                  1
#define bwIR_IR_FSM_IR_TRANSMIT_OK                             1
#define bwIR_IR_FSM_IR_TX_REPEAT                               1


/**
 ****************************************************************************************
 * @brief IR ctrl reg operations
 *
 * @param[in] IR_BASE              Base address of IR module
 ****************************************************************************************
 */
void IR_CtrlEnable(IR_TypeDef* IR_BASE);
void IR_CtrlSetIrMode(IR_TypeDef* IR_BASE, IR_IrMode_e mode);
void IR_CtrlSetIrEndDetectEn(IR_TypeDef* IR_BASE);
void IR_CtrlSetIrIntEn(IR_TypeDef* IR_BASE);
void IR_CtrlSetIrIntVerifyEn(IR_TypeDef* IR_BASE);
void IR_CtrlIrUsercodeVerify(IR_TypeDef* IR_BASE);
void IR_CtrlIrDatacodeVerify(IR_TypeDef* IR_BASE);
void IR_CtrlIrSetBitTime1(IR_TypeDef* IR_BASE, uint8_t time);
void IR_CtrlIrTxRepeatMode(IR_TypeDef* IR_BASE);
void IR_CleanIrTxRepeatMode(IR_TypeDef* IR_BASE);
void IR_CtrlIrSetBitTime2(IR_TypeDef* IR_BASE, uint8_t time);
void IR_CtrlSetTxRxMode(IR_TypeDef* IR_BASE, IR_TxRxMode_e mode);
void IR_CtrlIrSetIrBitCycle(IR_TypeDef* IR_BASE, uint8_t cycle);

/**
 ****************************************************************************************
 * @brief IR ir_tx_config reg operations
 *
 * @param[in] IR_BASE              Base address of IR module
 ****************************************************************************************
 */
void IR_TxConfigTxStart(IR_TypeDef* IR_BASE);
void IR_TxConfigIrTxPol(IR_TypeDef* IR_BASE);
void IR_TxConfigCarrierCntClr(IR_TypeDef* IR_BASE);
void IR_TxConfigIrIntEn(IR_TypeDef* IR_BASE);


/**
 ****************************************************************************************
 * @brief IR ir_carry_config reg operations
 *
 * @param[in] IR_BASE              Base address of IR module
 ****************************************************************************************
 */
void IR_CarryConfigSetIrCarryLow(IR_TypeDef* IR_BASE, uint16_t val);
void IR_CarryConfigSetIrCarryHigh(IR_TypeDef* IR_BASE, uint16_t val);


/**
 ****************************************************************************************
 * @brief IR ir_time reg operations
 *
 * @param[in] IR_BASE              Base address of IR module
 ****************************************************************************************
 */
void IR_TimeSetIrTime1(IR_TypeDef* IR_BASE, uint16_t time);
void IR_TimeSetIrTime2(IR_TypeDef* IR_BASE, uint16_t time);
void IR_TimeSetIrTime3(IR_TypeDef* IR_BASE, uint16_t time);
void IR_TimeSetIrTime4(IR_TypeDef* IR_BASE, uint16_t time);
void IR_TimeSetIrTime5(IR_TypeDef* IR_BASE, uint16_t time);

/**
 ****************************************************************************************
 * @brief IR ir_rx_code reg operations
 *
 * @param[in] IR_BASE              Base address of IR module
              Val                  Data Pointer
 ****************************************************************************************
 */
uint16_t IR_RxCodeGetIrRxUsercode(IR_TypeDef* IR_BASE);
uint16_t IR_RxCodeGetIrRxDatacode(IR_TypeDef* IR_BASE);

/**
 ****************************************************************************************
 * @brief IR ir_tx_code reg operations
 *
 * @param[in] IR_BASE              Base address of IR module
 ****************************************************************************************
 */
void IR_TxCodeSetIrTxUsercode(IR_TypeDef* IR_BASE, uint16_t val);
void IR_TxCodeSetIrTxDatacode(IR_TypeDef* IR_BASE, uint16_t val);

/**
 ****************************************************************************************
 * @brief IR ir_fsm reg operations
 *
 * @param[in] IR_BASE              Base address of IR module
 ****************************************************************************************
 */
uint8_t IR_FsmGetIrReceivedOk(IR_TypeDef* IR_BASE);
uint8_t IR_FsmGetIrUsercodeVerify(IR_TypeDef* IR_BASE);
uint8_t IR_FsmGetIrDatacodeVerify(IR_TypeDef* IR_BASE);
uint8_t IR_FsmGetIrRepeat(IR_TypeDef* IR_BASE);
uint8_t IR_FsmGetIrTransmitOk(IR_TypeDef* IR_BASE);
uint8_t IR_FsmGetIrTxRepeat(IR_TypeDef* IR_BASE);
void IR_FsmClearIrInt(IR_TypeDef* IR_BASE);

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

