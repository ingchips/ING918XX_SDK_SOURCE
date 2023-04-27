#include <string.h>
#include "ingsoc.h"
#include "peripheral_usb.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

// =============================================================================
// PRIVATE VARIABLES
// =============================================================================

// Ep0 Buffer
static uint8_t g_UsbBufferEp0Out[USB_EP0_MPS] __attribute__ ((aligned (4)));
static uint8_t g_UsbBufferEp0In[USB_EP0_MPS] __attribute__ ((aligned (4)));

USB_GLOBAL_VAR_S g_UsbVar =
{
  .DeviceState    = USB_DEVICE_NONE,
  .Ep0State       = EP0_DISCONNECT,
};


static void nop(uint32_t n)
{
    uint32_t i;
    for(i=0;i<n;i++){
        __asm volatile ("nop");
    }
}

void USB_CoreReset(void)
{
  while(!(AHB_USB->UsbRControl & (1U<<31)));
  AHB_USB->UsbRControl |= (1<<0);
  nop(100);//have to wait before access
  while(AHB_USB->UsbRControl & (1<<0));
}

void USB_SetupDataFifo(void)
{
  uint32_t addr,i;

  AHB_USB->UsbRFifoSize    = RXFIFOSIZE;
  addr                     = RXFIFOSIZE;

  #define  DEP_FSIZ (2*(TXFIFOSIZE/4))
  AHB_USB->UsbTFifoSize = (addr) | (DEP_FSIZ << 16);//in terms of 32-bit words.

  for(i = 0; i < 5; i++)
  {
    addr                += DEP_FSIZ;
    AHB_USB->UsbIFifo[i] = (addr) | (DEP_FSIZ << 16);//in terms of 32-bit words.
  }
}

void USB_FlushTXFifo(uint8_t FifoNum)
{
  if(!(AHB_USB->UsbIntStat & (0x1 << 6)))
  {
    AHB_USB->UsbDControl |= (0x1 << 7);
  }

  while(!(AHB_USB->UsbIntStat & (0x1 << 6)));
  while(!(AHB_USB->UsbRControl & (1U << 31)));
  while(AHB_USB->UsbRControl & (0x1 << 5));
  AHB_USB->UsbRControl = ((FifoNum & 0x1F) << 6) | (0x1 << 5);
  while(AHB_USB->UsbRControl & (0x1 << 5));

  AHB_USB->UsbDControl  |=  (0x1 << 8);
}

void USB_FlushRXFifo(void)
{
  AHB_USB->UsbDControl  |=  (0x1 << 9);
  while(!(AHB_USB->UsbDControl & (0x1 << 3)));
  AHB_USB->UsbRControl |=  (0x1 << 4);
  while(AHB_USB->UsbRControl & (0x1 << 4));
  AHB_USB->UsbDControl  |=  (0x1 << 10);
}

void USB_DeviceInitialization(void)
{
  AHB_USB->UsbIntMask &= ~((0x1 << 3) | (0x1 << 4) | (0x1 << 5) | (0x1 << 15));

  AHB_USB->UsbDConfig |= (3 << 0) | (1 << 24);
  AHB_USB->UsbDConfig &= ~(0x7F << 4);
  AHB_USB->UsbDConfig &= (~(1 << 23));
  AHB_USB->UsbDConfig &= (~(0x1 << 2));
  AHB_USB->UsbDControl &= (~(0x1 << 1));

  AHB_USB->UsbIntMask |=  (0x1 << 12) | (0x1 << 13) | (g_UsbVar.UserConfig.intmask);
}

void USB_DeviceSetThreshold(void)
{
  if(g_UsbVar.UserConfig.rx_thr_en)
  {
    AHB_USB->UsbDThreCtrl &= ~(0x1ff<<17);
    AHB_USB->UsbDThreCtrl |= (1 << 16) | (((g_UsbVar.UserConfig.rx_thr_length)&0x1ff) << 17);
  }
  if((g_UsbVar.UserConfig.iso_tx_thr_en)||(g_UsbVar.UserConfig.non_iso_tx_thr_en))
  {
    AHB_USB->UsbDThreCtrl &= ~(0x1ff<<2);
    AHB_USB->UsbDThreCtrl |= (g_UsbVar.UserConfig.iso_tx_thr_en << 1) | (g_UsbVar.UserConfig.non_iso_tx_thr_en << 1) | (((g_UsbVar.UserConfig.tx_thr_length)&0x1ff) << 2);
  }
}


void USB_ResetAndConfig(void)
{
  AHB_USB->UsbConfig |= 1 << 6;
  USB_CoreReset();

  AHB_USB->UsbAConfig |= (1 << 5) | (3 << 1) | (1 << 0);
  AHB_USB->UsbConfig |= (0x1 << 3) | (AHB_USB->UsbConfig & (~(0xF<<10))) | (9 << 10);

  AHB_USB->UsbIntMask |= (0x1 << 2) | (0x1 << 1);

  USB_SetupDataFifo();

  USB_FlushTXFifo(0x10);
  USB_FlushRXFifo();

  USB_DeviceInitialization();

  AHB_USB->UsbIntStat |= AHB_USB->UsbIntStat;
  AHB_USB->UsbOTGIntStat |= AHB_USB->UsbOTGIntStat;

  USB_ResetTransfert();
  USB_DeviceSetThreshold();
}

void USB_ReInit(void)
{
  USB_FlushTXFifo(0x10);
  USB_FlushRXFifo();

  USB_ClrConfig();
  USB_ResetTransfert();
}

USB_ERROR_TYPE_E USB_InitConfig(USB_INIT_CONFIG_T *config)
{
  USB_ERROR_TYPE_E status = USB_ERROR_NONE;

  if(!config){return USB_ERROR_INVALID_INPUT;}

  g_UsbVar.DeviceState = USB_DEVICE_NONE;
  g_UsbVar.Ep0State = EP0_DISCONNECT;
  memcpy(&(g_UsbVar.UserConfig), config, sizeof(USB_INIT_CONFIG_T));

  USB_ResetAndConfig();

  return status;
}

void USB_ResetTransfert(void)
{
    uint8_t i;

    for(i = 0; i < DIEP_NUM+1; ++i)
    {
        g_UsbVar.InTransfer[i].sizeRemaining   = -1;
        g_UsbVar.InTransfer[i].sizeTransfered  =  0;
        g_UsbVar.InTransfer[i].sizeTotalLen    =  0;
        g_UsbVar.InTransfer[i].flags    =  0;
    }
    for(i = 0; i < DOEP_NUM+1; ++i)
    {
        g_UsbVar.OutTransfer[i].sizeRemaining  = -1;
        g_UsbVar.OutTransfer[i].sizeTransfered =  0;
        g_UsbVar.OutTransfer[i].sizeTotalLen    =  0;
        g_UsbVar.OutTransfer[i].flags    =  0;
    }

}

void USB_CancelTransfert(uint8_t ep)
{
    uint8_t               epNum;
    epNum = USB_EP_NUM(ep);

    if(USB_IS_EP_DIRECTION_IN(ep))
    {
        g_UsbVar.InTransfer[epNum].sizeRemaining   = -1;
        g_UsbVar.InTransfer[epNum].sizeTransfered  =  0;
        g_UsbVar.InTransfer[epNum].sizeTotalLen    =  0;
        g_UsbVar.InTransfer[epNum].flags    =  0;
    }
    else
    {
        g_UsbVar.OutTransfer[epNum].sizeRemaining   = -1;
        g_UsbVar.OutTransfer[epNum].sizeTransfered  =  0;
        g_UsbVar.OutTransfer[epNum].sizeTotalLen    =  0;
        g_UsbVar.OutTransfer[epNum].flags    =  0;
    }
}

uint32_t USB_CheckTransferCondition(uint8_t ep)
{
    uint8_t                epNum;
    uint32_t               XferSize = 0;
    USB_TRANSFERT_T*      transfer;
    uint32_t               abort = 0;

    epNum = USB_EP_NUM(ep);

    /*
    For out transfer, we always program recv packet size to a multiple of the maximum packet size.
    for say, if app wants to receive 7bytes, the real register will be programed to 64bytes(mps)
    when device received the first 7bytes, we have to abort the transfer and reset state.
    */

    if(USB_IS_EP_DIRECTION_IN(ep))
    {
      transfer = &g_UsbVar.InTransfer[epNum];
      switch(epNum)
      {
        case 0:
          XferSize = (AHB_USB->UsbDISize0) & 0x7f;
        break;
        default:
          XferSize = (AHB_USB->UsbDIxConfig[epNum-1].DISizex) & 0x7ffff;
        break;
      }
    }
    else
    {
      transfer = &g_UsbVar.OutTransfer[epNum];
      switch(epNum)
      {
        case 0:
          XferSize = (AHB_USB->UsbDOSize0) & 0x7f;
        break;
        default:
          XferSize = (AHB_USB->UsbDOxConfig[epNum-1].DOSizex) & 0x7ffff;
        break;
      }
    }

    if((transfer->flags)&(1<<USB_TRANSFERT_FLAG_FLEXIBLE_RECV_LEN))
    {
      transfer->sizeTransfered = transfer->sizeTotalLen - XferSize;
      transfer->sizeRemaining = -1;
      transfer->sizeTotalLen = 0;
      abort = 1;
    }

    return abort;
}

uint32_t USB_GetTransferedSize(uint8_t ep)
{
    uint8_t               epNum;
    epNum = USB_EP_NUM(ep);

    if(USB_IS_EP_DIRECTION_IN(ep))
    {
        return g_UsbVar.InTransfer[epNum].sizeTransfered;
    }
    else
    {
        return g_UsbVar.OutTransfer[epNum].sizeTransfered;
    }
}

void USB_DisableEp(uint8_t ep)
{
    uint8_t epNum = USB_EP_NUM(ep);

    if(USB_IS_EP_DIRECTION_IN(ep))
    {
      if(0 == epNum)
      {
        AHB_USB->UsbDICtrl0 |= (0x1 << 27);
        if(AHB_USB->UsbDICtrl0 & (1U << 31))
        {
          AHB_USB->UsbDICtrl0 |= (0x1 << 30);
        }
        AHB_USB->UsbDIInt0 |= AHB_USB->UsbDIInt0;
        AHB_USB->UsbDIntMask &= ~(0x1 << 0);
      }
      else
      {
        AHB_USB->UsbDIxConfig[epNum-1].DICtrlx |= (0x1 << 27);
        if(AHB_USB->UsbDIxConfig[epNum-1].DICtrlx & (1U << 31))
        {
          AHB_USB->UsbDIxConfig[epNum-1].DICtrlx |= (0x1 << 30);
        }
        AHB_USB->UsbDIxConfig[epNum-1].DIIntx |= AHB_USB->UsbDIxConfig[epNum-1].DIIntx;
        AHB_USB->UsbDIntMask &= ~(1 << (epNum + 0));
      }
    }
    else
    {
      if(0 == epNum)
      {
        AHB_USB->UsbDOCtrl0 |= (0x1 << 27);
        if(AHB_USB->UsbDOCtrl0 & (1U << 31))
        {
          AHB_USB->UsbDOCtrl0 |= (0x1 << 30);
        }

        AHB_USB->UsbDOInt0 |= AHB_USB->UsbDOInt0;
        AHB_USB->UsbDIntMask &= ~(0x1 << 16);
      }
      else
      {
        AHB_USB->UsbDOxConfig[epNum-1].DOCtrlx |= (0x1 << 27);
        if(AHB_USB->UsbDOxConfig[epNum-1].DOCtrlx & (1U << 31))
        {
          AHB_USB->UsbDOxConfig[epNum-1].DOCtrlx |= (0x1 << 27) | (0x1 << 30);
        }

        AHB_USB->UsbDOxConfig[epNum-1].DOIntx |= AHB_USB->UsbDOxConfig[epNum-1].DOIntx;
        AHB_USB->UsbDIntMask &= ~(1 << (epNum + 16));
      }
    }

    USB_FlushTXFifo(epNum);
    USB_CancelTransfert(ep);

}

void USB_ClrConfig(void)
{
    uint8_t i;

    g_UsbVar.DeviceState = USB_DEVICE_NONE;
    g_UsbVar.Ep0State = EP0_DISCONNECT;

    // Disable all EP
    USB_SetStallEp(USB_EP_DIRECTION_IN(0),U_FALSE);
    for(i = 0; i < DIEP_NUM; ++i)
    {
        USB_SetStallEp(USB_EP_DIRECTION_IN(i+1),U_FALSE);
        USB_DisableEp(USB_EP_DIRECTION_IN (i+1));
    }

    USB_SetStallEp(USB_EP_DIRECTION_OUT(0),U_FALSE);
    for(i = 0; i < DOEP_NUM; ++i)
    {
        USB_SetStallEp(USB_EP_DIRECTION_OUT(i+1),U_FALSE);
        USB_DisableEp(USB_EP_DIRECTION_OUT(i+1));
    }

}

void USB_GetSetupPacket(void)
{
  AHB_USB->UsbDOSize0  =  (0x3 << 29) | ((8*3) << 0)| (0x1 << 19);
  AHB_USB->UsbDODma0   =  (uint32_t)g_UsbBufferEp0Out;
  AHB_USB->UsbDOCtrl0  |= (0x1 << 26) | (1U << 31);
}

uint32_t USB_GetMaxTransferSize(void)
{
  return(1 << (((AHB_USB->UsbHConfig3)&0xf) + 11));
}

uint16_t USB_GetMaxPacketSize(void)
{
  return(1 << ((((AHB_USB->UsbHConfig3)>>4)&0x7) + 4));
}

void USB_GetTransferSize(uint8_t ep, int32_t *size_p, uint16_t *nbPacket_p)
{
    uint8_t                epNum;
    USB_TRANSFERT_T*      transfer;
    uint16_t              nbPacket;
    int32_t               size;
    uint32_t               maxPacketSize;
    epNum = USB_EP_NUM(ep);

    maxPacketSize = g_UsbVar.ep[epNum].maxpacket;
    if(USB_IS_EP_DIRECTION_IN(ep))
    {
      transfer = &g_UsbVar.InTransfer[epNum];
      switch(epNum)
      {
        case 0:
        {
          /* Program the transfer size and packet count
           *      as follows: xfersize = N * maxpacket +
           *      short_packet pktcnt = N + (short_packet
           *      exist ? 1 : 0)
           */
          size = transfer->sizeRemaining;
          size = (size < maxPacketSize) ? size : maxPacketSize;
          nbPacket = 1;
        }
        break;

        default:
        {
          /* Transfer size[epnum] = n * mps[epnum] + sp
              (where n is an integer = 0, and 0 = sp < mps[epnum])
              If (sp > 0), then packet count[epnum] = n + 1.
              Otherwise, packet count[epnum] = n */

          size = transfer->sizeRemaining;
          size = (size < USB_GetMaxTransferSize()) ? size : USB_GetMaxTransferSize();

          nbPacket = (size <= maxPacketSize) ? 1 : ((size + (maxPacketSize - 1))/maxPacketSize);

          if(nbPacket > USB_GetMaxPacketSize())
          {
            nbPacket = USB_GetMaxPacketSize();
            size = nbPacket * maxPacketSize;
          }
        }
      }
    }
    else
    {
      transfer = &g_UsbVar.OutTransfer[epNum];
      switch(epNum)
      {
        case 0:
        {
          /* Program the transfer size and packet count
           *      as follows: xfersize = N * maxpacket +
           *      short_packet pktcnt = N + (short_packet
           *      exist ? 1 : 0)
           */
          size = maxPacketSize;
          nbPacket = 1;
        }
        break;

        default:
        {
          /* For OUT transfers, the Transfer Size field in the endpoint Transfer Size register must be a multiple
              of the maximum packet size of the endpoint, adjusted to the DWORD boundary. */

          size = transfer->sizeRemaining;
          size = (size < USB_GetMaxTransferSize()) ? size : USB_GetMaxTransferSize();

          nbPacket = (size <= maxPacketSize) ? 1 : ((size + (maxPacketSize - 1))/maxPacketSize);
          nbPacket = (nbPacket < USB_GetMaxPacketSize()) ? nbPacket : USB_GetMaxPacketSize();

          size = nbPacket * maxPacketSize;
        }
      }
    }

    *size_p = size;
    *nbPacket_p = nbPacket;
}

void USB_NewTransfer(uint8_t ep, int32_t size, uint16_t nbPacket)
{
    uint8_t                epNum;

    epNum = USB_EP_NUM(ep);

    if(USB_IS_EP_DIRECTION_IN(ep))
    {
      switch(epNum)
      {
        case 0:
        {
          AHB_USB->UsbDISize0 = ((size & 0x7F) << 0) | ((nbPacket & 0x3) << 19);
          AHB_USB->UsbDICtrl0 |= (1U << 31) | (0x1 << 26);
        }
        break;
        default:
          AHB_USB->UsbDIxConfig[epNum-1].DISizex = ((size & 0x7FFFF) << 0) | ((nbPacket & 0x3FF) << 19) | (1 << 29);
          AHB_USB->UsbDIxConfig[epNum-1].DICtrlx |= (1U << 31) | (0x1 << 26);
        break;
      }

    }
    else
    {
      switch(epNum)
      {
        case 0:
        {
          AHB_USB->UsbDOSize0 = (0x3 << 29) | ((size & 0x7F) << 0) | (nbPacket ? (0x1 << 19) : 0);
          AHB_USB->UsbDOCtrl0 |= (1U << 31) | (0x1 << 26);
        }
        break;
        default:
          AHB_USB->UsbDOxConfig[epNum-1].DOSizex = ((size & 0x7FFFF) << 0) | ((nbPacket & 0x3FF) << 19);
          AHB_USB->UsbDOxConfig[epNum-1].DOCtrlx |= (1U << 31) | (0x1 << 26);
        break;
      }
    }

}

uint8_t USB_StartTransfer(uint8_t ep, void *data, uint16_t size, uint32_t flag)
{
    uint8_t                epNum;
    USB_TRANSFERT_T*       transfer;
    uint16_t              sc_nbPacket;
    int32_t               sc_size;

    epNum = USB_EP_NUM(ep);

    if(USB_IS_EP_DIRECTION_IN(ep))
    {
      transfer = &g_UsbVar.InTransfer[epNum];
      switch(epNum)
      {
        case 0:
          AHB_USB->UsbDIDma0 = (uint32_t)data;
        break;
        default:
          AHB_USB->UsbDIxConfig[epNum-1].DIDmax = (uint32_t)data;
        break;
      }
    }
    else
    {
      transfer = &g_UsbVar.OutTransfer[epNum];
      switch(epNum)
      {
        case 0:
          AHB_USB->UsbDODma0 = (uint32_t)data;
        break;
        default:
          AHB_USB->UsbDOxConfig[epNum-1].DODmax = (uint32_t)data;
        break;
      }
    }

    if(transfer->sizeRemaining != -1)
    {
      //return 1;
    }

    transfer->sizeRemaining  = size;
    transfer->sizeTransfered = 0;
    transfer->flags = flag;

    USB_GetTransferSize(ep, &sc_size, &sc_nbPacket);
    transfer->sizeTotalLen = sc_size;

    // Program the EP
    USB_NewTransfer(ep, sc_size, sc_nbPacket);

    return 0;
}


uint8_t USB_ContinueTransfer(uint8_t ep)
{
    uint8_t                epNum;
    USB_TRANSFERT_T*      transfer;
    int32_t                size;
    uint32_t               XferSize = 0;
    uint16_t              sc_nbPacket;
    int32_t               sc_size;

    epNum = USB_EP_NUM(ep);

    if(USB_IS_EP_DIRECTION_IN(ep))
    {
      transfer = &g_UsbVar.InTransfer[epNum];
      switch(epNum)
      {
        case 0:
          XferSize = (AHB_USB->UsbDISize0) & 0x7f;
        break;
        default:
          XferSize = (AHB_USB->UsbDIxConfig[epNum-1].DISizex) & 0x7ffff;
        break;
      }
    }
    else
    {
      transfer = &g_UsbVar.OutTransfer[epNum];
      switch(epNum)
      {
        case 0:
          XferSize = (AHB_USB->UsbDOSize0) & 0x7f;
        break;
        default:
          XferSize = (AHB_USB->UsbDOxConfig[epNum-1].DOSizex) & 0x7ffff;
        break;
      }

    }

    if(transfer->sizeRemaining == -1)
    {
        return 1;
    }

    size = transfer->sizeTotalLen - XferSize;

    transfer->sizeRemaining  -= size;
    transfer->sizeTransfered += size;

    if(transfer->sizeRemaining <= 0)
    {
      // End of transfert
      transfer->sizeRemaining = -1;
      return 1;
    }

    USB_GetTransferSize(ep, &sc_size, &sc_nbPacket);
    transfer->sizeTotalLen = sc_size;

    USB_NewTransfer(ep, sc_size, sc_nbPacket);
    return 0;
}


void USB_EnableEp(uint8_t ep, USB_EP_TYPE_T type)
{
    uint8_t epNum = USB_EP_NUM(ep);

    if(USB_IS_EP_DIRECTION_IN(ep))
    {
      if(0 == epNum)
      {
          AHB_USB->UsbDICtrl0 |= ((type & 0x3) << 18) | (0x1 << 15) | ((epNum & 0xF) << 22) | ((epNum & 0xF) << 11);
          AHB_USB->UsbDIInt0 |= AHB_USB->UsbDIInt0;
          AHB_USB->UsbDIntMask |= (0x1 << 0);
      }
      else
      {
          AHB_USB->UsbDIxConfig[epNum-1].DICtrlx |= ((g_UsbVar.ep[epNum].maxpacket & 0x7FF) << 0) | ((type & 0x3) << 18) | (0x1 << 15) | ((epNum & 0xF) << 22) | (0x1 << 28) ;
          AHB_USB->UsbDIxConfig[epNum-1].DIIntx |= AHB_USB->UsbDIxConfig[epNum-1].DIIntx;
          AHB_USB->UsbDIntMask |= (1 << (epNum + 0));
      }

    }
    else
    {
      if(0 == epNum)
      {
          AHB_USB->UsbDOCtrl0 |= ((type & 0x3) << 18) | (0x1 << 15);
          AHB_USB->UsbDOInt0 |= AHB_USB->UsbDOInt0;
          AHB_USB->UsbDIntMask |= (0x1 << 16);
      }
      else
      {
          AHB_USB->UsbDOxConfig[epNum-1].DOCtrlx |= ((g_UsbVar.ep[epNum].maxpacket & 0x7FF) << 0) | ((type & 0x3) << 18) | (0x1 << 15) | (0x1 << 28);
          AHB_USB->UsbDOxConfig[epNum-1].DOIntx |= AHB_USB->UsbDOxConfig[epNum-1].DOIntx;
          AHB_USB->UsbDIntMask |= (1 << (epNum + 16));
      }
    }
}

USB_ERROR_TYPE_E USB_ConfigureEp(const USB_EP_DESCRIPTOR_REAL_T* ep)
{
    uint8_t epNum;
    USB_ERROR_TYPE_E error = USB_ERROR_NONE;

    epNum = USB_EP_NUM(ep->ep);

    g_UsbVar.ep[epNum].type = ep->type;
    g_UsbVar.ep[epNum].maxpacket = ep->mps;
    g_UsbVar.ep[epNum].is_in = USB_IS_EP_DIRECTION_IN(ep->ep) ? 1 : 0;

    USB_EnableEp(ep->ep, (USB_EP_TYPE_T)(ep->attributes));
    g_UsbVar.ep[epNum].active = 1;

    return(error);
}

void USB_SetStallEp(uint8_t ep, uint8_t stall)
{
    uint8_t epNum = USB_EP_NUM(ep);
    volatile uint32_t*  diepctrl;
    volatile uint32_t*  doepctrl;

    if(epNum == 0)
    {
        diepctrl = &AHB_USB->UsbDICtrl0;
        doepctrl = &AHB_USB->UsbDOCtrl0;
    }
    else
    {
        diepctrl = &AHB_USB->UsbDIxConfig[epNum-1].DICtrlx;
        doepctrl = &AHB_USB->UsbDOxConfig[epNum-1].DOCtrlx;
    }

    if(USB_IS_EP_DIRECTION_IN(ep))
    {
      if(U_TRUE == stall)
      {
        if(*diepctrl & (1U << 31))
        {
          *diepctrl |= (0x1 << 21) | (0x1 << 30);
        }
        else
        {
          *diepctrl |= (0x1 << 21);
        }
        USB_FlushTXFifo(0x10);
      }
      else
      {
        *diepctrl &= ~((0x1 << 21));
        *diepctrl |= (0x1 << 28);
      }
    }
    else
    {
      if(U_TRUE == stall)
      {
        *doepctrl |= (0x1 << 21);
      }
      else
      {
        *doepctrl &= ~(0x1 << 21);
      }
    }
}

void USB_PCStopPhyClcok(uint8_t stop)
{
  if(U_TRUE == stop)
  {
    AHB_USB->UsbPCConfig |= (0x1);
  }
  else
  {
    AHB_USB->UsbDControl &= ~(0x1);
  }
  nop(20);//required by HW
}

USB_ERROR_TYPE_E USB_SendData(uint8_t ep, void* buffer, uint16_t size, uint32_t flag)
{
    uint8_t   epNum;
    uint32_t  activeEp;
    USB_ERROR_TYPE_E error = USB_ERROR_NONE;

    epNum = USB_EP_NUM(ep);

    if((epNum > DIEP_NUM)||(((uint32_t)buffer)&0x3)){return USB_ERROR_INVALID_INPUT;}

    if(epNum == 0)
    {
        activeEp = AHB_USB->UsbDICtrl0 & (0x1 << 15);
    }
    else
    {
        activeEp = AHB_USB->UsbDIxConfig[epNum-1].DICtrlx & (0x1 << 15);
    }

    if(!activeEp){return USB_ERROR_INACTIVE_EP;}

    USB_StartTransfer(USB_EP_DIRECTION_IN(ep), buffer, size, flag);

    return(error);
}

USB_ERROR_TYPE_E USB_RecvData(uint8_t ep, void* buffer, uint16_t size, uint32_t flag)
{
    uint8_t   epNum;
    uint32_t  activeEp;
    USB_ERROR_TYPE_E error = USB_ERROR_NONE;

    epNum = USB_EP_NUM(ep);

    if((epNum > DOEP_NUM)||(((uint32_t)buffer)&0x3)){return USB_ERROR_INVALID_INPUT;}

    if(epNum == 0)
    {
        activeEp = AHB_USB->UsbDOCtrl0 & (0x1 << 15);
    }
    else
    {
        activeEp = AHB_USB->UsbDOxConfig[epNum-1].DOCtrlx & (0x1 << 15);
    }

    if(!activeEp){return USB_ERROR_INACTIVE_EP;}

    USB_StartTransfer(USB_EP_DIRECTION_OUT(ep), buffer, size, flag);

   return (error);
}

void USB_EP0StatusIn(void)
{
    g_UsbVar.Ep0State = EP0_IN_STATUS_PHASE;
    USB_SendData(0, g_UsbBufferEp0In, 0, 0);
    USB_GetSetupPacket();
}

void USB_EP0StatusOut(void)
{
  g_UsbVar.Ep0State = EP0_OUT_STATUS_PHASE;
  USB_RecvData(0, g_UsbBufferEp0Out, 0, 0);
}

USB_SETUP_T* USB_GetEp0SetupData(void)
{
  return ((USB_SETUP_T*)(g_UsbBufferEp0Out));
}

void USB_SetEp0Stall(uint8_t ep)
{
  USB_SetStallEp(ep, U_TRUE);
  g_UsbVar.Ep0State = EP0_IDLE;
  USB_GetSetupPacket();
}

void USB_HandleEp0(void)
{
  USB_SETUP_T* setup = (USB_SETUP_T*)(g_UsbBufferEp0Out);
  USB_EVNET_HANDLER_T event;

  switch(g_UsbVar.Ep0State)
  {
    case EP0_IDLE:
    {
      switch(setup->bmRequestType.Recipient)
      {
        case USB_REQUEST_DESTINATION_DEVICE:
        {
          switch(setup->bRequest)
          {
            case USB_REQUEST_DEVICE_SET_ADDRESS:
            {
              AHB_USB->UsbDConfig &= ~(0x7F << 4);
              AHB_USB->UsbDConfig |= (((setup->wValue) & 0x7F) << 4);
              g_UsbVar.DeviceState |= (1<<USB_DEVICE_ADDRESS);
            }
            break;
            case USB_REQUEST_DEVICE_SET_CONFIGURATION:
            {
              g_UsbVar.DeviceState |= (1<<USB_DEVICE_CONFIGURED);
            }
            break;
          }
        }
        break;
      }

      event.id = USB_EVENT_EP0_SETUP;
      uint32_t event_status = g_UsbVar.UserConfig.handler(&event);

      //setup requset which is not supported, by spec, device should return stall pid to host
      if(USB_ERROR_REQUEST_NOT_SUPPORT == event_status)
      {
        USB_SetEp0Stall((setup->bmRequestType.Direction) ? USB_EP_DIRECTION_IN(0) : USB_EP_DIRECTION_OUT(0));
      }

      //setup request was executed successfully
      if(USB_ERROR_NONE == event_status)
      {
        //for two stage setup, setup follows with a status
        if(setup->wLength == 0)
        {
          //go to status stage to complete setup
          USB_EP0StatusIn();
        }
        else
        {
          g_UsbVar.Ep0State = (setup->bmRequestType.Direction) ? EP0_IN_DATA_PHASE : EP0_OUT_DATA_PHASE;
          // it is a OUT token after SETUP. program DOEP0 to receive the data, wlength bytes
          if(EP0_OUT_DATA_PHASE == g_UsbVar.Ep0State)
          {
            USB_RecvData(0, g_UsbBufferEp0Out, setup->wLength, 0);
          }
        }
      }

    }
    break;
    case EP0_IN_DATA_PHASE:
    {
      if(USB_ContinueTransfer(USB_EP_DIRECTION_IN(0)))
      {
        event.id = USB_EVENT_EP_DATA_TRANSFER;
        event.data.ep = 0;
        event.data.size = 0;//no need to provide size for IN endpoint
        event.data.type = USB_CALLBACK_TYPE_TRANSMIT_END;
        g_UsbVar.UserConfig.handler(&event);
        USB_EP0StatusOut();
      }
    }
    break;
    case EP0_OUT_DATA_PHASE:
    {
      if(USB_ContinueTransfer(USB_EP_DIRECTION_OUT(0)))
      {
        // OUT was complete, prepare to receive IN for status
        event.id = USB_EVENT_EP_DATA_TRANSFER;
        event.data.ep = 0;
        event.data.size = g_UsbVar.OutTransfer[0].sizeTransfered;
        event.data.type = USB_CALLBACK_TYPE_RECEIVE_END;
        g_UsbVar.UserConfig.handler(&event);
        USB_EP0StatusIn();
      }
    }
    break;
      //status stage is done, prepare for next cycle
    case EP0_IN_STATUS_PHASE:
    {
      g_UsbVar.Ep0State = EP0_IDLE;
      USB_GetSetupPacket();
    }
    break;
    case EP0_OUT_STATUS_PHASE:
    {
      g_UsbVar.Ep0State = EP0_IDLE;
      USB_GetSetupPacket();
    }
    break;
    default:

    break;
  }
}

uint32_t USB_IrqHandler (void *user_data)
{
  uint32_t status = AHB_USB->UsbIntStat,i;
  uint32_t statusEp = 0, ep_intr = 0;
  USB_EVNET_HANDLER_T event;

  AHB_USB->UsbIntStat |= status;
  if(status & (0x1 << 12))
  {
    //1. Set the NAK bit for all OUT endpoints:
    AHB_USB->UsbDOCtrl0 |= (0x1 << 27);
    for(i = 0; i < 5; i++)
    {
      AHB_USB->UsbDOxConfig[i].DOCtrlx |= (0x1 << 27);
    }

    USB_ReInit();

    AHB_USB->UsbDIntMask |= (0x1 << 0) | (0x1 << 16);
    AHB_USB->UsbDIMask |=  (0x1 << 0) | (0x1 << 2) | (0x1 << 3);
    AHB_USB->UsbDOMask |=  (0x1 << 0) | (0x1 << 2) | (0x1 << 3) | (0x1 << 12) | (0x1 << 8) | (0x1 << 5);
    AHB_USB->UsbDConfig &= ~(0x7F << 4);

    //USB_GetSetupPacket();
    AHB_USB->UsbIntMask |= (0x1 << 19) | (0x1 << 18);

    event.id = USB_EVENT_DEVICE_RESET;
    g_UsbVar.UserConfig.handler(&event);
    g_UsbVar.DeviceState = (USB_DEVICE_STATE_E)((1<<USB_DEVICE_ATTACHED) | (1<<USB_DEVICE_POWERED));

  }

  if(status & (0x1 << 13))
  {
    g_UsbVar.Ep0State = EP0_IDLE;

    AHB_USB->UsbDICtrl0 &= ~(0x3 << 0);
    AHB_USB->UsbDOCtrl0  |= (1U << 31);

    USB_GetSetupPacket();

    g_UsbVar.ep[0].active = 1;
    g_UsbVar.ep[0].maxpacket = USB_EP0_MPS;
    g_UsbVar.DeviceState |= (1<<USB_DEVICE_DEFAULT);
  }

  if((status & (0x1 << 18)) && (g_UsbVar.DeviceState & (1<<USB_DEVICE_DEFAULT)))
  {
    ep_intr = (AHB_USB->UsbDInt) & (AHB_USB->UsbDIntMask);

    uint32_t epnum = 0;

    ep_intr = ep_intr & 0xffff;
    for(epnum = 0; epnum <= DIEP_NUM; epnum++)
    {
      if ((ep_intr >> epnum) & 0x1)
      {
        if(0 == epnum)
        {
          statusEp            = AHB_USB->UsbDIInt0;

          //SETUP - IN - IN - IN - OUT
          //SETUP - OUT - OUT - OUT - IN
          if(statusEp & (0x1 << 0))
          {
            //check if remaining data needs to be transfered
            //check if need to process status stage for OUT stage
            if((g_UsbVar.Ep0State != EP0_IDLE) && ((g_UsbVar.Ep0State != EP0_IN_DATA_PHASE) || USB_ContinueTransfer(USB_EP_DIRECTION_IN(epnum))))
            {
                USB_HandleEp0();
            }
          }

          AHB_USB->UsbDIInt0 = statusEp;
        }
        else
        {
          statusEp            = AHB_USB->UsbDIxConfig[epnum-1].DIIntx;

          AHB_USB->UsbDIxConfig[epnum-1].DIIntx = statusEp;
          if(statusEp & (0x1 << 0))
          {
            if( USB_ContinueTransfer(USB_EP_DIRECTION_IN(epnum)) )
            {
              event.id = USB_EVENT_EP_DATA_TRANSFER;
              event.data.ep = epnum;
              event.data.size = 0;//no need to provide size for IN endpoint
              event.data.type = USB_CALLBACK_TYPE_TRANSMIT_END;
              g_UsbVar.UserConfig.handler(&event);
            }
          }
        }
      }
    }

  }

  if((status & (0x1 << 19)) && (g_UsbVar.DeviceState & (1<<USB_DEVICE_DEFAULT)))
  {
    ep_intr = (AHB_USB->UsbDInt) & (AHB_USB->UsbDIntMask);
    uint32_t epnum = 0;

    ep_intr = ep_intr >> 16;
    for(epnum = 0; epnum <= DOEP_NUM; epnum++)
    {
      if ((ep_intr >> epnum) & 0x1)
      {
        if(0 == epnum)
        {
          statusEp            = AHB_USB->UsbDOInt0;
          AHB_USB->UsbDOInt0 = statusEp;

          if(statusEp & (0x1 << 0))
          {
            if(statusEp & (0x1 << 15))
            {
              g_UsbVar.Ep0State = EP0_IDLE;
              USB_HandleEp0();
            }
            else
            {
              if((g_UsbVar.Ep0State != EP0_IDLE) )
              {
                USB_HandleEp0();
              }
            }
          }
          else
          {
            if((statusEp & (0x1 << 5))&&(g_UsbVar.Ep0State == EP0_OUT_DATA_PHASE))
            {
              USB_EP0StatusIn();
            }
            if((statusEp & (0x1 << 3)))
            {
              USB_GetSetupPacket();
            }
          }
        }
        else
        {
          statusEp            = AHB_USB->UsbDOxConfig[epnum-1].DOIntx;

          AHB_USB->UsbDOxConfig[epnum-1].DOIntx = statusEp;
          if(statusEp & (0x1 << 0))
          {
            if(USB_CheckTransferCondition(USB_EP_DIRECTION_OUT(epnum)) ||
               USB_ContinueTransfer(USB_EP_DIRECTION_OUT(epnum)))
            {
              event.id = USB_EVENT_EP_DATA_TRANSFER;
              event.data.ep = epnum;
              event.data.size = g_UsbVar.OutTransfer[epnum].sizeTransfered;
              event.data.type = USB_CALLBACK_TYPE_RECEIVE_END;
              g_UsbVar.UserConfig.handler(&event);
            }
          }
        }
      }
    }
  }

  if((status & USBINTMASK_SOF) && (g_UsbVar.UserConfig.intmask & USBINTMASK_SOF) && (g_UsbVar.DeviceState & (1<<USB_DEVICE_DEFAULT)))
  {
    event.id = USB_EVENT_DEVICE_SOF;
    g_UsbVar.UserConfig.handler(&event);
  }

  if((status & USBINTMASK_SUSP) && (g_UsbVar.DeviceState & (1<<USB_DEVICE_DEFAULT)))
  {
    g_UsbVar.DeviceState |= USB_DEVICE_SUSPENDED;
    USB_PCStopPhyClcok(U_TRUE);
    event.id = USB_EVENT_DEVICE_SUSPEND;
    g_UsbVar.UserConfig.handler(&event);
  }

  if((status & USBINTMASK_RESUME) && (g_UsbVar.DeviceState & (1<<USB_DEVICE_DEFAULT)))
  {
    g_UsbVar.DeviceState &= ~USB_DEVICE_SUSPENDED;
    USB_PCStopPhyClcok(U_FALSE);
    event.id = USB_EVENT_DEVICE_RESUME;
    g_UsbVar.UserConfig.handler(&event);
  }

  return 0;
}

void USB_DeviceSetRemoteWakeupBit(uint8_t enable)
{
  if(U_TRUE == enable)
  {
    USB_PCStopPhyClcok(U_FALSE);
    AHB_USB->UsbDControl |= (0x1);
  }
  else
  {
    AHB_USB->UsbDControl &= ~(0x1);
    g_UsbVar.DeviceState &= ~USB_DEVICE_SUSPENDED;
  }
}

void USB_AsciiToUtf8(uint8_t *utf8, uint8_t *acsii)
{
  while(*acsii)
  {
      *utf8++ = *acsii++;
      *utf8++ = 0;
  }
}

USB_DEVICE_STATE_E USB_GetDeviceState(void)
{
  return (g_UsbVar.DeviceState);
}

void USB_Close(void)
{
  USB_ClrConfig();
  USB_ResetTransfert();

  AHB_USB->UsbIntMask = 0;
  AHB_USB->UsbDIntMask = 0;
  g_UsbVar.UserConfig.handler = 0;
  g_UsbVar.UserConfig.intmask = 0;
}

void USB_SetGlobalOutNak(uint8_t enable)
{
  if(U_TRUE == enable)
  {
    AHB_USB->UsbDControl |=  (0x1 << 9);
    while(!(AHB_USB->UsbDControl & (0x1 << 3)));
  }
  else
  {
    AHB_USB->UsbDControl |=  (0x1 << 10);
    AHB_USB->UsbDControl &=  ~(0x1 << 9);
  }
}

void USB_SetInEndpointNak(uint8_t ep, uint8_t enable)
{
  uint8_t epNum = USB_EP_NUM(ep);

  if(USB_IS_EP_DIRECTION_IN(ep))
  {
    if(U_TRUE == enable)
    {
      if(0 == epNum)
      {
        AHB_USB->UsbDICtrl0 |= (0x1 << 27);
      }
      else
      {
        AHB_USB->UsbDIxConfig[epNum-1].DICtrlx |= (0x1 << 27);
      }
    }
    else
    {
      if(0 == epNum)
      {
        AHB_USB->UsbDICtrl0 |= (0x1 << 26);
      }
      else
      {
        AHB_USB->UsbDIxConfig[epNum-1].DICtrlx |= (0x1 << 26);
      }
    }
  }
}

#endif

