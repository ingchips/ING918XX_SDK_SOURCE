
#define BLOCK_NUM           30
#define BLOCK_SIZE          240
#define NULL_INDEX          0xff

typedef struct mem_block
{
    uint8_t next;
    uint8_t len;
    uint8_t data[BLOCK_SIZE];
} mem_block_t;

static mem_block_t console_output[BLOCK_NUM] = {0};
static uint8_t first_free = 0;
static uint8_t first_ready = NULL_INDEX;
static uint8_t last_ready = NULL_INDEX;

extern void handle_output(const uint8_t *data, const int len);
extern void show_state(const io_state_t state);

void trigger_write(void);

static void reset_outputs(void)
{
    int i;
    first_free = 0;
    first_ready = NULL_INDEX;
    last_ready = NULL_INDEX;
    for (i = 0; i < BLOCK_NUM - 1; i++)
        console_output[i].next = i + 1;
    console_output[BLOCK_NUM - 1].next = NULL_INDEX;
}

static uint16_t get_free_space(void)
{
    uint8_t i = first_free;
    uint16_t r = 0;    
    while (i != NULL_INDEX)
    {
        r += BLOCK_SIZE;
        i = console_output[i].next;
    }
    return r;
}

int8_t send_data(const uint8_t *data, int len)
{
    uint8_t first;
    uint8_t last;
    uint8_t next;
    if ((INVALID_HANDLE == slave.conn_handle)
        || (len < 1)
        || (len > get_free_space()))
        return 1;

    __disable_irq();
    
    first = first_free;
    last = first;
    next = first;
    while (len)
    {
        int l = len > BLOCK_SIZE ? BLOCK_SIZE : len;
        last = next;
        memcpy(console_output[last].data, data, l);        
        console_output[last].len = l;
        
        next = console_output[last].next;

        data += l;
        len -= l;
    }
    
    console_output[last].next = NULL_INDEX;

    first_free = next;
    if (last_ready != NULL_INDEX)
        console_output[last_ready].next = first;
    else
        first_ready = first;
    last_ready = last;
    
    __enable_irq();
    
    trigger_write();
    return 0;
}

#ifdef IS_MASTER
void do_write(void)
{
    uint8_t first = first_ready;
    uint8_t last = first;
    uint8_t next = first;
    show_state(STATE_TX);

    while (next != NULL_INDEX)
    {
        last = next;
        uint8_t r = gatt_client_write_value_of_characteristic_without_response(slave.conn_handle, slave.input_char.value_handle, 
                                                                               console_output[next].len, 
                                                                               console_output[next].data);
        switch (r)
        {
        case GATT_CLIENT_BUSY:
            att_server_request_can_send_now_event(slave.conn_handle);
            return;
        case 0:
            next = console_output[next].next;
            break;
        default:
            dbg_printf("gatt_client_write err: %d\n", r);
            return;
        }
    }
}
#endif

