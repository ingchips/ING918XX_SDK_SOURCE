#include "peripheral_gpio.h"

#define GPIO_PIN_IE0  ((__IO uint32_t *)(APB_PINC_BASE + 0x00))
#define GPIO_PIN_PE0  ((__IO uint32_t *)(APB_PINC_BASE + 0x10))
#define GPIO_PIN_PS0  ((__IO uint32_t *)(APB_PINC_BASE + 0x18))
#define GPIO_PIN_DS00 ((__IO uint32_t *)(APB_PINC_BASE + 0x28))
#define GPIO_PIN_DS01 ((__IO uint32_t *)(APB_PINC_BASE + 0x2c))

static void GIO_MaskedWrite(__IO uint32_t *reg, const uint8_t index, const uint8_t v)
{
    uint32_t mask = 1 << index;
    *reg = (*reg & (~mask)) | (v << index);
}

void GIO_SetDirection(const GIO_Index_t io_index, const GIO_Direction_t dir)
{
    if (dir != GIO_DIR_NONE)
    {
        GIO_MaskedWrite(GPIO_OEB, io_index, dir == GIO_DIR_INPUT ? 0 : 1);
        GIO_MaskedWrite(GPIO_PIN_IE0, io_index, dir == GIO_DIR_OUTPUT ? 1 : 0);
    }
    else
    {
        GIO_MaskedWrite(GPIO_OEB, io_index, 0);
        GIO_MaskedWrite(GPIO_PIN_IE0, io_index, 1);
    }
}

GIO_Direction_t GIO_GetDirection(const GIO_Index_t io_index)
{
    return (GIO_Direction_t)((*GPIO_OEB >> io_index) & 1);
}

void GIO_WriteValue(const GIO_Index_t io_index, const uint8_t bit)
{
    GIO_MaskedWrite(GPIO_DO, io_index, bit);
}

uint8_t GIO_ReadValue(const GIO_Index_t io_index)
{
    return (*GPIO_DI >> io_index) & 1;
}

void GIO_ConfigIntSource(const GIO_Index_t io_index, const uint8_t enable, const GIO_IntTriggerType_t type)
{
    GIO_MaskedWrite(GPIO_LV, io_index, type);
    GIO_MaskedWrite(GPIO_NE, io_index, enable & 1);
    GIO_MaskedWrite(GPIO_PE, io_index, enable >> 1);
}

uint8_t GIO_GetIntStatus(const GIO_Index_t io_index)
{
    return (*GPIO_IS >> io_index) & 1;
}

void GIO_ClearIntStatus(const GIO_Index_t io_index)
{
    GIO_MaskedWrite(GPIO_IS, io_index, 0);
}
