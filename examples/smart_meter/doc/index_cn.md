# Smart Meter

[English](index.md)

_Smart Meter_ 作为中心设备，可以连接最多四个外围设备（四个 _Thermometer with FOTA_ 温度计）；作为外围设备，
可与手机连接并将收集到的温度数据传输到手机。此外，_Smart Meter_ 通过 UART 口接受上位机的控制，可以设置各温度计的地址、控制程序的启动等。

<img src="./img/smart_meter.png" width="50%" />

_Smart Meter_ 与手机的连接使用了一个自定义的 GATT 服务，通过这个服务在手机和蓝牙设备之间建立了
一个基于字符串的输入、输出通道。_Smart Meter_ 使用输出通道传输温度数据。有关这个自定义服务的使用
方法可参考 [_Peripheral Console_](../../peripheral_console/doc/index.md) 示例。

## 详细开发步骤

### 1. 使用项目向导创建一个 Peripheral 应用

使用向导工具设置广播数据和 GATT Profile。在 GATT Profile 里添加自定义的 GATT 服务。

<img src="./img/gatt.png" width="50%" />

### 2. 定义从设备（即温度计）

温度计设备用设备地址和 `id` 作为标识。每个设备占用一条独立的链路（`conn_handle`）。

```C
typedef struct slave_info
{
    uint8_t     id;
    bd_addr_t   addr;
    uint16_t    conn_handle;
    gatt_client_service_t                   service_thermo;
    gatt_client_characteristic_t            temp_char;
    gatt_client_characteristic_descriptor_t temp_desc;
    gatt_client_notification_t              temp_notify;
} slave_info_t;
```

### 3. 扫描并连接到温度计

调用两个 GAP 函数开始扫描，一旦扫描到待连接的温度计，就停止扫描并调用
`gap_ext_create_connection` 发起连接。连接建立成功后，检查是否还有待连接的温度计，如果有，再次开始扫描。

### 4. 发现服务

这部分功能需要调用一系列 `gatt_client` 模块的 API。这部分 API 的整体逻辑跟 Android、iOS 蓝牙 API 基本类似。

### 5. 数据传输

订阅温度数据，将温度转换成字符串，通过 UART 上传给上位机。如果已经连接到了中心设备（ING BLE app），
则将数据通过自定义的 GATT 服务传输到中心设备。

### 6. 加入容错性

* 如果检查到与某温度计的连接断开，则再次开始扫描。

* 如果与中心设备的连接断开，则再次开始广播。

## 测试方法

### 1. 准备蓝牙温度计

Downloader 内置 Pascal 脚本引擎，开始下载每个 bin 文件时会调用用户编写的脚本。通过脚本我们可以任意修改 bin 的内容。
将蓝牙地址写在一个 bin 文件内，然后添加到 Download 内（假设是第 6 个 bin 文件），下面的脚本会按照下载次数修改蓝牙地址的第一个字节：

```pascal
procedure OnStartBin(const BatchCounter, BinIndex: Integer;
                     var Data: TBytes; var Abort: Boolean);
begin
  if BinIndex <> 6 then Exit;
  Data[0] := BatchCounter;
end;
```

或者使用 Python 版本下载工具，并用 [Python 脚本](https://ingchips.github.io/user_guide_cn/core-tools.html#python-%E7%89%88%E6%9C%AC)实现上述功能。

假设这个 bin 文件的烧录地址为 `X`，修改 _Thermometer with FOTA_，使其从该地址获取地址：

```c
gap_set_adv_set_random_addr(0, (uint8_t *)X);
```

将修改后的 _Thermometer with FOTA_ 下载到 4 块开发板。

### 2. 测试

将 _Smart Meter_ 下载到另一块开发板，在上位机上通过串口工具输入 `start` 命令启动 _Smart Meter_ ，
用手机连接到名为 ING Smart Meter 的设备，就可以看到四个温度计上报的温度了。随机重启几个温度计，
可以看到 _Smart Meter_ 能够自动重新建立连接。