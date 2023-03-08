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

int erase_info_page(const int index)
{
    init();
    EflashProgramEnable();

    *(volatile uint32_t *)(0xc40a0) = 0x4;
    EraseEFlashPage(index & 1);
    *(volatile uint32_t *)(0xc40a0) = 0x0;

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

typedef enum {
    SPI_CMD_ADDR         = 0x0,
    SPI_BLOCK_SIZE       = 0x4,
    RX_STATUS            = 0x10,
    SPI_CFG              = 0x14,
} SPI_FLASH_Reg;

typedef void (*rom_FlashWaitBusyDown)(void);
typedef void (*rom_FlashDisableContinuousMode)(void);
typedef void (*rom_FlashEnableContinuousMode)(void);
#define ROM_FlashWaitBusyDown           ((rom_FlashWaitBusyDown)          (0x00000b6d))
#define ROM_FlashDisableContinuousMode  ((rom_FlashDisableContinuousMode) (0x000007c9))
#define ROM_FlashEnableContinuousMode   ((rom_FlashEnableContinuousMode)  (0x0000080d))

#define APB_SPI_FLASH_CTRL_BASE              ((uint32_t)0x40150000UL)
#define SPI_FLASH_LEFT_SHIFT(v, s)           ((v) << (s))
#define SPI_FLASH_RIGHT_SHIFT(v, s)          ((v) >> (s))
#define SPI_FLASH_MK_MASK(b)                 ((SPI_FLASH_LEFT_SHIFT(1, b)) - (1))
#define SPI_FLASH_REG_VAL(reg)               ((*((uint32_t *)((APB_SPI_FLASH_CTRL_BASE) + (reg)))))
#define REG_CLR(reg, b, s)                   ((SPI_FLASH_REG_VAL(reg)) & (~(SPI_FLASH_LEFT_SHIFT(SPI_FLASH_MK_MASK(b), s))))
#define SPI_FLASH_REG_WR_BITS(reg, v, s, b)  ((SPI_FLASH_REG_VAL(reg)) = ((REG_CLR(reg, b, s)) | (SPI_FLASH_LEFT_SHIFT(v, s))))
#define SPI_FLASH_REG_RD(reg, b, s)          ((SPI_FLASH_RIGHT_SHIFT((SPI_FLASH_REG_VAL(reg)), s)) & SPI_FLASH_MK_MASK(b))
static void SPI_FLASH_RegWrBits(SPI_FLASH_Reg reg, uint32_t v, uint8_t s, uint8_t b)
{
    SPI_FLASH_REG_WR_BITS(reg, v, s, b);
}
static uint32_t SPI_FLASH_RegRd(SPI_FLASH_Reg reg, uint8_t s, uint8_t b)
{
    return SPI_FLASH_REG_RD(reg, b, s);
}

int erase_flash_sector(const uint32_t addr)
{
    return ROM_erase_flash_sector(addr);
}

int program_flash(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    return ROM_program_flash(dest_addr, buffer, size);
}

int write_flash(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    return ROM_write_flash(dest_addr, buffer, size);
}

int flash_do_update(const int block_num, const fota_update_block_t *blocks, uint8_t *page_buffer)
{
    return ROM_flash_do_update(block_num, blocks, page_buffer);
}

#define __RAM_CODE    __attribute__((section(".data")))
__RAM_CODE uint32_t security_page_read(uint32_t addr)
 {
    SPI_FLASH_RegWrBits(SPI_CFG, 2, 17, 2);
    SPI_FLASH_RegWrBits(SPI_BLOCK_SIZE, 4, 8, 9);
    SPI_FLASH_RegWrBits(SPI_CMD_ADDR, ((addr&0xffffff)<<8) | 0x48, 0, 32);
    ROM_FlashWaitBusyDown();
    return SPI_FLASH_RegRd(RX_STATUS, 0, 32);
 }
__RAM_CODE uint32_t read_flash_security(uint32_t addr)
{
    uint32_t ret;
    ROM_FlashDisableContinuousMode();
    ret = security_page_read(addr);
    ROM_FlashEnableContinuousMode();
    return ret;
}

#endif
