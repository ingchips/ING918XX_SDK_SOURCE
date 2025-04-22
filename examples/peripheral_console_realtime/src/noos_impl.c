#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ingsoc.h"
#include "port_gen_os_driver.h"
#include "platform_api.h"

#include "profile.h"

#ifndef ADDITIONAL_ATTRIBUTE
#define ADDITIONAL_ATTRIBUTE
#endif

typedef void (*f_task_entry)(void *);
typedef void (*f_timer_cb)(void *);
typedef void (*f_plt_timer)(void);

static f_task_entry host_entry = NULL;
static void *host_param = NULL;
static int handle_cnt = 0;
static int int_lock_cnt = 0;

ADDITIONAL_ATTRIBUTE static void enter_critical(void)
{
    __disable_irq();
    int_lock_cnt++;
}

ADDITIONAL_ATTRIBUTE static void leave_critical(void)
{
    int_lock_cnt--;
    if (int_lock_cnt <= 0)
        __enable_irq();
}

struct simple_queue
{
    int len;
    int item_size;
    int msg_cnt;
    uint8_t data[0];
};

#define SW_TIMER_NUMBER     8

#define SW_TIMER_CB_OF(id) timer_cb_##id

static void timer_cb(int id);

#define DEF_SW_TIMER(id)    static void SW_TIMER_CB_OF(id)(void) { timer_cb(id); }

DEF_SW_TIMER(0)
DEF_SW_TIMER(1)
DEF_SW_TIMER(2)
DEF_SW_TIMER(3)
DEF_SW_TIMER(4)
DEF_SW_TIMER(5)
DEF_SW_TIMER(6)
DEF_SW_TIMER(7)

struct sw_timer
{
    f_plt_timer plt_cb;
    f_timer_cb cb;
    void *user_data;
    uint32_t timeout_in_625us;
    uint8_t used;
} sw_timers[SW_TIMER_NUMBER] = {
    { .plt_cb = SW_TIMER_CB_OF(0) },
    { .plt_cb = SW_TIMER_CB_OF(1) },
    { .plt_cb = SW_TIMER_CB_OF(2) },
    { .plt_cb = SW_TIMER_CB_OF(3) },
    { .plt_cb = SW_TIMER_CB_OF(4) },
    { .plt_cb = SW_TIMER_CB_OF(5) },
    { .plt_cb = SW_TIMER_CB_OF(6) },
    { .plt_cb = SW_TIMER_CB_OF(7) },
};

static void timer_cb(int id)
{
    if ((id < 0) || (id >= SW_TIMER_NUMBER) || (sw_timers[id].used == 0)) return;
    sw_timers[id].cb(sw_timers[id].user_data);
}

static gen_handle_t timer_create(
        uint32_t timeout_in_ms,
        void *user_data,
        void (* timer_cb)(void *))
{
    // find an unused timer context
    int id;
    for (id = 0; id < SW_TIMER_NUMBER; id++)
        if (sw_timers[id].used == 0) break;
    if (id >= SW_TIMER_NUMBER)
    {
        platform_raise_assertion("noos_impl.c", __LINE__);
        return (gen_handle_t)0;
    }

    sw_timers[id].cb = timer_cb;
    sw_timers[id].user_data = user_data;
    sw_timers[id].timeout_in_625us = timeout_in_ms + timeout_in_ms / 2 + timeout_in_ms / 10;
    sw_timers[id].used = 1;

    // avoid returning 0
    return (gen_handle_t)(&sw_timers[id]);
}

static void timer_start(gen_handle_t timer)
{
    struct sw_timer *p = (struct sw_timer *)timer;
    platform_set_timer(p->plt_cb, p->timeout_in_625us);
}

static void timer_stop(gen_handle_t timer)
{
    struct sw_timer *p = (struct sw_timer *)timer;
    platform_set_timer(p->plt_cb, 0);
}

static void timer_delete(gen_handle_t timer)
{
    struct sw_timer *p = (struct sw_timer *)timer;
    platform_set_timer(p->plt_cb, 0);
    p->used = 0;
}

static gen_handle_t task_create(
        const char *name,
        void (*entry)(void *),
        void *parameter,
        uint32_t stack_size,
        enum gen_os_task_priority priority
    )
{
    if (priority == GEN_TASK_PRIORITY_LOW)
    {
        host_entry = entry;
        host_param = parameter;
    }
    return (gen_handle_t)(++handle_cnt);
}

static gen_handle_t queue_create(int len, int item_size)
{
    struct simple_queue *q = (struct simple_queue *)malloc(len * item_size + sizeof(struct simple_queue));
    q->msg_cnt = 0;
    q->len = len;
    q->item_size = item_size;
    return (gen_handle_t)q;
}

static int queue_send_msg(gen_handle_t queue, void *msg)
{
    struct simple_queue *q = (struct simple_queue *)queue;
    if (q->msg_cnt >= q->len) return -1;
    memcpy(q->data + q->msg_cnt * q->item_size, msg, q->item_size);
    q->msg_cnt++;
    return 0;
}

/* Constants required to manipulate the core.  Registers first... */
#define portNVIC_SYSTICK_CTRL_REG			( * ( ( volatile uint32_t * ) 0xe000e010 ) )
/* ...then bits in the registers. */
#define portNVIC_SYSTICK_CLK_BIT	        ( 0UL << 2UL )
#define portNVIC_SYSTICK_ENABLE_BIT			( 1UL << 0UL )

ADDITIONAL_ATTRIBUTE static void idle_process(void)
{
#ifdef POWER_SAVING
    uint32_t cycles = platform_pre_suppress_cycles_and_sleep_processing((uint32_t)-1);
    if (cycles < 100) return;
    enter_critical();
    platform_pre_sleep_processing();
    platform_post_sleep_processing();
    leave_critical();
    platform_os_idle_resumed_hook();
#endif
}

// return 0 if msg received; otherwise failed (timeout)
ADDITIONAL_ATTRIBUTE static int queue_recv_msg(gen_handle_t queue, void *msg)
{
    platform_controller_run();

    struct simple_queue *q = (struct simple_queue *)queue;
    if (q->msg_cnt <= 0)
    {
        idle_process();
        return -1;
    }

    memcpy(msg, q->data, q->item_size);
    q->msg_cnt--;
    memmove(q->data, q->data + q->item_size, q->item_size * q->msg_cnt);
    return 0;
}

static gen_handle_t dummy_event_create() { return (gen_handle_t)(++handle_cnt); }
static int dummy_event_wait(gen_handle_t event) { return 0; }
ADDITIONAL_ATTRIBUTE static void dummy_event_set(gen_handle_t event) { }

void noos_start(void)
{
    // let the SysTick run, but do NOT generate interrupt requests
    portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_ENABLE_BIT | portNVIC_SYSTICK_CLK_BIT;
    __enable_irq();
    platform_init_controller();
    host_entry(host_param);
}

ADDITIONAL_ATTRIBUTE __attribute ((weak)) void tick_isr(void)
{
}

ADDITIONAL_ATTRIBUTE __attribute ((weak)) void svc_isr(void)
{
}

ADDITIONAL_ATTRIBUTE __attribute ((weak)) void pendsv_isr(void)
{
}

static const gen_os_driver_t driver =
{
    .timer_create = timer_create,
    .timer_delete = timer_delete,
    .timer_start = timer_start,
    .timer_stop = timer_stop,

    .task_create = task_create,
    .event_create = dummy_event_create,
    .event_set = dummy_event_set,
    .event_wait = dummy_event_wait,

    .queue_create = queue_create,
    .queue_recv_msg = queue_recv_msg,
    .queue_send_msg = queue_send_msg,

    .free = free,
    .malloc = malloc,

    .enter_critical = enter_critical,
    .leave_critical = leave_critical,

    .os_start = noos_start,
    .tick_isr = tick_isr,
    .svc_isr = svc_isr,
    .pendsv_isr = pendsv_isr,
};

const gen_os_driver_t *os_impl_get_driver(void)
{
    return &driver;
}

void platform_get_heap_status(platform_heap_status_t *status)
{
    // heap is using malloc/free
    // how to get unused size?
    status->bytes_free = 0;
    status->bytes_minimum_ever_free = 0;
}
