# BLE RPC Client

[中文版](index_cn.md)

This is the RPC _Client_ for the [_Server_](../../../examples/ble_rpc/doc/index.md).
The _Client_ is a program runs natively on a host computer, such as a PC, in which,

* Most of BLE APIs are available: the program looks like an app running on BLE SoC;
* All APIs of the host OS are available: the program is native.

Developers can use this client to:

* Make prototypes;
* Carry out automated tests;
* ...

This _Client_ has been ported to Windows and Linux.

_lib_ folder contains the supporting library to create a client, and _app_ folder
provides several examples. Developers can create their own client apps.

## Setup

### Development Tools

1. Windows

    * 32bit GNU GCC for Windows

        For example, download **i686** binaries from [here](https://github.com/skeeto/w64devkit/releases/)
        based on [MinGW-w64](https://www.mingw-w64.org/).

    * [GNU Make](https://www.gnu.org/software/make/)

        _make.exe_ can be found in SDK _wizard_ folder. Put it in PATH or copy it
        into _rpc_client_.

    * [Terminal](https://learn.microsoft.com/en-us/windows/terminal/install)

        This program uses ANSI color escape sequences. The traditional console does not support it.

1. Linux

    * Install GCC

    * Ensure 32bit app is supported (build and run)

        For 64bit Debian-based distributions, use following commands to enable support of
        32bit apps:

        ```shell
        $ sudo dpkg --add-architecture i386
        $ sudo apt-get update
        $ sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386
        $ sudo apt-get install gcc-multilib g++-multilib
        ```

    * Enlarge message maximum number of _mqueue_

        The default number (10) is too small for the client. Use below command
        to enlarge it on demand (or modify _init_ script to enlarge it permanently):

        ```shell
        sudo sysctl fs.mqueue.msg_max=100
        ```

1. Windows WSL

    WSL2 is also supported, while WSL1 would not work because it does not and
    will never support 32bit programs.

Entering _rpc_client_ directory, run `make rebuild app=shell`. If everything is OK,
`rpc.exe` is generated no matter which OS is used.

### Bridge UART to TCP

This _Client_ connects to _Server_ through a TCP socket. Therefore, it is needed to create
a TCP server waiting for the _Client_, and exchanging data between the _Client_ (TCP) and
_Server_ (UART).

1. For Linux

Use [_socat_](http://www.dest-unreach.org/socat/) to bridge TCP and UART.

1. For both Windows and Linux

Use the example Python script [_tcp_serial_redirect.py_](https://github.com/pyserial/pyserial/blob/master/examples/tcp_serial_redirect.py)
provided by [_pyserial_](https://pypi.org/project/pyserial), take COM3 as an example:

```
python tcp_serial_redirect.py COM3 115200 -P 8888
```

## Test

Several examples are provided. Download _BLE RPC_ to a Dev-Board, and start UART to TCP daemon.
If the Dev-Board is connected to the PC running _Client_, `rpc.exe` can be launched
without any parameters. If the Dev-Board is connected to another PC, assuming its IPv4
is AA:BB:CC:DD, then use the command line `rpc.exe -addr AA:BB:CC:DD` to launch it.

### _shell_

This example exposes a simple shell of the host over BLE. To build it:

```shell
make rebuild app=shell
```

Run _rpc.exe_. Then use _ING BLE_ to connect to the device named _Shell_, which includes
the _INGChips Console Service_. Type some commands and check the results:

<img src="./img/shell.png" width="250px" />

### _conn_test_

This example performs a batch of connection tests to a specific device,
and generates reports. To build it:

```shell
make rebuild app=conn_test
```

Download _Thermometer with FOTA_ to another Dev-Board. Run _rpc.exe_, and check
the test result:

<img src="./img/test_session.png" width="350px" />
