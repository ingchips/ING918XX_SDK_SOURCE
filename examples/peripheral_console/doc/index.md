# Peripheral Console

This example provides a simple console over BLE, after connected with _ING BLE_, commands can be
issued, and response are displayed.

This example also demonstrate the use of power saving, which can be turned on and off on-the-fly
through a command.

There are several compiling switches:

* **LISTEN_TO_POWER_SAVING**

    When defined, a buzzer can be used as an indicator that the SoC has entered deep sleep and waken
    up again.

* **USE_WATCHDOG**

    Enable watchdog.

* **USE_POWER_LIB** (ONLY for ING918xx)

    To further reduce power consumption.

* **USE_SLOW_CLK_RC** (ONLY for ING916xx)

    Use internal RC clock as source for _slow_clk_. Different frequencies can be
    selected:

    | **USE_SLOW_CLK_RC**  | Frequency (MHz)    |
    |:--------:|:-----------:|
    | 8  | 8  |
    | 16 | 16 |
    | 24 | 24 |
    | 32 | 32 |
    | 48 | 48 |
    | 64 | 64 |

## Hardware Setup

### ING918xx Dev-Board

| Dev-Board| Pin  | Note                                    |
|:--------:|:----:|:----------------------------------------|
| EXT_INT  | EXT_INT |                              |
| EXT_INT  | GPIO 9  | Key detection                |
| Buzzer   | GPIO 8  | Listen to power saving       |


### ING916xx Dev-Board

| Dev-Board| Pin  | Note|
|:--------:|:----:|:----------------------------------------|
| EXT_INT  | GPIO 0  | Emulating EXT_INT on ING918xx        |
| EXT_INT  | GPIO 9  | Key detection                        |
| Buzzer   | GPIO 8  | Listen to power saving               |

## Test

Download this example to Dev-Board, the buzzing indicates that the SoC has entered deep sleep and waken
up again, busy with advertising, etc. Connect it with _ING BLE_, the buzzing will become faster indicating
that the SoC is now busy with connection activities which is more frequent than advertising.

The console supports several commands. Some notable ones are listed below. Check out
[`service_console.c`](../src/service_console.c) for more commands.

* `ps`

    `ps 0` will disable power saving and the buzzing will stop, while `ps 1` will re-enable it.

* `ver`

    Show the version number of platform binary.

* `f`

    Show the actual frequency of the 32k clock.

* `f-cpu`

    Show the configured frequency of CPU.

* `m`

    Show memory usage information.

* `latency`

    Self-assign a peripheral latency parameter. For example, after issuing command `latency 4`, the buzzing
    will become slower since some connection events are skipped, and power consumption is reduced too.

Hold the `EXT_INT` key down will prevent the SoC from entering deep sleep mode, and the buzzing will
stop, too. Release the key, the buzzing resumes. If connected with _ING BLE_, a key event is also
reported when `EXT_INT` is clicked, clicked for multiple times, or keep pressed for a period of time.