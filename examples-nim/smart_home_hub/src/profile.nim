import
  bluetooth, bt_types, att_db, gap, btstack_event, btstack_defines, btstack_util, gatt_client,
  platform_api, btdatabuilder, sig_uuid, sequtils, sugar, FreeRTOS

let
  advData = ToArray([Flags({LEGeneralDiscoverableMode, BR_EDR_NotSupported}),
                     LocalName("ING Smart Home")])
  scanData = ToArray([@[0u8]])

defineProfile([Service(SIG_UUID_SERVICE_GENERIC_ACCESS),
               Characteristic(SIG_UUID_CHARACT_GAP_DEVICE_NAME, ATT_PROPERTY_READ, "ING Smart Home"),
               Characteristic(SIG_UUID_CHARACT_GAP_APPEARANCE, ATT_PROPERTY_READ, [0u8, 0]),
               Service("{00000004-494e-4743-4849-505355554944}"),
               Characteristic("{bf83f3f1-399a-414d-9035-ce64ceb3ff67}",
                              ATT_PROPERTY_WRITE_WITHOUT_RESPONSE + ATT_PROPERTY_DYNAMIC, [], "HANDLE_SMART_HOME_CTRL"),
               Characteristic("{bf83f3f2-399a-414d-9035-ce64ceb3ff67}",
                              ATT_PROPERTY_NOTIFY, [], "HANDLE_SMART_HOME_STATUS")],
              "profileData")

const
  SLAVE_NUMBER = 4
  MASTER_NUMBER = 4
  INVALID_HANDLE = 0xffffu16

type
  Slave = object
    id: uint8
    connHandle: uint16
    sThermo: gattClientServiceT
    cTemp: gattClientCharacteristicT
    dTemp: gattClientCharacteristicDescriptorT
    nTemp: gattClientNotificationT
    sRGB: gattClientServiceT
    cRGBCtrl: gattClientCharacteristicT

  SmartHomeCmdCode = enum
    ccDeviceStatus, # Packet format: CmdCode, DEV ID, set[StartHomeDevice]
    ccTemperatureReport, # Packet format: CmdCode, Dev ID, same as temperature measurement char
    ccRGB # Packet format: RGBPacket

  AttServer = object
    connHandle: hciConHandleT
    notify: bool
    dirty: set[SmartHomeCmdCode]

  RGB = object
    R*, G*, B*: uint8

  StartHomeDevice = enum
    sdThermoeter,
    sdRGBLed

  RGBPacket = object
    code: SmartHomeCmdCode
    RGBs: array[SLAVE_NUMBER, RGB]

let
  slaveAddreses: array[SLAVE_NUMBER, bdAddrT] = [
    [0xC2u8, 0x12, 0x35, 0x98, 0x67, 0x00],
    [0xC2u8, 0x12, 0x35, 0x98, 0x67, 0x01],
    [0xC2u8, 0x12, 0x35, 0x98, 0x67, 0x02],
    [0xC2u8, 0x12, 0x35, 0x98, 0x67, 0x03]]

var
  attServers: array[MASTER_NUMBER, AttServer] = [
    AttServer(connHandle: INVALID_HANDLE),
    AttServer(connHandle: INVALID_HANDLE),
    AttServer(connHandle: INVALID_HANDLE),
    AttServer(connHandle: INVALID_HANDLE)]

  slaves: array[SLAVE_NUMBER, Slave] = [
    Slave(id: 0, connHandle: INVALID_HANDLE),
    Slave(id: 1, connHandle: INVALID_HANDLE),
    Slave(id: 2, connHandle: INVALID_HANDLE),
    Slave(id: 3, connHandle: INVALID_HANDLE)]

  slaveInitiating: ptr Slave = nil
  initiatingTimer: TimerHandle_t = nil

  rgbPacket: RGBPacket = RGBPacket(code: ccRGB)

const DBG_PRINT = 1

when DBG_PRINT == 1:
  proc printf(fmt: cstring) {. varargs, importc: "printf", header: "stdio.h".}
  let iPrintf = platform_printf
else:
  proc iPrintf(format: cstring) {.varargs.} =
    discard

proc first[T](s: var openArray[T]; pred: proc (x: T): bool): ptr T =
  for i in 0..high(s):
    if pred(s[i]): return addr s[i]
  return nil

proc printAddr(address: bdAddrT) =
  iPrintf("%02X:%02X:%02X:%02X:%02X:%02X\n", address[0], address[1], address[2], address[3], address[4], address[5]);

proc startScanIfNeeded() =
  if slaves.any((slave) => slave.connHandle == INVALID_HANDLE):
    discard gapSetExtScanEnable(1, 0, 0, 0)

proc slaveFromConnHandle(connHandle: uint16): ptr Slave =
  var env {.global.} : uint16
  env = connHandle
  proc predSlave(slave: Slave): bool =
    return slave.connHandle == env

  return first(slaves, predSlave)

proc slaveFromAddr(address: bdAddrT): ptr Slave =
  var reversed: bdAddrT
  reverseBDAddr(unsafeAddr address[0], unsafeAddr reversed[0])
  for i in 0..high(slaveAddreses):
    if slaveAddreses[i] == reversed:
      return addr slaves[i]
  return nil

proc broadcast(data: ptr uint8; len: int; code: SmartHomeCmdCode; exclude: uint16 = INVALID_HANDLE) =
  for i in 0 .. high(attServers):
    var server = addr attServers[i]
    if not server.notify: continue
    if server.connHandle == exclude: continue
    if code != ccTemperatureReport:
      if attServerIndicate(server.connHandle, HANDLE_SMART_HOME_STATUS, data, cast[uint16](len)) != 0:
        incl(server.dirty, code)
    else:
      discard attServerNotify(server.connHandle, HANDLE_SMART_HOME_STATUS, data, cast[uint16](len))

proc attServerFromConnHandle(connHandle: uint16): ptr AttServer =
  var env {.global.} : uint16
  env = connHandle
  proc predServer(slave: AttServer): bool =
    return slave.connHandle == env

  return first(attServers, predServer)

proc broadcast(data: openArray[uint8]; code: SmartHomeCmdCode; exclude: uint16 = INVALID_HANDLE) =
  broadcast(unsafeAddr data[0], len(data), code, exclude)

proc status(slave: ptr Slave): uint8 =
  var t: set[StartHomeDevice] = {}
  if slave.connHandle != INVALID_HANDLE:
    if slave.cTemp.valueHandle != INVALID_HANDLE: incl(t, sdThermoeter)
    if slave.cRGBCtrl.valueHandle != INVALID_HANDLE: incl(t, sdRGBLed)
  return cast[uint8](t)

proc updateStatus(server: ptr AttServer) =
  var packet: array[1 + SLAVE_NUMBER * 2, uint8] = [cast[uint8](ccDeviceStatus), 0, 0, 1, 0, 2, 0, 3, 0]
  for i in 0 ..< slaves.len:
    packet[1 + i * 2 + 1] = status(addr slaves[i])
  if attServerIndicate(server.connHandle, HANDLE_SMART_HOME_STATUS, unsafeAddr packet[0], cast[uint16](len(packet))) != 0:
    incl(server.dirty, ccDeviceStatus)

proc updateStatus(connHandle: uint16) =
  var server = attServerFromConnHandle(connHandle)
  if server != nil:
    updateStatus(server)

proc syncStatus(connHandle: uint16) =
  var server = attServerFromConnHandle(connHandle)
  if server == nil:
    return
  if ccDeviceStatus in server.dirty:
    excl(server.dirty, ccDeviceStatus)
    updateStatus(connHandle)
  if ccRGB in server.dirty:
    if attServerIndicate(server.connHandle, HANDLE_SMART_HOME_STATUS, cast[ptr uint8](addr rgbPacket),
                         cast[uint16](sizeOf rgbPacket)) == 0:
      excl(server.dirty, ccRGB)

proc broadcastStatus(slave: ptr Slave) =
  let packet: array[3, uint8] = [cast[uint8](ccDeviceStatus), slave.id, status(slave)]
  broadcast(packet, ccDeviceStatus)

proc temperatureNotificationHandler(packetType: uint8; connHandle: uint16; packet: ptr uint8; size: uint16) {.noconv.} =
  case packet[]
  of GATT_EVENT_NOTIFICATION:
    let slave = slaveFromConnHandle(connHandle)
    if slave == nil: return
    var valueLen: uint16
    let value = gattEventNotificationParse(packet, size, addr valueLen)
    let packet: array[6, uint8] = [cast[uint8](ccTemperatureReport), slave.id,
                                   value.value[1], value.value[2], value.value[3], value.value[4]]
    broadcast(packet, ccTemperatureReport)
  else:
    discard

proc setRGB(id: uint8, rgb: ptr uint8): bool =
  var predSlaveEnv {.global.} : uint8
  predSlaveEnv = id
  proc predSlave(slave: Slave): bool =
    return slave.connHandle != INVALID_HANDLE and slave.id == predSlaveEnv

  let slave = first(slaves, predSlave)

  if slave == nil: return false
  if slave.cRGBCtrl.valueHandle == INVALID_HANDLE: return false
  discard gattClientWriteValueOfCharacteristicWithoutResponse(slave.connHandle,
    slave.cRGBCtrl.valueHandle, 3, rgb)
  return true

let
  # INGChips RGB Lighting Service & Characteristics:
  UUID_RGB_SERVICE = ToArray(uuidToSeq("{6a33a526-e004-4793-a084-8a1dc49b84fd}"))
  UUID_RGB_CTRL_CHAR = ToArray(uuidToSeq("{1c190e92-37dd-4ac4-8154-0444c69274c2}"))

proc discoverComplete(connHandle: uint16) =
  broadcastStatus(slaveFromConnHandle(connHandle))

proc descWriteCallback(packetType: uint8; channel: uint16; packet: ptr uint8; size: uint16) {.noconv.} =
  case packet[]
  of GATT_EVENT_QUERY_COMPLETE:
    iPrintf("descWrite: %d\n", gattEventQueryCompleteGetStatus(packet))
    let connHandle = gattEventQueryCompleteGetHandle(packet)
    if gattEventQueryCompleteGetStatus(packet) != 0:
      discard gapDisconnect(connHandle)
      return
    iPrintf("compl\n")

    discoverComplete(connHandle)
  else:
    discard

let charconfigNotification = ToArray([toSeqU8(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)])

proc startNotify(connHandle: uint16) =
  let slave = slaveFromConnHandle(connHandle)
  if slave.dTemp.handle != INVALID_HANDLE:
    gattClientListenForCharacteristicValueUpdates(addr slave.nTemp, temperatureNotificationHandler,
                                                  connHandle, addr slave.cTemp)
    discard gattClientWriteCharacteristicDescriptor(descWriteCallback, connHandle, addr slave.dTemp,
                                            cast[uint16](len(charconfigNotification)),
                                            unsafeAddr charconfigNotification[0])

proc rgbCharDiscoveryCallback(packetType: uint8; channel: uint16; packet: ptr uint8; size: uint16) {.noconv.} =
  case packet[]
  of GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
    let connHandle = gattEventCharacteristicQueryResultGetHandle(packet)
    let slave = slaveFromConnHandle(connHandle)
    gattEventCharacteristicQueryResultGetCharacteristic(packet, addr slave.cRGBCtrl)
  of GATT_EVENT_QUERY_COMPLETE:
    let connHandle = gattEventQueryCompleteGetHandle(packet)
    iPrintf("rgbCharDisc: %d\n", gattEventQueryCompleteGetStatus(packet))
    if gattEventQueryCompleteGetStatus(packet) != 0:
      discard gapDisconnect(connHandle)
      return
    let slave = slaveFromConnHandle(connHandle)
    # turn light to gree when connected
    var rgb: array[3, uint8] = [0u8, 200, 0]
    discard gattClientWriteValueOfCharacteristicWithoutResponse(connHandle,
      slave.cRGBCtrl.valueHandle, 3, cast[ptr uint8](unsafeAddr rgb))
    startNotify(connHandle)
  else:
    discard

proc rgbServiceDiscoveryCallback(packetType: uint8; channel: uint16; packet: ptr uint8; size: uint16) {.noconv.} =
  case packet[]
  of GATT_EVENT_SERVICE_QUERY_RESULT:
    let connHandle = gattEventServiceQueryResultGetHandle(packet)
    let slave = slaveFromConnHandle(connHandle)
    if slave == nil: return
    gattEventServiceQueryResultGetService(packet, addr slave.sRGB)
  of GATT_EVENT_QUERY_COMPLETE:
    iPrintf("rgbServiceDisc: %d\n", gattEventQueryCompleteGetStatus(packet))
    let connHandle = gattEventQueryCompleteGetHandle(packet)
    if gattEventQueryCompleteGetStatus(packet) != 0:
      discard gapDisconnect(connHandle)
      return
    let slave = slaveFromConnHandle(connHandle)
    if slave.sRGB.startGroupHandle != INVALID_HANDLE:
      discard gattClientDiscoverCharacteristicsForServiceByUuid128(rgbCharDiscoveryCallback, connHandle,
                                                          addr slave.sRGB, unsafeAddr UUID_RGB_CTRL_CHAR[0])
    else:
      startNotify(connHandle)
  else:
    discard

proc thermoDescDiscoveryCallback(packetType: uint8; channel: uint16; packet: ptr uint8; size: uint16) {.noconv.} =
  case packet[]
  of GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT:
    let connHandle = gattEventAllCharacteristicDescriptorsQueryResultGetHandle(packet)
    let slave = slaveFromConnHandle(connHandle)
    gattEventAllCharacteristicDescriptorsQueryResultGetCharacteristicDescriptor(packet, addr slave.dTemp)
  of GATT_EVENT_QUERY_COMPLETE:
    let connHandle = gattEventQueryCompleteGetHandle(packet)
    iPrintf("thermoDescDisc: %d\n", gattEventQueryCompleteGetStatus(packet))
    if gattEventQueryCompleteGetStatus(packet) != 0:
      discard gapDisconnect(connHandle)
      return
    discard gattClientDiscoverPrimaryServicesByUuid128(rgbServiceDiscoveryCallback, connHandle,
                                                       unsafeAddr UUID_RGB_SERVICE[0])
  else:
    discard

proc thermoCharDiscoveryCallback(packetType: uint8; channel: uint16; packet: ptr uint8; size: uint16) {.noconv.} =
  case packet[]
  of GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
    let connHandle = gattEventCharacteristicQueryResultGetHandle(packet)
    let slave = slaveFromConnHandle(connHandle)
    gattEventCharacteristicQueryResultGetCharacteristic(packet, addr slave.cTemp)
  of GATT_EVENT_QUERY_COMPLETE:
    let connHandle = gattEventQueryCompleteGetHandle(packet)
    iPrintf("thermoCharDisc: %d\n", gattEventQueryCompleteGetStatus(packet))
    if gattEventQueryCompleteGetStatus(packet) != 0:
      discard gapDisconnect(connHandle)
      return
    let slave = slaveFromConnHandle(connHandle)
    if slave.cTemp.valueHandle != INVALID_HANDLE:
      discard gattClientDiscoverCharacteristicDescriptors(thermoDescDiscoveryCallback, connHandle, addr slave.cTemp)
  else:
    discard

proc thermoServiceDiscoveryCallback(packetType: uint8; channel: uint16; packet: ptr uint8; size: uint16) {.noconv.} =
  case packet[]
  of GATT_EVENT_SERVICE_QUERY_RESULT:
    let connHandle = gattEventServiceQueryResultGetHandle(packet)
    let slave = slaveFromConnHandle(connHandle)
    if slave == nil:
      return
    gattEventServiceQueryResultGetService(packet, addr slave.sThermo);
  of GATT_EVENT_QUERY_COMPLETE:
    let connHandle = gattEventQueryCompleteGetHandle(packet)
    iPrintf("thermoServiceDisc: %d\n", gattEventQueryCompleteGetStatus(packet))
    if gattEventQueryCompleteGetStatus(packet) != 0:
      discard gapDisconnect(connHandle)
      return
    let slave = slaveFromConnHandle(connHandle)
    if slave.sThermo.startGroupHandle != INVALID_HANDLE:
      discard gattClientDiscoverCharacteristicsForServiceByUUID16(thermoCharDiscoveryCallback, connHandle,
                                                          addr slave.sThermo, SIG_UUID_CHARACT_TEMPERATURE_MEASUREMENT)
    else:
      discard gattClientDiscoverPrimaryServicesByUuid128(rgbServiceDiscoveryCallback, gattEventQueryCompleteGetHandle(packet),
                                                       unsafeAddr UUID_RGB_SERVICE[0])
  else:
    discard

proc slaveConnected(connComplete: ptr leMetaEventCreateConnCompleteT, slave: ptr Slave) =
  slave.connHandle = connComplete.handle
  slave.cTemp.valueHandle = INVALID_HANDLE
  slave.cRGBCtrl.valueHandle = INVALID_HANDLE
  slave.sThermo.startGroupHandle = INVALID_HANDLE
  slave.sRGB.startGroupHandle = INVALID_HANDLE
  discard gattClientDiscoverPrimaryServicesByUuid16(thermoServiceDiscoveryCallback, slave.connHandle,
                                                    SIG_UUID_SERVICE_HEALTH_THERMOMETER)

proc slaveDisconnected(connHandle: hciConHandleT): bool =
  let slave = slaveFromConnHandle(connHandle)
  if slave != nil:
    slave.connHandle = INVALID_HANDLE
    slave.cTemp.valueHandle = INVALID_HANDLE
    slave.cRGBCtrl.valueHandle = INVALID_HANDLE
    broadcastStatus(slave)
    startScanIfNeeded()
  return slave != nil

proc attServerFromConn(connHandle: hciConHandleT): ptr AttServer =
  for i in 0..high(attServers):
    if attServers[i].connHandle == connHandle:
      return addr attServers[i]
  return nil

proc attServerConnected(connComplete: ptr leMetaEventCreateConnCompleteT) =
  let server = attServerFromConn(INVALID_HANDLE)
  if server == nil:
    iPrintf("too many att servers\n")
    discard gapDisconnect(connComplete.handle)
    return
  server.notify = false
  server.connHandle = connComplete.handle
  attSetDb(connComplete.handle, unsafeAddr(profileData[0]))

proc attServerDisconnected(connHandle: hciConHandleT): bool =
  let server = attServerFromConn(connHandle)
  if server != nil:
    server.connHandle = INVALID_HANDLE
    server.notify = false
  return server != nil

proc attReadCallback(connHandle: hciConHandleT; attHandle: uint16;
                     offset: uint16;
                     buffer: ptr UncheckedArray[uint8]; bufferSize: uint16): uint16 {.noconv.} =
  case attHandle
  else:
    return 0

proc attWriteCallback(connHandle: hciConHandleT; attHandle: uint16;
                    transactionMode: uint16;
                    offset: uint16;
                    buffer: ptr UncheckedArray[uint8]; bufferSize: uint16): uint16 {.noconv.} =
  case attHandle
  of HANDLE_SMART_HOME_CTRL:
    case buffer[0]
    of ord(ccRGB):
      # iPrintf("RGB=%d,%d-%d-%d\n", buffer[1],buffer[2],buffer[3],buffer[4])
      if setRGB(buffer[1], addr buffer[2]):
        rgbPacket.RGBs[buffer[1]].R = buffer[2]
        rgbPacket.RGBs[buffer[1]].G = buffer[3]
        rgbPacket.RGBs[buffer[1]].B = buffer[4]
        broadcast(cast[ptr uint8](addr rgbPacket), sizeOf rgbPacket, ccRGB)
      discard
    else:
      discard
  of HANDLE_SMART_HOME_STATUS + 1:
    let server = attServerFromConn(connHandle)
    if server != nil:
      server.notify = cast[ptr UncheckedArray[uint16]](buffer)[0] == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION
      if server.notify:
        updateStatus(connHandle)
  else:
    return 0

const
  USER_MSG_INITIATE_TIMOUT = 1

proc initiatingTimerCallback(xTimer: TimerHandle_t) {.noconv.} =
  discard btstackPushUserMsg(USER_MSG_INITIATE_TIMOUT, nil, 0)

proc userMsgHandler(msgId: uint32; data: pointer; size: uint16) =
  # add your code
  case msgId
  of USER_MSG_INITIATE_TIMOUT:
    if slaveInitiating != nil:
      iPrintf("initiate timout\n")
      slaveInitiating = nil
      discard gapCreateConnectionCancel()
      startScanIfNeeded()
  else:
    discard

let
  extAdvSetEn = [extAdvSetEnT(handle: 0, duration: 0, maxEvents: 0)]

  scanConfigs =
    [
      scanPhyConfigT(phy: PHY_1M,    `type`: SCAN_PASSIVE, interval: 100, window: 25),
      scanPhyConfigT(phy: PHY_CODED, `type`: SCAN_PASSIVE, interval: 100, window: 25)
    ]

var
  initPhyConfigs =
    [
      initiatingPhyConfigT(
        phy: PHY_1M,
        connParam: connParaT(
          scanInt: 100,
          scanWin: 20,
          intervalMin: 80,
          intervalMax: 90,
          latency: 0,
          supervisionTimeout: 400,
          minCELen: 10,
          maxCELen: 15
        ))
    ]

  peerAddr: bdAddrT

proc setupAdv() =
  discard gapSetExtAdvPara(0,
          cast[uint8]({ADV_CONNECTABLE, ADV_SCANNABLE, ADV_LEGACY}),
          0x500, 0x500,              # Primary_Advertising_Interval_Min, Primary_Advertising_Interval_Max
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
  let randAddr: bdAddrT = [0xC0u8 or 0x12, 0x6C, 0xEF, 0x91, 0x5D, 0x5D]
  if packetType != HCI_EVENT_PACKET: return
  case hciEventPacketGetType(packet)
    of BTSTACK_EVENT_STATE:
      if cast[HCI_STATE](btstackEventStateGetState(packet)) != HCI_STATE_WORKING:
        return

      gapSetRandomDeviceAddress(unsafeAddr randAddr[0])
      discard gapSetAdvSetRandomAddr(0, unsafeAddr randAddr[0])
      setupAdv()

      discard gapSetExtScanPara(BD_ADDR_TYPE_LE_RANDOM, SCAN_ACCEPT_ALL_EXCEPT_NOT_DIRECTED,
                        cast[uint8](len(scanConfigs)),
                        unsafeAddr scanConfigs[0]);
      startScanIfNeeded()
    of HCI_EVENT_COMMAND_COMPLETE:
      # add your code to check command complete response
      # case hci_event_command_complete_get_command_opcode(packet)
      # of ...
      discard
    of HCI_EVENT_LE_META:
      case hciEventLEMetaGetSubEventCode(packet):
        of HCI_SUBEVENT_LE_EXTENDED_ADVERTISING_REPORT:
          if slaveInitiating != nil:
            return
          let report = unsafeAddr decodeHciLEMetaEvent(packet, le_meta_event_ext_adv_report_t).reports[0]
          let slave = slaveFromAddr(report.address)
          if slave != nil and slave.connHandle == INVALID_HANDLE:
            slaveInitiating = slave
            discard gapSetExtScanEnable(0, 0, 0, 0)
            reverse_bd_addr(unsafeAddr report.address[0], unsafeAddr peerAddr[0])
            iPrintf("connecting... ")
            printAddr(peerAddr)
            if (report.evtType and HCI_EXT_ADV_PROP_USE_LEGACY) != 0:
              initPhyConfigs[0].phy = PHY_1M
            else:
              initPhyConfigs[0].phy = cast[phyTypeT](if report.sPhy != 0: report.sPhy else: report.pPhy)
            discard  gapExtCreateConnection(INITIATING_ADVERTISER_FROM_PARAM, # Initiator_Filter_Policy,
                                  BD_ADDR_TYPE_LE_RANDOM,           # Own_Address_Type,
                                  report.addrType,                  # Peer_Address_Type,
                                  addr peerAddr[0],                 # Peer_Address,
                                  cast [uint8](len(initPhyConfigs)),
                                  addr initPhyConfigs[0]);
            discard xTimerReset(initiatingTimer, portMAX_DELAY)
        of HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
          let connComplete = decodeHciLEMetaEvent(packet, leMetaEventCreateConnCompleteT)
          iPrintf("role = %d, handle = %d\n", connComplete.role, connComplete.handle)
          if connComplete.role == HCI_ROLE_SLAVE:
            if connComplete.status == 0:
              attServerConnected(connComplete)
              llHintOnCELen(connComplete.handle, 10, 12)
          elif slaveInitiating != nil:
            discard xTimerStop(initiatingTimer, portMAX_DELAY)
            if connComplete.status == 0:
              slaveConnected(connComplete, slaveInitiating)
            slaveInitiating = nil
            startScanIfNeeded()
        of HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED:
          discard gapSetExtAdvEnable(1, cast[uint8](len extAdvSetEn), unsafeAddr(extAdvSetEn[0]))
        else:
          discard
    of HCI_EVENT_DISCONNECTION_COMPLETE:
      let disconn = decodeHciEventDisconnComplete(packet);
      iPrintf("disc %d\n", disconn.connHandle)
      if not attServerDisconnected(disconn.connHandle):
        discard slaveDisconnected(disconn.connHandle)
    of ATT_EVENT_CAN_SEND_NOW:
      # add your code
      discard
    of  ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE:
      syncStatus(att_event_handle_value_indication_complete_get_conn_handle(packet))
    of BTSTACK_EVENT_USER_MSG:
      let userMsg = hciEventPacketGetUserMsg(packet)
      userMsgHandler(userMsg.msgId, userMsg.data, userMsg.len)
    else:
      discard

proc setupProfile*(unused1: pointer; unused2: pointer): uint32 {.exportc noconv.} =
  # Note: security has not been enabled.
  initiatingTimer = xTimerCreate(cast[cstring]("a"),
                            pdMS_TO_TICKS(5000),
                            pdFALSE,
                            nil,
                            initiatingTimerCallback);
  attServerInit(cast[attReadCallbackT](attReadCallback), cast[attWriteCallbackT](attWriteCallback))
  var hciEventCallbackReg {.global.}= btstackPacketCallbackRegistrationT(callback: userPacketHandler)
  hciAddEventHandler(addr hciEventCallbackReg)
  attServerRegisterPacketHandler(userPacketHandler)
  return 0
