#ifndef __PERIPHERAL_GPIO_H__
#define __PERIPHERAL_GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "ingsoc.h"

typedef enum
{
    GIO_GPIO_0  ,
    GIO_GPIO_1  ,
    GIO_GPIO_2  ,
    GIO_GPIO_3  ,
    GIO_GPIO_4  ,
    GIO_GPIO_5  ,
    GIO_GPIO_6  ,
    GIO_GPIO_7  ,
    GIO_GPIO_8  ,
    GIO_GPIO_9  ,
    GIO_GPIO_10 ,
    GIO_GPIO_11 ,
    GIO_GPIO_12 ,
    GIO_GPIO_13 ,
    GIO_GPIO_14 ,
    GIO_GPIO_15 ,
    GIO_GPIO_16 ,
    GIO_GPIO_17 ,
    GIO_GPIO_18 ,
    GIO_GPIO_19 ,
    GIO_GPIO_20 ,
    GIO_GPIO_21 ,
    GIO_GPIO_22 ,
    GIO_GPIO_23 ,
    GIO_GPIO_24 ,
    GIO_GPIO_25 ,
    GIO_GPIO_26 ,
    GIO_GPIO_27 ,
    GIO_GPIO_28 ,
    GIO_GPIO_29 ,
    GIO_GPIO_30 ,
    GIO_GPIO_31 ,
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    GIO_GPIO_32 ,
    GIO_GPIO_33 ,
    GIO_GPIO_34 ,
    GIO_GPIO_35 ,
#endif
} GIO_Index_t;

typedef enum
{
    GIO_DIR_INPUT,
    GIO_DIR_OUTPUT,
    GIO_DIR_BOTH,
    GIO_DIR_NONE    // set to tri-state
} GIO_Direction_t;

typedef enum
{
    GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE = 1,
    GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE = 2
} GIO_IntTriggerEnable_t;

typedef enum
{
    GIO_INT_EDGE,
    GIO_INT_LOGIC
} GIO_IntTriggerType_t;

typedef enum
{
    GIO_PULL_UP,
    GIO_PULL_DOWN
} GIO_PullType_t;

/**
 * @brief Set I/O direction of a GPIO
 *
 * @param[in] io_index          the GPIO
 * @param[in] dir               I/O direction
 */
void GIO_SetDirection(const GIO_Index_t io_index, const GIO_Direction_t dir);

/**
 * @brief Get current I/O direction of a GPIO
 *
 * @param[in] io_index          the GPIO
 * @return                      I/O direction
 */
GIO_Direction_t GIO_GetDirection(const GIO_Index_t io_index);

/**
 * @brief Write output value of a GPIO
 *
 * @param[in] io_index          the GPIO
 * @param[in] bit               value
 */
void GIO_WriteValue(const GIO_Index_t io_index, const uint8_t bit);

/**
 * @brief Get current value of a GPIO
 *
 * @param[in] io_index          the GPIO
 * @return                      value
 */
uint8_t GIO_ReadValue(const GIO_Index_t io_index);

/**
 * @brief Config interrupt trigger type of a GPIO
 *
 * @param[in] io_index          the GPIO
 * @param[in] enable            a combination of GIO_IntTriggerEnable_t.
 *                              If = 0, interrupt triggering is disable for this GPIO.
 * @param[in] type              logic or edge
 */
void GIO_ConfigIntSource(const GIO_Index_t io_index, const uint8_t enable, const GIO_IntTriggerType_t type);

/**
 * @brief Get current interrupt status of a GPIO
 *
 * @param[in] io_index          the GPIO
 * @return                      if interrup is triggered by this GPIO
 */
uint8_t GIO_GetIntStatus(const GIO_Index_t io_index);

/**
 * @brief Clear current interrupt status of a GPIO
 *
 * @param[in] io_index          the GPIO
 */
void GIO_ClearIntStatus(const GIO_Index_t io_index);

void GIO_ClearBits(void);

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define GPIO_DI  ((__IO uint32_t *)(APB_GIO_BASE+0x00))
#define GPIO_DO  ((__IO uint32_t *)(APB_GIO_BASE+0x10))
#define GPIO_OEB ((__IO uint32_t *)(APB_GIO_BASE+0x20))
#define GPIO_IS  ((__IO uint32_t *)(APB_GIO_BASE+0x40)) // interrupt status
#define GPIO_LV  ((__IO uint32_t *)(APB_GIO_BASE+0x50)) // interrupt type 0-edge 1-level
#define GPIO_PE  ((__IO uint32_t *)(APB_GIO_BASE+0x60)) // Positive edge and High Level interrupt enable
#define GPIO_NE  ((__IO uint32_t *)(APB_GIO_BASE+0x70)) // Negtive edge and Low Level interrupt enable
#define GPIO_PIN_LED  ((__IO uint32_t *)(APB_GIO_BASE+0x10))

/**
 * @brief Write output value of all 32 GPIO
 *
 * @param[in] value               value
 */
static __INLINE void GIO_WriteAll(const uint32_t value) { *GPIO_DO = value; }

/**
 * @brief Get current value of all 32 GPIO
 *
 * @return                      value
 */
static __INLINE uint32_t GIO_ReadAll(void) { return *GPIO_DI; }

/**
 * @brief Get current interrupt status of all GPIO
 *
 * @return                      interrupt status
 */
static __INLINE uint32_t GIO_GetAllIntStatus(void)  { return (*GPIO_IS); }

/**
 * @brief Clear current interrupt status of all GPIO
 *
 */
static __INLINE void GIO_ClearAllIntStatus(void) { *GPIO_IS = 0; }

/**
 * @brief 
 * 
 */
static __INLINE void GIO_SetBits(void){ (*GPIO_PIN_LED) |= 0x1;
                                            (*GPIO_PIN_LED) &= (~0x1);};

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

typedef enum
{
    GIO_DB_CLK_32K,
    GIO_DB_CLK_PCLK,
} GIO_DbClk_t;

/**
 * @brief Set debounce parameters of GPIOs
 *
 * @param[in] group_mask        Group mask of GPIOs
 *      Note: There are two block of GPIOs, each has 18 GPIOs. Each group has its own debounce parameters.
 *            Each bit specifies one group.
 * @param[in] clk_pre_scale     Debounce clock pre scaling (0..255)
 * @param[in] clk               clock type for debounce
 */
void GIO_DebounceCtrl(uint8_t group_mask, uint8_t clk_pre_scale, GIO_DbClk_t clk);

/**
 * @brief Enable debounce feature of GPIOs
 *
 * @param[in] io_index          the GPIO
 * @param[in] enable            enable or disable
 */
void GIO_DebounceEn(const GIO_Index_t io_index, uint8_t enable);

/**
 * @brief Write output value of all GPIOs
 *
 * @param[in] value               value
 */
void GIO_WriteAll(const uint64_t value);

/**
 * @brief Get current value of all GPIOs
 *
 * @return                      value
 */
uint64_t GIO_ReadAll(void);

/**
 * @brief Get current interrupt status of all GPIO
 *
 * @return                      interrupt status
 */
uint64_t GIO_GetAllIntStatus(void);

/**
 * @brief Clear current interrupt status of all GPIO
 *
 */
void GIO_ClearAllIntStatus(void);

#endif

#ifdef __cplusplus
  }
#endif

#endif
