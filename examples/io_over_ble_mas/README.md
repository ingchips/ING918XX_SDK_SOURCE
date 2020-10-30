# I/O Over BLE (Central)

This example demonstrates how to:

* Build a generic I/O service over BLE (central part):
    * Scanning for devices;
    * Scan & connect to peripheral devices;
    * Discovery services & characteristics;
    * Get notification when characteristics value updated
* Transfer different data types over the generic I/O service:
    * Controlled by IO\_TYPE
    * Support UART and others

Note: 
    1. this example can work with "I/O Over BLE (Peripheral)".
       For UART data, this example also works with
       "Peripheral Console";
    2. addresses are stored in flash, check `pair_config_t`.
       Central & peripheral use the same configure data.
