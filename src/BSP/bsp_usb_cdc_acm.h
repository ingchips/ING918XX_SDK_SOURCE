#ifndef __USB_CDC_ACM_H
#define __USB_CDC_ACM_H

#include <stdint.h>
#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
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

#define USB_DEVICE_CLASS_COMMUNICATIONS        0x02

#define USB_DEVICE_DESCRIPTOR \
{ \
    .size = sizeof(USB_DEVICE_DESCRIPTOR_REAL_T), \
    .type = 1, \
    .bcdUsb = 0x0200, \
    .usbClass = USB_DEVICE_CLASS_COMMUNICATIONS, \
    .usbSubClass = 0x00, \
    .usbProto = 0x00, \
    .ep0Mps = USB_EP0_MPS, \
    .vendor = 0xFFF1, \
    .product = 0xFA2F, \
    .release = 0x00, \
    .iManufacturer = USB_STRING_MANUFACTURER_IDX, \
    .iProduct = USB_STRING_PRODUCT_IDX, \
    .iSerial = 0x00, \
    .nbConfig = 0x01 \
}

/* Table 18: Sample Communications Class Specific Interface Descriptor* */
typedef struct __attribute__((packed))
{
    uint8_t  bFunctionLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint16_t bcdCDC;
} USB_HEADER_FUNCTIONAL_DESCRIPTOR_T;

typedef struct __attribute__((packed))
{
    uint8_t  bFunctionLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bmCapabilities;
    uint8_t  bDataInterface;
} USB_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_T;

typedef struct __attribute__((packed))
{
    uint8_t  bFunctionLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bmCapabilities;
} USB_ABSTRACT_CONTROL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_T;

typedef struct __attribute__((packed))
{
    uint8_t  bFunctionLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bControlInterface;
    uint8_t  bSubordinateInterface0;
} USB_UNION_DESCRIPTOR_FUNCTIONAL_DESCRIPTOR_T;

typedef struct __attribute__((packed))
{
    USB_CONFIG_DESCRIPTOR_REAL_T config;
    USB_INTERFACE_DESCRIPTOR_REAL_T interface_0;
    USB_HEADER_FUNCTIONAL_DESCRIPTOR_T header;
    USB_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_T call;
    USB_ABSTRACT_CONTROL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_T abs;
    USB_UNION_DESCRIPTOR_FUNCTIONAL_DESCRIPTOR_T uni;
    USB_EP_DESCRIPTOR_REAL_T ep_0;
    USB_INTERFACE_DESCRIPTOR_REAL_T interface_1;
    USB_EP_DESCRIPTOR_REAL_T ep_1_out;
    USB_EP_DESCRIPTOR_REAL_T ep_1_in;
}BSP_USB_DESC_STRUCTURE_T;

#define SELF_POWERED (1)
#define REMOTE_WAKEUP (0)

#define USB_CONFIG_DESCRIPTOR \
{ \
  .size = sizeof(USB_CONFIG_DESCRIPTOR_REAL_T), \
  .type = 2, \
  .totalLength = sizeof(BSP_USB_DESC_STRUCTURE_T), \
  .nbInterface = 0x02, \
  .configIndex = 0x01, \
  .iDescription = 0x00, \
  .attrib = 0x80| (SELF_POWERED<<6) | (REMOTE_WAKEUP<<5), \
  .maxPower = 0xFA \
}
/* usbcdc11.pdf */
#define CDC_COMMUNICATION_INTERFACE_CLASS       0x02
#define CDC_DATA_INTERFACE_CLASS                0x0A

#define CDC_DIRECT_LINE_CONTROL_MODEL           0x01
#define CDC_ABSTRACT_CONTROL_MODEL              0x02
#define CDC_TELEPHONE_CONTROL_MODEL             0x03
#define CDC_MULTI_CHANNEL_CONTROL_MODEL         0x04
#define CDC_CAPI_CONTROL_MODEL                  0x05
#define CDC_ETHERNET_NETWORKING_CONTROL_MODEL   0x06
#define CDC_ATM_NETWORKING_CONTROL_MODEL        0x07

#define CDC_CS_INTERFACE                        0x24
#define CDC_CS_ENDPOINT                         0x25

#define CDC_HEADER                              0x00
#define CDC_CALL_MANAGEMENT                     0x01
#define CDC_ABSTRACT_CONTROL_MANAGEMENT         0x02
#define CDC_DIRECT_LINE_MANAGEMENT              0x03
#define CDC_TELEPHONE_RINGER                    0x04
#define CDC_REPORTING_CAPABILITIES              0x05
#define CDC_UNION                               0x06
#define CDC_COUNTRY_SELECTION                   0x07
#define CDC_TELEPHONE_OPERATIONAL_MODES         0x08
#define CDC_USB_TERMINAL                        0x09
#define CDC_NETWORK_CHANNEL                     0x0A
#define CDC_PROTOCOL_UNIT                       0x0B
#define CDC_EXTENSION_UNIT                      0x0C
#define CDC_MULTI_CHANNEL_MANAGEMENT            0x0D
#define CDC_CAPI_CONTROL_MANAGEMENT             0x0E
#define CDC_ETHERNET_NETWORKING                 0x0F
#define CDC_ATM_NETWORKING                      0x10

#define WBVAL(x)                          (x & 0xFF),((x >> 8) & 0xFF)

#define USB_INTERFACE_0_DESCRIPTOR \
{ \
  .size = sizeof(USB_INTERFACE_DESCRIPTOR_REAL_T), \
  .type = 4, \
  .interfaceIndex = 0x00, \
  .alternateSetting = 0x00, \
  .nbEp = 1,  \
  .usbClass = CDC_COMMUNICATION_INTERFACE_CLASS, \
  .usbSubClass = CDC_ABSTRACT_CONTROL_MODEL, \
  .usbProto = 0x01, \
  .iDescription = 0x00 \
}

#define USB_HEADER_FUNCTIONAL_DESCRIPTOR \
{ \
  .bFunctionLength = sizeof(USB_HEADER_FUNCTIONAL_DESCRIPTOR_T), \
  .bDescriptorType = CDC_CS_INTERFACE, \
  .bDescriptorSubtype = CDC_HEADER, \
  .bcdCDC = 0x0110 \
}

#define USB_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR \
{ \
  .bFunctionLength = sizeof(USB_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_T), \
  .bDescriptorType = CDC_CS_INTERFACE, \
  .bDescriptorSubtype = CDC_CALL_MANAGEMENT, \
  .bmCapabilities = 0x03, \
  .bDataInterface = 0x01 \
}

/* Table 4: Abstract Control Management Functional Descriptor : bmCapabilities
D7..D4: RESERVED (Reset to zero) 
D3: 1 - Device supports the notification  Network_Connection. 
D2: 1 - Device supports the request Send_Break 
D1: 1 - Device supports the request combination of 
    Set_Line_Coding, Set_Control_Line_State, 
    Get_Line_Coding, and the notification 
    Serial_State. 
D0: 1 - Device supports the request combination of 
    Set_Comm_Feature, Clear_Comm_Feature, and 
    Get_Comm_Feature. 
*/
#define USB_ABSTRACT_CONTROL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR \
{ \
  .bFunctionLength = sizeof(USB_ABSTRACT_CONTROL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR_T), \
  .bDescriptorType = CDC_CS_INTERFACE, \
  .bDescriptorSubtype = CDC_ABSTRACT_CONTROL_MANAGEMENT, \
  .bmCapabilities = 0x02 \
}

#define USB_UNION_DESCRIPTOR_FUNCTIONAL_DESCRIPTOR \
{ \
  .bFunctionLength = sizeof(USB_UNION_DESCRIPTOR_FUNCTIONAL_DESCRIPTOR_T), \
  .bDescriptorType = CDC_CS_INTERFACE, \
  .bDescriptorSubtype = CDC_UNION, \
  .bControlInterface = 0x00, \
  .bSubordinateInterface0 = 0x01 \
}

#define EP_CDC_INI_IN (1)/* INTERRUPT */

#define USB_EP_CDC_INI_DESCRIPTOR \
{ \
  .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
  .type = 5, \
  .ep = USB_EP_DIRECTION_IN(EP_CDC_INI_IN), \
  .attributes = USB_EP_TYPE_INTERRUPT, \
  .mps = 64, \
  .interval = 2 \
}

#define USB_INTERFACE_1_DESCRIPTOR \
{ \
  .size = sizeof(USB_INTERFACE_DESCRIPTOR_REAL_T), \
  .type = 4, \
  .interfaceIndex = 0x01, \
  .alternateSetting = 0x00, \
  .nbEp = 2,  \
  .usbClass = CDC_DATA_INTERFACE_CLASS, \
  .usbSubClass = 0x00, \
  .usbProto = 0x00, \
  .iDescription = 0x00 \
}

#define EP_CDC_BULK_OUT (2)
#define EP_CDC_BULK_IN  (3)
#define EP_X_MPS_BYTES (64)

#define USB_EP_CDC_BULK_OUT_DESCRIPTOR \
{ \
  .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
  .type = 5, \
  .ep = USB_EP_DIRECTION_OUT(EP_CDC_BULK_OUT), \
  .attributes = USB_EP_TYPE_BULK, \
  .mps = EP_X_MPS_BYTES, \
  .interval = 0 \
}

#define USB_EP_CDC_BULK_IN_DESCRIPTOR \
{ \
  .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
  .type = 5, \
  .ep = USB_EP_DIRECTION_IN(EP_CDC_BULK_IN), \
  .attributes = USB_EP_TYPE_BULK, \
  .mps = EP_X_MPS_BYTES, \
  .interval = 0 \
}

/* CDC class-specific notification codes */
/* see Table 67 for Info about class-specific notifications */
#define CDC_NOTIFICATION_NETWORK_CONNECTION     0x00
#define CDC_RESPONSE_AVAILABLE                  0x01
#define CDC_AUX_JACK_HOOK_STATE                 0x08
#define CDC_RING_DETECT                         0x09
#define CDC_NOTIFICATION_SERIAL_STATE           0x20
#define CDC_CALL_STATE_CHANGE                   0x28
#define CDC_LINE_STATE_CHANGE                   0x29
#define CDC_CONNECTION_SPEED_CHANGE             0x2A

/* UART state bitmap values (Serial state notification). */
#define CDC_SERIAL_STATE_OVERRUN                (1 << 6)  /* receive data overrun error has occurred */
#define CDC_SERIAL_STATE_OVERRUN_Pos            (     6)
#define CDC_SERIAL_STATE_OVERRUN_Msk            (1 << CDC_SERIAL_STATE_OVERRUN_Pos)
#define CDC_SERIAL_STATE_PARITY                 (1 << 5)  /* parity error has occurred */
#define CDC_SERIAL_STATE_PARITY_Pos             (     5)
#define CDC_SERIAL_STATE_PARITY_Msk             (1 << CDC_SERIAL_STATE_PARITY_Pos)
#define CDC_SERIAL_STATE_FRAMING                (1 << 4)  /* framing error has occurred */
#define CDC_SERIAL_STATE_FRAMING_Pos            (     4)
#define CDC_SERIAL_STATE_FRAMING_Msk            (1 << CDC_SERIAL_STATE_FRAMING_Pos)
#define CDC_SERIAL_STATE_RING                   (1 << 3)  /* state of ring signal detection */
#define CDC_SERIAL_STATE_RING_Pos               (     3)
#define CDC_SERIAL_STATE_RING_Msk               (1 << CDC_SERIAL_STATE_RING_Pos)
#define CDC_SERIAL_STATE_BREAK                  (1 << 2)  /* state of break detection */
#define CDC_SERIAL_STATE_BREAK_Pos              (     2)
#define CDC_SERIAL_STATE_BREAK_Msk              (1 << CDC_SERIAL_STATE_BREAK_Pos)
#define CDC_SERIAL_STATE_TX_CARRIER             (1 << 1)  /* state of transmission carrier */
#define CDC_SERIAL_STATE_TX_CARRIER_Pos         (     1)
#define CDC_SERIAL_STATE_TX_CARRIER_Msk         (1 << CDC_SERIAL_STATE_TX_CARRIER_Pos)
#define CDC_SERIAL_STATE_RX_CARRIER             (1 << 0)  /* state of receiver carrier */
#define CDC_SERIAL_STATE_RX_CARRIER_Pos         (     0)
#define CDC_SERIAL_STATE_RX_CARRIER_Msk         (1 << CDC_SERIAL_STATE_RX_CARRIER_Pos)

/* CDC class-specific request codes */
#define CDC_SEND_ENCAPSULATED_COMMAND           0x00
#define CDC_GET_ENCAPSULATED_RESPONSE           0x01
#define CDC_SET_COMM_FEATURE                    0x02
#define CDC_GET_COMM_FEATURE                    0x03
#define CDC_CLEAR_COMM_FEATURE                  0x04
#define CDC_SET_AUX_LINE_STATE                  0x10
#define CDC_SET_HOOK_STATE                      0x11
#define CDC_PULSE_SETUP                         0x12
#define CDC_SEND_PULSE                          0x13
#define CDC_SET_PULSE_TIME                      0x14
#define CDC_RING_AUX_JACK                       0x15
#define CDC_SET_LINE_CODING                     0x20
#define CDC_GET_LINE_CODING                     0x21
#define CDC_SET_CONTROL_LINE_STATE              0x22
#define CDC_SEND_BREAK                          0x23
#define CDC_SET_RINGER_PARMS                    0x30
#define CDC_GET_RINGER_PARMS                    0x31
#define CDC_SET_OPERATION_PARMS                 0x32
#define CDC_GET_OPERATION_PARMS                 0x33
#define CDC_SET_LINE_PARMS                      0x34
#define CDC_GET_LINE_PARMS                      0x35
#define CDC_DIAL_DIGITS                         0x36
#define CDC_SET_UNIT_PARAMETER                  0x37
#define CDC_GET_UNIT_PARAMETER                  0x38
#define CDC_CLEAR_UNIT_PARAMETER                0x39
#define CDC_GET_PROFILE                         0x3A
#define CDC_SET_ETHERNET_MULTICAST_FILTERS      0x40
#define CDC_SET_ETHERNET_PMP_FILTER             0x41
#define CDC_GET_ETHERNET_PMP_FILTER             0x42
#define CDC_SET_ETHERNET_PACKET_FILTER          0x43
#define CDC_GET_ETHERNET_STATISTIC              0x44
#define CDC_SET_ATM_DATA_FORMAT                 0x50
#define CDC_GET_ATM_DEVICE_STATISTICS           0x51
#define CDC_SET_ATM_DEFAULT_VC                  0x52
#define CDC_GET_ATM_VC_STATISTICS               0x53

/* Parity enumerator */
typedef enum {
    UART_PARITY_NONE    = 0,
    UART_PARITY_ODD     = 1,
    UART_PARITY_EVEN    = 2,
    UART_PARITY_MARK    = 3,
    UART_PARITY_SPACE   = 4
} UART_Parity;

/* Stop Bits enumerator */
typedef enum {
    UART_STOP_BITS_1    = 0,
    UART_STOP_BITS_1_5  = 1,
    UART_STOP_BITS_2    = 2
} UART_StopBits;

/* Data Bits enumerator */
typedef enum {
    UART_DATA_BITS_5    = 5,
    UART_DATA_BITS_6    = 6,
    UART_DATA_BITS_7    = 7,
    UART_DATA_BITS_8    = 8,
    UART_DATA_BITS_16   = 16
} UART_DataBits;

/* Flow control enumerator */
typedef enum {
    UART_FLOW_CONTROL_NONE     = 0,
    UART_FLOW_CONTROL_RTS_CTS  = 1,
    UART_FLOW_CONTROL_XON_XOFF = 2
} UART_FlowControl;

typedef struct __attribute__((packed))
{
    uint32_t dwDTERate;                            /* Data terminal rate in bits per second */
    uint8_t  bCharFormat;                          /* Number of stop bits */
    uint8_t  bParityType;                          /* Parity bit type */
    uint8_t  bDataBits;                            /* Number of data bits */
} USB_CDC_LINE_CODING_T;

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
    uint32_t remoteWakeup:1;
    uint32_t cmdSetLineCoding:1;
    uint32_t configuration:1;
    uint32_t txblock:1;
    uint32_t unused:28;
    uint32_t uEndPointMask;

    USB_CDC_LINE_CODING_T line_coding;
    /*
    D15..D2 RESERVED (Reset to zero) 
    D1 Carrier control for half duplex modems. This signal corresponds to V.24 signal 
        105 and RS-232 signal RTS. 
         0 - Deactivate carrier 
         1 - Activate carrier 
        The device ignores the value of this bit when operating in full duplex mode. 
    D0 Indicates to DCE if DTE is present or not. This signal corresponds to V.24 
        signal 108/2 and RS-232 signal DTR. 
         0 - Not Present 
         1 - Present 

    */
    uint16_t control_line_state;
    /*
    This request sends special carrier modulation that generates an RS-232 style break. 
    The wValue field contains the length of time, in milliseconds, of the break signal. If wValue contains a 
    value of FFFFh, then the device will send a break until another SendBreak request is received with the 
    wValue of 0000h. 
    */
    uint16_t Duration_of_Break;
}BSP_USB_CDC_VAR_s;

extern BSP_USB_CDC_VAR_s USB_CDC_Var;

/**
 * @brief use this API to initialize USB module and MSC function
 *
 * @param[in] null.
 * @param[out] null.
 */
extern void bsp_usb_init(void);
extern void bsp_usb_device_remote_wakeup(void);
extern int bsp_cdc_push_data(uint8_t *data, uint32_t len);

#ifdef FEATURE_DISCONN_DETECT
void bsp_usb_device_disconn_timeout(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
