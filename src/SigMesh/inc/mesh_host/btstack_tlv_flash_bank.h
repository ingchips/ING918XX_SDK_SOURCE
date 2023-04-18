/*
 *  btstack_tlv_flash_bank.h
 *
 *  Implementation for BTstack's Tag Value Length Persistent Storage implementations
 *  using hal_flash_bank storage
 */

#ifndef BTSTACK_TLV_FLASH_BANK_H
#define BTSTACK_TLV_FLASH_BANK_H

#include <stdint.h>
#include "btstack_tlv.h"
#include "hal_flash_bank.h"

#if defined __cplusplus
extern "C" {
#endif

typedef struct {
    const hal_flash_bank_t * hal_flash_bank_impl;
    void * hal_flash_bank_context;
    int current_bank;
    int write_offset;
    int delete_tag_len;
} btstack_tlv_flash_bank_t;

/**
 * Init Tag Length Value Store
 * @param context btstack_tlv_flash_bank_t 
 * @param hal_flash_bank_impl    of hal_flash_bank interface
 * @Param hal_flash_bank_context of hal_flash_bank_interface
 */
const btstack_tlv_t * btstack_tlv_flash_bank_init_instance(btstack_tlv_flash_bank_t * context, const hal_flash_bank_t * hal_flash_bank_impl, void * hal_flash_bank_context);

#if defined __cplusplus
}
#endif
#endif // BTSTACK_TLV_FLASH_BANK_H
