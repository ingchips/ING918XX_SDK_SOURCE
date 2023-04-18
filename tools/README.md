# SDK Tools

## Flash Downloader

_icsdw.py_ is the flash downloader supporting both ING918xx and ING916xx chip
families.

* Usage:

    ```shell
    python icsdw.py /path/to/flash_download.ini
    ```

use 'pip install -r requirements.txt' to install all dependency packages

### usage for UART Flash download

-	type 'icsdw.py' to see the help message for command line format.
-	check config options in 'flash_download.ini':
	-	'family': ing916, ing918, etc.
	-	'Checked': according FileName and Address need to be provided, Address will be used to differentiate Flash or RAM, maximum of 6 bins are allowed.

Note: Python 3 and _pyserial_ is required to use this script.

### usage for USB Flash download

- 	use icsdw.device.query_all_active_usb_ports(timeout) to query all usb port, it will wait and hold&return all ports until timeout(seconds).
	-	call functin: example: python -c "import icsdw;x=icsdw.device.query_all_active_usb_ports(10);".
	-	enter boot.
	-	record all returned ports, if USB is re-connected after last query, the port is not valid anymore.
- 	use port as input to download:
    ```shell
    python icsdw.py /path/to/flash_download.ini --port USB#VID_FFFF#PID_FA2F#25#01#02
    ```
- 	if no port provided, it will check and use the first usb port, in this case, you have to enter boot first before call script.

Note: Python 3 and _pyusb_ is required to use this script.

## RAM Downloader

-	same procedure as Flash download,  but need to modify Address to RAM address(the first 0x1500 bytes are reserved, application should use address larger than 0x1500).

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