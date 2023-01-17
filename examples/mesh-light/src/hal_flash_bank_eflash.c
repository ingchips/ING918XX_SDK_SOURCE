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


static uint32_t hal_flash_bank_eflash_get_size(void * context){
	hal_flash_bank_eflash_t * self = (hal_flash_bank_eflash_t *) context;
	return self->bank_size;
}

static uint32_t hal_flash_bank_memory_get_alignment(void * context){
    return 4;
}

static void hal_flash_bank_eflash_erase(void * context, int bank){
	hal_flash_bank_eflash_t * self = (hal_flash_bank_eflash_t *) context;
	if (bank > 1) return;

    // Erase page.
	erase_flash_page(self->banks[bank]);
    app_log_debug("== erase flash[0x%x]\n", self->banks[bank]);
}

static void hal_flash_bank_eflash_read(void * context, int bank, uint32_t offset, uint8_t * buffer, uint32_t size){
	hal_flash_bank_eflash_t * self = (hal_flash_bank_eflash_t *) context;

	if (bank > 1) return;
	if (offset > self->bank_size) return;
	if ((offset + size) > self->bank_size) return;

	memcpy(buffer, ((uint8_t *) self->banks[bank]) + offset, size);
//    app_log_debug("== read flash[0x%x][%d]: ", self->banks[bank] + offset, size);
//    app_log_debug_hexdump(buffer, size);
}

static void hal_flash_bank_eflash_write(void * context, int bank, uint32_t offset, const uint8_t * data, uint32_t size){
	hal_flash_bank_eflash_t * self = (hal_flash_bank_eflash_t *) context;

	if (bank > 1) return;
	if (offset > self->bank_size) return;
	if ((offset + size) > self->bank_size) return;
    if (size == 0) return;
    
#if !defined(MESH_STACK_TLV_USE_FLASH)
    return;
#endif
    
#ifdef EFLASH_WRITE_SUPPORT_OFFSET_ADDR_NOT_ALIGMENT
    
    uint32_t aligment = hal_flash_bank_memory_get_alignment(NULL);
    if ((size & (aligment - 1)) != 0) return; //size must be aligment.

    uint32_t addr = self->banks[bank] + offset;
    uint32_t addr_lower_bits = addr & (aligment - 1);

    /* start addr is aligment, and size is also aligment. */
    if (addr_lower_bits == 0) {
        write_flash(addr, data, size);
        return;
    }
    
    /* start addr not aligment, size is aligment. */

    // write part.1 (not aligment, fill with flash reading data.)
    uint8_t * pBuffer = (uint8_t *)data;
    uint32_t fill_len = aligment - addr_lower_bits;
    uint32_t addr_aligned = addr - addr_lower_bits;
    uint8_t tmpData[8]; //care: aligment <= 8.
    memset(tmpData, 0xff, sizeof(tmpData));
    memcpy(tmpData, ((uint8_t *) addr_aligned), addr_lower_bits); //read flash
    memcpy(tmpData + addr_lower_bits, pBuffer, fill_len);
    write_flash(addr_aligned, tmpData, aligment);
    addr_aligned += aligment;
    pBuffer += fill_len;
    size    -= fill_len;

    // write part.2 (if exist. aligment.)
    uint32_t size_lower_bits = size & (aligment - 1);
    uint32_t size_aligned = size - size_lower_bits;
    if(size_aligned){
        write_flash(addr_aligned, pBuffer, size_aligned);
        addr_aligned += size_aligned;
        pBuffer      += size_aligned;
        size         -= size_aligned;
    }

    // write part.3 (not aligment, fill 0xff)
    if(size == 0) return;
    memset(tmpData, 0xff, sizeof(tmpData));
    memcpy(tmpData, pBuffer, size);
    write_flash(addr_aligned, tmpData, aligment);
    
#else 

    // Write flash
    int ret = write_flash(self->banks[bank] + offset, data, size);
    app_log_debug("== write flash[0x%x][%d]: ", self->banks[bank] + offset, size);
    app_log_debug_hexdump(data, size);
    if(ret){
        app_log_error("~~ write fail!!! \n");
    }
    
#endif
}

static const hal_flash_bank_t hal_flash_bank_eflash_impl = {
	/* uint32_t (*get_size)() */         &hal_flash_bank_eflash_get_size,
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

    // Set eflash init here, if needed.

	return &hal_flash_bank_eflash_impl;
}
