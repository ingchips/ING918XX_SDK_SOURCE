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
#define    DEF_UPDATE_FLAG     (0x5A5A5A5A)

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

typedef void (*rom_FlashWaitBusyDown)(void);
typedef void (*rom_FlashDisableContinuousMode)(void);
typedef void (*rom_FlashEnableContinuousMode)(void);
#define ROM_FlashWaitBusyDown           ((rom_FlashWaitBusyDown)          (0x00000b6d))
#define ROM_FlashDisableContinuousMode  ((rom_FlashDisableContinuousMode) (0x000007c9))
#define ROM_FlashEnableContinuousMode   ((rom_FlashEnableContinuousMode)  (0x0000080d))

int erase_flash_sector(const uint32_t addr)
{
    uint32_t val = (uint32_t)-1;
    return program_flash(addr, (const uint8_t *)&val, sizeof(val));
}

int program_flash(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    return ROM_program_flash(dest_addr, buffer, size);
}

int write_flash(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    uint32_t next_page = (dest_addr & (~((uint32_t)EFLASH_PAGE_SIZE - 1))) + EFLASH_PAGE_SIZE;
    while (size > 0)
    {
        uint32_t block = next_page - dest_addr;
        if (block >= size) block = size;
        int r = ROM_write_flash(dest_addr, buffer, block);
        if (r) return r;
        dest_addr += block;
        buffer += block;
        size -= block;
        next_page += EFLASH_PAGE_SIZE;
    }
    return 0;
}

int flash_do_update(const int block_num, const fota_update_block_t *blocks, uint8_t *page_buffer)
{
    return ROM_flash_do_update(block_num, blocks, page_buffer);
}

//#define APB_SPI_FLASH_CTRL_BASE              ((uint32_t)0x40150000UL)
//uint32_t security_page_read(uint32_t addr)
//{
//    io_write(APB_SPI_FLASH_CTRL_BASE + 0x14, (io_read(APB_SPI_FLASH_CTRL_BASE + 0x14) & (~(0x3<<17))) | (0x2<<17));//tx_rx_size
//    io_write(APB_SPI_FLASH_CTRL_BASE + 0x4, (io_read(APB_SPI_FLASH_CTRL_BASE + 0x4) & (~(0x1ff<<8))) | (4<<8));//tx_block_size
//    io_write(APB_SPI_FLASH_CTRL_BASE + 0x00, ((addr&0xffffff)<<8) | 0x48);
//    ROM_FlashWaitBusyDown();
//    return io_read(APB_SPI_FLASH_CTRL_BASE + 0x10);
//}

// this is compiled binary of the above `security_page_read`
static const uint8_t prog_security_page_read[] = {
    0x80, 0xB5, 0x82, 0xB0, 0x01, 0x90, 0x14, 0x21,
    0xC4, 0xF2, 0x15, 0x01, 0x08, 0x68, 0x02, 0x22,
    0x62, 0xF3, 0x52, 0x40, 0x08, 0x60, 0x04, 0x21,
    0xC4, 0xF2, 0x15, 0x01, 0x08, 0x68, 0x04, 0x22,
    0x62, 0xF3, 0x10, 0x20, 0x08, 0x60, 0x01, 0x98,
    0x00, 0x02, 0x48, 0x30, 0x00, 0x21, 0xC4, 0xF2,
    0x15, 0x01, 0x08, 0x60, 0x40, 0xF6, 0x6D, 0x30,
    0x80, 0x47, 0x10, 0x20, 0xC4, 0xF2, 0x15, 0x00,
    0x00, 0x68, 0x02, 0xB0, 0x80, 0xBD, 0x00, 0x00
};


#if ((defined __ARMCC_VERSION) && (__ARMCC_VERSION < 6000000))
asm static uint32_t security_page_read(uint32_t addr, uint32_t prog)
{
    ADD r1, r1, #1
    BX  r1
}
#else
__attribute__((naked)) static uint32_t security_page_read(uint32_t addr, uint32_t prog)
{
    __asm("ADD r1, r1, #1");
    __asm("BX  r1");
}
#endif

uint32_t read_flash_security(uint32_t addr)
{
    uint32_t ret;
    uint32_t prog[sizeof(prog_security_page_read) / 4];
    memcpy(prog, prog_security_page_read, sizeof(prog));
    ROM_FlashDisableContinuousMode();
    ret = security_page_read(addr, (uint32_t)prog);
    ROM_FlashEnableContinuousMode();
    return ret;
}

#define FACTORY_DATA_LOC    (0x02000000 + 0x1000)

#define MAGIC_0             0x494e4743
#define MAGIC_1             0x48495053

#pragma pack (push, 1)
typedef struct
{
    die_info_t die_info;
    factory_calib_data_t calib;
} factory_data_t;
#pragma pack (pop)

static int is_data_ready(void)
{
    const die_info_t *p = (const die_info_t *)FACTORY_DATA_LOC;
    if ((p->cid[0] == MAGIC_0) && (p->cid[1] == MAGIC_1))
        return 1;
    else
        return 0;
}

static void copy_security_data(uint32_t dst, uintptr_t src, int word_len)
{
    uint32_t buff[16];
    while (word_len > 0)
    {
        int len = 0;
        while ((word_len > 0) && (len < sizeof(buff) / sizeof(buff[0])))
        {
            buff[len] = read_flash_security(src);
            len++;
            word_len--;
            src += 4;
        }
        write_flash(dst, (const uint8_t *)buff, len * sizeof(buff[0]));
        dst += len * sizeof(buff[0]);
    }
}

int flash_prepare_factory_data(void)
{
    if (is_data_ready()) return 0;
    uint32_t t = read_flash_security(0x1000);
    if (t != MAGIC_0) return 1;
    t = read_flash_security(0x1004);
    if (t != MAGIC_1) return 2;
    erase_flash_sector(FACTORY_DATA_LOC);
    copy_security_data(FACTORY_DATA_LOC,
        0x1000, sizeof(die_info_t) / 4);
    copy_security_data(FACTORY_DATA_LOC + sizeof(die_info_t),
        0x1100, sizeof(factory_calib_data_t) / 4);
    copy_security_data(FACTORY_DATA_LOC + sizeof(factory_data_t),
        0x2000, 320 / 4);
    return 0;
}

const die_info_t *flash_get_die_info(void)
{
    if (is_data_ready())
        return &((const factory_data_t *)FACTORY_DATA_LOC)->die_info;
    else
        return NULL;
}

const factory_calib_data_t *flash_get_factory_calib_data(void)
{
    if (is_data_ready())
        return &((const factory_data_t *)FACTORY_DATA_LOC)->calib;
    else
        return NULL;
}

const void *flash_get_adc_calib_data(void)
{
    if (is_data_ready())
        return (const void *)(FACTORY_DATA_LOC + sizeof(factory_data_t));
    else
        return NULL;
}

#endif
