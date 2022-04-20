# Massive Connections

This example demonstrate that `MAX_CONN_NUMBER` connections can be supported for both central &
peripheral roles.

When each new connection is established or a connection is lost, connection and heap status are printed.

_Central_ tries to connect to peripherals whose most significant 5 bytes of addresses are defined by
`peer_addr`, and the least significant byte is `[0 .. MAX_CONN_NUMBER - 1]`. When _Peripheral_ sets up
its advertising, it uses the first connection ID that has not been established as the least significant byte
of address: if no connection is established, `peer_addr` is its address; if connection ID 0 & 1
have been established and ID 2 not, the least significant byte of address is `2`.

## Test

1. Use `MAX_CONN_NUMBER + 1` Dev-Boards or modules

    * Download _Central_ to one Dev-Board, and _Peripheral_ to all other boards or modules,
      and check UART log of _Central_ that all _Peripherals_ can be connected,
      (**Remember** to update _Peripheral_ to use a fixed address), or

    * Download _Peripheral_ to one Dev-Board, and _Central_ to all other boards or modules,
      and check UART log of _Peripheral_ that all _Centrals_ can be connected.

1. Use 2 Dev-Boards

    Since both _Central_ and _Peripheral_ can supported `MAX_CONN_NUMBER` simultaneous connections,
    we can download _Central_ & _Peripheral_ to two Dev-Boards and wait for the establishing
    of all connections between these boards.

    Note: BLE Spec does not allow multiple connections between two devices. This is for test only.


After _Central_ connects to all `MAX_CONN_NUMBER` peripheral, it will output (take `MAX_CONN_NUMBER`
is 24 as an example):

```
Connected to #xx. Total = 24
Conn Map:
 0: ********
 8: ********
16: ********
ALL Connected
heap status:
    free: ... B
min free: ... B
```

## Performance Tips

1. Connection parameters for initiation need to be fine tuned for better performance;
1. `gap_update_connection_parameters` is critical.
