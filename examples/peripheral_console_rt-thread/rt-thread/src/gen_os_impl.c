#include "port_gen_os_driver.h"
#include "rtthread.h"
#include "rthw.h"
#include <string.h>
#include "platform_api.h"
#include "ingsoc.h"

#ifndef RT_THREAD_HEAP_SIZE
#define RT_THREAD_HEAP_SIZE             (20 * 1024)
#endif

#ifndef CS_MAX_NEST_DEPTH
#define CS_MAX_NEST_DEPTH               20
#endif

gen_handle_t port_timer_create(uint32_t timeout_in_ms, void *user_data, void (* timer_cb)(void *)
    )
{
    return rt_timer_create(NULL, timer_cb, user_data,
                           rt_tick_from_millisecond(timeout_in_ms),
                           RT_TIMER_FLAG_DEACTIVATED | RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_ONE_SHOT);
}

void port_timer_start(gen_handle_t timer)
{
    rt_timer_start((rt_timer_t)timer);
}

void port_timer_stop(gen_handle_t timer)
{
    rt_timer_stop((rt_timer_t)timer);
}

void port_timer_delete(gen_handle_t timer)
{
    rt_timer_delete((rt_timer_t)timer);
}

#define APP_PRIO_LOW                (RT_THREAD_PRIORITY_MAX - 3)
#define APP_PRIO_HIGH               (1)

gen_handle_t port_task_create(
        const char *name,
        void (*entry)(void *),
        void *parameter,
        uint32_t stack_size,                    // stack size in bytes
        enum gen_os_task_priority priority)
{
    rt_thread_t tid = rt_thread_create(name, entry, parameter,
               stack_size,
               priority == GEN_TASK_PRIORITY_HIGH ? APP_PRIO_HIGH : APP_PRIO_LOW,
               50);
    rt_thread_startup(tid);
    return tid;
}

struct queue_info
{
    rt_mq_t handle;
    int msg_size;
};

gen_handle_t port_queue_create(int len, int item_size)
{
    struct queue_info *p = rt_malloc(sizeof(struct queue_info));
    p->handle = rt_mq_create(NULL, item_size, len, RT_IPC_FLAG_FIFO);
    p->msg_size = item_size;
    return p;
}

int port_queue_send_msg(gen_handle_t queue, void *msg)
{
    struct queue_info *p = (struct queue_info *)queue;
    rt_err_t err = rt_mq_send(p->handle, msg, p->msg_size);
    return err == RT_EOK ? 0 : 1;
}

// return 0 if msg received; otherwise failed (timeout)
int port_queue_recv_msg(gen_handle_t queue, void *msg)
{
    struct queue_info *p = (struct queue_info *)queue;
    rt_err_t err = rt_mq_recv(p->handle, msg, p->msg_size, RT_WAITING_FOREVER);
    return err == RT_EOK ? 0 : 1;
}    

gen_handle_t port_event_create()
{
    return rt_event_create(NULL, RT_IPC_FLAG_FIFO);
}    

// return 0 if msg received; otherwise failed (timeout)
int port_event_wait(gen_handle_t event)
{
    rt_err_t err = rt_event_recv((rt_event_t)event, 1, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
    return err == RT_EOK ? 0 : 1;
}    

// event_set(event) will release the task in waiting.
void port_event_set(gen_handle_t event)
{
    rt_event_send((rt_event_t)event, 1);
}

static rt_base_t interrupt_states[CS_MAX_NEST_DEPTH] = {0};
static int nest_level = 0;

void port_enter_critical(void)
{
    if (nest_level >= sizeof(interrupt_states) / sizeof(interrupt_states[0]))
        platform_raise_assertion(__FILE__, __LINE__);
    interrupt_states[nest_level] = rt_hw_interrupt_disable();
    nest_level++;
}

void port_leave_critical(void)
{
    nest_level--;
    if (nest_level < 0)
        platform_raise_assertion(__FILE__, __LINE__);
    rt_hw_interrupt_enable(interrupt_states[nest_level]);
}

void *port_malloc(uint32_t size)
{
    return rt_malloc(size);
}

extern void PendSV_Handler(void);
extern void rt_application_init(void);

static void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
    rt_tick_increase();
    /* leave interrupt */
    rt_interrupt_leave();
}

#define _SYSTICK_PRI    (*(uint8_t  *)(0xE000ED23UL))

/* Constants required to manipulate the core.  Registers first... */
#define portNVIC_SYSTICK_CTRL_REG			( * ( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG			( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG	( * ( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_SYSPRI2_REG				( * ( ( volatile uint32_t * ) 0xe000ed20 ) )
/* ...then bits in the registers. */
#define portNVIC_SYSTICK_CLK_BIT	        ( 0UL << 2UL )
#define portNVIC_SYSTICK_INT_BIT			( 1UL << 1UL )
#define portNVIC_SYSTICK_ENABLE_BIT			( 1UL << 0UL )
#define portNVIC_SYSTICK_COUNT_FLAG_BIT		( 1UL << 16UL )
#define portNVIC_PENDSVCLEAR_BIT 			( 1UL << 27UL )
#define portNVIC_PEND_SYSTICK_CLEAR_BIT		( 1UL << 25UL )
#define portSY_FULL_READ_WRITE              15

uint32_t _SysTick_Config(rt_uint32_t ticks)
{
    if ((ticks - 1) > 0xFFFFFF)
    {
        return 1;
    }

    _SYSTICK_PRI = 0xFF;
    portNVIC_SYSTICK_LOAD_REG = ticks - 1;    
    portNVIC_SYSTICK_CURRENT_VALUE_REG  = 0;
    portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT | portNVIC_SYSTICK_CLK_BIT;

    return 0;
}

static void SVC_Handler(void)
{
    platform_raise_assertion(__FILE__, __LINE__);
}

#define RTC_CYCLES_PER_TICK                 (RTC_CLK_FREQ / RT_TICK_PER_SECOND)
#define MAXIMUM_SUPPRESSED_TICKS            (0xffffff / RTC_CYCLES_PER_TICK)
#define EXPECTED_IDLE_TIME_BEFORE_SLEEP     2
#define MISSED_COUNTS_FACTOR                45
#define STOPPED_TIMER_COMPENSATION          (MISSED_COUNTS_FACTOR / ( SYSCTRL_GetHClk() / RTC_CLK_FREQ ))

#ifdef POWER_SAVING

// This is a re-implementation of FreeRTOS's suppress ticks and sleep function.
static uint32_t _rt_suppress_ticks_and_sleep(uint32_t expected_ticks)
{
    uint32_t ulCompleteTickPeriods;
    
    portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;

    uint32_t ulReloadValue = portNVIC_SYSTICK_CURRENT_VALUE_REG + (RTC_CYCLES_PER_TICK * (expected_ticks - 1UL));
    if( ulReloadValue > STOPPED_TIMER_COMPENSATION )
    {
        ulReloadValue -= STOPPED_TIMER_COMPENSATION;
    }

    rt_base_t lock = rt_hw_interrupt_disable();

    portNVIC_SYSTICK_LOAD_REG = ulReloadValue;
    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
    portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
    
    platform_pre_sleep_processing();
    platform_post_sleep_processing();

    rt_hw_interrupt_enable(lock);

    lock = rt_hw_interrupt_disable();

    portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT );

    if( ( portNVIC_SYSTICK_CTRL_REG & portNVIC_SYSTICK_COUNT_FLAG_BIT ) != 0 )
    {
        uint32_t ulCalculatedLoadValue;

        ulCalculatedLoadValue = (RTC_CYCLES_PER_TICK - 1UL) - (ulReloadValue - portNVIC_SYSTICK_CURRENT_VALUE_REG);

        if ((ulCalculatedLoadValue < STOPPED_TIMER_COMPENSATION ) || ( ulCalculatedLoadValue > RTC_CYCLES_PER_TICK))
        {
            ulCalculatedLoadValue = (RTC_CYCLES_PER_TICK - 1UL);
        }

        portNVIC_SYSTICK_LOAD_REG = ulCalculatedLoadValue;

        ulCompleteTickPeriods = expected_ticks - 1UL;
    }
    else
    {
        uint32_t ulCompletedSysTickDecrements = (expected_ticks * RTC_CYCLES_PER_TICK) - portNVIC_SYSTICK_CURRENT_VALUE_REG;

        ulCompleteTickPeriods = ulCompletedSysTickDecrements / RTC_CYCLES_PER_TICK;

        portNVIC_SYSTICK_LOAD_REG = ((ulCompleteTickPeriods + 1UL) * RTC_CYCLES_PER_TICK) - ulCompletedSysTickDecrements;
    }

    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
    portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
    rt_tick_set(rt_tick_get() + ulCompleteTickPeriods);

    portNVIC_SYSTICK_LOAD_REG = RTC_CYCLES_PER_TICK - 1UL;

    rt_hw_interrupt_enable(lock);
    return ulCompleteTickPeriods;
}

#endif

void rt_system_power_manager(void)
{
#ifdef POWER_SAVING
    rt_tick_t timeout_tick;
   
    timeout_tick = rt_timer_next_timeout_tick();
    if (timeout_tick != RT_TICK_MAX)
        timeout_tick = timeout_tick - rt_tick_get();
    if (timeout_tick < EXPECTED_IDLE_TIME_BEFORE_SLEEP)
        return;

    if (timeout_tick > MAXIMUM_SUPPRESSED_TICKS)
        timeout_tick = MAXIMUM_SUPPRESSED_TICKS;

    timeout_tick = platform_pre_suppress_ticks_and_sleep_processing(timeout_tick);
    if (timeout_tick < EXPECTED_IDLE_TIME_BEFORE_SLEEP)
        return;
    
    rt_enter_critical();
    
    uint32_t delta_ticks = _rt_suppress_ticks_and_sleep(timeout_tick);

    rt_exit_critical();
    
    platform_os_idle_resumed_hook();
    
    if (delta_ticks)
        rt_timer_check();
#endif
}

static void rtthread_startup(void)
{
    /* timer thread initialization */
    rt_system_timer_thread_init();

    /* idle thread initialization */
    rt_thread_idle_init();

    _SysTick_Config(RTC_CLK_FREQ / RT_TICK_PER_SECOND);

    /* start scheduler */
    rt_system_scheduler_start();

    /* never reach here */
}

const gen_os_driver_t gen_os_driver =
{
    .timer_create = port_timer_create,
    .timer_start = port_timer_start,
    .timer_stop = port_timer_stop,
    .timer_delete = port_timer_delete,
    
    .task_create = port_task_create,
    
    .queue_create = port_queue_create,
    .queue_send_msg = port_queue_send_msg,
    .queue_recv_msg = port_queue_recv_msg,
    
    .event_create = port_event_create,
    .event_set = port_event_set,
    .event_wait = port_event_wait,
    
    .malloc = port_malloc,
    .free = rt_free,
    .enter_critical = port_enter_critical,
    .leave_critical = port_leave_critical,
    .os_start = rtthread_startup,
    .tick_isr = SysTick_Handler,
    .svc_isr = SVC_Handler,
    .pendsv_isr = PendSV_Handler,
};

const gen_os_driver_t *os_impl_get_driver(void)
{
    static uint8_t heap[RT_THREAD_HEAP_SIZE] = {0};

    rt_hw_interrupt_disable();
    
    rt_system_heap_init(heap, heap + sizeof(heap));

    /* timer system initialization */
    rt_system_timer_init();

    /* scheduler system initialization */
    rt_system_scheduler_init();

    return &gen_os_driver;
}

void platform_get_heap_status(platform_heap_status_t *status)
{
    rt_uint32_t used, max_used;
    rt_memory_info(NULL, &used, &max_used);
    status->bytes_free = RT_THREAD_HEAP_SIZE - used;
    status->bytes_minimum_ever_free = RT_THREAD_HEAP_SIZE - max_used;
}
