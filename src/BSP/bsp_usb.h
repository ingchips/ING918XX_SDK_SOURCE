#ifndef __USB_H
#define __USB_H

#include <stdint.h>
#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

// the flag to enable disconnection(cable unplugged), valid only when DP and DM are powered by VBUS
//#define FEATURE_DISCONN_DETECT
// the flag to enable WCID function
#define FEATURE_WCID_SUPPORT

// ATTENTION ! FIXED IO FOR USB on 916 series
#define USB_PIN_DP GIO_GPIO_16
#define USB_PIN_DM GIO_GPIO_17

#define USB_STRING_LANGUAGE_IDX  0x00
#define USB_STRING_LANGUAGE { 0x04, 0x03, 0x09, 0x04}

#define USB_STRING_MANUFACTURER_IDX  0x01
#define USB_STRING_MANUFACTURER {18,0x3,'I',0,'n',0,'g',0,'c',0,'h',0,'i',0,'p',0,'s',0}

#define USB_STRING_PRODUCT_IDX  0x02
#define USB_STRING_PRODUCT {16,0x3,'w',0,'i',0,'n',0,'-',0,'d',0,'e',0,'v',0}

#ifdef FEATURE_WCID_SUPPORT
#define USB_WCID_VENDOR_ID 0x17
#define USB_STRING_WCID_IDX  0xee
#define USB_STRING_WCID {0x12,0x3,'M',0,'S',0,'F',0,'T',0,'1',0,'0',0,'0',0,USB_WCID_VENDOR_ID,0}
#endif

#define USB_DEVICE_DESCRIPTOR \
{ \
    .size = sizeof(USB_DEVICE_DESCRIPTOR_REAL_T), \
    .type = 1, \
    .bcdUsb = 0x0200, \
    .usbClass = 0xFF, \
    .usbSubClass = 0x01, \
    .usbProto = 0x01, \
    .ep0Mps = USB_EP0_MPS, \
    .vendor = 0xFFF1, \
    .product = 0xFA2F, \
    .release = 0x00, \
    .iManufacturer = USB_STRING_MANUFACTURER_IDX, \
    .iProduct = USB_STRING_PRODUCT_IDX, \
    .iSerial = 0x00, \
    .nbConfig = 0x01 \
}

typedef struct
{
  USB_CONFIG_DESCRIPTOR_REAL_T config;
  USB_INTERFACE_DESCRIPTOR_REAL_T interface[1];
  USB_EP_DESCRIPTOR_REAL_T endpoint[2];
}BSP_USB_DESC_STRUCTURE_T;

#define SELF_POWERED (1)
#define REMOTE_WAKEUP (0)

#define USB_CONFIG_DESCRIPTOR \
{ \
  .size = sizeof(USB_CONFIG_DESCRIPTOR_REAL_T), \
  .type = 2, \
  .totalLength = sizeof(USB_CONFIG_DESCRIPTOR_REAL_T) + sizeof(USB_INTERFACE_DESCRIPTOR_REAL_T)*1 + \
                 sizeof(USB_EP_DESCRIPTOR_REAL_T)*2, \
  .nbInterface = 0x01, \
  .configIndex = 0x01, \
  .iDescription = 0x00, \
  .attrib = 0x80| (SELF_POWERED<<6) | (REMOTE_WAKEUP<<5), \
  .maxPower = 0xFA \
}

#define USB_INTERFACE_DESCRIPTOR \
{ \
  .size = sizeof(USB_INTERFACE_DESCRIPTOR_REAL_T), \
  .type = 4, \
  .interfaceIndex = 0x00, \
  .alternateSetting = 0x00, \
  .nbEp = 2,  \
  .usbClass = 0xFF, \
  .usbSubClass = 0x01, \
  .usbProto = 0x01, \
  .iDescription = 0x00 \
}

#define EP_IN (1)/* EP1 is in */
#define EP_OUT (2)
#define EP_X_MPS_BYTES (64)

#define USB_EP_1_DESCRIPTOR \
{ \
  .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
  .type = 5, \
  .ep = USB_EP_DIRECTION_IN(EP_IN), \
  .attributes = USB_EP_TYPE_BULK, \
  .mps = EP_X_MPS_BYTES, \
  .interval = 0 \
}

#define USB_EP_2_DESCRIPTOR \
{ \
  .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
  .type = 5, \
  .ep = USB_EP_DIRECTION_OUT(EP_OUT), \
  .attributes = USB_EP_TYPE_BULK, \
  .mps = EP_X_MPS_BYTES, \
  .interval = 0 \
}
#ifdef FEATURE_WCID_SUPPORT
#define USB_WCID_DESCRIPTOR_INDEX_4 \
{ \
  0x28, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x01, \
  'W','I','N','U','S','B', \
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
}

#define USB_WCID_DESCRIPTOR_INDEX_5 \
{ \
  0x8e, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x00, 0x01, 0x00, 0x84, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x28, 0x00, \
  'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,'G',0,'U',0,'I',0,'D',0, \
  0x00, 0x00, 0x4e, 0x00, 0x00, 0x00, \
  '{',0,'7',0,'8',0,'9',0,'A',0,'4',0,'2',0,'C',0,'7',0,'-',0,'F',0,'B',0,'2',0,'5',0,'-',0,'4',0,'4',0,'3',0,'B',0,'-',0, \
  '9',0,'E',0,'7',0,'E',0,'-',0,'A',0,'4',0,'2',0,'6',0,'0',0,'F',0,'3',0,'7',0,'3',0,'9',0,'8',0,'2',0,'}',0, \
  0x00, 0x00 \
}
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

typedef struct
{
  uint32_t remote_wakeup:1;
  uint32_t unused:31;
}BSP_USB_VAR_s;

extern void bsp_usb_init(void);
extern void bsp_usb_device_remote_wakeup(void);

#ifdef FEATURE_DISCONN_DETECT
void bsp_usb_device_disconn_timeout(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
