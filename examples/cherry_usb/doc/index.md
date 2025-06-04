# Supports host and device modes

The configuration is done through the usb_config.h file by enabling two key macros:

CONFIG_USE_USB_HOST: Enables the USB host functionality.
CONFIG_USE_USB_DEVICE: Enables the USB device functionality.
With these settings, the ING916 chip is able to act as both a USB host and a USB device at the same time.

## Host Side Functionality

In host mode, the system supports USB mass storage devices (MSC). The example shows how to:

Detect and enumerate a connected USB MSC device.
Send SCSI commands to read data from the storage media.
Read one sector of data (typically 512 bytes) and display or process it.
This provides a basic but complete demonstration of how to access external USB storage using CherryUSB on the ING916 platform.

## Device Side Functionality

In device mode, the system acts as a Human Interface Device (HID) mouse. It performs the following tasks:

Enumerates as a standard USB HID mouse when connected to a host PC.
Sends periodic HID reports to simulate mouse movement.
Updates mouse coordinates at fixed intervals to demonstrate real-time interaction with the host.
This part helps developers understand how to create custom USB device applications using CherryUSB.

## Target Platform

This project is built specifically for the ING916 series chips and does not support any other platforms at this time. It uses the built-in USB controller available on the ING916 to implement both host and device operations.
