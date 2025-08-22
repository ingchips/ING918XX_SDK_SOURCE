#ifndef _fota_client_h
#define _fota_client_h

#include "ota_service.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_UPDATE_BLOCKS   6

typedef struct ota_item
{
    uint32_t local_storage_addr;
    uint32_t target_storage_addr;
    uint32_t target_load_addr;
    uint32_t size;
} ota_item_t;

typedef uint16_t (* f_crc_t)(uint8_t *buffer, uint16_t len);

typedef struct ecc_driver
{
    const uint8_t *root_sk;
    const uint8_t *session_sk;
    const uint8_t *session_pk;

    /**
     * @brief calculate SHA-256 of a message
     *
     * @param hash[out]         result
     * @param input             input message
     * @param len               length of input message
     */
    void (*sha_256)(uint8_t hash[32], const void *input, int len);

    /**
     * @brief Verify the signature of a message
     *
     * @param pk                public key
     * @param msg               message
     * @param msg_len           length of message
     * @param signature         signature
     * @return                  1 if the signature is valid, 0 if it is invalid.
     */
    int  (*verify)(const uint8_t *pk, const uint8_t *msg, int msg_len, const uint8_t *signature);

    /**
     * @brief Sign a message
     *
     * @param sk                private key
     * @param msg               message
     * @param msg_len           length of message
     * @param signature         signature
     */
    void (*sign)(const uint8_t *sk, const uint8_t *msg, int msg_len, uint8_t *signature);

    /**
     * @brief Valid if a key is a valid public key
     *
     * @param pk                a key to be checked
     * @return                  1 if the signature is valid, 0 if it is invalid.
     */
    int (*validate_pk)(const uint8_t *pk);

    /**
     * @brief Calculate the shared secret
     *
     * @param local_sk          local private key
     * @param peer_pk           peer public key
     * @param shared[out]       shared secret
     */
    void (*shared_secret)(const uint8_t *local_sk, const uint8_t *peer_pk, uint8_t *shared);

    /**
     * @brief memory allocation
     *
     * @param len               length in byte
     * @return                  address of allocated buffer
     */
    void *(*alloc)(size_t len);

    /**
     * @brief memory free
     *
     * @param p                 address of a previously allocated buffer
     */
    void (*free)(void *p);
} ecc_driver_t;

#ifndef _INCLUDE_BY_ZIG

/**
 * @brief FOTA client.
 *
 * @param target_family     target chip family (INGCHIPS_FAMILY_916, INGCHIPS_FAMILY_918, etc)
 * @param latest            latest version (a.k.a this version)
 * @param conn_handle       connection handle
 * @param handle_ver        handle of INGCHIPS_UUID_OTA_VER
 * @param handle_ctrl       handle of INGCHIPS_UUID_OTA_CTRL
 * @param handle_data       handle of INGCHIPS_UUID_OTA_DATA
 * @param item_cnt          number of binaries (<= MAX_UPDATE_BLOCKS)
 * @param items             information for each binary
 * @param entry             entry address
 * @param on_done           callback when FOTA completes
 *                          When err_code == 0: updated successfully
 *                               err_code == 1: nothing to update
 *                               err_code < 0 : error occurs
 * */
void fota_client_do_update(int target_chip_family,
                              const ota_ver_t *latest, uint16_t conn_handle,
                              uint16_t handle_ver, uint16_t handle_ctrl, uint16_t handle_data,
                              int item_cnt, const ota_item_t *items,
                              uint32_t entry,
                              void (* on_done)(int err_code));


/**
 * @brief FOTA client.
 *
 * @param target_family     target chip family (INGCHIPS_FAMILY_916, INGCHIPS_FAMILY_918, etc)
 * @param latest            latest version (a.k.a this version)
 * @param conn_handle       connection handle
 * @param handle_ver        handle of INGCHIPS_UUID_OTA_VER
 * @param handle_ctrl       handle of INGCHIPS_UUID_OTA_CTRL
 * @param handle_data       handle of INGCHIPS_UUID_OTA_DATA
 * @param handle_pk         handle of INGCHIPS_UUID_OTA_PUBKEY
 * @param item_cnt          number of binaries (<= MAX_UPDATE_BLOCKS)
 * @param items             information for each binary
 * @param entry             entry address
 * @param on_done           callback when FOTA completes
 *                          When err_code == 0: updated successfully
 *                               err_code == 1: nothing to update
 *                               err_code < 0 : error occurs
 * @param driver            ECC function driver
 */
void secure_fota_client_do_update(int target_chip_family,
                              const ota_ver_t *latest, uint16_t conn_handle,
                              uint16_t handle_ver, uint16_t handle_ctrl, uint16_t handle_data,
                              uint16_t handle_pk,
                              int item_cnt, const ota_item_t *items,
                              uint32_t entry,
                              void (* on_done)(int err_code),
                              const ecc_driver_t *driver);

#endif

#ifdef __cplusplus
}
#endif
#endif
