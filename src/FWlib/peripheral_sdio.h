#ifndef PERIPHERAL_SDIO_H
#define PERIPHERAL_SDIO_H
#include "ingsoc.h"
#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
typedef struct {
    uint32_t cmd_timeout, data_timeout; /* 24-bit hardware timeout counts */
    uint8_t divider, width4, clock_invert, little_endian;
} SDIO_ConfigTypeDef;
typedef enum { SDIO_NO_RESPONSE, SDIO_SHORT_RESPONSE, SDIO_LONG_RESPONSE } SDIO_Response;
typedef enum { SDIO_NO_DATA, SDIO_READ_DATA, SDIO_WRITE_DATA } SDIO_DataDirection;

/**
 * @brief Reset and configure the SDIO controller.
 * @param[in] base SDIO controller instance.
 * @param[in] config Clock, bus-width, byte-order, and timeout configuration.
 * @return 0 on success; -1 if an argument or timeout value is invalid.
 */
int SDIO_Config(SDIO_TypeDef *base, const SDIO_ConfigTypeDef *config);

/**
 * @brief Configure the SDIO data-transfer buffer and block geometry.
 * @param[in] base SDIO controller instance.
 * @param[in] address Word-aligned DMA buffer address.
 * @param[in] block_bytes Number of bytes per block in the range 1 to 4096.
 * @param[in] blocks Number of blocks in the range 1 to 65536.
 * @return 0 on success; -1 if an argument is invalid.
 */
int SDIO_ConfigData(SDIO_TypeDef *base, uint32_t address, uint32_t block_bytes, uint32_t blocks);

/**
 * @brief Start an SDIO command transaction.
 * @param[in] base SDIO controller instance.
 * @param[in] command Command index in the range 0 to 63.
 * @param[in] argument Command argument.
 * @param[in] response Expected response type.
 * @param[in] direction Data-transfer direction, or @ref SDIO_NO_DATA for a command-only transaction.
 * @param[in] check_crc Nonzero to enable response CRC checking.
 * @param[in] check_index Nonzero to enable response command-index checking.
 * @param[in] wait_busy Nonzero to wait for the card busy indication to clear.
 * @return 0 on success; -1 if an argument is invalid.
 * @note The caller must serialize transactions and poll status events or service them by interrupt.
 */
int SDIO_SendCommand(SDIO_TypeDef *base, uint8_t command, uint32_t argument,
                     SDIO_Response response, SDIO_DataDirection direction,
                     uint8_t check_crc, uint8_t check_index, uint8_t wait_busy);

/**
 * @brief Read the latched command-event flags.
 * @param[in] base SDIO controller instance.
 * @return Command-event flags from the low five bits of the event register.
 */
uint32_t SDIO_GetCommandStatus(SDIO_TypeDef *base);

/**
 * @brief Read the latched data-event flags.
 * @param[in] base SDIO controller instance.
 * @return Data-event flags from the low four bits of the event register.
 */
uint32_t SDIO_GetDataStatus(SDIO_TypeDef *base);

/**
 * @brief Clear selected command-event flags.
 * @param[in] base SDIO controller instance.
 * @param[in] flags Command-event flags to clear; only the low five bits are used.
 */
void SDIO_ClearCommandStatus(SDIO_TypeDef *base, uint32_t flags);

/**
 * @brief Clear selected data-event flags.
 * @param[in] base SDIO controller instance.
 * @param[in] flags Data-event flags to clear; only the low four bits are used.
 */
void SDIO_ClearDataStatus(SDIO_TypeDef *base, uint32_t flags);

/**
 * @brief Configure command and data interrupt masks.
 * @param[in] base SDIO controller instance.
 * @param[in] command_mask Command-event interrupt mask; only the low five bits are used.
 * @param[in] data_mask Data-event interrupt mask; only the low four bits are used.
 */
void SDIO_IntEnable(SDIO_TypeDef *base, uint32_t command_mask, uint32_t data_mask);

/**
 * @brief Read one word from the SDIO response registers.
 * @param[in] base SDIO controller instance.
 * @param[in] word Response-word index in the range 0 to 3.
 * @return Selected response word, or 0 if the index is invalid.
 */
uint32_t SDIO_GetResponse(SDIO_TypeDef *base, uint8_t word);

#endif /* INGCHIPS_FAMILY_26 */

#ifdef __cplusplus
}
#endif

#endif
