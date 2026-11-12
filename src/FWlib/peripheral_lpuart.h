
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
/**
 * @brief Write one data word to the LPUART transmit data register.
 *
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] Data Data to transmit. The valid width is determined by the configured frame format.
 * @return Always returns 0.
 */
uint8_t lpUART_SendData(HUART_TypeDef* pBase, uint16_t Data);
/**
 * @brief Read one data word from the LPUART receive data register.
 *
 * @param[in] pBase LPUART peripheral instance.
 * @return Received data. Only the least-significant 9 bits are returned.
 */
uint16_t lpUart_getData(HUART_TypeDef* pBase);

/**
 * @brief Configure the LPUART baud-rate generator.
 *
 * The function calculates the divisor from @p fclk, @p BaudRate and @p Dividend,
 * then writes both the dividend and divisor fields to the baud-rate register.
 *
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] fclk LPUART source clock frequency, in Hz.
 * @param[in] BaudRate Requested baud rate, in bit/s.
 * @param[in] Dividend Baud-rate dividend; valid range is 1 to 0x7ff.
 * @return 0 on success; -1 if an argument or the calculated divisor is invalid.
 */
int lpuart_bauRateSet(HUART_TypeDef* pBase, uint32_t fclk, uint32_t BaudRate, uint32_t Dividend);
/**
 * @brief Get the baud rate configured in the LPUART baud-rate register.
 *
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] fclk LPUART source clock frequency, in Hz.
 * @return Configured baud rate in bit/s; 0 if the configured dividend is zero.
 */
uint32_t lpuart_baudRateGet(HUART_TypeDef* pBase, uint32_t fclk);

/**
 * @brief Set the LPUART B configuration bit.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to clear the bit; any nonzero value to set it.
 * @return Always returns 0.
 */
uint8_t lpUART_setb(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Clear the LPUART FIFO.
 * @param[in] pBase LPUART peripheral instance.
 * @return Always returns 0.
 */
uint8_t lpUART_clear_fifo(HUART_TypeDef* pBase);
/**
 * @brief Select the LPUART clock source.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to clear the PCLK selection; any nonzero value to select PCLK.
 * @return Always returns 0.
 */
uint8_t lpUART_sel_pclk(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Check whether the selected LPUART clock is valid.
 * @param[in] pBase LPUART peripheral instance.
 * @return 1 if the clock is valid; otherwise 0.
 */
uint8_t lpUART_get_valid_clock(HUART_TypeDef* pBase);
/**
 * @brief Configure the CMSB field of the LPUART configuration register.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value CMSB value. Values greater than 8 are limited to 8.
 * @return Always returns 0.
 */
uint8_t lpUART_set_cmsb(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Configure the SMSB field of the LPUART configuration register.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value SMSB value; only bit 0 is used.
 * @return Always returns 0.
 */
uint8_t lpUART_set_smsb(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Enable or disable LPUART parity generation and checking.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable parity; any nonzero value to enable parity.
 * @return Always returns 0.
 */
uint8_t lpUART_set_parity(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Select odd or even parity for LPUART frames.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to select odd parity; any nonzero value to select even parity.
 * @return Always returns 0.
 */
uint8_t lpUART_set_even(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Configure the LPUART transmit delay field.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value Transmit delay; only the least-significant 4 bits are used.
 * @return Always returns 0.
 */
uint8_t lpUART_set_delay(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Enable or disable LPUART DMA requests.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable DMA requests; any nonzero value to enable them.
 * @return Always returns 0.
 */
uint8_t lpUART_set_dma_ena(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Read the current LPUART count value.
 * @param[in] pBase LPUART peripheral instance.
 * @return Current 5-bit count value.
 */
uint8_t lpUART_get_cnt(HUART_TypeDef* pBase);
/**
 * @brief Enable or disable LPUART hardware flow control.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable flow control; any nonzero value to enable it.
 * @return Always returns 0.
 */
uint8_t lpUART_set_fc_ena(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Get the LPUART ready status.
 * @param[in] pBase LPUART peripheral instance.
 * @return 1 if LPUART is ready; otherwise 0.
 */
uint8_t lpUART_get_ready(HUART_TypeDef* pBase);
/**
 * @brief Select the LPUART transfer direction.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] dir 0 to select receive direction; any nonzero value to select transmit direction.
 * @return Always returns 0.
 */
uint8_t lpUART_SetDir(HUART_TypeDef* pBase, uint8_t dir);
/**
 * @brief Configure the TMSB field on a supported high-speed LPUART instance.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value TMSB value; only the least-significant 7 bits are used.
 * @return 0 on success; 1 when @p pBase is AHB_LPUART, which does not support this field.
 */
uint8_t lpUART_set_tmsb (HUART_TypeDef* pBase, uint8_t value);

/**
 * @brief Configure the LPUART FIFO interrupt threshold.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value FIFO threshold; only the least-significant 5 bits are used.
 * @return Always returns 0.
 */
uint8_t lpUART_set_fifo_th(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Get the LPUART FIFO threshold-trigger status.
 * @param[in] pBase LPUART peripheral instance.
 * @return 1 if the configured FIFO threshold has been triggered; otherwise 0.
 */
uint8_t lpUART_get_fifo_trig(HUART_TypeDef* pBase);
/**
 * @brief Enable or disable the FIFO threshold-trigger interrupt.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable the interrupt; any nonzero value to enable it.
 * @return Always returns 0.
 */
uint8_t lpUART_set_fifo_irq_enable(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Get the LPUART FIFO-empty status.
 * @param[in] pBase LPUART peripheral instance.
 * @return 1 if the FIFO is empty; otherwise 0.
 */
uint8_t lpUART_get_fifo_empty(HUART_TypeDef* pBase);
/**
 * @brief Enable or disable the LPUART FIFO-empty interrupt.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable the interrupt; any nonzero value to enable it.
 * @return Always returns 0.
 */
uint8_t lpUART_set_fifo_empty_irq_enable(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Get the LPUART FIFO-full status.
 * @param[in] pBase LPUART peripheral instance.
 * @return 1 if the FIFO is full; otherwise 0.
 */
uint8_t lpUART_get_fifo_full(HUART_TypeDef* pBase);
/**
 * @brief Enable or disable the LPUART FIFO-full interrupt.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable the interrupt; any nonzero value to enable it.
 * @return Always returns 0.
 */
uint8_t lpUART_set_fifo_full_irq_enable(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Set or clear the LPUART start-event status flag.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to clear the flag; any nonzero value to set it.
 * @return Always returns 0.
 */
uint8_t lpUART_set_start(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Get the LPUART start-event status.
 * @param[in] pBase LPUART peripheral instance.
 * @return 1 if a start event is pending; otherwise 0.
 */
uint8_t lpUART_get_start(HUART_TypeDef* pBase);
/**
 * @brief Enable or disable the LPUART start-event interrupt.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable the interrupt; any nonzero value to enable it.
 * @return Always returns 0.
 */
uint8_t lpUART_set_start_irq_enable(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Configure the character value used by the LPUART match detector.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value Character-match value; only the least-significant 9 bits are used.
 * @return Always returns 0.
 */
uint8_t lpUART_set_match_char(HUART_TypeDef* pBase, uint16_t value);
/**
 * @brief Clear the LPUART character-match status flag.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value Write-one-to-clear value; use 1 to acknowledge the status flag.
 * @return Always returns 0.
 */
uint8_t lpUART_clear_match_char(HUART_TypeDef *pBase, uint8_t value);
/**
 * @brief Get the LPUART character-match status.
 * @param[in] pBase LPUART peripheral instance.
 * @return 1 if the received character matched the configured value; otherwise 0.
 */
uint8_t lpUART_get_char_is_matched(HUART_TypeDef* pBase);
/**
 * @brief Enable or disable the LPUART character-match interrupt.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable the interrupt; any nonzero value to enable it.
 * @return Always returns 0.
 */
uint8_t lpUART_set_char_match_irq_enable(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Set or clear the LPUART stop-event status flag.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to clear the flag; any nonzero value to set it.
 * @return Always returns 0.
 */
uint8_t lpUART_set_stop(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Get the LPUART stop-event status.
 * @param[in] pBase LPUART peripheral instance.
 * @return 1 if a stop event is pending; otherwise 0.
 */
uint8_t lpUART_get_stop(HUART_TypeDef* pBase);
/**
 * @brief Enable or disable the LPUART stop-event interrupt.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable the interrupt; any nonzero value to enable it.
 * @return Always returns 0.
 */
uint8_t lpUART_set_stop_irq_enable(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Get the LPUART parity-error status.
 * @param[in] pBase LPUART peripheral instance.
 * @return 1 if a parity error has been detected; otherwise 0.
 */
uint8_t lpUART_get_parityerr(HUART_TypeDef* pBase);
/**
 * @brief Clear the LPUART parity-error status flag.
 * @param[in] pBase LPUART peripheral instance.
 * @return Always returns 0.
 */
uint8_t lpUART_clear_parityerr(HUART_TypeDef* pBase);
/**
 * @brief Enable or disable the LPUART parity-error interrupt.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable the interrupt; any nonzero value to enable it.
 * @return Always returns 0.
 */
uint8_t lpUART_set_parityerr_irq_enable(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Get the stop-bit error status on a supported high-speed LPUART instance.
 * @param[in] pBase LPUART peripheral instance.
 * @return 1 if a stop-bit error has been detected; 0 if no error is present or the instance is AHB_LPUART.
 */
uint8_t lpUART_get_stoperr(HUART_TypeDef* pBase);
/**
 * @brief Clear the stop-bit error status on a supported high-speed LPUART instance.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value Reserved by the current implementation.
 * @return 0 on success; 1 when @p pBase is AHB_LPUART, which does not support this status.
 */
uint8_t lpUART_clear_stoperr(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Enable or disable the stop-bit error interrupt on a supported high-speed LPUART instance.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable the interrupt; any nonzero value to enable it.
 * @return 0 on success; 1 when @p pBase is AHB_LPUART, which does not support this interrupt.
 */
uint8_t lpUART_set_stoperr_irq_enable(HUART_TypeDef* pBase, uint8_t value);
/**
 * @brief Clear the receive-timeout status on a supported high-speed LPUART instance.
 * @param[in] pBase LPUART peripheral instance.
 * @return 0 on success; 1 when @p pBase is AHB_LPUART, which does not support timeout status.
 */
uint8_t lpUART_clear_timeout(HUART_TypeDef* pBase);
/**
 * @brief Enable or disable the receive-timeout interrupt on a supported high-speed LPUART instance.
 * @param[in] pBase LPUART peripheral instance.
 * @param[in] value 0 to disable the interrupt; any nonzero value to enable it.
 * @return 0 on success; 1 when @p pBase is AHB_LPUART, which does not support this interrupt.
 */
uint8_t lpUART_set_timeout_irq_enable(HUART_TypeDef* pBase, uint8_t value);







#endif /* INGCHIPS_FAMILY_26 */

#ifdef __cplusplus
  }
#endif

#endif
