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
int SDIO_Config(SDIO_TypeDef *base, const SDIO_ConfigTypeDef *config);
/** Program DMA buffer (word aligned), 1..4096 bytes/block, 1..65536 blocks. */
int SDIO_ConfigData(SDIO_TypeDef *base, uint32_t address, uint32_t block_bytes, uint32_t blocks);
/** Launch command; caller must serialize transactions and poll events or use IRQ. */
int SDIO_SendCommand(SDIO_TypeDef *base, uint8_t command, uint32_t argument,
                     SDIO_Response response, SDIO_DataDirection direction,
                     uint8_t check_crc, uint8_t check_index, uint8_t wait_busy);
uint32_t SDIO_GetCommandStatus(SDIO_TypeDef *base);
uint32_t SDIO_GetDataStatus(SDIO_TypeDef *base);
void SDIO_ClearCommandStatus(SDIO_TypeDef *base, uint32_t flags);
void SDIO_ClearDataStatus(SDIO_TypeDef *base, uint32_t flags);
void SDIO_IntEnable(SDIO_TypeDef *base, uint32_t command_mask, uint32_t data_mask);
uint32_t SDIO_GetResponse(SDIO_TypeDef *base, uint8_t word);

#endif /* INGCHIPS_FAMILY_26 */

#ifdef __cplusplus
}
#endif

#endif
