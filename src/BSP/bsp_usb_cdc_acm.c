#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "platform_api.h"
#include "bsp_usb_cdc_acm.h"
#include "ring_buf.h"

/* User defined area */
#ifndef RX_BUFFER_SIZE
#define RX_BUFFER_SIZE      (1024)
#endif

static uint8_t ring_buff_storage_tx[RX_BUFFER_SIZE];
static struct ring_buf *ring_buffer_tx;
static int cb_ring_buf_peek_data_tx(const void *data, int len, int has_more, void *extra);

/* variables and functions area */
const USB_DEVICE_DESCRIPTOR_REAL_T DeviceDescriptor __attribute__ ((aligned (4))) = USB_DEVICE_DESCRIPTOR;

const BSP_USB_DESC_STRUCTURE_T ConfigDescriptor __attribute__ ((aligned (4))) = { 
    USB_CONFIG_DESCRIPTOR, 
    USB_INTERFACE_0_DESCRIPTOR, 
    USB_HEADER_FUNCTIONAL_DESCRIPTOR, 
    USB_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR, 
    USB_ABSTRACT_CONTROL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR,
    USB_UNION_DESCRIPTOR_FUNCTIONAL_DESCRIPTOR,
    USB_EP_CDC_INI_DESCRIPTOR,
    USB_INTERFACE_1_DESCRIPTOR,
    USB_EP_CDC_BULK_OUT_DESCRIPTOR, 
    USB_EP_CDC_BULK_IN_DESCRIPTOR
};

const uint8_t StringDescriptor_0[] __attribute__ ((aligned (4))) = USB_STRING_LANGUAGE;
const uint8_t StringDescriptor_1[] __attribute__ ((aligned (4))) = USB_STRING_MANUFACTURER;
const uint8_t StringDescriptor_2[] __attribute__ ((aligned (4))) = USB_STRING_PRODUCT;

BSP_USB_CDC_VAR_s USB_CDC_Var;
uint8_t DataRecvBuf[EP_X_MPS_BYTES] __attribute__ ((aligned (4)));
uint8_t DataSendBuf[EP_X_MPS_BYTES] __attribute__ ((aligned (4)));
uint8_t DataXSendBuf[EP_X_MPS_BYTES] __attribute__ ((aligned (4)));

void bsp_cdc_set_stall_ep_ (uint32_t EPNum)
{
    if(0 == USB_EP_NUM(EPNum))
    {
      USB_SetEp0Stall(EPNum);
    }
    else
    {
      USB_SetStallEp(EPNum, U_TRUE);
    }
}

void bsp_cdc_clr_stall_ep_ (uint32_t EPNum)
{
    USB_SetStallEp(EPNum, U_FALSE);
}

uint32_t bsp_cdc_write_ep (uint32_t EPNum, uint8_t *pData, uint32_t cnt)
{
    if(!USB_IS_EP_DIRECTION_IN(EPNum)){
      return 0;
    }

    memcpy(DataSendBuf, pData, cnt);
    USB_SendData(EPNum, (void*)DataSendBuf, cnt, 0);

    return cnt;
}

uint32_t bsp_cdc_read_ep (uint32_t EPNum, uint8_t *pData, uint32_t size)
{
    uint32_t cnt = size;
    USB_SETUP_T* setup = USB_GetEp0SetupData();

    if(USB_IS_EP_DIRECTION_IN(EPNum)){
      return 0;
    }

    if(0 == USB_EP_NUM(EPNum))
    {
      memcpy(pData, setup, cnt);
      return cnt;
    }
    else
    {
      cnt = USB_GetTransferedSize(EPNum);
      if(size < cnt)
      {
        cnt = size;
      }
      memcpy(pData, DataRecvBuf, cnt);

      USB_RecvData(EPNum, DataRecvBuf, sizeof(DataRecvBuf), 1<<USB_TRANSFERT_FLAG_FLEXIBLE_RECV_LEN);
      return cnt;
    }
}

void bsp_cdc_set_stall_ep(uint32_t EPNum)        /* set EP halt status according stall status */
{
    bsp_cdc_set_stall_ep_(EPNum);
    USB_CDC_Var.uEndPointMask  |= (EPNum & 0x80) ? ((1 << 16) << (EPNum & 0x0F)) : (1 << EPNum);
}


void bsp_cdc_uart_set_configuration(void)
{
    if(0 == USB_CDC_Var.line_coding.dwDTERate) USB_CDC_Var.line_coding.dwDTERate = 115200;
    apUART_BaudRateSet(APB_UART0, OSC_CLK_FREQ, USB_CDC_Var.line_coding.dwDTERate);
}

void bsp_cdc_acm_setlinecoding(void)
{
    uint8_t* ep0_buf = (uint8_t*)USB_GetEp0SetupData();
    USB_CDC_Var.line_coding.dwDTERate   = (ep0_buf[0] <<  0) |
                              (ep0_buf[1] <<  8) |
                              (ep0_buf[2] << 16) |
                              (ep0_buf[3] << 24) ;
    USB_CDC_Var.line_coding.bCharFormat =  ep0_buf[4];
    USB_CDC_Var.line_coding.bParityType =  ep0_buf[5];
    USB_CDC_Var.line_coding.bDataBits   =  ep0_buf[6];

    bsp_cdc_uart_set_configuration();
}

void bsp_cdc_acm_portgetlinecoding(uint8_t *buf)
{
    memcpy(buf, &USB_CDC_Var.line_coding, sizeof(USB_CDC_LINE_CODING_T));
}

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
        ring_buf_peek_data(ring_buffer_tx, cb_ring_buf_peek_data_tx, 0);
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
                USB_CDC_Var.remoteWakeup = (setup->wValue&0xF) ? 0 : 1;
                status = USB_ERROR_NONE;
              }
              break;
              case USB_REQUEST_DEVICE_SET_FEATURE:
              {
                USB_CDC_Var.remoteWakeup = (setup->wValue&0xF) ? 1 : 0;
                status = USB_ERROR_NONE;
              }
              break;
              case USB_REQUEST_DEVICE_SET_CONFIGURATION:
              {
                // uint8_t cfg_idx = setup->wValue&0xFF;
                // check if the cfg_idx is correct
                status |= USB_ConfigureEp(&(ConfigDescriptor.ep_0));
                status |= USB_ConfigureEp(&(ConfigDescriptor.ep_1_out));
                status |= USB_ConfigureEp(&(ConfigDescriptor.ep_1_in));

                status |= USB_RecvData(ConfigDescriptor.ep_1_out.ep, DataRecvBuf,
                          ConfigDescriptor.ep_1_out.mps, 1<<USB_TRANSFERT_FLAG_FLEXIBLE_RECV_LEN);
                USB_CDC_Var.configuration = U_TRUE;
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
                DataSendBuf[0] = SELF_POWERED | (REMOTE_WAKEUP << 1);
                DataSendBuf[1] = 0;
                status |= USB_SendData(0, DataSendBuf, 2, 0);
              }
              break;
              case USB_REQUEST_DEVICE_GET_CONFIGURATION:
              {
                DataSendBuf[0] = ConfigDescriptor.config.configIndex;
                status |= USB_SendData(0, DataSendBuf, 1, 0);
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
            switch(setup->bmRequestType.Type)
            {
              case USB_REQUEST_TYPE_CLASS:
              {
                switch(setup->bRequest)
                {
                  //required according to Table 11: Requests Abstract Control Model*
                  case CDC_SEND_ENCAPSULATED_COMMAND:
                  break;
                  case CDC_GET_ENCAPSULATED_RESPONSE:
                  break;
                  case CDC_SET_LINE_CODING:
                  {
                    USB_CDC_Var.cmdSetLineCoding = 1;
                  }
                  break;
                  case CDC_GET_LINE_CODING:
                  {
                    size = sizeof(USB_CDC_LINE_CODING_T);
                    size = (setup->wLength < size) ? (setup->wLength) : size;
                    bsp_cdc_acm_portgetlinecoding(DataSendBuf);
                    status |= USB_SendData(0, (void*)DataSendBuf, size, 0);
                  }
                  break;
                  case CDC_GET_COMM_FEATURE:
                  break;
                  case CDC_CLEAR_COMM_FEATURE:
                  break;
                  case CDC_SET_CONTROL_LINE_STATE:
                  {
                    USB_CDC_Var.control_line_state = setup->wValue;
                  }
                  break;
                  case CDC_SEND_BREAK:
                  {
                    USB_CDC_Var.Duration_of_Break = setup->wValue;
                  }
                  break;
                  default:
                    status = USB_ERROR_REQUEST_NOT_SUPPORT;
                }
              }break;
              default:
              {
                status = USB_ERROR_REQUEST_NOT_SUPPORT;
              }
            }
          }
          break;

          case USB_REQUEST_DESTINATION_EP:
          {
            switch(setup->bRequest)
            {
              case USB_REQUEST_DEVICE_CLEAR_FEATURE:
              {
                uint8_t n = setup->wIndex & 0x8F;
                uint32_t m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);

                if (setup->wValue == USB_FEATURE_ENDPOINT_STALL) {
                    bsp_cdc_clr_stall_ep_(n);
                    USB_CDC_Var.uEndPointMask &= ~m;
                }
              }
              break;
              case USB_REQUEST_DEVICE_SET_FEATURE:
              {
                uint8_t n = setup->wIndex & 0x8F;
                uint32_t m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);

                if (setup->wValue == USB_FEATURE_ENDPOINT_STALL) {
                  bsp_cdc_set_stall_ep_(n);
                  USB_CDC_Var.uEndPointMask |=  m;
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
            switch(event->data.ep)
            {
              case 0:
              if(USB_CDC_Var.cmdSetLineCoding)
              {
                bsp_cdc_acm_setlinecoding();
                USB_CDC_Var.cmdSetLineCoding = 0;                
              }
              break;
              case EP_CDC_BULK_OUT:
              {
                bsp_receive_cdc_data((uint8_t*)DataRecvBuf, event->data.size);
                status |= USB_RecvData(ConfigDescriptor.ep_1_out.ep, DataRecvBuf,
                          ConfigDescriptor.ep_1_out.mps, 1<<USB_TRANSFERT_FLAG_FLEXIBLE_RECV_LEN);
              }
              break;
            }
          }break;
          case USB_CALLBACK_TYPE_TRANSMIT_END:
          {
              if(event->data.ep == EP_CDC_BULK_IN)
              {
                USB_CDC_Var.txblock = 0;
                ring_buf_peek_data(ring_buffer_tx, cb_ring_buf_peek_data_tx, 0);
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

int bsp_cdc_push_data(uint8_t *data, uint32_t len)
{
    return ring_buf_write_data(ring_buffer_tx, data, len);
}

static int cb_ring_buf_peek_data_tx(const void *data, int len, int has_more, void *extra)
{
    int r = 0, status = 0;
    int size = len > EP_X_MPS_BYTES ? EP_X_MPS_BYTES : len;

  if(USB_CDC_Var.txblock)
  {
      USB_DisableEp(ConfigDescriptor.ep_1_in.ep);
      USB_EnableEp(ConfigDescriptor.ep_1_in.ep, ConfigDescriptor.ep_1_in.attributes);
      USB_CDC_Var.txblock = 0;
  }

    if((USB_CDC_Var.txblock == 0) && (USB_CDC_Var.configuration == U_TRUE))
    {
        memcpy(DataXSendBuf, data, size);
        USB_CDC_Var.txblock = 1;
        status = USB_SendData(USB_EP_DIRECTION_IN(EP_CDC_BULK_IN), (void*)DataXSendBuf, size, 0);
        r += size;
    }
    return r;
}

static void ring_buf_highwater_cb_tx(struct ring_buf *buf)
{
    ring_buf_peek_data(buf, cb_ring_buf_peek_data_tx, 0);
}

void bsp_usb_init(void)
{
    USB_INIT_CONFIG_T config;

    SYSCTRL_ClearClkGateMulti(1 << SYSCTRL_ITEM_APB_USB);
    //use SYSCTRL_GetClk(SYSCTRL_ITEM_APB_USB) to confirm, USB module clock has to be 48M.
    SYSCTRL_SelectUSBClk((SYSCTRL_ClkMode)(SYSCTRL_GetPLLClk()/48000000));

    PINCTRL_SelUSB(USB_PIN_DP,USB_PIN_DM);
    SYSCTRL_USBPhyConfig(BSP_USB_PHY_ENABLE,BSP_USB_PHY_DP_PULL_UP);

    memset(&config, 0x00, sizeof(USB_INIT_CONFIG_T));
    config.intmask = USBINTMASK_SUSP | USBINTMASK_RESUME | USBINTMASK_SOF;
    config.handler = bsp_usb_event_handler;
    USB_InitConfig(&config);
    ring_buffer_tx = ring_buf_init(ring_buff_storage_tx, sizeof(ring_buff_storage_tx), ring_buf_highwater_cb_tx);
}

void bsp_usb_state_init(void)
{
    USB_INIT_CONFIG_T config;

    memset(&config, 0x00, sizeof(USB_INIT_CONFIG_T));
    config.intmask = USBINTMASK_SUSP | USBINTMASK_RESUME | USBINTMASK_SOF;
    config.handler = bsp_usb_event_handler;
    USB_InitConfig(&config);
    ring_buffer_tx = ring_buf_init(ring_buff_storage_tx, sizeof(ring_buff_storage_tx), ring_buf_highwater_cb_tx);
}

int32_t bsp_cdc_acm_notify(uint16_t stat)
{
    int32_t status = 0;
    if (USB_CDC_Var.configuration == U_TRUE) {
        DataSendBuf[0] = 0xA1;   /* bmRequestType                      */
        DataSendBuf[1] = CDC_NOTIFICATION_SERIAL_STATE;/* bNotification
                                          (SERIAL_STATE)                      */
        DataSendBuf[2] = 0x00;   /* wValue                             */
        DataSendBuf[3] = 0x00;
        DataSendBuf[4] = 0x00;   /* wIndex (Interface 0)               */
        DataSendBuf[5] = 0x00;
        DataSendBuf[6] = 0x02;   /* wLength                            */
        DataSendBuf[7] = 0x00;
        DataSendBuf[8] = stat >> 0; /* UART State Bitmap                  */
        DataSendBuf[9] = stat >> 8;
        /* Write notification to be sent      */
        status = USB_SendData(USB_EP_DIRECTION_IN(EP_CDC_INI_IN), (void*)DataSendBuf, 10, 0);
    }

    return status;
}


void bsp_usb_disable(void)
{
    SYSCTRL_USBPhyConfig(BSP_USB_PHY_DISABLE,0);
    USB_Close();
    SYSCTRL_SetClkGateMulti(1 << SYSCTRL_ITEM_APB_USB);
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

__attribute((weak)) void bsp_receive_cdc_data(uint8_t *data, uint32_t len)
{
}

#ifdef FEATURE_DISCONN_DETECT
void bsp_usb_device_disconn_timeout(void)
{
    bsp_usb_disable();
    platform_printf("USB cable disconnected.");
}
#endif
