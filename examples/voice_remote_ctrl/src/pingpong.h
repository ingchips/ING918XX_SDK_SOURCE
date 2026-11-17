#ifndef __PINGPONG_H__
#define __PINGPONG_H__

#include "peripheral_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916 || INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

/* Define consistently for pingpong.c and all callers through build options. */
#ifndef DMA_PINGPONG_USE_MALLOC
#define DMA_PINGPONG_USE_MALLOC 0
#endif

#if DMA_PINGPONG_USE_MALLOC != 0 && DMA_PINGPONG_USE_MALLOC != 1
#error DMA_PINGPONG_USE_MALLOC must be 0 or 1
#endif

typedef enum
{
    DMA_PINGPONG_ERROR_INVALID = -1,
    DMA_PINGPONG_ERROR_BUSY = -2,
    DMA_PINGPONG_ERROR_MEMORY = -3,
    DMA_PINGPONG_ERROR_TRANSFER = -4,
    DMA_PINGPONG_ERROR_TIMEOUT = -5,
} DMA_PingPongError;

typedef enum
{
    DMA_PINGPONG_STOPPED,
    DMA_PINGPONG_RUNNING,
    DMA_PINGPONG_STOPPING,
} DMA_PingPongState;

typedef struct
{
    void *buffers[2];
    uint32_t buffer_bytes;
    uint32_t buffer_capacity_bytes;
    SYSCTRL_DMA source;
    DMA_SrcBurstSize burst_size;
    DMA_Descriptor descriptors[2];
    uint8_t active_buffer;
    uint8_t channel;
    volatile DMA_PingPongState state;
    uint8_t configured;
} DMA_PingPong_t;

/**
 * Configure capture. Initialize ctx to zero before first use.
 * buffer_bytes is the byte count of EACH buffer, independent of source width.
 * burst_size is a DMA_SrcBurstSize encoding. Length is never rounded down.
 * Serialize setup/binding/start/stop calls in one application context.
 * Configure the DMA clock and request routing before Enable.
 * Returns 0 on success or a negative DMA_PingPongError.
 */
int DMA_PingPongSetup(DMA_PingPong_t *ctx, SYSCTRL_DMA source,
                     uint32_t buffer_bytes, DMA_SrcBurstSize burst_size);

#if !DMA_PINGPONG_USE_MALLOC
/**
 * Bind two non-overlapping buffers, each at least capacity_bytes in size.
 * Call after Setup. Keep buffers alive until Disable succeeds.
 */
int DMA_PingPongSetBuffers(DMA_PingPong_t *ctx, void *ping_buffer,
                          void *pong_buffer, uint32_t capacity_bytes);
#endif

/**
 * Start an idle channel, then enable peripheral DMA requests externally.
 * Malloc mode allocates two buffer_bytes buffers, with failure rollback.
 */
int DMA_PingPongEnable(DMA_PingPong_t *ctx, uint8_t channel);

/**
 * Handle this instance from the DMA ISR, including status checking/clearing.
 * Returns 1 with a completed buffer, 0 for no completion, or a negative error.
 * The next buffer starts before returning. Consume the completed buffer before
 * the next TC event reuses it. completed_buffer is NULL unless the result is 1.
 * After an error, stop peripheral requests and call Disable before restarting.
 */
int DMA_PingPongIntProc(DMA_PingPong_t *ctx, void **completed_buffer);

/** Return the configured byte count of each completed buffer. */
uint32_t DMA_PingPongGetBufferBytes(const DMA_PingPong_t *ctx);

/**
 * Stop peripheral DMA requests BEFORE calling. Stops only the owned channel.
 * On timeout retains ownership/buffers so Disable can safely be retried.
 * Malloc mode frees buffers only after the channel has stopped.
 */
int DMA_PingPongDisable(DMA_PingPong_t *ctx);

#endif

#ifdef __cplusplus
}
#endif

#endif

