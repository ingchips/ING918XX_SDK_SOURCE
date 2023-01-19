# Peripheral Console

This example demonstrates how to:

* Design something like serial port in BLE;
* Power saving;
* Use RC for _slow_clk_
* Detect `EXT_INT` key events in power saving;
* Over-the-air debugging;
* RF power mapping.

Note:
    1. '\0' masks the end of a string;
    1. Connect `EXT_INT` key to both `EXT_INT` and `KEY_PIN`.

Known Issue: When `USE_POWER_LIB`, only single click is OK, and other
    key events (multiple clicked & long pressed) might break BLE connection
    with small interval.
