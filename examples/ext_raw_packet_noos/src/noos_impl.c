#include <stdlib.h>
#include "ingsoc.h"
#include "port_gen_os_driver.h"

static int int_lock_cnt = 0;

static void enter_critical(void)
{
    __disable_irq();
    int_lock_cnt++;
}

static void dummy(gen_handle_t event)
{
}

static void leave_critical(void)
{
    int_lock_cnt--;
    if (int_lock_cnt <= 0)
        __enable_irq();
}

extern void noos_start(void);

__attribute ((weak)) void tick_isr(void)
{
}

__attribute ((weak)) void svc_isr(void)
{
}

__attribute ((weak)) void pendsv_isr(void)
{
}

static const gen_os_driver_t driver = 
{
    .timer_create = NULL,
    .timer_delete = NULL,
    .timer_start = NULL,
    .timer_stop = NULL,
    
    .task_create = NULL,
    .event_create = NULL,
    .event_set = dummy,
    .event_wait = NULL,
    
    .queue_create = NULL,
    .queue_recv_msg = NULL,
    .queue_send_msg = NULL,
    
    .free = NULL,
    .malloc = NULL,
    
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
