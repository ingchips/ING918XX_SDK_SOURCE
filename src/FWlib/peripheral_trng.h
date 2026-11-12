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
/**
 * @brief Configure the TRNG while leaving it disabled.
 * @param[in] base TRNG controller instance.
 * @param[in] rate Sampling-rate field written to the hardware configuration register.
 * @param[in] post_process Nonzero to enable hardware post-processing.
 * @param[in] bit_select Bit-selection mask; only the low 19 bits are valid.
 * @return 0 on success; -1 if an argument is invalid.
 */
int TRNG_Config(TRNG_TypeDef *base, uint8_t rate, uint8_t post_process, uint32_t bit_select);

/**
 * @brief Enable or disable random-number generation.
 * @param[in] base TRNG controller instance.
 * @param[in] enable Nonzero to enable generation; zero to disable it.
 */
void TRNG_Enable(TRNG_TypeDef *base, uint8_t enable);

/**
 * @brief Enable or disable the TRNG interrupt.
 * @param[in] base TRNG controller instance.
 * @param[in] enable Nonzero to enable the interrupt; zero to mask it.
 */
void TRNG_IntEnable(TRNG_TypeDef *base, uint8_t enable);

/**
 * @brief Clear the pending TRNG interrupt.
 * @param[in] base TRNG controller instance.
 */
void TRNG_ClearInt(TRNG_TypeDef *base);

/**
 * @brief Read one 32-bit random sample without blocking.
 * @param[in] base TRNG controller instance.
 * @param[out] data Receives the random sample.
 * @return 0 on success; 1 if no sample is ready; -1 if an argument or hardware mode is invalid.
 */
int TRNG_Read(TRNG_TypeDef *base, uint32_t *data);

#endif /* INGCHIPS_FAMILY_26 */

#ifdef __cplusplus
}
#endif

#endif
