# I/O Over BLE

This pair of examples (_Central_ and _Peripheral_) create a opaque channel over BLE between two
devices identified by `pair_config`.

Following types of I/O are defined:

1. `IO_TYPE_UART_STR`

    Strings send to device A's UART will be transfer to device B, and output from its UART, and vice versa.

1. `IO_TYPE_UART_BIN`

    Data send to device A's UART will be transfer to device B, and output from its UART, and vice versa.

    Data input from UART are treated as raw bytes and transferred to peer device untouched, while
    in `IO_TYPE_UART_STR`, data are firstly split into strings then transferred.

1. `IO_TYPE_LOOPBACK`

    Data received from BLE are automatically bounced back. Obviously that, only one device (_Central_
    or _Peripheral_) is allowed to use this type of I/O.

## Hardware Setup

Jumpers of both _Central_ and _Peripheral_ marked as yellow should be connected:

<img src="./img/hardware.png" width="50%">

## Design Details

_Central_ and _Peripheral_ share some source codes.

A ring buffer (`ring_buf` module) is used to cache UART data. High water mark event is used to trigger BLE
transmission.