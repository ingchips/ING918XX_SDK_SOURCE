# Smart Home Hub

This example demonstrates how to:

* Design an BLE hub (BLE gateway) supporting lights and thermometers;
* Use central role to:
    * Scan & connect to peripheral devices;
    * Discovery SIG and user defined services & characteristics;
    * Get notification when characteristics value updated;
* Use multiple central & multiple periphral roles simultaneously.

Note: 

1. This demo searches for slaves with specific addresses;
1. Slaves should support SIG Health Thermometer and/or 
   INGChips LED service.

