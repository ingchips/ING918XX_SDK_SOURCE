#ifndef _port_gen_os_h
#define _port_gen_os_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *gen_handle_t;

enum gen_os_task_priority
{
    GEN_TASK_PRIORITY_LOW,      // for host task
    GEN_TASK_PRIORITY_HIGH,     // for controller task
};

/**
 ****************************************************************************************
 * @brief Create a software timer
 *
 * Note: 1. The timer must be in STOPPED state after creation.
 *       2. Timer should not auto reload (one-shot timer).
 *
 * @param[in] timeout_in_ms         timeout is millisecond
 * @param[in] user_data             user data for timer callback
 * @param[in] timer_cb              timer callback
 * @return                          handle of the timer
 ****************************************************************************************
 */
gen_handle_t port_timer_create(
        uint32_t timeout_in_ms,
        void *user_data,
        void (* timer_cb)(void *)
    );

/**
 ****************************************************************************************
 * @brief Start a software timer
 *
 * @param[in] timer                 handle of the timer
 ****************************************************************************************
 */
void port_timer_start(gen_handle_t timer);

/**
 ****************************************************************************************
 * @brief Stop a software timer
 *
 * @param[in] timer                 handle of the timer
 ****************************************************************************************
 */
void port_timer_stop(gen_handle_t timer);

/**
 ****************************************************************************************
 * @brief Delete a software timer
 *
 * All resources allocated for the timer should be freed/deleted.
 *
 * @param[in] timer                 handle of the timer
 ****************************************************************************************
 */
void port_timer_delete(gen_handle_t timer);

/**
 ****************************************************************************************
 * @brief Create a task
 *
 * Note: 1. The timer must be in STOPPED state after creation.
 *       2. Timer should not auto reload.
 *
 * @param[in] name                  name of task
 * @param[in] entry                 entry of the task
 * @param[in] parameter             user data for the `entry`
 * @param[in] stack_size            stack size in bytes
 * @param[in] priority              priority of the task
 * @return                          handle of the task
 ****************************************************************************************
 */
gen_handle_t port_task_create(
        const char *name,
        void (*entry)(void *),
        void *parameter,
        uint32_t stack_size,                    // stack size in bytes
        enum gen_os_task_priority priority
    );

/**
 ****************************************************************************************
 * @brief Create a message queue
 *
 * A message queue stores a list of messages by value.
 * A message is a list of bytes.
 *
 * @param[in] len                   maximum allowed number of messages of the queue
 * @param[in] item_size             length of a message
 * @return                          handle of the queue
 ****************************************************************************************
 */
gen_handle_t port_queue_create(int len, int item_size);

/**
 ****************************************************************************************
 * @brief Send a message to a queue
 *
 * This function can be used ISR or task context.
 *
 * @param[in] queue                 handle of the queue
 * @param[in] msg                   the message
 * @return                          0 if successful else `error_code`
 ****************************************************************************************
 */
int port_queue_send_msg(gen_handle_t queue, void *msg);

/**
 ****************************************************************************************
 * @brief Receive a message from a queue
 *
 * This function can be used ISR or task context.
 *
 * @param[in] queue                 handle of the queue
 * @param[in] msg                   the received message
 * @return                          0 if successful else `error_code`
 ****************************************************************************************
 */
int port_queue_recv_msg(gen_handle_t queue, void *msg);

/**
 ****************************************************************************************
 * @brief Create an event
 *
 * An event can be waited for. Once `set`, the event is put into `signaled` state, and
 * the waiting task is resumed.
 *
 * @return                          handle of the event
 ****************************************************************************************
 */
gen_handle_t port_event_create(void);

/**
 ****************************************************************************************
 * @brief Wait for an event to be signaled
 *
 * Once this function is returned (the event is set to `signaled`), the event is changed
 * to un-signaled again.
 *
 * @param[in]                       handle of the event
 ****************************************************************************************
 */
int port_event_wait(gen_handle_t event);

/**
 ****************************************************************************************
 * @brief Set an event into signaled state
 *
 * @param[in]                       handle of the event
 ****************************************************************************************
 */
void port_event_set(gen_handle_t event);

#ifdef __cplusplus
}
#endif

#endif
