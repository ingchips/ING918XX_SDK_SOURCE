#ifndef __PERIPHERAL_DMA_H__
#define __PERIPHERAL_DMA_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"
#include "peripheral_sysctrl.h"

#if INGCHIPS_FAMILY == INGCHIPS_FAMILY_916

#define DMA_CHANNEL_NUMBER          8

typedef enum
{
    DMA_ADDRESS_INC   = 0,
    DMA_ADDRESS_DEC   = 1,
    DMA_ADDRESS_FIXED = 2,
} DMA_AddressControl;

typedef enum
{
    DMA_DESC_OPTION_BIT_DISABLE_TERMINAL_COUNT_INT = 1,
    DMA_DESC_OPTION_BIT_DISABLE_ERROR_INT = 2,
    DMA_DESC_OPTION_BIT_DISABLE_ABORT_INT = 3,
    DMA_DESC_OPTION_BIT_INTERRUPT_EACH_DESC = 28,
    DMA_DESC_OPTION_BIT_HIGH_PRIORITY = 29,
} DMA_DescriptorOptionBit;

#define DMA_MAKE_BURST_SIZE_OPTION(size)        (((uint32_t)(size) & 0x7) << 24)

/**
 * @brief Prepare DMA descriptor for RAM to RAM transfer
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination address
 * @param[in] src               source address
 * @param[in] size              size in byte
 * @param[in] dst_addr_ctrl     destination address control
 * @param[in] src_addr_ctrl     source address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PrepareRAM2RAM(DMA_Descriptor *pDesc,
                        void *dst, void *src, int size,
                        DMA_AddressControl dst_addr_ctrl, DMA_AddressControl src_addr_ctrl,
                        uint32_t options);

/**
 * @brief Prepare DMA descriptor for peripheral to RAM transfer
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination address
 * @param[in] src               source peripheral
 * @param[in] size              size in byte
 * @param[in] dst_addr_ctrl     destination address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PreparePeripheral2RAM(DMA_Descriptor *pDesc,
                            uint32_t *dst, SYSCTRL_DMA src, int size,
                            DMA_AddressControl dst_addr_ctrl,
                            uint32_t options);

/**
 * @brief Prepare DMA descriptor for RAM to peripheral transfer
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination peripheral
 * @param[in] src               source address
 * @param[in] size              size in byte
 * @param[in] src_addr_ctrl     source address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PrepareRAM2Peripheral(DMA_Descriptor *pDesc,
                            SYSCTRL_DMA dst, uint32_t *src, int size,
                            DMA_AddressControl src_addr_ctrl,
                            uint32_t options);

/**
 * @brief Prepare DMA descriptor for RAM to peripheral transfer
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination peripheral
 * @param[in] src               source peripheral
 * @param[in] size              size in byte
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PreparePeripheral2Peripheral(DMA_Descriptor *pDesc,
                                    SYSCTRL_DMA dst, SYSCTRL_DMA src, int size,
                                    uint32_t options);

void DMA_Reset(uint8_t reset);

void DMA_EnableChannel(int channel_id, DMA_Descriptor *first);
void DMA_AbortChannel(int channel_id);

/**
 * @brief `memcpy` backed by DMA
 *
 * Note: This function is a demo of DMA, not thread-safe, and slower than `memcpy`.
 *
 * @param[in] channel_id        channel id to be used
 * @param[in] dst               destination address
 * @param[in] src               source address
 * @param[in] size              size in byte
 * @return                      0 if ok else non-0
 */
int DMA_MemCopy(int channel_id, void *dst, void *src, int size);

typedef enum
{
    DMA_IRQ_ERROR = 1,
    DMA_IRQ_ABORT = 2,
    DMA_IRQ_TC = 4,
} DMA_IRQ;

/**
 * @brief Get interrupt state of a channel
 *
 * @param[in] channel_id                channel id
 * @return                              interrupt state (combination of `DMA_IRQ`)
 */
uint32_t DMA_GetChannelIntState(int channel_id);

/**
 * @brief Clear interrupt state of a channel
 *
 * @param[in] channel_id                channel id
 * @param[in] state                     interrupt state (combination of `DMA_IRQ`) to be cleared.
 */
void DMA_ClearChannelIntState(int channel_id, uint32_t state);

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

