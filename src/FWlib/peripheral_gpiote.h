#ifndef PERIPHERAL_GPIOTE_H
#define PERIPHERAL_GPIOTE_H
#include "ingsoc.h"
#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
/**
 * @brief Configure a GPIOTE channel.
 * @param[in] base GPIOTE controller instance.
 * @param[in] channel Channel index in the range 0 to 3.
 * @param[in] mode Channel mode: 0 disabled, 1 event, or 3 task.
 * @param[in] pin GPIO pin index in the range 0 to 55.
 * @param[in] initial Initial task output level; zero selects low and nonzero selects high.
 * @return 0 on success; -1 if an argument is invalid.
 */
int GPIOTE_Config(GPIOTE_TypeDef *base, uint8_t channel, uint8_t mode, uint8_t pin, uint8_t initial);

/**
 * @brief Read the raw configuration of a GPIOTE channel.
 * @param[in] base GPIOTE controller instance.
 * @param[in] channel Channel index in the range 0 to 3.
 * @return Raw channel configuration value, or 0 if an argument is invalid.
 */
uint32_t GPIOTE_GetConfig(GPIOTE_TypeDef *base, uint8_t channel);

#endif /* INGCHIPS_FAMILY_26 */

#ifdef __cplusplus
}
#endif

#endif
