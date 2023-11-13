
#ifndef __PERIPHERAL_USB_H__
#define __PERIPHERAL_USB_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "ingsoc.h"

// =============================================================================
// macro and structure definition
// -----------------------------------------------------------------------------
//
// =============================================================================
#ifdef HAL_USB_PRINTF
  #include <stdio.h>
  #define HAL_USB_TRACE(format, args...) fprintf(stdout, format, ##args)
#else
  #define HAL_USB_TRACE(format, ...)
#endif /* HAL_USB_PRINTF */

#define USB_EP_DIRECTION  0x80

/* Fixed, not allowed to change */
#define RXFIFOSIZE 64
#define TXFIFOSIZE 64

// =============================================================================
/// This macro set the direction IN on an endpoint
// =============================================================================
#define USB_EP_DIRECTION_IN(X)    ((X)|USB_EP_DIRECTION)

// =============================================================================
/// This macro set the direction OUT on an endpoint
// =============================================================================
#define USB_EP_DIRECTION_OUT(X)   ((X)&~USB_EP_DIRECTION)

// =============================================================================
/// This macro define if the direction is IN
// =============================================================================
#define USB_IS_EP_DIRECTION_IN(X) ((X)&USB_EP_DIRECTION)

// =============================================================================
/// This macro return the endpoint index
// =============================================================================
#define USB_EP_NUM(X)             ((X)&(~USB_EP_DIRECTION))

/* fixed, not allowed to change */
#define USB_EP0_MPS             64
#define DIEP_NUM            5
#define DOEP_NUM            5
#define TOTAL_EP_NUM        6

#define U_FALSE (0)
#define U_TRUE  (1)

// =============================================================================
/// Enum and struct of USB module
// =============================================================================

/* user intmask, used in struct USB_INIT_CONFIG_T */
#define USBINTMASK_SOF (0x1 << 3)
#define USBINTMASK_SUSP (0x1 << 11)
#define USBINTMASK_RESUME (0x1U << 31)

typedef enum
{
  USB_ERROR_NONE = (1 << 1),
  USB_ERROR_INVALID_INPUT = (1 << 2),
  USB_ERROR_INACTIVE_EP = (1 << 3),
  USB_ERROR_REQUEST_NOT_SUPPORT = (1 << 4)
} USB_ERROR_TYPE_E ;

typedef enum
{
  USB_DEBUG_OPTIONS_NONE                                 = 0<<0,
  USB_DEBUG_OPTIONS_TRANSFER_SIZE                        = 1<<0,
  USB_DEBUG_OPTIONS_IRQ_STATUS                           = 1<<1,
  USB_DEBUG_OPTIONS_EP0_IN_STATUS                        = 1<<2,
  USB_DEBUG_OPTIONS_EP0_OUT_STATUS                       = 1<<3,
  USB_DEBUG_OPTIONS_GET_DUMP_REGS                        = 1<<4
} USB_DEBUG_OPTIONS_E ;

typedef enum
{
  USB_CONTROL_TRANSFER_STAGE_NONE                        = 0<<0,
  USB_CONTROL_TRANSFER_SETUP_STAGE                       = 1<<0,
  USB_CONTROL_TRANSFER_DATA_STAGE_OUT                    = 1<<1,
  USB_CONTROL_TRANSFER_DATA_STAGE_IN                     = 1<<2,
  USB_CONTROL_TRANSFER_STATUS_STAGE_IN                   = 1<<3,
  USB_CONTROL_TRANSFER_STATUS_STAGE_OUT                  = 1<<4,
  USB_CONTROL_TRANSFER_NO_DATA_CONTROL_STATUS_STAGE_IN   = 1<<5,
  USB_CONTROL_TRANSFER_DATA_STAGE_NOT_SUPPORT_CMD        = 1<<6,
  USB_CONTROL_TRANSFER_SETUP_TWO_STAGE                   = 1<<7,
  USB_CONTROL_TRANSFER_SETUP_THREE_STAGE                 = 1<<8,
  USB_CONTROL_TRANSFER_SETUP_STAGE_PROCESS               = 1<<9
} USB_CONTROL_TRANSFER_STAGE_E ;

typedef enum
{
  USB_REQUEST_DESTINATION_DEVICE    = 0,
  USB_REQUEST_DESTINATION_INTERFACE = 1,
  USB_REQUEST_DESTINATION_EP        = 2
} USB_REQUEST_DESTINATION_E ;

typedef enum
{
  USB_REQUEST_TYPE_STANDARD          = 0,
  USB_REQUEST_TYPE_CLASS             = 1,
  USB_REQUEST_TYPE_VENDOR            = 2,
  USB_REQUEST_TYPE_RESERVED          = 3
} USB_REQUEST_TYPE_E ;

typedef enum
{
  USB_REQUEST_DEVICE_GET_STATUS = 0x00,
  USB_REQUEST_DEVICE_CLEAR_FEATURE = 0x01,
  USB_REQUEST_DEVICE_SET_FEATURE = 0x03,
  USB_REQUEST_DEVICE_SET_ADDRESS = 0x05,
  USB_REQUEST_DEVICE_GET_DESCRIPTOR = 0x06,
  USB_REQUEST_DEVICE_SET_DESCRIPTOR = 0x07,
  USB_REQUEST_DEVICE_GET_CONFIGURATION = 0x08,
  USB_REQUEST_DEVICE_SET_CONFIGURATION = 0x09,
  USB_REQUEST_DEVICE_GET_INTERFACE = 0x0a,
  USB_REQUEST_DEVICE_SET_INTERFACE = 0x0b
} USB_REQUEST_DEVICE_E ;

/* USB Standard Feature selectors */
#define USB_FEATURE_ENDPOINT_STALL             0
#define USB_FEATURE_REMOTE_WAKEUP              1

typedef enum
{
  USB_REQUEST_DEVICE_DESCRIPTOR_DEVICE = 0x01,
  USB_REQUEST_DEVICE_DESCRIPTOR_CONFIGURATION = 0x02,
  USB_REQUEST_DEVICE_DESCRIPTOR_STRING = 0x03,
  USB_REQUEST_DEVICE_DESCRIPTOR_INTERFACE = 0x04,
  USB_REQUEST_DEVICE_DESCRIPTOR_ENDPOINT = 0x05,
  USB_REQUEST_DEVICE_DESCRIPTOR_DEVICE_QUALIFIER = 0x06,
  USB_REQUEST_DEVICE_DESCRIPTOR_OTHER_SPEED_CONFIGURATION = 0x07,
  USB_REQUEST_DEVICE_DESCRIPTOR_INTERFACE_POWER = 0x08
} USB_REQUEST_DEVICE_DESCRIPTOR_TYPES_E ;

typedef struct
{
    uint8_t  Recipient      :5;
    uint8_t  Type           :2;
    uint8_t  Direction:1;
} USB_SETUP_REQUEST_DESC_T;

typedef struct
{
    USB_SETUP_REQUEST_DESC_T    bmRequestType;
    uint8_t                         bRequest;
    uint16_t                        wValue;
    uint16_t                        wIndex;
    uint16_t                        wLength;
    uint8_t                         data[];
} USB_SETUP_T;

// =============================================================================
/// Device descriptor structure
// =============================================================================
typedef struct __attribute__((packed))
{
    uint8_t  size;
    uint8_t  type;
    uint16_t bcdUsb;
    uint8_t  usbClass;
    uint8_t  usbSubClass;
    uint8_t  usbProto;
    uint8_t  ep0Mps;
    uint16_t vendor;
    uint16_t product;
    uint16_t release;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerial;
    uint8_t  nbConfig;
} USB_DEVICE_DESCRIPTOR_REAL_T;

// =============================================================================
/// Config descriptor structure
// =============================================================================
typedef struct __attribute__((packed))
{
    uint8_t  size;
    uint8_t  type;
    uint16_t totalLength;
    uint8_t  nbInterface;
    uint8_t  configIndex;
    uint8_t  iDescription;
    uint8_t  attrib;
    uint8_t  maxPower;
} USB_CONFIG_DESCRIPTOR_REAL_T;

// =============================================================================
/// Interface descriptor structure
// =============================================================================
typedef struct __attribute__((packed))
{
    uint8_t  size;
    uint8_t  type;
    uint8_t  interfaceIndex;
    uint8_t  alternateSetting;
    uint8_t  nbEp;
    uint8_t  usbClass;
    uint8_t  usbSubClass;
    uint8_t  usbProto;
    uint8_t  iDescription;
} USB_INTERFACE_DESCRIPTOR_REAL_T;

// =============================================================================
/// Ep descriptor structure
// =============================================================================
typedef struct __attribute__((packed))
{
    uint8_t  size;
    uint8_t  type;
    uint8_t  ep;
    uint8_t  attributes;
    uint16_t mps;
    uint8_t  interval;
} USB_EP_DESCRIPTOR_REAL_T;

// =============================================================================
/// Return value of callback
// =============================================================================
typedef enum {
    USB_CALLBACK_RETURN_OK,
    USB_CALLBACK_RETURN_RUNNING,
    USB_CALLBACK_RETURN_KO
} USB_CALLBACK_RETURN_T ;

// =============================================================================
/// Type of callback
// =============================================================================
typedef enum
{
    /// Event send when a receive transfert is finish
    USB_CALLBACK_TYPE_RECEIVE_END,
    /// Event send when a transmit transfert is finish
    USB_CALLBACK_TYPE_TRANSMIT_END,
    /// Event send when interface is enable
    USB_CALLBACK_TYPE_ENABLE,
    /// Event send when interface is disable
    USB_CALLBACK_TYPE_DISABLE
} USB_CALLBACK_EP_TYPE_T;

// =============================================================================
// USB_CALLBACK_T
// -----------------------------------------------------------------------------
/// Type use to define endpoint/interface callback. It's call when an endpoint
/// commande  are receive on EP0.
/// @param type kind of action
/// @param setup Setup is the USB command data
// =============================================================================
typedef USB_CALLBACK_RETURN_T
(*USB_CALLBACK_T)(USB_CALLBACK_EP_TYPE_T   type,
                      USB_SETUP_T*             setup);


// ============================================================================
/// EndPoint type
// =============================================================================
typedef enum
{
    USB_EP_TYPE_CONTROL                     = 0x00000000,
    USB_EP_TYPE_ISO                         = 0x00000001,
    USB_EP_TYPE_BULK                        = 0x00000002,
    USB_EP_TYPE_INTERRUPT                   = 0x00000003
} USB_EP_TYPE_T;

typedef enum
{
  USB_DEVICE_NONE,
  /* A USB device may be attached or detached from the USB */
  USB_DEVICE_ATTACHED,
  /*USB devices may obtain power from an external source  */
  USB_DEVICE_POWERED,
  /*  After the device has been powered, and reset is done  */
  USB_DEVICE_DEFAULT,
  /*  All USB devices use the default address when initially powered or after the device has been reset. Each
      USB device is assigned a unique address by the host after attachment or after reset. */
  USB_DEVICE_ADDRESS,
  /* Before a USB device function may be used, the device must be configured. */
  USB_DEVICE_CONFIGURED,
  /* In order to conserve power, USB devices automatically enter the Suspended state when the device has
     observed no bus traffic for a specified period */
  USB_DEVICE_SUSPENDED,
  USB_DEVICE_TEST_RESET_DONE
}USB_DEVICE_STATE_E;

typedef enum
{
  EP0_DISCONNECT,
  EP0_IDLE,
  EP0_IN_DATA_PHASE,
  EP0_OUT_DATA_PHASE,
  EP0_IN_STATUS_PHASE,
  EP0_OUT_STATUS_PHASE,
  EP0_STALL,
}USB_EP0_STATE_E ;

typedef enum
{
  USB_EVENT_EP0_SETUP,
  USB_EVENT_EP_DATA_TRANSFER,
  USB_EVENT_DEVICE_RESET,
  USB_EVENT_DEVICE_SOF,
  USB_EVENT_DEVICE_SUSPEND,
  USB_EVENT_DEVICE_RESUME
}USB_EVNET_TYPE_E ;

typedef struct
{
  uint8_t  ep;
  USB_CALLBACK_EP_TYPE_T type;
  uint32_t size;
}USB_EVNET_DATA_TYPE_T ;

typedef struct
{
  USB_EVNET_TYPE_E id;
  USB_EVNET_DATA_TYPE_T data;
}USB_EVNET_HANDLER_T ;

typedef uint32_t (*USB_USER_EVENT_HANDLER)(USB_EVNET_HANDLER_T *);

typedef struct
{
  uint32_t intmask;//use "USBINTMASK_xx" to open extra interrupt
  /** Thresholding enable flags and length varaiables, only valid for isochronous transfer **/
  uint16_t rx_thr_en;
  uint16_t iso_tx_thr_en;
  uint16_t non_iso_tx_thr_en;
  uint16_t rx_thr_length;
  uint16_t tx_thr_length;
  /** Event handler for application layer **/
  USB_USER_EVENT_HANDLER handler;
}USB_INIT_CONFIG_T;

// =============================================================================
// -----------------------------------------------------------------------------
/// Structure containt the transfert parameter
// =============================================================================

typedef enum
{
  USB_TRANSFERT_FLAG_NONE,
  USB_TRANSFERT_FLAG_FLEXIBLE_RECV_LEN
} USB_TRANSFERT_FLAGS_E;

typedef struct
{
    int32_t  sizeRemaining;
    int32_t  sizeTransfered;
    int32_t  sizeTotalLen;
    uint32_t flags;
} USB_TRANSFERT_T;

typedef struct {
  /** EP number used for register address lookup */
  uint8_t num;
  /** EP direction 0 = OUT */
  uint32_t is_in:1;
  /** EP active. */
  uint32_t active:1;
  uint32_t type:2;
  /** Max Packet bytes */
  uint32_t maxpacket:11;

  /** stall clear flag */
  uint32_t stall_clear_flag:1;
  uint32_t unused:16;
} USB_VAR_EP_T;

typedef struct
{
  USB_DEVICE_STATE_E               DeviceState;
  USB_EP0_STATE_E                  Ep0State;

  // Transfert data
  USB_TRANSFERT_T             InTransfer [DIEP_NUM+1];
  USB_TRANSFERT_T             OutTransfer[DOEP_NUM+1];
  USB_VAR_EP_T                ep[TOTAL_EP_NUM];
  USB_INIT_CONFIG_T           UserConfig;
} USB_GLOBAL_VAR_S;

// =============================================================================
// external function definition
// -----------------------------------------------------------------------------
//
// =============================================================================

/**
 * @brief interface API. initilize usb module and related variables, must be called before any usb usage
 *
 * @param[in] device callback function with structure USB_INIT_CONFIG_T.
 *            When this function has been called your device is ready to be enumerated by the USB host.
 * @param[out] null.
 */
extern USB_ERROR_TYPE_E USB_InitConfig(USB_INIT_CONFIG_T *config);
/**
 * @brief interface API. send usb device data packet.
 *
 * @param[in] ep, endpoint number, the highest bit represent direction, use USB_EP_DIRECTION_IN/OUT
 * @param[in] buffer, global buffer to hold data of the packet, it must be a DWORD-aligned address.
 * @param[in] size. it should be a value smaller than 512*mps(eg, for EP0, mps is 64byte, maximum packet number is 512, MPS is 64).
 * @param[in] flag. null
 * @param[out] return U_TRUE if successful, otherwise U_FALSE.
 */
extern USB_ERROR_TYPE_E USB_SendData(uint8_t ep, void* buffer, uint16_t size, uint32_t flag);
/**
 * @brief interface API. receive usb device data packet.
 *
 * @param[in] ep, endpoint number, the highest bit represent direction, use USB_EP_DIRECTION_IN/OUT
 * @param[in] buffer, global buffer to hold data of the packet, it must be a DWORD-aligned address.
 * @param[in] size. For OUT transfers, the Transfer Size field in the endpoint Transfer Size register must be a multiple
 *            of the maximum packet size of the endpoint(eg, EP0 is 64byte), adjusted to the DWORD boundary
 * @param[in] flag. null
 * @param[out] return U_TRUE if successful, otherwise U_FALSE.
              for example, if the MPS of ep is 64bytes, there are two options:
              1. you know that you need to recieve exactly 64bytes, then set size to 64 and set flag to 0.
                 the driver will only call the event handler when it has received all 64bytes.
              2. you do know that size of next OUT packet, then set size to 64 and set to flag to 1<<USB_TRANSFERT_FLAG_FLEXIBLE_RECV_LEN.
                 in this case, driver will call back the event handler when it receives its first packet(no matter what the size is).
 * @param[out] return U_TRUE if successful, otherwise U_FALSE.
 */
extern USB_ERROR_TYPE_E USB_RecvData(uint8_t ep, void* buffer, uint16_t size, uint32_t flag);
/**
 * @brief interface API. These functions terminate transfers that are initiated, normally it can be used to prepare for next
 *        new transfer. it will abort all active transfer .
 * @param[in] null.
 * @param[out] null.
 */
extern void USB_ResetTransfert(void);
/**
 * @brief interface API. These functions terminate transfers that are initiated, normally it can be used to prepare for next
 *        new transfer. it will only abort the specific transfer .
 * @param[in] ep number.
 * @param[out] null.
 */
extern void USB_CancelTransfert(uint8_t ep);
/**
 * @brief interface API. the function erase all usb software configuration including descriptor settings.
 *        it will also break current connection, put the state machine into default status.
 * @param[in] null.
 * @param[out] null.
 */
extern void USB_ClrConfig(void);
/**
 * @brief interface API. shutdown usb module and reset all status data.
 *
 * @param[in] null.
 * @param[out] null.
 */
extern void USB_Close(void);
/**
 * @brief interface API. get current state of usb process, refer to USB_DEVICE_STATE_E for each state.
 *
 * @param[in] null.
 * @param[out] refer to USB_DEVICE_STATE_E.
 */
extern USB_DEVICE_STATE_E USB_GetDeviceState(void);
/**
 * @brief interface API. get setup data for ep0.
 *
 * @param[in] null.
 * @param[out] refer to USB_SETUP_T.
 */
extern USB_SETUP_T* USB_GetEp0SetupData(void);
/**
 * @brief interface API. used in enum process to enable a configured EP number.
 *
 * @param[in] ep descriptor structure.
 * @param[out] refer to USB_ERROR_TYPE_E.
 */
extern USB_ERROR_TYPE_E USB_ConfigureEp(const USB_EP_DESCRIPTOR_REAL_T* ep);
/**
 * @brief interface APIs. use this pair for enable/disable certain ep.
 *
 * @param[in] ep number with USB_EP_DIRECTION_IN/OUT.
 * @param[out] null
 */
extern void USB_EnableEp(uint8_t ep, USB_EP_TYPE_T type);
extern void USB_DisableEp(uint8_t ep);
/**
 * @brief internal API.
 *
 * @param[in] convert asicc to utf8.
 * @param[out] null.
 */
extern void USB_AsciiToUtf8(uint8_t *utf8, uint8_t *acsii);
/**
 * @brief interface API. system USB handler, config with "platform_set_irq_callback"
 *
 * @param[in] custom user data.
 * @param[out] null.
 */
extern uint32_t USB_IrqHandler (void *user_data);
/**
 * @brief interface API. set ep0 stall pid for current transfer
 *
 * @param[in] ep 0 with direction.
 * @param[out] null.
 */
extern void USB_SetEp0Stall(uint8_t ep);
/**
 * @brief interface API. set ep stall pid for current transfer
 *
 * @param[in] ep num with direction.
 * @param[in] U_TRUE: stall, U_FALSE: set back to normal
 * @param[out] null.
 */
extern void USB_SetStallEp(uint8_t ep, uint8_t stall);
/**
 * @brief interface API. use this reg to set resume signal on bus,
 * according to spec, the duration should be value large than 1ms but less than 15ms
 *
 * @param[in] U_TRUE: send resume signal; others: stop sending resume signal.
 * @param[out] null.
 */
extern void USB_DeviceSetRemoteWakeupBit(uint8_t enable);
/**
 * @brief interface API. use this api to set global NAK(the core will stop writing data on all out ep except setup packet)
 *
 * @param[in] U_TRUE: enable global NAK on all out ep. U_FALSE: stop global NAK
 * @param[out] null.
 */
extern void USB_SetGlobalOutNak(uint8_t enable);
/**
 * @brief interface API. use this api to set NAK on a specific IN ep
 *
 * @param[in] U_TRUE: enable NAK on required IN ep. U_FALSE: stop NAK
 * @param[in] ep: ep number with USB_EP_DIRECTION_IN/OUT.
 * @param[out] null.
 */
extern void USB_SetInEndpointNak(uint8_t ep, uint8_t enable);
/**
 * @brief interface API. use this api to get the real received size of OUT endpoint, normally this value is provided in event handler callback
 *
 * @param[in] ep: ep number with USB_EP_DIRECTION_IN/OUT.
 * @param[out] null.
 */
extern uint32_t USB_GetTransferedSize(uint8_t ep);

#ifdef __cplusplus
  }
#endif

#endif // __PERIPHERAL_USB_H__
