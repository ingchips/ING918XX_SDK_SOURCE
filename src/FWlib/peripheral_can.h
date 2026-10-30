#ifndef PERIPHERAL_CAN_H
#define PERIPHERAL_CAN_H
#include "ingsoc.h"
#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
/* Caller supplies the verified CAN instance address; published maps disagree. */
typedef struct {
    __IO uint32_t RxBuffer[20], TxBuffer[18], TxTimestamp[2];
    __IO uint32_t Ctrl0, Ctrl1, NominalTiming, DataTiming, Error, FilterSelect, FilterCode;
} CAN_TypeDef;
/** Caller provides encoded timing fields from the CAN design specification. */
int CAN_SetTiming(CAN_TypeDef *base, uint32_t nominal, uint32_t data);
void CAN_Enable(CAN_TypeDef *base, uint8_t enable);
int CAN_SetFilter(CAN_TypeDef *base, uint8_t index, uint32_t code, uint32_t mask);
uint32_t CAN_GetStatus(CAN_TypeDef *base);
/** Raw hardware slot transfers; 0 success, 1 busy/empty, -1 invalid argument.
 *  No frame byte layout is inferred: supply slots matching the target IP version.
 */
int CAN_TransmitSlot(CAN_TypeDef *base, const uint32_t words[18]);
int CAN_ReceiveSlot(CAN_TypeDef *base, uint32_t words[20]);

#endif /* INGCHIPS_FAMILY_26 */

#ifdef __cplusplus
}
#endif

#endif
