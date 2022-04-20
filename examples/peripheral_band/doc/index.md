# Peripheral Band

This example demonstrate a wearable band, which includes an SPI OLED display and a heart rate sensor.

## Hardware Setup

To test this example, an 128x32 SPI OLED, a BH1790 sensor are needed. Check [`spi.h`](../src/spi.h)
and [`main.c`](../src/main.c) for hardware setup.

## Design Details

* BH1790 sensor is sampled @ `SAMPLING_CNT_32HZ`, and heart rate calculation can be found in
    [`heartRate.c`](../src/bh1790/heartRate.c);

* _Heart Rate Service_ and _Running Speed and Candence Service_ are defined in GATT profile;

    * Heart rate is reported through _Heart Rate Service_

    * Randomly generated values are reported through _Running Speed and Candence Service_

    * See [_Peripheral Pedometer_](../../peripheral_pedometer/doc/index.md) for reporting real values
      through _Running Speed and Candence Service_
