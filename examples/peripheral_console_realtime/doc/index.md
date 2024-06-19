# Peripheral Console (Realtime)

This example is the same as [_Peripheral Console_](../../peripheral_console/doc/index.md), except that
no RTOS is used, and `EXT_INT` key event report is not available, either.

Since no RTOS is used, it is expected to have better performance in some realtime scenarios.

## Design Details

The generic RTOS driver interface is in [`noos_impl.c`](../src/noos_impl.c).

### Task Scheduling

There two tasks in the BLE stack, the Host task and Controller task. Generally, a RTOS is used to
run these two tasks. The Host task looks like:

```cpp
void host_task(void)
{
    host_init();
    while (true)
    {
        if (recv_msg(msg) != 0) continue;
        process_msg(msg);
    }
}
```

The Controller task looks like:

```cpp
void controller_task(void)
{
    platform_init_controller();
    while (true)
    {
        if (wait_for_event(event) != 0) continue;
        platform_controller_run();
    }
}
```

Due to the differences of these two tasks, Controller task can be merged with the Host task, then RTOS
is not needed any more. This is what `queue_recv_msg` does:

```c
static int queue_recv_msg(gen_handle_t queue, void *msg)
{
    platform_controller_run();

    struct simple_queue *q = (struct simple_queue *)queue;
    if (no message in queue)
    {
        idle_process();
        return -1;
    }

    copy message;
    return 0;
}
```

### Power Saving

The idle process `idle_process` is called each time the message queue is empty.

### SysTick

Although this example does not need ticks, SysTick's counter must be enabled because the internal
wake up process will always ensure that SysTick is counting normally. SysTick exception is disabled.

### Timers

Timer API in the generic RTOS driver interface is used by Host task. With the help of
`platform_set_timer`, timers are implemented without SysTick. For simplicity, the supported number
of timers (`SW_TIMER_NUMBER`) is hard-coded.

## Limitations

`noos_impl.c` is not a fully functional RTOS. Events, message queues, SysTick are not implemented.
When these features are required, please use a **true** RTOS instead. There are some tool
modules provided in SDK that requires a fully functional RTOS, therefore, these modules are
can't be used with this `noos_impl.c`:

* `btstack_mt`
* `btstack_sync`
* `ecc_driver`
* `trace` (`trace_full_dump2` does depends on RTOS, so it is OK.)
* `uart_driver`
* `ble_brpc`
