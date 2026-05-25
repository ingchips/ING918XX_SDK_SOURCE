# Enhanced ATT (EATT) Example

This example shows how to use Enhanced ATT (EATT) on ING chips with a simple UART console.

It can run in either role:

* Master/Central: connect to a peripheral, perform GATT discovery, and start an EATT client with multiple bearers.
* Slave/Peripheral: advertise a custom GATT database, accept a connection, and expose read/write/notify/indicate characteristics.

## What It Demonstrates

* GATT server setup with EATT support.
* EATT client setup with multiple bearers.
* Parallel characteristic read/write operations over EATT.
* Notification and indication handling on the peripheral side.
* UART command control for testing the full flow.

## Project Layout

* `src/main.c` initializes the platform, UART console, watchdog, and trace output.
* `src/profile.c` contains the BLE profile, GATT database, role-specific logic, and EATT setup.
* `src/eatt_uart_console.c` implements the UART command interface.
* `data/` contains the generated GATT/advertising data used by the example.

## Build And Flash

This example is provided as a Keil MDK project.

1. Open `eatt.uvprojx` in Keil.
2. Select the target board and build configuration that matches your hardware.
3. Build and download the firmware to the board.
4. Open a UART terminal at the console baud rate used by your board configuration.

## Role Configuration

The active role is selected in `src/profile.c`:

```c
#ifndef APP_ROLE
#define APP_ROLE        CONST_MASTER
// #define APP_ROLE        CONST_SLAVE
#endif
```

* `CONST_MASTER` is the default role.
* Switch to `CONST_SLAVE` to run the peripheral/advertising side.

## How To Run

### Slave / Peripheral

1. Flash the firmware with `APP_ROLE` set to `CONST_SLAVE`.
2. After boot, the device prints its role and waits for advertising to be started.
3. Use the `adv` command in the UART console to start advertising.
4. After the master connects, the slave can send notifications and indications.

### Master / Central

1. Flash the firmware with `APP_ROLE` set to `CONST_MASTER`.
2. Wait for pairing to complete; the example then discovers the remote GATT database, checks whether the server supports EATT, and starts the EATT client.
3. Use the UART commands below to trigger concurrent reads and writes.

## UART Commands

The console accepts the following commands:

* `h` or `?`: show help.
* `r`: read custom characteristics.
* `w`: write custom characteristics.
* `wn`: write custom characteristics without response.
* `n`: trigger a notification test.
* `i`: trigger an indication test.
* `adv`: start advertising on the slave side.

## Expected Behavior

When the connection is established and EATT is available, the master attempts to read or write multiple custom characteristics in parallel, one per bearer. The slave side returns characteristic values with timestamps so you can see each request being handled independently.

If the peer does not support EATT, the example still connects, but the EATT-specific multi-bearer flow will not start.

## Notes

* The example uses a fixed number of EATT bearers configured in `src/profile.c`.
* The custom characteristic handles are generated at runtime and stored in the `custom_char_handles` array.
* The UART console is started from `src/main.c`, so the board must have a working UART output path for logs and commands.