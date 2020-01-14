# I/O Over BLE (Master)

This example demonstrates how to:

* Build a generic I/O service over BLE (master part):
    * Scanning for devices;
    * Scan & connect to peripheral devices;
    * Discovery services & characteristics;
    * Get notification when characteristics value updated
* Transfer different data types over the generic I/O service:
    * Controlled by IO\_TYPE
    * Support UART and others

Note: 
    1. this example can work with `io_over_ble_sla`.
       For UART data, this example also works with
       `peripheral_console`;
    2. addresses are stored in flash, check `pair_config_t`.
       Master & slave use the same configure data.
