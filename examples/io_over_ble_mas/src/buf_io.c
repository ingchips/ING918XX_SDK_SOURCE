#include "ingsoc.h"
#include "ring_buf.h"

#define BLOCK_SIZE          (247 - 3)

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    #define RX_BUFFER_SIZE      (BLOCK_SIZE * 65 + RING_BUF_OBJ_SIZE)
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    #define RX_BUFFER_SIZE      (BLOCK_SIZE * 15 + RING_BUF_OBJ_SIZE)
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)
    #define RX_BUFFER_SIZE      (BLOCK_SIZE * 10 + RING_BUF_OBJ_SIZE)
#else
    #error unknown or unsupported chip family
#endif

static uint8_t ring_buff_storage[RX_BUFFER_SIZE];
static struct ring_buf *ring_buffer;

extern void show_state(const io_state_t state);

void trigger_write(int flush);
extern int is_triggering;

static void ring_buf_highwater_cb(struct ring_buf *buf)
{
    trigger_write(0);
}

void init_buffer(void)
{
    ring_buffer = ring_buf_init(ring_buff_storage, sizeof(ring_buff_storage), ring_buf_highwater_cb);
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

    return 0;
}

extern void uart_driver_read_data(void);

void do_write(int flush)
{
#ifdef IS_MASTER
    if (slave.conn_handle == INVALID_HANDLE)
        return;
#else
    if (0 == notify_enable)
        return;
#endif

    if (flush) uart_driver_read_data();
    ring_buf_peek_data(ring_buffer, cb_ring_buf_peek_data, (void *)flush);
    //show_state(STATE_TX);
}

