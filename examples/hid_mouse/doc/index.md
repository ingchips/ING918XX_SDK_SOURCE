# HID Mouse

This example implements an HID mouse with Just Works paring.

## Hardware Setup

* ING918xx Dev-Board

    <img src="img/overview.png" width="50%" alt></img>

    Connect jumpers for GPIO 4 (Key 4) & 6 (LED 3).

* ING916xx Dev-Board

    Connect jumpers for GPIO 9 (Key 4) & 6 (LED 3).

    On ING916xx, two work modes are supported, `SIMULATION` and mouse wheel mode.
    To test mouse wheel mode, remove compiling option `SIMULATION` and
    connect a scroll wheel's A/B signals to GPIO 21 & 32.

## Test

Keep Key 4 pressed for several seconds then release, LED 3 will start flashing. Now take this
example as a normal Bluetooth mouse and add it to the system. This example has been tested on below
systems:

* RedMi Note5 (MIUI 11.0.2)
* iPhone XR (iOS 14.5)

    Note: If their is no pointer on the screen, turn on AssistiveTouch.

* Thinkbook 13s (Win10 Home 20H2, Win11 Home).

### Simulation Mode

After the mouse is added, the pointer will start to move in a cycle. Size of the cycle depends on
the resolution of the hardware and the underlying operating system.

![](./img/test_iphone.gif)

### Mouse Wheel Mode

Scroll the wheel and check if it works like the middle "wheel" of a mouse.

## Design Details

This example uses resolvable address, and device's identity address is protected from unintentional
access.

Mouse movement is simulated by sending predefined $(d x, d y)$ pairs (see `delta_xy`) periodically.

Paring data is stored into Flash with the help of `kv_storage` module.