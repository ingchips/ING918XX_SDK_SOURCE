#ifndef __EFLASH_H
#define __EFLASH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Write a block of data to flash
 * @param[in] dest_addr         target address (unified address) in flash 
 * @param[in] buffer            buffer to be written
 * @param[in] size              byte number to be written
 * @return                      0 if successful else non-0
 */
int program_flash(const uint32_t dest_addr, const uint8_t *buffer, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif

