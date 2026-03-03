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

static uint32_t prim_irq;

#define FLASH_PRE_OPS()                     \
    prim_irq = __get_PRIMASK();             \
    __disable_irq();

#define FLASH_POST_OPS()                    \
    if (!prim_irq) __enable_irq()

static void init(void)
{
    FLASH_PRE_OPS();

    ClkFreq = (*(uint32_t volatile*)RTC_CHIP_STAT_ADDR >> CLK_FREQ_STAT_POS) & 0x1;
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

    FLASH_POST_OPS();
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
    int r = program_flash0(dest_addr, buffer, size, 0);
    if (r != 0) return r;
    r = memcmp((const void *)dest_addr, buffer, size) == 0 ? 0 : 2;
    return r;
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

#define FACTORY_DATA_LOC    (0x80000 + 0x4000)
#define VERSION             0x20230817

const die_info_t *flash_get_die_info(void)
{
    return ((const die_info_t *)(FACTORY_DATA_LOC + 0x4000));
}

const factory_calib_data_t *flash_get_factory_calib_data(void)
{
    const factory_calib_data_t *p = (const factory_calib_data_t *)(FACTORY_DATA_LOC + 0x3000);

    if(p->ft_version > VERSION)
        return p;
    else
        return NULL;
}

const adc_calib_data_t *flash_get_adc_calib_data(void)
{
    return (const adc_calib_data_t *)(FACTORY_DATA_LOC);
}

void flash_read_uid(uint32_t uid[4])
{
    const die_info_t* die_info;
    const factory_calib_data_t* factory_calib;

    if(NULL == uid)
        return;

    EflashCacheBypass();
    die_info = flash_get_die_info();
    factory_calib = flash_get_factory_calib_data();
    uid[0] = die_info->lot_id[0];
    uid[1] = die_info->lot_id[1] | (die_info->metal_id<<16) | (die_info->wafer_id<<24);
    if(factory_calib)
    {
        uid[2] = die_info->Die_x_local | (die_info->Die_y_local<<8) | ((*((uint32_t*)(&factory_calib->TRng_data[0]))) << 16);
        uid[3] = *((uint32_t*)(&factory_calib->TRng_data[2]));
    }
    else
    {
        uid[2] = die_info->Die_x_local | (die_info->Die_y_local<<8);
        uid[3] = 0;
    }
    EflashCacheEna();
}

int flash_read_uid45(uint8_t uid[6])
{
    const die_info_t* pDie_info;

    if(NULL == uid)
        return -1;

    EflashCacheBypass();
    pDie_info = flash_get_die_info();
    uint8_t *pLot = (uint8_t*)&pDie_info->lot_id[0];
    uid[0] = pDie_info->wafer_id&0x1f;
    uid[1] = pLot[1];
    uid[2] = pLot[3];
    uid[3] = pLot[5];
    uid[4] = pDie_info->Die_x_local;
    uid[5] = pDie_info->Die_y_local;
    EflashCacheEna();

    return 0;
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#include "rom_tools.h"
#include "peripheral_sysctrl.h"

typedef void (* f_erase_sector)(uint32_t addr);
typedef void (* f_void)(void);
typedef void (* f_prog_page)(uint32_t addr, const uint8_t data[256], uint32_t len);

typedef int (* f_program_flash)(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size);
typedef int (* f_write_flash)(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size);
typedef int (* f_flash_do_update)(const int block_num, const fota_update_block_t *blocks, uint8_t *ram_buffer);

#define ROM_program_flash               ((f_program_flash)0x00003b9b)
#define ROM_write_flash                 ((f_write_flash)0x00003cff)
#define ROM_flash_do_update             ((f_flash_do_update)0x00001d73)

typedef void (*rom_void_void)(void);
#define ROM_FlashWaitBusyDown           ((rom_void_void)(0x00000b6d))
#define ROM_FlashDisableContinuousMode  ((rom_void_void)(0x000007c9))
#define ROM_FlashEnableContinuousMode   ((rom_void_void)(0x0000080d))
#define ROM_DCacheFlush                 ((rom_void_void)(0x00000651))

typedef void (*rom_prog_page)(uint32_t addr, const uint8_t *data, uint32_t len);
#define ROM_ProgPage                    ((rom_prog_page)(0x00003cd7))

typedef void (*rom_FlashSetStatusReg)(uint16_t data);
#define ROM_FlashSetStatusReg           ((rom_FlashSetStatusReg)(0x00000b01))

typedef uint16_t (*rom_FlashGetStatusReg)(void);
#define ROM_FlashGetStatusReg           ((rom_FlashGetStatusReg)(0x0000084d))

static void flash_read_protection_status(uint8_t *region, uint8_t *reverse_selection);

int erase_flash_sector(const uint32_t addr)
{
    uint32_t val = (uint32_t)-1;
    return program_flash(addr, (const uint8_t *)&val, sizeof(val));
}

int program_flash(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    return ROM_program_flash(dest_addr, buffer, size);
}

#define FLASH_PRE_OPS()                     \
    uint32_t prim = __get_PRIMASK();        \
    __disable_irq();                        \
    ROM_FlashDisableContinuousMode();

#define FLASH_POST_OPS()                    \
    ROM_DCacheFlush();                      \
    ROM_FlashEnableContinuousMode();        \
    if (!prim) __enable_irq()

int write_flash0(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    uint32_t next_page = (dest_addr & (~((uint32_t)EFLASH_PAGE_SIZE - 1))) + EFLASH_PAGE_SIZE;
    uint32_t i;
    for (i = 0; i < size; i++)
    {
        if ((*(const uint8_t *)(dest_addr + i) & buffer[i]) != buffer[i])
            return 1;
    }

    FLASH_PRE_OPS();
    {
        while (size > 0)
        {
            uint32_t block = next_page - dest_addr;
            if (block >= size) block = size;

            ROM_ProgPage(dest_addr, buffer, block);

            dest_addr += block;
            buffer += block;
            size -= block;
            next_page += EFLASH_PAGE_SIZE;
        }
    }
    FLASH_POST_OPS();
    return 0;
}

int write_flash(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    int r = write_flash0(dest_addr, buffer, size);
    if (r != 0) return r;
    r = memcmp((const void *)dest_addr, buffer, size) == 0 ? 0 : 2;
    return r;
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

static uint32_t read_flash_security0(uint32_t addr, void *prog)
{
    uint32_t ret;
    memcpy(prog, prog_security_page_read, sizeof(prog_security_page_read));
    FLASH_PRE_OPS();
    {
        ret = security_page_read(addr, (uint32_t)prog);
    }
    FLASH_POST_OPS();
    return ret;
}

uint32_t read_flash_security(uint32_t addr)
{
    uint32_t prog[sizeof(prog_security_page_read) / 4];
    if ((uintptr_t)prog < 0x40000000)
    {
        return read_flash_security0(addr, prog);
    }
    else
    {
        uint32_t ret;
        void *prog2 = (void *)SYSCTRL_I_CACHE_AS_MEM_BASE_ADDR;
        SYSCTRL_ICacheControl(SYSCTRL_MEM_BLOCK_AS_SYS_MEM);
        {
            ret = read_flash_security0(addr, prog2);
        }
        SYSCTRL_ICacheControl(SYSCTRL_MEM_BLOCK_AS_CACHE);
        return ret;
    }
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

#define EXTRA_DATA_LEN      320
#define FT_CHECK_LEN        (((sizeof(factory_data_t) + EXTRA_DATA_LEN) + 3) & ~4)
#define FT_CEHCKSUM_OFFSET  FT_CHECK_LEN

static uint32_t calc_checksum_32(const uint32_t* data, uint32_t length)
{
    uint32_t sum = 0;
    while(length--)
        sum += *data++;
    return sum;
}

static uint32_t calc_ft_sum()
{
    return calc_checksum_32((uint32_t *)FACTORY_DATA_LOC, FT_CHECK_LEN >> 2);
}

static uint32_t get_ft_sum()
{
    return *(uint32_t*)(FACTORY_DATA_LOC + FT_CEHCKSUM_OFFSET);
}

static void write_ft_sum()
{
    uint32_t checksum = calc_ft_sum();
    write_flash(FACTORY_DATA_LOC + FT_CEHCKSUM_OFFSET, (uint8_t *)&checksum, sizeof(uint32_t));
}

static int is_data_ready(void)
{
    const die_info_t *p = (const die_info_t *)FACTORY_DATA_LOC;
    if ((p->cid[0] != MAGIC_0) || (p->cid[1] != MAGIC_1))
        return 0;
    if (get_ft_sum() != calc_ft_sum())
        return 0;
    return 1;
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

static int check_security_data(uint32_t dst, uintptr_t src, int word_len)
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
        if (memcmp((void *)dst, (void *)buff, len * sizeof(buff[0])))
            return 1;
        dst += len * sizeof(buff[0]);
    }
    return 0;
}

int flash_prepare_factory_data(void)
{
    if (is_data_ready()) return 0;
    uint32_t t = read_flash_security(0x1000);
    if (t != MAGIC_0) return 1;
    t = read_flash_security(0x1004);
    if (t != MAGIC_1) return 2;

    uint8_t region;
    uint8_t reverse_selection;
    flash_read_protection_status(&region, &reverse_selection);
    flash_enable_write_protection(FLASH_REGION_NONE, 0);

    erase_flash_sector(FACTORY_DATA_LOC);
    copy_security_data(FACTORY_DATA_LOC,
        0x1000, sizeof(die_info_t) / 4);
    copy_security_data(FACTORY_DATA_LOC + sizeof(die_info_t),
        0x1100, sizeof(factory_calib_data_t) / 4);
    copy_security_data(FACTORY_DATA_LOC + sizeof(factory_data_t),
        0x2000, EXTRA_DATA_LEN / 4);
    if(check_security_data(FACTORY_DATA_LOC,
        0x1000, sizeof(die_info_t) / 4))
        goto check_failed;
    if(check_security_data(FACTORY_DATA_LOC + sizeof(die_info_t),
        0x1100, sizeof(factory_calib_data_t) / 4))
        goto check_failed;
    if(check_security_data(FACTORY_DATA_LOC + sizeof(factory_data_t),
        0x2000, EXTRA_DATA_LEN / 4))
        goto check_failed;
    write_ft_sum();

    flash_enable_write_protection(region, reverse_selection);

    return 0;

check_failed:
    erase_flash_sector(FACTORY_DATA_LOC);
    flash_enable_write_protection(region, reverse_selection);
    
    return 3;
}

const die_info_t *flash_get_die_info(void)
{
    if (flash_prepare_factory_data() == 0)
        return &((const factory_data_t *)FACTORY_DATA_LOC)->die_info;
    else
        return NULL;
}

const factory_calib_data_t *flash_get_factory_calib_data(void)
{
    if (flash_prepare_factory_data() == 0)
        return &((const factory_data_t *)FACTORY_DATA_LOC)->calib;
    else
        return NULL;
}

const void *flash_get_adc_calib_data(void)
{
    if (flash_prepare_factory_data() == 0)
        return (const void *)(FACTORY_DATA_LOC + sizeof(factory_data_t));
    else
        return NULL;
}

static void flash_read_protection_status(uint8_t *region, uint8_t *reverse_selection)
{
    FLASH_PRE_OPS();
    {
        uint16_t status = ROM_FlashGetStatusReg();
        *reverse_selection = ((status >> 14) & 1ul);
        *region = (0x1ful & (status >> 2));
    }
    FLASH_POST_OPS();
}

void flash_enable_write_protection(flash_region_t region, uint8_t reverse_selection)
{
    FLASH_PRE_OPS();
    {
        uint16_t status = ROM_FlashGetStatusReg();
        uint16_t old_status = status;
        status &= ~((1ul << 14) | (0x1ful << 2));
        status |= (uint16_t)reverse_selection << 14 | ((uint16_t)region << 2);
        if (old_status != status)
           ROM_FlashSetStatusReg(status);
    }
    FLASH_POST_OPS();
}

typedef void (* rom_FlashRUID)(uint32_t *UID);
#define ROM_FlashRUID  ((rom_FlashRUID) (0x00000a41))

void flash_read_uid(uint32_t uid[4])
{
    FLASH_PRE_OPS();
    {
        ROM_FlashRUID(uid);
    }
    FLASH_POST_OPS();
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
#include "peripheral_sysctrl.h"
#include "platform_api.h"
typedef void (* f_void)(void);
typedef void (* f_prog_page)(uint32_t addr, const uint8_t data[256], uint32_t len);
typedef int (* f_erase_flash_sector)(uint32_t addr);
typedef int (* f_program_flash)(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size);
typedef int (* f_write_flash)(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size);
typedef int (* f_flash_do_update)(const int block_num, const fota_update_block_t *blocks, uint8_t *ram_buffer);

typedef void (*rom_void_void)(void);
typedef void (*rom_FlashSectorErase)(uint32_t addr);
typedef void (*rom_FlashPageProgram)(uint32_t addr, const uint8_t *data, uint32_t len);
typedef void (*rom_FlashSetStatusReg)(uint16_t data);
typedef uint16_t (*rom_FlashGetStatusReg)(void);

#define ROM_erase_flash_sector              ((f_erase_flash_sector)0x000082b5)
#define ROM_program_flash                   ((f_program_flash)0x00025751)
#define ROM_write_flash                     ((f_write_flash)0x0002b785)
#define ROM_flash_do_update                 ((f_flash_do_update)0x000087d9)

#define ROM_FlashSectorErase                ((rom_FlashSectorErase)0x000007cd)
#define ROM_FlashDisableContinuousMode      ((rom_void_void)0x000005c5)
#define ROM_FlashEnableContinuousMode       ((rom_void_void)0x00000601)
#define ROM_FlashPageProgram                ((rom_FlashPageProgram)0x00000681)

#define FLASH_PRE_OPS()                         \
    uint32_t prim = __get_PRIMASK();            \
    uint8_t mode = 0;                           \
    __disable_irq();                            \
    if((*(uint32_t*)0x40150004) & 0x1000000ul) {    \
        mode = 1;                               \
        ROM_FlashDisableContinuousMode();       \
    }

#define FLASH_POST_OPS()                    \
    if (mode) ROM_FlashEnableContinuousMode(); \
    if (!prim) __enable_irq()

int erase_flash_sector(const uint32_t addr)
{
    FLASH_PRE_OPS();
    ROM_FlashSectorErase(addr);
    SYSCTRL_ICacheFlush();
    FLASH_POST_OPS();
    return 0;
}

int program_flash(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    if (dest_addr & (EFLASH_SECTOR_SIZE - 1)) return -1;

    FLASH_PRE_OPS();
    {
        while (size > 0)
        {
            uint32_t remain = EFLASH_SECTOR_SIZE;

            ROM_FlashSectorErase(dest_addr);

            while ((remain > 0) && (size > 0))
            {
                uint32_t cnt = size > EFLASH_PAGE_SIZE ? EFLASH_PAGE_SIZE : size;
                cnt = cnt > remain ? remain : cnt;
                ROM_FlashPageProgram(dest_addr, buffer, cnt);
                dest_addr += cnt;
                buffer += cnt;
                remain -= cnt;
                size -= cnt;
            }
        }
    }
    SYSCTRL_ICacheFlush();
    FLASH_POST_OPS();

    return 0;
}

int write_flash(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    uint32_t next_page = (dest_addr & (~((uint32_t)EFLASH_PAGE_SIZE - 1))) + EFLASH_PAGE_SIZE;
    uint32_t i;
    for (i = 0; i < size; i++)
    {
        if ((*(const uint8_t *)(dest_addr + i) & buffer[i]) != buffer[i])
            return 1;
    }

    FLASH_PRE_OPS();
    {
        while (size > 0)
        {
            uint32_t block = next_page - dest_addr;
            if (block >= size) block = size;

            ROM_FlashPageProgram(dest_addr, buffer, block);

            dest_addr += block;
            buffer += block;
            size -= block;
            next_page += EFLASH_PAGE_SIZE;
        }
    }
    SYSCTRL_ICacheFlush();
    FLASH_POST_OPS();
    return 0;
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
    for (i = 0; i < block_num; i++)
    {
        {
            uint32_t dest_addr = blocks[i].dest;
            const uint8_t *buffer = (const uint8_t *)blocks[i].src;
            int size = blocks[i].size;

            while (size > 0)
            {
                uint32_t remain = EFLASH_SECTOR_SIZE;

                ROM_erase_flash_sector(dest_addr);
                SYSCTRL_ICacheFlush();

                while ((remain > 0) && (size > 0))
                {
                    int j;
                    uint32_t cnt = size > EFLASH_PAGE_SIZE ? EFLASH_PAGE_SIZE : size;
                    cnt = cnt > remain ? remain : cnt;

                    for (j = 0; j < cnt; j++)
                        page_buffer[j] = buffer[j];

                    ROM_FlashPageProgram(dest_addr, page_buffer, cnt);

                    dest_addr += cnt;
                    buffer += cnt;
                    remain -= cnt;
                    size -= cnt;
                }
            }
        }
    }
    platform_reset();
    FLASH_POST_OPS();

    return 0;
}

#endif
