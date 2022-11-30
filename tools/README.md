# SDK Tools

## Flash Downloader

_icsdw.py_ is the flash downloader supporting both ING918xx and ING916xx chip
families.

* Usage:

    ```shell
    python icsdw.py /path/to/flash_download.ini
    ```

Note: Python 3 and _pyserial_ is required to use this script.

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