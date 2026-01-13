# EATT Example: UART GATT Console

## Overview

This example demonstrates how to use EATT (Enhanced Attribute Protocol) to perform GATT operations over multiple ATT bearers,

## Key Features
- **GATT Operations**: 
- GATT Client Operations over EATT: 
- Service and characteristic discovery
- Characteristic read

## Command Interface

The example provides a UART-based command line interface for controlling BLE operations. Commands cover advertising, connection establishment, EATT client operations

## Supported Platforms

This example is designed for all series development boards and demonstrates the capabilities of ING916XX ING918XX ING20.

## Getting Started

1. Build and flash the example firmware onto two development boards. One board acts as the Master, and the other acts as the Slave.
2. After booting, the Slave starts advertising.
3. On the Master board, use the following command to initiate a connection to the Slave:`sconn xx xx xx xx xx xx`
4. Once the connection is established, the devices complete pairing and bonding, and EATT (Enhanced ATT) channels are successfully created.
5. On the Master, perform GATT service and characteristic discovery using commands such as `discover`
6. The Master reads characteristics value using a command like `read 3 6 `,3 is attribute handle, 6 is the second one.
7. At this point, you can observe that many characteristics read are executed successfully.
8. If use only gatt without eatt , the second characteristic read will return error .
