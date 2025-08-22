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
int DMA_PrepareMem2Mem(DMA_Descriptor *pDesc,
                       void *dst, void *src, int size,
                       DMA_AddressControl dst_addr_ctrl, DMA_AddressControl src_addr_ctrl,
                       uint32_t options);

/**
 * @brief Prepare DMA descriptor for peripheral to memory transfer
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination address
 * @param[in] src               source peripheral
 * @param[in] size              size in byte
 * @param[in] dst_addr_ctrl     destination address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PreparePeripheral2Mem(DMA_Descriptor *pDesc,
                              uint32_t *dst, SYSCTRL_DMA src, int size,
                              DMA_AddressControl dst_addr_ctrl,
                              uint32_t options);

/**
 * @brief Prepare DMA descriptor for memory to peripheral transfer
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination peripheral
 * @param[in] src               source address
 * @param[in] size              size in byte
 * @param[in] src_addr_ctrl     source address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PrepareMem2Peripheral(DMA_Descriptor *pDesc,
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
 * @param[out] *srcWidth        size(bits) per unit, refer to DMA_TransferWidth
 * @param[out] *srcSize         size in unit per transfer, real transfer num is 1<<srcSize
 * @return                      0 if no error else non-0
 * Application need to check the return value of *srcWidth and *srcSize and set corresponding
 * fifo trigger level.
 * for example, if srcWidth is DMA_WIDTH_BYTE, and srcSize is 2(which means 1<<2 = 4)
 * then the fifo trigger level should be set to 4bytes(srcWidth*(1<<srcSize))
 * so that src fifo will issue a dma req every 4bytes to trigger dma transaction.
 */
int DMA_PreparePeripheral2Peripheral(DMA_Descriptor *pDesc,
                                     SYSCTRL_DMA dst, SYSCTRL_DMA src, int size,
                                     uint32_t options, uint8_t *srcWidth, uint8_t *srcSize);
/**
 * @brief Reset DMA peripheral
 *
 * @param[in] reset             reset (1) or release (0)
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


/**
 * @brief Modify source burst size in the DMA descriptor
 *
 * @param[in] pDesc             the descriptor
 * @param[in] burst_size        Source burst size of `DMA_SrcBurstSize`, The burst transfer byte number is SrcBurstSize * SrcWidth.
 */
void DMA_ConfigSrcBurstSize(DMA_Descriptor *pDesc, DMA_SrcBurstSize burst_size);

/**
 * @brief Get DMA descriptor of a channel
 *
 * @param[in] channel_id                channel id
 * @return                              DMA descriptor
 */
DMA_Descriptor *DMA_GetChannelDescriptor(int channel_id);

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_920)

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
int DMA_PrepareMem2Mem(DMA_Descriptor *pDesc,
                       void *dst, void *src, int size,
                       DMA_AddressControl dst_addr_ctrl, DMA_AddressControl src_addr_ctrl,
                       uint32_t options);

/**
 * @brief Prepare DMA descriptor for peripheral to memory transfer
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination address
 * @param[in] src               source peripheral
 * @param[in] size              size in byte
 * @param[in] dst_addr_ctrl     destination address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PreparePeripheral2Mem(DMA_Descriptor *pDesc,
                              uint32_t *dst, SYSCTRL_DMA src, int size,
                              DMA_AddressControl dst_addr_ctrl,
                              uint32_t options);

/**
 * @brief Prepare DMA descriptor for memory to peripheral transfer
 *
 * @param[in] pDesc             the descriptor
 * @param[in] dst               destination peripheral
 * @param[in] src               source address
 * @param[in] size              size in byte
 * @param[in] src_addr_ctrl     source address control
 * @param[in] options           bit combination of `DMA_DescriptorOptionBit`
 * @return                      0 if no error else non-0
 */
int DMA_PrepareMem2Peripheral(DMA_Descriptor *pDesc,
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
 * @param[out] *srcWidth        size(bits) per unit, refer to DMA_TransferWidth
 * @param[out] *srcSize         size in unit per transfer, real transfer num is 1<<srcSize
 * @return                      0 if no error else non-0
 * Application need to check the return value of *srcWidth and *srcSize and set corresponding
 * fifo trigger level.
 * for example, if srcWidth is DMA_WIDTH_BYTE, and srcSize is 2(which means 1<<2 = 4)
 * then the fifo trigger level should be set to 4bytes(srcWidth*(1<<srcSize))
 * so that src fifo will issue a dma req every 4bytes to trigger dma transaction.
 */
int DMA_PreparePeripheral2Peripheral(DMA_Descriptor *pDesc,
                                     SYSCTRL_DMA dst, SYSCTRL_DMA src, int size,
                                     uint32_t options, uint8_t *srcWidth, uint8_t *srcSize);
/**
 * @brief Reset DMA peripheral
 *
 * @param[in] reset             reset (1) or release (0)
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

