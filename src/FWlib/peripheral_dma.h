#ifndef __PERIPHERAL_DMA_H__
#define __PERIPHERAL_DMA_H__

#ifdef	__cplusplus
extern "C" {	/* allow C++ to use these headers */
#endif	/* __cplusplus */

#include "ingsoc.h"
#include "peripheral_sysctrl.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#define DMA_CHANNEL_NUMBER          8

typedef enum
{
    DMA_ADDRESS_INC   = 0,
    DMA_ADDRESS_DEC   = 1,
    DMA_ADDRESS_FIXED = 2,
} DMA_AddressControl;

typedef enum
{
    DMA_DESC_OPTION_BIT_ENABLE = 0,
    DMA_DESC_OPTION_BIT_DISABLE_TERMINAL_COUNT_INT = 1,
    DMA_DESC_OPTION_BIT_DISABLE_ERROR_INT = 2,
    DMA_DESC_OPTION_BIT_DISABLE_ABORT_INT = 3,
    DMA_DESC_OPTION_BIT_INTERRUPT_EACH_DESC = 28,
    DMA_DESC_OPTION_BIT_HIGH_PRIORITY = 29,
} DMA_DescriptorOptionBit;

/* Encoded source transfer count per burst: transfers = 1u << enum_value.
 * Burst bytes = transfers * source_width_bytes. Match the peripheral FIFO
 * request threshold to this byte count.
 */
typedef enum
{
    DMA_BURST_SIZE_1   = 0,
    DMA_BURST_SIZE_2   = 1,
    DMA_BURST_SIZE_4   = 2,
    DMA_BURST_SIZE_8   = 3,
    DMA_BURST_SIZE_16  = 4,
    DMA_BURST_SIZE_32  = 5,
    DMA_BURST_SIZE_64  = 6,
    DMA_BURST_SIZE_128 = 7,
} DMA_SrcBurstSize;

#define DMA_MAKE_BURST_SIZE_OPTION(size)        (((uint32_t)(size) & 0x7) << 24)

/**
 * @brief Prepare DMA descriptor for memory to memory transfer
 * @note Prepare fills Ctrl, TranSize and data addresses. Initialize the descriptor
 * to zero and set Next explicitly. It neither allocates buffers nor starts DMA.
 * TranSize counts SOURCE accesses; total bytes = TranSize * source_width_bytes.
 * The caller must ensure 1 <= TranSize <= 0x3fffff and valid buffer lifetimes.
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination address
 * @param[in] src               source address
 * @param[in] size              total byte count, independent of pointer type
 * @param[in] dst_addr_ctrl     destination address control
 * @param[in] src_addr_ctrl     source address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PrepareMem2Mem(DMA_Descriptor *pDesc,
                       void *dst, void *src, uint32_t size,
                       DMA_AddressControl dst_addr_ctrl, DMA_AddressControl src_addr_ctrl,
                       uint32_t options);

/**
 * @brief Prepare DMA descriptor for peripheral to memory transfer
 * @note size is bytes. TranSize is size / source_width_bytes. The peripheral
 * configuration determines source access width, which can differ from sample
 * width. uint32_t * is the API pointer type; destination access width is selected
 * from source width and the actual destination address alignment.
 * Initialize the descriptor to zero, set Next explicitly, and keep storage alive.
 * Prepare neither allocates buffers nor starts DMA. Zero/oversized counts must
 * be rejected by the caller (valid TranSize is 1..0x3fffff).
 * Burst bits supplied in options are ORed with the inferred burst encoding.
 * Use DMA_ConfigSrcBurstSize after Prepare to replace that field explicitly.
 * Validate burst bytes against destination width and peripheral FIFO requests.
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination address
 * @param[in] src               source peripheral
 * @param[in] size              total byte count, independent of pointer type
 * @param[in] dst_addr_ctrl     destination address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PreparePeripheral2Mem(DMA_Descriptor *pDesc,
                              uint32_t *dst, SYSCTRL_DMA src, uint32_t size,
                              DMA_AddressControl dst_addr_ctrl,
                              uint32_t options);

/**
 * @brief Prepare DMA descriptor for memory to peripheral transfer
 * @note size is bytes. Source access width is inferred from memory alignment,
 * length and peripheral width; TranSize counts these SOURCE accesses.
 * uint32_t * does not force word accesses. Initialize the descriptor to zero and
 * set Next explicitly. Buffers must remain valid until DMA has stopped.
 * Validate 1 <= TranSize <= 0x3fffff. Prepare does not start the channel.
 * options burst bits are ORed with inferred bits; DMA_ConfigSrcBurstSize replaces
 * the field. A replacement must preserve destination-width and FIFO constraints.
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination peripheral
 * @param[in] src               source address
 * @param[in] size              total byte count, independent of pointer type
 * @param[in] src_addr_ctrl     source address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PrepareMem2Peripheral(DMA_Descriptor *pDesc,
                              SYSCTRL_DMA dst, uint32_t *src, uint32_t size,
                              DMA_AddressControl src_addr_ctrl,
                              uint32_t options);

/**
 * @brief Prepare DMA descriptor for peripheral to peripheral transfer
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination peripheral
 * @param[in] src               source peripheral
 * @param[in] size              total byte count, independent of pointer type
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @param[out] *srcWidth        source width encoding (0/1/2); bytes per access = 1u << *srcWidth
 * @param[out] *srcSize         burst encoding; source accesses per burst = 1u << *srcSize
 * @return                      0 if no error else non-0
 * Application need to check the return value of *srcWidth and *srcSize and set corresponding
 * fifo trigger level.
 * for example, if srcWidth is DMA_WIDTH_BYTE, and srcSize is 2(which means 1<<2 = 4)
 * then burst bytes = (1u << *srcWidth) * (1u << *srcSize) = 4.
 * so that src fifo will issue a dma req every 4bytes to trigger dma transaction.
 */
int DMA_PreparePeripheral2Peripheral(DMA_Descriptor *pDesc,
                                     SYSCTRL_DMA dst, SYSCTRL_DMA src, uint32_t size,
                                     uint32_t options, uint8_t *srcWidth, uint8_t *srcSize);
/**
 * @brief Reset DMA peripheral
 *
 * @note Reset affects the entire DMA core and disables ALL channels.
 * @param[in] reset             write reset command (1), or clear control bit (0)
 */
void DMA_Reset(uint8_t reset);

/**
 * @brief Check if a DMA descriptor's address is valid
 *
 * Rules:
 * * Each descriptor shall not cross the 4K byte boundary;
 * * Each descriptor shall be aligned at 8 bytes boundary.
 *
 * Tips: use `__attribute__((aligned (8)))` to align the descriptor, e.g.
 *
 * ```c
 * DMA_Descriptor desc __attribute__((aligned (8)));
 * ```
 *
 * @result                      true if valid, false if invalid
 */
#define IS_VALID_DMA_DESC_ADDR(p)  \
    (((((uint32_t)(p)) & 0xfff) <= (0x1000 - sizeof(DMA_Descriptor))) \
    && ((((uint32_t)(p)) & 0x7) == 0))

/**
 * @brief Enable (Start) a DMA channel
 * @note Copies first into the channel registers, then sets Enable. Only call on
 * an idle channel. Next == NULL selects a single block. For a single block,
 * handle TC/error/abort status before reloading and starting the next block.
 *
 * Caution: All DMA descriptors' address in the chain that is pointed to by a
 * preceeding descriptor shall be valid, i.e. `IS_VALID_DMA_DESC_ADDR` evaluated to true.
 *
 * @param[in] channel_id            channel id to be enabled/started
 * @param[in] first                 first descriptor
 */
void DMA_EnableChannel(int channel_id, DMA_Descriptor *first);

/**
 * @brief Abort a DMA channel
 * @note Issues an abort request. Wait until the channel enable status clears
 * before releasing buffers, then clear abort/error/TC status as appropriate.
 * Abort is ignored by hardware when the channel is already disabled.
 *
 * @param[in] channel_id            channel id to be enabled/started
 */
void DMA_AbortChannel(int channel_id);

/**
 * @brief `memcpy` backed by DMA
 *
 * Note: This function is a demo of DMA, not thread-safe, and slower than `memcpy`.
 *
 * @param[in] channel_id        channel id to be used
 * @param[in] dst               destination address
 * @param[in] src               source address
 * @param[in] size              total byte count, independent of pointer type
 * @return                      0 if ok else non-0
 */
int DMA_MemCopy(int channel_id, void *dst, void *src, uint32_t size);

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


/**
 * @brief Modify source burst size in the DMA descriptor
 *
 * @param[in] pDesc             the descriptor
 * @param[in] burst_size        encoded DMA_SrcBurstSize; burst bytes = (1u << burst_size)
 *                              * source_width_bytes. Replaces the burst field.
 * @note Apply to an inactive descriptor. The caller must validate alignment and
 * peripheral FIFO request thresholds after changing the burst.
 */
void DMA_ConfigSrcBurstSize(DMA_Descriptor *pDesc, DMA_SrcBurstSize burst_size);

/**
 * @brief Get DMA descriptor of a channel
 *
 * @param[in] channel_id                channel id
 * @return                              DMA descriptor
 */
DMA_Descriptor *DMA_GetChannelDescriptor(int channel_id);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

#define DMA_CHANNEL_NUMBER          8

typedef enum
{
    DMA_ADDRESS_INC   = 0,
    DMA_ADDRESS_DEC   = 1,
    DMA_ADDRESS_FIXED = 2,
} DMA_AddressControl;

typedef enum
{
    DMA_DESC_OPTION_BIT_ENABLE = 0,
    DMA_DESC_OPTION_BIT_DISABLE_TERMINAL_COUNT_INT = 1,
    DMA_DESC_OPTION_BIT_DISABLE_ERROR_INT = 2,
    DMA_DESC_OPTION_BIT_DISABLE_ABORT_INT = 3,
    DMA_DESC_OPTION_BIT_INTERRUPT_EACH_DESC = 28,
    DMA_DESC_OPTION_BIT_HIGH_PRIORITY = 29,
} DMA_DescriptorOptionBit;

/* Encoded source transfer count per burst: transfers = 1u << enum_value.
 * Burst bytes = transfers * source_width_bytes. Match the peripheral FIFO
 * request threshold to this byte count.
 */
typedef enum
{
    DMA_BURST_SIZE_1   = 0,
    DMA_BURST_SIZE_2   = 1,
    DMA_BURST_SIZE_4   = 2,
    DMA_BURST_SIZE_8   = 3,
    DMA_BURST_SIZE_16  = 4,
    DMA_BURST_SIZE_32  = 5,
    DMA_BURST_SIZE_64  = 6,
    DMA_BURST_SIZE_128 = 7,
} DMA_SrcBurstSize;

#define DMA_MAKE_BURST_SIZE_OPTION(size)        (((uint32_t)(size) & 0x7) << 24)

/**
 * @brief Prepare DMA descriptor for memory to memory transfer
 * @note Prepare fills Ctrl, TranSize and data addresses. Initialize the descriptor
 * to zero and set Next explicitly. It neither allocates buffers nor starts DMA.
 * TranSize counts SOURCE accesses; total bytes = TranSize * source_width_bytes.
 * The caller must ensure 1 <= TranSize <= 0x3fffff and valid buffer lifetimes.
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination address
 * @param[in] src               source address
 * @param[in] size              total byte count, independent of pointer type
 * @param[in] dst_addr_ctrl     destination address control
 * @param[in] src_addr_ctrl     source address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PrepareMem2Mem(DMA_Descriptor *pDesc,
                       void *dst, void *src, uint32_t size,
                       DMA_AddressControl dst_addr_ctrl, DMA_AddressControl src_addr_ctrl,
                       uint32_t options);

/**
 * @brief Prepare DMA descriptor for peripheral to memory transfer
 * @note size is bytes. TranSize is size / source_width_bytes. The peripheral
 * configuration determines source access width, which can differ from sample
 * width. uint32_t * is the API pointer type; destination access width is selected
 * from source width and the actual destination address alignment.
 * Initialize the descriptor to zero, set Next explicitly, and keep storage alive.
 * Prepare neither allocates buffers nor starts DMA. Zero/oversized counts must
 * be rejected by the caller (valid TranSize is 1..0x3fffff).
 * Burst bits supplied in options are ORed with the inferred burst encoding.
 * Use DMA_ConfigSrcBurstSize after Prepare to replace that field explicitly.
 * Validate burst bytes against destination width and peripheral FIFO requests.
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination address
 * @param[in] src               source peripheral
 * @param[in] size              total byte count, independent of pointer type
 * @param[in] dst_addr_ctrl     destination address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PreparePeripheral2Mem(DMA_Descriptor *pDesc,
                              uint32_t *dst, SYSCTRL_DMA src, uint32_t size,
                              DMA_AddressControl dst_addr_ctrl,
                              uint32_t options);

/**
 * @brief Prepare DMA descriptor for memory to peripheral transfer
 * @note size is bytes. Source access width is inferred from memory alignment,
 * length and peripheral width; TranSize counts these SOURCE accesses.
 * uint32_t * does not force word accesses. Initialize the descriptor to zero and
 * set Next explicitly. Buffers must remain valid until DMA has stopped.
 * Validate 1 <= TranSize <= 0x3fffff. Prepare does not start the channel.
 * options burst bits are ORed with inferred bits; DMA_ConfigSrcBurstSize replaces
 * the field. A replacement must preserve destination-width and FIFO constraints.
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination peripheral
 * @param[in] src               source address
 * @param[in] size              total byte count, independent of pointer type
 * @param[in] src_addr_ctrl     source address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PrepareMem2Peripheral(DMA_Descriptor *pDesc,
                              SYSCTRL_DMA dst, uint32_t *src, uint32_t size,
                              DMA_AddressControl src_addr_ctrl,
                              uint32_t options);

/**
 * @brief Prepare DMA descriptor for peripheral to peripheral transfer
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination peripheral
 * @param[in] src               source peripheral
 * @param[in] size              total byte count, independent of pointer type
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @param[out] *srcWidth        source width encoding (0/1/2); bytes per access = 1u << *srcWidth
 * @param[out] *srcSize         burst encoding; source accesses per burst = 1u << *srcSize
 * @return                      0 if no error else non-0
 * Application need to check the return value of *srcWidth and *srcSize and set corresponding
 * fifo trigger level.
 * for example, if srcWidth is DMA_WIDTH_BYTE, and srcSize is 2(which means 1<<2 = 4)
 * then burst bytes = (1u << *srcWidth) * (1u << *srcSize) = 4.
 * so that src fifo will issue a dma req every 4bytes to trigger dma transaction.
 */
int DMA_PreparePeripheral2Peripheral(DMA_Descriptor *pDesc,
                                     SYSCTRL_DMA dst, SYSCTRL_DMA src, uint32_t size,
                                     uint32_t options, uint8_t *srcWidth, uint8_t *srcSize);
/**
 * @brief Reset DMA peripheral
 *
 * @note Reset affects the entire DMA core and disables ALL channels.
 * @param[in] reset             write reset command (1), or clear control bit (0)
 */
void DMA_Reset(uint8_t reset);

/**
 * @brief Check if a DMA descriptor's address is valid
 *
 * Rules:
 * * Each descriptor shall not cross the 4K byte boundary;
 * * Each descriptor shall be aligned at 8 bytes boundary.
 *
 * Tips: use `__attribute__((aligned (8)))` to align the descriptor, e.g.
 *
 * ```c
 * DMA_Descriptor desc __attribute__((aligned (8)));
 * ```
 *
 * @result                      true if valid, false if invalid
 */
#define IS_VALID_DMA_DESC_ADDR(p)  \
    (((((uint32_t)(p)) & 0xfff) <= (0x1000 - sizeof(DMA_Descriptor))) \
    && ((((uint32_t)(p)) & 0x7) == 0))

/**
 * @brief Enable (Start) a DMA channel
 * @note Copies first into the channel registers, then sets Enable. Only call on
 * an idle channel. Next == NULL selects a single block. For a single block,
 * handle TC/error/abort status before reloading and starting the next block.
 *
 * Caution: All DMA descriptors' address in the chain that is pointed to by a
 * preceeding descriptor shall be valid, i.e. `IS_VALID_DMA_DESC_ADDR` evaluated to true.
 *
 * @param[in] channel_id            channel id to be enabled/started
 * @param[in] first                 first descriptor
 */
void DMA_EnableChannel(int channel_id, DMA_Descriptor *first);

/**
 * @brief Abort a DMA channel
 * @note Issues an abort request. Wait until the channel enable status clears
 * before releasing buffers, then clear abort/error/TC status as appropriate.
 * Abort is ignored by hardware when the channel is already disabled.
 *
 * @param[in] channel_id            channel id to be enabled/started
 */
void DMA_AbortChannel(int channel_id);

/**
 * @brief `memcpy` backed by DMA
 *
 * Note: This function is a demo of DMA, not thread-safe, and slower than `memcpy`.
 *
 * @param[in] channel_id        channel id to be used
 * @param[in] dst               destination address
 * @param[in] src               source address
 * @param[in] size              total byte count, independent of pointer type
 * @return                      0 if ok else non-0
 */
int DMA_MemCopy(int channel_id, void *dst, void *src, uint32_t size);

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

/**
 * @brief Modify source burst size in the DMA descriptor
 *
 * @param[in] pDesc             the descriptor
 * @param[in] burst_size        encoded DMA_SrcBurstSize; burst bytes = (1u << burst_size)
 *                              * source_width_bytes. Replaces the burst field.
 * @note Apply to an inactive descriptor. The caller must validate alignment and
 * peripheral FIFO request thresholds after changing the burst.
 */
void DMA_ConfigSrcBurstSize(DMA_Descriptor *pDesc, DMA_SrcBurstSize burst_size);

/*
* @brief Get DMA descriptor of a channel
*
* @param[in] channel_id                channel id
* @return                              DMA descriptor
*/
DMA_Descriptor *DMA_GetChannelDescriptor(int channel_id);

#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif

