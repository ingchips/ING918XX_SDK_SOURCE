# Central CTE

This example receives CTE AoA in connection mode or proprietary conn-less mode, then forwards IQ
through UART and/or GATT.

Check out _Application Note - Direction Finding Solution_ for more information on AoA algorithm, etc.

To ease app downloading/updating, this app also includes [_INGCHIPS FOTA Sevice_](../../thermo_ota/doc/index.md#other-resources).

## Work Modes

Use compiler switch `PRO_MODE` to switch between connection mode and proprietary conn-less mode:

* **When `PRO_MODE` not defined**: Standard connection mode is selected;
* **When `PRO_MODE` is defined**: proprietary conn-less mode is selected.

Proprietary conn-less mode is SILabs-compatible (with limitation).

## Data Output

This example also acts as a peripheral. Once connected, it will forward IQ data through GATT. When not
connected, data is forwarded to UART.

Data is Base64 encoding of `le_meta_conn_iq_report_t` for connection mode, and `le_meta_pro_connless_iq_report_t`
for proprietary conn-less mode.

## Switching Patterns

Up to `PAT_NUMBER` switching patterns can be configured in this example. A different pattern is used for each
AoA reception.

Note:

   * For the 4x4 Antenna Array Board, only one switching pattern is enough for precious direction calculation;
   * Modify `CURRENT_ARRAY` in [`profile.c`](../src/profile.c) to select a sub array for evaluation, such as `#define CURRENT_ARRAY ANTENNA_ARRAY_1x4`.

## Configure over BLE

Once connected over BLE, this example accepts several commands, see [`service_console.c`](../src/service_console.c) for details.
