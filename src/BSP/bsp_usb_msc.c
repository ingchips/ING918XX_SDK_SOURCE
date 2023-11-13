#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "platform_api.h"
#include "bsp_usb_msc.h"
#include "eflash.h"
#include "ota_service.h"

/* User defined area */

#if BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK || BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK_NO_VFS \
      || BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK_DOWNLOADER

/* empty flash start address, used for usb stick */
#ifndef VFS_START_ADDR
#define VFS_START_ADDR 0x02040000
#endif

/* total size of usb disk, the size must be aligned with vector size and cnt */
#ifndef VFS_TOTAL_MEM_SIZE
#define VFS_TOTAL_MEM_SIZE MB(128)
#endif

/* block/sector size of usb disk, for total size of 256M, block size should be 4K.
   ATTENTION: flash operation requires that the minimum value of block size must be 4K */
#ifndef VFS_DISK_BLOCK_SIZE
#define VFS_DISK_BLOCK_SIZE KB(4)
#endif

#define VFS_DISK_BLOCK_NUM (VFS_TOTAL_MEM_SIZE/VFS_DISK_BLOCK_SIZE)
#define VFS_DISK_FATSZ_16 ((VFS_DISK_BLOCK_NUM*2)/VFS_DISK_BLOCK_SIZE)
COMPILER_ASSERT((VFS_DISK_BLOCK_NUM > FAT_CLUSTERS_MIN) && (VFS_DISK_BLOCK_NUM < FAT_CLUSTERS_MAX));
#endif

/* variables and functions area */
const USB_DEVICE_DESCRIPTOR_REAL_T DeviceDescriptor __attribute__ ((aligned (4))) = USB_DEVICE_DESCRIPTOR;

#define EP_IN_IDX 0
#define EP_OUT_IDX 1
const BSP_USB_DESC_STRUCTURE_T ConfigDescriptor __attribute__ ((aligned (4))) =
{USB_CONFIG_DESCRIPTOR, {USB_INTERFACE_DESCRIPTOR},{USB_EP_1_DESCRIPTOR,USB_EP_2_DESCRIPTOR}};
const uint8_t StringDescriptor_0[] __attribute__ ((aligned (4))) = USB_STRING_LANGUAGE;
const uint8_t StringDescriptor_1[] __attribute__ ((aligned (4))) = USB_STRING_MANUFACTURER;
const uint8_t StringDescriptor_2[] __attribute__ ((aligned (4))) = USB_STRING_PRODUCT;
const uint8_t StringInquiryData[] = USBD_MSC_INQUIRY_DATA;

BSP_USB_MSC_VAR_s USB_Var;
uint8_t DataRecvBuf[EP_X_MPS_BYTES] __attribute__ ((aligned (4)));
uint8_t DataSendBuf[EP_X_MPS_BYTES] __attribute__ ((aligned (4)));
uint8_t DataMscBuffer[VFS_DISK_BLOCK_SIZE];

#if BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK || BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK_DOWNLOADER
const uint8_t msc_disk[19][VFS_DISK_BLOCK_SIZE] __MEMORY_AT(VFS_START_ADDR) =
#endif
#if BSP_USB_MSC_FUNC != BSP_USB_MSC_FLASH_DISK_NO_VFS
{
  //------------- Block0: Boot Sector -------------//
  // BS_jmpBoot = EB; BS_OEMName = MSDOS; BPB_BytsPerSec = VFS_DISK_BLOCK_SIZE; BPB_RsvdSecCnt = 1;
  // BPB_NumFATs = 1; BPB_RootEntCnt = VFS_DISK_BLOCK_SIZE/32; BPB_TotSec16 = VFS_DISK_BLOCK_NUM; 
  // BPB_Media = F8; BPB_FATSz16 = VFS_DISK_FATSZ_16;
  // BPB_SecPerTrk = 1; BPB_NumHeads = 1; BPB_HiddSec = 0;
  // BPB_TotSec32 = 0; BS_DrvNum = 80; BS_VolID = 1234; BS_VolLab = INGCHIPSMSC; BS_FilSysType = FAT16
  {
      0xEB, 0x3C, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, VFS_DISK_BLOCK_SIZE&0xFF, 
      (VFS_DISK_BLOCK_SIZE>>8)&0xFF, 0x01, 0x01, 0x00,
      0x01, (VFS_DISK_BLOCK_SIZE/32)&0xFF, ((VFS_DISK_BLOCK_SIZE/32)>>8)&0xFF, VFS_DISK_BLOCK_NUM&0xFF, 
      (VFS_DISK_BLOCK_NUM>>8)&0xFF, 0xF8, VFS_DISK_FATSZ_16&0xFF, (VFS_DISK_FATSZ_16>>8)&0xFF,
      0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x29, 0x34, 0x12, 0x00, 0x00, 'I' , 'N' , 'G' , 'C' , 'H' ,
      'I' , 'P' , 'S' , 'M' , 'S' , 'C' , 0x46, 0x41, 0x54, 0x31, 0x36, 0x20, 0x20, 0x20, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
  },

  //------------- Block1-16: FAT16 Table -------------//
  {
      0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF // first 2 entries are reserved
  },
  {0x00,},{0x00,},{0x00,},{0x00,},{0x00,},{0x00,},{0x00,},
  {0x00,},{0x00,},{0x00,},{0x00,},{0x00,},{0x00,},{0x00,},{0x00,},
  //------------- Block17: Root Directory -------------//
  // DIR_Name = ; DIR_Attr = VOLUME ID; DIR_CrtTime = 0; DIR_CrtDate = 0;
  // DIR_CrtDate = 0; DIR_LstAccDate = 0; DIR_FileSize = SIZEOF()
  {
      // first entry is volume label
      'I' , 'N' , 'G' , 'C' , 'H' , 'I' , 'P' , 'S' , 'M' , 'S' , 'C' , 0x08, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x6D, 0x6f, 0x56, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      // second entry is readme file
      'R' , 'E' , 'A' , 'D' , 'M' , 'E' , ' ' , ' ' , 'T' , 'X' , 'T' , 0x20, 0x00, 0xC6, 0x52, 0x6D,
      0x6f, 0x56, 0x6f, 0x56, 0x00, 0x00, 0x88, 0x6D, 0x6f, 0x56, 0x02, 0x00,
      sizeof(README_CONTENTS)-1, 0x00, 0x00, 0x00 // readme's files size (4 Bytes)
  },

  //------------- Block18: Readme Content -------------//
  README_CONTENTS
};
#endif

void bsp_msc_write_sect(uint32_t sector, uint8_t *buf, uint32_t num_of_sectors)
{
    #if BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK || BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK_NO_VFS
    program_flash(VFS_START_ADDR + (sector * VFS_DISK_BLOCK_SIZE), buf, num_of_sectors * VFS_DISK_BLOCK_SIZE);
    #endif

    #if BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK_DOWNLOADER
    bsp_msc_image_detect_and_download(sector, buf, num_of_sectors);
    #endif
}

void bsp_msc_read_sect(uint32_t sector, uint8_t *buf, uint32_t num_of_sectors)
{
    #if BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK || BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK_NO_VFS \
        || BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK_DOWNLOADER
    memcpy(buf, (void*)(VFS_START_ADDR + (sector * VFS_DISK_BLOCK_SIZE)), num_of_sectors * VFS_DISK_BLOCK_SIZE);
    #endif
}

void bsp_msc_set_stall_ep_ (uint32_t EPNum)
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

void bsp_msc_clr_stall_ep_ (uint32_t EPNum)
{
    USB_SetStallEp(EPNum, U_FALSE);
}

uint32_t bsp_msc_write_ep (uint32_t EPNum, uint8_t *pData, uint32_t cnt)
{
    if(!USB_IS_EP_DIRECTION_IN(EPNum)){
      return 0;
    }

    memcpy(DataSendBuf, pData, cnt);
    USB_SendData(EPNum, (void*)DataSendBuf, cnt, 0);

    return cnt;
}

uint32_t bsp_msc_read_ep (uint32_t EPNum, uint8_t *pData, uint32_t size)
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

void bsp_msc_set_stall_ep(uint32_t EPNum)        /* set EP halt status according stall status */
{
    bsp_msc_set_stall_ep_(EPNum);
    USB_Var.uEndPointMask  |= (EPNum & 0x80) ? ((1 << 16) << (EPNum & 0x0F)) : (1 << EPNum);
}

void bsp_msc_clear_stall_ep(uint32_t EPNum)        /* clear EP halt status according stall status */
{
    uint32_t n, m;
    USB_SETUP_T* setup = USB_GetEp0SetupData();
    n = setup->wIndex & 0x8F;
    m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);

    if ((n == (USB_EP_DIRECTION_IN(EP_MSC_IN))) && ((USB_Var.uEndPointMask & m) != 0)) {
        if (USB_Var.USB_MSC_CSW.dSignature == MSC_CSW_Signature) {
            bsp_msc_write_ep(USB_EP_DIRECTION_IN(EP_MSC_IN), (uint8_t *)&USB_Var.USB_MSC_CSW, sizeof(USB_Var.USB_MSC_CSW));
        }
    }
}

void bsp_msc_set_csw(void)
{
    USB_Var.USB_MSC_CSW.dSignature = MSC_CSW_Signature;
    bsp_msc_write_ep(USB_EP_DIRECTION_IN(EP_MSC_IN), (uint8_t *)&USB_Var.USB_MSC_CSW, sizeof(USB_Var.USB_MSC_CSW));
    USB_Var.BulkStage = MSC_BS_CSW;
}

uint8_t bsp_msc_reset(void)
{
    USB_Var.uEndPointMask = 0x00000000;         /* EP must stay stalled */
    USB_Var.USB_MSC_CSW.dSignature = 0;             /* invalid signature */
    USB_Var.BulkStage = MSC_BS_RESET;
    return (U_TRUE);
}

uint32_t bsp_msc_check_media(void)
{
    USB_Var.uMediaReadyEx = USB_Var.uMediaReady;

    if (!USB_Var.uMediaReady) {
        if (USB_Var.USB_MSC_CBW.dDataLength) {
            if ((USB_Var.USB_MSC_CBW.bmFlags & 0x80) != 0) {
                bsp_msc_set_stall_ep(USB_EP_DIRECTION_IN(EP_MSC_IN));
            } else {
                if (USB_Var.USB_MSC_CSW.dDataResidue != USB_Var.BulkLen) {
                    // Only stall if this isn't the last transfer
                    bsp_msc_set_stall_ep(EP_MSC_OUT);
                }
            }
        }

        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_FAILED;
        bsp_msc_set_csw();
        return (U_FALSE);
    }

    return (U_TRUE);
}


void bsp_msc_memory_read(void)
{
    uint32_t n, m;

    if (USB_Var.Block >= USB_Var.uBlockCount) {
        n = 0;
        bsp_msc_set_stall_ep(USB_EP_DIRECTION_IN(EP_MSC_IN));
        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_PASSED;
        bsp_msc_set_csw();
    } else {
        if (USB_Var.Length > USBD_MSC_MAX_PACKET) {
            n = USBD_MSC_MAX_PACKET;
        } else {
            n = USB_Var.Length;
        }
    }

    if (!bsp_msc_check_media()) {
        n = 0;
    }

    if ((USB_Var.Offset == 0) && (n != 0)) {
        m = (USB_Var.Length + (USB_Var.uBlockSize - 1)) / USB_Var.uBlockSize;

        if (m > USB_Var.uBlockGroup) {
            m = USB_Var.uBlockGroup;
        }

        bsp_msc_read_sect(USB_Var.Block, USB_Var.uBlockBuf, m);
    }

    if (n) {
        bsp_msc_write_ep(USB_EP_DIRECTION_IN(EP_MSC_IN), &USB_Var.uBlockBuf[USB_Var.Offset], n);
        USB_Var.Offset += n;
        USB_Var.Length -= n;
    }

    if (USB_Var.Offset == USB_Var.uBlockGroup * USB_Var.uBlockSize) {
        USB_Var.Offset = 0;
        USB_Var.Block += USB_Var.uBlockGroup;
    }

    USB_Var.USB_MSC_CSW.dDataResidue -= n;

    if (!n) {
        return;
    }

    if (USB_Var.Length == 0) {
        USB_Var.BulkStage = MSC_BS_DATA_IN_LAST;
        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_PASSED;
    }
}

void bsp_msc_memory_write(void)
{
    uint32_t n;

    if (USB_Var.Block >= USB_Var.uBlockCount) {
        USB_Var.BulkLen = 0;
        bsp_msc_set_stall_ep(EP_MSC_OUT);
        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_PASSED;
        bsp_msc_set_csw();
    }

    if (!bsp_msc_check_media()) {
        USB_Var.BulkLen = 0;
    }

    if (USB_Var.Offset + USB_Var.BulkLen > USB_Var.uBlockSize) {
        // This write would have overflowed USB_Var.uBlockBuf
        return;
    }

    for (n = 0; n < USB_Var.BulkLen; n++) {
        USB_Var.uBlockBuf[USB_Var.Offset + n] = USB_Var.uBulkBuf[n];
    }

    USB_Var.Offset += USB_Var.BulkLen;
    USB_Var.Length -= USB_Var.BulkLen;

    if (USB_Var.BulkLen) {
        if ((USB_Var.Length == 0) && (USB_Var.Offset != 0)) {
            n = (USB_Var.Offset + (USB_Var.uBlockSize - 1)) / USB_Var.uBlockSize;

            if (n > USB_Var.uBlockGroup) {
                n = USB_Var.uBlockGroup;
            }

            bsp_msc_write_sect(USB_Var.Block, USB_Var.uBlockBuf, n);

            USB_Var.Offset = 0;
            USB_Var.Block += n;
        } else if (USB_Var.Offset == USB_Var.uBlockGroup * USB_Var.uBlockSize) {
            bsp_msc_write_sect(USB_Var.Block, USB_Var.uBlockBuf, USB_Var.uBlockGroup);
            USB_Var.Offset = 0;
            USB_Var.Block += USB_Var.uBlockGroup;
        }
    }

    USB_Var.USB_MSC_CSW.dDataResidue -= USB_Var.BulkLen;

    if (!USB_Var.BulkLen) {
        return;
    }

    if ((USB_Var.Length == 0) || (USB_Var.BulkStage == MSC_BS_CSW)) {
        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_PASSED;
        bsp_msc_set_csw();
    }
}

void bsp_msc_memory_verify(void)
{
    uint32_t n;

    if (USB_Var.Block >= USB_Var.uBlockCount) {
        USB_Var.BulkLen = 0;
        bsp_msc_set_stall_ep(EP_MSC_OUT);
        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_PASSED;
        bsp_msc_set_csw();
    }

    if (!bsp_msc_check_media()) {
        USB_Var.BulkLen = 0;
    }

    if (USB_Var.BulkLen) {
        if ((USB_Var.Offset == 0) && (USB_Var.BulkLen != 0)) {
            n = (USB_Var.Length + (USB_Var.uBlockSize - 1)) / USB_Var.uBlockSize;

            if (n > USB_Var.uBlockGroup) {
                n = USB_Var.uBlockGroup;
            }

           bsp_msc_read_sect(USB_Var.Block, USB_Var.uBlockBuf, n);
        }

        for (n = 0; n < USB_Var.BulkLen; n++) {
            if (USB_Var.uBlockBuf[USB_Var.Offset + n] != USB_Var.uBulkBuf[n]) {
                USB_Var.MemOK = U_FALSE;
                break;
            }
        }
    }

    USB_Var.Offset += USB_Var.BulkLen;
    USB_Var.Length -= USB_Var.BulkLen;

    if (USB_Var.Offset == USB_Var.uBlockGroup * USB_Var.uBlockSize) {
        USB_Var.Offset = 0;
        USB_Var.Block += USB_Var.uBlockGroup;
    }

    USB_Var.USB_MSC_CSW.dDataResidue -= USB_Var.BulkLen;

    if (!USB_Var.BulkLen) {
        return;
    }

    if ((USB_Var.Length == 0) || (USB_Var.BulkStage == MSC_BS_CSW)) {
        USB_Var.USB_MSC_CSW.bStatus = (USB_Var.MemOK) ? CSW_CMD_PASSED : CSW_CMD_FAILED;
        bsp_msc_set_csw();
    }
}

uint8_t bsp_msc_rw_setup(void)
{
    uint32_t n;
    /* Logical USB_Var.Block Address of First USB_Var.Block */
    n = (USB_Var.USB_MSC_CBW.CB[2] << 24) |
        (USB_Var.USB_MSC_CBW.CB[3] << 16) |
        (USB_Var.USB_MSC_CBW.CB[4] <<  8) |
        (USB_Var.USB_MSC_CBW.CB[5] <<  0);
    USB_Var.Block  = n;
    USB_Var.Offset = 0;

    /* Number of Blocks to transfer */
    switch (USB_Var.USB_MSC_CBW.CB[0]) {
        case SCSI_WRITE10:
        case SCSI_VERIFY10:
            if (!bsp_msc_check_media()) {
                return (U_FALSE);
            }

        case SCSI_READ10:
            n = (USB_Var.USB_MSC_CBW.CB[7] <<  8) |
                (USB_Var.USB_MSC_CBW.CB[8] <<  0);
            break;

        case SCSI_WRITE12:
            if (!bsp_msc_check_media()) {
                return (U_FALSE);
            }

        case SCSI_READ12:
            n = (USB_Var.USB_MSC_CBW.CB[6] << 24) |
                (USB_Var.USB_MSC_CBW.CB[7] << 16) |
                (USB_Var.USB_MSC_CBW.CB[8] <<  8) |
                (USB_Var.USB_MSC_CBW.CB[9] <<  0);
            break;
    }

    USB_Var.Length = n * USB_Var.uBlockSize;

    if (USB_Var.USB_MSC_CBW.dDataLength == 0) {     /* host requests no data */
        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_FAILED;
        bsp_msc_set_csw();
        return (U_FALSE);
    }

    if (USB_Var.USB_MSC_CBW.dDataLength != USB_Var.Length) {
        if ((USB_Var.USB_MSC_CBW.bmFlags & 0x80) != 0) {  /* stall appropriate EP */
            bsp_msc_set_stall_ep(USB_EP_DIRECTION_IN(EP_MSC_IN));
        } else {
            bsp_msc_set_stall_ep(EP_MSC_OUT);
        }

        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_FAILED;
        bsp_msc_set_csw();
        return (U_FALSE);
    }

    return (U_TRUE);
}



uint8_t bsp_msc_data_in_format(void)
{
    if (USB_Var.USB_MSC_CBW.dDataLength == 0) {
        USB_Var.USB_MSC_CSW.bStatus = CSW_PHASE_ERROR;
        bsp_msc_set_csw();
        return (U_FALSE);
    }

    if ((USB_Var.USB_MSC_CBW.bmFlags & 0x80) == 0) {
        bsp_msc_set_stall_ep(EP_MSC_OUT);
        USB_Var.USB_MSC_CSW.bStatus = CSW_PHASE_ERROR;
        bsp_msc_set_csw();
        return (U_FALSE);
    }

    return (U_TRUE);
}

void bsp_msc_data_in_transfer(void)
{
    if (USB_Var.BulkLen >= USB_Var.USB_MSC_CBW.dDataLength) {
        USB_Var.BulkLen = USB_Var.USB_MSC_CBW.dDataLength;
        USB_Var.BulkStage = MSC_BS_DATA_IN_LAST;
    } else {
        USB_Var.BulkStage = MSC_BS_DATA_IN_LAST_STALL; /* short or zero packet */
    }

    bsp_msc_write_ep(USB_EP_DIRECTION_IN(EP_MSC_IN), USB_Var.uBulkBuf, USB_Var.BulkLen);
    USB_Var.USB_MSC_CSW.dDataResidue -= USB_Var.BulkLen;
    USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_PASSED;
}

void bsp_msc_test_unit_ready(void)
{
    if (USB_Var.USB_MSC_CBW.dDataLength != 0) {
        if ((USB_Var.USB_MSC_CBW.bmFlags & 0x80) != 0) {
            bsp_msc_set_stall_ep(USB_EP_DIRECTION_IN(EP_MSC_IN));
        } else {
            bsp_msc_set_stall_ep(EP_MSC_OUT);
        }

        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_FAILED;
        bsp_msc_set_csw();
        return;
    }

    if (!bsp_msc_check_media()) {
        return;
    }

    USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_PASSED;
    bsp_msc_set_csw();
}

void bsp_msc_request_sense(void)
{
    if (!bsp_msc_data_in_format()) {
        return;
    }

    memset(USB_Var.uBulkBuf,0,sizeof(USB_Var.uBulkBuf));
    USB_Var.uBulkBuf[ 0] = 0x70;             /* Response Code */
    USB_Var.uBulkBuf[ 1] = 0x00;

    if ((USB_Var.uMediaReadyEx ^ USB_Var.uMediaReady) & USB_Var.uMediaReady) {  /* If media state changed to ready */
        USB_Var.uBulkBuf[ 2] = 0x06;           /* UNIT ATTENTION */
        USB_Var.uBulkBuf[12] = 0x28;           /* Additional Sense Code: Not ready to ready transition */
        USB_Var.uBulkBuf[13] = 0x00;           /* Additional Sense Code Qualifier */
        USB_Var.uMediaReadyEx = USB_Var.uMediaReady;
    } else if (!USB_Var.uMediaReady) {
        USB_Var.uBulkBuf[ 2] = 0x02;           /* NOT READY */
        USB_Var.uBulkBuf[12] = 0x3A;           /* Additional Sense Code: Medium not present */
        USB_Var.uBulkBuf[13] = 0x00;           /* Additional Sense Code Qualifier */
    } else {
        if (USB_Var.USB_MSC_CSW.bStatus == CSW_CMD_PASSED) {
            USB_Var.uBulkBuf[ 2] = 0x00;         /* NO SENSE */
            USB_Var.uBulkBuf[12] = 0x00;         /* Additional Sense Code: No additional code */
            USB_Var.uBulkBuf[13] = 0x00;         /* Additional Sense Code Qualifier */
        } else {
            USB_Var.uBulkBuf[ 2] = 0x05;         /* ILLEGAL REQUEST */
            USB_Var.uBulkBuf[12] = 0x20;         /* Additional Sense Code: Invalid command */
            USB_Var.uBulkBuf[13] = 0x00;         /* Additional Sense Code Qualifier */
        }
    }

    USB_Var.uBulkBuf[ 7] = 0x0A;             /* Additional USB_Var.Length */

    USB_Var.BulkLen = 18;
    bsp_msc_data_in_transfer();
}

void bsp_msc_inquiry(void)
{
    uint8_t  i;
    uint8_t *ptr_str;

    if (!bsp_msc_data_in_format()) {
        return;
    }

    USB_Var.uBulkBuf[ 0] = 0x00;             /* Direct Access Device */
    USB_Var.uBulkBuf[ 1] = 0x80;             /* RMB = 1: Removable Medium */
    USB_Var.uBulkBuf[ 2] = 0x02;             /* Version: ANSI X3.131: 1994 */
    USB_Var.uBulkBuf[ 3] = 0x02;
    USB_Var.uBulkBuf[ 4] = 36 - 4;           /* Additional USB_Var.Length */
    USB_Var.uBulkBuf[ 5] = 0x00;             /* SCCS = 0: No Storage Controller Component */
    USB_Var.uBulkBuf[ 6] = 0x00;
    USB_Var.uBulkBuf[ 7] = 0x00;
    ptr_str = (uint8_t *)StringInquiryData;

    for (i = 8; i < 36; i++) {               /* Product Information    + */
        if (*ptr_str) {                        /* Product Revision Level   */
            USB_Var.uBulkBuf[i] = *ptr_str++;
        } else {
            USB_Var.uBulkBuf[i] = ' ';
        }
    }

    USB_Var.BulkLen = 36;
    bsp_msc_data_in_transfer();
}

void bsp_msc_start_stop_unit(void)
{
    if (!USB_Var.USB_MSC_CBW.CB[3]) {               /* If power condition modifier is 0 */
        USB_Var.uMediaReady  = USB_Var.USB_MSC_CBW.CB[4] & 0x01;   /* Media ready = START bit value */
        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_PASSED; /* Start Stop Unit -> pass */
        bsp_msc_set_csw();
        return;
    }

    USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_FAILED;   /* Start Stop Unit -> fail */
    bsp_msc_set_csw();
}

void bsp_msc_media_removal(void)
{
    if (USB_Var.USB_MSC_CBW.CB[4] & 1) {            /* If prevent */
        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_FAILED;    /* Prevent media removal -> fail */
    } else {                                 /* If allow */
        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_PASSED;    /* Allow media removal -> pass */
    }

    bsp_msc_set_csw();
}

void bsp_msc_mode_sense6(void)
{
    if (!bsp_msc_data_in_format()) {
        return;
    }

    if (!bsp_msc_check_media()) {
        return;
    }

    USB_Var.uBulkBuf[ 0] = 0x03;
    USB_Var.uBulkBuf[ 1] = 0x00;
    USB_Var.uBulkBuf[ 2] = (USB_Var.uReadOnly << 7);
    USB_Var.uBulkBuf[ 3] = 0x00;
    USB_Var.BulkLen = 4;

    /* Win host requests maximum number of bytes but as all we have is 4 bytes we have
       to tell host back that it is all we have, that's why we correct residue */
    if (USB_Var.USB_MSC_CSW.dDataResidue > USB_Var.BulkLen) {
        USB_Var.USB_MSC_CBW.dDataLength  = USB_Var.BulkLen;
        USB_Var.USB_MSC_CSW.dDataResidue = USB_Var.BulkLen;
    }

    bsp_msc_data_in_transfer();
}

void bsp_msc_mode_sense10(void)
{
    if (!bsp_msc_data_in_format()) {
        return;
    }

    if (!bsp_msc_check_media()) {
        return;
    }

    USB_Var.uBulkBuf[ 0] = 0x00;
    USB_Var.uBulkBuf[ 1] = 0x06;
    USB_Var.uBulkBuf[ 2] = 0x00;
    USB_Var.uBulkBuf[ 3] = (USB_Var.uReadOnly << 7);
    USB_Var.uBulkBuf[ 4] = 0x00;
    USB_Var.uBulkBuf[ 5] = 0x00;
    USB_Var.uBulkBuf[ 6] = 0x00;
    USB_Var.uBulkBuf[ 7] = 0x00;
    USB_Var.BulkLen = 8;

    /* Win host requests maximum number of bytes but as all we have is 8 bytes we have
       to tell host back that it is all we have, that's why we correct residue */
    if (USB_Var.USB_MSC_CSW.dDataResidue > USB_Var.BulkLen) {
        USB_Var.USB_MSC_CBW.dDataLength  = USB_Var.BulkLen;
        USB_Var.USB_MSC_CSW.dDataResidue = USB_Var.BulkLen;
    }

    bsp_msc_data_in_transfer();
}

void bsp_msc_read_capacity(void)
{
    if (!bsp_msc_data_in_format()) {
        return;
    }

    if (!bsp_msc_check_media()) {
        return;
    }

    /* Last Logical USB_Var.Block */
    USB_Var.uBulkBuf[ 0] = ((USB_Var.uBlockCount - 1) >> 24) & 0xFF;
    USB_Var.uBulkBuf[ 1] = ((USB_Var.uBlockCount - 1) >> 16) & 0xFF;
    USB_Var.uBulkBuf[ 2] = ((USB_Var.uBlockCount - 1) >>  8) & 0xFF;
    USB_Var.uBulkBuf[ 3] = ((USB_Var.uBlockCount - 1) >>  0) & 0xFF;
    /* USB_Var.Block USB_Var.Length */
    USB_Var.uBulkBuf[ 4] = (USB_Var.uBlockSize        >> 24) & 0xFF;
    USB_Var.uBulkBuf[ 5] = (USB_Var.uBlockSize        >> 16) & 0xFF;
    USB_Var.uBulkBuf[ 6] = (USB_Var.uBlockSize        >>  8) & 0xFF;
    USB_Var.uBulkBuf[ 7] = (USB_Var.uBlockSize        >>  0) & 0xFF;
    USB_Var.BulkLen = 8;
    bsp_msc_data_in_transfer();
}

void bsp_msc_read_format_capacity(void)
{
    if (!bsp_msc_data_in_format()) {
        return;
    }

    if (!bsp_msc_check_media()) {
        return;
    }

    USB_Var.uBulkBuf[ 0] = 0x00;
    USB_Var.uBulkBuf[ 1] = 0x00;
    USB_Var.uBulkBuf[ 2] = 0x00;
    USB_Var.uBulkBuf[ 3] = 0x08;                      /* Capacity List USB_Var.Length */
    /* USB_Var.Block Count */
    USB_Var.uBulkBuf[ 4] = (USB_Var.uBlockCount >> 24) & 0xFF;
    USB_Var.uBulkBuf[ 5] = (USB_Var.uBlockCount >> 16) & 0xFF;
    USB_Var.uBulkBuf[ 6] = (USB_Var.uBlockCount >>  8) & 0xFF;
    USB_Var.uBulkBuf[ 7] = (USB_Var.uBlockCount >>  0) & 0xFF;
    /* USB_Var.Block USB_Var.Length */
    //USB_Var.uBulkBuf[ 8] = 0x01;                      /* Descriptor Code: UnFormatted Media */
    USB_Var.uBulkBuf[ 8] = 0x02;                      /* Descriptor Code: Formatted Media */
    USB_Var.uBulkBuf[ 9] = (USB_Var.uBlockSize  >> 16) & 0xFF;
    USB_Var.uBulkBuf[10] = (USB_Var.uBlockSize  >>  8) & 0xFF;
    USB_Var.uBulkBuf[11] = (USB_Var.uBlockSize  >>  0) & 0xFF;
    USB_Var.BulkLen = 12;

    /* Win host requests maximum number of bytes but as all we have is 12 bytes we have
       to tell host back that it is all we have, that's why we correct residue */
    if (USB_Var.USB_MSC_CSW.dDataResidue > USB_Var.BulkLen) {
        USB_Var.USB_MSC_CBW.dDataLength  = USB_Var.BulkLen;
        USB_Var.USB_MSC_CSW.dDataResidue = USB_Var.BulkLen;
    }

    bsp_msc_data_in_transfer();
}


void bsp_msc_synchronize_cache(void)
{
    /* Synchronize check always passes as we always write data directly
       so cache is always synchronized                                          */
    USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_PASSED;
    bsp_msc_set_csw();
}

void bsp_msc_ata_pass_through(void)
{
    if (!bsp_msc_data_in_format()) {
        return;
    }

    USB_Var.uBulkBuf[ 0] = 0x02;
    USB_Var.uBulkBuf[ 1] = 0x02;
    USB_Var.BulkLen = 2;
    USB_Var.BulkStage = MSC_BS_DATA_IN_LAST;

    /* Win host requests maximum number of bytes but as all we have is 2 bytes we have
       to tell host back that it is all we have, that's why we correct residue */
    if (USB_Var.USB_MSC_CSW.dDataResidue > USB_Var.BulkLen) {
        USB_Var.USB_MSC_CBW.dDataLength  = USB_Var.BulkLen;
        USB_Var.USB_MSC_CSW.dDataResidue = USB_Var.BulkLen;
    }

    bsp_msc_write_ep(USB_EP_DIRECTION_IN(EP_MSC_IN), USB_Var.uBulkBuf, USB_Var.BulkLen);
    USB_Var.USB_MSC_CSW.dDataResidue -= USB_Var.BulkLen;
    USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_FAILED;
}

void bsp_msc_service_action_in16(void)
{
    if (!bsp_msc_data_in_format()) {
        return;
    }

    USB_Var.uBulkBuf[ 0] = 0x20;
    USB_Var.uBulkBuf[ 1] = 0x00;
    USB_Var.uBulkBuf[31] = 0x00;
    USB_Var.BulkLen = 32;
    USB_Var.BulkStage = MSC_BS_DATA_IN_LAST;
    bsp_msc_write_ep(USB_EP_DIRECTION_IN(EP_MSC_IN), USB_Var.uBulkBuf, USB_Var.BulkLen);
    USB_Var.USB_MSC_CSW.dDataResidue -= USB_Var.BulkLen;
    USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_FAILED;
}

void bsp_msc_get_cbw(void)
{
    uint32_t n;
    uint32_t copy_size;
    copy_size = MIN(USB_Var.BulkLen, sizeof(USB_Var.USB_MSC_CBW));

    for (n = 0; n < copy_size; n++) {
        *((uint8_t *)&USB_Var.USB_MSC_CBW + n) = USB_Var.uBulkBuf[n];
    }

    if ((USB_Var.BulkLen == sizeof(USB_Var.USB_MSC_CBW)) && (USB_Var.USB_MSC_CBW.dSignature == MSC_CBW_Signature)) {
        /* Valid USB_Var.USB_MSC_CBW */
        USB_Var.USB_MSC_CSW.dTag = USB_Var.USB_MSC_CBW.dTag;
        USB_Var.USB_MSC_CSW.dDataResidue = USB_Var.USB_MSC_CBW.dDataLength;

        if ((USB_Var.USB_MSC_CBW.bLUN      >  0) ||
                (USB_Var.USB_MSC_CBW.bCBLength <  1) ||
                (USB_Var.USB_MSC_CBW.bCBLength > 16)) {
fail:
            USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_FAILED;
            bsp_msc_set_csw();
        } else {
            switch (USB_Var.USB_MSC_CBW.CB[0]) {
                case SCSI_TEST_UNIT_READY:
                    bsp_msc_test_unit_ready();
                    break;

                case SCSI_REQUEST_SENSE:
                    bsp_msc_request_sense();
                    break;

                case SCSI_FORMAT_UNIT:
                    goto fail;

                case SCSI_INQUIRY:
                    bsp_msc_inquiry();
                    break;

                case SCSI_START_STOP_UNIT:
                    bsp_msc_start_stop_unit();
                    break;

                case SCSI_MEDIA_REMOVAL:
                    bsp_msc_media_removal();
                    break;

                case SCSI_MODE_SELECT6:
                    goto fail;

                case SCSI_MODE_SENSE6:
                    bsp_msc_mode_sense6();
                    break;

                case SCSI_MODE_SELECT10:
                    goto fail;

                case SCSI_MODE_SENSE10:
                    bsp_msc_mode_sense10();
                    break;

                case SCSI_READ_FORMAT_CAPACITIES:
                    bsp_msc_read_format_capacity();
                    break;

                case SCSI_READ_CAPACITY:
                    bsp_msc_read_capacity();
                    break;

                case SCSI_ATA_COMMAND_PASS_THROUGH16:
                case SCSI_ATA_COMMAND_PASS_THROUGH12:
                    bsp_msc_ata_pass_through();
                    break;

                case SCSI_SERVICE_ACTION_IN16:
                    bsp_msc_service_action_in16();
                    break;

                case SCSI_READ10:
                case SCSI_READ12:
                    if (bsp_msc_rw_setup()) {
                        if ((USB_Var.USB_MSC_CBW.bmFlags & 0x80) != 0) {
                            USB_Var.BulkStage = MSC_BS_DATA_IN;
                            bsp_msc_memory_read();
                        } else {                       /* direction mismatch */
                            bsp_msc_set_stall_ep(EP_MSC_OUT);
                            USB_Var.USB_MSC_CSW.bStatus = CSW_PHASE_ERROR;
                            bsp_msc_set_csw();
                        }
                    }

                    break;

                case SCSI_WRITE10:
                case SCSI_WRITE12:
                    if (bsp_msc_rw_setup()) {
                        if ((USB_Var.USB_MSC_CBW.bmFlags & 0x80) == 0) {
                            USB_Var.BulkStage = MSC_BS_DATA_OUT;
                        } else {                       /* direction mismatch */
                            bsp_msc_set_stall_ep(USB_EP_DIRECTION_IN(EP_MSC_IN));
                            USB_Var.USB_MSC_CSW.bStatus = CSW_PHASE_ERROR;
                            bsp_msc_set_csw();
                        }
                    }

                    break;

                case SCSI_VERIFY10:
                    if ((USB_Var.USB_MSC_CBW.CB[1] & 0x02) == 0) {
                        // BYTCHK = 0 -> CRC Check (not implemented)
                        USB_Var.USB_MSC_CSW.bStatus = CSW_CMD_PASSED;
                        bsp_msc_set_csw();
                        break;
                    }

                    if (bsp_msc_rw_setup()) {
                        if ((USB_Var.USB_MSC_CBW.bmFlags & 0x80) == 0) {
                            USB_Var.BulkStage = MSC_BS_DATA_OUT;
                            USB_Var.MemOK = U_TRUE;
                        } else {
                            bsp_msc_set_stall_ep(USB_EP_DIRECTION_IN(EP_MSC_IN));
                            USB_Var.USB_MSC_CSW.bStatus = CSW_PHASE_ERROR;
                            bsp_msc_set_csw();
                        }
                    }

                    break;

                case SCSI_SYNC_CACHE10:
                case SCSI_SYNC_CACHE16:
                    bsp_msc_synchronize_cache();
                    break;

                case SCSI_REPORT_ID_INFO:
                    bsp_msc_set_stall_ep(USB_EP_DIRECTION_IN(EP_MSC_IN));
                    goto fail;

                default:
                    goto fail;
            }
        }
    } else {
        /* Invalid USB_Var.USB_MSC_CBW */
        bsp_msc_set_stall_ep(USB_EP_DIRECTION_IN(EP_MSC_IN));
        bsp_msc_set_stall_ep(EP_MSC_OUT);
        USB_Var.BulkStage = MSC_BS_ERROR;
    }
}

void bsp_msc_bulk_in(void)
{
    switch (USB_Var.BulkStage) {
        case MSC_BS_DATA_IN:
            switch (USB_Var.USB_MSC_CBW.CB[0]) {
                case SCSI_READ10:
                case SCSI_READ12:
                    bsp_msc_memory_read();
                    break;
            }

            break;

        case MSC_BS_DATA_IN_LAST:
            bsp_msc_set_csw();
            break;

        case MSC_BS_DATA_IN_LAST_STALL:
            bsp_msc_set_stall_ep(USB_EP_DIRECTION_IN(EP_MSC_IN));
            bsp_msc_set_csw();
            break;

        case MSC_BS_CSW:
            USB_Var.BulkStage = MSC_BS_CBW;
            break;

        default:
            break;
    }

}

void bsp_msc_bulk_out(void)
{
    USB_Var.BulkLen = bsp_msc_read_ep(EP_MSC_OUT, USB_Var.uBulkBuf, USB_Var.uBulkBufSize);
    switch (USB_Var.BulkStage) {
        case MSC_BS_CBW:
            bsp_msc_get_cbw();
            break;

        case MSC_BS_DATA_OUT:
            switch (USB_Var.USB_MSC_CBW.CB[0]) {
                case SCSI_WRITE10:
                case SCSI_WRITE12:
                    bsp_msc_memory_write();
                    break;

                case SCSI_VERIFY10:
                    bsp_msc_memory_verify();
                    break;
            }
            break;

        case MSC_BS_CSW:
            // Previous transfer must be complete
            // before the next transfer begins.
            //
            // If bulk out is stalled then just
            // drop this packet and don't assert.
            // This packet was left over from before
            // the transfer aborted with a stall.
            break;

        case MSC_BS_RESET:
            // If Bulk-Only Mass Storage Reset command was received on
            // Control Endpoint ignore next Bulk OUT transfer if it was not
            // a CBW (as it means it was a unprocessed leftover from
            // transfer before reset happened)
            USB_Var.BulkStage = MSC_BS_CBW;
            if (USB_Var.BulkLen == sizeof(USB_Var.USB_MSC_CBW)) {
                // If it is a CBW size packet go process it as CBW
                bsp_msc_get_cbw();
            }
            break;

        default:
            bsp_msc_set_stall_ep(EP_MSC_OUT);
            USB_Var.USB_MSC_CSW.bStatus = CSW_PHASE_ERROR;
            bsp_msc_set_csw();
            break;
    }

}

void bsp_msc_init(void)
{
    USB_Var.BulkStage = MSC_BS_CBW;
    USB_Var.uBulkBufSize = USBD_MSC_MAX_PACKET;
    USB_Var.uEndPointMask = 0;
    USB_Var.uMediaReady = 1;
    USB_Var.uMediaReadyEx = 0;
    USB_Var.uReadOnly = 0;
    USB_Var.uMemorySize = VFS_TOTAL_MEM_SIZE;
    USB_Var.uBlockSize  = VFS_DISK_BLOCK_SIZE;
    USB_Var.uBlockGroup = 1;
    USB_Var.uBlockCount = USB_Var.uMemorySize / USB_Var.uBlockSize;
    USB_Var.uBlockBuf   = (uint8_t *)DataMscBuffer;
    memset(DataMscBuffer,0,sizeof(DataMscBuffer));
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
                USB_Var.remoteWakeup = (setup->wValue&0xF) ? 0 : 1;
                status = USB_ERROR_NONE;
              }
              break;
              case USB_REQUEST_DEVICE_SET_FEATURE:
              {
                USB_Var.remoteWakeup = (setup->wValue&0xF) ? 1 : 0;
                status = USB_ERROR_NONE;
              }
              break;
              case USB_REQUEST_DEVICE_SET_CONFIGURATION:
              {
                // uint8_t cfg_idx = setup->wValue&0xFF;
                // check if the cfg_idx is correct
                status |= USB_ConfigureEp(&(ConfigDescriptor.endpoint[0]));
                status |= USB_ConfigureEp(&(ConfigDescriptor.endpoint[1]));

                status |= USB_RecvData(ConfigDescriptor.endpoint[EP_OUT_IDX].ep, DataRecvBuf,
                          ConfigDescriptor.endpoint[EP_OUT_IDX].mps, 1<<USB_TRANSFERT_FLAG_FLEXIBLE_RECV_LEN);
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
                  case MSC_REQUEST_RESET:
                  {
                    if ((setup->wValue   == 0) && (setup->wLength  == 0)) {
                      bsp_msc_reset();
                    }
                  }break;
                  case MSC_REQUEST_GET_MAX_LUN:
                  {
                    if ((setup->wValue == 0) && (setup->wLength == 1)) {
                      DataSendBuf[0] = 0;
                      status |= USB_SendData(0, DataSendBuf, 1, 0);
                    }
                  }break;
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
                    bsp_msc_clr_stall_ep_(n);
                    USB_Var.uEndPointMask &= ~m;
                }
              }
              break;
              case USB_REQUEST_DEVICE_SET_FEATURE:
              {
                uint8_t n = setup->wIndex & 0x8F;
                uint32_t m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);

                if (setup->wValue == USB_FEATURE_ENDPOINT_STALL) {
                  bsp_msc_set_stall_ep_(n);
                  USB_Var.uEndPointMask |=  m;
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
            if(event->data.ep == EP_MSC_OUT)
            {
              bsp_msc_bulk_out();
            }
          }break;
          case USB_CALLBACK_TYPE_TRANSMIT_END:
          {
            if(event->data.ep == EP_MSC_IN)
            {
              bsp_msc_bulk_in();
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

    bsp_msc_init();

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

#if BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK_DOWNLOADER
void bsp_msc_image_detect_and_download(uint32_t sector, uint8_t *buf, uint32_t num_of_sectors)
{
    #define ROOT_DIR_SECTOR_NUM (VFS_DISK_FATSZ_16 + 1)
    #define ROOT_DIR_CNT (VFS_DISK_BLOCK_SIZE/32)
    uint32_t i;
    static uint8_t bin_type = 0;
    static int32_t file_size = 0;
    static uint32_t file_sector = 0, meta_size = 0, meta_addr = 0;
    if(sector == ROOT_DIR_SECTOR_NUM)
    {
      BSP_USB_MSC_DIR_ENTRY_t *new_dir = (BSP_USB_MSC_DIR_ENTRY_t*)buf;
      for (i = 0; i < ROOT_DIR_CNT; i++) {
        if (0 == strncmp("BIN", &(new_dir[i].fileName[8]), 3)) {
          bin_type = 1;
          file_size = new_dir[i].fileSize;
          file_sector = ROOT_DIR_SECTOR_NUM + 1 + new_dir[i].firstClusterLow16 - 2;
          meta_size = file_size;
          meta_addr = VFS_START_ADDR + (file_sector * VFS_DISK_BLOCK_SIZE);
          break;
        }
      }
    }

    program_flash(VFS_START_ADDR + (sector * VFS_DISK_BLOCK_SIZE), buf, num_of_sectors * VFS_DISK_BLOCK_SIZE);

    if((bin_type == 1) && (sector == file_sector)){
      file_size = file_size - num_of_sectors * VFS_DISK_BLOCK_SIZE;
      if(file_size > 0){
        file_sector += num_of_sectors;
      }
      else{
        bin_type = 0;
        //download complete, reboot now
        uint8_t buffer[20];
        ota_meta_t  *meta = (ota_meta_t *)(buffer + 0);
        meta->blocks[0].dest = 0x02002000;
        meta->blocks[0].src = meta_addr;
        meta->blocks[0].size = meta_size;
        flash_do_update(1, meta->blocks, USB_Var.uBlockBuf);
      }
    }
}
#endif
