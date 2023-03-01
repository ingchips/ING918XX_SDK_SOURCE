/*
 *  hal_flash_bank.h
 * 
 *  HAL abstraction for Flash memory that can be written anywhere
 *  after being erased
 */

#ifndef HAL_FLASH_BANK_H
#define HAL_FLASH_BANK_H

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

typedef struct {

	/**
	 * Get total number of used banks.
	 */
	uint32_t (*get_bank_num)(void * context);

	/**
	 * Get size of flash banks
	 */
	uint32_t (*get_size)(void * context);

	/**
	 * Get flash read/write alignmenent requirements
	 */
	uint32_t (*get_alignment)(void * context);

	/**
	 * Erase a bank
	 * @param context
	 * @param bank
	 */
	void (*erase)(void * context, int bank);

	/**
	 * Read from flash into provided buffer
	 * @param context
	 * @param bank
	 * @param offset into flash bank
	 * @param buffer to read data
	 * @param size of data to read
	 */
	void (*read)(void * context, int bank, uint32_t offset, uint8_t * buffer, uint32_t size);

	/**
	 * Write data into flash. Each offset can only be written once after bank was erased
	 * @param context
	 * @param bank
	 * @param offset into flash bank
	 * @param data to read data
	 * @param size of data to store
	 */
	void (*write)(void * context, int bank, uint32_t offset, const uint8_t * data, uint32_t size);

} hal_flash_bank_t;

#if defined __cplusplus
}
#endif
#endif // HAL_FLASH_BANK_H
