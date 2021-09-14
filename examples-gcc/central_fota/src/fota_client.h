#ifndef _fota_client_h
#define _fota_client_h

#include "ota_service.h"

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

#ifndef _INCLUDE_BY_ZIG

/**
 * @brief FOTA client.
 *
 * @param latest            latest version (a.k.a this version)
 * @param conn_handle       connection handle
 * @param handle_ver        handle of INGCHIPS_UUID_OTA_VER
 * @param handle_ctrl       handle of INGCHIPS_UUID_OTA_CTRL
 * @param handle_data       handle of INGCHIPS_UUID_OTA_DATA
 * @param item_cnt          number of binaries (<= MAX_UPDATE_BLOCKS)
 * @param items             information for each binary
 * @param entry             entry address
 * @param on_done           callback when FOTA completes (err_code != 0 if error occurs)
 * */
void fota_client_do_update(const ota_ver_t *latest, uint16_t conn_handle,
                              uint16_t handle_ver, uint16_t handle_ctrl, uint16_t handle_data,
                              int item_cnt, const ota_item_t *items,
                              uint32_t entry,
                              void (* on_done)(int err_code));

#endif

#ifdef __cplusplus
}
#endif
#endif
