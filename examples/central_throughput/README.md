# Central Throughput

This example demonstrates how to:

* Use central role to:
    * Scan & connect to peripheral devices;
    * Discovery services & characteristics defined by 128bit UUID;
    * Get notification when characteristics value updated;
    * Write to characteristics value;
* Update PHY/connection parameters;
* Moniter connection (check RSSI);
* Use keys;
* Use OLED to display information.

Note:
    1. This example works with `peripheral_throughput`;
    1. To use command `loopback`, `peripheral_throughput` should be
       running in `LOOPBACK` mode;
    1. OLED can be enabled by defining `USE_DISPLAY`
       (see `oled_ssd1306.h` for hardware setup).
