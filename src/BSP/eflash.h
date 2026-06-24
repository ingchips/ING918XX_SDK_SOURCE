#ifndef __EFLASH_H
#define __EFLASH_H

#include <stdint.h>

#include "ingsoc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
/**
 * @note
 * For the ING20, when FLASH_PROGRAM_WITH_DMA is set to 0, it is recommended that the size of a single write
 * operation not exceed 32 bytes. Any data exceeding 32 bytes will be split into multiple write operations,
 * increasing the number of flash write operations.
 *
 * When FLASH_PROGRAM_WITH_DMA is set to 1, the interface will use DMA Channel 0 to operate the flash.
 * Note that DMA Channel 0 must not be used for other applications;
 * otherwise, the interface will modify the configuration of Channel 0, causing DMA transfer errors. Additionally,
 * when using DMA transfers, the interface will set the flash frequency to 24 MHz. At higher FLASH frequencies,
 * programming more than 32 bytes of data may result in an error, manifesting as only 16 bytes being successfully programmed.
 */
#ifndef FLASH_PROGRAM_WITH_DMA
#define FLASH_PROGRAM_WITH_DMA      0
#endif

#endif

/**
 * @brief Erase a block of data in flash then write data.
 *
 * Note: For ING916 or ING20, `buffer` must not be in Flash.
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * For ING20.
 * Please refer to the description of the FLASH_PROGRAM_WITH_DMA macro.
 * It is recommended that a single write operation not exceed 32 bytes.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
 * For ING916 ING20:
 *      * `buffer` must be in RAM. In order to copy data within flash, it must be copied to RAM
 *          firstly, then write this piece of RAM to flash.
 *
 * For ING918:
 *      * `dest_addr` must be 32-bit aligned
 *      * `size` must be multiple of 4 bytes.
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * For ING20.
 * Please refer to the description of the FLASH_PROGRAM_WITH_DMA macro.
 * It is recommended that a single write operation not exceed 32 bytes.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * @param[in] dest_addr         target address (unified address) in flash
 * @param[in] buffer            buffer to be written
 * @param[in] size              byte number to be written
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

#pragma pack (push, 1)
typedef struct
{
    uint32_t cid[2];            // company ID
    uint32_t reserved0[2];
    uint32_t metal_id;          // metal ID
    uint32_t reserved1[3];
    uint32_t lot_id[4];         // Lot ID
    uint32_t wafer_id;          // wafer ID
    uint32_t reserved2[3];
    uint32_t Die_x_local;          // die_x ID
    uint32_t reserved3[3];
    uint32_t Die_y_local;          // die_y ID
    uint32_t reserved4[3];
} die_info_t;

typedef struct
{
    uint32_t adc_trim0[256];    // ADC trim data
    uint32_t adc_version;       // ADC version
    uint32_t reserved0[255];
    uint32_t adc_trim1[256];    // ADC trim data
} adc_calib_data_t;

typedef struct{
    uint8_t TRng_data[16];     // TRNG data
    uint32_t ft_version;       // factory data version
    uint32_t ldo_flag;         // LDO flag
    uint32_t ldo_vol[4];       // LDO voltage
} factory_calib_data_t;
#pragma pack (pop)

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

/**
 * @brief Get die information
 *
 * @return  pointer to `die_info_t`
 * @note
 *          Before accessing the returned pointer, you **must** call `EflashCacheBypass()` to disable cache,
 *          and after accessing the data, call `EflashCacheEna()` to re-enable cache.
 *
 *          This API returns a pointer to data stored in flash.
 *          It is **not recommended** to use this API to return a pointer to global/static variables
 *          that require cache coherence, due to the need to bypass cache during access.
 */
const die_info_t *flash_get_die_info(void);

/**
 * @brief Get ADC calibration data
 * @return  pointer to `adc_calib_data_t`
 * @note
 *          Before accessing the returned pointer, you **must** call `EflashCacheBypass()` to disable cache,
 *          and after accessing the data, call `EflashCacheEna()` to re-enable cache.
 *
 *          This API returns a pointer to data stored in flash.
 *          It is **not recommended** to use this API to return a pointer to global/static variables
 *          that require cache coherence, due to the need to bypass cache during access.
 */
const adc_calib_data_t *flash_get_adc_calib_data(void);

/**
 * @brief   Read UID of flash
 *
 * @param[out]  uid            128-bit UID buffer to store the unique identifier
 *
 * @note
 *          Before accessing the returned pointer, you **must** call `EflashCacheBypass()` to disable cache,
 *          and after accessing the data, call `EflashCacheEna()` to re-enable cache.
 *
 *          This API returns a pointer to data stored in flash.
 *          It is **not recommended** to use this API to return a pointer to global/static variables
 *          that require cache coherence, due to the need to bypass cache during access.
 */
const factory_calib_data_t *flash_get_factory_calib_data(void);

/**
 * @brief Read UID of flash
 *
 * @param[out]  uid            128-bit UID
 * @note
 *          The UID is generated by combining chip-specific manufacturing information
 *          with a software-generated random number.
 *          On some older batches of ICs, the random number generation may not be supported,
 *          in which case the last 6 bytes will be filled with 0x00.
 *
 *          **Do not** perform an unlock operation after locking the chip, as this may cause
 *          abnormal behavior in the RNG data, leading to incorrect or unstable data in
 *          the last 6 bytes of the UID in subsequent reads.
 */
void flash_read_uid(uint32_t uid[4]);

/**
 * @brief Read UID 45 bit of flash
 *
 * @param[out]  uid            45-bit unique use
 * @return                     0 if successful else non-0
 * @note
 *         The UID 45 bit consists of a 5-bit wafer ID, a 24 bit Lot number, and the x and y coordinates of the chip on the wafer
 */
int flash_read_uid45(uint8_t uid[6]);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define EFLASH_PAGE_SIZE        256
#define EFLASH_SECTOR_SIZE      4096
#define EFLASH_ERASABLE_SIZE    EFLASH_SECTOR_SIZE

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
 *                              CAUTION: DO NOT use memory from `SYSCTRL_SHARE_BLOCK_XXX` for this,
 *                                       while `SYSCTRL_MEM_BLOCK_XXX` and D-CACHE (as memory) are OK.
 * @return                      non-0 if error occurs else platform is reset
 */
int flash_do_update(const int block_num, const fota_update_block_t *blocks, uint8_t *ram_buffer);

typedef enum
{
    FLASH_REGION_NONE       = 0x00,         // none         (        0KB)
    FLASH_REGION_UPPER_1_8  = 0x01,         // upper 1/8    (upper  64KB)
    FLASH_REGION_UPPER_1_4  = 0x02,         // upper 1/4    (upper 128KB)
    FLASH_REGION_UPPER_1_2  = 0x03,         // upper 1/2    (upper 256KB)
    FLASH_REGION_LOWER_1_8  = 0x09,         // lower 1/8    (lower  64KB)
    FLASH_REGION_LOWER_1_4  = 0x0A,         // lower 1/4    (lower 128KB)
    FLASH_REGION_LOWER_1_2  = 0x0B,         // lower 1/2    (lower 256KB)
    FLASH_REGION_UPPER_1_128 = 0x11,        // upper 1/128  (upper   4KB)
    FLASH_REGION_UPPER_1_64  = 0x12,        // upper 1/64   (upper   8KB)
    FLASH_REGION_UPPER_1_32  = 0x13,        // upper 1/32   (upper  16KB)
    FLASH_REGION_UPPER_1_16  = 0x14,        // upper 1/16   (upper  32KB)
    FLASH_REGION_LOWER_1_128 = 0x19,        // lower 1/128  (lower   4KB)
    FLASH_REGION_LOWER_1_64  = 0x1A,        // lower 1/64   (lower   8KB)
    FLASH_REGION_LOWER_1_32  = 0x1B,        // lower 1/32   (lower  16KB)
    FLASH_REGION_LOWER_1_16  = 0x1C,        // lower 1/16   (lower  32KB)
    FLASH_REGION_ALL         = 0x1f,        // all          (      512KB)
} flash_region_t;

/**
 * @brief Enable write protection on a selected region
 *
 * e.g.
 *
 * ```c
 * // turn on write protection for the upper 1/8 of flash
 * flash_enable_write_protection(FLASH_REGION_UPPER_1_8, 0);
 *
 * // turn on write protection for the lower 7/8 of flash,
 * // i.e. reverse the selection of `FLASH_REGION_UPPER_1_8`
 * flash_enable_write_protection(FLASH_REGION_UPPER_1_8, 1);
 * ```
 *
 * Call this before doing write operations on flash if related region is write
 * protected. Select NONE region will disable the protection completely:
 *
 * ```c
 * flash_enable_write_protection(FLASH_REGION_NONE, 0);
 * ```
 *
 * Note: Write protection is a global configuration for the flash. It is impossible
 * to enable the protection for two separated region, such as `FLASH_REGION_UPPER_1_8`,
 * `FLASH_REGION_LOWER_1_128` by invoking this API twice.
 *
 * @param[in] region                specify a region
 * @param[in] reverse_selection     select `region` (0) or reverse the selection of `region` (1)
 */
void flash_enable_write_protection(flash_region_t region, uint8_t reverse_selection);

#pragma pack (push, 1)
typedef struct
{
    uint32_t cid[2];            // company ID
    uint32_t metal_id;          // metal ID
    uint32_t package_id;        // package ID
    uint32_t chip_uuid[8];      // chip ID (random number)
    uint32_t customer_id[8];    // customer ID
    uint32_t date;              // date information
    uint32_t batch_number[8];   // batch information
    uint32_t bin_number;        // bin information
} die_info_t;

typedef struct
{
    uint8_t  band_gap;
    uint8_t  reserved4[3];
    uint16_t vaon[16];
    uint16_t vcore[16];
    uint16_t v12_adc[19];
    uint16_t reserved0;
    uint32_t reserved1;
    uint32_t adc_calib_ver;     // ADC calibration data version
    uint16_t slow_rc[5];
    uint16_t reserved2;
    uint32_t reserved3[4];
} factory_calib_data_t;
#pragma pack (pop)

/**
 * @brief Prepare factory data in flash
 *
 * Factory data includes `die_info_t`, `factory_calib_data_t`,
 * ADC calibration data, etc.
 *
 * This function copies factory data from security pages to normal
 * pages in flash.
 *
 * This function succeeds if
 *   - factory data is ready in flash, or
 *   - factory data can be found in security pages and copied.
 *
 * This function can be called at the beginning of app.
 *
 * WARNING: If stack does not lie in EXECUTABLE memory region, I-Cache will be
 * switched to RAM mode temporarily and then restored to Cache.
 *
 * @return                      0 if succeeded else non-0
 */
int flash_prepare_factory_data(void);

/**
 * @brief Get die information from flash
 *
 * When such information does not exists, NULL is returned.
 *
 * This function uses `flash_prepare_factory_data()`.
 *
 * @return                      die information
 */
const die_info_t *flash_get_die_info(void);

/**
 * @brief Get factory calibration data from flash
 *
 * When such information does not exists, NULL is returned.
 *
 * This function uses `flash_prepare_factory_data()`.
 *
 * @return                      factory data
 */
const factory_calib_data_t *flash_get_factory_calib_data(void);

/**
 * @brief Get ADC calibration data from flash
 *
 * Note: There are multiple versions of ADC calibration data,
 *       so, here, a plain pointer is returned.
 *
 * This function uses `flash_prepare_factory_data()`.
 *
 * See `adc_calib_ver` in `factory_calib_data_t`.
 *
 * When such information does not exists, NULL is returned.
 *
 * @return                      ADC calibration data
 */
const void *flash_get_adc_calib_data(void);

/**
 * @brief Read one word from security area of flash
 *
 * Note: this is for internal use.
 */
uint32_t read_flash_security(uint32_t addr);

/**
 * @brief Read UID of flash
 *
 * @param[out]  uid            128-bit UID
 */
void flash_read_uid(uint32_t uid[4]);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

#define EFLASH_PAGE_SIZE        256
#define EFLASH_SECTOR_SIZE      4096
#define EFLASH_ERASABLE_SIZE    EFLASH_SECTOR_SIZE

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
 * CAUTION: FOTA is not supported on test chips using flash that can't support
 * continuous mode.
 *
 * @param[in] block_num         number of blocks
 * @param[in] blocks            an array of `fota_update_block_t`
 * @param[in] ram_buffer        buffer large enough for holding `EFLASH_ERASABLE_SIZE` bytes of data
 *                              CAUTION: DO NOT use memory from `SYSCTRL_SHARE_BLOCK_XXX` for this,
 *                                       while `SYSCTRL_MEM_BLOCK_XXX` and D-CACHE (as memory) are OK.
 * @return                      non-0 if error occurs else platform is reset
 */
int flash_do_update(const int block_num, const fota_update_block_t *blocks, uint8_t *ram_buffer);


typedef enum
{
    FLASH_REGION_NONE       = 0x00,         // none         (        0KB)
    FLASH_REGION_UPPER_1_8  = 0x01,         // upper 1/8    (upper  64KB)
    FLASH_REGION_UPPER_1_4  = 0x02,         // upper 1/4    (upper 128KB)
    FLASH_REGION_UPPER_1_2  = 0x03,         // upper 1/2    (upper 256KB)
    FLASH_REGION_LOWER_1_8  = 0x09,         // lower 1/8    (lower  64KB)
    FLASH_REGION_LOWER_1_4  = 0x0A,         // lower 1/4    (lower 128KB)
    FLASH_REGION_LOWER_1_2  = 0x0B,         // lower 1/2    (lower 256KB)
    FLASH_REGION_UPPER_1_128 = 0x11,        // upper 1/128  (upper   4KB)
    FLASH_REGION_UPPER_1_64  = 0x12,        // upper 1/64   (upper   8KB)
    FLASH_REGION_UPPER_1_32  = 0x13,        // upper 1/32   (upper  16KB)
    FLASH_REGION_UPPER_1_16  = 0x14,        // upper 1/16   (upper  32KB)
    FLASH_REGION_LOWER_1_128 = 0x19,        // lower 1/128  (lower   4KB)
    FLASH_REGION_LOWER_1_64  = 0x1A,        // lower 1/64   (lower   8KB)
    FLASH_REGION_LOWER_1_32  = 0x1B,        // lower 1/32   (lower  16KB)
    FLASH_REGION_LOWER_1_16  = 0x1C,        // lower 1/16   (lower  32KB)
    FLASH_REGION_ALL         = 0x1f,        // all          (      512KB)
} flash_region_t;

/**
 * @brief Enable write protection on a selected region
 *
 * e.g.
 *
 * ```c
 * // turn on write protection for the upper 1/8 of flash
 * flash_enable_write_protection(FLASH_REGION_UPPER_1_8, 0);
 *
 * // turn on write protection for the lower 7/8 of flash,
 * // i.e. reverse the selection of `FLASH_REGION_UPPER_1_8`
 * flash_enable_write_protection(FLASH_REGION_UPPER_1_8, 1);
 * ```
 *
 * Call this before doing write operations on flash if related region is write
 * protected. Select NONE region will disable the protection completely:
 *
 * ```c
 * flash_enable_write_protection(FLASH_REGION_NONE, 0);
 * ```
 *
 * Note: Write protection is a global configuration for the flash. It is impossible
 * to enable the protection for two separated region, such as `FLASH_REGION_UPPER_1_8`,
 * `FLASH_REGION_LOWER_1_128` by invoking this API twice.
 *
 * @param[in] region                specify a region
 * @param[in] reverse_selection     select `region` (0) or reverse the selection of `region` (1)
 */
void flash_enable_write_protection(flash_region_t region, uint8_t reverse_selection);


#pragma pack (push, 1)
typedef struct
{
    uint16_t version;
    uint8_t metal_id;
    uint8_t bin_number;
    uint8_t chip_uuid[16];
    uint16_t ate_version;
    uint16_t dut_version;
    uint8_t batch_number[16];
    uint16_t info_crc16;
    uint16_t trim_crc16;
} die_info_t;

typedef struct
{
    uint8_t version;
    uint8_t v33_index;
    uint16_t v33[8];
    uint8_t vaon_index;
    uint16_t vaon[8];
    uint8_t vdc33_index;
    uint16_t vdc33[16];
    uint8_t vcore_index;
    uint16_t vcore[16];
}factory_calib_pmu_t;

typedef struct {
    uint8_t version;
    uint16_t channel_mask;
    uint16_t int_vbat33_ain_ch0_8[9][2];
    uint16_t vbat33_flt_ain_ch0_8[9][2];
    uint16_t int_vbat33_int_ch9_11[3];
    uint16_t vbat33_flt_int_ch9_11[3];
    uint16_t vbat25_flt_int_ch9_11[3];
} factory_calib_adc_t;

typedef struct {
    uint8_t version;
    uint16_t verf;
    uint16_t mic_bias_int;
    uint16_t vp_2v0_vn0v0_int;
    uint16_t vp_0v0_vn2v0_int;
    uint16_t vp_1v5_int;
    uint16_t vp_0v5_int;
    uint16_t mic_bias_ext;
    uint16_t vp_2v0_vn0v0_ext;
    uint16_t vp_0v0_vn2v0_ext;
    uint16_t vp_1v5_ext;
    uint16_t vp_0v5_ext;
} factory_calib_asdm_t;

typedef struct
{
    factory_calib_pmu_t calib_pmu;
    factory_calib_adc_t calib_adc;
    factory_calib_asdm_t calib_asdm;
} factory_calib_data_t;

#define FACTORY_DATA_MAGIC_0    0x494E4743u
#define FACTORY_DATA_MAGIC_1    0x48495053u

typedef struct
{
    float k;
    float b;
} adc_linear_calib_t;

typedef struct
{
    float k;
    float b;
} adc_vbat_calib_t;

typedef struct
{
    uint16_t version;
    uint16_t flags;
    adc_linear_calib_t ch0_8_int_ref[9];
    adc_linear_calib_t ch0_8_vbat_ref[9];
    adc_vbat_calib_t ch9_vbat;
    uint32_t reserved[10];
    uint32_t magic_0;
    uint32_t magic_1;
} factory_clc_data_t;
#pragma pack (pop)

#define FACTORY_CLC_DATA_VERSION       0x0001u

/**
 * @brief Prepare factory data in flash
 *
 * This function copies factory data from security pages to normal
 * pages in flash when needed.
 *
 * @return                      0 if succeeded else non-0
 */
int flash_prepare_factory_data(void);

/**
 * @brief Get die information from flash
 *
 * When such information does not exists, NULL is returned.
 *
 * This function uses `flash_prepare_factory_data()`.
 *
 * @return                      die information
 */
const die_info_t *flash_get_die_info(void);

/**
 * @brief Get factory calibration data from flash
 *
 * When such information does not exists, NULL is returned.
 *
 * This function uses `flash_prepare_factory_data()`.
 *
 * @return                      factory data
 */
const factory_calib_data_t *flash_get_factory_calib_data(void);

/**
 * @brief Get calculated calibration data from flash
 *
 * This function uses `flash_prepare_factory_data()`.
 *
 * @return                      cached calibration data
 */
const factory_clc_data_t *flash_get_factory_clc_data(void);

/**
 * @brief Build calculated ADC calibration data from factory FT data
 *
 * @param[in] src               source FT data
 * @param[out] dst              calculated calibration data
 */
void flash_build_factory_clc_data(const factory_calib_data_t *src, factory_clc_data_t *dst);

/**
 * @brief Set Vcore value from FT data;
 *
 */
int Vcore_calib(void);

/**
 * @brief Read UID of flash
 *
 * @param[out]  uid            128-bit UID
 */
void flash_read_uid(uint32_t uid[4]);


/**
 * @brief OTP (One-Time Programmable) Flash API Definitions
 * * @note The OTP memory region is limited to 256 bytes total capacity.
 * * Restrictions:
 * - Programming: Only supports data write operations. Erase operations are NOT supported.
 * - Lock Mechanism: The flash_otp_enable function permanently locks the entire 
 * OTP region. Once called, the memory becomes read-only and cannot be modified.
 */
 
extern const unsigned char flash_api_bin[320];
/**
 * @brief Program data into the OTP region
 * @param[in] addr Target address within the OTP area (0 to 255)
 * @param[in] data Pointer to the buffer containing data to be written
 * @param[in] len  Number of bytes to write
 *
 * @return 0 on success, non-zero value if parameters are invalid or operation fails
 */
typedef int (*Func_FlashProgram)(uint32_t addr, void* data, uint32_t size);
#define flash_otp_program           ((Func_FlashProgram)(flash_api_bin + 0x01))

/**
 * @brief Read data from the OTP region
 * @param[in]  addr Target address within the OTP area (0 to 255)
 * @param[out] data Pointer to the buffer where read data will be stored
 * @param[in]  len  Number of bytes to read
 *
 * @return 0 on success, non-zero value if parameters are invalid or operation fails
 */
typedef int (*Func_FlashRead)(uint32_t addr, void* data, uint32_t size);
#define flash_otp_read              ((Func_FlashRead)(flash_api_bin + 0x65))

/**
 * @brief Permanently lock the OTP region
 * @warning Irreversible operation. Once called, the OTP area is hardware-locked
 * and cannot be modified again.
 */
typedef void (*Func_FlashOTP)(void);
#define flash_otp_enable            ((Func_FlashOTP)(flash_api_bin + 0xFD))

#endif

#ifdef __cplusplus
}
#endif

#endif

