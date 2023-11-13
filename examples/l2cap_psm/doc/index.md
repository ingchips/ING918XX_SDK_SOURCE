# L2CAP PSM

This example can be built as _Central_ or _Peripheral_ role, and create an LE
credit based connection between two devices. Role is defined by macro `APP_ROLE`:

|Value |Role                      |
|:----:|:-------------------------|
| 0    | _Central_                |
| 1    | _Peripheral_             |

Once connected, _Central_ sends "PING" to the peer periodically. _Peripheral_
responses "PONG" to each "PING" message.

Both roles will track each other's credits, and assign more credits to the peer automatically.

:exclamation:**IMPORTANT:** Value of SPSM should be negotiated properly. This example focuses on the usage L2CAP APIs.

## Test

Build this example for two times, one with `APP_ROLE=0`, and the other `APP_ROLE=1`.
Download to two Dev-Boards respectively. Check the output from the default UART.
