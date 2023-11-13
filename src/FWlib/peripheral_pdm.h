#ifndef __PERIPHERAL_PDM_H__
#define __PERIPHERAL_PDM_H__

#include <stdint.h>
#include "ingsoc.h"

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

/**
 * @brief Config PDM
 *
 * @param[in] base                      PDM peripheral base address
 * @param[in] left_en                   Enable/Disable left channel
 * @param[in] right_en                  Enable/Disable right channel
 * @param[in] hpf_en                    Enable/Disable HPF
 * @param[in] i2s_slave_mode            Enable/Disable I2S slave mode
 * @param[in] i2s_slave_bitwidth        I2S slave mode bitwidth (0: 24bits: 1: 32bits)
 */
void PDM_Config(PDM_TypeDef *base, uint8_t left_en, uint8_t right_en,
                uint8_t hpf_en,
                uint8_t i2s_slave_mode, uint8_t i2s_slave_bitwidth);

/**
 * @brief Start/stop PDM
 *
 * @param[in] base              PDM peripheral base address
 * @param[in] enable            Enable/start (1) or disable/stop (0)
 */
void PDM_Start(PDM_TypeDef *base, uint8_t enable);

/**
 * @brief Enable/Disable DMA transfer
 *
 * @param[in] base              PDM peripheral base address
 * @param[in] trig_cfg          DMA trigger configure ([0..7])
 * @param[in] enable            enable(1)/disable(0)
 */
void PDM_DmaEnable(PDM_TypeDef *base, uint8_t trig_cfg, uint8_t enable);

#endif

#ifdef	__cplusplus
}
#endif	/* __cplusplus */

#endif
