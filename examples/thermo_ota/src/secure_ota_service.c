/*
* Copyright (C) INGCHIPS. All rights reserved.
* This code is INGCHIPS proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of INGCHIPS.
*/

#include <ingsoc.h>
#include <stdio.h>
#include <string.h>

#include "bluetooth.h"
#include "att_db_util.h"
#include "secure_ota_service.h"
#include "platform_api.h"
#include "rom_tools.h"

#include "ing918_uecc.h"

#define PAGE_SIZE (8192)

#define DEF_UUID(var, ID)  static uint8_t var[] = ID;

DEF_UUID(uuid_ota_service,  INGCHIPS_UUID_OTA_SERVICE);
DEF_UUID(uuid_ota_ver,      INGCHIPS_UUID_OTA_VER);
DEF_UUID(uuid_ota_data,     INGCHIPS_UUID_OTA_DATA);
DEF_UUID(uuid_ota_ctrl,     INGCHIPS_UUID_OTA_CTRL);
DEF_UUID(uuid_ota_pk,       INGCHIPS_UUID_OTA_PUBKEY);

uint16_t att_ota_ver_handle = 0;
uint16_t att_ota_data_handle = 0;
uint16_t att_ota_ctrl_handle = 0;
uint16_t att_ota_pk_handle = 0;

#define ATT_OTA_HANDLE_VER          att_ota_ver_handle
#define ATT_OTA_HANDLE_DATA         att_ota_data_handle
#define ATT_OTA_HANDLE_CTRL         att_ota_ctrl_handle
#define ATT_OTA_HANDLE_PK           att_ota_pk_handle

static uint8_t  ota_ctrl[] = {OTA_STATUS_DISABLED};
static uint8_t  ota_downloading = 0;
static uint32_t ota_start_addr = 0;
static uint32_t ota_page_offset = 0;
static uint8_t  page_buffer[PAGE_SIZE];

typedef struct
{
    uint8_t sk[32];
    uint8_t pk[64];
    uint8_t peer_pk[64];
    uint8_t dh_sk[32];
    uint8_t xor_sk[32];
} ecc_keys_t;

#define CURVE   uECC_secp256r1()

const static uint8_t root_pk[64] = 
{
    0x14, 0x1b, 0x0b, 0x28, 0x46, 0xc4, 0xaf, 0x97, 0x41, 0x59, 0x97, \
    0x4f, 0x17, 0x52, 0xe0, 0x1c, 0x9a, 0xea, 0x21, 0xc7, 0xc6, 0xe3, \
    0x04, 0x30, 0x4f, 0x8d, 0x9c, 0xf0, 0x7f, 0x1d, 0x1f, 0x0a, 0x83, \
    0xaf, 0x76, 0xe0, 0x4d, 0xc1, 0xcc, 0x96, 0xb4, 0xb8, 0x3f, 0xbb, \
    0x73, 0x6c, 0x66, 0x3f, 0x0b, 0xdf, 0x52, 0x86, 0xbf, 0x60, 0xe8, \
    0x91, 0x27, 0x00, 0x85, 0xc8, 0xbf, 0x55, 0xa8, 0x96
};

static ecc_keys_t session_keys;

void print_hex(const char *str, const uint8_t *data, int len)
{
    int i;
    platform_printf("%s:\n", str);
    for (i = 0; i < len; i++)
    {
        platform_printf("0x%02X, ", data[i]);
        if (7 == (i & 7))
            platform_printf("\n");
    }
}

void ota_init_service()
{
    uECC_make_key(session_keys.pk, session_keys.sk, CURVE);

    att_db_util_add_service_uuid128(uuid_ota_service);
    att_ota_ver_handle = att_db_util_add_characteristic_uuid128(uuid_ota_ver,
        ATT_PROPERTY_READ | ATT_PROPERTY_DYNAMIC, NULL, 0);
    att_ota_data_handle = att_db_util_add_characteristic_uuid128(uuid_ota_data,
        ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC, NULL, 0);
    att_ota_ctrl_handle = att_db_util_add_characteristic_uuid128(uuid_ota_ctrl,
        ATT_PROPERTY_READ | ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC, ota_ctrl, sizeof(ota_ctrl));
    att_ota_pk_handle = att_db_util_add_characteristic_uuid128(uuid_ota_pk,
        ATT_PROPERTY_READ | ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC, NULL, 0);
}

void ota_init_handles(const uint16_t handler_ver, const uint16_t handle_ctrl, const uint16_t handle_data,
    const uint16_t handle_pk)
{
    att_ota_ver_handle = handler_ver;
    att_ota_data_handle = handle_data;
    att_ota_ctrl_handle = handle_ctrl;
    att_ota_pk_handle = handle_pk;
}

int uecc_verify_pk(const uint8_t *data, int len, const uint8_t *sig, const uint8_t *pk)
{
    static uint8_t hash[32];
    calc_sha_256(hash, data, len);
    return uECC_verify(pk,
                hash,
                sizeof(hash),
                sig,
                CURVE) ? 0 : 1;
}

int decrypt_and_verify(uint8_t *buffer, int len, uint16_t crc_value, const uint8_t *sig)
{
    int i;
    
    if (crc(buffer, len) != crc_value) return 1;
    
    for (i = 0; i < len; i++)
        buffer[i] ^= session_keys.xor_sk[i & 31];
    
    return uecc_verify_pk(buffer, len, sig, session_keys.peer_pk);
}

int ota_write_callback(uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    if (transaction_mode != ATT_TRANSACTION_MODE_NONE)
    {
        printf("transaction_mode: %d\n", transaction_mode);
        return 0;
    }

    if (att_handle == ATT_OTA_HANDLE_CTRL)
    {
        if (OTA_CTRL_START == buffer[0])
        {
            ota_ctrl[0] = OTA_STATUS_OK;
            ota_start_addr = 0;
            ota_downloading = 0;
            return 0;
        }

        switch (buffer[0])
        {
        case OTA_CTRL_PAGE_BEGIN:
            ota_start_addr = *(uint32_t *)(buffer + 1);
            if (ota_start_addr & 0x3)
            {
                ota_ctrl[0] = OTA_STATUS_ERROR;
                return 0;
            }
            else
                ota_ctrl[0] = OTA_STATUS_OK;
            ota_downloading = 1;
            ota_page_offset = 0;
            break;
        case OTA_CTRL_PAGE_END:
            {
                uint16_t len = *(uint16_t *)(buffer + 1);
                uint16_t crc_value = *(uint16_t *)(buffer + 3);
                const uint8_t *sig = buffer + 5;
                if (ota_page_offset < len)
                {
                    ota_ctrl[0] = OTA_STATUS_WAIT_DATA;
                    break;
                }
                
                if (decrypt_and_verify(page_buffer, len, crc_value, sig))
                {
                    ota_ctrl[0] = OTA_STATUS_ERROR;
                    break;
                }
                else
                    ota_ctrl[0] = OTA_STATUS_OK;

                program_flash(ota_start_addr, page_buffer, ota_page_offset);

                ota_downloading = 0;
            }
            break;
        case OTA_CTRL_READ_PAGE:
            if (ota_downloading)
                ota_ctrl[0] = OTA_STATUS_ERROR;
            else
            {
                ota_start_addr = *(uint32_t *)(buffer + 1);
                ota_ctrl[0] = OTA_STATUS_OK;
            }
            break;
        case OTA_CTRL_METADATA:
            if (OTA_STATUS_OK != ota_ctrl[0])
                break;
            if ((0 == ota_downloading) || (buffer_size < 1 + SIG_SIZE + sizeof(ota_meta_t)))
            {
                const ota_meta_t *meta = (const ota_meta_t *)(buffer + 1);
                int s = buffer + buffer_size - (uint8_t *)&meta->entry;
                if (decrypt_and_verify((uint8_t *)&meta->entry,
                                       s,
                                       meta->crc_value,
                                       meta->sig))
                {
                    ota_ctrl[0] = OTA_STATUS_ERROR;
                    break;
                }
                program_fota_metadata(meta->entry, 
                                      (s - sizeof(meta->entry)) / sizeof(meta->blocks[0]),
                                      meta->blocks);
            }
            else
            {
                ota_ctrl[0] = OTA_STATUS_ERROR;
            }
            break;
        case OTA_CTRL_REBOOT:
            if (OTA_STATUS_OK == ota_ctrl[0])
            {
                if (ota_downloading)
                    ota_ctrl[0] = OTA_STATUS_ERROR;
                else
                    platform_reset();
            }
            break;
        default:
            ota_ctrl[0] = OTA_STATUS_ERROR;
        }
    }
    else if (att_handle == ATT_OTA_HANDLE_DATA)
    {
        if (OTA_STATUS_OK == ota_ctrl[0])
        {
            if (   (buffer_size & 0x3) || (0 == ota_downloading)
                || (ota_page_offset + buffer_size > PAGE_SIZE))
            {
                ota_ctrl[0] = OTA_STATUS_ERROR;
                return 0;
            }
            
            memcpy(page_buffer + ota_page_offset,
                   buffer, buffer_size);
            ota_page_offset += buffer_size;
        }
    }
    else if (att_handle == ATT_OTA_HANDLE_PK)
    {
        if ((buffer_size != sizeof(session_keys.pk) * 2)
            || (uECC_valid_public_key(buffer, CURVE) == 0)
            || (uecc_verify_pk(buffer, sizeof(session_keys.pk), buffer + sizeof(session_keys.pk),
                               root_pk)))
        {
            ota_ctrl[0] = OTA_STATUS_ERROR;
            return 0;
        }        
        memcpy(session_keys.peer_pk, buffer, sizeof(session_keys.peer_pk));
        uECC_shared_secret(buffer, session_keys.sk, session_keys.dh_sk, CURVE);
        calc_sha_256(session_keys.xor_sk, session_keys.dh_sk, sizeof(session_keys.dh_sk));
    }
    else;

    return 0;
}

int ota_read_callback(uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    extern prog_ver_t prog_ver;

    if (buffer == NULL)
    {
        if (att_handle == ATT_OTA_HANDLE_CTRL)
            return 1;
        else if (att_handle == ATT_OTA_HANDLE_VER)
            return sizeof(ota_ver_t);
        else if (att_handle == ATT_OTA_HANDLE_PK)
            return sizeof(session_keys.pk);
        else
            return 0;
    }

    if (att_handle == ATT_OTA_HANDLE_CTRL)
    {
        buffer[0] = ota_ctrl[0];
    }
    else if (att_handle == ATT_OTA_HANDLE_VER)
    {
        ota_ver_t *this_version = (ota_ver_t *)buffer;
        const platform_ver_t * v = platform_get_version();

        this_version->platform.major = v->major;
        this_version->platform.minor = v->minor;
        this_version->platform.patch = v->patch;
        this_version->app = prog_ver;
    }
    else if (att_handle == ATT_OTA_HANDLE_PK)
    {
        memcpy(buffer, session_keys.pk, sizeof(session_keys.pk));
    }

    return buffer_size;
}
