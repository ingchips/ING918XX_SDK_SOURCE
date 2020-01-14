/*
* Copyright (C) INGCHIPS. All rights reserved.
* This code is INGCHIPS proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of INGCHIPS.
*/

#include <cm32gpm3.h>
#include <stdio.h>
#include <string.h>

#include "bluetooth.h"
#include "att_db_util.h"
#include "ota_service.h"
#include "platform_api.h"

#define RTC_CHIP_STAT_ADDR  (0x40050004)
#define CLK_FREQ_STAT_POS   3

#define EFLASH_BASE        ((uint32_t)0x00004000UL)
#define EFLASH_SIZE        ((uint32_t)0x00080000UL)		//512k byte

#include "eflash.c"

uint32_t ClkFreq; //0:16M 1:24M

#define PAGE_SIZE (8192)

void start_eflash_prog(void)
{
		ClkFreq = (*(uint32_t *)RTC_CHIP_STAT_ADDR>>CLK_FREQ_STAT_POS)&0x1;
		EflashCacheBypass();
		EflashBaseTime();
#ifdef FOR_ASIC
		EflashRepair();
#endif
}

#define DEF_UUID(var, ID)  static uint8_t var[] = ID;

DEF_UUID(uuid_ota_service,  INGCHIPS_UUID_OTA_SERVICE);
DEF_UUID(uuid_ota_ver,      INGCHIPS_UUID_OTA_VER);
DEF_UUID(uuid_ota_data,     INGCHIPS_UUID_OTA_DATA);
DEF_UUID(uuid_ota_ctrl,     INGCHIPS_UUID_OTA_CTRL);

uint16_t att_ota_ver_handle = 0;
uint16_t att_ota_data_handle = 0;
uint16_t att_ota_ctrl_handle = 0;

uint8_t  ota_ctrl[] = {OTA_STATUS_DISABLED};
uint8_t  ota_downloading = 0;
uint32_t ota_addr = 0;
uint32_t ota_start_addr = 0;

//#define V2

ota_ver_t this_version =
{
    .app = 
#ifdef V2
        { .major = 1, .minor = 2, .patch = 0 }
#else
        { .major = 1, .minor = 1, .patch = 0 }
#endif
};

void ota_init_service(void)
{
    uint8_t ota_data_buff[20];
    const platform_ver_t *ver = platform_get_version();
    this_version.platform.major = ver->major;
    this_version.platform.minor = ver->minor;
    this_version.platform.patch = ver->patch;

    att_db_util_add_service_uuid128(uuid_ota_service);
    att_ota_ver_handle = att_db_util_add_characteristic_uuid128(uuid_ota_ver,
        ATT_PROPERTY_READ, (uint8_t *)&this_version, sizeof(this_version));
    att_ota_data_handle = att_db_util_add_characteristic_uuid128(uuid_ota_data,
        ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC, ota_data_buff, sizeof(ota_data_buff));
    att_ota_ctrl_handle = att_db_util_add_characteristic_uuid128(uuid_ota_ctrl,
        ATT_PROPERTY_READ | ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC, ota_ctrl, sizeof(ota_ctrl));

    // printf("att_ota_ver_handle   = %d\n"
    //        "att_ota_data_handle  = %d\n"
    //        "att_ota_ctrl_handle  = %d\n", att_ota_ver_handle, att_ota_data_handle, att_ota_ctrl_handle);
}

void ota_init_handles(const uint16_t handler_ver, const uint16_t handle_ctrl, const uint16_t handle_data)
{
    att_ota_ver_handle = handler_ver;
    att_ota_data_handle = handle_data;
    att_ota_ctrl_handle = handle_ctrl;
}

typedef uint16_t (* f_crc_t)(uint8_t *buffer , uint16_t usDataLen);

#define crc     ((f_crc_t)(0x000009fd))

int ota_write_callback(uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    if (transaction_mode != ATT_TRANSACTION_MODE_NONE)
    {
        printf("transaction_mode: %d\n", transaction_mode);
        return 0;
    }

    if (att_handle == att_ota_ctrl_handle)
    {
        if (OTA_CTRL_START == buffer[0])
        {
            if (OTA_STATUS_DISABLED != ota_ctrl[0])
            {
                if (ota_downloading)
                    EflashProgramDisable();
                EflashCacheEna();
                EflashCacheFlush();
            }

            start_eflash_prog();
            ota_ctrl[0] = OTA_STATUS_OK;
            ota_addr = 0;
            ota_downloading = 0;
            return 0;
        }

        switch (buffer[0])
        {
        case OTA_CTRL_PAGE_BEGIN:
            ota_addr = *(uint32_t *)(buffer + 1);
            if (ota_addr & 0x3)
            {
                ota_ctrl[0] = OTA_STATUS_ERROR;
                return 0;
            }
            else
                ota_ctrl[0] = OTA_STATUS_OK;
            EflashProgramEnable();
            EraseEFlashPage(((ota_addr - EFLASH_BASE) >> 13) & 0x3f);
            ota_downloading = 1;
            ota_start_addr = ota_addr;
            break;
        case OTA_CTRL_PAGE_END:
            EflashProgramDisable();
            ota_downloading = 0;
            ota_addr = 0;
            if (crc((uint8_t *)ota_start_addr, *(uint16_t *)(buffer + 1)) != *(uint16_t *)(buffer + 3))
                ota_ctrl[0] = OTA_STATUS_ERROR;
            break;
        case OTA_CTRL_READ_PAGE:
            if (ota_downloading)
                ota_ctrl[0] = OTA_STATUS_ERROR;
            else
            {
                ota_addr = *(uint32_t *)(buffer + 1);
                ota_ctrl[0] = OTA_STATUS_OK;
            }

            break;
        case OTA_CTRL_SWITCH_APP:
            platform_switch_app(0x80000);
            break;
        case OTA_CTRL_REBOOT:
            if (OTA_STATUS_OK == ota_ctrl[0])
            {
                if (ota_downloading)
                    ota_ctrl[0] = OTA_STATUS_ERROR;
                else
                {
                    EflashCacheEna();
                    EflashCacheFlush();
                    NVIC_SystemReset();
                    while(1);
                }
            }
            break;
        default:
            ota_ctrl[0] = OTA_STATUS_ERROR;
        }
    }
    else if (att_handle == att_ota_data_handle)
    {
        if (OTA_STATUS_OK == ota_ctrl[0])
        {
            int i;
            uint32_t *p32 = (uint32_t *)buffer;
            if ((buffer_size & 0x3) || (0 == ota_addr))
            {
                ota_ctrl[0] = OTA_STATUS_ERROR;
                return 0;
            }

            for (i = 0; i < buffer_size >> 2; i++)
            {
                EflashProgram(ota_addr, *p32);
                p32++;
                ota_addr += 4;
            }
        }
    }
    else;

    return 0;
}

int ota_read_callback(uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    if (buffer == NULL)
    {
        if (att_handle == att_ota_ver_handle)
            return sizeof(this_version);
        else if (att_handle == att_ota_ctrl_handle)
            return 1;
        else
            return 0;
    }

    if (att_handle == att_ota_ctrl_handle)
    {
        buffer[0] = ota_ctrl[0];
    }
    return buffer_size;
}

