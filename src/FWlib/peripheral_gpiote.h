#ifndef PERIPHERAL_GPIOTE_H
#define PERIPHERAL_GPIOTE_H
#include "ingsoc.h"
#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
/** Configure one of four GPIOTE channels; mode: 0 disabled, 1 event, 3 task. */
int GPIOTE_Config(GPIOTE_TypeDef *base, uint8_t channel, uint8_t mode, uint8_t pin, uint8_t initial);
uint32_t GPIOTE_GetConfig(GPIOTE_TypeDef *base, uint8_t channel);

#endif /* INGCHIPS_FAMILY_26 */

#ifdef __cplusplus
}
#endif

#endif
