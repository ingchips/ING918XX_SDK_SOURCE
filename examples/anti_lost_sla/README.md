# Anti-Lost (Peripheral)

This example demonstrates how to:

* Make an anti-lost kit based on active scanning;
* Send scannable advertising;
* Use white list;
* Use GPIO/PWM (see `main.c` for hardware connections);
* Use software timers.

Note that this example:

1. Works with `anti_lost_mas`;
2. Blinks when a proper scan request is received;
3. Turns on the buzzer when timed out. 
