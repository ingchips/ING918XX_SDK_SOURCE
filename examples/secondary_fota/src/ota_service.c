/*
* Copyright (C) INGCHIPS. All rights reserved.
* This code is INGCHIPS proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of INGCHIPS.
*/

#include <stdio.h>
#include <string.h>
#include "ingsoc.h"

#include "att_db.h"
#include "ota_service.h"
#include "platform_api.h"
#include "rom_tools.h"

#define RTC_CHIP_STAT_ADDR  (0x40050004)
#define CLK_FREQ_STAT_POS   3

#define EFLASH_BASE        ((uint32_t)0x00004000UL)
#define EFLASH_SIZE        ((uint32_t)0x00080000UL)		//512k byte

#define EFLASH_END         (EFLASH_BASE + EFLASH_SIZE)

#include "eflash.inc"

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

extern ota_ver_t this_version;

#define ATT_OTA_HANDLE_VER          6
#define ATT_OTA_HANDLE_DATA         10
#define ATT_OTA_HANDLE_CTRL         8

uint8_t  ota_ctrl[] = {OTA_STATUS_DISABLED};
uint8_t  ota_downloading = 0;
uint32_t ota_addr = 0;
uint32_t ota_start_addr = 0;

int ota_write_callback(uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    if (transaction_mode != ATT_TRANSACTION_MODE_NONE)
    {
        platform_printf("transaction_mode: %d\n", transaction_mode);
        return 0;
    }

    if (att_handle == ATT_OTA_HANDLE_CTRL)
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
            if (ota_addr >= EFLASH_END)
            {
                *(volatile uint32_t *)(0xc40a0) = 0x4;
                EraseEFlashPage(ota_addr >= EFLASH_END + PAGE_SIZE ? 1 : 0);
                *(volatile uint32_t *)(0xc40a0) = 0x0;
            }
            else
            {
                EraseEFlashPage(((ota_addr - EFLASH_BASE) >> 13) & 0x3f);
            }
            ota_downloading = 1;
            ota_start_addr = ota_addr;
            break;
        case OTA_CTRL_PAGE_END:
            EflashProgramDisable();
            ota_downloading = 0;
            ota_addr = 0;
            {
                uint16_t len = *(uint16_t *)(buffer + 1);
                uint16_t crc_value = *(uint16_t *)(buffer + 3);
                if (ota_addr - ota_start_addr < len)
                {
                    ota_ctrl[0] = OTA_STATUS_WAIT_DATA;
                    break;
                }
                    
                if (crc((uint8_t *)ota_start_addr, len) != crc_value)
                    ota_ctrl[0] = OTA_STATUS_ERROR;
                else
                    ota_ctrl[0] = OTA_STATUS_OK;
            }
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
        case OTA_CTRL_REBOOT:
            if (OTA_STATUS_OK == ota_ctrl[0])
            {
                if (ota_downloading)
                    ota_ctrl[0] = OTA_STATUS_ERROR;
                else
                {
                    EflashCacheEna();
                    EflashCacheFlush();
                    platform_reset();
                }
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
        if (att_handle == ATT_OTA_HANDLE_CTRL)
            return 1;
        else
            return 0;
    }

    if (att_handle == ATT_OTA_HANDLE_CTRL)
    {
        buffer[0] = ota_ctrl[0];
    }

    return buffer_size;
}
