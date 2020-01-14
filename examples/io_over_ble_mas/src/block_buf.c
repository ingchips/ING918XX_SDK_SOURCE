
#define BLOCK_NUM           25
#define BLOCK_SIZE          240

typedef struct mem_block
{
    uint8_t len;
    uint8_t data[BLOCK_SIZE];
} mem_block_t;

#define NEXT_INDEX(i)   ((i) >= BLOCK_NUM - 1 ? 0 : (i) + 1)

static mem_block_t console_output[BLOCK_NUM] = {0};
static uint8_t write_index = 0;
static uint8_t read_index = 0;

extern void handle_output(const uint8_t *data, const int len);
extern void show_state(const io_state_t state);

void trigger_write(void);

// static void reset_outputs(void)
// {
//     uint8_t i;
//     write_index = 0;
//     read_index = 0;
//     for (i = 0; i < BLOCK_NUM; i++)
//         console_output[i].len = 0;
// }

uint16_t get_free_space(void)
{
    uint8_t i = write_index;
    uint16_t r = 0;    
    while (0 == console_output[i].len)
    {
        r += BLOCK_SIZE;
        i = NEXT_INDEX(i);
        if (i == write_index)
            break;
    }
    return r;
}

int8_t send_data(const uint8_t *data, int len)
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
    
    if (len > get_free_space())
    {
        trigger_write();
        return 1;
    }

    while (len)
    {
        int l = len > BLOCK_SIZE ? BLOCK_SIZE : len;
        memcpy(console_output[write_index].data, data, l);

        console_output[write_index].len = l;
        write_index = NEXT_INDEX(write_index);

        data += l;
        len -= l;
    }
    
    trigger_write();
    return 0;
}

void do_write(void)
#ifdef IS_MASTER
{
    show_state(STATE_TX);

    while (console_output[read_index].len)
    {
        uint8_t r = gatt_client_write_value_of_characteristic_without_response(slave.conn_handle, slave.input_char.value_handle, 
                                                                               console_output[read_index].len, 
                                                                               console_output[read_index].data);
        switch (r)
        {
        case GATT_CLIENT_BUSY:
            att_server_request_can_send_now_event(slave.conn_handle);
            return;
        case 0:
            console_output[read_index].len = 0;
            read_index = NEXT_INDEX(read_index);
            break;
        default:
            dbg_printf("gatt_client_write err: %d\n", r);
            return;
        }
    }
}
#else
{
    if (0 == notify_enable)
        return;
    show_state(STATE_TX);
    while (console_output[read_index].len)
    {
        if (att_server_can_send_packet_now(handle_send))
        {
            if (0 == att_server_notify(handle_send, HANDLE_GENERIC_OUTPUT, console_output[read_index].data,
                                                                           console_output[read_index].len))
            {
                console_output[read_index].len = 0;
                read_index = read_index <= BLOCK_NUM - 2 ? read_index + 1 : 0;
                continue;
            }
            else
                return;
        }
        else;
        
        att_server_request_can_send_now_event(handle_send);
        return;
    }
}
#endif

