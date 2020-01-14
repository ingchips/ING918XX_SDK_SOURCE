#
#  Copyright (C) INGCHIPS. All rights reserved.
#  This code is INGCHIPS proprietary and confidential.
#  Any use of the code for whatever purpose is subject to
#  specific written permission of INGCHIPS.
#

## This module implements *compile time* data builders for advertising & GATT profile.
## 
## Functionalities:
##
## 1. Use ``includeCBytes`` to load bytes defined in C-compatible format
##
## .. code-block::  
##   let advData = includeCBytes"./data/advertising.adv"
## 
## 2. Use ``Flags``, ``LocalName``, ``iBeacon``, etc to compose advertising data manualy
##
## .. code-block::  
##   let advData = concatToArray([Flags({LEGeneralDiscoverableMode, BR_EDR_NotSupported}),
##                                iBeacon("{E9052F1E-9D67-4A6E-B2D7-459D132D6A94}", 0, 0, -50)])
## 
## 3. Use ``defineProfile``, ``Service``, ``Characteristic``, and ``Descriptor`` to compose GATT profile.
##    Handles and value offset are defined as consts.
##
##    In below example, `profileData` contains the profile data, `HANDLE_BATTERY_LEVEL` represents its handle,
##    and, `HANDLE_BATTERY_LEVEL_OFFSET` represents the offset (in byte) of value (battery level, 10u8) in the profile.
##    
##    Use `*` suffix to make profile data or other consts public.
##    
##    Data of each item (Service/Characteristic/Descriptor) and generated code are all printed during compiling.
##
## .. code-block::
##   defineProfile([Service(SIG_UUID_SERVICE_GENERIC_ACCESS),
##                  Characteristic(SIG_UUID_CHARACT_GAP_DEVICE_NAME, ATT_PROPERTY_READ, "nim profile"),
##                  Characteristic(SIG_UUID_CHARACT_GAP_APPEARANCE, ATT_PROPERTY_READ, [0u8, 0]),
##                  Service(SIG_UUID_SERVICE_BATTERY_SERVICE),
##                  Characteristic(SIG_UUID_CHARACT_BATTERY_LEVEL, ATT_PROPERTY_READ, @[10u8]), "HANDLE_BATTERY_LEVEL*"],
##                  "profileData*", false)
##

import
    strutils, sequtils, macros, sugar, bluetooth, algorithm, strformat

proc a2B(a: string): uint8 =
    cast[uint8]((if a.toLower.startsWith("0x"): parseHexInt else: parseInt) a)
        
proc staticParse(filename: string): seq[uint8] {.compileTime.} =
    result = @[]
    for s in readFile(filename).splitLines():
        if s.startsWith("//"):
            continue
        result.add s.replace(" ").split({',', '\r', '\n'}).filterIt(len(it) > 0).map(a2B)

macro includeCBytes*(filename: static[string]): untyped =
    result = nnkBracket.newTree
    let data = staticParse(filename)
    result.add data.map(newLit)

func assertMsg(b: bool; msg: string) {.compileTime.} =
    if not b:
        debugEcho msg
        assert(false)

type
    AdvFlag* = enum
        LELimitedDiscoverableMode
        LEGeneralDiscoverableMode
        BR_EDR_NotSupported
        Simultaneous_LE_BR_EDR_Controller
        Simultaneous_LE_BR_EDR_Host

func toSeqU8(v: uint8): seq[uint8] {.compileTime.} = @[v]
func toSeqU8*(v: uint16): seq[uint8] {.compileTime.} = @[cast[uint8](v and 0xff), cast[uint8](v shr 8)]
func toSeqU8(v: openArray[uint8]): seq[uint8] {.compileTime.} = return toSeq(v)
func toSeqU8(s: string): seq[uint8] {.compileTime.} =
    return toSeq(s).map(c => cast[uint8](ord(c)))

func uuidToSeqU8(v: uint16): seq[uint8] {.compileTime.} = return toSeqU8 v
func uuidToSeqU8(s: string): seq[uint8] {.compileTime.} =
    var s = s.filterit(it in HexDigits)
    result = s.distribute(len(s) div 2).mapIt(cast[uint8](parseHexInt "0x" & join(it))).reversed()
    assertMsg(len(result) == 16, "invalid GUID. example: {01234567-89AB-CDEF-0123-456789ABCDEF}")

func uuidToSeq*(s: string): seq[uint8] {.compileTime.} = uuidToSeqU8(s).reversed()

func Geneneric*(id: uint8; param: openArray[uint8]): seq[uint8] {.compileTime.} =
    result = @[cast[uint8](1 + len(param)), id]
    result.add(param)

func Flags*(flags: set[AdvFlag]): seq[uint8] {.compileTime.} =
    var v: uint8 = 0
    for x in AdvFlag:
        if x in flags: v += 1u8 shl ord(x)
    return @[2u8, 1, cast[uint8](v)]

func Services*[T](uuids: openArray[T]; complete: bool = true): seq[uint8] {.compileTime.} =
    var param: seq[uint8] = concat uuids.map(uuidToSeqU8)
    let t: uint8 = if typeof(T) is uint8: 2 else: (if typeof(T) is uint32: 4 else: 6)
    return Geneneric(t + (if complete: 1 else: 0), param)

func ServiceData*[UUIDT, DataT](uuid: UUIDT; data: DataT): seq[uint8] {.compileTime.} =
    let t: uint8 = if typeof(UUIDT) is uint8: 16 else: (if typeof(UUIDT) is uint32: 20 else: 21)
    return Geneneric(t, uuid.uuidToSeqU8() & data.toSeqU8)

func LocalName*(name: string; shorten: bool = false): seq[uint8] {.compileTime.} =
    return Geneneric(if shorten: 8 else: 9, toSeq(name).map(c => cast[uint8](ord(c))))

func TxPower*(power: int8): seq[uint8] {.compileTime.} = @[2u8, 0xa, cast[uint8](power)]

func Manufacturer*[T](data: T): seq[uint8] {.compileTime.} = return Geneneric(0xff, data.toSeqU8)

func iBeacon*(guid: string; major, minor: uint16; txPower: int8): seq[uint8] {.compileTime.} =
    var param = @[0x4Cu8, 0x00, 0x02, 0x15]
    param.add reversed uuidToSeqU8 guid
    param.add toSeqU8 major
    param.add toSeqU8 minor
    param.add cast[uint8](txPower)
    return Geneneric(0xff, param)

const EddystoneServiceID* = 0xFEAAu16

func EddystoneUID*(nameSpace, instance: openArray[uint8]; txPowerAt0: int8; ): seq[uint8] {.compileTime.} =
    if nameSpace.len != 10: debugEcho "nameSpace should be 10 bytes"
    if instance.len != 6: debugEcho "instance should be 6 bytes"
    return ServiceData(EddystoneServiceID, concat(@[@[0x00u8, cast[uint8](txPowerAt0)], toSeqU8 namespace, toSeqU8 instance]))

func EddystoneEncodeURL(aurl: string): seq[uint8] {.compileTime.} =
    const
        URLCompo = [".com/", ".org/", ".edu/", ".net/", ".info/",
           ".biz/", ".gov/", ".com", ".org", ".edu", ".net", ".info", ".biz", ".gov"]
        URLProto = ["http://www.", "https://www.", "http://", "https://"]
    result = @[]
    var url = aurl
    for i, v in URLProto:
        if url.startsWith(v):
            result.add(cast[uint8](i))
            url.delete(0, v.len - 1)
            break
    while url.len > 0:
        block whileBody:
            for i, v in URLCompo:
                if url.startsWith(v):
                    result.add(cast[uint8](i))
                    url.delete(0, v.len - 1)
                    break whileBody
            result.add(cast[uint8](ord(url[0])))
            url.delete(0, 0)
        
func EddystoneURL*(txPowerAt0: int, url: string): seq[uint8] {.compileTime.} =
    return ServiceData(EddystoneServiceID, concat(@[@[0x10u8, cast[uint8](txPowerAt0)], EddystoneEncodeURL url]))

func toSeqBigU8(v: uint16): seq[uint8] {.compileTime.} = @[cast[uint8](v shr 8), cast[uint8](v and 0xff)]
func toSeqBigU8(v: uint32): seq[uint8] {.compileTime.} =
    concat(@[toSeqBigU8(cast[uint16](v shr 16)), toSeqBigU8(cast[uint16](v and 0xffff))])

func EddystoneTLMUnenc*(batLevelmV, temperatureC: int; advCounter, secondCounter: uint32): seq[uint8] {.compileTime.} =
    return ServiceData(EddystoneServiceID, concat(@[@[0x20u8, 0x00],
            toSeqBigU8(cast[uint16](batLevelmV)), toSeqBigU8(cast[uint16](temperatureC)),
            toSeqBigU8(advCounter), toSeqBigU8(secondCounter)]))

macro ToArray*(args: static[openArray[seq[uint8]]]): untyped =
    result = nnkBracket.newTree
    result.add concat(args).map(newLit)
    debugEcho("Adv data dump: ", result.repr)

macro ToArray*(arg: static[openArray[uint8]]): untyped =
    result = nnkBracket.newTree
    result.add arg.map(newLit)
    debugEcho("Array data dump: ", result.repr)

var
    nextHandle {.compileTime.} : uint16  = 1

type
    HandleValue = tuple
        handle: uint16
        value: seq[uint8]
    
    NameHandleValue = tuple
        name: string
        handle: uint16
        value: seq[uint8]
        offset: int

proc addAttribute(uuid: seq[uint8]; flags: uint16; data: openArray[uint8]): HandleValue {.compileTime.} =
    let size: uint16 = cast[uint16](2 + 2 + 2 + len(uuid) + len(data))
    var value = toSeqU8 size
    var handle = nextHandle
    nextHandle += 1
    value.add toSeqU8 flags
    value.add toSeqU8 handle
    value.add uuid
    value.add data
    return (handle, value)
proc addAttribute(uuid: uint16; flags: uint16; data: openArray[uint8]): HandleValue {.compileTime.} =
    return addAttribute(uuidtoSeqU8 uuid, flags, data)

proc Service*[T](uuid: T): NameHandleValue {.compileTime.} =
    let (h, v) = addAttribute(GATT_PRIMARY_SERVICE_UUID, ATT_PROPERTY_READ, uuidToSeqU8 uuid)
    return ("", h, v, 0)

proc addCharacteristic(name: string; uuid: seq[uint8]; properties: uint16; data: openArray[uint8]): NameHandleValue {.compileTime.} =
    var buffer: seq[uint8] = @[cast[uint8](properties and 0xff)]
    let handle = nextHandle + 1
    buffer.add toSeqU8 handle
    buffer.add uuid
    var (_, data1) = addAttribute(GATT_CHARACTERISTICS_UUID, ATT_PROPERTY_READ, buffer)
    var (_, data2) = addAttribute(uuid, properties, data)
    var acc = concat(@[data1, data2])
    var offset = len(acc) - len(data)
    if (properties and (ATT_PROPERTY_NOTIFY or ATT_PROPERTY_INDICATE)) != 0:
        let flags: uint16 = ATT_PROPERTY_READ or ATT_PROPERTY_WRITE or ATT_PROPERTY_DYNAMIC
        var (_, data3) = addAttribute(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION, flags, [0u8, 0])
        acc.add data3
    (name, handle, acc, offset)

proc Characteristic*[T, T2](uuid: T; properties: uint16; data: T2; name: string = ""): NameHandleValue {.compileTime.} =
    return addCharacteristic(name, uuidToSeqU8 uuid, properties, toSeqU8 data)

proc Descriptor*[T](uuid: uint16; flags: uint16; data: T; name: string = ""): NameHandleValue {.compileTime.} =
    return addCharacteristic(name, uuidToSeqU8 uuid, flags, toSeqU8 data)

func makeIntDel(node: NimNode; name: string) {.compileTime.} =
    if name.endsWith('*'):
        var post = nnkPostfix.newTree
        post.add newIdentNode "*"
        post.add newIdentNode name[0..len(name) - 2]
        node.add post
    else:
        node.add newIdentNode name

func makeIntDef(name: string; v: NimNode): NimNode {.compileTime.} =
    result = nnkIdentDefs.newTree
    
    makeIntDel(result, name)
    result.add newEmptyNode()
    result.add v 

func makeConstDef[T](name: string; v: T): NimNode {.compileTime.} =
    result = nnkConstDef.newTree
    
    makeIntDel(result, name)
    result.add newEmptyNode()
    result.add newLit v

func makeOffstInt(name: string): string {.compileTime.} =
    return if name.endsWith('*'): name[0..len(name) - 2] & "_OFFSET*" else: name & "_OFFSET"

macro defineProfile*(items: static[openArray[NameHandleValue]]; dbName: static[string]; readonly: static[bool] = true): untyped =
    result = nnkStmtList.newTree
    var sSect = newTree (if readonly: nnkLetSection else: nnkVarSection)
    var sConst = nnkConstSection.newTree
    result.add sSect
    result.add sConst

    debugEcho "Items in profile:"
    for x in items:
        debugEcho x.repr

    var db = nnkBracket.newTree
    db.add concat(items.mapIt(it.value)).map(newLit)
    db.add @[0u8, 0].map(newLit)
    sSect.add makeIntDef(dbName, db)

    var total = 0
    for x in items:
        if x.name != "":
            sConst.add makeConstDef(x.name, x.handle)
            sConst.add makeConstDef(makeOffstInt(x.name), x.offset + total) 
        total += len(x.value)

    debugEcho result.repr
