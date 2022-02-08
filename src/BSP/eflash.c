#include <stdint.h>
#include <string.h>

#include "eflash.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

static uint32_t ClkFreq; //0:16M 1:24M

#define PAGE_SIZE (EFLASH_PAGE_SIZE)
#define PAGE_SIZE_SHIFT 13

#define RTC_CHIP_STAT_ADDR (0x40050004)
#define CLK_FREQ_STAT_POS 3

#define EFLASH_BASE ((uint32_t)0x00004000UL)
#define EFLASH_SIZE ((uint32_t)0x00080000UL) //512k byte

#include "eflash.inc"

static void init(void)
{
    ClkFreq = (*(uint32_t *)RTC_CHIP_STAT_ADDR >> CLK_FREQ_STAT_POS) & 0x1;
    EflashCacheBypass();
    EflashBaseTime();
#ifdef FOR_ASIC
    EflashRepair();
#endif
}

static void uninit()
{
    EflashCacheEna();
    EflashCacheFlush();
}

int program_flash0(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size, int erase)
{
    uint32_t addr = dest_addr;
    if (erase)
    {
        if ((addr & (PAGE_SIZE - 1)) != 0)
            return 1;
    }
    else
    {
        if ((addr & 0x3) != 0)
            return 1;
    }
    if ((size < 1) || ((size & 0x3) != 0))
        return 1;

    init();
    EflashProgramEnable();

    while (size)
    {
        int i;
        uint32_t sz = size > PAGE_SIZE ? PAGE_SIZE : size;
        uint32_t *p32 = (uint32_t *)buffer;
        if (erase)
        {
            uint32_t page_idx = ((addr - EFLASH_BASE) >> PAGE_SIZE_SHIFT) & 0x3f;
            EraseEFlashPage(page_idx);
        }
        for (i = 0; i<(sz + 3)>> 2; i++)
            EflashProgram(addr + (i << 2), *p32++);

        buffer += sz;
        addr += sz;
        size -= sz;
    }
    EflashProgramDisable();

    uninit();
    return 0;
}

int erase_flash_page(const uint32_t addr)
{
    if ((addr & (PAGE_SIZE - 1)) != 0)
        return 1;

    init();
    EflashProgramEnable();

    uint32_t page_idx = ((addr - EFLASH_BASE) >> PAGE_SIZE_SHIFT) & 0x3f;
    EraseEFlashPage(page_idx);

    EflashProgramDisable();

    uninit();

    return 0;
}

int program_flash(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    return program_flash0(dest_addr, buffer, size, 1);
}

int write_flash(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    return program_flash0(dest_addr, buffer, size, 0);
}

int program_fota_metadata(const uint32_t entry, const int block_num, const fota_update_block_t *blocks)
{
#define START               (EFLASH_BASE + EFLASH_SIZE + 2 * PAGE_SIZE)
#define	DEF_UPDATE_FLAG     (0x5A5A5A5A)

    uint32_t backup[4];
    uint32_t addr = START - 4;
    int i;

    init();
    memcpy(backup, (void *)(START - sizeof(backup)), sizeof(backup));
    if (entry)
    {
        backup[1] = entry;
        backup[2] = DEF_UPDATE_FLAG;
    }

    EflashProgramEnable();

    *(volatile uint32_t *)(0xc40a0) = 0x4;
    EraseEFlashPage(1);
    *(volatile uint32_t *)(0xc40a0) = 0x0;

    for (i = sizeof(backup) / sizeof(backup[0]) - 1; i >= 0;  i--)
    {
        EflashProgram(addr, backup[i]);
        addr -= 4;
    }

    for (i = 0; i < block_num; i++)
    {
        EflashProgram(addr, DEF_UPDATE_FLAG);
        addr -= 4;
        EflashProgram(addr, blocks[i].src);
        addr -= 4;
        EflashProgram(addr, blocks[i].dest);
        addr -= 4;
        EflashProgram(addr, blocks[i].size);
        addr -= 4;
    }

    EflashProgramDisable();
    uninit();
    return 0;
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#include "rom_tools.h"

#define __RAM_CODE    __attribute__((section(".data")))

__RAM_CODE static void ram_rase_sector(uint32_t addr)
{
    FlashSectorErase(addr);
    FlashWriteDisable();
}

__RAM_CODE static void ram_prog_page(uint32_t addr, const uint8_t *data, uint32_t len)
{
    FlashPageProgram(addr, data, len);
    FlashWriteDisable();
}

int erase_flash_sector(const uint32_t addr)
{
    if (addr & (EFLASH_SECTOR_SIZE - 1)) return -1;
    ram_rase_sector(addr);
    return 0;
}

#define FLASH_PRE_OPS()                     \
    uint32_t prim = __get_PRIMASK();        \
    __disable_irq();                        \
    FlashDisableContinuousMode();

#define FLASH_POST_OPS()                    \
    DCacheFlush();                          \
    FlashEnableContinuousMode();            \
    if (!prim) __enable_irq()

int program_flash(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    if (dest_addr & (EFLASH_SECTOR_SIZE - 1)) return -1;

    FLASH_PRE_OPS();
    {
        while (size > 0)
        {
            uint32_t remain = EFLASH_SECTOR_SIZE;
            
            ram_rase_sector(dest_addr);

            while ((remain > 0) && (size > 0))
            {
                uint32_t cnt = size > EFLASH_PAGE_SIZE ? EFLASH_PAGE_SIZE : size;
                cnt = cnt > remain ? remain : cnt;
                ram_prog_page(dest_addr, buffer, cnt);
                dest_addr += cnt;
                buffer += cnt;
                remain -= cnt;
                size -= cnt;
            }
        }
    }
    FLASH_POST_OPS();

    return 0;
}

int write_flash(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    FLASH_PRE_OPS();
    {
        while (size > 0)
        {
            uint32_t cnt = size > EFLASH_PAGE_SIZE ? EFLASH_PAGE_SIZE : size;
            int i;
            for (i = 0; i < cnt; i++)
                if (*(const uint8_t *)(dest_addr + i) != 0xff) return -1;

            ram_prog_page(dest_addr, buffer, cnt);
            dest_addr += cnt;
            buffer += cnt;
            size -= cnt;
        }
    }
    FLASH_POST_OPS();

    return 0;
}

__RAM_CODE static void ram_flash_do_update(const int block_num, const fota_update_block_t *blocks,
    uint8_t *page_buffer)
{
    int i;   

    for (i = 0; i < block_num; i++)
    {
        {
            uint32_t dest_addr = blocks[i].dest;
            const uint8_t *buffer = (const uint8_t *)blocks[i].src;
            int size = blocks[i].size;

            while (size > 0)
            {
                uint32_t remain = EFLASH_SECTOR_SIZE;

                FlashSectorErase(dest_addr);
                FlashWriteDisable();

                while ((remain > 0) && (size > 0))
                {
                    int j;
                    uint32_t cnt = size > EFLASH_PAGE_SIZE ? EFLASH_PAGE_SIZE : size;
                    cnt = cnt > remain ? remain : cnt;

                    for (j = 0; j < cnt; j++)
                        page_buffer[j] = buffer[j];

                    FlashPageProgram(dest_addr, page_buffer, cnt);
                    FlashWriteDisable();

                    dest_addr += cnt;
                    buffer += cnt;
                    remain -= cnt;
                    size -= cnt;
                }
            }
        }
    }
    APB_SYSCTRL->CguCfg[1] &= ~((1ul << 14) | (1ul << 18));
    ROM_NVIC_SystemReset();
}

int flash_do_update(const int block_num, const fota_update_block_t *blocks, uint8_t *page_buffer)
{
    int i;

    if (block_num < 1) return -2;

    for (i = 0; i < block_num; i++)
    {
        if (blocks[i].dest & (EFLASH_SECTOR_SIZE - 1)) return -1;
    }

    FLASH_PRE_OPS();
    ram_flash_do_update(block_num, blocks, page_buffer);

    return 0;
}

#endif
