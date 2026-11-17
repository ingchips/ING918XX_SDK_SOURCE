#include "pingpong.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916 || INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

#include <string.h>
#if DMA_PINGPONG_USE_MALLOC
#include <stdlib.h>
#endif

#define DMA_PINGPONG_MAX_TRANSFER_COUNT 0x3fffffu
#define DMA_PINGPONG_STOP_POLL_LIMIT 100000u

int DMA_PingPongSetup(DMA_PingPong_t *ctx, SYSCTRL_DMA source,
                     uint32_t buffer_bytes, DMA_SrcBurstSize burst_size)
{
    if (ctx == NULL || buffer_bytes == 0 ||
        (unsigned int)burst_size > DMA_BURST_SIZE_128)
        return DMA_PINGPONG_ERROR_INVALID;
    if (ctx->state != DMA_PINGPONG_STOPPED)
        return DMA_PINGPONG_ERROR_BUSY;

    memset(ctx, 0, sizeof(*ctx));
    ctx->source = source;
    ctx->buffer_bytes = buffer_bytes;
    ctx->burst_size = burst_size;
    ctx->configured = 1;
    return 0;
}

#if !DMA_PINGPONG_USE_MALLOC
int DMA_PingPongSetBuffers(DMA_PingPong_t *ctx, void *ping_buffer,
                          void *pong_buffer, uint32_t capacity_bytes)
{
    uintptr_t ping = (uintptr_t)ping_buffer;
    uintptr_t pong = (uintptr_t)pong_buffer;

    if (ctx == NULL || !ctx->configured || ping_buffer == NULL ||
        pong_buffer == NULL || capacity_bytes < ctx->buffer_bytes)
        return DMA_PINGPONG_ERROR_INVALID;
    if (ctx->state != DMA_PINGPONG_STOPPED)
        return DMA_PINGPONG_ERROR_BUSY;
    if ((ping <= pong ? pong - ping : ping - pong) < ctx->buffer_bytes)
        return DMA_PINGPONG_ERROR_INVALID;

    ctx->buffers[0] = ping_buffer;
    ctx->buffers[1] = pong_buffer;
    ctx->buffer_capacity_bytes = capacity_bytes;
    return 0;
}
#endif

static int DMA_PingPongPrepare(DMA_PingPong_t *ctx)
{
    unsigned int i;
    for (i = 0; i < 2; ++i)
    {
        DMA_Descriptor *desc = &ctx->descriptors[i];
        if (DMA_PreparePeripheral2Mem(desc, (uint32_t *)ctx->buffers[i],
                                     ctx->source, ctx->buffer_bytes,
                                     DMA_ADDRESS_INC, 0) != 0)
            return DMA_PINGPONG_ERROR_INVALID;
        if (desc->TranSize == 0 || desc->TranSize > DMA_PINGPONG_MAX_TRANSFER_COUNT)
            return DMA_PINGPONG_ERROR_INVALID;

        /* Complete whole handshake bursts without a partial final request. */
        if (desc->TranSize % (1u << ctx->burst_size) != 0)
            return DMA_PINGPONG_ERROR_INVALID;
        DMA_ConfigSrcBurstSize(desc, ctx->burst_size);
        desc->Next = NULL;
    }
    return 0;
}

int DMA_PingPongEnable(DMA_PingPong_t *ctx, uint8_t channel)
{
    int result;
    uint32_t interrupt_mask;

    if (ctx == NULL || !ctx->configured || channel >= DMA_CHANNEL_NUMBER)
        return DMA_PINGPONG_ERROR_INVALID;
    if (ctx->state != DMA_PINGPONG_STOPPED || (APB_DMA->En & (1u << channel)))
        return DMA_PINGPONG_ERROR_BUSY;

#if DMA_PINGPONG_USE_MALLOC
    ctx->buffers[0] = malloc(ctx->buffer_bytes);
    ctx->buffers[1] = malloc(ctx->buffer_bytes);
    result = DMA_PINGPONG_ERROR_MEMORY;
    if (ctx->buffers[0] == NULL || ctx->buffers[1] == NULL)
        goto release_buffers;
#else
    if (ctx->buffers[0] == NULL || ctx->buffers[1] == NULL ||
        ctx->buffer_capacity_bytes < ctx->buffer_bytes)
        return DMA_PINGPONG_ERROR_INVALID;
#endif

    result = DMA_PingPongPrepare(ctx);
    if (result != 0)
    {
#if DMA_PINGPONG_USE_MALLOC
        goto release_buffers;
#else
        return result;
#endif
    }

    interrupt_mask = __get_PRIMASK();
    __disable_irq();
    ctx->channel = channel;
    ctx->active_buffer = 0;
    DMA_ClearChannelIntState(channel, DMA_IRQ_TC | DMA_IRQ_ABORT | DMA_IRQ_ERROR);
    ctx->state = DMA_PINGPONG_RUNNING;
    __DMB();
    DMA_EnableChannel(channel, &ctx->descriptors[0]);
    __set_PRIMASK(interrupt_mask);
    return 0;

#if DMA_PINGPONG_USE_MALLOC
release_buffers:
    free(ctx->buffers[0]);
    free(ctx->buffers[1]);
    ctx->buffers[0] = NULL;
    ctx->buffers[1] = NULL;
    return result;
#endif
}

int DMA_PingPongIntProc(DMA_PingPong_t *ctx, void **completed_buffer)
{
    uint32_t state;
    uint8_t completed;

    if (completed_buffer == NULL)
        return DMA_PINGPONG_ERROR_INVALID;
    *completed_buffer = NULL;
    if (ctx == NULL || ctx->state == DMA_PINGPONG_STOPPED)
        return 0;

    state = DMA_GetChannelIntState(ctx->channel);
    DMA_ClearChannelIntState(ctx->channel, state);
    if (state & (DMA_IRQ_ERROR | DMA_IRQ_ABORT))
    {
        ctx->state = DMA_PINGPONG_STOPPING;
        return DMA_PINGPONG_ERROR_TRANSFER;
    }
    if (ctx->state != DMA_PINGPONG_RUNNING || !(state & DMA_IRQ_TC))
        return 0;

    completed = ctx->active_buffer;
    ctx->active_buffer ^= 1u;
    __DMB();
    DMA_EnableChannel(ctx->channel, &ctx->descriptors[ctx->active_buffer]);
    __DMB();
    *completed_buffer = ctx->buffers[completed];
    return 1;
}

uint32_t DMA_PingPongGetBufferBytes(const DMA_PingPong_t *ctx)
{
    return ctx == NULL ? 0 : ctx->buffer_bytes;
}

int DMA_PingPongDisable(DMA_PingPong_t *ctx)
{
    uint32_t interrupt_mask;
    uint32_t remaining = DMA_PINGPONG_STOP_POLL_LIMIT;
    uint32_t channel_mask;

    if (ctx == NULL || !ctx->configured)
        return DMA_PINGPONG_ERROR_INVALID;
    if (ctx->state == DMA_PINGPONG_STOPPED)
        return 0;

    interrupt_mask = __get_PRIMASK();
    __disable_irq();
    ctx->state = DMA_PINGPONG_STOPPING;
    channel_mask = 1u << ctx->channel;
    if (APB_DMA->En & channel_mask)
        DMA_AbortChannel(ctx->channel);
    __set_PRIMASK(interrupt_mask);

    while (APB_DMA->En & channel_mask)
    {
        if (--remaining == 0)
            return DMA_PINGPONG_ERROR_TIMEOUT;
    }
    __DMB();
    /* Exclude a pending ISR while clearing state and releasing owned buffers. */
    interrupt_mask = __get_PRIMASK();
    __disable_irq();
    DMA_ClearChannelIntState(ctx->channel, DMA_IRQ_TC | DMA_IRQ_ABORT | DMA_IRQ_ERROR);
    ctx->state = DMA_PINGPONG_STOPPED;
    __set_PRIMASK(interrupt_mask);
#if DMA_PINGPONG_USE_MALLOC
    free(ctx->buffers[0]);
    free(ctx->buffers[1]);
    ctx->buffers[0] = NULL;
    ctx->buffers[1] = NULL;
#endif
    return 0;
}

#endif
