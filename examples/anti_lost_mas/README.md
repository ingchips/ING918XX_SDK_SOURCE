# Anti-Lost (Master)

This example demonstrates how to:

* Make an anti-lost kit based on active scanning;
* Use central role to:
    * Scan for devices;
    * Use white list;
* Use GPIO/PWM (see `main.c` for hardware connections);
* Use software timers.

Note that this example:

1. Works with `anti_lost_sla`;
2. Blinks when a proper advertising is received;
3. Turns on the buzzer when timed out. 
