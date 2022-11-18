# BLE RPC

This example exposes an RPC (Remote Procedure Call) server interface over UART.

Download this app to a Dev-Board, and connect to it with the RPC client.
For detailed information, see [BLE RPC Client](../../../example_host/rpc_client/doc/index.md).

## Hardware Setup

| IO Pin          | Note                         |
| ---             | ---                          |
| Default UART Tx | RPC communication UART Tx    |
| Default UART Rx | RPC communication UART Rx    |
| 4               | Debug UART Tx (i.e. `printf` logging)  |

