#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "eflash.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) || (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
static void flash_read_protection_status(uint8_t *region, uint8_t *reverse_selection);
#endif

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
    #if defined(__GNUC__) && !defined(__ARMCC_VERSION)
    (void)addr;
    (void)prog;
    #endif
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

static void copy_security_data(uint32_t dst, uintptr_t src, uint32_t word_len)
{
    uint32_t buff[16];
    while (word_len > 0)
    {
        uint32_t len = 0;
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

static int check_security_data(uint32_t dst, uintptr_t src, uint32_t word_len)
{
    uint32_t buff[16];
    while (word_len > 0)
    {
        uint32_t len = 0;
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

typedef void (* f_void)(void);
typedef void (* f_prog_page)(uint32_t addr, const uint8_t data[256], uint32_t len);
typedef int (* f_flash_do_update)(const int block_num, const fota_update_block_t *blocks, uint8_t *ram_buffer);

typedef void (*rom_void_void)(void);
typedef void (*rom_FlashSectorErase)(uint8_t info, uint32_t addr);
typedef void (*rom_FlashPageProgram)(uint8_t info, uint32_t addr, const uint8_t *data, uint32_t len);
typedef void (*rom_FlashWriteStatusReg)(uint16_t data);
typedef uint8_t (*rom_FlashReadReg)(uint8_t cmd);
typedef uint32_t (*rom_FlashSecurityPageRead)(uint32_t addr);

#define ROM_flash_do_update                 ((f_flash_do_update)0x00008eb9)

#define ROM_FlashSectorErase                ((rom_FlashSectorErase)0x000007a1)
#define ROM_FlashDisableContinuousMode      ((rom_void_void)0x0000072d)
#define ROM_FlashEnableContinuousMode       ((rom_void_void)0x0000077d)
#define ROM_FlashPageProgram                ((rom_FlashPageProgram)0x000007c9)

#define IS_CONTINUOUS_MODE()                ((io_read(0x40150004) & 0x1000000ul) != 0)

#define ROM_FlashReadReg                    ((rom_FlashReadReg)(0x000009dd))
#define ROM_FlashWriteStatusReg             ((rom_FlashWriteStatusReg)(0x00000b71))
#define read_flash_security                 ((rom_FlashSecurityPageRead)(0x00000a4d))

#define FLASH_PROGRAM_CHUNK_SIZE            32u

#define FLASH_PRE_OPS()                         \
    uint32_t prim;              \
    uint8_t mode = 0; \
    prim =  __get_PRIMASK();  \
    __disable_irq();                            \
    if (IS_CONTINUOUS_MODE()) {                 \
        mode = 1;                               \
        ROM_FlashDisableContinuousMode();       \
    }

#define FLASH_POST_OPS()                    \
    if (mode) ROM_FlashEnableContinuousMode(); \
    if (!prim) __enable_irq()

static inline void flash_program_write(uint32_t dest_addr, const uint8_t *buffer, uint32_t len)
{
    uint32_t chunk;
#if (FLASH_PROGRAM_WITH_DMA == 1)
    uint32_t reg_state = (*(volatile uint32_t *)(AON1_CTRL_BASE + 0x18))&(0x1<<31);

    if(reg_state) *(volatile uint32_t *)(AON1_CTRL_BASE + 0x18)&=~(0x1<<31);
    ROM_FlashPageProgram(0x02, dest_addr, buffer, len);
    if(reg_state) *(volatile uint32_t *)(AON1_CTRL_BASE + 0x18)|=(0x1<<31);
#else
    while (len > 0u)
    {
        chunk = (len > FLASH_PROGRAM_CHUNK_SIZE) ? FLASH_PROGRAM_CHUNK_SIZE : len;
        ROM_FlashPageProgram(0x02, dest_addr, buffer, chunk);
        dest_addr += chunk;
        buffer += chunk;
        len -= chunk;
    }
#endif
}

int erase_flash_sector(const uint32_t addr)
{
    FLASH_PRE_OPS();
    ROM_FlashSectorErase(0x20, addr);
    SYSCTRL_ICacheFlush();
    FLASH_POST_OPS();
    return 0;
}

int erase_flash_page(const uint32_t addr)
{
    FLASH_PRE_OPS();
    ROM_FlashSectorErase(0x81, addr);
    SYSCTRL_ICacheFlush();
    FLASH_POST_OPS();
    return 0;
}

int program_flash(uint32_t dest_addr, const uint8_t *buffer, uint32_t size)
{
    if (dest_addr & (EFLASH_SECTOR_SIZE - 1)) return -1;

    FLASH_PRE_OPS();
    while (size > 0)
    {
        uint32_t remain = EFLASH_SECTOR_SIZE;

        ROM_FlashSectorErase(0x20, dest_addr);

        while ((remain > 0) && (size > 0))
        {
            uint32_t cnt = size > EFLASH_PAGE_SIZE ? EFLASH_PAGE_SIZE : size;
            cnt = cnt > remain ? remain : cnt;
            flash_program_write(dest_addr, buffer, cnt);
            dest_addr += cnt;
            buffer += cnt;
            remain -= cnt;
            size -= cnt;
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

    while (size > 0)
    {
        uint32_t block = next_page - dest_addr;
        if (block >= size) block = size;

        flash_program_write(dest_addr, buffer, block);
        dest_addr += block;
        buffer += block;
        size -= block;
        next_page += EFLASH_PAGE_SIZE;
    }
    
    SYSCTRL_ICacheFlush();
    FLASH_POST_OPS();
    return 0;
}

int flash_do_update(const int block_num, const fota_update_block_t *blocks, uint8_t *page_buffer)
{
    const uint8_t is_continuous = IS_CONTINUOUS_MODE();
    if (0 == is_continuous)
    {
        ROM_FlashEnableContinuousMode();
    }
    
    *(volatile uint32_t *)(AON1_CTRL_BASE + 0x18)&=~(0x1ul<<31);
    
    int r = ROM_flash_do_update(block_num, blocks, page_buffer);
    SYSCTRL_ICacheFlush();
    if (0 == is_continuous)
    {
        ROM_FlashDisableContinuousMode();
    }
    return r;
}

static uint16_t crc16(uint8_t *puchMsg, uint16_t usDataLen) {
    uint16_t crc = 0xFFFF;
	  int i;
    while (usDataLen--) {
        crc ^= *puchMsg++;
        for (i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return ((crc & 0xFF) << 8) | (crc >> 8);
}

#define FACTORY_DATA_LOC    (0x02000000 + 0x1000)
#define FACTORY_DIE_INFO_SRC_ADDR      (0x00000000u)
#define FACTORY_CALIB_SRC_ADDR         (0x00000100u)
#define FACTORY_DATA_DIE_INFO          (FACTORY_DATA_LOC)
#define FACTORY_DATA_CALIB             (FACTORY_DATA_LOC + 0x100)
#define FACTORY_DATA_CLC               (FACTORY_DATA_LOC + 0x200)
#define ADC_CAL_CHANNEL_NUM            9


static uint16_t calc_factory_info_crc16(const die_info_t *info)
{
    return crc16((uint8_t *)info, sizeof(die_info_t) - 4);
}

static uint16_t calc_factory_calib_crc16(const factory_calib_data_t *calib)
{
    return crc16((uint8_t *)calib, sizeof(factory_calib_data_t));
}

static void copy_security_bytes(uint8_t *dst, uint32_t src, uint32_t size)
{
    while (size >= 4U)
    {
        uint32_t word = read_flash_security(src);
        memcpy(dst, &word, sizeof(word));
        dst += sizeof(word);
        src += sizeof(word);
        size -= sizeof(word);
    }

    if (size > 0U)
    {
        uint32_t word = read_flash_security(src);
        memcpy(dst, &word, size);
    }
}

static int factory_data_ready(void)
{
    const factory_clc_data_t *factory = (const factory_clc_data_t *)FACTORY_DATA_CLC;

    if ((factory->magic_0 != FACTORY_DATA_MAGIC_0) ||
        (factory->magic_1 != FACTORY_DATA_MAGIC_1))
        return 0;

    return 1;
}

const factory_clc_data_t *flash_get_factory_clc_data(void)
{
    const factory_clc_data_t *factory = (const factory_clc_data_t *)FACTORY_DATA_CLC;

    if ((factory->magic_0 != FACTORY_DATA_MAGIC_0) ||
        (factory->magic_1 != FACTORY_DATA_MAGIC_1))
        return 0;

    return (const factory_clc_data_t *)FACTORY_DATA_CLC;
}

static void factory_set_linear_calib(adc_linear_calib_t *cal,
                                     uint16_t raw0,
                                     float phy0,
                                     uint16_t raw1,
                                     float phy1)
{
    if ((cal == NULL) || (raw0 == raw1))
        return;

    cal->k = (phy1 - phy0) / (float)((int32_t)raw1 - (int32_t)raw0);
    cal->b = phy0 - cal->k * (float)raw0;
}

static void factory_set_vbat_calib(adc_vbat_calib_t *cal,
                                   uint16_t raw_vbat33,
                                   float vbat33,
                                   uint16_t raw_vbat25,
                                   float vbat25)
{
    const float x0 = (raw_vbat33 == 0U) ? 0.0f : (1.0f / (float)raw_vbat33);
    const float x1 = (raw_vbat25 == 0U) ? 0.0f : (1.0f / (float)raw_vbat25);

    if ((cal == 0) || (raw_vbat33 == 0) || (raw_vbat25 == 0))
        return;

    cal->k = (vbat25 - vbat33) / (x1 - x0);
    cal->b = vbat33 - cal->k * x0;
}

void flash_build_factory_clc_data(const factory_calib_data_t *src, factory_clc_data_t *dst)
{
    int i;

    if ((src == 0) || (dst == 0))
        return;

    memset(dst, 0xff, sizeof(*dst));
    dst->magic_0 = FACTORY_DATA_MAGIC_0;
    dst->magic_1 = FACTORY_DATA_MAGIC_1;
    dst->version = FACTORY_CLC_DATA_VERSION;
    dst->flags = 0;

    for (i = 0; i < ADC_CAL_CHANNEL_NUM; ++i)
    {
        if (i<2)
        {
            factory_set_linear_calib(&dst->ch0_8_int_ref[i],
                                 src->calib_adc.int_vbat33_ain_ch0_8[i][0],
                                 341.3333f,
                                 src->calib_adc.int_vbat33_ain_ch0_8[i][1],
                                 1137.7778f);
            factory_set_linear_calib(&dst->ch0_8_vbat_ref[i],
                                     src->calib_adc.vbat33_flt_ain_ch0_8[i][0],
                                     310.3030f,
                                     src->calib_adc.vbat33_flt_ain_ch0_8[i][1],
                                     1675.6364f);
        }
        else
        {
            factory_set_linear_calib(&dst->ch0_8_int_ref[i],
                                     src->calib_adc.int_vbat33_ain_ch0_8[i][0],
                                     682.6667f,
                                     src->calib_adc.int_vbat33_ain_ch0_8[i][1],
                                     2275.5556f);
            factory_set_linear_calib(&dst->ch0_8_vbat_ref[i],
                                     src->calib_adc.vbat33_flt_ain_ch0_8[i][0],
                                     620.6060f,
                                     src->calib_adc.vbat33_flt_ain_ch0_8[i][1],
                                     3351.2727f);
        }
    }

    factory_set_vbat_calib(&dst->ch9_vbat,
                           src->calib_adc.vbat33_flt_int_ch9_11[0],
                           3.3f,
                           src->calib_adc.vbat25_flt_int_ch9_11[0],
                           2.5f);
}

int flash_prepare_factory_data(void)
{
    factory_calib_data_t calib;
    die_info_t die_info;
    uint8_t region;
    uint8_t reverse_selection;
    factory_clc_data_t generated;
    uint32_t reg_state;

    if (factory_data_ready())
        return 0;

    reg_state = (*(volatile uint32_t *)(AON1_CTRL_BASE + 0x18))&(0x1ul<<31);
    
    flash_read_protection_status(&region, &reverse_selection);
    flash_enable_write_protection(FLASH_REGION_NONE, 0);
    
    erase_flash_sector(FACTORY_DATA_LOC);

    copy_security_bytes((uint8_t *)&die_info,
                        FACTORY_DIE_INFO_SRC_ADDR,
                        sizeof(die_info));
    copy_security_bytes((uint8_t *)&calib,
                        FACTORY_CALIB_SRC_ADDR,
                        sizeof(calib));

    uint16_t crc = calc_factory_info_crc16(&die_info);
    if (crc != die_info.info_crc16)
        return 1;
    crc = calc_factory_calib_crc16(&calib);
    if (crc != die_info.trim_crc16)
        return 2;

    if (write_flash(FACTORY_DATA_LOC,
                    (const uint8_t *)&die_info,
                    sizeof(die_info)) != 0)
        goto check_failed;
    if (write_flash(FACTORY_DATA_CALIB,
                    (const uint8_t *)&calib,
                    sizeof(calib)) != 0)
        goto check_failed;

    if (memcmp((const void *)((const uint8_t *)FACTORY_DATA_DIE_INFO),
               (const void *)&die_info,
               sizeof(die_info)) != 0)
        goto check_failed;
    if (memcmp((const void *)((const uint8_t *)FACTORY_DATA_CALIB),
               (const void *)&calib,
               sizeof(calib)) != 0)
        goto check_failed;

    flash_build_factory_clc_data(&calib, &generated);

    if(reg_state) *(volatile uint32_t *)(AON1_CTRL_BASE + 0x18)&=~(0x1ul<<31);
    if (write_flash(FACTORY_DATA_CLC,
                    (const uint8_t *)&generated,
                    sizeof(generated) - 8) != 0)
        goto check_failed;
    if(reg_state) *(volatile uint32_t *)(AON1_CTRL_BASE + 0x18)|=(0x1ul<<31);
    if (memcmp((const void *)((const uint8_t *)FACTORY_DATA_CLC),
               (const void *)&generated,
               sizeof(generated) - 8) != 0)
        goto check_failed;
    if (write_flash(FACTORY_DATA_CLC + sizeof(generated) - 8,
                    (const uint8_t *)&generated.magic_0,
                    8) != 0)
        goto check_failed;

    flash_enable_write_protection((flash_region_t)region, reverse_selection);
    return 0;

check_failed:
    if(reg_state) *(volatile uint32_t *)(AON1_CTRL_BASE + 0x18)|=(0x1ul<<31);
    erase_flash_sector(FACTORY_DATA_LOC);
    flash_enable_write_protection((flash_region_t)region, reverse_selection);
    
    return 3;
}

const die_info_t *flash_get_die_info(void)
{
    if (flash_prepare_factory_data() == 0)
        return (const die_info_t *)FACTORY_DATA_DIE_INFO;

    return NULL;
}

const factory_calib_data_t *flash_get_factory_calib_data(void)
{
    if (flash_prepare_factory_data() == 0)
        return (const factory_calib_data_t *)FACTORY_DATA_CALIB;

    return NULL;
}

typedef void (* rom_FlashRUID)(uint32_t *UID);
#define ROM_FlashRUID  ((rom_FlashRUID) (0x00000989))

void flash_read_uid(uint32_t uid[4])
{
    FLASH_PRE_OPS();
    {
        ROM_FlashRUID(uid);
    }
    FLASH_POST_OPS();
}

int Vcore_calib(void)
{
    int i;
    uint8_t vcc_index;
    uint32_t reg_data;
    const factory_calib_data_t * calib_data = flash_get_factory_calib_data();
    if (calib_data)
    {
        for (i = 0; i < 8; i++)
        {
            if (calib_data->calib_pmu.vaon[i] > 1010)
            {
                vcc_index =  i+calib_data->calib_pmu.vaon_index;
                reg_data = *(uint32_t*)(AON1_CTRL_BASE+0x30);
                reg_data &= ~(0xful<<28);
                reg_data |=(((vcc_index&0x3)|((~vcc_index)&0xc))&0xf)<<28;
                *(uint32_t*)(AON1_CTRL_BASE+0x30) = reg_data;
                
                reg_data = *(uint32_t*)(AON1_CTRL_BASE+0x38);
                reg_data &= ~(0xf<<15);
                reg_data |= (((vcc_index&0x3)|((~vcc_index)&0xc))&0xf)<<15;
                *(uint32_t*)(AON1_CTRL_BASE+0x38) = reg_data;
                break;
            }
        }
        for (i = 0; i < 16; i++)
        {
            if (calib_data->calib_pmu.vcore[i] > 1170)
            {
                vcc_index =  i+calib_data->calib_pmu.vcore_index;
                reg_data = *(uint32_t*)(AON1_CTRL_BASE+0x30);
                reg_data &= ~(0x1f<<5);
                reg_data |=(((vcc_index&0xf)|((~vcc_index)&0x10))&0x1f)<<5;
                *(uint32_t*)(AON1_CTRL_BASE+0x30) = reg_data;
                break;
            }
        }
        for (i = 15; i >= 0; i--)
        {
            if (calib_data->calib_pmu.vdc33[i] > 1470)
            {
                vcc_index =  i+calib_data->calib_pmu.vdc33_index;
                reg_data = *(uint32_t*)(AON1_CTRL_BASE+0x34);
                reg_data &= ~(0x3f<<14);
                reg_data |=(vcc_index&0x3f)<<14;
                *(uint32_t*)(AON1_CTRL_BASE+0x34) = reg_data;
                break;
            }
        }
        return 0;
    }
    return -1;
}

#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916) 

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

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

static void flash_read_protection_status(uint8_t *region, uint8_t *reverse_selection)
{
    FLASH_PRE_OPS();
    {
        uint16_t status = ROM_FlashReadReg(0x05) | (ROM_FlashReadReg(0x35)<<8);
        *reverse_selection = ((status >> 14) & 1ul);
        *region = (0x1ful & (status >> 2));
    }
    FLASH_POST_OPS();
}

void flash_enable_write_protection(flash_region_t region, uint8_t reverse_selection)
{
    FLASH_PRE_OPS();
    {
        uint8_t status = ROM_FlashReadReg(0x05) | (ROM_FlashReadReg(0x35)<<8);
        uint16_t old_status = status;
        status &= ~((1ul << 14) | (0x1ful << 2));
        status |= (uint16_t)reverse_selection << 14 | ((uint16_t)region << 2);
        if (old_status != status)
            ROM_FlashWriteStatusReg(status);
    }
    FLASH_POST_OPS();
}
#endif
