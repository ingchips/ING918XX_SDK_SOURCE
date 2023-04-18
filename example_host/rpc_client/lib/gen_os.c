#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#include "port_gen_os_driver.h"
#include "platform_api.h"

#include "log.h"

#if (FreeRTOS == 0)

static pthread_mutex_t mutex;

static void enter_critical(void)
{
    pthread_mutex_lock(&mutex);
}

static void leave_critical(void)
{
    pthread_mutex_unlock(&mutex);
}

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

static HANDLE hTimerQueue = NULL;
struct timer_obj
{
    HANDLE hTimer;
    uint8_t paused;
    uint32_t timeout_in_ms;
    void (* timer_cb)(void *);
    void *user_data;
};

static VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    struct timer_obj *obj = (struct timer_obj *)lpParam;
    if (obj->paused) return;
    obj->timer_cb(obj->user_data);
}

gen_handle_t port_timer_create(
        uint32_t timeout_in_ms,
        void *user_data,
        void (* timer_cb)(void *)
    )
{
    if (hTimerQueue == NULL)
        hTimerQueue = CreateTimerQueue();
    if (hTimerQueue == NULL)
        LOG_E("failed to create timer queue");
    struct timer_obj *obj = (struct timer_obj *)malloc(sizeof(struct timer_obj));
    obj->timer_cb = timer_cb;
    obj->user_data = user_data;
    obj->timeout_in_ms = timeout_in_ms;
    obj->paused = 1;

    if (!CreateTimerQueueTimer(&obj->hTimer, hTimerQueue,
            (WAITORTIMERCALLBACK)TimerRoutine, obj, 0, 0, 0))
    {
        LOG_E("CreateTimerQueueTimer failed (%d)\n", GetLastError());
        return NULL;
    }

    return (gen_handle_t)obj;
}

void port_timer_start(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;
    obj->paused = 0;
    if (!ChangeTimerQueueTimer(hTimerQueue, obj->hTimer, obj->timeout_in_ms, 0))
        LOG_E("ChangeTimerQueueTimer failed (%d)\n", GetLastError());
}

void port_timer_stop(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;
    obj->paused = 1;
    if (!ChangeTimerQueueTimer(hTimerQueue, obj->hTimer, 0, 0))
        LOG_E("ChangeTimerQueueTimer failed (%d)\n", GetLastError());
}

void port_timer_delete(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;
    obj->paused = 1;
    DeleteTimerQueueTimer(hTimerQueue, obj->hTimer, INVALID_HANDLE_VALUE);
}

struct msg
{
    struct msg *next;
    uint8_t body[0];
};

struct msg_queue
{
    gen_handle_t event;
    int item_size;
    struct msg *first;
};

gen_handle_t port_queue_create(int len, int item_size)
{
    struct msg_queue *q = (struct msg_queue *)malloc(sizeof(struct msg_queue));
    memset(q, 0, sizeof(*q));
    q->event = port_event_create();
    q->item_size = item_size;
}

int port_queue_send_msg(gen_handle_t queue, void *msg)
{
    struct msg_queue *q = (struct msg_queue *)queue;
    struct msg *m = (struct msg *)malloc(sizeof(struct msg) + q->item_size);
    memcpy(m->body, msg, q->item_size);
    m->next = NULL;

    enter_critical();
    if (q->first)
    {
        struct msg *prev = q->first;
        while (prev->next) prev = prev->next;
        prev->next = m;
    }
    else
        q->first = m;
    leave_critical();

    port_event_set(q->event);
}

int port_queue_recv_msg(gen_handle_t queue, void *msg)
{
    struct msg_queue *q = (struct msg_queue *)queue;
    struct msg *m = NULL;

check_msg:
    enter_critical();
    m = q->first;
    if (m)
        q->first = m->next;
    leave_critical();

    if (m)
    {
        memcpy(msg, m->body, q->item_size);
        free(m);
        return 0;
    }

    int r = port_event_wait(q->event);
    if (r)
        return r;

    goto check_msg;
}

gen_handle_t port_event_create(void)
{
    return CreateEventA(NULL,
                        FALSE, //    bManualReset,
                        FALSE, //    bInitialState,
                        NULL);
}

int port_event_wait(gen_handle_t event)
{
    return WaitForSingleObject(event, INFINITE) == WAIT_OBJECT_0 ? 0 : 1;
}

void port_event_set(gen_handle_t event)
{
    SetEvent(event);
}

#endif

#ifdef __linux__

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <time.h>
    #include <signal.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <mqueue.h>
struct timer_obj
{
    timer_t id;
    uint32_t timeout_in_ms;
    void (* timer_cb)(void *);
    void *user_data;
};

static void expired(union sigval timer_data){
    struct timer_obj *data = (struct timer_obj *)timer_data.sival_ptr;
    data->timer_cb(data->user_data);
}

gen_handle_t port_timer_create(
        uint32_t timeout_in_ms,
        void *user_data,
        void (* timer_cb)(void *)
    )
{
    struct timer_obj *obj = (struct timer_obj *)malloc(sizeof(struct timer_obj));
    obj->timer_cb = timer_cb;
    obj->user_data = user_data;
    obj->timeout_in_ms = timeout_in_ms;

    /*  sigevent specifies behaviour on expiration  */
    struct sigevent sev = { 0 };

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = &expired;
    sev.sigev_value.sival_ptr = obj;

    int res = timer_create(CLOCK_REALTIME, &sev, &obj->id);
    if (res != 0){
        return NULL;
    }

    return (gen_handle_t)obj;
}

void port_timer_start(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;

    uint32_t ms = obj->timeout_in_ms % 1000;
    struct itimerspec its = {   .it_value.tv_sec  = obj->timeout_in_ms / 1000,
                                .it_value.tv_nsec = ms * 1000000,
                                .it_interval.tv_sec  = 0,
                                .it_interval.tv_nsec = 0
                            };

    /* start timer */
    int res = timer_settime(obj->id, 0, &its, NULL);
    if (res != 0){
        perror("timer_settime:");
    }
}

void port_timer_stop(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;
    struct itimerspec its = {   .it_value.tv_sec  = 0,
                                .it_value.tv_nsec = 0,
                                .it_interval.tv_sec  = 0,
                                .it_interval.tv_nsec = 0
                            };

    timer_settime(obj->id, 0, &its, NULL);
}

void port_timer_delete(gen_handle_t timer)
{
    struct timer_obj *obj = (struct timer_obj *)timer;
    timer_delete(obj->id);
    free(obj);
}

struct queue_obj
{
    mqd_t id_send;
    mqd_t id_read;
    int item_size;
};

#define QUEUE_PERMISSIONS (S_IRUSR | S_IWUSR)

gen_handle_t port_queue_create(int len, int item_size)
{

    struct queue_obj *obj = (struct queue_obj *)malloc(sizeof(struct queue_obj));
    obj->item_size = item_size;
    struct mq_attr attr =
    {
        .mq_flags = 0,
        .mq_maxmsg = len,
        .mq_msgsize = item_size,
        .mq_curmsgs = 0,
    };
    char name[100];
    sprintf(name, "/sq-port-%08x", rand());
    obj->id_read = mq_open(name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);
    if (obj->id_read == -1)
        perror("mq_open(client)");
    obj->id_send = mq_open(name, O_WRONLY);
    if (obj->id_send == -1)
        perror("mq_open(server)");
    return obj;
}

int port_queue_send_msg(gen_handle_t queue, void *msg)
{
    struct queue_obj *obj = (struct queue_obj *)queue;
    return mq_send(obj->id_send, msg, obj->item_size, 0);
}


int port_queue_recv_msg(gen_handle_t queue, void *msg)
{
    struct queue_obj *obj = (struct queue_obj *)queue;
    return mq_receive(obj->id_read, msg, obj->item_size, NULL) == obj->item_size ? 0 : -1;
}

struct event_obj
{
    int state;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

gen_handle_t port_event_create(void)
{
    struct event_obj *obj = (struct event_obj *)malloc(sizeof(struct event_obj));
    obj->state = 0;
    if (pthread_mutex_init(&obj->mutex, NULL))
    {
        perror("pthread_mutex_init");
        return NULL;
    }
    if (pthread_cond_init(&obj->cond, NULL))
    {
        perror("pthread_cond_init");
        return NULL;
    }
    return (gen_handle_t)obj;
}

int port_event_wait(gen_handle_t event)
{
    struct event_obj *obj = (struct event_obj *)event;
    if (pthread_mutex_lock(&obj->mutex))
        return -1;

    while (!obj->state)
    {
        if (pthread_cond_wait(&obj->cond, &obj->mutex))
        {
            pthread_mutex_unlock(&obj->mutex);
            return -1;
        }
    }

    obj->state = 0;

    if (pthread_mutex_unlock(&obj->mutex))
        return -1;

    return 0;
}

void port_event_set(gen_handle_t event)
{
    struct event_obj *obj = (struct event_obj *)event;
    pthread_mutex_lock(&obj->mutex);

    obj->state = 1;

    pthread_cond_broadcast(&obj->cond);

    pthread_mutex_unlock(&obj->mutex);
}

#endif

struct thread_arg
{
    pthread_t thid;
    void *ret;
    void (*entry)(void *);
    void *parameter;
};

static void *thread(void *arg) {
    struct thread_arg *p = (struct thread_arg *)arg;
    p->entry(p->parameter);
    free(p);
    pthread_exit(NULL);
}

gen_handle_t port_task_create(
        const char *name,
        void (*entry)(void *),
        void *parameter,
        uint32_t stack_size,                    // stack size in bytes
        enum gen_os_task_priority priority
    )
{
    struct thread_arg *arg = (struct thread_arg *)malloc(sizeof(struct thread_arg));
    arg->entry = entry;
    arg->parameter = parameter;

    if (pthread_create(&arg->thid, NULL, thread, arg) != 0) {
        perror("pthread_create() error");
    }

    return (gen_handle_t)arg->thid;
}

gen_handle_t port_get_current_tid(void)
{
    return (gen_handle_t)pthread_self();
}

void port_os_start(void)
{
}

const static struct gen_os_driver driver =
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

    .malloc = malloc,
    .free = free,
    .enter_critical = enter_critical,
    .leave_critical = leave_critical,

    .os_start = port_os_start,
};

const void *platform_get_gen_os_driver(void)
{
    return &driver;
}

void gen_os_init(void)
{
    pthread_mutex_init(&mutex, NULL);
}

#else

#include "pthread.h"
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 4096
#endif
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

extern void sleep_ms(uint32_t ms);

/* When configSUPPORT_STATIC_ALLOCATION is set to 1 the application writer can
 * use a callback function to optionally provide the memory required by the idle
 * and timer tasks.  This is the stack that will be used by the timer task.  It is
 * declared here, as a global, so it can be checked by a test that is implemented
 * in a different file. */
StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

void vAssertCalled( const char * const pcFileName,
                    unsigned long ulLine )
{
    platform_raise_assertion(pcFileName, ulLine);
}

void vApplicationMallocFailedHook( void )
{
    vAssertCalled( __FILE__, __LINE__ );
}

void vApplicationIdleHook( void )
{
    sleep_ms(15);
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask,
                                    char * pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    vAssertCalled( __FILE__, __LINE__ );
}

void vApplicationTickHook( void )
{
}

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
     * state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 * application must provide an implementation of vApplicationGetTimerTaskMemory()
 * to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationDaemonTaskStartupHook( void )
{
}

void *pvPortMalloc( size_t xWantedSize )
{
    return malloc(xWantedSize);
}

void vPortFree( void *pv )
{
    free(pv);
}

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
    int s = (stack_size + 3) >> 2;
    if (s < configMINIMAL_STACK_SIZE) s = configMINIMAL_STACK_SIZE;

    xTaskCreate(entry,
               name,
               s,
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
};

gen_handle_t port_get_current_tid(void)
{
    return (gen_handle_t)xTaskGetCurrentTaskHandle();
}

const void *platform_get_gen_os_driver(void)
{
    return &gen_os_driver;
}

void gen_os_init(void)
{
}

#endif
