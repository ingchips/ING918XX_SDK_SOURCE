import
  ingsoc

type
  GIO_Index_t* {.size: sizeof(cint).} = enum
    GIO_GPIO_0, GIO_GPIO_1, GIO_GPIO_2, GIO_GPIO_3, GIO_GPIO_4, GIO_GPIO_5,
    GIO_GPIO_6, GIO_GPIO_7, GIO_GPIO_8, GIO_GPIO_9, GIO_GPIO_10, GIO_GPIO_11,
    GIO_GPIO_12, GIO_GPIO_13, GIO_GPIO_14, GIO_GPIO_15, GIO_GPIO_16, GIO_GPIO_17,
    GIO_GPIO_18, GIO_GPIO_19, GIO_GPIO_20, GIO_GPIO_21, GIO_GPIO_22, GIO_GPIO_23,
    GIO_GPIO_24, GIO_GPIO_25, GIO_GPIO_26, GIO_GPIO_27, GIO_GPIO_28, GIO_GPIO_29,
    GIO_GPIO_30, GIO_GPIO_31
  GIO_Direction_t* {.size: sizeof(cint).} = enum
    GIO_DIR_INPUT, GIO_DIR_OUTPUT
  GIO_IntTriggerEnable_t* {.size: sizeof(cint).} = enum
    GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE = 1,
    GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE = 2
  GIO_IntTriggerType_t* {.size: sizeof(cint).} = enum
    GIO_INT_EDGE, GIO_INT_LOGIC
  GIO_PullType_t* {.size: sizeof(cint).} = enum
    GIO_PULL_UP, GIO_PULL_DOWN






const
  GPIO_DI* = (cast[ptr uint32]((APB_GIO_BASE + 0x00000000)))
  GPIO_DO* = (cast[ptr uint32]((APB_GIO_BASE + 0x00000010)))
  GPIO_OEB* = (cast[ptr uint32]((APB_GIO_BASE + 0x00000020)))
  GPIO_IS* = (cast[ptr uint32]((APB_GIO_BASE + 0x00000040))) ##  interrupt status
  GPIO_LV* = (cast[ptr uint32]((APB_GIO_BASE + 0x00000050))) ##  interrupt type 0-edge 1-level
  GPIO_PE* = (cast[ptr uint32]((APB_GIO_BASE + 0x00000060))) ##  Positive edge and High Level interrupt enable
  GPIO_NE* = (cast[ptr uint32]((APB_GIO_BASE + 0x00000070))) ##  Negtive edge and Low Level interrupt enable

## *
##  @brief Set I/O direction of a GPIO
##
##  @param[in] io_index          the GPIO
##  @param[in] dir               I/O direction
##

proc GIO_SetDirection*(io_index: GIO_Index_t; dir: GIO_Direction_t) {.
    importc: "GIO_SetDirection", header: "peripheral_gpio.h".}
## *
##  @brief Set pull state of a GPIO
##
##  @param[in] io_index          the GPIO
##  @param[in] enable            enabled or not
##  @param[in] type              pull up or down
##

proc GIO_SetPull*(io_index: GIO_Index_t; enable: uint8; `type`: GIO_PullType_t) {.
    importc: "GIO_SetPull", header: "peripheral_gpio.h".}
## *
##  @brief Set driven capacity of a GPIO
##
##  @param[in] io_index          the GPIO
##  @param[in] index             index of driven capacity (0: weakest; 2: strongest)
##

proc GIO_SetDrivenCapacity*(io_index: GIO_Index_t; index: uint8) {.
    importc: "GIO_SetDrivenCapacity", header: "peripheral_gpio.h".}
## *
##  @brief Get current I/O direction of a GPIO
##
##  @param[in] io_index          the GPIO
##  @return                      I/O direction
##

proc GIO_GetDirection*(io_index: GIO_Index_t): GIO_Direction_t {.
    importc: "GIO_GetDirection", header: "peripheral_gpio.h".}
## *
##  @brief Write output value of a GPIO
##
##  @param[in] io_index          the GPIO
##  @param[in] bit               value
##

proc GIO_WriteValue*(io_index: GIO_Index_t; bit: uint8) {.importc: "GIO_WriteValue",
    header: "peripheral_gpio.h".}
## *
##  @brief Write output value of all 32 GPIO
##
##  @param[in] value               value
##

proc GIO_WriteAll*(value: uint32) =
  GPIO_DO[] = value

## *
##  @brief Get current value of a GPIO
##
##  @param[in] io_index          the GPIO
##  @return                      value
##

proc GIO_ReadValue*(io_index: GIO_Index_t): uint8 {.importc: "GIO_ReadValue",
    header: "peripheral_gpio.h".}
## *
##  @brief Get current value of all 32 GPIO
##
##  @return                      value
##

proc GIO_ReadAll*(): uint32 =
  return GPIO_DI[]

## *
##  @brief Config interrupt trigger type of a GPIO
##
##  @param[in] io_index          the GPIO
##  @param[in] enable            a combination of GIO_IntTriggerEnable_t.
##                               If = 0, interrupt triggering is disable for this GPIO.
##  @param[in] type              logic or edge
##

proc GIO_ConfigIntSource*(io_index: GIO_Index_t; enable: uint8;
                         `type`: GIO_IntTriggerType_t) {.
    importc: "GIO_ConfigIntSource", header: "peripheral_gpio.h".}
## *
##  @brief Get current interrupt status of a GPIO
##
##  @param[in] io_index          the GPIO
##  @return                      if interrup is triggered by this GPIO
##

proc GIO_GetIntStatus*(io_index: GIO_Index_t): uint8 {.importc: "GIO_GetIntStatus",
    header: "peripheral_gpio.h".}
## *
##  @brief Get current interrupt status of all GPIO
##
##  @return                      interrupt status
##

proc GIO_GetAllIntStatus*(): uint32 =
  return GPIO_IS[]

## *
##  @brief Clear current interrupt status of a GPIO
##
##  @param[in] io_index          the GPIO
##

proc GIO_ClearIntStatus*(io_index: GIO_Index_t) {.importc: "GIO_ClearIntStatus",
    header: "peripheral_gpio.h".}
## *
##  @brief Clear current interrupt status of all GPIO
##
##

proc GIO_ClearAllIntStatus*() =
  GPIO_IS[] = 0
