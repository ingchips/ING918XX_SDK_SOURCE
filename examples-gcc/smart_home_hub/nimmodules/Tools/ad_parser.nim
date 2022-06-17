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

##  API_START

type
  ad_context_t* {.importc: "ad_context_t", header: "ad_parser.h", bycopy.} = object
    data* {.importc: "data".}: ptr uint8
    offset* {.importc: "offset".}: uint16
    length* {.importc: "length".}: uint16


##  Advertising or Scan Response data iterator

proc ad_iterator_init*(context: ptr ad_context_t; ad_len: uint16; ad_data: ptr uint8) {.
    importc: "ad_iterator_init", header: "ad_parser.h".}
proc ad_iterator_has_more*(context: ptr ad_context_t): cint {.
    importc: "ad_iterator_has_more", header: "ad_parser.h".}
proc ad_iterator_next*(context: ptr ad_context_t) {.importc: "ad_iterator_next",
    header: "ad_parser.h".}
##  Access functions

proc ad_iterator_get_data_type*(context: ptr ad_context_t): uint8 {.
    importc: "ad_iterator_get_data_type", header: "ad_parser.h".}
proc ad_iterator_get_data_len*(context: ptr ad_context_t): uint8 {.
    importc: "ad_iterator_get_data_len", header: "ad_parser.h".}
proc ad_iterator_get_data*(context: ptr ad_context_t): ptr uint8 {.
    importc: "ad_iterator_get_data", header: "ad_parser.h".}
##  convenience function on complete advertisements

proc ad_data_contains_uuid16*(ad_len: uint16; ad_data: ptr uint8; uuid: uint16): cint {.
    importc: "ad_data_contains_uuid16", header: "ad_parser.h".}
proc ad_data_contains_uuid128*(ad_len: uint16; ad_data: ptr uint8; uuid128: ptr uint8): cint {.
    importc: "ad_data_contains_uuid128", header: "ad_parser.h".}
##  find data with type "ad_type"
##  output: found_length: data length of "ad_type", 0 if not found
##  return: pointer to the start of data within "ad_data", NULL if not found

proc ad_data_from_type*(ad_len: uint16; ad_data: ptr uint8; ad_type: uint8;
                       found_length: ptr uint16): ptr uint8 {.
    importc: "ad_data_from_type", header: "ad_parser.h".}
##  API_END
