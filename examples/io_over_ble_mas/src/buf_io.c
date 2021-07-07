#include "ring_buf.h"

#define BLOCK_SIZE          (247 - 3)

#define RX_BUFFER_SIZE      (BLOCK_SIZE * 15 + RING_BUF_OBJ_SIZE)
static uint8_t ring_buff_storage[RX_BUFFER_SIZE];
static struct ring_buf *ring_buffer;

extern void show_state(const io_state_t state);

void trigger_write(int flush);
extern int is_triggering;

void init_buffer(void)
{
    ring_buffer = ring_buf_init(ring_buff_storage, sizeof(ring_buff_storage));
}

int8_t send_data(const uint8_t *data, int len, int flush)
{
#ifdef IS_MASTER
    if ((INVALID_HANDLE == slave.conn_handle)
        || (len < 1))
        return 1;
#else
    if ((0 == notify_enable)
        || (len < 1))
        return 1;
#endif
    
    int written = ring_buf_write_data(ring_buffer, data, len);
    if (written < len)
        dbg_printf("data lost: %d!!!\n", len - written);
    
    trigger_write(flush);
    return 0;
}

void do_write(int flush)
{
#ifdef IS_MASTER
    if (slave.conn_handle == INVALID_HANDLE)
        return;
#else
    if (0 == notify_enable)
        return;
#endif

    show_state(STATE_TX);
    ring_buf_peek_data(ring_buffer, cb_ring_buf_peek_data, (void *)flush);
}

