/** @file                                                               *
*  @brief    helper function to cache data into flash                   *
*  @version  1.0.0.                                                     *
*  @date     2019/10/10                                                 *
*                                                                       *
*  Copyright (c) 2019 IngChips corp.                                    *
*                                                                       *
*                                                                       *
 *************************************************************************************************************/

#ifndef _KV_STORAGE_H
#define _KV_STORAGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bluetooth
 * @defgroup Bluetooth_NVM
 * @ingroup bluetooth_stack
 * @{
 */
/**
 * @brief A simple key-value storage
 *
 * A key is just an integer and its associated value is an array of bytes.
 *
 * When modified, a timer is (re-)started. When this timer timed out, whole db is committed to nvm.
 */


/**
 * @brief key-collision between different modules should be strictly avoided.
 *        Here is a pre-defined range for keys:
 */
#define KV_HOST_KEY_START              1
#define KV_HOST_KEY_END                50
#define KV_MESH_KEY_START              51
#define KV_MESH_KEY_END                200
#define KV_USER_KEY_START              201
#define KV_USER_KEY_END                255

#ifndef DB_CAPACITY_SIZE
    #define DB_CAPACITY_SIZE 1024
#endif

/**
 * @brief The whole data.
 */
struct kv_db;
typedef struct kv_db kv_db_t;

typedef uint8_t kvkey_t;

#define KV_VALUE_MAX_LEN            (253)

/**
 * @brief return value for callbacks
 */
#define KV_OK                           0
#define KV_ERR_OUT_OF_MEM               1
#define KV_ERR_KEY_NOT_EXISTS           2
#define KV_ERR_ABORT                    3

/**
 * @brief callback function to save whole db into non-volatile memory (such as flash)
 * @param[in] db    the whole db
 * @param[in] size  total bytes that need to be saved
 */
typedef int (*f_kv_write_to_nvm)(const void *db, const int size);

/**
 * @brief callback function restore whole db from non-volatile memory (such as flash)
 * @param[out] db               the whole db
 * @param[in]  max_size         max_size of bytes can be restored into db
 * @return                      KV_OK or error code
 *
 * Note: (sizeof(db->size) + db->size) bytes need to be read from nvm.
 *       If max_size is not big enough, KV_ERR_OUT_OF_MEM should be returned.
 */
typedef int (*f_kv_read_from_nvm)(void *db, const int max_size);

/**
 * @brief visitor function for each k-v pair
 * @param[in]  key              current key
 * @param[in]  data             data for current key
 * @param[in]  len              data length for current key
 * @param[in]  user_data        user data
 * @return                      KV_OK to continue visit other k-v pair, else to abor visiting
 */
typedef int (*f_kv_visitor)(const kvkey_t key, const uint8_t *data, const int16_t len, void *user_data);

/**
 * @brief intialize k-v storage
 * @param[in]  f_write          callback function for save whole db into non-volatile memory
 * @param[in]  f_read           callback function restore whole db from non-volatile memory
 */
void kv_init(f_kv_write_to_nvm f_write,
             f_kv_read_from_nvm f_read);

/**
 * @brief remove all k-v parirs
 */
void kv_remove_all(void);

/**
 * @brief remove a k-v pair
 */
void kv_remove(const kvkey_t key);

/**
 * @brief put a k-v pair into the db
 * @param[in]   key             the key
 * @param[in]   data            data for the key
 * @param[in]   len             data length for the key
 *
 * Note: if key does not exist, k-v pair is created; if already exists, value is updated.
 */
int kv_put(const kvkey_t key, const uint8_t *data, int16_t len);

/**
 * @brief get the value associated with a key
 * @param[in]   key             the key
 * @param[out]  len             data length for the key (can be set to NULL)
 * @return                      value for the key
 *
 * Note: if key does not exist, NULL is returned & len is set to 0.
 */
uint8_t *kv_get(const kvkey_t key, int16_t *len);

/**
 * @brief notify that value of a key (got from `kv_get`) has been modified.
 *
 * Note: whole db will be commited to nvm later.
 */
void kv_value_modified(void);

/**
 * @brief commited whole db to nvm immediately
 * @param[in]   flag_always     if signaled, always write to nvm no matter if modified or not.
 *                              Otherwise, do not write to nvm if not modified.
 */
void kv_commit(int flag_always_write);

/**
 * @brief traverse each k-v pair
 * @param[in]   visitor         visitor function
 * @param[in]   user_data       user data passing to visitor
 *
 * Note: data should not be modified in visitor
 */
void kv_visit(f_kv_visitor visitor, void *user_data);

/**
 * @brief append more data to the end of an existing value
 * @param[in]   key             the key
 * @param[in]   more_data       data to be appended for the key
 * @param[in]   len             data length to be appended for the key
 * @return                      KV_OK if successful else error code
 */
// int kv_value_append(const kvkey_t key, const uint8_t *more_data, const int16_t len);
// WARNING: ^^^ this API is not available in this release


/**
 * @brief truncate an existing value
 * @param[in]   key             the key
 * @param[in]   new_len         new length of data for the key
 * @return                      KV_OK if successful else error code
 */
// int kv_value_trunc(const kvkey_t key, const int16_t new_len);
// WARNING: ^^^ this API is not available in this release


/*
* @}
*/

#ifdef __cplusplus
}
#endif

#endif
