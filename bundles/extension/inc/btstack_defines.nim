##  ----------------------------------------------------------------------------
##  Copyright Message
##  ----------------------------------------------------------------------------
##
##  INGCHIPS confidential and proprietary.
##  COPYRIGHT (c) 2018 by INGCHIPS
##
##  All rights are reserved. Reproduction in whole or in part is
##  prohibited without the written consent of the copyright owner.
##
##  ----------------------------------------------------------------------------

##  DEFINES

const
  DAEMON_EVENT_PACKET* = 0x00000005

##  L2CAP data

const
  L2CAP_DATA_PACKET* = 0x00000006

##  RFCOMM data

const
  RFCOMM_DATA_PACKET* = 0x00000007

##  Attribute protocol data

const
  ATT_DATA_PACKET* = 0x00000008

##  Security Manager protocol data

const
  SM_DATA_PACKET* = 0x00000009

##  SDP query result
##  format: type (8), record_id (16), attribute_id (16), attribute_length (16), attribute_value (max 1k)

const
  SDP_CLIENT_PACKET* = 0x0000000A

##  BNEP data

const
  BNEP_DATA_PACKET* = 0x0000000B

##  Unicast Connectionless Data

const
  UCD_DATA_PACKET* = 0x0000000C

##  debug log messages

const
  LOG_MESSAGE_PACKET* = 0x000000FC

##  ERRORS
##  last error code in 2.1 is 0x38 - we start with 0x50 for BTstack errors

const
  BTSTACK_CONNECTION_TO_BTDAEMON_FAILED* = 0x00000050
  BTSTACK_ACTIVATION_FAILED_SYSTEM_BLUETOOTH* = 0x00000051
  BTSTACK_ACTIVATION_POWERON_FAILED* = 0x00000052
  BTSTACK_ACTIVATION_FAILED_UNKNOWN* = 0x00000053
  BTSTACK_NOT_ACTIVATED* = 0x00000054
  BTSTACK_BUSY* = 0x00000055
  BTSTACK_MEMORY_ALLOC_FAILED* = 0x00000056
  BTSTACK_ACL_BUFFERS_FULL* = 0x00000057
  BTSTACK_LE_CHANNEL_DISCONNECTED* = 0x00000058
  BTSTACK_LE_CHANNEL_NOT_EXIST* = 0x00000059

##  l2cap errors - enumeration by the command that created them

const
  L2CAP_COMMAND_REJECT_REASON_COMMAND_NOT_UNDERSTOOD* = 0x00000060
  L2CAP_COMMAND_REJECT_REASON_SIGNALING_MTU_EXCEEDED* = 0x00000061
  L2CAP_COMMAND_REJECT_REASON_INVALID_CID_IN_REQUEST* = 0x00000062
  L2CAP_CONNECTION_RESPONSE_RESULT_SUCCESSFUL* = 0x00000063
  L2CAP_CONNECTION_RESPONSE_RESULT_PENDING* = 0x00000064
  L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_PSM* = 0x00000065
  L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_SECURITY* = 0x00000066
  L2CAP_CONNECTION_RESPONSE_RESULT_REFUSED_RESOURCES* = 0x00000067
  L2CAP_CONNECTION_RESPONSE_RESULT_RTX_TIMEOUT* = 0x00000068
  L2CAP_SERVICE_ALREADY_REGISTERED* = 0x00000069
  L2CAP_DATA_LEN_EXCEEDS_REMOTE_MTU* = 0x0000006A
  RFCOMM_MULTIPLEXER_STOPPED* = 0x00000070
  RFCOMM_CHANNEL_ALREADY_REGISTERED* = 0x00000071
  RFCOMM_NO_OUTGOING_CREDITS* = 0x00000072
  RFCOMM_AGGREGATE_FLOW_OFF* = 0x00000073
  RFCOMM_DATA_LEN_EXCEEDS_MTU* = 0x00000074
  SDP_HANDLE_ALREADY_REGISTERED* = 0x00000080
  SDP_QUERY_INCOMPLETE* = 0x00000081
  SDP_SERVICE_NOT_FOUND* = 0x00000082
  SDP_HANDLE_INVALID* = 0x00000083
  SDP_QUERY_BUSY* = 0x00000084
  ATT_HANDLE_VALUE_INDICATION_IN_PORGRESS* = 0x00000090
  ATT_HANDLE_VALUE_INDICATION_TIMEOUT* = 0x00000091
  GATT_CLIENT_NOT_CONNECTED* = 0x00000093
  GATT_CLIENT_BUSY* = 0x00000094
  GATT_CLIENT_IN_WRONG_STATE* = 0x00000095
  GATT_CLIENT_DIFFERENT_CONTEXT_FOR_ADDRESS_ALREADY_EXISTS* = 0x00000096
  GATT_CLIENT_VALUE_TOO_LONG* = 0x00000097
  GATT_CLIENT_CHARACTERISTIC_NOTIFICATION_NOT_SUPPORTED* = 0x00000098
  GATT_CLIENT_CHARACTERISTIC_INDICATION_NOT_SUPPORTED* = 0x00000099
  BNEP_SERVICE_ALREADY_REGISTERED* = 0x000000A0
  BNEP_CHANNEL_NOT_CONNECTED* = 0x000000A1
  BNEP_DATA_LEN_EXCEEDS_MTU* = 0x000000A2

##  DAEMON COMMANDS

const
  OGF_BTSTACK* = 0x0000003D

##  cmds for BTstack
##  get state: @returns HCI_STATE

const
  BTSTACK_GET_STATE* = 0x00000001

##  set power mode: param HCI_POWER_MODE

const
  BTSTACK_SET_POWER_MODE* = 0x00000002

##  set capture mode: param on

const
  BTSTACK_SET_ACL_CAPTURE_MODE* = 0x00000003

##  get BTstack version

const
  BTSTACK_GET_VERSION* = 0x00000004

##  get system Bluetooth state

const
  BTSTACK_GET_SYSTEM_BLUETOOTH_ENABLED* = 0x00000005

##  set system Bluetooth state

const
  BTSTACK_SET_SYSTEM_BLUETOOTH_ENABLED* = 0x00000006

##  enable inquiry scan for this client

const
  BTSTACK_SET_DISCOVERABLE* = 0x00000007

##  set global Bluetooth state

const
  BTSTACK_SET_BLUETOOTH_ENABLED* = 0x00000008

##  create l2cap channel: param bd_addr(48), psm (16)

const
  L2CAP_CREATE_CHANNEL* = 0x00000020

##  disconnect l2cap disconnect, param channel(16), reason(8)

const
  L2CAP_DISCONNECT* = 0x00000021

##  register l2cap service: param psm(16), mtu (16)

const
  L2CAP_REGISTER_SERVICE* = 0x00000022

##  unregister l2cap disconnect, param psm(16)

const
  L2CAP_UNREGISTER_SERVICE* = 0x00000023

##  accept connection param bd_addr(48), dest cid (16)

const
  L2CAP_ACCEPT_CONNECTION* = 0x00000024

##  decline l2cap disconnect,param bd_addr(48), dest cid (16), reason(8)

const
  L2CAP_DECLINE_CONNECTION* = 0x00000025

##  create l2cap channel: param bd_addr(48), psm (16), mtu (16)

const
  L2CAP_CREATE_CHANNEL_MTU* = 0x00000026

##  register SDP Service Record: service record (size)

const
  SDP_REGISTER_SERVICE_RECORD* = 0x00000030

##  unregister SDP Service Record

const
  SDP_UNREGISTER_SERVICE_RECORD* = 0x00000031

##  Get remote RFCOMM services

const
  SDP_CLIENT_QUERY_RFCOMM_SERVICES* = 0x00000032

##  Get remote SDP services

const
  SDP_CLIENT_QUERY_SERVICES* = 0x00000033

##  RFCOMM "HCI" Commands

const
  RFCOMM_CREATE_CHANNEL* = 0x00000040
  RFCOMM_DISCONNECT* = 0x00000041
  RFCOMM_REGISTER_SERVICE* = 0x00000042
  RFCOMM_UNREGISTER_SERVICE* = 0x00000043
  RFCOMM_ACCEPT_CONNECTION* = 0x00000044
  RFCOMM_DECLINE_CONNECTION* = 0x00000045
  RFCOMM_PERSISTENT_CHANNEL* = 0x00000046
  RFCOMM_CREATE_CHANNEL_WITH_CREDITS* = 0x00000047
  RFCOMM_REGISTER_SERVICE_WITH_CREDITS* = 0x00000048
  RFCOMM_GRANT_CREDITS* = 0x00000049

##  GAP Classic 0x50

const
  GAP_DISCONNECT* = 0x00000050

##  GAP LE      0x60

const
  GAP_LE_SCAN_START* = 0x00000060
  GAP_LE_SCAN_STOP* = 0x00000061
  GAP_LE_CONNECT* = 0x00000062
  GAP_LE_CONNECT_CANCEL* = 0x00000063
  GAP_LE_SET_SCAN_PARAMETERS* = 0x00000064

##  GATT (Client) 0x70

const
  GATT_DISCOVER_ALL_PRIMARY_SERVICES* = 0x00000070
  GATT_DISCOVER_PRIMARY_SERVICES_BY_UUID16* = 0x00000071
  GATT_DISCOVER_PRIMARY_SERVICES_BY_UUID128* = 0x00000072
  GATT_FIND_INCLUDED_SERVICES_FOR_SERVICE* = 0x00000073
  GATT_DISCOVER_CHARACTERISTICS_FOR_SERVICE* = 0x00000074
  GATT_DISCOVER_CHARACTERISTICS_FOR_SERVICE_BY_UUID128* = 0x00000075
  GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS* = 0x00000076
  GATT_READ_VALUE_OF_CHARACTERISTIC* = 0x00000077
  GATT_READ_LONG_VALUE_OF_CHARACTERISTIC* = 0x00000078
  GATT_WRITE_VALUE_OF_CHARACTERISTIC_WITHOUT_RESPONSE* = 0x00000079
  GATT_WRITE_VALUE_OF_CHARACTERISTIC* = 0x0000007A
  GATT_WRITE_LONG_VALUE_OF_CHARACTERISTIC* = 0x0000007B
  GATT_RELIABLE_WRITE_LONG_VALUE_OF_CHARACTERISTIC* = 0x0000007C
  GATT_READ_CHARACTERISTIC_DESCRIPTOR* = 0x0000007D
  GATT_READ_LONG_CHARACTERISTIC_DESCRIPTOR* = 0x0000007E
  GATT_WRITE_CHARACTERISTIC_DESCRIPTOR* = 0x0000007F
  GATT_WRITE_LONG_CHARACTERISTIC_DESCRIPTOR* = 0x00000080
  GATT_WRITE_CLIENT_CHARACTERISTIC_CONFIGURATION* = 0x00000081
  GATT_GET_MTU* = 0x00000082

##  hci con handles (12 bit): 0x0000..0x0fff

const
  HCI_CON_HANDLE_INVALID* = 0x0000FFFF

##  EVENTS
##  data: event(8), len(8), nr hci connections

const
  BTSTACK_EVENT_NR_CONNECTIONS_CHANGED* = 0x00000061

## *
##  @format
##

const
  BTSTACK_EVENT_POWERON_FAILED* = 0x00000062

## *
##  @format 1
##  @param discoverable
##

const
  BTSTACK_EVENT_DISCOVERABLE_ENABLED* = 0x00000066

##  Daemon Events
## *
##  @format 112
##  @param major
##  @param minor
##  @ @param revision
##

const
  DAEMON_EVENT_VERSION* = 0x00000063

##  data: system bluetooth on/off (bool)
## *
##  @format 1
##  param system_bluetooth_enabled
##

const
  DAEMON_EVENT_SYSTEM_BLUETOOTH_ENABLED* = 0x00000064

##  data: event (8), len(8), status (8) == 0, address (48), name (1984 bits = 248 bytes)
##
##  @format 1BT
##  @param status == 0 to match read_remote_name_request
##  @param address
##  @param name
##

const
  DAEMON_EVENT_REMOTE_NAME_CACHED* = 0x00000065

##  internal - data: event(8)

const
  DAEMON_EVENT_CONNECTION_OPENED* = 0x00000067

##  internal - data: event(8)

const
  DAEMON_EVENT_CONNECTION_CLOSED* = 0x00000068

##  data: event(8), len(8), local_cid(16), credits(8)

const
  DAEMON_EVENT_L2CAP_CREDITS* = 0x00000074

## *
##  @format 12
##  @param status
##  @param psm
##

const
  DAEMON_EVENT_L2CAP_SERVICE_REGISTERED* = 0x00000075

## *
##  @format 21
##  @param rfcomm_cid
##  @param credits
##

const
  DAEMON_EVENT_RFCOMM_CREDITS* = 0x00000084

## *
##  @format 11
##  @param status
##  @param channel_id
##

const
  DAEMON_EVENT_RFCOMM_SERVICE_REGISTERED* = 0x00000085

## *
##  @format 11
##  @param status
##  @param server_channel_id
##

const
  DAEMON_EVENT_RFCOMM_PERSISTENT_CHANNEL* = 0x00000086

## *
##  @format 14
##  @param status
##  @param service_record_handle
##

const
  DAEMON_EVENT_SDP_SERVICE_REGISTERED* = 0x00000090

##  additional HCI events
## *
##  @brief Outgoing packet
##

const
  HCI_EVENT_TRANSPORT_PACKET_SENT* = 0x0000006E

## *
##  @format B
##  @param handle
##

const
  HCI_EVENT_SCO_CAN_SEND_NOW* = 0x0000006F

##  L2CAP EVENTS
## *
##  @format 1BH222222
##  @param status
##  @param address
##  @param handle
##  @param psm
##  @param local_cid
##  @param remote_cid
##  @param local_mtu
##  @param remote_mtu
##  @param flush_timeout
##

const
  L2CAP_EVENT_CHANNEL_OPENED* = 0x00000070

##
##  @format 2
##  @param local_cid
##

const
  L2CAP_EVENT_CHANNEL_CLOSED* = 0x00000071

## *
##  @format BH222
##  @param address
##  @param handle
##  @param psm
##  @param local_cid
##  @param remote_cid
##

const
  L2CAP_EVENT_INCOMING_CONNECTION* = 0x00000072

##  ??
##  data: event(8), len(8), handle(16)

const
  L2CAP_EVENT_TIMEOUT_CHECK* = 0x00000073

## *
##  @format H2222
##  @param handle
##  @param interval_min
##  @param interval_max
##  @param latencey
##  @param timeout_multiplier
##

const
  L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST* = 0x00000076

##  data: event(8), len(8), handle(16), result (16) (0 == ok, 1 == fail)
## *
##  @format H2
##  @param handle
##  @result
##

const
  L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE* = 0x00000077

## *
##  @format 2
##  @param local_cid
##

const
  L2CAP_EVENT_CAN_SEND_NOW* = 0x00000078

##  RFCOMM EVENTS
## *
##  @format 1B2122
##  @param status
##  @param bd_addr
##  @param con_handle
##  @param server_channel
##  @param rfcomm_cid
##  @param max_frame_size
##

const
  RFCOMM_EVENT_CHANNEL_OPENED* = 0x00000080

## *
##  @format 2
##  @param rfcomm_cid
##

const
  RFCOMM_EVENT_CHANNEL_CLOSED* = 0x00000081

## *
##  @format B12
##  @param bd_addr
##  @param server_channel
##  @param rfcomm_cid
##

const
  RFCOMM_EVENT_INCOMING_CONNECTION* = 0x00000082

## *
##  @format 21
##  @param rfcomm_cid
##  @param line_status
##

const
  RFCOMM_EVENT_REMOTE_LINE_STATUS* = 0x00000083

## *
##  @format 21
##  @param rfcomm_cid
##  @param modem_status
##

const
  RFCOMM_EVENT_REMOTE_MODEM_STATUS* = 0x00000087

## *
##  TODO: format for variable data 2?
##  param rfcomm_cid
##  param rpn_data
##

const
  RFCOMM_EVENT_PORT_CONFIGURATION* = 0x00000088

## *
##  @format 2
##  @param rfcomm_cid
##

const
  RFCOMM_EVENT_CAN_SEND_NOW* = 0x00000089

## *
##  @format 1
##  @param status
##

const
  SDP_EVENT_QUERY_COMPLETE* = 0x00000091

## *
##  @format 1T
##  @param rfcomm_channel
##  @param name
##

const
  SDP_EVENT_QUERY_RFCOMM_SERVICE* = 0x00000092

## *
##  @format 22221
##  @param record_id
##  @param attribute_id
##  @param attribute_length
##  @param data_offset
##  @param data
##

const
  SDP_EVENT_QUERY_ATTRIBUTE_BYTE* = 0x00000093

## *
##  @format 22LV
##  @param record_id
##  @param attribute_id
##  @param attribute_length
##  @param attribute_value
##

const
  SDP_EVENT_QUERY_ATTRIBUTE_VALUE* = 0x00000094

## *
##  @format 224
##  @param total_count
##  @param record_index
##  @param record_handle
##  @note Not provided by daemon, only used for internal testing
##

const
  SDP_EVENT_QUERY_SERVICE_RECORD_HANDLE* = 0x00000095

## *
##  @format H1
##  @param handle
##  @param status
##

const
  GATT_EVENT_QUERY_COMPLETE* = 0x000000A0

## *
##  @format HX
##  @param handle
##  @param service
##

const
  GATT_EVENT_SERVICE_QUERY_RESULT* = 0x000000A1

## *
##  @format HY
##  @param handle
##  @param characteristic
##

const
  GATT_EVENT_CHARACTERISTIC_QUERY_RESULT* = 0x000000A2

## *
##  @format H2X
##  @param handle
##  @param include_handle
##  @param service
##

const
  GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT* = 0x000000A3

## *
##  @format HZ
##  @param handle
##  @param characteristic_descriptor
##

const
  GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT* = 0x000000A4

## *
##  @format H2LV
##  @param handle
##  @param value_handle
##  @param value_length
##  @param value
##

const
  GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT* = 0x000000A5

## *
##  @format H22LV
##  @param handle
##  @param value_handle
##  @param value_offset
##  @param value_length
##  @param value
##

const
  GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT* = 0x000000A6

## *
##  @format H2LV
##  @param handle
##  @param value_handle
##  @param value_length
##  @param value
##

const
  GATT_EVENT_NOTIFICATION* = 0x000000A7

## *
##  @format H2LV
##  @param handle
##  @param value_handle
##  @param value_length
##  @param value
##

const
  GATT_EVENT_INDICATION* = 0x000000A8

## *
##  @format H2LV
##  @param descriptor_handle
##  @param descriptor_length
##  @param descriptor
##

const
  GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT* = 0x000000A9

## *
##  @format H2LV
##  @param handle
##  @param descriptor_offset
##  @param descriptor_length
##  @param descriptor
##

const
  GATT_EVENT_LONG_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT* = 0x000000AA

## *
##  @format H2
##  @param handle
##  @param MTU
##

const
  GATT_EVENT_MTU* = 0x000000AB

## *
##  @format H2
##  @param handle
##  @param MTU
##

const
  ATT_EVENT_MTU_EXCHANGE_COMPLETE* = 0x000000B5

## *
##  @format 1H2
##  @param status
##  @param conn_handle
##  @param attribute_handle
##

const
  ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE* = 0x000000B6

##  TODO: daemon only event
## *
##  @format 12
##  @param status
##  @param service_uuid
##

const
  BNEP_EVENT_SERVICE_REGISTERED* = 0x000000C0

## *
##  @format 12222B
##  @param status
##  @param bnep_cid
##  @param source_uuid
##  @param destination_uuid
##  @param mtu
##  @param remote_address
##

const
  BNEP_EVENT_CHANNEL_OPENED* = 0x000000C1

## *
##  @format 222B
##  @param bnep_cid
##  @param source_uuid
##  @param destination_uuid
##  @param remote_address
##

const
  BNEP_EVENT_CHANNEL_CLOSED* = 0x000000C2

## *
##  @format 222B1
##  @param bnep_cid
##  @param source_uuid
##  @param destination_uuid
##  @param remote_address
##  @param channel_state
##

const
  BNEP_EVENT_CHANNEL_TIMEOUT* = 0x000000C3

## *
##  @format 222B
##  @param bnep_cid
##  @param source_uuid
##  @param destination_uuid
##  @param remote_address
##

const
  BNEP_EVENT_CAN_SEND_NOW* = 0x000000C4

## *
##  @format H1B
##  @param handle
##  @param addr_type
##  @param address
##

const
  SM_EVENT_JUST_WORKS_REQUEST* = 0x000000D0

## *
##  @format H1B
##  @param handle
##  @param addr_type
##  @param address
##

const
  SM_EVENT_JUST_WORKS_CANCEL* = 0x000000D1

## *
##  @format H1B4
##  @param handle
##  @param addr_type
##  @param address
##  @param passkey
##

const
  SM_EVENT_PASSKEY_DISPLAY_NUMBER* = 0x000000D2

## *
##  @format H1B
##  @param handle
##  @param addr_type
##  @param address
##

const
  SM_EVENT_PASSKEY_DISPLAY_CANCEL* = 0x000000D3

## *
##  @format H1B421
##  @param handle
##  @param addr_type
##  @param address
##

const
  SM_EVENT_PASSKEY_INPUT_NUMBER* = 0x000000D4

## *
##  @format H1B
##  @param handle
##  @param addr_type
##  @param address
##

const
  SM_EVENT_PASSKEY_INPUT_CANCEL* = 0x000000D5

## *
##  @format H1B
##  @param handle
##  @param addr_type
##  @param address
##

const
  SM_EVENT_IDENTITY_RESOLVING_STARTED* = 0x000000D6

## *
##  @format H1B
##  @param handle
##  @param addr_type
##  @param address
##

const
  SM_EVENT_IDENTITY_RESOLVING_FAILED* = 0x000000D7

## *
##  @format H1B2
##  @param handle
##  @param addr_type
##  @param address
##  @param le_device_db_index
##

const
  SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED* = 0x000000D8

## *
##  @format H1B
##  @param handle
##  @param addr_type
##  @param address
##

const
  SM_EVENT_AUTHORIZATION_REQUEST* = 0x000000D9

## *
##  @format H1B1
##  @param handle
##  @param addr_type
##  @param address
##  @param authorization_result
##

const
  SM_EVENT_AUTHORIZATION_RESULT* = 0x000000DA

## *
##  @format H1B1
##  @param handle
##  @param new private resolvable/non-resolvable address
##

const
  SM_EVENT_PRIVATE_RANDOM_ADDR_UPDATE* = 0x000000DB

##  GAP
## *
##  @format H1
##  @param handle
##  @param security_level
##

const
  GAP_EVENT_SECURITY_LEVEL* = 0x000000E0

## *
##  @format 1B
##  @param status
##  @param address
##

const
  GAP_EVENT_DEDICATED_BONDING_COMPLETED* = 0x000000E1

## *
##  @format 11B1JV
##  @param advertising_event_type
##  @param address_type
##  @param address
##  @param rssi
##  @param data_length
##  @param data
##
##  Meta Events, see below for sub events

const
  HCI_EVENT_HSP_META* = 0x000000E8
  HCI_EVENT_HFP_META* = 0x000000E9
  HCI_EVENT_ANCS_META* = 0x000000EA

##  Potential other meta groups
##  #define HCI_EVENT_BNEP_META                                0xxx
##  #define HCI_EVENT_GAP_META                                 0xxx
##  #define HCI_EVENT_GATT_META                                0xxx
##  #define HCI_EVENT_PAN_META                                 0xxx
##  #define HCI_EVENT_SDP_META                                 0xxx
##  #define HCI_EVENT_SM_META                                  0xxx
## * HSP Subevent
## *
##  @format 11
##  @param subevent_code
##  @param status 0 == OK
##

const
  HSP_SUBEVENT_RFCOMM_CONNECTION_COMPLETE* = 0x00000001

## *
##  @format 11
##  @param subevent_code
##  @param status 0 == OK
##

const
  HSP_SUBEVENT_RFCOMM_DISCONNECTION_COMPLETE* = 0x00000002

## *
##  @format 11H
##  @param subevent_code
##  @param status 0 == OK
##  @param handle
##

const
  HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE* = 0x00000003

## *
##  @format 11
##  @param subevent_code
##  @param status 0 == OK
##

const
  HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE* = 0x00000004

## *
##  @format 1
##  @param subevent_code
##

const
  HSP_SUBEVENT_RING* = 0x00000005

## *
##  @format 11
##  @param subevent_code
##  @param gain Valid range: [0,15]
##

const
  HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED* = 0x00000006

## *
##  @format 11
##  @param subevent_code
##  @param gain Valid range: [0,15]
##

const
  HSP_SUBEVENT_SPEAKER_GAIN_CHANGED* = 0x00000007

## *
##  @format 1JV
##  @param subevent_code
##  @param value_length
##  @param value
##

const
  HSP_SUBEVENT_HS_COMMAND* = 0x00000008

## *
##  @format 1JV
##  @param subevent_code
##  @param value_length
##  @param value
##

const
  HSP_SUBEVENT_AG_INDICATION* = 0x00000009

## * HFP Subevent
## *
##  @format 11H
##  @param subevent_code
##  @param status 0 == OK
##  @param con_handle
##

const
  HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_ESTABLISHED* = 0x00000001

## *
##  @format 1
##  @param subevent_code
##

const
  HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_RELEASED* = 0x00000002

## *
##  @format 11H
##  @param subevent_code
##  @param status 0 == OK
##  @param handle
##

const
  HFP_SUBEVENT_AUDIO_CONNECTION_ESTABLISHED* = 0x00000003

## *
##  @format 1
##  @param subevent_code
##

const
  HFP_SUBEVENT_AUDIO_CONNECTION_RELEASED* = 0x00000004

## *
##  @format 11
##  @param subevent_code
##  @param status 0 == OK
##

const
  HFP_SUBEVENT_COMPLETE* = 0x00000005

## *
##  @format 111T
##  @param subevent_code
##  @param indicator_index
##  @param indicator_status
##  @param indicator_name
##

const
  HFP_SUBEVENT_AG_INDICATOR_STATUS_CHANGED* = 0x00000006

## *
##  @format 1111T
##  @param subevent_code
##  @param network_operator_mode
##  @param network_operator_format
##  @param network_operator_name
##

const
  HFP_SUBEVENT_NETWORK_OPERATOR_CHANGED* = 0x00000007

## *
##  @format 11
##  @param subevent_code
##  @param error
##

const
  HFP_SUBEVENT_EXTENDED_AUDIO_GATEWAY_ERROR* = 0x00000008

## *
##  @format 11
##  @param subevent_code
##  @param status
##

const
  HFP_SUBEVENT_CODECS_CONNECTION_COMPLETE* = 0x00000009

## *
##  @format 1
##  @param subevent_code
##

const
  HFP_SUBEVENT_START_RINGINIG* = 0x0000000A

## *
##  @format 1
##  @param subevent_code
##

const
  HFP_SUBEVENT_STOP_RINGINIG* = 0x0000000B

## *
##  @format 1
##  @param subevent_code
##

const
  HFP_SUBEVENT_CALL_TERMINATED* = 0x0000000C

## *
##  @format 1T
##  @param subevent_code
##  @param number
##

const
  HFP_SUBEVENT_PLACE_CALL_WITH_NUMBER* = 0x0000000D

## *
##  @format 1
##  @param subevent_code
##

const
  HFP_SUBEVENT_ATTACH_NUMBER_TO_VOICE_TAG* = 0x0000000E

## *
##  @format 1T
##  @param subevent_code
##  @param number
##

const
  HFP_SUBEVENT_NUMBER_FOR_VOICE_TAG* = 0x0000000F

## *
##  @format 1T
##  @param subevent_code
##  @param dtmf code
##

const
  HFP_SUBEVENT_TRANSMIT_DTMF_CODES* = 0x00000010

## *
##  @format 1
##  @param subevent_code
##

const
  HFP_SUBEVENT_CALL_ANSWERED* = 0x00000011

## *
##  @format 1
##  @param subevent_code
##

const
  HFP_SUBEVENT_CONFERENCE_CALL* = 0x00000012

## *
##  @format 1
##  @param subevent_code
##

const
  HFP_SUBEVENT_RING* = 0x00000013

## *
##  @format 111
##  @param subevent_code
##  @param status
##  @param gain
##

const
  HFP_SUBEVENT_SPEAKER_VOLUME* = 0x00000014

## *
##  @format 111
##  @param subevent_code
##  @param status
##  @param gain
##

const
  HFP_SUBEVENT_MICROPHONE_VOLUME* = 0x00000015

## *
##  @format 11T
##  @param subevent_code
##  @param type
##  @param number
##

const
  HFP_SUBEVENT_CALL_WAITING_NOTIFICATION* = 0x00000016

## *
##  @format 11T
##  @param subevent_code
##  @param type
##  @param number
##

const
  HFP_SUBEVENT_CALLING_LINE_INDETIFICATION_NOTIFICATION* = 0x00000017

## *
##  @format 111111T
##  @param subevent_code
##  @param clcc_idx
##  @param clcc_dir
##  @param clcc_status
##  @param clcc_mpty
##  @param bnip_type
##  @param bnip_number
##

const
  HFP_SUBEVENT_ENHANCED_CALL_STATUS* = 0x00000018

## *
##  @format 111T
##  @param subevent_code
##  @param status
##  @param bnip_type
##  @param bnip_number
##

const
  HFP_SUBEVENT_SUBSCRIBER_NUMBER_INFORMATION* = 0x00000019

## *
##  @format 1T
##  @param subevent_code
##  @param value
##

const
  HFP_SUBEVENT_RESPONSE_AND_HOLD_STATUS* = 0x0000001A

##  ANCS Client
## *
##  @format 1H
##  @param subevent_code
##  @param handle
##

const
  ANCS_SUBEVENT_CLIENT_CONNECTED* = 0x000000F0

## *
##  @format 1H2T
##  @param subevent_code
##  @param handle
##  @param attribute_id
##  @param text
##

const
  ANCS_SUBEVENT_CLIENT_NOTIFICATION* = 0x000000F1

## *
##  @format 1H
##  @param subevent_code
##  @param handle
##

const
  ANCS_SUBEVENT_CLIENT_DISCONNECTED* = 0x000000F2
