/*
 *  hal_flash_bank_eflash.h
 * 
 *  HAL abstraction for Flash memory that can be written anywhere
 *  after being erased
 */

#ifndef __HAL_FLASH_BANK_EFLASH_H
#define __HAL_FLASH_BANK_EFLASH_H

#include <stdint.h>
#include "hal_flash_bank.h"


typedef struct {
    uint32_t   bank_size;
    uint32_t   bank_num;
    uintptr_t  banks[2];
} hal_flash_bank_eflash_t;

/**
 * Configure eflash HAL Flash Implementation
 *
 * @param context of hal_flash_bank_eflash_t
 * @param bank_size
 * @param bank_0_addr
 * @param bank_1_addr
 * @return 
 */
const hal_flash_bank_t * hal_flash_bank_eflash_init_instance(hal_flash_bank_eflash_t * context, 
        uint32_t bank_size, uintptr_t bank_0_addr, uintptr_t bank_1_addr);


#endif
