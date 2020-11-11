
#ifndef __PERIPHERAL_TIMER_H__
#define __PERIPHERAL_TIMER_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "ingsoc.h"

// we have 3 timers in total. Timer 0 can be used as watchdog.
#define TIMER_NUMBER                        3

#define TMR_CLK_FREQ                        OSC_CLK_FREQ

/**
 ****************************************************************************************
 * @brief Get counter of a timer
 *
 * @param[in] pTMR          timer address
 * @return                  counter
 ****************************************************************************************
 */
uint32_t TMR_GetCNT(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Reload a timer: set its counter to zero
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_Reload(TMR_TypeDef *pTMR);

//-----------
// TMR_CMP
//
void TMR_SetCMP(TMR_TypeDef *pTMR, uint32_t value);

/**
 ****************************************************************************************
 * @brief Set comparison value of a timer
 *
 * @param[in] pTMR          timer address
 * @param[in] value         comparison value
 ****************************************************************************************
 */
uint32_t TMR_GetCMP(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Enable a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_Enable(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Disable a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_Disable(TMR_TypeDef *pTMR);

// timer work mode
#define TMR_CTL_OP_MODE_WRAPPING            0            // 0 - continuous wrapping mode
#define TMR_CTL_OP_MODE_ONESHOT             1            // 1 - one-stop mode
#define TMR_CTL_OP_MODE_FREERUN             2            // 2 - continuous free-run mode

/**
 ****************************************************************************************
 * @brief Set work mode of a timer
 *
 * @param[in] pTMR          timer address
 * @param[in] mode          work mode
 ****************************************************************************************
 */
void TMR_SetOpMode(TMR_TypeDef *pTMR, uint8_t mode);

/**
 ****************************************************************************************
 * @brief Enable interrupt of a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_IntEnable(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Disable interrupt of a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_IntDisable(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Clear interrupt request of a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
void TMR_IntClr(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Get interrupt status of a timer
 *
 * @param[in] pTMR          timer address
 ****************************************************************************************
 */
uint8_t TMR_IntHappened(TMR_TypeDef *pTMR);

/**
 ****************************************************************************************
 * @brief Enable watchdog.
 *        Watchdog timed out after its counter counts to `timeout` for 2 times,
 *        i.e., if `timeout` == TMR_CLK_FREQ (aka 1s), watchdog timed out after 2sec.
 *
 * @param[in] timeout    timeout counter.
 ****************************************************************************************
 */
void TMR_WatchDogEnable(uint32_t timeout);

/**
 ****************************************************************************************
 * @brief Disable watchdog.
 ****************************************************************************************
 */
void TMR_WatchDogDisable(void);

/**
 ****************************************************************************************
 * @brief Watchdog restart (aka feeding)
 ****************************************************************************************
 */
#define TMR_WatchDogRestart()         TMR_Reload(APB_TMR0)

//-----------
// TMR_LOCK
//
void TMR0_LOCK(void);
void TMR0_UNLOCK(void);


#ifdef __cplusplus
  }
#endif

#endif // __CM32GPM0_UART_H__
