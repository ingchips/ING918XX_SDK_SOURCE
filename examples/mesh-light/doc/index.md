# Mesh Light

This example creates a SIG Mesh light.

## Hardware Setup

This is example uses 1 GPIO connected to a key for clearing flash information,
and 1 GPIO connected to an LED to demonstrate a light. These options are
used which can be customized by preprocessor symbols.

|  Name             |   Default   | Note                            |
|:-----------:|:-----------:|:---------------------------|
|PIN_LED            | GIO_GPIO_17 | PIN connecting to an LED        |
|LED_LEVEL_ON       | 1           | PIN level to turn the LED on    |
|LED_LEVEL_OFF      | 0           | PIN level to turn the LED off   |
|KB_KEY_RESET_PARAMS| 5           | PIN connecting to a key         |

Note for ING918XX Dev-Board: this uses `0` to turn on the LED on.

<img src="../../llsync_light/doc/img/hardware.png" width="50%" />

## Test

This example can be tested with following apps:

* ING Mesh (WeChat mini program)

## Software Setup

### Storage setup

* By default, this example stores the following information in flash:
  * __Device name__
  * __MAC address__ (gatt_adv_addr and non-connect_adv_addr)
  * __Adv UUID__
  * __Provisioning Information__

* Press the __KEY__ connected to `KB_KEY_RESET_PARAMS` and then press __KEY-RESET__ to clear flash information of
__Provisioning Information__, keep the others.

* If you don't want to store any information in flash, you can disable the corresponding flash storage function by masking the following macro definitions:
  * __MESH_UUID_USE_FLASH__ : For Adv UUID
  * __MESH_NAME_USE_FLASH__ : For Device name
  * __MESH_GATT_ADV_ADDR_USE_FLASH__ : For gatt_adv_addr
  * __MESH_BEACON_ADV_ADDR_USE_FLASH__ : For non-connect_adv_addr
    * After you mask above macro definitions, the uuid, the name and the address will always use the default value.
  * __MESH_STACK_TLV_USE_FLASH__ : For Provisioning Information
    * After you mask above macro definition, The provisioning information will not be stored in flash each time the provisioning is complete, so, if a chip reset occurs, the provisioning information will be lost.