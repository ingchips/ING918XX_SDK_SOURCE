#include "peripheral_lpuart.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
#include "peripheral_lpuart.h"
#include "ingsoc.h"
#include <stdint.h>

/* Preserve enables/configuration, acknowledge only explicitly requested IRQ flags. */
static void LPUART_UpdateIrq(HUART_TypeDef *base, uint32_t mask, uint32_t value)
{
    uint32_t irq = base->uart_irq & ~0x54400800u;
    base->uart_irq = (irq & ~mask) | (value & mask);
}

int lpuart_bauRateSet(HUART_TypeDef* pBase, uint32_t fclk, uint32_t BaudRate, uint32_t Dividend) {
    uint32_t tmp;
    if (!fclk || !BaudRate || !Dividend || Dividend > 0x7ffu) return -1;
    tmp = (uint32_t)(((uint64_t)Dividend * BaudRate + fclk / 2u) / fclk);
    if (!tmp || tmp > 0x7ffu) return -1;
    tmp = tmp <<16;
    tmp |= Dividend & 0xFFFF; // Mask Dividend to 16 bits
    pBase->uart_baud = tmp;
    return 0;
}

uint32_t lpuart_baudRateGet(HUART_TypeDef* pBase, uint32_t fclk) {
    uint32_t baud = pBase->uart_baud;
    uint32_t dividend = baud & 0xFFFF;
    uint32_t divisior = (baud >> 16) & 0xFFFF;
    if (!dividend) return 0;
    baud = (uint32_t)((uint64_t)fclk * divisior / dividend);
    return baud;
}

uint8_t lpUART_Check_Rece_ERROR(HUART_TypeDef* pBase) {
    return (pBase->uart_irq >> 26) & (pBase == AHB_LPUART ? 1u : 5u);
}

uint8_t lpUART_Check_RXFIFO_EMPTY(HUART_TypeDef* pBase) {
    return (pBase->uart_irq >> 7) & 1u;
}

uint8_t lpUART_SendData(HUART_TypeDef* pBase, uint16_t Data) {
    // Data &= 0x1FF; // Ensure Data is within 9 bits
    pBase->uart_data = Data;
    return 0;
}

uint16_t lpUart_getData(HUART_TypeDef* pBase) {
    return pBase->uart_data & 0x1FF; // Return the data, masked to 9 bits
}

uint8_t lpUART_setb(HUART_TypeDef* pBase, uint8_t value) {
    if (value) pBase->uart_cfg |= 1 << 0;
    else  pBase->uart_cfg &= ~(1 << 0);
    return 0;
}

uint8_t lpUART_clear_fifo(HUART_TypeDef* pBase) {
    pBase->uart_cfg |= 0x1 << 1; // Set the clear FIFO bit
    return 0;
}

uint8_t lpUART_sel_pclk(HUART_TypeDef* pBase, uint8_t value) {
    if (value) {
        pBase->uart_cfg |= 0x1 << 2; // Set the PCLK bit
    } else {
        pBase->uart_cfg &= ~(0x1 << 2); // Clear the PCLK bit
    }
    return 0;
}


uint8_t lpUART_get_valid_clock(HUART_TypeDef* pBase) {
    // Check if the clock is valid
    return (pBase->uart_cfg>>3) & 0x1;
}

uint8_t lpUART_set_cmsb(HUART_TypeDef* pBase, uint8_t value) {
    if (value > 0x8) {
        value = 0x8; // Limit value to 8 bits
    }
    pBase->uart_cfg &= ~(0xF << 4); // Clear the previous value
    pBase->uart_cfg |= (value << 4);
		return 0;
}

uint8_t lpUART_set_smsb(HUART_TypeDef* pBase, uint8_t value) {
    value = value & 0x1; // One stop-bit field; bit9 controls FIFO drop
    pBase->uart_cfg &= ~(0x1 << 8); // Clear the previous value
    pBase->uart_cfg |= (value << 8);
    return 0;
}

uint8_t lpUART_set_parity(HUART_TypeDef* pBase, uint8_t value) {
    if (value) pBase->uart_cfg |= (0x1 << 10); // Set the parity bits
    else pBase->uart_cfg &= ~(0x1 << 10); // Clear the previous parity bits
    return 0;
}

uint8_t lpUART_set_even(HUART_TypeDef* pBase, uint8_t value) {
    if (value) pBase->uart_cfg |= (0x1 << 11); // Set the even parity bit
    else pBase->uart_cfg &= ~(0x1 << 11); // Clear the even parity bit
    return 0;
}

uint8_t lpUART_set_delay(HUART_TypeDef* pBase, uint8_t value) {
    value = value & 0x0F; // Ensure value is within 4 bits
    pBase->uart_cfg &= ~(0xfu << 12); // Clear the previous value
    pBase->uart_cfg |= (value << 12); // Set the new value
    return 0;
}

uint8_t lpUART_set_dma_ena(HUART_TypeDef* pBase, uint8_t value) {
    if (value) pBase->uart_cfg |= (0x1 << 16); // Set the DMA enable bit
    else pBase->uart_cfg &= ~(0x1 << 16); // Clear the DMA enable bit
		return 0;
}

uint8_t lpUART_get_cnt(HUART_TypeDef* pBase) {
    uint8_t count = (pBase->uart_cfg >> 25) & 0x1F; // Get the count bits
    return count; // Return the count value
}

uint8_t lpUART_set_fc_ena(HUART_TypeDef* pBase, uint8_t value) {
    if (value) pBase->uart_cfg |= (0x1 << 22); // Set the flow control enable bit
    else pBase->uart_cfg &= ~(0x1 << 22); // Clear the flow control enable bit
    return 0;
}

uint8_t lpUART_get_ready(HUART_TypeDef* pBase) {
    // Check if the UART is ready
    return (pBase->uart_cfg >> 23) & 0x1; // Return the ready bit
}

uint8_t lpUART_SetDir(HUART_TypeDef* pBase, uint8_t dir) {
    if (dir) pBase->uart_cfg |= 1 << 24; // Set the direction bit
    else pBase->uart_cfg &= ~(0x1 << 24); // Clear the direction bit
    return 0;
}

uint8_t lpUART_set_tmsb (HUART_TypeDef* pBase, uint8_t value) {
    if (pBase == AHB_LPUART) return 1;
    value = value & 0x7F; // Ensure value is within 7 bits
    pBase->uart_cfg &= ~(0x7F << 18); // Clear the previous value
    pBase->uart_cfg |= (value << 18); // Set the new value
    return 0;
}

uint8_t lpUART_set_fifo_th(HUART_TypeDef* pBase, uint8_t value) {
    value = value & 0x1F; // Ensure value is within 5 bits
    LPUART_UpdateIrq(pBase, 0x1F << 0, 0); // Clear the previous threshold value
    LPUART_UpdateIrq(pBase, (value << 0), (value << 0)); // Set the new threshold value
    return 0;
}

uint8_t lpUART_get_fifo_trig(HUART_TypeDef* pBase) {
    return (pBase->uart_irq >> 5) & 0x1; // Get the FIFO trigger level
}

uint8_t lpUART_set_fifo_irq_enable(HUART_TypeDef* pBase, uint8_t value) {
    if (value) LPUART_UpdateIrq(pBase, (0x1 << 6), (0x1 << 6)); // Enable FIFO interrupt
    else LPUART_UpdateIrq(pBase, 0x1 << 6, 0); // Disable FIFO interrupt
    return 0;
}

uint8_t lpUART_get_fifo_empty(HUART_TypeDef* pBase) {
    // Check if the FIFO is empty
    return (pBase->uart_irq >> 7) & 0x1; // Return the FIFO empty bit

}

uint8_t lpUART_set_fifo_empty_irq_enable(HUART_TypeDef* pBase, uint8_t value) {
    if (value) LPUART_UpdateIrq(pBase, (0x1 << 8), (0x1 << 8)); // Enable FIFO empty interrupt
    else LPUART_UpdateIrq(pBase, 0x1 << 8, 0); // Disable FIFO empty interrupt
    return 0;
}

uint8_t lpUART_get_fifo_full(HUART_TypeDef* pBase) {
    // Check if the FIFO is full
    return (pBase->uart_irq >> 9) & 0x1; // Return the FIFO full bit
}

uint8_t lpUART_set_fifo_full_irq_enable(HUART_TypeDef* pBase, uint8_t value) {
    if (value) LPUART_UpdateIrq(pBase, (0x1 << 10), (0x1 << 10)); // Enable FIFO full interrupt
        else LPUART_UpdateIrq(pBase, 0x1 << 10, 0); // Disable FIFO full interrupt
    return 0;
}

uint8_t lpUART_set_start(HUART_TypeDef* pBase, uint8_t value) {
    if (value) LPUART_UpdateIrq(pBase, (0x1 << 11), (0x1 << 11)); // Set the start bit
    else LPUART_UpdateIrq(pBase, 0x1 << 11, 0); // Clear the start bit
    return 0;
}

uint8_t lpUART_get_start(HUART_TypeDef* pBase) {
    return (pBase->uart_irq >> 11) & 0x1; // Return the start bit
}


uint8_t lpUART_set_start_irq_enable(HUART_TypeDef* pBase, uint8_t value) {
    if (value) LPUART_UpdateIrq(pBase, (0x1 << 12), (0x1 << 12)); // Enable start interrupt
    else LPUART_UpdateIrq(pBase, 0x1 << 12, 0); // Disable start interrupt
    return 0;
}

uint8_t lpUART_set_match_char(HUART_TypeDef* pBase, uint16_t value) {
    LPUART_UpdateIrq(pBase, 0x1FF << 13, 0); // Clear the previous match character
    LPUART_UpdateIrq(pBase, 0x1ffu << 13, ((uint32_t)value & 0x1ffu) << 13); // Set the new match character
    return 0;
}
uint8_t lpUART_clear_match_char(HUART_TypeDef *pBase, uint8_t value) {
    LPUART_UpdateIrq(pBase, (value << 22), (value << 22)); // Clear the match character
    return 0;
}
uint8_t lpUART_get_char_is_matched(HUART_TypeDef* pBase) {
    // Check if the character is matched
    return (pBase->uart_irq >> 22) & 0x1; // Return the character match bit
}

uint8_t lpUART_set_char_match_irq_enable(HUART_TypeDef* pBase, uint8_t value) {
    if (value) LPUART_UpdateIrq(pBase, (0x1 << 23), (0x1 << 23)); // Enable character match interrupt
    else LPUART_UpdateIrq(pBase, 0x1 << 23, 0); // Disable character match interrupt
    return 0;
}

uint8_t lpUART_set_stop(HUART_TypeDef* pBase, uint8_t value) {
    if (value) LPUART_UpdateIrq(pBase, (0x1 << 24), (0x1 << 24)); // Set the stop bit
    else LPUART_UpdateIrq(pBase, 0x1 << 24, 0); // Clear the stop bit
    return 0;
}

uint8_t lpUART_get_stop(HUART_TypeDef* pBase) {
    return (pBase->uart_irq >> 24) & 0x1; // Return the start bit
}

uint8_t lpUART_set_stop_irq_enable(HUART_TypeDef* pBase, uint8_t value) {
    if (value) LPUART_UpdateIrq(pBase, (0x1 << 25), (0x1 << 25)); // Enable stop interrupt
    else LPUART_UpdateIrq(pBase, 0x1 << 25, 0); // Disable stop interrupt
    return 0;
}
uint8_t lpUART_get_parityerr(HUART_TypeDef* pBase) {
    return (pBase->uart_irq >> 26) & 0x1; // Return the parity error bit
}
uint8_t lpUART_clear_parityerr(HUART_TypeDef* pBase) {
    LPUART_UpdateIrq(pBase, (0x1 << 26), (0x1 << 26)); // Set the clear parity error bit
    return 0;
}

uint8_t lpUART_set_parityerr_irq_enable(HUART_TypeDef* pBase, uint8_t value) {
    if (value) LPUART_UpdateIrq(pBase, (0x1 << 27), (0x1 << 27)); // Enable parity error interrupt
    else LPUART_UpdateIrq(pBase, 0x1 << 27, 0); // Disable parity error interrupt
    return 0;
}

uint8_t lpUART_clear_stoperr(HUART_TypeDef* pBase, uint8_t value) {
    if (pBase == AHB_LPUART) return 1;
    LPUART_UpdateIrq(pBase, (0x1 << 28), (0x1 << 28)); // Set the clear stop error bit
    return 0;
}
uint8_t lpUART_get_stoperr(HUART_TypeDef* pBase) {
    if (pBase == AHB_LPUART) return 0;
    return (pBase->uart_irq >> 28) & 0x1; // Return the stop error bit
}
uint8_t lpUART_set_stoperr_irq_enable(HUART_TypeDef* pBase, uint8_t value)
{
    if (pBase == AHB_LPUART) return 1;
    if (value) LPUART_UpdateIrq(pBase, (0x1 << 29), (0x1 << 29)); // Enable stop error interrupt
    else LPUART_UpdateIrq(pBase, 0x1 << 29, 0); // Disable stop error interrupt
    return 0;
}

uint8_t lpUART_clear_timeout(HUART_TypeDef* pBase) {
    if (pBase == AHB_LPUART) return 1;
    LPUART_UpdateIrq(pBase, (0x1 << 30), (0x1 << 30)); // Set the clear timeout bit
    return 0;
}

uint8_t lpUART_set_timeout_irq_enable(HUART_TypeDef* pBase, uint8_t value) {
    if (pBase == AHB_LPUART) return 1;
    if (value) LPUART_UpdateIrq(pBase, (uint32_t)(1u << 31), (uint32_t)(1u << 31)); // Enable timeout interrupt
    else LPUART_UpdateIrq(pBase, 1u << 31, 0); // Disable timeout interrupt
    return 0;
}


#endif
