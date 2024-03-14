# Extension: Packet Tx/Rx

This example use "extension" bundle to transmit/receive raw packets.

## Test

### Raw Packet Tx/Rx

Build this example twice, one with `FOR_TX=1`, and the other `FOR_TX=0`, and download them
to two Dev-Boards. Check the log printed through the default UART.

`APP_TESTER` should not be defined.

### Make ING916 into a Tester

Build this example with `APP_TESTER` and download it to an ING916 Dev-Board. Then, this
Dev-Board can be used as a tester by Frequency Offset Tester program.

## Other Resources

* _BLE Extensions Reference_