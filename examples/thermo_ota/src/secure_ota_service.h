/*
* Copyright (C) INGCHIPS. All rights reserved.
* This code is INGCHIPS proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of INGCHIPS.
*/

#ifndef _secure_ota_service_h
#define _secure_ota_service_h

#include <stdint.h>
#include "eflash.h"
#include "ota_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INGCHIPS_UUID_OTA_PUBKEY    {INGCHIPS_UUID_PART1,0xf4,INGCHIPS_UUID_PART2}

#define HASH_SIZE   32
#define SIG_SIZE    64

#pragma pack (push, 1)
typedef struct sec_ota_meta
{
    uint8_t  sig[SIG_SIZE];
    uint16_t crc_value;
    uint32_t entry;
    fota_update_block_t blocks[0];
} sec_ota_meta_t;
#pragma pack (pop)

/**
 *@brief Tell this module handles used by FOTA service
 *
 *   If FOTA related GATT service is *not* initialized by `ota_init_service`, then call this function.
 */
void secure_ota_init_handles(const uint16_t handler_ver, const uint16_t handle_ctrl, const uint16_t handle_data,
                      const uint16_t handle_pk);

void ota_connected(void);

#ifdef __cplusplus
}
#endif

#endif
