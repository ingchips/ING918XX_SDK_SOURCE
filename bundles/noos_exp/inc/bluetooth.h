/** @file
*    @brief basic bluetooth macro definition
*
*  Copyright Message
*
*
*  INGCHIPS confidential and proprietary.
*  COPYRIGHT (c) 2018-2023 by INGCHIPS
*
*  All rights are reserved. Reproduction in whole or in part is
*  prohibited without the written consent of the copyright owner.
*
*/

#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include <stdint.h>

#define BD_ADDR_LEN              0x06

/**
 * @brief hci connection handle type
 */
typedef uint16_t hci_con_handle_t;

/**
 * @brief Length of a bluetooth device address.
 */

/**
 * @brief Bluetooth address
 */
typedef uint8_t bd_addr_t[BD_ADDR_LEN];

/**
 * Address types
 * @note: btstack uses a custom addr type to refer to classic ACL and SCO devices
 */
 typedef enum {
    BD_ADDR_TYPE_LE_PUBLIC = 0,
    BD_ADDR_TYPE_LE_RANDOM = 0x01,
    BD_ADDR_TYPE_LE_RESOLVED_PUB = 0x02,
    BD_ADDR_TYPE_LE_RESOLVED_RAN = 0x03
} bd_addr_type_t;

// back-compatibility for typos
#define BD_ADDR_TYPE_LE_RESOVLED_PUB        BD_ADDR_TYPE_LE_RESOLVED_PUB
#define BD_ADDR_TYPE_LE_RESOVLED_RAN        BD_ADDR_TYPE_LE_RESOLVED_RAN

/**
 * @brief link key
 */
#define LINK_KEY_LEN 16
#define LINK_KEY_STR_LEN (LINK_KEY_LEN*2)
typedef uint8_t link_key_t[LINK_KEY_LEN];

/**
 * @brief link key type
 */
typedef enum {
  COMBINATION_KEY = 0,  // standard pairing
  LOCAL_UNIT_KEY,
  REMOTE_UNIT_KEY,
} link_key_type_t;

/**
 * HCI Transport
 */

/**
 * packet types
 */
#define HCI_COMMAND_DATA_PACKET 0x01
#define HCI_ACL_DATA_PACKET     0x02
#define HCI_EVENT_PACKET        0x04
#define L2CAP_EVENT_PACKET      0x0A

// packet header sizes
#define HCI_CMD_HEADER_SIZE          3
#define HCI_ACL_HEADER_SIZE          4
#define HCI_SCO_HEADER_SIZE          3
#define HCI_EVENT_HEADER_SIZE        2

/**
 * HCI Layer
 */

//
// Error Codes
//

// from Bluetooth Core Specification
#define ERROR_CODE_SUCCESS                                 0x00
#define ERROR_CODE_UNKNOWN_HCI_COMMAND                     0x01
#define ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER           0x02
#define ERROR_CODE_HARDWARE_FAILURE                        0x03
#define ERROR_CODE_PAGE_TIMEOUT                            0x04
#define ERROR_CODE_AUTHENTICATION_FAILURE                  0x05
#define ERROR_CODE_PIN_OR_KEY_MISSING                      0x06
#define ERROR_CODE_MEMORY_CAPACITY_EXCEEDED                0x07
#define ERROR_CODE_CONNECTION_TIMEOUT                      0x08
#define ERROR_CODE_CONNECTION_LIMIT_EXCEEDED               0x09
#define ERROR_CODE_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED  0x0A
#define ERROR_CODE_ACL_CONNECTION_ALREADY_EXISTS           0x0B
#define ERROR_CODE_COMMAND_DISALLOWED                      0x0C
#define ERROR_CODE_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES 0x0D
#define ERROR_CODE_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS  0x0E
#define ERROR_CODE_CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR 0x0F
#define ERROR_CODE_CONNECTION_ACCEPT_TIMEOUT_EXCEEDED      0x10
#define ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE  0x11
#define ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS          0x12
#define ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION       0x13
#define ERROR_CODE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES 0x14
#define ERROR_CODE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_POWER_OFF     0x15
#define ERROR_CODE_CONNECTION_TERMINATED_BY_LOCAL_HOST     0x16
#define ERROR_CODE_REPEATED_ATTEMPTS                       0x17
#define ERROR_CODE_PAIRING_NOT_ALLOWED                     0x18
#define ERROR_CODE_UNKNOWN_LMP_PDU                         0x19
#define ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE_UNSUPPORTED_LMP_FEATURE 0x1A
#define ERROR_CODE_SCO_OFFSET_REJECTED                     0x1B
#define ERROR_CODE_SCO_INTERVAL_REJECTED                   0x1C
#define ERROR_CODE_SCO_AIR_MODE_REJECTED                   0x1D
#define ERROR_CODE_INVALID_LMP_PARAMETERS_INVALID_LL_PARAMETERS 0x1E
#define ERROR_CODE_UNSPECIFIED_ERROR                       0x1F
#define ERROR_CODE_UNSUPPORTED_LMP_PARAMETER_VALUE_UNSUPPORTED_LL_PARAMETER_VALUE 0x20
#define ERROR_CODE_ROLE_CHANGE_NOT_ALLOWED                 0x21
#define ERROR_CODE_LMP_RESPONSE_TIMEOUT_LL_RESPONSE_TIMEOUT 0x22
#define ERROR_CODE_LMP_ERROR_TRANSACTION_COLLISION         0x23
#define ERROR_CODE_LMP_PDU_NOT_ALLOWED                     0x24
#define ERROR_CODE_ENCRYPTION_MODE_NOT_ACCEPTABLE          0x25
#define ERROR_CODE_LINK_KEY_CANNOT_BE_CHANGED              0x26
#define ERROR_CODE_REQUESTED_QOS_NOT_SUPPORTED             0x27
#define ERROR_CODE_INSTANT_PASSED                          0x28
#define ERROR_CODE_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED     0x29
#define ERROR_CODE_DIFFERENT_TRANSACTION_COLLISION         0x2A
#define ERROR_CODE_RESERVED                                0x2B
#define ERROR_CODE_QOS_UNACCEPTABLE_PARAMETER              0x2C
#define ERROR_CODE_QOS_REJECTED                            0x2D
#define ERROR_CODE_CHANNEL_CLASSIFICATION_NOT_SUPPORTED    0x2E
#define ERROR_CODE_INSUFFICIENT_SECURITY                   0x2F
#define ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE        0x30
// #define ERROR_CODE_RESERVED
#define ERROR_CODE_ROLE_SWITCH_PENDING                     0x32
// #define ERROR_CODE_RESERVED
#define ERROR_CODE_RESERVED_SLOT_VIOLATION                 0x34
#define ERROR_CODE_ROLE_SWITCH_FAILED                      0x35
#define ERROR_CODE_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE     0x36
#define ERROR_CODE_SECURE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST 0x37
#define ERROR_CODE_HOST_BUSY_PAIRING                       0x38
#define ERROR_CODE_CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNEL_FOUND 0x39
#define ERROR_CODE_CONTROLLER_BUSY                         0x3A
#define ERROR_CODE_UNACCEPTABLE_CONNECTION_PARAMETERS      0x3B
#define ERROR_CODE_DIRECTED_ADVERTISING_TIMEOUT            0x3C
#define ERROR_CODE_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE 0x3D
#define ERROR_CODE_CONNECTION_FAILED_TO_BE_ESTABLISHED     0x3E
#define ERROR_CODE_MAC_CONNECTION_FAILED                   0x3F
#define ERROR_CODE_COARSE_CLOCK_ADJUSTMENT_REJECTED_BUT_WILL_TRY_TO_ADJUST_USING_CLOCK_DRAGGING 0x40
#define ERROR_CODE_UNKNOWN_ADVERTISING_IDENTIFIER                               0x42
#define ERROR_CODE_LIMIT_REACHED                                                0x43
#define ERROR_CODE_OP_CANCELLED_BY_HOST                                         0x44
#define ERROR_CODE_PACKET_TOO_LONG                                              0x45
#define ERROR_CODE_TOO_LATE                                                     0x46
#define ERROR_CODE_TOO_EARLY                                                    0x47

// HCI roles
#define HCI_ROLE_MASTER 0
#define HCI_ROLE_SLAVE  1

// packet sizes (max payload)
#define HCI_ACL_DM1_SIZE            17
#define HCI_ACL_DH1_SIZE            27
#define HCI_ACL_2DH1_SIZE           54
#define HCI_ACL_3DH1_SIZE           83
#define HCI_ACL_DM3_SIZE           121
#define HCI_ACL_DH3_SIZE           183
#define HCI_ACL_DM5_SIZE           224
#define HCI_ACL_DH5_SIZE           339
#define HCI_ACL_2DH3_SIZE          367
#define HCI_ACL_3DH3_SIZE          552
#define HCI_ACL_2DH5_SIZE          679
#define HCI_ACL_3DH5_SIZE         1021

#define HCI_EVENT_PAYLOAD_SIZE     255
#define HCI_CMD_PAYLOAD_SIZE       255

#define LE_ADVERTISING_DATA_SIZE    31

/**
 * Default INQ Mode
 */
#define HCI_INQUIRY_LAP 0x9E8B33L  // 0x9E8B33: General/Unlimited Inquiry Access Code (GIAC)

/**
 * SSP IO Capabilities
 */
#define SSP_IO_CAPABILITY_DISPLAY_ONLY   0
#define SSP_IO_CAPABILITY_DISPLAY_YES_NO 1
#define SSP_IO_CAPABILITY_KEYBOARD_ONLY  2
#define SSP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT 3
#define SSP_IO_CAPABILITY_UNKNOWN 0xff


/**
 * SSP Authentication Requirements, see IO Capability Request Reply Commmand
 */

// Numeric comparison with automatic accept allowed.
#define SSP_IO_AUTHREQ_MITM_PROTECTION_NOT_REQUIRED_NO_BONDING 0x00

// Use IO Capabilities to deter- mine authentication procedure
#define SSP_IO_AUTHREQ_MITM_PROTECTION_REQUIRED_NO_BONDING 0x01

// Numeric compar- ison with automatic accept allowed.
#define SSP_IO_AUTHREQ_MITM_PROTECTION_NOT_REQUIRED_DEDICATED_BONDING 0x02

// Use IO Capabilities to determine authentication procedure
#define SSP_IO_AUTHREQ_MITM_PROTECTION_REQUIRED_DEDICATED_BONDING 0x03

// Numeric Compari- son with automatic accept allowed.
#define SSP_IO_AUTHREQ_MITM_PROTECTION_NOT_REQUIRED_GENERAL_BONDING 0x04

// Use IO capabilities to determine authentication procedure.
#define SSP_IO_AUTHREQ_MITM_PROTECTION_REQUIRED_GENERAL_BONDING 0x05


// OGFs
#define OGF_LINK_CONTROL          0x01
#define OGF_LINK_POLICY           0x02
#define OGF_CONTROLLER_BASEBAND   0x03
#define OGF_INFORMATIONAL_PARAMETERS 0x04
#define OGF_STATUS_PARAMETERS     0x05
#define OGF_TESTING               0x06
#define OGF_LE_CONTROLLER 0x08
#define OGF_VENDOR  0x3f


// Events from host controller to host

/**
 * @format 1
 * @param status
 */
#define HCI_EVENT_INQUIRY_COMPLETE                         0x01
// no format yet, can contain multiple results

/**
 * @format 1B11132
 * @param num_responses
 * @param bd_addr
 * @param page_scan_repetition_mode
 * @param reserved1
 * @param reserved2
 * @param class_of_device
 * @param clock_offset
 */
#define HCI_EVENT_INQUIRY_RESULT                           0x02

/**
 * @format 12B11
 * @param status
 * @param connection_handle
 * @param bd_addr
 * @param link_type
 * @param encryption_enabled
 */
#define HCI_EVENT_CONNECTION_COMPLETE                      0x03
/**
 * @format B31
 * @param bd_addr
 * @param class_of_device
 * @param link_type
 */
#define HCI_EVENT_CONNECTION_REQUEST                       0x04
/**
 * @format 121
 * @param status
 * @param connection_handle
 * @param reason
 */
#define HCI_EVENT_DISCONNECTION_COMPLETE                   0x05
/**
 * @format 12
 * @param status
 * @param connection_handle
 */
#define HCI_EVENT_AUTHENTICATION_COMPLETE_EVENT            0x06
/**
 * @format 1BN
 * @param status
 * @param bd_addr
 * @param remote_name
 */
#define HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE             0x07
/**
 * @format 121
 * @param status
 * @param connection_handle
 * @param encryption_enabled
 */
#define HCI_EVENT_ENCRYPTION_CHANGE                        0x08
/**
 * @format 12
 * @param status
 * @param connection_handle
 */
#define HCI_EVENT_CHANGE_CONNECTION_LINK_KEY_COMPLETE      0x09
/**
 * @format 121
 * @param status
 * @param connection_handle
 * @param key_flag
 */
#define HCI_EVENT_MASTER_LINK_KEY_COMPLETE                 0x0A

#define HCI_EVENT_READ_REMOTE_VERSION_INFORMATION_COMPLETE 0x0C

/**
 * @format 12R
 * @param num_hci_command_packets
 * @param command_opcode
 * @param return_parameters
 */
#define HCI_EVENT_COMMAND_COMPLETE                         0x0E
/**
 * @format 112
 * @param status
 * @param num_hci_command_packets
 * @param command_opcode
 */
#define HCI_EVENT_COMMAND_STATUS                           0x0F

/**
 * @format 121
 * @param hardware_code
 */
#define HCI_EVENT_HARDWARE_ERROR                           0x10

#define HCI_EVENT_FLUSH_OCCURED                            0x11

/**
 * @format 1B1
 * @param status
 * @param bd_addr
 * @param role
 */
#define HCI_EVENT_ROLE_CHANGE                              0x12

// TODO: number_of_handles 1, connection_handle[H*i], hc_num_of_completed_packets[2*i]
#define HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS              0x13

/**
 * @format 1H12
 * @param status
 * @param handle
 * @param mode
 * @param interval
 */
#define HCI_EVENT_MODE_CHANGE_EVENT                        0x14

// TODO: num_keys, bd_addr[B*i], link_key[16 octets * i]
#define HCI_EVENT_RETURN_LINK_KEYS                         0x15

/**
 * @format B
 * @param bd_addr
 */
#define HCI_EVENT_PIN_CODE_REQUEST                         0x16

/**
 * @format B
 * @param bd_addr
 */
#define HCI_EVENT_LINK_KEY_REQUEST                         0x17

// TODO: bd_addr B, link_key 16octets, key_type 1
#define HCI_EVENT_LINK_KEY_NOTIFICATION                    0x18

/**
 * @format 1
 * @param link_type
 */
#define HCI_EVENT_DATA_BUFFER_OVERFLOW                     0x1A

/**
 * @format H1
 * @param handle
 * @param lmp_max_slots
 */
#define HCI_EVENT_MAX_SLOTS_CHANGED                        0x1B

/**
 * @format 1H2
 * @param status
 * @param handle
 * @param clock_offset
 */
#define HCI_EVENT_READ_CLOCK_OFFSET_COMPLETE               0x1C

/**
 * @format 1H2
 * @param status
 * @param handle
 * @param packet_types
 */
#define HCI_EVENT_CONNECTION_PACKET_TYPE_CHANGED           0x1D

/**
 * @format 1B11321
 * @param num_responses
 * @param bd_addr
 * @param page_scan_repetition_mode
 * @param reserved
 * @param class_of_device
 * @param clock_offset
 * @param rssi
 */
#define HCI_EVENT_INQUIRY_RESULT_WITH_RSSI                 0x22

/**
 * @format 1HB111221
 * @param status
 * @param handle
 * @param bd_addr
 * @param link_type
 * @param transmission_interval
 * @param retransmission_interval
 * @param rx_packet_length
 * @param tx_packet_length
 * @param air_mode
 */
#define HCI_EVENT_SYNCHRONOUS_CONNECTION_COMPLETE          0x2C

// TODO: serialize extended_inquiry_response and provide parser
/**
 * @format 1B11321
 * @param num_responses
 * @param bd_addr
 * @param page_scan_repetition_mode
 * @param reserved
 * @param class_of_device
 * @param clock_offset
 * @param rssi
 */
#define HCI_EVENT_EXTENDED_INQUIRY_RESPONSE                0x2F

 /**
  * @format 1H
  * @param status
  * @param handle
  */
#define HCI_EVENT_ENCRYPTION_KEY_REFRESH_COMPLETE          0x30

#define HCI_EVENT_IO_CAPABILITY_REQUEST                    0x31
#define HCI_EVENT_IO_CAPABILITY_RESPONSE                   0x32
#define HCI_EVENT_USER_CONFIRMATION_REQUEST                0x33
#define HCI_EVENT_USER_PASSKEY_REQUEST                     0x34
#define HCI_EVENT_REMOTE_OOB_DATA_REQUEST                  0x35
#define HCI_EVENT_SIMPLE_PAIRING_COMPLETE                  0x36

#define HCI_EVENT_LE_META                                  0x3E
/*add the event of BLE dengyiyun*/
#define HCI_EVENT_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED    0x57

#define HCI_EVENT_VENDOR_SPECIFIC                          0xFF


// add by dengyiyun for event originated in HCI to L2CAP

#define HCI_EVENT_L2CAP_CONN_PARA_UPDATE_REQUEST   0xFB

/**
 * @format 11H11B2221
 * @param subevent_code
 * @param status
 * @param connection_handle
 * @param role
 * @param peer_address_type
 * @param peer_address
 * @param conn_interval
 * @param conn_latency
 * @param supervision_timeout
 * @param master_clock_accuracy
 */
#define HCI_SUBEVENT_LE_CONNECTION_COMPLETE                0x01
#define HCI_SUBEVENT_LE_ADVERTISING_REPORT                 0x02
#define HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE         0x03
#define HCI_SUBEVENT_LE_READ_REMOTE_USED_FEATURES_COMPLETE 0x04
#define HCI_SUBEVENT_LE_LONG_TERM_KEY_REQUEST              0x05
#define HCI_SUBEVENT_LE_REMOTE_CONNECTION_PARAMETER_REQUEST 0x06
#define HCI_SUBEVENT_LE_DATA_LENGTH_CHANGE_EVENT           0x07
#define HCI_SUBEVENT_LE_P256_PUB_KEY_COMPLETE              0x08
#define HCI_SUBEVENT_LE_GENERATE_DHKEY_COMPLETE            0x09
#define HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE       0x0A
#define HCI_SUBEVENT_LE_DIRECT_ADVERTISING_REPORT          0x0B
#define HCI_SUBEVENT_LE_PHY_UPDATE_COMPLETE                0X0C
#define HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT        0X0D
#define HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED 0X0E
#define HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_REPORT        0X0F
#define HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_LOST     0X10
#define HCI_SUBEVENT_LE_SCAN_TIMEOUT                       0X11
#define HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED         0X12
#define HCI_SUBEVENT_LE_SCAN_REQUEST_RECEIVED              0X13
#define HCI_SUBEVENT_LE_CHANNEL_SELECTION_ALGORITHM        0X14
#define HCI_SUBEVENT_LE_CONNECTIONLESS_IQ_REPORT           0x15
#define HCI_SUBEVENT_LE_CONNECTION_IQ_REPORT               0x16
#define HCI_SUBEVENT_LE_CTE_REQ_FAILED                     0x17
#define HCI_SUBEVENT_LE_PRD_ADV_SYNC_TRANSFER_RCVD         0x18
#define HCI_SUBEVENT_LE_CIS_ESTABLISHED                    0x19
#define HCI_SUBEVENT_LE_CIS_REQUEST                        0x1a
#define HCI_SUBEVENT_LE_CREATE_BIG_COMPLETE                0x1b
#define HCI_SUBEVENT_LE_TERMINATE_BIG_COMPLETE             0x1c
#define HCI_SUBEVENT_LE_BIG_SYNC_ESTABLISHED               0x1d
#define HCI_SUBEVENT_LE_BIG_SYNC_LOST                      0x1e
#define HCI_SUBEVENT_LE_REQUEST_PEER_SCA                   0x1F
#define HCI_SUBEVENT_LE_PATH_LOSS_THRESHOLD                0x20
#define HCI_SUBEVENT_LE_TRANSMIT_POWER_REPORTING           0x21
#define HCI_SUBEVENT_LE_BIGINFO_ADV_REPORT                 0x22
#define HCI_SUBEVENT_LE_SUBRATE_CHANGE                     0x23
#define HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_V2                0x24
#define HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_REPORT_V2                          0x25
#define HCI_SUBEVENT_LE_PRD_ADV_SYNC_TRANSFER_RCVD_V2                           0x26
#define HCI_SUBEVENT_PRD_ADV_SUBEVT_DATA_REQ                                    0x27
#define HCI_SUBEVENT_PRD_ADV_RSP_REPORT                                         0x28
#define HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V2                         0x29

// for backward compatibility
#define HCI_SUBEVENT_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_COMPLETE HCI_SUBEVENT_LE_REMOTE_CONNECTION_PARAMETER_REQUEST

// Vendor specific subevents
#define HCI_SUBEVENT_LE_VENDOR_CONNECTION_ABORTED             0xFD
#define HCI_SUBEVENT_LE_VENDOR_CHANNEL_MAP_UPDATE             0xFE
#define HCI_SUBEVENT_LE_VENDOR_PRO_CONNECTIONLESS_IQ_REPORT   0xFF

// last used HCI_EVENT in 2.1 is 0x3d
// last used HCI_EVENT in 4.1 is 0x57

/**
 * L2CAP Layer
 */

#define L2CAP_HEADER_SIZE 4

#define L2CAP_SIG_ID_INVALID 0

// size of HCI ACL + L2CAP Header for regular data packets (8)
#define COMPLETE_L2CAP_HEADER (HCI_ACL_HEADER_SIZE + L2CAP_HEADER_SIZE)

// minimum signaling MTU
#define L2CAP_MINIMAL_MTU 48
#define L2CAP_DEFAULT_MTU 672

// Minimum/default MTU
#define L2CAP_LE_DEFAULT_MTU  23

// L2CAP Fixed Channel IDs

#define L2CAP_CID_ATTRIBUTE_PROTOCOL        0x0004
#define L2CAP_CID_SIGNALING_LE              0x0005
#define L2CAP_CID_SECURITY_MANAGER_PROTOCOL 0x0006

// L2CAP Configuration Result Codes
#define L2CAP_CONF_RESULT_UNKNOWN_OPTIONS   0x0003

// L2CAP Reject Result Codes
#define L2CAP_REJ_CMD_UNKNOWN               0x0000

// Response Timeout eXpired
#define L2CAP_RTX_TIMEOUT_MS   10000

// Extended Response Timeout eXpired
#define L2CAP_ERTX_TIMEOUT_MS 120000

/**
 * ATT
 */

// Minimum/default MTU
#define ATT_DEFAULT_MTU               23

// MARK: Attribute PDU Opcodes
#define ATT_ERROR_RESPONSE              0x01

#define ATT_EXCHANGE_MTU_REQUEST        0x02
#define ATT_EXCHANGE_MTU_RESPONSE       0x03

#define ATT_FIND_INFORMATION_REQUEST    0x04
#define ATT_FIND_INFORMATION_REPLY      0x05
#define ATT_FIND_BY_TYPE_VALUE_REQUEST  0x06
#define ATT_FIND_BY_TYPE_VALUE_RESPONSE 0x07

#define ATT_READ_BY_TYPE_REQUEST        0x08
#define ATT_READ_BY_TYPE_RESPONSE       0x09
#define ATT_READ_REQUEST                0x0a
#define ATT_READ_RESPONSE               0x0b
#define ATT_READ_BLOB_REQUEST           0x0c
#define ATT_READ_BLOB_RESPONSE          0x0d
#define ATT_READ_MULTIPLE_REQUEST       0x0e
#define ATT_READ_MULTIPLE_RESPONSE      0x0f
#define ATT_READ_BY_GROUP_TYPE_REQUEST  0x10
#define ATT_READ_BY_GROUP_TYPE_RESPONSE 0x11

#define ATT_WRITE_REQUEST               0x12
#define ATT_WRITE_RESPONSE              0x13

#define ATT_PREPARE_WRITE_REQUEST       0x16
#define ATT_PREPARE_WRITE_RESPONSE      0x17
#define ATT_EXECUTE_WRITE_REQUEST       0x18
#define ATT_EXECUTE_WRITE_RESPONSE      0x19

#define ATT_HANDLE_VALUE_NOTIFICATION   0x1b
#define ATT_HANDLE_VALUE_INDICATION     0x1d
#define ATT_HANDLE_VALUE_CONFIRMATION   0x1e


#define ATT_WRITE_COMMAND                0x52
#define ATT_SIGNED_WRITE_COMMAND         0xD2

// MARK: ATT Error Codes
#define ATT_ERROR_INVALID_HANDLE                   0x01
#define ATT_ERROR_READ_NOT_PERMITTED               0x02
#define ATT_ERROR_WRITE_NOT_PERMITTED              0x03
#define ATT_ERROR_INVALID_PDU                      0x04
#define ATT_ERROR_INSUFFICIENT_AUTHENTICATION      0x05
#define ATT_ERROR_REQUEST_NOT_SUPPORTED            0x06
#define ATT_ERROR_INVALID_OFFSET                   0x07
#define ATT_ERROR_INSUFFICIENT_AUTHORIZATION       0x08
#define ATT_ERROR_PREPARE_QUEUE_FULL               0x09
#define ATT_ERROR_ATTRIBUTE_NOT_FOUND              0x0a
#define ATT_ERROR_ATTRIBUTE_NOT_LONG               0x0b
#define ATT_ERROR_INSUFFICIENT_ENCRYPTION_KEY_SIZE 0x0c
#define ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH   0x0d
#define ATT_ERROR_UNLIKELY_ERROR                   0x0e
#define ATT_ERROR_INSUFFICIENT_ENCRYPTION          0x0f
#define ATT_ERROR_UNSUPPORTED_GROUP_TYPE           0x10
#define ATT_ERROR_INSUFFICIENT_RESOURCES           0x11
#define ATT_ERROR_DATABASE_OUT_OF_SYNC             0x12
#define ATT_ERROR_VALUE_NOT_ALLOWED                0x13
// MARK: Attribute Property Flags
#define ATT_PROPERTY_BROADCAST           0x01
#define ATT_PROPERTY_READ                0x02
#define ATT_PROPERTY_WRITE_WITHOUT_RESPONSE 0x04
#define ATT_PROPERTY_WRITE               0x08
#define ATT_PROPERTY_NOTIFY              0x10
#define ATT_PROPERTY_INDICATE            0x20
#define ATT_PROPERTY_AUTHENTICATED_SIGNED_WRITE 0x40
#define ATT_PROPERTY_EXTENDED_PROPERTIES 0x80

// MARK: Attribute Property Flag, btstack extension
// value is asked from client
#define ATT_PROPERTY_DYNAMIC             0x100
// 128 bit UUID used
#define ATT_PROPERTY_UUID128             0x200
// Authentication required
#define ATT_PROPERTY_AUTHENTICATION_REQUIRED 0x400
// Authorization from user required
#define ATT_PROPERTY_AUTHORIZATION_REQUIRED  0x800
// Encryption key size stored in upper 4 bits, 0 == no encryption, encryption key size - 1 otherwise

// ATT Transaction Timeout of 30 seconds for Command/Response or Incidationc/Confirmation
#define ATT_TRANSACTION_TIMEOUT_MS     30000

#define ATT_TRANSACTION_MODE_NONE      0x0
#define ATT_TRANSACTION_MODE_ACTIVE    0x1
#define ATT_TRANSACTION_MODE_EXECUTE   0x2
#define ATT_TRANSACTION_MODE_CANCEL    0x3

// MARK: GATT UUIDs
#define GATT_PRIMARY_SERVICE_UUID                   0x2800
#define GATT_SECONDARY_SERVICE_UUID                 0x2801
#define GATT_INCLUDE_SERVICE_UUID                   0x2802
#define GATT_CHARACTERISTICS_UUID                   0x2803
#define GATT_CHARACTERISTIC_EXTENDED_PROPERTIES     0x2900
#define GATT_CHARACTERISTIC_USER_DESCRIPTION        0x2901
#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION   0x2902
#define GATT_SERVER_CHARACTERISTICS_CONFIGURATION   0x2903
#define GATT_CHARACTERISTIC_PRESENTATION_FORMAT     0x2904
#define GATT_CHARACTERISTIC_AGGREGATE_FORMAT        0x2905

#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NONE          0
#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION  1
#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION    2

// GAP Service and Characteristics
#define GAP_SERVICE_UUID               0x1800
#define GAP_DEVICE_NAME_UUID           0x2a00
#define GAP_APPEARANCE_UUID            0x2a01
#define GAP_PERIPHERAL_PRIVACY_FLAG    0x2a02
#define GAP_RECONNECTION_ADDRESS_UUID  0x2a03
#define GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_UUID 0x2a04
#define GAP_SERVICE_CHANGED            0x2a05

/**
 * SM - LE Security Manager
 */
// Bluetooth Spec definitions
typedef enum {
    SM_CODE_PAIRING_REQUEST = 0X01,
    SM_CODE_PAIRING_RESPONSE,
    SM_CODE_PAIRING_CONFIRM,
    SM_CODE_PAIRING_RANDOM,
    SM_CODE_PAIRING_FAILED,
    SM_CODE_ENCRYPTION_INFORMATION,
    SM_CODE_MASTER_IDENTIFICATION,
    SM_CODE_IDENTITY_INFORMATION,
    SM_CODE_IDENTITY_ADDRESS_INFORMATION,
    SM_CODE_SIGNING_INFORMATION,
    SM_CODE_SECURITY_REQUEST,
    SM_CODE_PAIRING_PUBLIC_KEY,
    SM_CODE_PAIRING_DHKEY_CHECK,
    SM_CODE_PAIRING_KEYPRESS_NOTIFICATION
} SECURITY_MANAGER_COMMANDS;

// IO Capability Values
typedef enum {
    IO_CAPABILITY_UNINITIALIZED = -1,
    IO_CAPABILITY_DISPLAY_ONLY = 0,
    IO_CAPABILITY_DISPLAY_YES_NO,
    IO_CAPABILITY_KEYBOARD_ONLY,
    IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
    IO_CAPABILITY_KEYBOARD_DISPLAY, // not used by secure simple pairing
} io_capability_t;

// Authentication requirement flags
#define SM_AUTHREQ_NO_BONDING 0x00
#define SM_AUTHREQ_BONDING 0x01
#define SM_AUTHREQ_MITM_PROTECTION 0x04
#define SM_AUTHREQ_SC 0x08                  // support of LE Secure Connections pairing

// Key distribution flags used by spec
#define SM_KEYDIST_ENC_KEY 0X01
#define SM_KEYDIST_ID_KEY  0x02
#define SM_KEYDIST_SIGN    0x04

// Key distribution flags used internally
#define SM_KEYDIST_FLAG_ENCRYPTION_INFORMATION       0x01
#define SM_KEYDIST_FLAG_MASTER_IDENTIFICATION        0x02
#define SM_KEYDIST_FLAG_IDENTITY_INFORMATION         0x04
#define SM_KEYDIST_FLAG_IDENTITY_ADDRESS_INFORMATION 0x08
#define SM_KEYDIST_FLAG_SIGNING_IDENTIFICATION       0x10

// STK Generation Methods
#define SM_STK_GENERATION_METHOD_JUST_WORKS 0x01
#define SM_STK_GENERATION_METHOD_OOB        0x02
#define SM_STK_GENERATION_METHOD_PASSKEY    0x04
#define SM_STK_GENERATION_METHOD_NUM_COMPARISON    0x08

// Pairing Failed Reasons
#define SM_REASON_RESERVED                     0x00
#define SM_REASON_PASSKEYT_ENTRY_FAILED        0x01
#define SM_REASON_OOB_NOT_AVAILABLE            0x02
#define SM_REASON_AUTHENTHICATION_REQUIREMENTS 0x03
#define SM_REASON_CONFIRM_VALUE_FAILED         0x04
#define SM_REASON_PAIRING_NOT_SUPPORTED        0x05
#define SM_REASON_ENCRYPTION_KEY_SIZE          0x06
#define SM_REASON_COMMAND_NOT_SUPPORTED        0x07
#define SM_REASON_UNSPECIFIED_REASON           0x08
#define SM_REASON_REPEATED_ATTEMPTS            0x09
#define SM_REASON_REPEATED_ATTEMPTS            0x09
#define SM_REASON_INVALID_PARAMETERS           0x0a
#define SM_REASON_DHKEY_CHECK_FAILED           0x0b
#define SM_REASON_NUMERIC_COMPARISON_FAILED    0x0c
#define SM_REASON_KEY_REJECTED_FAILED          0x0f
// also, invalid parameters
// and reserved

// Company identifiers / manufacturers
#define COMPANY_ID_INGCHIPS                    0x06AC

/** Generic UUID type, to be used only as a pointer */
typedef struct {
    /** Type of the UUID */
    uint8_t type;
} ble_uuid_t;

/** 16-bit UUID */
typedef struct {
    ble_uuid_t u;
    uint16_t value;
} ble_uuid16_t;

/** 32-bit UUID */
typedef struct {
    ble_uuid_t u;
    uint32_t value;
} ble_uuid32_t;

/** 128-bit UUID */
typedef struct {
    ble_uuid_t u;
    uint8_t value[16];
} ble_uuid128_t;

/** Universal UUID type, to be used for any-UUID static allocation */
typedef union {
    ble_uuid_t u;
    ble_uuid16_t u16;
    ble_uuid32_t u32;
    ble_uuid128_t u128;
} ble_uuid_any_t;

#endif
