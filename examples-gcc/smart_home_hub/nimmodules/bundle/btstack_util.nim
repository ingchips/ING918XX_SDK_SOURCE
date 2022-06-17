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
##
##  ----------------------------------------------------------------------------

import
  bluetooth

template read_little_endian_t*(T, buffer, position: untyped): untyped =
  (cast[ptr T]((cast[uint32]((buffer)) + (position)))[])

template little_endian_store_t*(T, buffer, position, v: untyped): untyped =
  (cast[ptr T]((cast[uint32]((buffer)) + (position)))[] = (v))

## basic utility:

template little_endian_read_16*(buffer, position: untyped): untyped =
  read_little_endian_t(uint16, buffer, position)

template little_endian_read_24*(buffer, position: untyped): untyped =
  (little_endian_read_32(buffer, position) and 0x00FFFFFF)

template little_endian_read_32*(buffer, position: untyped): untyped =
  read_little_endian_t(uint32, buffer, position)

template little_endian_store_16*(buffer, position, value: untyped): untyped =
  little_endian_store_t(uint16, buffer, position, value)

template little_endian_store_32*(buffer, position, value: untyped): untyped =
  little_endian_store_t(uint32, buffer, position, value)

## *
##  @brief Read 16/24/32 bit big endian value from buffer
##  @param buffer
##  @param position in buffer
##  @return value
##

proc big_endian_read_16*(buffer: ptr uint8; pos: cint): uint32 {.
    importc: "big_endian_read_16", header: "btstack_util.h".}
proc big_endian_read_32*(buffer: ptr uint8; pos: cint): uint32 {.
    importc: "big_endian_read_32", header: "btstack_util.h".}
## *
##  @brief Write 16/32 bit big endian value into buffer
##  @param buffer
##  @param position in buffer
##  @param value
##

proc big_endian_store_16*(buffer: ptr uint8; pos: uint16; value: uint16) {.
    importc: "big_endian_store_16", header: "btstack_util.h".}
proc big_endian_store_32*(buffer: ptr uint8; pos: uint16; value: uint32) {.
    importc: "big_endian_store_32", header: "btstack_util.h".}
## *
##  @brief Copy from source to destination and reverse byte order
##  @param src
##  @param dest
##  @param len
##

proc reverse_bytes*(src: ptr uint8; dest: ptr uint8; len: cint) {.
    importc: "reverse_bytes", header: "btstack_util.h".}
## *
##  @brief Wrapper around reverse_bytes for common buffer sizes
##  @param src
##  @param dest
##

proc reverse_24*(src: ptr uint8; dest: ptr uint8) {.importc: "reverse_24",
    header: "btstack_util.h".}
proc reverse_48*(src: ptr uint8; dest: ptr uint8) {.importc: "reverse_48",
    header: "btstack_util.h".}
proc reverse_56*(src: ptr uint8; dest: ptr uint8) {.importc: "reverse_56",
    header: "btstack_util.h".}
proc reverse_64*(src: ptr uint8; dest: ptr uint8) {.importc: "reverse_64",
    header: "btstack_util.h".}
proc reverse_128*(src: ptr uint8; dest: ptr uint8) {.importc: "reverse_128",
    header: "btstack_util.h".}
proc reverse_bd_addr*(src: ptr uint8; dest: ptr uint8) {.importc: "reverse_bd_addr",
    header: "btstack_util.h".}
##  will be moved to daemon/btstack_device_name_db.h
## *
##  @brief The device name type
##

const
  DEVICE_NAME_LEN* = 248

type
  device_name_t* = array[DEVICE_NAME_LEN + 1, uint8]

## *
##  @brief ASCII character for 4-bit nibble
##  @return character
##

proc char_for_nibble*(nibble: cint): char {.importc: "char_for_nibble",
                                        header: "btstack_util.h".}
## *
##  @brif 4-bit nibble from ASCII character
##  @return value
##

proc nibble_for_char*(c: char): cint {.importc: "nibble_for_char",
                                   header: "btstack_util.h".}
## *
##  @brief Compare two Bluetooth addresses
##  @param a
##  @param b
##  @return true if equal
##

proc bd_addr_cmp*(a: bd_addr_t; b: bd_addr_t): cint {.importc: "bd_addr_cmp",
    header: "btstack_util.h".}
## *
##  @brief Copy Bluetooth address
##  @param dest
##  @param src
##

proc bd_addr_copy*(dest: bd_addr_t; src: bd_addr_t) {.importc: "bd_addr_copy",
    header: "btstack_util.h".}
## *
##  @brief Use printf to write hexdump as single line of data
##

proc printf_hexdump*(data: pointer; size: cint) {.importc: "printf_hexdump",
    header: "btstack_util.h".}
## *
##  @brief Create human readable representation for UUID128
##  @note uses fixed global buffer
##  @return pointer to UUID128 string
##

proc uuid128_to_str*(uuid: ptr uint8): cstring {.importc: "uuid128_to_str",
    header: "btstack_util.h".}
## *
##  @brief Create human readable represenationt of Bluetooth address
##  @note uses fixed global buffer
##  @return pointer to Bluetooth address string
##

proc bd_addr_to_str*(`addr`: bd_addr_t): cstring {.importc: "bd_addr_to_str",
    header: "btstack_util.h".}
## *
##  @brief Parse Bluetooth address
##  @param address_string
##  @param buffer for parsed address
##  @return 1 if string was parsed successfully
##

proc sscanf_bd_addr*(addr_string: cstring; `addr`: bd_addr_t): cint {.
    importc: "sscanf_bd_addr", header: "btstack_util.h".}
## *
##  @brief Constructs UUID128 from 16 or 32 bit UUID using Bluetooth base UUID
##  @param uuid128 output buffer
##  @param short_uuid
##

proc uuid_add_bluetooth_prefix*(uuid128: ptr uint8; short_uuid: uint32) {.
    importc: "uuid_add_bluetooth_prefix", header: "btstack_util.h".}
## *
##  @brief Checks if UUID128 has Bluetooth base UUID prefix
##  @param uui128 to test
##  @return 1 if it can be expressed as UUID32
##

proc uuid_has_bluetooth_prefix*(uuid128: ptr uint8): cint {.
    importc: "uuid_has_bluetooth_prefix", header: "btstack_util.h".}