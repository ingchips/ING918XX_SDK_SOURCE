# Central Throughput

This example demonstrates how to:

* Use central (master) role to:
    * Scan & connect to peripheral devices;
    * Discovery services & characteristics defined by 128bit UUID;
    * Get notification when characteristics value updated;
    * Write to characteristics value;
* Update PHY/connection parameters;
* Moniter connection (check RSSI).

Note:
    1. This example works with `peripheral_throughput`;
    1. To use command `loopback`, `peripheral_throughput` should be
       running in `LOOPBACK` mode.
