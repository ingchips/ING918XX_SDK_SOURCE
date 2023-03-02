#ifndef __EFLASH_H
#define __EFLASH_H

#include <stdint.h>

#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Erase a block of data in flash then write data.
 *
 * Note: For ING916, `buffer` must not be in Flash.
 *
 * @param[in] dest_addr         target address (unified address, aligned at EFLASH_ERASABLE_SIZE) in flash
 * @param[in] buffer            buffer to be written
 * @param[in] size              byte number to be written (must be multiple of 4 bytes)
 * @return                      0 if successful else non-0
 */
int program_flash(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size);

/**
 * @brief Write a block of data to flash without erasing.
 *
 * Note: `dest_addr` must points to a block of flash that has been erased, otherwise,
 *        data can't be written into it.
 *
 * Note: For ING916, `buffer` must be in RAM.
 *
 * @param[in] dest_addr         32-bit aligned target address (unified address) in flash
 * @param[in] buffer            buffer to be written
 * @param[in] size              byte number to be written (must be multiple of 4 bytes)
 * @return                      0 if successful else non-0
 */
int write_flash(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size);

typedef struct fota_update_block
{
    uint32_t src;
    uint32_t dest;
    uint32_t size;
} fota_update_block_t;

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define EFLASH_PAGE_SIZE        8192
#define EFLASH_ERASABLE_SIZE    EFLASH_PAGE_SIZE

/**
 * @brief Erase a page of flash
 *
 * @param[in] addr              start address (unified address) of the page
 * @return                      0 if successful else non-0
 */
int erase_flash_page(const uint32_t addr);

/**
 * @brief Erase a info page of flash
 *
 * @param[in] index             page index (0..1)
 * @return                      0 if successful else non-0
 */
int erase_info_page(const int index);

/**
 * @brief Program FOTA metadata.
 *
 * @param[in] entry             new entry address (0 if use old entry address)
 * @param[in] block_num         number of blocks
 * @param[in] blocks            an array of `fota_update_block_t`
 * @return                      0 if successful else non-0
 */
int program_fota_metadata(const uint32_t entry, const int block_num, const fota_update_block_t *blocks);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define EFLASH_PAGE_SIZE        256
#define EFLASH_SECTOR_SIZE      4096
#define EFLASH_ERASABLE_SIZE    EFLASH_SECTOR_SIZE

typedef void (*rom_FlashWaitBusyDown)(void);
typedef void (*rom_FlashDisableContinuousMode)(void);
typedef void (*rom_FlashEnableContinuousMode)(void);
#define ROM_FlashWaitBusyDown           ((rom_FlashWaitBusyDown)          (0x00000b6d))
#define ROM_FlashDisableContinuousMode  ((rom_FlashDisableContinuousMode) (0x000007c9))
#define ROM_FlashEnableContinuousMode   ((rom_FlashEnableContinuousMode)  (0x0000080d))

typedef enum {
    SPI_CMD_ADDR         = 0x0,
    SPI_BLOCK_SIZE       = 0x4,
    RX_STATUS            = 0x10,
    SPI_CFG              = 0x14,
} SPI_FLASH_Reg;

/**
 * @brief Erase a sector of flash
 *
 * @param[in] addr              start address (unified address) of the sector
 * @return                      0 if successful else non-0
 */
int erase_flash_sector(const uint32_t addr);

/**
 * @brief do FOTA update
 *
 * @param[in] block_num         number of blocks
 * @param[in] blocks            an array of `fota_update_block_t`
 * @param[in] ram_buffer        buffer large enough for holding `EFLASH_ERASABLE_SIZE` bytes of data
 * @return                      non-0 if error occurs else platform is reset
 */
int flash_do_update(const int block_num, const fota_update_block_t *blocks, uint8_t *ram_buffer);

/**
 * @brief Read a sector of flash
 *
 * @param[in] addr              start address (unified address) of the sector
 * @return                      flash data
 */
uint32_t ReadFlashSecurity(uint32_t addr);

#endif

#ifdef __cplusplus
}
#endif

#endif

