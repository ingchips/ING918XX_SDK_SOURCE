import
  bluetooth, bt_types, att_db, gap, btstack_event, btstack_defines,
  platform_api, btdatabuilder, sig_uuid, FreeRTOS

let
  advData = ToArray([Flags({LEGeneralDiscoverableMode, BR_EDR_NotSupported}),
                     LocalName("Nim Battery")])
  scanData = ToArray([TxPower(-30)])

defineProfile([Service(SIG_UUID_SERVICE_GENERIC_ACCESS),
               Characteristic(SIG_UUID_CHARACT_GAP_DEVICE_NAME, ATT_PROPERTY_READ, "Nim Battery"),
               Characteristic(SIG_UUID_CHARACT_GAP_APPEARANCE, ATT_PROPERTY_READ, [0u8, 0]),
               Service(SIG_UUID_SERVICE_BATTERY_SERVICE),
               Characteristic(SIG_UUID_CHARACT_BATTERY_LEVEL, ATT_PROPERTY_READ, [20u8], "HANDLE_BATTERY_LEVEL")],
               "profileData", false)


proc attReadCallback(connHandle: hciConHandleT; attHandle: uint16;
                     offset: uint16;
                     buffer: ptr UncheckedArray[uint8]; bufferSize: uint16): uint16 {.noconv.} =
  case connHandle

    else:
      return 0

proc attWriteCallback(connHandle: hciConHandleT; attHandle: uint16;
                    transactionMode: uint16; 
                    offset: uint16;
                    buffer: ptr UncheckedArray[uint8]; bufferSize: uint16): uint16 {.noconv.} =
  case connHandle

    else:
      return 0

proc userMsgHandler(msgId: uint32; data: pointer; size: uint16) = 
  # add your code
  case msgId        
    #of MY_MESSAGE_ID:
    #    
  else:
    discard

let
  extAdvSetEn = [extAdvSetEnT(handle: 0, duration: 0, maxEvents: 0)]

proc setupAdv() =
    discard gapSetExtAdvPara(0, 
            cast[uint8]({ADV_CONNECTABLE, ADV_SCANNABLE, ADV_LEGACY}),
            0x00a1, 0x00a1,            # Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
            PRIMARY_ADV_ALL_CHANNELS,  # Primary_Advertising_Channel_Map
            BD_ADDR_TYPE_LE_RANDOM,    # Own_Address_Type
            BD_ADDR_TYPE_LE_PUBLIC,    # Peer_Address_Type (ignore)
            nil,                       # Peer_Address      (ignore)
            ADV_FILTER_ALLOW_ALL,      # Advertising_Filter_Policy
            0x00,                      # Advertising_Tx_Power
            PHY_1M,                    # Primary_Advertising_PHY
            0,                         # Secondary_Advertising_Max_Skip
            PHY_1M,                    # Secondary_Advertising_PHY
            0x00,                      # Advertising_SID
            0x00);                     # Scan_Request_Notification_Enable

    discard gapSetExtAdvData(0, cast[uint16](len advData), unsafeAddr(advData[0]))
    discard gapSetExtScanResponseData(0, cast[uint16](len scanData), unsafeAddr(scanData[0]))
    discard gapSetExtAdvEnable(1, cast[uint8](len extAdvSetEn), unsafeAddr(extAdvSetEn[0]))

proc userPacketHandler(packetType: uint8; channel: uint16; packet: ptr uint8; size: uint16) {.noconv.} =
  let randAddr: bdAddrT  = [0xC0u8, 0x9E, 0x3C, 0x0C, 0x00, 0x29]
  if packetType != HCI_EVENT_PACKET: return
  case hciEventPacketGetType(packet)
    of BTSTACK_EVENT_STATE:
      if cast[HCI_STATE](btstackEventStateGetState(packet)) != HCI_STATE_WORKING:
        return

      discard gapSetAdvSetRandomAddr(0, unsafeAddr randAddr[0])      
      setupAdv()
    of HCI_EVENT_COMMAND_COMPLETE:
      # add your code to check command complete response
      # case hci_event_command_complete_get_command_opcode(packet)
      # of ...
      discard
    of HCI_EVENT_LE_META:
      case hciEventLEMetaGetSubEventCode(packet):
        of HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
          attSetDb(decodeHciLEMetaEvent(packet, leMetaEventCreateConnCompleteT).handle,
                   unsafeAddr(profileData[0]))
        else:
          discard
    of HCI_EVENT_DISCONNECTION_COMPLETE:
      discard gapSetExtAdvEnable(1, cast[uint8](len extAdvSetEn), unsafeAddr(extAdvSetEn[0]))
    of ATT_EVENT_CAN_SEND_NOW:
      # add your code
      discard
    of BTSTACK_EVENT_USER_MSG:
      let userMsg = hciEventPacketGetUserMsg(packet)
      userMsgHandler(userMsg.msgId, userMsg.data, userMsg.len)
    else:
      discard

proc rand(): cint {. importc: "rand", header: "stdlib.h".}

proc updateBatteryLevel(unused: pointer) {.noconv.} =
  while true:
    vTaskDelay(pdMS_TO_TICKS(1000))
    profileData[HANDLE_BATTERY_LEVEL_OFFSET] = cast[uint8](rand() mod 101)

proc setupProfile*(unused1: pointer; unused2: pointer): uint32 {.exportc noconv.} =
  # Note: security has not been enabled.
  attServerInit(cast[attReadCallbackT](attReadCallback), cast[attWriteCallbackT](attWriteCallback))
  var hciEventCallbackReg {.global.}= btstackPacketCallbackRegistrationT(callback: userPacketHandler)
  hciAddEventHandler(addr hciEventCallbackReg)
  attServerRegisterPacketHandler(userPacketHandler)
  discard xTaskCreate(updateBatteryLevel, "b", configMINIMAL_STACK_SIZE, nil, configMAX_PRIORITIES - 1, nil)
  return 0
