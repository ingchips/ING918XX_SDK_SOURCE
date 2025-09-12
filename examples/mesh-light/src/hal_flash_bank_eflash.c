/*
 *  hal_flash_bank_eflash.c
 *
 *  HAL abstraction for Flash memory that can be written anywhere
 *  after being erased
 */

#include <stdint.h>
#include <string.h>
#include "stdio.h"
#include "eflash.h"
#include "hal_flash_bank_eflash.h"
#include "app_config.h"
#include "app_debug.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    #define ERASE_BANK              erase_flash_page
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #define ERASE_BANK              erase_flash_sector
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
    #define ERASE_BANK              erase_flash_sector
#else
    #error unknown or unsupported chip family
#endif

static uint32_t hal_flash_bank_eflash_get_bank_num(void * context){
    hal_flash_bank_eflash_t * self = (hal_flash_bank_eflash_t *) context;
    return self->bank_num;
}

static uint32_t hal_flash_bank_eflash_get_size(void * context){
    hal_flash_bank_eflash_t * self = (hal_flash_bank_eflash_t *) context;
    return self->bank_size;
}

static uint32_t hal_flash_bank_memory_get_alignment(void * context){
    return 4;
}

static void hal_flash_bank_eflash_erase(void * context, int bank){
    hal_flash_bank_eflash_t * self = (hal_flash_bank_eflash_t *) context;
    if (bank > 1) { app_log_error("Erase: bank error."); return; }

    // Erase page.
    ERASE_BANK(self->banks[bank]);
    app_log_debug("== erase flash[0x%x]\n", self->banks[bank]);
}

static void hal_flash_bank_eflash_read(void * context, int bank, uint32_t offset, uint8_t * buffer, uint32_t size){
    hal_flash_bank_eflash_t * self = (hal_flash_bank_eflash_t *) context;

    if (bank > 1) { app_log_error("Read: bank error."); return; }
    if (offset > self->bank_size) { app_log_error("Read: offset error."); return; }
    if ((offset + size) > self->bank_size) { app_log_error("Read: size error."); return; }

    memcpy(buffer, ((uint8_t *) self->banks[bank]) + offset, size);
}

static void hal_flash_bank_eflash_write(void * context, int bank, uint32_t offset, const uint8_t * data, uint32_t size){
    hal_flash_bank_eflash_t * self = (hal_flash_bank_eflash_t *) context;

#if !defined(MESH_STACK_TLV_USE_FLASH)
    app_log_debug("Write not avaliable!");
    return;
#endif

    if (bank > 1) { app_log_error("Write: bank error."); return; }
    if (offset > self->bank_size) { app_log_error("Write: offset error."); return; }
    if ((offset + size) > self->bank_size) { app_log_error("Write: size error."); return; }
    if (size == 0) { app_log_error("Write: size=0 error."); return; }

    uint32_t aligment = hal_flash_bank_memory_get_alignment(NULL);
    app_assert((size & (aligment - 1)) == 0); // size must be aligment.
    app_assert((offset & (aligment - 1)) == 0); // offset must be aligment.

    /* start addr is aligment, and size is also aligment. */
    write_flash(self->banks[bank] + offset, data, size);
}

static const hal_flash_bank_t hal_flash_bank_eflash_impl = {
    /* uint32_t (*get_bank_num)()     */ &hal_flash_bank_eflash_get_bank_num,
    /* uint32_t (*get_size)()         */ &hal_flash_bank_eflash_get_size,
    /* uint32_t (*get_alignment)(..); */ &hal_flash_bank_memory_get_alignment,
    /* void (*erase)(..);             */ &hal_flash_bank_eflash_erase,
    /* void (*read)(..);              */ &hal_flash_bank_eflash_read,
    /* void (*write)(..);             */ &hal_flash_bank_eflash_write,
};

const hal_flash_bank_t * hal_flash_bank_eflash_init_instance(hal_flash_bank_eflash_t * context,
        uint32_t bank_size, uintptr_t bank_0_addr, uintptr_t bank_1_addr){
    context->bank_size  = bank_size;
    context->banks[0]   = bank_0_addr;
    context->banks[1]   = bank_1_addr;
    context->bank_num   = sizeof(context->banks)/sizeof(context->banks[0]);

    // Set eflash init here, if needed.

    return &hal_flash_bank_eflash_impl;
}
