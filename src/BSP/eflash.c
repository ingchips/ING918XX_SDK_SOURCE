#include <stdint.h>
#include <string.h>

static uint32_t ClkFreq; //0:16M 1:24M

#define PAGE_SIZE (8192)
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

