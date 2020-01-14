# Smart Meter

This example demonstrates how to:

* Use central role to:
    * Scan & connect to peripheral devices;
    * Discovery services & characteristics;
    * Get notification when characteristics value updated;
* Use multiple central & multiple periphral roles simultaneously.

Note: 

1. This demo has a command line interface on UART0 (see also 
`uart_console`), and output temperature information over the 
air (see also `peripheral_console`);
1. This demo searches for slaves with specific addresses;
1. Slaves should support SIG Health Thermometer service.
