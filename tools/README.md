# SDK Tools

## Flash Downloader

_icsdw.py_ is the flash downloader supporting both ING918xx and ING916xx chip
families.

* Usage:

    ```shell
    python icsdw.py /path/to/flash_download.ini
    ```

This script supports COM (UART), USB and SWD (through J-Link).

> [!NOTE]
> Downloading using ING-DAPLink is not supported yet.

Use 'pip install -r requirements.txt' to install all dependency packages

### Extra Command Line Options

Type `python icsdw.py` to see the help message for command line format.

|Option                 | Explanation                            |
|:----------------------|:-----------------------------------------|
|--go                   |Skip handshaking.                      |
|--user_data STRING     |Pass `STRING` to user defined `on_start_bin2`. |
|--port PORT            |Use `PORT` to download.                |
|--batch                |Enable batch operation.                |
|--counter  NNN         |Set batch counter to `NNN`.            |
|--timeout  TTT         |Set time out to `TTT` in seconds.      |

Note: `--port`, `--batch`, `--counter` and `--timeout` override the corresponding
settings in the project file (`flash_download.ini`).

When a `.bin` file is given, _icsdw.py_ will download the file to a specified location
through J-Link. The `.bin` file can either be a file on local system, or on the Web
(`http` or `https` URL).

|Option                 | Explanation                              |
|:----------------------|:-----------------------------------------|
|--port PORT            |Use `PORT` to download.                   |
|--addr AAA             |Downloading to this address.                 |
|--family FFF           |Specify chip family (such as ing918, ing916).|
|--loop                 |Enable loop mode.                            |


### Note for USB Flash Download

- Use `python icsdw.py list-usb` to query all usb port. It will print the full name of
  all available USB devices.

- If `port` is set to `USB`, then the 1st found USB device is chosen. Use the full
  name to specify it explicitly, for example:

    ```shell
    python icsdw.py /path/to/flash_download.ini --port USB#VID_FFFF#PID_FA2F#25#01#02
    ```

### Note for J-Link Download

- Use `python icsdw.py list-jlink` to query all J-Link probes. It will print the full information of
  all available J-Link probes.

- If `port` is set to `JLINK`, then the 1st found J-Link probe is chosen. Use `JLINK#serail_no`
  to specify it explicitly when there are multiple probes, for example:

    ```shell
    python icsdw.py /path/to/flash_download.ini --port JLINK#123
    ```

    where `123` is the serial number of the selected probe.

### Note for Single Bin Download

When the 1st parameter ends with `.bin`, single binary file downloading mode is selected.
In this case, chip family, address must be specified from command line.
The file path can be a local one or an URL.

Example:

```shell
python icsdw.py /path/to/bin/file --family ing916 --addr 0x02002000 --port JLINK
```

## Flash Dumper

_flash_dump.py_ is flash dumping tool.

Note: Python 3 and _pyserial_ is required to use this script.

## File Generator

_gen_files.nim_ generates following files for other tools:

1. `.hex` files for downloading tools
1. `symdef` files for various linkers (Keil, IAR, GNU Arm Toolchain and SEGGER)

Usage:

```shell
nim -d:release c gen_files.nim
gen_files /path/to/sdk/bundles
```

Note: [Nim](https://nim-lang.org/) is required to build this tool.

## RTT Logger

_rtt_logger.py_ is based on [pyocd](https://pyocd.io/), and supports J-Link,
DAP-Link, etc.

Usage:

```shell
python rtt_logger.py -RTTSearchRanges "0x2000XXXX 0xYYYY" log_file
```

### Command line options

|Option  | Explanation                              |
|:--------------------|:-----------------------------------------|
|-Speed `SpeedInKHZ`  |Sets speed in kHz              |
|-ID `id`             | Connects to the probe with unique `id`   |
|-RTTAddress `RTTAddress` | Sets RTT address to `RTTAddress`     |
|-RTTSearchRanges "`Ranges`" |Sets RTT search ranges to `Ranges` |
|-RTTChannel `RTTChannel` | Sets RTT channel to `RTTChannel`     |

`Ranges` is specified as `start` address and `size`, for example
"0x2000XXXX 0xYYYY" tells this tool to search for RTT in the memory range
from `0x2000XXXX` to `(0x2000XXXX + 0xYYYY)`.

### Add Jlink and PyOCD support

Use the ingchips_packs_addon.py script to install Jlink and PyOCD support. You can use the -h parameter to get help on how to use it.

```shell
python ingchips_packs_addon.py -h
```

Example:
Jlink versions prior to V7.62 add device support using the following command:

```shell
python ingchips_packs_addon.py -t jlink_v6 -tp "/path/to/sdk" -p "/path/to/jlink_path"
```

Versions of Jlink after V7.62 use the following command to add device support:

```shell
python ingchips_packs_addon.py -t jlink_v7 -tp "/path/to/sdk"
```

You must run `pycod pack update` before pyocd can install the pack.
To install PyOCD pack, use the following command:

```shell
python ingchips_packs_addon.py -t pyocd -tp "/path/to/sdk"
```
