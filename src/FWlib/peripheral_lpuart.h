
#ifndef __PERIPHERAL_LPUART_H__
#define __PERIPHERAL_LPUART_H__

#include <stdint.h>
#include "ingsoc.h"
#ifdef __cplusplus
 extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)



typedef struct lpUART_xStateStruct {
	// Line Control Register, LineCon_H
  uint32_t        word_length;   // WLEN
  uint32_t        parity;        // PEN, EPS, SPS
  uint8_t         fifo_enable;   // FEN
  uint8_t         two_stop_bits; // STP2
  // Control Register, UARTCR
  uint8_t         receive_en;        // RXE
  uint8_t         transmit_en;       // TXE
  uint8_t         UART_en;           // UARTEN
  uint8_t         cts_en;            //CTSEN
  uint8_t         rts_en;            //RTSEN
  // Interrupt FIFO Level Select Register, UARTIFLS
  uint8_t         rxfifo_waterlevel; // RXIFLSEL
  uint8_t         txfifo_waterlevel; // TXIFLSEL
  //UART_eFIFO_WATERLEVEL    rxfifo_waterlevel; // RXIFLSEL
  //UART_eFIFO_WATERLEVEL    txfifo_watchlevel; // TXIFLSEL

  // UART Clock Frequency
  uint32_t        ClockFrequency;
  uint32_t        BaudRate;

} lpUART_sStateStruct;

// #define OPEN_LPUART_DEBUG
uint8_t lpUART_SendData(HUART_TypeDef* pBase, uint16_t Data);
uint16_t lpUart_getData(HUART_TypeDef* pBase);

int lpuart_bauRateSet(HUART_TypeDef* pBase, uint32_t fclk, uint32_t BaudRate, uint32_t Dividend);
uint32_t lpuart_baudRateGet(HUART_TypeDef* pBase, uint32_t fclk);

uint8_t lpUART_setb(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_clear_fifo(HUART_TypeDef* pBase);
uint8_t lpUART_sel_pclk(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_get_valid_clock(HUART_TypeDef* pBase);
uint8_t lpUART_set_cmsb(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_set_smsb(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_set_parity(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_set_even(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_set_delay(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_set_dma_ena(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_get_cnt(HUART_TypeDef* pBase);
uint8_t lpUART_set_fc_ena(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_get_ready(HUART_TypeDef* pBase);
uint8_t lpUART_SetDir(HUART_TypeDef* pBase, uint8_t dir);
uint8_t lpUART_set_tmsb (HUART_TypeDef* pBase, uint8_t value);

uint8_t lpUART_set_fifo_th(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_get_fifo_trig(HUART_TypeDef* pBase);
uint8_t lpUART_set_fifo_irq_enable(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_get_fifo_empty(HUART_TypeDef* pBase);
uint8_t lpUART_set_fifo_empty_irq_enable(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_get_fifo_full(HUART_TypeDef* pBase);
uint8_t lpUART_set_fifo_full_irq_enable(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_set_start(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_get_start(HUART_TypeDef* pBase);
uint8_t lpUART_set_start_irq_enable(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_set_match_char(HUART_TypeDef* pBase, uint16_t value);
uint8_t lpUART_clear_match_char(HUART_TypeDef *pBase, uint8_t value);
uint8_t lpUART_get_char_is_matched(HUART_TypeDef* pBase);
uint8_t lpUART_set_char_match_irq_enable(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_set_stop(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_get_stop(HUART_TypeDef* pBase);
uint8_t lpUART_set_stop_irq_enable(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_get_parityerr(HUART_TypeDef* pBase);
uint8_t lpUART_clear_parityerr(HUART_TypeDef* pBase);
uint8_t lpUART_set_parityerr_irq_enable(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_get_stoperr(HUART_TypeDef* pBase);
uint8_t lpUART_clear_stoperr(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_set_stoperr_irq_enable(HUART_TypeDef* pBase, uint8_t value);
uint8_t lpUART_clear_timeout(HUART_TypeDef* pBase);
uint8_t lpUART_set_timeout_irq_enable(HUART_TypeDef* pBase, uint8_t value);







#endif /* INGCHIPS_FAMILY_26 */

#ifdef __cplusplus
  }
#endif

#endif
