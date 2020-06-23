## * @file                                                               *
##   @brief    helper function to cache data into flash                   *
##   @version  1.0.0.                                                     *
##   @date     2019/10/10                                                 *
##                                                                        *
##   Copyright (c) 2019 IngChips corp.                                    *
##                                                                        *
##                                                                        *
##
## ***********************************************************************************************************

## *
##  @brief Bluetooth
##  @defgroup Bluetooth_NVM
##  @ingroup bluetooth_stack
##  @{
##
## *
##  @brief A simple key-value storage
##
##  A key is just an integer and its associated value is an array of bytes.
##
##  When modified, a timer is (re-)started. When this timer timed out, whole db is committed to nvm.
##
## *
##  @brief key-collision between different modules should be strictly avoided.
##         Here is a pre-defined range for keys:
##

const
  KV_HOST_KEY_START* = 1
  KV_HOST_KEY_END* = 50
  KV_MESH_KEY_START* = 51
  KV_MESH_KEY_END* = 200
  KV_USER_KEY_START* = 201
  KV_USER_KEY_END* = 255

## *
##  @brief The whole data.
##

type
  kv_db* {.importc: "kv_db", header: "kv_storage.h", bycopy.} = object

  kv_db_t* = kv_db
  kvkey_t* = uint8

const
  KV_VALUE_MAX_LEN* = (253)

## *
##  @brief return value for callbacks
##

const
  KV_OK* = 0
  KV_ERR_OUT_OF_MEM* = 1
  KV_ERR_KEY_NOT_EXISTS* = 2
  KV_ERR_ABORT* = 3

## *
##  @brief callback function to save whole db into non-volatile memory (such as flash)
##  @param[in] db    the whole db
##  @param[in] size  total bytes that need to be saved
##

type
  f_kv_write_to_nvm* = proc (db: pointer; size: cint): cint {.noconv.}

## *
##  @brief callback function restore whole db from non-volatile memory (such as flash)
##  @param[out] db               the whole db
##  @param[in]  max_size         max_size of bytes can be restored into db
##  @return                      KV_OK or error code
##
##  Note: (sizeof(db->size) + db->size) bytes need to be read from nvm.
##        If max_size is not big enough, KV_ERR_OUT_OF_MEM should be returned.
##

type
  f_kv_read_from_nvm* = proc (db: pointer; max_size: cint): cint {.noconv.}

## *
##  @brief visitor function for each k-v pair
##  @param[in]  key              current key
##  @param[in]  data             data for current key
##  @param[in]  len              data length for current key
##  @param[in]  user_data        user data
##  @return                      KV_OK to continue visit other k-v pair, else to abor visiting
##

type
  f_kv_visitor* = proc (key: kvkey_t; data: ptr uint8; len: int16; user_data: pointer): cint {.
      noconv.}

## *
##  @brief intialize k-v storage
##  @param[in]  f_write          callback function for save whole db into non-volatile memory
##  @param[in]  f_read           callback function restore whole db from non-volatile memory
##

proc kv_init*(f_write: f_kv_write_to_nvm; f_read: f_kv_read_from_nvm) {.
    importc: "kv_init", header: "kv_storage.h".}
## *
##  @brief remove all k-v parirs
##
##  void kv_remove_all(void);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief remove a k-v pair
##

proc kv_remove*(key: kvkey_t) {.importc: "kv_remove", header: "kv_storage.h".}
## *
##  @brief put a k-v pair into the db
##  @param[in]   key             the key
##  @param[in]   data            data for the key
##  @param[in]   len             data length for the key
##
##  Note: if key does not exist, k-v pair is created; if already exists, value is updated.
##

proc kv_put*(key: kvkey_t; data: ptr uint8; len: int16): cint {.importc: "kv_put",
    header: "kv_storage.h".}
## *
##  @brief get the value associated with a key
##  @param[in]   key             the key
##  @param[out]  len             data length for the key (can be set to NULL)
##  @return                      value for the key
##
##  Note: if key does not exist, NULL is returned & len is set to 0.
##

proc kv_get*(key: kvkey_t; len: ptr int16): ptr uint8 {.importc: "kv_get",
    header: "kv_storage.h".}
## *
##  @brief notify that value of a key (got from `kv_get`) has been modified.
##
##  Note: whole db will be commited to nvm later.
##

proc kv_value_modified*() {.importc: "kv_value_modified", header: "kv_storage.h".}
## *
##  @brief commited whole db to nvm immediately
##
##  void kv_commit(void);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief traverse each k-v pair
##  @param[in]   visitor         visitor function
##  @param[in]   user_data       user data passing to visitor
##
##  Note: data should not be modified in visitor
##

proc kv_visit*(visitor: f_kv_visitor; user_data: pointer) {.importc: "kv_visit",
    header: "kv_storage.h".}
## *
##  @brief append more data to the end of an existing value
##  @param[in]   key             the key
##  @param[in]   more_data       data to be appended for the key
##  @param[in]   len             data length to be appended for the key
##  @return                      KV_OK if successful else error code
##
##  int kv_value_append(const kvkey_t key, const uint8_t *more_data, const int16_t len);
##  WARNING: ^^^ this API is not available in this release
## *
##  @brief truncate an existing value
##  @param[in]   key             the key
##  @param[in]   new_len         new length of data for the key
##  @return                      KV_OK if successful else error code
##
##  int kv_value_trunc(const kvkey_t key, const int16_t new_len);
##  WARNING: ^^^ this API is not available in this release
##
##  @}
##
