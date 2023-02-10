#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "platform_api.h"
#include "bsp_usb.h"

const USB_DEVICE_DESCRIPTOR_REAL_T DeviceDescriptor __attribute__ ((aligned (4))) = USB_DEVICE_DESCRIPTOR;
const BSP_USB_DESC_STRUCTURE_T ConfigDescriptor __attribute__ ((aligned (4))) =
{USB_CONFIG_DESCRIPTOR, {USB_INTERFACE_DESCRIPTOR},{USB_EP_1_DESCRIPTOR,USB_EP_2_DESCRIPTOR}};
const uint8_t StringDescriptor_0[] __attribute__ ((aligned (4))) = USB_STRING_LANGUAGE;
const uint8_t StringDescriptor_1[] __attribute__ ((aligned (4))) = USB_STRING_MANUFACTURER;
const uint8_t StringDescriptor_2[] __attribute__ ((aligned (4))) = USB_STRING_PRODUCT;
uint8_t DynamicDescriptor[64] __attribute__ ((aligned (4)));

// WCID descriptor
#ifdef FEATURE_WCID_SUPPORT
const uint8_t StringDescriptor_3[] __attribute__ ((aligned (4))) = USB_STRING_WCID;
const uint8_t WcidDescriptor_4[] __attribute__ ((aligned (4))) = USB_WCID_DESCRIPTOR_INDEX_4;
const uint8_t WcidDescriptor_5[] __attribute__ ((aligned (4))) = USB_WCID_DESCRIPTOR_INDEX_5;
#endif

BSP_USB_VAR_s UsbVar;
uint8_t DataRecvBuf[EP_X_MPS_BYTES] __attribute__ ((aligned (4)));
uint8_t DataSendBuf[EP_X_MPS_BYTES] __attribute__ ((aligned (4)));
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
              status |= USB_ConfigureEp(&(ConfigDescriptor.endpoint[0]));
              status |= USB_ConfigureEp(&(ConfigDescriptor.endpoint[1]));

              status |= USB_RecvData(ConfigDescriptor.endpoint[EP_OUT-1].ep, DataRecvBuf, ConfigDescriptor.endpoint[EP_OUT-1].mps, 0);
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
                    #ifdef FEATURE_WCID_SUPPORT
                    case USB_STRING_WCID_IDX:
                    {
                      size = sizeof(StringDescriptor_3);
                      addr = StringDescriptor_3;
                    }break;
                    #endif
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
            #ifdef FEATURE_WCID_SUPPORT
            case USB_WCID_VENDOR_ID:
            {
              if((setup->wIndex&0xFF) == 0x04)
              {
                size = 0x28;//40
                size = (setup->wLength < size) ? (setup->wLength) : size;

                status |= USB_SendData(0, (void*)&WcidDescriptor_4, size, 0);
              }

              if((setup->wIndex&0xFF) == 0x05)
              {
                size = 0x8e;//total len 142
                size = (setup->wLength < size) ? (setup->wLength) : size;

                status |= USB_SendData(0, (void*)&WcidDescriptor_5, size, 0);
              }
            }
            break;
            #endif
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
            #ifdef FEATURE_WCID_SUPPORT
            case USB_WCID_VENDOR_ID:
            {
              if((setup->wIndex&0xFF) == 0x05)
              {
                size = 0x8e;//total len 142
                size = (setup->wLength < size) ? (setup->wLength) : size;

                status |= USB_SendData(0, (void*)&WcidDescriptor_5, size, 0);
              }
            }break;
            #endif
            default:
            {
              status = USB_ERROR_REQUEST_NOT_SUPPORT;
            }break;
          }
        }
        break;

        case USB_REQUEST_DESTINATION_EP:

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
          if(event->data.ep == EP_OUT)
          {
            uint32_t i;
            platform_printf("(%d)data: ",event->data.ep);for(i=0;i<10;i++){platform_printf(" 0x%x ",DataRecvBuf[i]);}platform_printf("\n");

            memset(DataSendBuf, 0x33, sizeof(DataSendBuf));
            status |= USB_SendData(ConfigDescriptor.endpoint[EP_IN-1].ep, DataRecvBuf, ConfigDescriptor.endpoint[EP_IN-1].mps, 0);
          }
        }break;
        case USB_CALLBACK_TYPE_TRANSMIT_END:
        {
          if(event->data.ep == EP_IN)
          {
            status |= USB_RecvData(ConfigDescriptor.endpoint[EP_OUT-1].ep, DataRecvBuf, ConfigDescriptor.endpoint[EP_OUT-1].mps, 0);
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

