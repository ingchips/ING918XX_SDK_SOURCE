#ifndef __USB_MSC_H
#define __USB_MSC_H

#include <stdint.h>
#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Several functions of usb msc, see the macro definition in .h file */
#ifndef BSP_USB_MSC_FUNC
#define BSP_USB_MSC_FUNC BSP_USB_MSC_FLASH_DISK
#endif

// the flag to enable disconnection(cable unplugged), valid only when DP and DM are powered by VBUS
//#define FEATURE_DISCONN_DETECT

//! @brief Round up division to nearest number of words.
#define WORDS(s) (((s) + sizeof(uint32_t) - 1) / sizeof(uint32_t))

#if !defined(ARRAY_SIZE)
//! @brief Get number of elements in the array.
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

#if !defined(MIN)
#define MIN(a,b)                        ((a) < (b) ? (a) : (b))
#endif

#if !defined(MAX)
#define MAX(a,b)                        ((a) > (b) ? (a) : (b))
#endif

#define MB(size)                        ((size) * 1024 * 1024)
#define KB(size)                        ((size) * 1024)

#define ROUND_UP(value, boundary)       ((value) + ((boundary) - (value)) % (boundary))
#define ROUND_DOWN(value, boundary)     ((value) - ((value) % (boundary)))

#define COMPILER_CONCAT_(a, b) a##b
#define COMPILER_CONCAT(a, b) COMPILER_CONCAT_(a, b)

// Divide by zero if the the expression is false.  This
// causes an error at compile time.
//
// The special value '__COUNTER__' is used to create a unique value to
// append to 'compiler_assert_' to create a unique token.  This prevents
// conflicts resulting from the same enum being declared multiple times.
#define COMPILER_ASSERT(e) enum { COMPILER_CONCAT(compiler_assert_, __COUNTER__) = 1/((e) ? 1 : 0) }

#ifndef __MEMORY_AT
  #if (defined (__CC_ARM))
    #define  __MEMORY_AT(x)     __attribute__((at(x)))
  #elif (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
    #define  __MEMORY_AT__(x)   __attribute__((section(".ARM.__at_"#x)))
    #define  __MEMORY_AT(x)     __MEMORY_AT__(x)
  #else
    #define  __MEMORY_AT(x)
    #warning Position memory containing __MEMORY_AT macro at absolute address!
  #endif
#endif

// ATTENTION ! FIXED IO FOR USB on 916 series
#define USB_PIN_DP GIO_GPIO_16
#define USB_PIN_DM GIO_GPIO_17

#define USB_STRING_LANGUAGE_IDX  0x00
#define USB_STRING_LANGUAGE { 0x04, 0x03, 0x09, 0x04}

#define USB_STRING_MANUFACTURER_IDX  0x01
#define USB_STRING_MANUFACTURER {18,0x3,'I',0,'n',0,'g',0,'c',0,'h',0,'i',0,'p',0,'s',0}

#define USB_STRING_PRODUCT_IDX  0x02
#define USB_STRING_PRODUCT {16,0x3,'m',0,'s',0,'c',0,'-',0,'d',0,'e',0,'v',0}

#define USB_DEVICE_DESCRIPTOR \
{ \
    .size = sizeof(USB_DEVICE_DESCRIPTOR_REAL_T), \
    .type = 1, \
    .bcdUsb = 0x0200, \
    .usbClass = 0xFF, \
    .usbSubClass = 0x00, \
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

#define USB_DEVICE_CLASS_STORAGE               0x08
#define MSC_SUBCLASS_SCSI               0x06
#define MSC_PROTOCOL_BULK_ONLY          0x50

#define USB_INTERFACE_DESCRIPTOR \
{ \
  .size = sizeof(USB_INTERFACE_DESCRIPTOR_REAL_T), \
  .type = 4, \
  .interfaceIndex = 0x00, \
  .alternateSetting = 0x00, \
  .nbEp = 2,  \
  .usbClass = USB_DEVICE_CLASS_STORAGE, \
  .usbSubClass = MSC_SUBCLASS_SCSI, \
  .usbProto = MSC_PROTOCOL_BULK_ONLY, \
  .iDescription = 0x00 \
}

#define EP_MSC_IN (1)/* EP1 is in */
#define EP_MSC_OUT (2)
#define EP_X_MPS_BYTES (64)
#define USBD_MSC_MAX_PACKET     EP_X_MPS_BYTES

#define USB_EP_1_DESCRIPTOR \
{ \
  .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
  .type = 5, \
  .ep = USB_EP_DIRECTION_IN(EP_MSC_IN), \
  .attributes = USB_EP_TYPE_BULK, \
  .mps = EP_X_MPS_BYTES, \
  .interval = 0 \
}

#define USB_EP_2_DESCRIPTOR \
{ \
  .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
  .type = 5, \
  .ep = USB_EP_DIRECTION_OUT(EP_MSC_OUT), \
  .attributes = USB_EP_TYPE_BULK, \
  .mps = EP_X_MPS_BYTES, \
  .interval = 0 \
}

/* USB disk with all data stored in flash, with a fat12 file system */
#define BSP_USB_MSC_FLASH_DISK 0x03
/* USB disk with all data stored in flash, no file system, need to be formatted by host */
#define BSP_USB_MSC_FLASH_DISK_NO_VFS 0x04
/* USB disk with drag-n-drop bin file downloading function */
#define BSP_USB_MSC_FLASH_DISK_DOWNLOADER 0x05

#define MSC_REQUEST_RESET               0xFF
#define MSC_REQUEST_GET_MAX_LUN         0xFE

/* MSC Bulk-only Stage */
#define MSC_BS_CBW                      0       /* Command Block Wrapper */
#define MSC_BS_DATA_OUT                 1       /* Data Out Phase */
#define MSC_BS_DATA_IN                  2       /* Data In Phase */
#define MSC_BS_DATA_IN_LAST             3       /* Data In Last Phase */
#define MSC_BS_DATA_IN_LAST_STALL       4       /* Data In Last Phase with Stall */
#define MSC_BS_CSW                      5       /* Command Status Wrapper */
#define MSC_BS_ERROR                    6       /* Error */
#define MSC_BS_RESET                    7       /* Bulk-Only Mass Storage Reset */

/* Bulk-only Command Block Wrapper */
typedef __PACKED_STRUCT _MSC_CBW {
    uint32_t dSignature;
    uint32_t dTag;
    uint32_t dDataLength;
    uint8_t  bmFlags;
    uint8_t  bLUN;
    uint8_t  bCBLength;
    uint8_t  CB[16];
} MSC_CBW;

/* Bulk-only Command Status Wrapper */
typedef __PACKED_STRUCT _MSC_CSW {
    uint32_t dSignature;
    uint32_t dTag;
    uint32_t dDataResidue;
    uint8_t  bStatus;
} MSC_CSW;

#define MSC_CBW_Signature               0x43425355
#define MSC_CSW_Signature               0x53425355

/* CSW Status Definitions */
#define CSW_CMD_PASSED                  0x00
#define CSW_CMD_FAILED                  0x01
#define CSW_PHASE_ERROR                 0x02


/* SCSI Commands */
#define SCSI_TEST_UNIT_READY            0x00
#define SCSI_REQUEST_SENSE              0x03
#define SCSI_FORMAT_UNIT                0x04
#define SCSI_INQUIRY                    0x12
#define SCSI_MODE_SELECT6               0x15
#define SCSI_MODE_SENSE6                0x1A
#define SCSI_START_STOP_UNIT            0x1B
#define SCSI_MEDIA_REMOVAL              0x1E
#define SCSI_READ_FORMAT_CAPACITIES     0x23
#define SCSI_READ_CAPACITY              0x25
#define SCSI_READ10                     0x28
#define SCSI_WRITE10                    0x2A
#define SCSI_VERIFY10                   0x2F
#define SCSI_SYNC_CACHE10               0x35
#define SCSI_READ12                     0xA8
#define SCSI_WRITE12                    0xAA
#define SCSI_MODE_SELECT10              0x55
#define SCSI_MODE_SENSE10               0x5A
#define SCSI_SYNC_CACHE16               0x91
#define SCSI_ATA_COMMAND_PASS_THROUGH12 0xA1
#define SCSI_ATA_COMMAND_PASS_THROUGH16 0x85
#define SCSI_SERVICE_ACTION_IN12        0xAB
#define SCSI_SERVICE_ACTION_IN16        0x9E
#define SCSI_SERVICE_ACTION_OUT12       0xA9
#define SCSI_SERVICE_ACTION_OUT16       0x9F
#define SCSI_REPORT_ID_INFO             0xA3

#define USBD_MSC_INQUIRY_DATA       "INGCHIPS"         \
                                    "VFS             " \
                                    "0.1"

#define README_CONTENTS \
"This is ingchips's MassStorage Class demo.\r\n\r\n\
access https://ingchips.github.io/ for more information."

// FAT16 limitations +- safety margin
#define FAT_CLUSTERS_MAX (65525 - 100)
#define FAT_CLUSTERS_MIN (4086 + 100)

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

typedef char vfs_filename_t[11];

typedef struct FatDirectoryEntry {
    vfs_filename_t fileName;
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creationTimeMs;
    uint16_t creationTime;
    uint16_t creationDate;
    uint16_t accessedDate;
    uint16_t firstClusterHigh16;
    uint16_t modificationTime;
    uint16_t modificationDate;
    uint16_t firstClusterLow16;
    uint32_t fileSize;
} __attribute__((packed)) BSP_USB_MSC_DIR_ENTRY_t;
COMPILER_ASSERT(sizeof(BSP_USB_MSC_DIR_ENTRY_t) == 32);

typedef struct
{
  uint32_t remoteWakeup:1;
  uint32_t unused:31;
  
  MSC_CBW USB_MSC_CBW;
  MSC_CSW USB_MSC_CSW;
  uint16_t uBulkBufSize;
  uint8_t uBulkBuf[USBD_MSC_MAX_PACKET];

  uint8_t BulkStage;
  uint32_t BulkLen;
  uint32_t uEndPointMask;

  uint8_t uMediaReady;
  uint8_t uReadOnly;
  uint32_t uMemorySize;
  uint32_t uBlockSize;
  uint32_t uBlockGroup;
  uint32_t uBlockCount;
  uint8_t *uBlockBuf;
  uint8_t uMediaReadyEx;

  uint32_t Block;      /* R/W Block  */
  uint32_t Offset;     /* R/W Offset */
  uint32_t Length;     /* R/W Length */

  uint8_t MemOK;
}BSP_USB_MSC_VAR_s;

extern BSP_USB_MSC_VAR_s USB_Var;

/**
 * @brief use this API to initialize USB module and MSC function
 *
 * @param[in] null.
 * @param[out] null.
 */
extern void bsp_usb_init(void);
extern void bsp_usb_device_remote_wakeup(void);

#ifdef FEATURE_DISCONN_DETECT
void bsp_usb_device_disconn_timeout(void);
#endif
#if BSP_USB_MSC_FUNC == BSP_USB_MSC_FLASH_DISK_DOWNLOADER
void bsp_msc_image_detect_and_download(uint32_t sector, uint8_t *buf, uint32_t num_of_sectors);
#endif
#ifdef __cplusplus
}
#endif

#endif
