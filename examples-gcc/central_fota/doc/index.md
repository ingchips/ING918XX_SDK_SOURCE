# Central FOTA

This example update a peripheral over the air. This example shares the same design with
[_ING BLE_](https://github.com/ingchips/INGdemo/) and
[_FOTA Web Tool_](https://ingchips.github.io/web_apps/ing918_ota/index.html).

The core FOTA procedure is implemented by module [`fota_client.zig`](../src/fota_client.zig)
which is written in [Zig](https://ziglang.org) programming language due to its simplicity in developing
`async` programs:

```rust
pub fn update(...) {
    if (!self.enable_fota()) return -2;
    if (!self.burn_items(bitmap, item_cnt, items)) return -3;
    if (!self.burn_meta(bitmap, item_cnt, items, entry)) return -4;
    if (!self.reboot()) return -5;
    return 0;
}
```

This example supports both secure and unsecure FOTA.

## Data Storage

Data storage of both local and remote are defined in `ota_items` in [`profile.c`](../src/profile.c).
This example can be built for different chip families (`INGCHIPS_FAMILY`), and the device to be updated
is of another family which is defined by compiler switch `TARGET_FAMILY`. There are three addresses
related to each `ota_item_t`:

1. `local_storage_addr`: Where the item (binary) is store in **this** device;
1. `target_storage_addr`: Where the item (binary) is going to be stored in the peripheral device, i.e.
    the location to hold the new version;
1. `target_load_addr`: The actual address of the item (binary) in the peripheral device, i.e. the
    location to be replace by the new version after updating.

Since all data are stored in the embedded flash, an ING918xx device may not be able to hold the whole
firmware for an ING916xx device. [_Data Logger_](../../../examples/data_logger/doc/index.md) demonstrate
how to use an external mass storage device.

## Test

[Get familiar](https://ingchips.github.io/blog/fota_demo_zh/) with the whole FOTA procedure,
and test it using [_ING BLE_](https://github.com/ingchips/INGdemo/) and
[_FOTA Web Tool_](https://ingchips.github.io/web_apps/ing918_ota/index.html). After this,

1. Download `Thermometer with FOTA` to one Dev-Board (Board-A) and run;
1. Rebuild `Thermometer with FOTA` with `V2`;
1. Make sure `TARGET_FAMILY` is set to chip family of Board-A, rebuild & download
    to another Dev-Board (Board-B), and run;
1. Output of Board-B will be somethings like

    ```
    connecting...
    discovering...

    === UNSECURE FOTA ===
    device version:
    platform: 1.7.12
        app: 1.1.0
    update bitmap: 02

    start new item: 1
    burn_page: 00066000, 00066000, 8192
    burn_page: 00068000, 00068000, 8192

    fota_done: 0
    disconnected
    ```
1. Connect to Board-A and check if its app has been updated, or
1. Reset Board-B to try to update it once more, and output of Board-B will be somethings like

    ```
    connecting...
    discovering...

    === UNSECURE FOTA ===
    device version:
    platform: 1.7.12
        app: 1.2.0
    fota_done: 1
    ```