# BQB RF Test App

This is a customizable BQB RF test app. It parses HCI commands from UART, calls GAP APIs, and sends
results to UART. This app uses the default UART for HCI transmission, supports BLE test mode, and CW test.

Recent releases of BLE Core Spec added new variants of test commands. A simple summary:

* **HCI_LE_Receiver_Test_v2**: Support different PHYs and modulation indices;
* **HCI_LE_Receiver_Test_v3**: CTE;
* **HCI_LE_Transmitter_Test_v2**: Support different PHYs;
* **HCI_LE_Transmitter_Test_v3**: CTE;
* **HCI_LE_Transmitter_Test_v4**: Change Tx power.

## Customization

1. UART configuration

    Update `config_uart` to configure UART.

1. RF Tx power boosting

    Add `rf_enable_powerboost()` to `app_main()`.

## Testing

1. BLE Tester

    Connect HCI-UART to testers and do the test as usual.

1. Self Test

    Download this example to two dev-boards, and use two RF Test Tool to control two boards: let one
    as transmitter and the other as receiver.