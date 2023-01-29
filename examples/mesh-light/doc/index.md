# Mesh Light

This example creates a SIG Mesh light.

## Hardware Setup

* ING918XX Dev-Board

Jumpers marked in below figure should be connected:

<img src="../../llsync_light/doc/img/hardware.png" width="50%" />

## Test

This example can be tested with following apps:

* ING Mesh (WeChat mini program) - recommended
* ING Mesh (iOS App) - recommended
* nRF Mesh (iOS/Android)

## Software Setup

### BOARD_ID setup
* At present, Ingchips development board supports two types of LED driver ICs(WS2811 or 59731), and may support more types in the future, as shown in the figure below: 

<img src="./img/LED_DRV_IC.png" width="70%" />

* Different LED driver IC require different software configuration. Therefore, in order to make LED work normally, you need to switch the software macro definition __BOARD_ID__ according to the silkscreen of the IC on your board: 
  * __WS2811__ : BOARD_ING91881B_02_02_06
  * __59731__  : BOARD_ING91881B_02_02_05
  * __others__ : Please contact Ingchips-FAE.

### Storage setup

* By default, this example stores the following information in flash:
  * __Device name__
  * __MAC address__ (gatt_adv_addr and non-connect_adv_addr)
  * __Adv UUID__
  * __Provisioning Information__
* Press __KEY-2__ and then press __KEY-RESET__ to clear all flash information.

* If you don't want to store any information in flash, you can disable the corresponding flash storage function by masking the following macro definitions:
  * __MESH_UUID_USE_FLASH__ : For Adv UUID
  * __MESH_NAME_USE_FLASH__ : For Device name
  * __MESH_GATT_ADV_ADDR_USE_FLASH__ : For gatt_adv_addr
  * __MESH_BEACON_ADV_ADDR_USE_FLASH__ : For non-connect_adv_addr
    * After you mask above macro definitions, the uuid, the name and the address will always use the default value.
  * __MESH_STACK_TLV_USE_FLASH__ : For Provisioning Information
    * After you mask above macro definition, The provisioning information will not be stored in flash each time the provisioning is complete, so, if a chip reset occurs, the provisioning information will be lost.