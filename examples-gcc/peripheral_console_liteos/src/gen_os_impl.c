#include "port_gen_os_driver.h"
#include <stdio.h>
#include "platform_api.h"
#include "los_task.h"
#include "los_swtmr.h"
#include "los_queue.h"
#include "los_event.h"
#include "los_memory.h"
#include "los_mux.h"
#include "los_sem.h"
#include "ingsoc.h"
#include "los_config.h"
#include "los_tick.h"
#include "los_sched.h"
#include "los_pm.h"

#ifndef LOS_MAX_NEST_DEPTH
#define LOS_MAX_NEST_DEPTH 10
#endif

struct timer_user_data
{
    void *user_data;
    void (*timer_cb)(void *);
};

static void *port_malloc(uint32_t size)
{
    void *ret_ptr;
    ret_ptr = LOS_MemAlloc(m_aucSysMem0, (size + 0x3) & ~0x3);
    return ret_ptr;
}

static void port_free(void *mem)
{
    uint32_t ret;
    ret = LOS_MemFree(m_aucSysMem0, mem);
}

gen_handle_t port_timer_create(
    uint32_t timeout_in_ms,
    void *user_data,
    void (*timer_cb)(void *))
{
    UINT32 *id = (UINT32 *)port_malloc(sizeof(UINT32));
    LOS_SwtmrCreate(LOS_MS2Tick(timeout_in_ms), LOS_SWTMR_MODE_NO_SELFDELETE, (SWTMR_PROC_FUNC)timer_cb, id, *(UINT32 *)user_data);
    return (gen_handle_t)id;
}

void port_timer_start(gen_handle_t timer)
{
    UINT32 *id = (UINT32 *)timer;
    LOS_SwtmrStart(*id);
}

void port_timer_stop(gen_handle_t timer)
{
    UINT32 *id = (UINT32 *)timer;
    LOS_SwtmrStop(*id);
}

void port_timer_delete(gen_handle_t timer)
{
    UINT32 *id = (UINT32 *)timer;
    LOS_SwtmrDelete(*id);
    port_free(id);
}

gen_handle_t port_task_create(
    const char *name,
    void (*entry)(void *),
    void *parameter,
    uint32_t stack_size, // stack size in bytes
    enum gen_os_task_priority priority)
{
    UINT32 taskId;
    TSK_INIT_PARAM_S initParam =
        {
            .uwArg = (UINT32)parameter,
            .pcName = (char *)name,
            .pfnTaskEntry = (TSK_ENTRY_FUNC)entry,
            .uwStackSize = (stack_size + 7) & ~0x7,
            .usTaskPrio = priority == GEN_TASK_PRIORITY_LOW ? OS_TASK_PRIORITY_HIGHEST + 7 : OS_TASK_PRIORITY_HIGHEST + 4,
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

extern VOID HalPendSV(VOID);
extern VOID HalExcSvcCall(VOID);

static void port_systick_handler(void)
{
    UINT32 intSave = LOS_IntLock();
    OsTickHandler();
    LOS_IntRestore(intSave);
}

VOID OsStart(VOID)
{
    LOS_Start();
}

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
        .svc_isr = HalExcSvcCall,
        .pendsv_isr = HalPendSV,
};

static struct
{
    uint8_t enhanced_ticks;
} pm_info;

void gen_os_enable_enhanced_ticks(void)
{
    platform_config(PLATFORM_CFG_RTOS_ENH_TICK, 1);
    pm_info.enhanced_ticks = 1;
}

STATIC volatile UINT32 g_SleepTime = 0;
STATIC VOID UserLpTimeStart(UINT64 nextResponseTime);
STATIC VOID UserLpTimeStop(VOID);
STATIC UINT64 UserLpTimeGet(VOID);
STATIC VOID UserKernelTimerLock(VOID);
STATIC VOID UserKernelTimerUnlock(VOID);
STATIC UINT32 UserDeepSleepSuspend(VOID);
STATIC VOID UserDeepSleepResume(VOID);
STATIC UINT32 UserDeviceSuspend(UINT32 mode);
STATIC VOID UserDeviceResume(UINT32 mode);
void OsSysTickTimerInit(UINT32 reloadValue);

STATIC LosPmTickTimer gs_PmTickSt = {
    .freq = OS_SYS_CLOCK,
    .timerStart = UserLpTimeStart,
    .timerStop = UserLpTimeStop,
    .timerCycleGet = UserLpTimeGet,
    .tickLock = UserKernelTimerLock,
    .tickUnlock = UserKernelTimerUnlock,
};

STATIC LosPmSysctrl gs_PmSysctrlSt = {
    /* Default handler functions, which are implemented by the product */
    .early = NULL,
    .late = NULL,
    .normalSuspend = ArchEnterSleep,
    .normalResume = NULL,
    .lightSuspend = ArchEnterSleep,
    .lightResume = NULL,
    .deepSuspend = UserDeepSleepSuspend,
    .deepResume = UserDeepSleepResume,
    .shutdownSuspend = NULL,
    .shutdownResume = NULL,
};

STATIC LosPmDevice gs_PmDeviceSt = {
    .suspend = UserDeviceSuspend,
    .resume = UserDeviceResume,
};

const gen_os_driver_t *os_impl_get_driver(void)
{
    LOS_KernelInit();
    OsSysTickTimerInit(LOSCFG_BASE_CORE_TICK_RESPONSE_MAX);
#ifdef LOSCFG_KERNEL_LOWPOWER
    LOS_PmRegister(LOS_PM_TYPE_TICK_TIMER, &gs_PmTickSt);
    LOS_PmRegister(LOS_PM_TYPE_SYSCTRL, &gs_PmSysctrlSt);
    LOS_PmRegister(LOS_PM_TYPE_DEVICE, &gs_PmDeviceSt);
    LOS_PmModeSet(LOS_SYS_DEEP_SLEEP);
#endif
    return &gen_os_driver;
}

#define _SYSTICK_PRI (*(uint8_t *)(0xE000ED23UL))

/* Constants required to manipulate the core.  Registers first... */
#define portNVIC_SYSTICK_CTRL_REG               (*((volatile uint32_t *)0xe000e010))
#define portNVIC_SYSTICK_LOAD_REG               (*((volatile uint32_t *)0xe000e014))
#define portNVIC_SYSTICK_CURRENT_VALUE_REG      (*((volatile uint32_t *)0xe000e018))
#define portNVIC_SYSPRI2_REG                    (*((volatile uint32_t *)0xe000ed20))
#define portNVIC_CCR_REG                        (*((volatile uint32_t *)0xE000ED14))
/* ...then bits in the registers. */
#define portNVIC_SYSTICK_CLK_BIT                (0UL << 2UL)
#define portNVIC_SYSTICK_INT_BIT                (1UL << 1UL)
#define portNVIC_SYSTICK_ENABLE_BIT             (1UL << 0UL)
#define portNVIC_SYSTICK_COUNT_FLAG_BIT         (1UL << 16UL)
#define portNVIC_PENDSVCLEAR_BIT                (1UL << 27UL)
#define portNVIC_PEND_SYSTICK_CLEAR_BIT         (1UL << 25UL)
#define portSY_FULL_READ_WRITE                  15

#define RTC_CYCLES_PER_TICK                     (RTC_CLK_FREQ / LOSCFG_BASE_CORE_TICK_PER_SECOND)
#define MAXIMUM_SUPPRESSED_TICKS                (0xffffff / RTC_CYCLES_PER_TICK)
#define EXPECTED_IDLE_TIME_BEFORE_SLEEP         2
#define MISSED_COUNTS_FACTOR                    15

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#define STOPPED_TIMER_COMPENSATION              (MISSED_COUNTS_FACTOR / (SYSCTRL_GetPLLClk() / RTC_CLK_FREQ))
#define API_EXPECTED_TICK_FREQ                  1024
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#define STOPPED_TIMER_COMPENSATION              (MISSED_COUNTS_FACTOR / (PLL_CLK_FREQ / RTC_CLK_FREQ))
#define API_EXPECTED_TICK_FREQ                  1000
#else
#error "unknown chip"
#endif

#define TICK_NUM_SCALE                          (API_EXPECTED_TICK_FREQ / LOSCFG_BASE_CORE_TICK_PER_SECOND)

void OsSysTickTimerInit(UINT32 reloadValue)
{
    // Set CCR register value to default
    #if (__CORTEX_M == 0x04U)
    portNVIC_CCR_REG = 0x200; 
    #elif (__CORTEX_M == 0x03U) 
    portNVIC_CCR_REG = 0x0; 
    #endif
    

    if ((reloadValue - 1UL) > 0xffffff)
    {
        return;
    }
    portNVIC_SYSTICK_CTRL_REG = 0;
    portNVIC_SYSTICK_LOAD_REG = (uint32_t)(reloadValue - 1UL);
    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
    portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT |
                                portNVIC_SYSTICK_INT_BIT |
                                portNVIC_SYSTICK_ENABLE_BIT;
}

#ifdef LOSCFG_KERNEL_LOWPOWER
STATIC VOID UserLpTimeStart(UINT64 nextResponseTime)
{
    UINT32 intSave;

    intSave = LOS_IntLock();
    g_SleepTime = nextResponseTime / (LOSCFG_BASE_CORE_TICK_RESPONSE_MAX);
    if (g_SleepTime < MISSED_COUNTS_FACTOR)
    {
        g_SleepTime = 0;
        __WFI();
        LOS_IntRestore(intSave);
        return;
    }

    g_SleepTime = platform_pre_suppress_ticks_and_sleep_processing(g_SleepTime);
    LOS_IntRestore(intSave);
}

STATIC VOID UserLpTimeStop(VOID)
{
}

STATIC UINT64 UserLpTimeGet(VOID)
{
    return g_SleepTime * LOSCFG_BASE_CORE_TICK_RESPONSE_MAX;
}

STATIC VOID UserKernelTimerLock(VOID)
{
}

STATIC VOID UserKernelTimerUnlock(VOID)
{
}

STATIC UINT32 UserDeepSleepSuspend(VOID)
{
    UINT32 intSave;
    uint32_t ulCompleteTickPeriods;

    if (g_SleepTime == 0)
    {
        return 0;
    }
    intSave = LOS_IntLock();
    portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT; // close systick
    // calculate the expected ticks
    uint32_t ulReloadValue = portNVIC_SYSTICK_CURRENT_VALUE_REG + (RTC_CYCLES_PER_TICK * (g_SleepTime - 1UL));
    if (ulReloadValue > STOPPED_TIMER_COMPENSATION)
    {
        ulReloadValue -= STOPPED_TIMER_COMPENSATION;
    }
    portNVIC_SYSTICK_LOAD_REG = ulReloadValue;
    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
    portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

    platform_pre_sleep_processing();
    platform_post_sleep_processing();

    portNVIC_SYSTICK_CTRL_REG = (portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT);
    if ((portNVIC_SYSTICK_CTRL_REG & portNVIC_SYSTICK_COUNT_FLAG_BIT) != 0)
    {
        uint32_t ulCalculatedLoadValue;

        ulCalculatedLoadValue = (RTC_CYCLES_PER_TICK - 1UL) - (ulReloadValue - portNVIC_SYSTICK_CURRENT_VALUE_REG);

        if ((ulCalculatedLoadValue < STOPPED_TIMER_COMPENSATION) || (ulCalculatedLoadValue > RTC_CYCLES_PER_TICK))
        {
            ulCalculatedLoadValue = (RTC_CYCLES_PER_TICK - 1UL);
        }

        portNVIC_SYSTICK_LOAD_REG = ulCalculatedLoadValue;
        ulCompleteTickPeriods = g_SleepTime - 1UL;
    }
    else
    {
        uint32_t ulCompletedSysTickDecrements = (g_SleepTime * RTC_CYCLES_PER_TICK) - portNVIC_SYSTICK_CURRENT_VALUE_REG;
        ulCompleteTickPeriods = ulCompletedSysTickDecrements / RTC_CYCLES_PER_TICK;

        portNVIC_SYSTICK_LOAD_REG = ((ulCompleteTickPeriods + 1UL) * RTC_CYCLES_PER_TICK) - ulCompletedSysTickDecrements;
    }

    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
    portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
    // update tick

    portNVIC_SYSTICK_LOAD_REG = RTC_CYCLES_PER_TICK - 1UL;
    g_SleepTime = ulCompleteTickPeriods;

    LOS_IntRestore(intSave);

    return 0;
}

STATIC VOID UserDeepSleepResume(VOID)
{
    UINT32 intSave;

    intSave = LOS_IntLock();
    platform_os_idle_resumed_hook();
    LOS_IntRestore(intSave);
}

STATIC UINT32 UserDeviceSuspend(UINT32 mode)
{
    (UINT32) mode;
    return 0;
}

STATIC VOID UserDeviceResume(UINT32 mode)
{
    (UINT32) mode;
}
#endif

extern UINT8 *m_aucSysMem0;
void platform_get_heap_status(platform_heap_status_t *status)
{
    static uint16_t bytes_minimum_ever_free = LOSCFG_SYS_HEAP_SIZE;
    LOS_MEM_POOL_STATUS s;
    UINT32 used_size, pool_size;

    status->bytes_minimum_ever_free = bytes_minimum_ever_free;
    if (LOS_OK == LOS_MemInfoGet(m_aucSysMem0, &s))
    {
        status->bytes_free = s.totalFreeSize;
        if (status->bytes_free < bytes_minimum_ever_free)
            bytes_minimum_ever_free = status->bytes_free;
        return;
    }
    else
        status->bytes_free = 0;
    status->bytes_minimum_ever_free = bytes_minimum_ever_free;
}
