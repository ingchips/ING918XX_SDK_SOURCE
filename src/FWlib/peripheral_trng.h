#ifndef PERIPHERAL_TRNG_H
#define PERIPHERAL_TRNG_H
#include "ingsoc.h"
#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
typedef struct {
    __IO uint32_t Config, BitSel, DataCtrl, IntStatus, Data;
} TRNG_TypeDef;
#define APB1_TRNG ((TRNG_TypeDef *)APB1_TRNG_BASE)
/** Configure disabled generator, 32-bit samples with valid handshake. */
int TRNG_Config(TRNG_TypeDef *base, uint8_t rate, uint8_t post_process, uint32_t bit_select);
void TRNG_Enable(TRNG_TypeDef *base, uint8_t enable);
void TRNG_IntEnable(TRNG_TypeDef *base, uint8_t enable);
void TRNG_ClearInt(TRNG_TypeDef *base);
/** Nonblocking read: 0 success, 1 no sample, -1 invalid argument. */
int TRNG_Read(TRNG_TypeDef *base, uint32_t *data);

#endif /* INGCHIPS_FAMILY_26 */

#ifdef __cplusplus
}
#endif

#endif
