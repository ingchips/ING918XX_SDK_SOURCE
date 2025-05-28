#include <stdint.h>
#include "usb_config.h"
#include "usbh_core.h"
#include "usb_dwc2_reg.h"
#include "ingsoc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "platform_api.h"
#include "usb_hc.h"
#include "usb_dc.h"


#define USB_PIN_DP GIO_GPIO_16
#define USB_PIN_DM GIO_GPIO_17

#if (CONFIG_USE_USB_DEVICE == 0)&&(CONFIG_USE_USB_HOST == 0)
#error "not enable define CONFIG_USE_USB_DEVICE or CONFIG_USE_USB_HOST"
#endif
typedef enum
{
  BSP_USB_PHY_DISABLE,
  BSP_USB_PHY_ENABLE
}BSP_USB_PHY_ENABLE_e;

typedef enum
{
  BSP_USB_PHY_DP_PULL_UP = 1,
  BSP_USB_PHY_DM_PULL_UP,
  BSP_USB_PHY_DP_DM_PULL_DOWN
}BSP_USB_PHY_PULL_e;

uint32_t SystemCoreClock;


uint32_t cherrusb_hander(void *user_data)
{
    #if CONFIG_USE_USB_DEVICE
    USBD_IRQHandler(0);
    #elif CONFIG_USE_USB_HOST
    USBH_IRQHandler(0);
    #endif

    return 0;
}
#if CONFIG_USE_USB_DEVICE
void usb_dc_low_level_init(uint8_t busid)
#elif CONFIG_USE_USB_HOST
__WEAK void usb_dc_low_level_init(uint8_t busid)
{

}

void usb_hc_low_level_init(uint8_t busid)
#endif
{
    (void)busid;
    
    USB_INIT_CONFIG_T config;
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ITEM_APB_USB)|(1 << SYSCTRL_ITEM_APB_PinCtrl)
                                |(1 << SYSCTRL_ITEM_APB_GPIO0));

    //use SYSCTRL_GetClk(SYSCTRL_ITEM_APB_USB) to confirm, USB module clock has to be 48M.
    SYSCTRL_SelectUSBClk((SYSCTRL_ClkMode)(SYSCTRL_GetPLLClk()/48000000));

    platform_set_irq_callback(PLATFORM_CB_IRQ_USB, cherrusb_hander, NULL);
    PINCTRL_SelUSB(USB_PIN_DP,USB_PIN_DM);
    #if CONFIG_USE_USB_DEVICE
    SYSCTRL_USBPhyConfig(BSP_USB_PHY_ENABLE,BSP_USB_PHY_DP_PULL_UP);
    #elif CONFIG_USE_USB_HOST
    SYSCTRL_USBPhyConfig(BSP_USB_PHY_ENABLE,BSP_USB_PHY_DP_DM_PULL_DOWN);
    #endif
}
#if CONFIG_USE_USB_DEVICE
void usb_dc_low_level_deinit(uint8_t busid)
#elif CONFIG_USE_USB_HOST
__WEAK void usb_dc_low_level_deinit(uint8_t busid)
{

}

void usb_hc_low_level_deinit(uint8_t busid)
#endif
{
    USB_Close();
    SYSCTRL_SetClkGateMulti(1 << SYSCTRL_ITEM_APB_USB);

    SYSCTRL_USBPhyConfig(BSP_USB_PHY_DISABLE,0);
}

void usbd_dwc2_delay_ms(uint8_t ms)
{
    vTaskDelay(1000);
}

uint32_t usbd_get_dwc2_gccfg_conf(uint32_t reg_base)
{
    return ((1 << 16) | (1 << 21));
}

uint32_t usbh_get_dwc2_gccfg_conf(uint32_t reg_base)
{
    return ((1 << 16) | (1 << 21));
}

