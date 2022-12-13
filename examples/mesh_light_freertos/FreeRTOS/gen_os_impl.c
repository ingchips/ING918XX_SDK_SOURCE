#include "port_gen_os_driver.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

struct timer_user_data
{
    void *user_data;
    void (* timer_cb)(void *);
};

static void os_timer_cb(TimerHandle_t xTimer)
{
    struct timer_user_data *data = (struct timer_user_data *)pvTimerGetTimerID(xTimer);
    data->timer_cb(data->user_data);
}

gen_handle_t port_timer_create(
        uint32_t timeout_in_ms,
        void *user_data,
        void (* timer_cb)(void *)
    )
{
    struct timer_user_data *data = (struct timer_user_data *)pvPortMalloc(sizeof(struct timer_user_data));
    data->user_data = user_data;
    data->timer_cb = timer_cb;
    return xTimerCreate("t",
                            pdMS_TO_TICKS(timeout_in_ms),
                            pdFALSE,
                            data,
                            os_timer_cb);
}

void port_timer_start(gen_handle_t timer)
{
    if (IS_IN_INTERRUPT())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTimerStartFromISR(timer, &xHigherPriorityTaskWoken);

    }
    else
        xTimerStart(timer, portMAX_DELAY);
}

void port_timer_stop(gen_handle_t timer)
{
    if (IS_IN_INTERRUPT())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTimerStopFromISR(timer, &xHigherPriorityTaskWoken);

    }
    else
        xTimerStop(timer, portMAX_DELAY);
}

void port_timer_delete(gen_handle_t timer)
{
    vPortFree(pvTimerGetTimerID(timer));
    xTimerDelete(timer, portMAX_DELAY);
}

#define APP_PRIO_LOW               2
#define APP_PRIO_HIGH             (configMAX_PRIORITIES - 1)

gen_handle_t port_task_create(
        const char *name,
        void (*entry)(void *),
        void *parameter,
        uint32_t stack_size,                    // stack size in bytes
        enum gen_os_task_priority priority
    )
{
    TaskHandle_t r;
    xTaskCreate(entry,
               name,
               (stack_size + 3) >> 2,
               parameter,
               priority == GEN_TASK_PRIORITY_HIGH ? APP_PRIO_HIGH : APP_PRIO_LOW,
               &r);
    return r;
}

gen_handle_t port_queue_create(int len, int item_size)
{
    return xQueueCreate(len, item_size);
}

int port_queue_send_msg(gen_handle_t queue, void *msg)
{
    int8_t err;
    if (IS_IN_INTERRUPT())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        err = xQueueSendToBackFromISR(queue, msg, &xHigherPriorityTaskWoken);

    }
    else
        err = xQueueSendToBack(queue, msg, portMAX_DELAY);
    return err == pdPASS ? 0 : 1;
}

// return 0 if msg received; otherwise failed (timeout)
int port_queue_recv_msg(gen_handle_t queue, void *msg)
{
    int8_t err;
    if (IS_IN_INTERRUPT())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        err = xQueueReceiveFromISR(queue, msg, &xHigherPriorityTaskWoken);

    }
    else
        err = xQueueReceive(queue, msg, portMAX_DELAY);
    return err == pdPASS ? 0 : 1;
}

gen_handle_t port_event_create()
{
    return xSemaphoreCreateBinary();
}

// return 0 if msg received; otherwise failed (timeout)
int port_event_wait(gen_handle_t event)
{
    return xSemaphoreTake(event, portMAX_DELAY) == pdPASS ? 0 : 1;
}

// event_set(event) will release the task in waiting.
void port_event_set(gen_handle_t event)
{
    if (IS_IN_INTERRUPT())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(event, &xHigherPriorityTaskWoken);
    }
    else
        xSemaphoreGive(event);
}

extern void xPortSysTickHandler(void);
extern void xPortPendSVHandler(void);
extern void vPortSVCHandler(void);

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

    .malloc = pvPortMalloc,
    .free = vPortFree,
    .enter_critical = vPortEnterCritical,
    .leave_critical = vPortExitCritical,
    .os_start = vTaskStartScheduler,
    .tick_isr = xPortSysTickHandler,
    .svc_isr = vPortSVCHandler,
    .pendsv_isr = xPortPendSVHandler,
};

const gen_os_driver_t *os_impl_get_driver(void)
{
    return &gen_os_driver;
}

#if configSUPPORT_STATIC_ALLOCATION
static StaticTask_t idle_task_TCB_buffer;
static StackType_t  idle_task_stack[configMINIMAL_STACK_SIZE];
static StaticTask_t timer_task_TCB_buffer;
static StackType_t  timer_task_stack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &idle_task_TCB_buffer;
    *ppxIdleTaskStackBuffer = idle_task_stack;
    *pulIdleTaskStackSize = sizeof(idle_task_stack) / sizeof(idle_task_stack[0]);
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &timer_task_TCB_buffer;
    *ppxTimerTaskStackBuffer = timer_task_stack;
    *pulTimerTaskStackSize = sizeof(timer_task_stack) / sizeof(timer_task_stack[0]);
}
#endif

void vApplicationMallocFailedHook(void)
{
    platform_raise_assertion(__FILE__, __LINE__);
}

void platform_get_heap_status(platform_heap_status_t *status)
{
    extern size_t xFreeBytesRemaining;
    extern size_t xMinimumEverFreeBytesRemaining;
    status->bytes_free = xFreeBytesRemaining;
    status->bytes_minimum_ever_free = xMinimumEverFreeBytesRemaining;
}

TickType_t sysPreSuppressTicksAndSleepProcessing(TickType_t expectedTicks)
{
    return platform_pre_suppress_ticks_and_sleep_processing(expectedTicks);
}

void sysPreSleepProcessing(TickType_t idleTime)
{
    platform_pre_sleep_processing();
}

void sysPostSleepProcessing(TickType_t idleTime)
{
    platform_post_sleep_processing();
}

void vApplicationIdleResumedHook(void)
{
    platform_os_idle_resumed_hook();
}
