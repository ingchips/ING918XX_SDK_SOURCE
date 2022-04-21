# Data Logger

[中文版](index_cn.md)

This app is inspired by [OpenLog](https://github.com/sparkfun/OpenLog).
It can read configuration and save data from serial or BLE to SD/TF cards.

The configuration is stored in a file named **CONFIG.TXT** in the card. If this file does not exist,
when this app starts, it will create one containing default values for each key. In this file,
each line contains a key-value pair: `key=value`.

[FatFs](http://elm-chan.org/fsw/ff/00index_e.html) is used for file I/O. The SD card must be using
FAT or exFAT Filesystem. Log files are named as **XXXXX.TXT**, where `XXXXX` is a number increased each new log is created.

## Hardware Setup

See [profile.c](../src/profile.h) for default PIN definition. To run this app, an SD/TF module
with SPI interface, an SD or TF card, and a 3.3v power supply are needed.

When using an ING918xx Dev-Board, the default setup looks like this:

![](./img/use_918board.png)

## Configuration

|Key                | Note                          | Allowed Values                |
---                 | ---                           | ---                           |
|mode               | Work mode (data source)       | ble \| uart                   |
|use_timestamp      | If add timestamp in log or not| 0 \| 1                        |
|uart_baud          | UART baud rate                | integer (115200, 9600, etc)   |
|uart_parity        | UART parity                   | none \| odd \| even           |
|uart_stop_bits     | UART stop bits number         | 1 \| 2                        |
|uart_data_bits     | UART data bits number         | 5 \| 6 \| 7 \| 8              |
|uart_data_fmt      | recording format of UART data.| raw \| str                    |
|ble_data_fmt       | recording format of BLE data  | raw \| hex                    |
|ble_phy            | BLE PHY for connection        | coded \| 1M \| 2M             |
|ble_addr_type      | BLE device address type       | public \| random              |
|ble_addr           | BLE devide address            | 16 bytes. any.                |
|ble_service_uuid   | BLE service UUID              | 16 bytes. any.                |
|ble_char_uuid      | BLE characteristic UUID       | 16 bytes. any.                |

Note:

* For UART mode, timestamp is only available when `uart_data_fmt` is set to `str`.

For BLE mode, there are also keys for configure BLE connection.

|Key                 | Note                           |
---                  | ---                            |
| scan_int           | scan interval                  |
| scan_win           | scan window                    |
| interval_min       | minimum connection interval    |
| interval_max       | maximum connection interval    |
| latency            | slave latency                  |
| supervision_timeout| supervision timeout            |
| min_ce_len         | minimum connection event length|
| max_ce_len         | maximum connection event length|

## Test

Insert an empty card, download the app and check `printf` output from the default UART to see if the
filesystem is successfully mounted. Move the card to a card reader and check if **CONFIG.TXT** is created.
If everything is OK, the data logger is ready.

### Record serial data

Get another Dev-Board (Board-B), use the Wizard to create a new app. Make it to continuously print strings to UART.
For example:

```c
uint32_t setup_profile(void *data, void *user_data)
{
   int i = 0;
   for (;;)
   {
      printf("message #%d\n", i++);
      vTaskDelay(100);
   }
}
```

Make sure the relevant configurations in **CONFIG.TXT** are:

```
mode=uart
use_timestamp=1
uart_baud=115200
uart_parity=none
uart_stop_bits=1
uart_data_bits=8
uart_data_fmt=str
```

Connect **UART Rx Probe** to UART Tx of Board-B, power on both Dev-Boards. Wait a while (1 minute or longer),
move the card to a card read, and check if there is a new **XXXXX.TXT** file containing strings from Board-B:

```
[00:00:05.936,004]message #406
[00:00:06.137,664]message #407
[00:00:06.239,135]message #408
```

### Record temperature measurements

Get another Dev-Board (Board-B), download the `Thermometer with FOTA` example.

Modify relevant configurations in **CONFIG.TXT** as below:

```
mode=ble
use_timestamp=1
ble_data_fmt=hex
ble_phy=2M
ble_addr_type=random
ble_addr=CDA32811893e
ble_service_uuid=00001809-0000-1000-8000-00805F9B34FB
ble_char_uuid=00002A1C-0000-1000-8000-00805F9B34FB
```

Power on both Dev-Boards. Wait a while (1 minute or longer), move the card to a card read, and check
if there is a new **XXXXX.TXT** file containing temperature measurements reported by Board-B, where
each report is format as a hexadecimal table:

```
[00:00:02.470,153]
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
===============================================
00 D1 09 00 FE                                   | .....
```
