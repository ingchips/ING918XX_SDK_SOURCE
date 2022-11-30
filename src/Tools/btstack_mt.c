#include "btstack_mt.h"
#include "platform_api.h"
#include "port_gen_os_driver.h"
#include "btstack_event.h"

#ifndef MAX_EVENTS_NUM
#define MAX_EVENTS_NUM   5
#endif

struct btstack_mt_event
{
    struct btstack_mt_event *next;
    gen_handle_t event;
};

static struct btstack_mt_event first_node = {0};

#define GEN_OS          ((const gen_os_driver_t *)platform_get_gen_os_driver())
#define IS_IN_HOST()    (platform_get_current_task() == PLATFORM_TASK_HOST)

struct btstack_mt_event *btstack_mt_event_alloc(void)
{
    struct btstack_mt_event * r = (void *)0;
    GEN_OS->enter_critical();

    if (first_node.next == (void *)0)
    {
        r = (struct btstack_mt_event *)GEN_OS->malloc(sizeof(struct btstack_mt_event));
        r->next = (void *)0;
        r->event = GEN_OS->event_create();
        first_node.next = r;
    }
    r = first_node.next;
    first_node.next = r->next;

    GEN_OS->leave_critical();

    return r;
}

void btstack_mt_event_free(struct btstack_mt_event * event)
{
    GEN_OS->enter_critical();
    event->next = first_node.next;
    first_node.next = event;
    GEN_OS->leave_critical();
}

void btstack_mt_event_wait(struct btstack_mt_event * event)
{
    GEN_OS->event_wait(event->event);
}

void btstack_mt_event_set(struct btstack_mt_event * event)
{
    GEN_OS->event_set(event->event);
}

#define CAT(x, y) CAT_(x, y)
#define CAT_(x, y) x ## y

#define CALL_USER_VOID(fn)      do { \
    btstack_push_user_runnable((f_btstack_user_runnable)CAT(fn, _0), &ctx, 0);  \
    btstack_mt_event_wait(ctx._event);                                        \
    btstack_mt_event_free(ctx._event); } while (0)

#define CALL_USER_RET(fn)      do { \
    btstack_push_user_runnable((f_btstack_user_runnable)CAT(fn, _0), &ctx, 0);  \
    btstack_mt_event_wait(ctx._event);                                        \
    btstack_mt_event_free(ctx._event);                                     \
    return ctx._ret; } while (0)

// Machine-generated code. DO NOT modify.
#include "btstack_mt.inc"
