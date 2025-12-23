# EATT Example: UART GATT Console

## Overview

The EATT (Enhanced Attribute Protocol) example demonstrates a comprehensive set of Bluetooth Low Energy (BLE) functionalities controlled via UART commands. This project show cases advanced BLE features including advertising, scanning, connection management, GATT operations over eatt channels

## Key Features
- **GATT Operations**: Read, write, and subscribe to characteristics over EATT channel

## Command Interface

The example provides a UART-based command line interface for controlling BLE operations. Commands cover advertising, scanning, connection establishment, GATT client operations, connection parameters, power management, and subrating.

## Supported Platforms

This example is designed for ING918XX series development boards and demonstrates the capabilities of the ING918XX BLE stack.

## Getting Started

1. Build and flash the example to your development boards, one is master, the other is slave
2. slave is advertising 
3. master board use commands like `sconn xx:xx:xx:xx:xx:xx` to to establish connection.
4. it will connect then bonding then  perform EATT operations
