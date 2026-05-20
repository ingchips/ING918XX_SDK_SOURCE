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
3. The Master will initiate connection automatically after startup.
4. Once the connection is established, the devices complete pairing and bonding, then EATT bearers are created.
5. Use UART commands such as `r`, `w`, `wn`, `n`, and `i` to verify concurrent read/write/notify/indicate behavior.
6. If EATT is unavailable and the stack falls back to UATT, concurrent indication behavior is limited by single outstanding confirmation.
