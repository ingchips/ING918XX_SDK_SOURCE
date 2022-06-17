##  API_START
## *
##  @brief Init ATT DB storage
##

proc att_db_util_init*(att_db_storage: ptr uint8; db_max_size: int16) {.
    importc: "att_db_util_init", header: "att_db_util.h".}
## *
##  @brief Add primary service for 16-bit UUID
##

proc att_db_util_add_service_uuid16*(uuid16: uint16) {.
    importc: "att_db_util_add_service_uuid16", header: "att_db_util.h".}
## *
##  @brief Add primary service for 128-bit UUID
##

proc att_db_util_add_service_uuid128*(uuid128: ptr uint8) {.
    importc: "att_db_util_add_service_uuid128", header: "att_db_util.h".}
## *
##  @brief Add Characteristic with 16-bit UUID, properties, and data
##  @returns attribute value handle
##  @see ATT_PROPERTY_* in ble/att_db.h
##

proc att_db_util_add_characteristic_uuid16*(uuid16: uint16; properties: uint16;
    data: ptr uint8; data_len: uint16): uint16 {.
    importc: "att_db_util_add_characteristic_uuid16", header: "att_db_util.h".}
proc att_db_util_add_descriptor_uuid16*(uuid16: uint16; flags: uint16;
                                       data: ptr uint8; data_len: uint16) {.
    importc: "att_db_util_add_descriptor_uuid16", header: "att_db_util.h".}
## *
##  @brief Add Characteristic with 128-bit UUID, properties, and data
##  @returns attribute value handle
##  @see ATT_PROPERTY_* in ble/att_db.h
##

proc att_db_util_add_characteristic_uuid128*(uuid128: ptr uint8; properties: uint16;
    data: ptr uint8; data_len: uint16): uint16 {.
    importc: "att_db_util_add_characteristic_uuid128", header: "att_db_util.h".}
## *
##  @brief Get address of constructed ATT DB
##

proc att_db_util_get_address*(): ptr uint8 {.importc: "att_db_util_get_address",
    header: "att_db_util.h".}
## *
##  @brief Get size of constructed ATT DB
##

proc att_db_util_get_size*(): uint16 {.importc: "att_db_util_get_size",
                                    header: "att_db_util.h".}
##  API_END
