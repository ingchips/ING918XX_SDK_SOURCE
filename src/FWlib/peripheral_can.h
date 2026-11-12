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
/**
 * @brief Configure the nominal and data-phase CAN bit timing.
 * @param[in] base CAN controller instance.
 * @param[in] nominal Encoded nominal bit-timing register value.
 * @param[in] data Encoded data-phase bit-timing register value.
 * @return 0 on success; -1 if an argument is invalid or the controller is enabled.
 */
int CAN_SetTiming(CAN_TypeDef *base, uint32_t nominal, uint32_t data);

/**
 * @brief Enable or disable the CAN controller.
 * @param[in] base CAN controller instance.
 * @param[in] enable Nonzero to enable the controller; zero to place it in configuration mode.
 */
void CAN_Enable(CAN_TypeDef *base, uint8_t enable);

/**
 * @brief Configure an acceptance filter entry.
 * @param[in] base CAN controller instance.
 * @param[in] index Filter entry index in the range 0 to 15.
 * @param[in] code Acceptance code value.
 * @param[in] mask Acceptance mask value.
 * @return 0 on success; -1 if an argument is invalid or the controller is enabled.
 */
int CAN_SetFilter(CAN_TypeDef *base, uint8_t index, uint32_t code, uint32_t mask);

/**
 * @brief Read the raw CAN controller status and control register.
 * @param[in] base CAN controller instance.
 * @return Raw Ctrl0 register value.
 */
uint32_t CAN_GetStatus(CAN_TypeDef *base);

/**
 * @brief Submit one raw transmit slot to the CAN controller.
 * @param[in] base CAN controller instance.
 * @param[in] words Array containing the 18 hardware transmit-slot words.
 * @return 0 on success; 1 if the controller is busy; -1 if an argument is invalid.
 * @note The caller must encode the frame using the layout required by the target CAN IP version.
 */
int CAN_TransmitSlot(CAN_TypeDef *base, const uint32_t words[18]);

/**
 * @brief Read one raw receive slot from the CAN controller.
 * @param[in] base CAN controller instance.
 * @param[out] words Buffer receiving the 20 hardware receive-slot words.
 * @return 0 on success; 1 if no frame is available; -1 if an argument is invalid.
 */
int CAN_ReceiveSlot(CAN_TypeDef *base, uint32_t words[20]);

#endif /* INGCHIPS_FAMILY_26 */

#ifdef __cplusplus
}
#endif

#endif
