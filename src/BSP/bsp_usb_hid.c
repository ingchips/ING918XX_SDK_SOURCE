#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "platform_api.h"
#include "bsp_usb_hid.h"

const USB_DEVICE_DESCRIPTOR_REAL_T DeviceDescriptor __attribute__ ((aligned (4))) = USB_DEVICE_DESCRIPTOR;
const BSP_USB_DESC_STRUCTURE_T ConfigDescriptor __attribute__ ((aligned (4))) =
{USB_CONFIG_DESCRIPTOR,USB_INTERFACE_DESCRIPTOR_KB, USB_HID_DESCRIPTOR_KB,{USB_EP_IN_DESCRIPTOR_KB},
USB_INTERFACE_DESCRIPTOR_MO, USB_HID_DESCRIPTOR_MO,{USB_EP_IN_DESCRIPTOR_MO}};
const uint8_t StringDescriptor_0[] __attribute__ ((aligned (4))) = USB_STRING_LANGUAGE;
const uint8_t StringDescriptor_1[] __attribute__ ((aligned (4))) = USB_STRING_MANUFACTURER;
const uint8_t StringDescriptor_2[] __attribute__ ((aligned (4))) = USB_STRING_PRODUCT;
uint8_t DynamicDescriptor[64] __attribute__ ((aligned (4)));
const uint8_t ReportMouseDescriptor[] __attribute__ ((aligned (4))) = USB_HID_MOUSE_REPORT_DESCRIPTOR;
const uint8_t ReportKeybDescriptor[] __attribute__ ((aligned (4))) = USB_HID_KB_REPORT_DESCRIPTOR;

BSP_USB_VAR_s UsbVar;
uint8_t DataSendBuf[EP_X_MPS_BYTES] __attribute__ ((aligned (4)));

BSP_KEYB_DATA_s KeybReport __attribute__ ((aligned (4))) = {.pending = U_TRUE};
BSP_MOUSE_DATA_s MouseReport __attribute__ ((aligned (4))) = {.pending = U_TRUE};
static uint32_t bsp_usb_event_handler(USB_EVNET_HANDLER_T *event)
{
  uint32_t size;
  uint32_t status = USB_ERROR_NONE;

  switch(event->id)
  {
    case USB_EVENT_DEVICE_RESET:
    {
      #ifdef FEATURE_DISCONN_DETECT
      platform_set_timer(bsp_usb_device_disconn_timeout,160);
      #endif
    }break;
    case USB_EVENT_DEVICE_SOF:
    {
      // handle sof, need enable interrupt in config.intmask
    }break;
    case USB_EVENT_DEVICE_SUSPEND:
    {
      // handle suspend, need enable interrupt in config.intmask
    }break;
    case USB_EVENT_DEVICE_RESUME:
    {
      // handle resume, need enable interrupt in config.intmask
    }break;
    case USB_EVENT_EP0_SETUP:
    {
      USB_SETUP_T* setup = USB_GetEp0SetupData();
      switch(setup->bmRequestType.Recipient)
      {
        case USB_REQUEST_DESTINATION_DEVICE:
        {
          switch(setup->bRequest)
          {
            case USB_REQUEST_DEVICE_SET_ADDRESS:
            {
              // handled internally
              #ifdef FEATURE_DISCONN_DETECT
              platform_set_timer(bsp_usb_device_disconn_timeout,0);
              #endif
              status = USB_ERROR_NONE;
            }
            break;
            case USB_REQUEST_DEVICE_CLEAR_FEATURE:
            {
              UsbVar.remote_wakeup = (setup->wValue&0xF) ? 0 : 1;
              status = USB_ERROR_NONE;
            }
            break;
            case USB_REQUEST_DEVICE_SET_FEATURE:
            {
              UsbVar.remote_wakeup = (setup->wValue&0xF) ? 1 : 0;
              status = USB_ERROR_NONE;
            }
            break;
            case USB_REQUEST_DEVICE_SET_CONFIGURATION:
            {
              // uint8_t cfg_idx = setup->wValue&0xFF;
              // check if the cfg_idx is correct
              status |= USB_ConfigureEp(&(ConfigDescriptor.ep_kb[0]));
              status |= USB_ConfigureEp(&(ConfigDescriptor.ep_mo[0]));

            }
            break;
            case USB_REQUEST_DEVICE_GET_DESCRIPTOR:
            {
              switch(setup->wValue >> 8)
              {
                case USB_REQUEST_DEVICE_DESCRIPTOR_DEVICE:
                {
                  size = sizeof(USB_DEVICE_DESCRIPTOR_REAL_T);
                  size = (setup->wLength < size) ? (setup->wLength) : size;

                  status |= USB_SendData(0, (void*)&DeviceDescriptor, size, 0);
                }
                break;
                case USB_REQUEST_DEVICE_DESCRIPTOR_CONFIGURATION:
                {
                  size = sizeof(BSP_USB_DESC_STRUCTURE_T);
                  size = (setup->wLength < size) ? (setup->wLength) : size;

                  status |= USB_SendData(0, (void*)&ConfigDescriptor, size, 0);
                }
                break;
                case USB_REQUEST_DEVICE_DESCRIPTOR_STRING:
                {
                  const uint8_t *addr;
                  switch(setup->wValue&0xFF)
                  {
                    case USB_STRING_LANGUAGE_IDX:
                    {
                      size = sizeof(StringDescriptor_0);
                      addr = StringDescriptor_0;
                    }break;
                    case USB_STRING_MANUFACTURER_IDX:
                    {
                      size = sizeof(StringDescriptor_1);
                      addr = StringDescriptor_1;
                    }break;
                    case USB_STRING_PRODUCT_IDX:
                    {
                      size = sizeof(StringDescriptor_2);
                      addr = StringDescriptor_2;
                    }break;
                  }

                  size = (setup->wLength < size) ? (setup->wLength) : size;
                  status |= USB_SendData(0, (void*)addr, size, 0);
                }
                break;
                default:
                {
                  status = USB_ERROR_REQUEST_NOT_SUPPORT;
                }break;
              }
            }
            break;
            case USB_REQUEST_DEVICE_GET_STATUS:
            {
              DynamicDescriptor[0] = SELF_POWERED | (REMOTE_WAKEUP << 1);
              DynamicDescriptor[1] = 0;
              status |= USB_SendData(0, DynamicDescriptor, 2, 0);
            }
            break;
            case USB_REQUEST_DEVICE_GET_CONFIGURATION:
            {
              DynamicDescriptor[0] = ConfigDescriptor.config.configIndex;
              status |= USB_SendData(0, DynamicDescriptor, 1, 0);
            }
            break;
            default:
            {
              status = USB_ERROR_REQUEST_NOT_SUPPORT;
            }break;
          }
        }
        break;

        case USB_REQUEST_DESTINATION_INTERFACE:
        {
          switch(setup->bRequest)
          {
            //This request is mandatory and must be supported by all devices.
            case USB_REQUEST_HID_CLASS_REQUEST_GET_REPORT:
            {
              switch(((setup->wValue)>>8)&0xFF)
              {
                case USB_REQUEST_HID_CLASS_REQUEST_REPORT_INPUT:
                {
                  switch(setup->wIndex)
                  {
                    case 0:
                    {
                      USB_SendData(0, (void*)&KeybReport, sizeof(BSP_KEYB_REPORT_s), 0);
                    }break;
                    case 1:
                    {
                      USB_SendData(0, (void*)&MouseReport, sizeof(BSP_MOUSE_REPORT_s), 0);
                    }break;
                  }
                }break;
                default:
                {
                  status = USB_ERROR_REQUEST_NOT_SUPPORT;
                }break;
              }
            }break;
            case USB_REQUEST_HID_CLASS_REQUEST_SET_REPORT:
            {
              switch(((setup->wValue)>>8)&0xFF)
              {
                case USB_REQUEST_HID_CLASS_REQUEST_REPORT_OUTPUT:
                {
                  switch(setup->wIndex)
                  {
                    case 0:
                    {
                      // check the length, setup->wLength, for keyb, 8bit led state output is defined
                      KeybReport.led_state = setup->data[0];
                      // refer to BSP_KEYB_KEYB_LED_e
                    }break;
                  }
                }break;
                default:
                {
                  status = USB_ERROR_REQUEST_NOT_SUPPORT;
                }break;
              }
            }break;
            case USB_REQUEST_HID_CLASS_REQUEST_SET_IDLE:
            {
              switch(setup->wIndex)
              {
                case 0:
                {
                  KeybReport.pending = U_TRUE;
                }break;
                case 1:
                {
                  MouseReport.pending = U_TRUE;
                }break;
              }
            }break;
            case USB_REQUEST_DEVICE_GET_DESCRIPTOR:
            {
              switch(((setup->wValue)>>8)&0xFF)
              {
                case USB_REQUEST_HID_CLASS_DESCRIPTOR_REPORT:
                {
                  switch(setup->wIndex)
                  {
                    case 0:
                    {
                      size = sizeof(ReportKeybDescriptor);
                      size = (setup->wLength < size) ? (setup->wLength) : size;

                      status |= USB_SendData(0, (void*)&ReportKeybDescriptor, size, 0);
                      KeybReport.pending = U_FALSE;
                    }break;
                    case 1:
                    {
                      size = sizeof(ReportMouseDescriptor);
                      size = (setup->wLength < size) ? (setup->wLength) : size;

                      status |= USB_SendData(0, (void*)&ReportMouseDescriptor, size, 0);
                      MouseReport.pending = U_FALSE;
                    }break;
                  }
                }break;
                default:
                {
                  status = USB_ERROR_REQUEST_NOT_SUPPORT;
                }break;
              }
            }
            break;
            default:
            {
              status = USB_ERROR_REQUEST_NOT_SUPPORT;
            }break;
          }
        }
        break;
        default:
        {
          status = USB_ERROR_REQUEST_NOT_SUPPORT;
        }break;
      }

    // if status equals to USB_ERROR_REQUEST_NOT_SUPPORT: it is not supported request.
    // if status equals to USB_ERROR_NONE: it is successfully executed.
    if((USB_ERROR_NONE != status) && (USB_ERROR_REQUEST_NOT_SUPPORT != status))
    {
      platform_printf("USB event exec error %x (0x%x 0x%x)\n", status, *(uint32_t*)setup,*((uint32_t*)setup+1));
    }
    }break;

    case USB_EVENT_EP_DATA_TRANSFER:
    {
      switch(event->data.type)
      {
        case USB_CALLBACK_TYPE_RECEIVE_END:
        {

        }break;
        case USB_CALLBACK_TYPE_TRANSMIT_END:
        {
          switch(event->data.ep)
          {
            case EP_KB_IN:
            {
              KeybReport.pending = U_FALSE;
            }break;
            case EP_MO_IN:
            {
              MouseReport.pending = U_FALSE;
            }break;
          }
        }break;
        default:
          break;
      }
    }break;
    default:
      break;
  }

  return status;
}
void bsp_usb_handle_hid_keyb_key_report(uint8_t key, uint8_t press)
{
  uint32_t j;
  if(U_FALSE == KeybReport.pending)
  {
    if(press)
    {
      for(j = 0; j < KEY_TABLE_LEN; j++)
      {
        if(key == KeybReport.report.key_table[j])
        {
          // already pressed
          return;
        }
      }
      for(j = 0; j < KEY_TABLE_LEN; j++)
      {
        if(0 == KeybReport.report.key_table[j])
        {
          // find first empty spot, populate it
          KeybReport.report.key_table[j] = key;
          break;
        }
      }
      // no empty spot, return
      if(j == KEY_TABLE_LEN){return;}
    }
    else
    {
      for(j = 0; j < KEY_TABLE_LEN; j++)
      {
        if(key == KeybReport.report.key_table[j])
        {
          // already pressed, clear it
          KeybReport.report.key_table[j] = 0x00;
          break;
        }
      }
      if(j == KEY_TABLE_LEN){return;}
    }

    USB_SendData(USB_EP_DIRECTION_IN(EP_KB_IN), (void*)&(KeybReport.report), sizeof(BSP_KEYB_REPORT_s), 0);
    KeybReport.pending = U_TRUE;
  }
}

void bsp_usb_handle_hid_keyb_modifier_report(BSP_KEYB_KEYB_MODIFIER_e modifier, uint8_t press)
{
  uint8_t last_press_state = ((KeybReport.report.modifier & modifier) != 0);
  if((U_FALSE == KeybReport.pending)&&(last_press_state != press))
  {
    if(press)
    {
      KeybReport.report.modifier |= modifier;
    }
    else
    {
      KeybReport.report.modifier &= ~modifier;
    }

    USB_SendData(USB_EP_DIRECTION_IN(EP_KB_IN), (void*)&(KeybReport.report), sizeof(BSP_KEYB_REPORT_s), 0);
    KeybReport.pending = U_TRUE;
  }
}

uint8_t bsp_usb_get_hid_keyb_led_report(void)
{
  return (KeybReport.led_state);
}

void bsp_usb_handle_hid_mouse_report(int8_t x, int8_t y, uint8_t btn)
{
  if((U_FALSE == MouseReport.pending)&&((0!=x)||(0!=y)||(btn!=MouseReport.report.button)))
  {
    MouseReport.report.pos_x = x;
    MouseReport.report.pos_y = y;
    MouseReport.report.button = btn;

    USB_SendData(USB_EP_DIRECTION_IN(EP_MO_IN), (void*)&MouseReport, sizeof(BSP_MOUSE_REPORT_s), 0);
    MouseReport.pending = U_TRUE;
  }
}


void bsp_usb_handle_hid_keyb_clear_report_buffer(void)
{
  memset(&(KeybReport.report),0x00, sizeof(BSP_KEYB_REPORT_s));
}

void bsp_usb_handle_hid_mouse_clear_report_buffer(void)
{
  memset(&(MouseReport.report),0x00, sizeof(BSP_MOUSE_REPORT_s));
}

void bsp_usb_init(void)
{
  USB_INIT_CONFIG_T config;

  SYSCTRL_ClearClkGateMulti(1 << SYSCTRL_ITEM_APB_USB);
  //use SYSCTRL_GetClk(SYSCTRL_ITEM_APB_USB) to confirm, USB module clock has to be 48M.
  SYSCTRL_SelectUSBClk((SYSCTRL_ClkMode)(SYSCTRL_GetPLLClk()/48000000));

  platform_set_irq_callback(PLATFORM_CB_IRQ_USB, USB_IrqHandler, NULL);

  PINCTRL_SelUSB(USB_PIN_DP,USB_PIN_DM);

  SYSCTRL_USBPhyConfig(BSP_USB_PHY_ENABLE,BSP_USB_PHY_DP_PULL_UP);

  memset(&config, 0x00, sizeof(USB_INIT_CONFIG_T));
  config.intmask = USBINTMASK_SUSP | USBINTMASK_RESUME;
  config.handler = bsp_usb_event_handler;
  USB_InitConfig(&config);
}

void bsp_usb_disable(void)
{
  USB_Close();
  SYSCTRL_SetClkGateMulti(1 << SYSCTRL_ITEM_APB_USB);

  SYSCTRL_USBPhyConfig(BSP_USB_PHY_DISABLE,0);
}

static void internal_bsp_usb_device_remote_wakeup_stop(void)
{
  USB_DeviceSetRemoteWakeupBit(U_FALSE);
}

void bsp_usb_device_remote_wakeup(void)
{
  USB_DeviceSetRemoteWakeupBit(U_TRUE);
  platform_set_timer(internal_bsp_usb_device_remote_wakeup_stop,16);// setup timer for 10ms, then disable resume signal
}

#ifdef FEATURE_DISCONN_DETECT
void bsp_usb_device_disconn_timeout(void)
{
  bsp_usb_disable();
  platform_printf("USB cable disconnected.");
}
#endif

