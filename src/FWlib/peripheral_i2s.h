#ifndef __PERIPHERAL_I2S_H__
#define __PERIPHERAL_I2S_H__

#include <stdint.h>
#include "ingsoc.h"

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define I2S_FIFO_DEPTH      16

typedef enum
{
    I2S_ROLE_MASTER,
    I2S_ROLE_SLAVE,
} i2s_role_t;

typedef enum
{
    I2S_MODE_STANDARD,       // standard I2S
    I2S_MODE_LEFT_ALIGNED,
    I2S_MODE_PCM_0_DELAY,
    I2S_MODE_PCM_1_DELAY,
} i2s_mode_t;

typedef enum
{
    I2S_IRQ_TX = 1,
    I2S_IRQ_RX = 2
} i2s_irq_t;

/**
 * @brief Configure I2S
 *
 * @param[in] base              I2S peripheral base address
 * @param[in] role              role
 * @param[in] mode              mode
 * @param[in] mono              Mono flag: transmit same data on left/right channel (ignored for PCM modes)
 * @param[in] lrclk_polar       Polar of LRCLK (0: High level for left channel; 1: Low level for left channel)
 * @param[in] txbclk_polar      Polar of BCLK on transmission (0: Data transmitted on falling edge of BCLK; 1: rising edge)
 * @param[in] rxbclk_polar      Polar of BCLK on reception (0: Data sampled on falling edge of BCLK; 1: rising edge)
 * @param[in] data_width        Date bits width ([16..32])
 */
void I2S_Config(I2S_TypeDef *base, i2s_role_t role, i2s_mode_t mode,
                uint8_t mono,
                uint8_t lrclk_polar, uint8_t txbclk_polar, uint8_t rxbclk_polar,
                uint8_t data_width);

/**
 * @brief Configure I2S
 *
 * @param[in] base              I2S peripheral base address
 * @param[in] b_div             clock divider for BCLK
 * @param[in] lr_div            clock divider for LRCLK
 *
 * Note 1:
 *   F_bclk  = F_clk / (2 * b_div)
 *   F_lrclk = F_bclk / (2 * lr_div)
 *
 *   F_bclk should be >= F_lrclk * 48, i.e. lr_div should be >= 24.
 *
 * Note 2:
 *   (lr_div - data_width) should be >= 3.
 *
 * Note 3:
 *   Sample rate = F_lrclk
 */
void I2S_ConfigClk(I2S_TypeDef *base, uint8_t b_div, uint8_t lr_div);

/**
 * @brief Configure I2S IRQ
 *
 * @param[in] base                      I2S peripheral base address
 * @param[in] tx_irq_en                 Enable TX IRQ
 * @param[in] rx_irq_en                 Enable RX IRQ
 * @param[in] tx_irq_trigger_level      Trigger level of Tx FIFO
 * @param[in] rx_irq_trigger_level      Trigger level of Rx FIFO
 *
 * Note on Trigger level:
 *   * 0: triggering disabled
 *   * 1: triggered by 1 word
 *   * ...
 *   * 15: triggered by 15 words
 */
void I2S_ConfigIRQ(I2S_TypeDef *base, uint8_t tx_irq_en, uint8_t rx_irq_en,
                   uint8_t tx_irq_trigger_level, uint8_t rx_irq_trigger_level);

/**
 * @brief Enable/Disable I2S DMA
 *
 * Note: These two bits are "write-only", any other _write_ operation to _Config_
 *       clears this two bits.
 *
 * @param[in] base              I2S peripheral base address
 * @param[in] tx_en             Enable DMA for tansmission
 * @param[in] rx_en             Enable DMA for reception
 */
void I2S_DMAEnable(I2S_TypeDef *base, uint8_t tx_en, uint8_t rx_en);

/**
 * @brief Enable/Disable I2S transmission/reception
 *
 * @param[in] base              I2S peripheral base address
 * @param[in] tx_en             Enable tansmission
 * @param[in] rx_en             Enable reception
 */
void I2S_Enable(I2S_TypeDef *base, uint8_t tx_en, uint8_t rx_en);

/**
 * @brief Read a word from I2S Rx FIFO
 *
 * @param[in] base                      I2S peripheral base address
 * @return                              word value (see `I2S_PushTxFIFO`)
 */
uint32_t I2S_PopRxFIFO(I2S_TypeDef *base);

/**
 * @brief Push a word into I2S Tx FIFO
 *
 * Note on `value`:
 *
 * * When `data_width` == 16, each `value` represent two samples: the most significant
 *   16 bits for the left channel, while the least significant 16 bits for the right;
 * * When `data_width` > 16, each `value` represent one sample of a channel:
 *   within one snapshot of two channels, the sample for left channel comes firstly,
 *   followed by the sample for right chanel.
 *
 * @param[in] base                      I2S peripheral base address
 * @param[in] value                     Word value
 */
void I2S_PushTxFIFO(I2S_TypeDef *base, uint32_t value);

/**
 * @brief Clear I2S Rx FIFO
 *
 * @param[in] base                      I2S peripheral base address
 */
void I2S_ClearRxFIFO(I2S_TypeDef *base);

/**
 * @brief Clear I2S Tx FIFO
 *
 * @param[in] base                      I2S peripheral base address
 */
void I2S_ClearTxFIFO(I2S_TypeDef *base);

/**
 * @brief Get I2S interrupt state
 *
 * @param[in] base                      I2S peripheral base address
 * @return                              interrupt state (combination of `i2s_irq_t`)
 */
uint32_t I2S_GetIntState(I2S_TypeDef *base);

/**
 * @brief Clear I2S interrupt state
 *
 * @param[in] base                      I2S peripheral base address
 * @param[in] state                     interrupt state (combination of `i2s_irq_t`) to be cleared
 */
void I2S_ClearIntState(I2S_TypeDef *base, uint32_t state);

/**
 * @brief Get I2S Tx FIFO count
 *
 * @param[in] base                      I2S peripheral base address
 * @return                              words count in Tx FIFO ([0..I2S_FIFO_DEPTH])
 */
int I2S_GetTxFIFOCount(I2S_TypeDef *base);

/**
 * @brief Get I2S Rx FIFO count
 *
 * @param[in] base                      I2S peripheral base address
 * @return                              words count in Rx FIFO ([0..I2S_FIFO_DEPTH])
 */
int I2S_GetRxFIFOCount(I2S_TypeDef *base);

/**
 * @brief Enable/Disable data from PDM
 *
 * @param[in] enable                    Enable or disable(default)
 */
void I2S_DataFromPDM(uint8_t enable);

#endif

#ifdef	__cplusplus
}
#endif	/* __cplusplus */
#endif
