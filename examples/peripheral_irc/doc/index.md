# Peripheral IRC

This is a BLE chat relay, demonstrating that the device can be connect by several Central devices.

## Test

Download this example to a Dev-Board. Connect 4 devices with _ING BLE_ to the
_ING Relay Chat - X_ devices. Then a simple chat room is created:

<img src="./img/test.png" width="70%" />

## Design Details

* When a piece of data (a chatting message) is written to the device from one _Central_ device, it is
    forwarded to all other _Central_ devices.