#include "port_gen_os_driver.h"
#include "platform_api.h"
#include "los_task.h"
#include "los_swtmr.h"
#include "los_queue.h"
#include "los_event.h"
#include "los_tick_pri.h"
#include "los_task_pri.h"
#include "los_swtmr_pri.h"
#include "los_sched_pri.h"
#include "los_memory_pri.h"
#include "los_sem_pri.h"
#include "los_mux_pri.h"
#include "los_queue_pri.h"
#include "los_hwi_pri.h"
#include "los_spinlock.h"
#include "los_mp_pri.h"
#include "ingsoc.h"

#ifndef LOS_MAX_NEST_DEPTH
#define LOS_MAX_NEST_DEPTH  10
#endif

struct timer_user_data
{
    void *user_data;
    void (* timer_cb)(void *);
};

static void *port_malloc(uint32_t size)
{
    return LOS_MemAlloc(m_aucSysMem1, (size + 0x3) & ~0x3);
}

static void port_free(void *mem)
{
    LOS_MemFree(m_aucSysMem1, mem);
}

gen_handle_t port_timer_create(
        uint32_t timeout_in_ms,
        void *user_data,
        void (* timer_cb)(void *)
    )
{
    UINT16 *id = (UINT16 *)port_malloc(sizeof(UINT16));
    LOS_SwtmrCreate(LOS_MS2Tick(timeout_in_ms), LOS_SWTMR_MODE_NO_SELFDELETE, (SWTMR_PROC_FUNC)timer_cb, id, (UINTPTR)user_data);
    return (gen_handle_t)id;
}

void port_timer_start(gen_handle_t timer)
{
    UINT16 *id = (UINT16 *)timer;
    LOS_SwtmrStart(*id);
}

void port_timer_stop(gen_handle_t timer)
{
    UINT16 *id = (UINT16 *)timer;
    LOS_SwtmrStop(*id);
}

void port_timer_delete(gen_handle_t timer)
{
    UINT16 *id = (UINT16 *)timer;
    LOS_SwtmrDelete(*id);
    port_free(id);
}

gen_handle_t port_task_create(
        const char *name,
        void (*entry)(void *),
        void *parameter,
        uint32_t stack_size,                    // stack size in bytes
        enum gen_os_task_priority priority
    )
{
    UINT32 taskId;
    TSK_INIT_PARAM_S initParam =
    {
        .pArgs = parameter,
        .pcName = (char *)name,
        .pfnTaskEntry = (TSK_ENTRY_FUNC)entry,
        .uwStackSize = (stack_size + 7) & ~0x7,
        .usTaskPrio = priority == GEN_TASK_PRIORITY_LOW ?
                                     OS_TASK_PRIORITY_HIGHEST + 5 : OS_TASK_PRIORITY_HIGHEST + 2,
    };
    LOS_TaskCreate(&taskId, &initParam);
    return (gen_handle_t)taskId;
}

struct queue_info
{
    UINT32 id;
    int msg_size;
};

gen_handle_t port_queue_create(int len, int item_size)
{
    struct queue_info *p = (struct queue_info *)port_malloc(sizeof(struct queue_info));
    p->msg_size = item_size;
    LOS_QueueCreate("q", len, &p->id, 0, item_size);
    return (gen_handle_t)p;
}

int port_queue_send_msg(gen_handle_t queue, void *msg)
{
    struct queue_info *p = (struct queue_info *)queue;
    int r = LOS_QueueWriteCopy(p->id, msg, p->msg_size, LOS_WAIT_FOREVER);
    return r == LOS_OK ? 0 : 1;
}

// return 0 if msg received; otherwise failed (timeout)
int port_queue_recv_msg(gen_handle_t queue, void *msg)
{
    struct queue_info *p = (struct queue_info *)queue;
    UINT32 bufferSize = p->msg_size;
    int r = LOS_QueueReadCopy(p->id, msg, &bufferSize, LOS_WAIT_FOREVER);
    return r == LOS_OK ? 0 : 1;
}

gen_handle_t port_event_create()
{
    EVENT_CB_S *event = (EVENT_CB_S *)port_malloc(sizeof(*event));
    LOS_EventInit(event);
    return (gen_handle_t)event;
}

// return 0 if msg received; otherwise failed (timeout)
int port_event_wait(gen_handle_t event)
{
    EVENT_CB_S *evt = (EVENT_CB_S *)event;
    uint32_t r = LOS_EventRead(evt, 1, LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
    if (r == 1)
    {
        return 0;
    }
    else
    {
        return r | 1;
    }
}

// event_set(event) will release the task in waiting.
void port_event_set(gen_handle_t event)
{
    LOS_EventWrite((EVENT_CB_S *)event, 1);
}

static UINT32 interrupt_states[LOS_MAX_NEST_DEPTH] = {0};
static int nest_level = 0;

void port_enter_critical(void)
{
    if (nest_level >= sizeof(interrupt_states) / sizeof(interrupt_states[0]))
        platform_raise_assertion(__FILE__, __LINE__);
    interrupt_states[nest_level] = LOS_IntLock();
    nest_level++;
}

void port_leave_critical(void)
{
    nest_level--;
    if (nest_level < 0)
        platform_raise_assertion(__FILE__, __LINE__);
    LOS_IntRestore(interrupt_states[nest_level]);
}

static void port_svc_handler(void)
{
}

void LOS_ISRPostProcessing(void)
{
    if (OsTaskProcSignal() != 0) {
        OsSchedPreempt();
    }
}

static void port_systick_handler(void)
{
    OsTickHandler();
    LOS_ISRPostProcessing();
}

VOID OsStart(VOID)
{
    LosTaskCB *taskCB = NULL;
    UINT32 cpuid = ArchCurrCpuid();

    OsTickStart();

    LOS_SpinLock(&g_taskSpin);
    taskCB = OsGetTopTask();

    OS_SCHEDULER_SET(cpuid);

    OsStartToRun(taskCB);
}

extern VOID osPendSV(VOID);

static const gen_os_driver_t gen_os_driver =
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
    .free = port_free,
    .enter_critical = port_enter_critical,
    .leave_critical = port_leave_critical,
    .os_start = OsStart,
    .tick_isr = port_systick_handler,
    .svc_isr = port_svc_handler,
    .pendsv_isr = osPendSV,
};

static VOID OsRegister(VOID)
{
    g_tickPerSecond         = LOSCFG_BASE_CORE_TICK_PER_SECOND;
    SET_SYS_CLOCK(OS_SYS_CLOCK);

    return;
}

static struct
{
    uint8_t enhanced_ticks;
} pm_info;

void gen_os_enable_enhanced_ticks(void)
{
    platform_config(PLATFORM_CFG_RTOS_ENH_TICK, 1);
    pm_info.enhanced_ticks = 1;
}

static uint32_t os_heap[OS_SYS_MEM_SIZE / sizeof(uint32_t)];
static void os_tickless(void);

const gen_os_driver_t *os_impl_get_driver(void)
{
    OsMemSystemInit((uintptr_t)os_heap);

    OsRegister();

    OsTickInit(GET_SYS_CLOCK(), LOSCFG_BASE_CORE_TICK_PER_SECOND);

    OsTaskInit();

    OsSemInit();
    OsMuxInit();
    OsQueueInit();

    OsSwtmrInit();

    OsIdleTaskCreate();

#ifdef LOSCFG_KERNEL_LOWPOWER
    LOS_LowpowerHookReg(os_tickless);
#endif

    return &gen_os_driver;
}

void platform_get_heap_status(platform_heap_status_t *status)
{
    static uint16_t bytes_minimum_ever_free = OS_SYS_MEM_SIZE;
    LOS_MEM_POOL_STATUS s;
    status->bytes_minimum_ever_free = bytes_minimum_ever_free;
    if (LOS_OK == LOS_MemInfoGet(m_aucSysMem1, &s))
    {
        status->bytes_free = s.uwTotalFreeSize;
        if (status->bytes_free < bytes_minimum_ever_free)
            bytes_minimum_ever_free = status->bytes_free;
        return;
    }
    else
        status->bytes_free = 0;
    status->bytes_minimum_ever_free = bytes_minimum_ever_free;
}

void LOS_BackTrace(void)
{
}

size_t IntActive()
{
    return IS_IN_INTERRUPT();
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

#define RTC_CYCLES_PER_TICK                 (RTC_CLK_FREQ / LOSCFG_BASE_CORE_TICK_PER_SECOND)
#define MAXIMUM_SUPPRESSED_TICKS            (0xffffff / RTC_CYCLES_PER_TICK)
#define EXPECTED_IDLE_TIME_BEFORE_SLEEP     2
#define MISSED_COUNTS_FACTOR                45
#define STOPPED_TIMER_COMPENSATION          (MISSED_COUNTS_FACTOR / ( PLL_CLK_FREQ / RTC_CLK_FREQ ))

VOID HalClockIrqClear(VOID)
{
}

VOID HalClockStart(VOID)
{
    HalClockInit();
    portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
}

VOID HalClockInit(VOID)
{
    uint32_t ticks = OS_SYS_CLOCK / LOSCFG_BASE_CORE_TICK_PER_SECOND;

    _SYSTICK_PRI = 0xFF;
    portNVIC_SYSTICK_LOAD_REG = ticks - 1;
    portNVIC_SYSTICK_CURRENT_VALUE_REG  = 0;
    portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_CLK_BIT;
}

LITE_OS_SEC_TEXT VOID OsSysTimeUpdate(UINT32 sleepTicks)
{
    UINT32 intSave;

    if (sleepTicks == 0) {
        return;
    }

    intSave = LOS_IntLock();
    g_tickCount[ArchCurrCpuid()] += (sleepTicks - 1);
    LOS_SpinLock(&g_taskSpin);
    OsSortLinkUpdateExpireTime(sleepTicks, &OsPercpuGet()->taskSortLink);
    LOS_SpinUnlock(&g_taskSpin);

#ifdef LOSCFG_BASE_CORE_SWTMR
    LOS_SpinLock(&g_swtmrSpin);
    OsSortLinkUpdateExpireTime(sleepTicks, &OsPercpuGet()->swtmrSortLink);
    LOS_SpinUnlock(&g_swtmrSpin);
#endif

    LOS_IntRestore(intSave);
}

LITE_OS_SEC_TEXT UINT32 OsSleepTicksGet(VOID)
{
    UINT32 tskSortLinkTicks, sleepTicks;

    UINT32 intSave = LOS_IntLock();
    LOS_SpinLock(&g_taskSpin);
    tskSortLinkTicks = OsSortLinkGetNextExpireTime(&OsPercpuGet()->taskSortLink);
    LOS_SpinUnlock(&g_taskSpin);

#ifdef LOSCFG_BASE_CORE_SWTMR
    UINT32 swtmrSortLinkTicks;
    LOS_SpinLock(&g_swtmrSpin);
    swtmrSortLinkTicks = OsSortLinkGetNextExpireTime(&OsPercpuGet()->swtmrSortLink);
    LOS_SpinUnlock(&g_swtmrSpin);
    sleepTicks = (tskSortLinkTicks < swtmrSortLinkTicks) ? tskSortLinkTicks : swtmrSortLinkTicks;
#else
    sleepTicks = tskSortLinkTicks;
#endif

    LOS_IntRestore(intSave);
    return sleepTicks;
}

// This is a re-implementation of FreeRTOS's suppress ticks and sleep function.
static void _suppress_ticks_and_sleep(uint32_t expected_ticks)
{
    uint32_t ulCompleteTickPeriods;

    portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;

    uint32_t ulReloadValue = portNVIC_SYSTICK_CURRENT_VALUE_REG + (RTC_CYCLES_PER_TICK * (expected_ticks - 1UL));
    if( ulReloadValue > STOPPED_TIMER_COMPENSATION )
    {
        ulReloadValue -= STOPPED_TIMER_COMPENSATION;
    }

    uint32_t intSave = LOS_IntLock();

    portNVIC_SYSTICK_LOAD_REG = ulReloadValue;
    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
    portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

    if (pm_info.enhanced_ticks)
        while (0 == portNVIC_SYSTICK_CURRENT_VALUE_REG) ;

    platform_pre_sleep_processing();
    platform_post_sleep_processing();

    LOS_IntRestore(intSave);
    intSave = LOS_IntLock();

    if (pm_info.enhanced_ticks)
        while (portNVIC_SYSTICK_CURRENT_VALUE_REG == portNVIC_SYSTICK_CURRENT_VALUE_REG);

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
        LOS_ISRPostProcessing();
    }

    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
    portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
    OsSysTimeUpdate(ulCompleteTickPeriods);
    if (pm_info.enhanced_ticks)
        while (0 == portNVIC_SYSTICK_CURRENT_VALUE_REG) ;
    portNVIC_SYSTICK_LOAD_REG = RTC_CYCLES_PER_TICK - 1UL;

    LOS_IntRestore(intSave);
}

static void os_tickless(void)
{
    UINT32 intSave;
    UINT32 sleepTicks;

    intSave = LOS_IntLock();
    sleepTicks = OsSleepTicksGet();

    if (sleepTicks > 1) {
        if (sleepTicks >= MAXIMUM_SUPPRESSED_TICKS) {
            sleepTicks = MAXIMUM_SUPPRESSED_TICKS;
        }
        sleepTicks = platform_pre_suppress_ticks_and_sleep_processing(sleepTicks);
        if (sleepTicks < EXPECTED_IDLE_TIME_BEFORE_SLEEP)
        {
            LOS_IntRestore(intSave);
            wfi();
            return;
        }
        LOS_IntRestore(intSave);

        _suppress_ticks_and_sleep(sleepTicks);

        platform_os_idle_resumed_hook();
    }
    else
    {
        LOS_IntRestore(intSave);
        wfi();
    }
    return;
}
