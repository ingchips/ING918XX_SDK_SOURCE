#include "peripheral_sdio.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_26)
int SDIO_Config(SDIO_TypeDef *base, const SDIO_ConfigTypeDef *config)
{
    if (!base || !config || config->cmd_timeout > 0xffffffu || config->data_timeout > 0xffffffu) return -1;
    base->mmc_reset = 1u;
    base->mmc_reset = 0u;
    base->mmc_cmd_timeout = config->cmd_timeout;
    base->mmc_data_timeout = config->data_timeout;
    base->mmc_div = config->divider;
    base->mmc_ctrl = (config->width4 != 0) | ((uint32_t)(config->clock_invert != 0) << 1)
                      | 0x2cu | ((uint32_t)(config->little_endian != 0) << 4);
    base->mmc_cmd_event_mask = 0;
    base->mmc_data_event_mask = 0;
    base->mmc_cmd_event = 0x1fu;
    base->mmc_data_event = 0xfu;
    return 0;
}
int SDIO_ConfigData(SDIO_TypeDef *base, uint32_t address, uint32_t block_bytes, uint32_t blocks)
{
    if (!base || (address & 3u) || !block_bytes || block_bytes > 4096u || !blocks || blocks > 65536u) return -1;
    base->mmc_block_size = block_bytes - 1u;
    base->mmc_block_count = blocks - 1u;
    base->mmc_data_buf_addr = address;
    return 0;
}
int SDIO_SendCommand(SDIO_TypeDef *base, uint8_t command, uint32_t argument,
                     SDIO_Response response, SDIO_DataDirection direction,
                     uint8_t check_crc, uint8_t check_index, uint8_t wait_busy)
{
    uint32_t response_bits;
    if (!base || command > 63 || (unsigned int)response > SDIO_LONG_RESPONSE ||
        (unsigned int)direction > SDIO_WRITE_DATA)
        return -1;
    response_bits = response == SDIO_NO_RESPONSE ? 0u : response == SDIO_SHORT_RESPONSE ? 1u : 2u;
    base->mmc_cmd_event = 0x1fu;
    if (direction != SDIO_NO_DATA) base->mmc_data_event = 0xfu;
    base->mmc_arg = argument;
    base->mmc_cmd_cfg = response_bits | ((uint32_t)(wait_busy != 0) << 2)
                      | ((uint32_t)(check_crc != 0) << 3) | ((uint32_t)(check_index != 0) << 4)
                      | ((uint32_t)direction << 5) | ((uint32_t)command << 8);
    return 0;
}
uint32_t SDIO_GetCommandStatus(SDIO_TypeDef *base) { return base->mmc_cmd_event & 0x1fu; }
uint32_t SDIO_GetDataStatus(SDIO_TypeDef *base) { return base->mmc_data_event & 0xfu; }
void SDIO_ClearCommandStatus(SDIO_TypeDef *base, uint32_t flags) { base->mmc_cmd_event = flags & 0x1fu; }
void SDIO_ClearDataStatus(SDIO_TypeDef *base, uint32_t flags) { base->mmc_data_event = flags & 0xfu; }
void SDIO_IntEnable(SDIO_TypeDef *base, uint32_t command_mask, uint32_t data_mask)
{
    base->mmc_cmd_event_mask = command_mask & 0x1fu;
    base->mmc_data_event_mask = data_mask & 0xfu;
}
uint32_t SDIO_GetResponse(SDIO_TypeDef *base, uint8_t word)
{
    switch (word) {
        case 0:
            return base->mmc_resp[0];
        case 1:
            return base->mmc_resp[1];
        case 2:
            return base->mmc_resp[2];
        case 3:
            return base->mmc_resp[3];
        default:
            return 0;
    }
}

#endif
