#ifndef _port_gen_os_driver_h
#define _port_gen_os_driver_h

#include "port_gen_os.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gen_os_driver
{
    // see `port_xxxx` in `port_gen_os.h`
    // All functions in this struct should be implemented and provided, except
    // for Link Layer Extension API only mode, in which case, some functions
    // must/can be omitted:
    // - `task_create`: **must** be NULL
    // - timer APIs: ignored
    // - queue APIs: ignored
    // - `event_create` / `event_wait`/ : ignored
    // - `event_set`: must be provided (an empty function is OK)
    // - `malloc` / `free`: ignored
    // - `enter_critical` / `exit_critical`: must be provided
    // - `os_start`: must be provided
    // - Exception handlers: must be provided

    gen_handle_t (* timer_create)(
        uint32_t timeout_in_ms,
        void *user_data,
        void (* timer_cb)(void *)
    );
    void (* timer_start)(gen_handle_t timer);
    void (* timer_stop)(gen_handle_t timer);
    void (* timer_delete)(gen_handle_t timer);

    gen_handle_t (* task_create)(
        const char *name,
        void (*entry)(void *),
        void *parameter,
        uint32_t stack_size,                    // stack size in bytes
        enum gen_os_task_priority priority
    );

    gen_handle_t (* queue_create)(
        int len,
        int item_size
    );
    int (* queue_send_msg)(
        gen_handle_t queue,
        void *msg
    );
    // return 0 if msg received; otherwise failed (timeout)
    int (* queue_recv_msg)(
        gen_handle_t queue,
        void *msg
    );

    gen_handle_t (* event_create)();
    int (* event_wait)(gen_handle_t event);
    void (* event_set)(gen_handle_t event);

    /**
     ****************************************************************************************
    * @brief allocate memory in the heap
    *
    * Note: This is only used in tasks.
    *
    * @param[in] size                   size of the memory to be allocated
    * @return                           point to the buffer
    ****************************************************************************************
    */
    void *(*malloc)(uint32_t size);

    /**
     ****************************************************************************************
    * @brief free the memory allocated by `malloc`
    *
    * Note: This is only used in tasks.
    *
    * @param[in] buf                   point to the buffer
    ****************************************************************************************
    */
    void  (*free)(void *buf);

    /**
     ****************************************************************************************
    * @brief enter a global critical section
    ****************************************************************************************
    */
    void (*enter_critical)(void);

    /**
     ****************************************************************************************
    * @brief leave a global critical section
    ****************************************************************************************
    */
    void (*leave_critical)(void);

    /**
     ****************************************************************************************
    * @brief Start the RTOS
    * This function should never return
    ****************************************************************************************
    */
    void (*os_start)(void);

    /**
     ****************************************************************************************
    * @brief SysTick ISR
    ****************************************************************************************
    */
    void (*tick_isr)(void);
    /**
     ****************************************************************************************
    * @brief SVC ISR
    ****************************************************************************************
    */
    void (*svc_isr)(void);
    /**
     ****************************************************************************************
    * @brief PendSV ISR
    ****************************************************************************************
    */
    void (*pendsv_isr)(void);
} gen_os_driver_t;

/**
 ****************************************************************************************
* @brief Get the RTOS driver from an implentation
****************************************************************************************
*/
const gen_os_driver_t *os_impl_get_driver(void);

#ifdef __cplusplus
}
#endif

#endif
