#include <stdint.h>

uint32_t ClkFreq; //0:16M 1:24M

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
