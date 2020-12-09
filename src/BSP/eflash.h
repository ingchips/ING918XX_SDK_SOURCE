#ifndef __EFLASH_H
#define __EFLASH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Erase a block of data in flash then write data.
 *
 * @param[in] dest_addr         page aligned target address (unified address) in flash
 * @param[in] buffer            buffer to be written
 * @param[in] size              byte number to be written (must be multiple of 4 bytes)
 * @return                      0 if successful else non-0
 */
int program_flash(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size);

/**
 * @brief Write a block of data to flash without erasing.
 *        Note: `dest_addr` must points to a block of flash that has been erased, otherwise,
 *              data can't be written into it.
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

/**
 * @brief Program FOTA metadata.
 *
 * @param[in] entry             new entry address (0 if use old entry address)
 * @param[in] block_num         number of blocks
 * @param[in] blocks            an array of `fota_update_block_t`
 * @return                      0 if successful else non-0
 */
int program_fota_metadata(const uint32_t entry, const int block_num, const fota_update_block_t *blocks);

#ifdef __cplusplus
}
#endif

#endif

