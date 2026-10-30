#ifndef PERIPHERAL_TIMER_COUNTER_H
#define PERIPHERAL_TIMER_COUNTER_H

#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)

// ETMR_CTL
#define bsETMR_CTL_TMR_EN                    0            // timer enable
#define bsETMR_CTL_RELOAD                    1            // when set, timer counter clear zero.
#define bsETMR_CTL_MODE                      2

#define bsETMR_CTL_INT_EN                    4            // int enable
#define bsETMR_CTL_INT_ERR_EN                5            // int error enable
#define bsETMR_CTL_INT_STATUS                6            // read as int status, write as int clear
#define bwETMR_CTL_INT_ERR_STATUS            7

#define bsETMR_CTL_HALT_EN                   8           // hardware do not support for all three timer 0, 1, 2.
#define bsETMR_CTL_LOCK                      9           // lock control
#define bsETMR_CTL_HIT_STATUS                10          // read as hit status
#define bsETMR_CTL_READ_LOCK                 11          // read as lock status

#define ETMR_ONE_SHOT_MODE 1
#define ETMR_FREE_RUN_MODE 0

#define INT_ENABLE      (0x00000010)
#define INT_ERR_ENABLE  (0x00000020)
#define INT_STATUS      (0x00000040)
#define INT_ERR_STATUS  (0x00000080)

/**
 ****************************************************************************************
 * @brief Get counter of a timer
 *
 * @param[in] pTMR          timer address
 * @return                  counter
 ****************************************************************************************
 */
uint32_t ETMR_GetCNT(ETMR_TypeDef *pTMR);

/**
 *
 * @param pTMR            timer address
 * @return               none
 */
void ETMR_ReadLock(ETMR_TypeDef* pTMR);

/**
 *
 * @param pTMR            timer address
 * @return               raw RDY handshake bit: 1 busy, 0 read completed
 */
uint32_t ETMR_GetReadReady(ETMR_TypeDef* pTMR);
/**
 ****************************************************************************************
 * @brief Set enable or disable reload of a timer
 * @param pTMR            timer address
 * @param enable          enable or disable
 * @return               none
 ****************************************************************************************
 */
void ETMR_SetReload(ETMR_TypeDef* pTMR, uint8_t enable);

/**
 ****************************************************************************************
 * @brief Set value of a timer
 * @param pTMR            timer address
 * @param value           value to set
 * @return               none
 ****************************************************************************************
 */
void ETMR_SetValue(ETMR_TypeDef* pTMR, uint32_t value);

/**
 **
 * @brief Enable of a timer
 * @param pTMR            timer address
 * @param enable          enable or disable
 * @return               none
 **
 */
void ETMR_Cmd(ETMR_TypeDef* pTMR, uint8_t enable);

/**
 *
 * @param pTMR            timer address
 * @param mode            mode to set
 * @return               none
 */
void ETMR_SetMode(ETMR_TypeDef* pTMR, uint8_t mode);

/**
 * @brief Set interrupt of a timer
 * @param pTMR            timer address
 * @param IntIT           interrupt to set
 * @param enable          enable or disable
 * @return               none
 */
void ETMR_IntCmd(ETMR_TypeDef* pTMR, uint8_t IntIT, uint8_t enable);

/**
 * @brief Clear interrupt of a timer
 * @param pTMR            timer address
 * @param IntIT           interrupt to clear
 * @return               none
 */
void ETMR_IntClr(ETMR_TypeDef* pTMR, uint8_t IntIT);

/**
 *
 * @param pTMR            timer address
 * @param IntIT           interrupt to get status
 * @return               status of interrupt
 */
uint8_t ETMR_GetStatus(ETMR_TypeDef* pTMR, uint8_t IntIT);

/**
 *
 * @param pTMR            timer address
 * @param enable          enable or disable
 * @return               none
 */
void ETMR_HaltCmd(ETMR_TypeDef* pTMR, uint8_t enable);

/**
 *
 * @param pTMR            timer address
 * @param enable          enable or disable
 * @return               none
 */
void ETMR_LockCmd(ETMR_TypeDef* pTMR, uint8_t enable);

/**
 *
 * @param pTMR            timer address
 * @return               hit status
 */
uint8_t ETMR_GetHit(ETMR_TypeDef* pTMR);

#endif /* ING26 */
#ifdef __cplusplus
}
#endif

#endif //PERIPHERAL_TIMER_COUNTER_H
