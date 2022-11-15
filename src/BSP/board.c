#include <stdio.h>
#include "ingsoc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"

#include "peripheral_pwm.h"
#include "peripheral_sysctrl.h"
#include "peripheral_pinctrl.h"

#if(BOARD_ID == BOARD_ING91881B_02_02_05)

#elif (BOARD_ID == BOARD_ING91881B_02_02_05)

#endif

//-------------------------------------------------RGB_LED驱动整理-------------------------------------------------
#ifndef PWM_LED

#include "rgb_led.c"

void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t cmd = (0x3a << 24) | (b << 16) | (r << 8) | g;
#if(BOARD_ID == BOARD_ING91881B_02_02_05)
    tlc59731_write(cmd);
#elif(BOARD_ID == BOARD_ING91881B_02_02_06)
    ws2881_write(cmd);
#endif
}

void setup_rgb_led()
{
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_GPIO0) | (1 << SYSCTRL_ClkGate_APB_GPIO1));
    PINCTRL_SetPadMux(PIN_RGB_LED, IO_SOURCE_GPIO);

    GIO_SetDirection(PIN_RGB_LED, GIO_DIR_OUTPUT);
    GIO_WriteValue(PIN_RGB_LED, 0);

    set_rgb_led_color(50, 50, 50);
}

#else

#define PIN_RED     4
#define PIN_GREEN   0
#define PIN_BLUE    6

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

#define PERA_THRESHOLD (OSC_CLK_FREQ / 1000)

void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b)
{
#define TO_PERCENT(v) (((uint32_t)(v) * 100) >> 8)

    PWM_SetHighThreshold(PIN_RED   >> 1, 0, PERA_THRESHOLD / 100 * TO_PERCENT(r));
    PWM_SetHighThreshold(PIN_GREEN >> 1, 0, PERA_THRESHOLD / 100 * TO_PERCENT(g >> 1));  // GREEN & BLUE led seems too bright
    PWM_SetHighThreshold(PIN_BLUE  >> 1, 0, PERA_THRESHOLD / 100 * TO_PERCENT(b >> 1));
}

static void setup_channel(uint8_t channel_index)
{
    PWM_HaltCtrlEnable(channel_index, 1);
    PWM_Enable(channel_index, 0);
    PWM_SetPeraThreshold(channel_index, PERA_THRESHOLD);
    PWM_SetMultiDutyCycleCtrl(channel_index, 0);        // do not use multi duty cycles
    PWM_SetHighThreshold(channel_index, 0, PERA_THRESHOLD / 2);
    PWM_SetMode(channel_index, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
    PWM_SetMask(channel_index, 0, 0);
    PWM_Enable(channel_index, 1); 
    PWM_HaltCtrlEnable(channel_index, 0);
}

void setup_rgb_led()
{
    SYSCTRL_ClearClkGateMulti(  (1 << SYSCTRL_ClkGate_APB_GPIO)
                              | (1 << SYSCTRL_ClkGate_APB_PWM));
    PINCTRL_SetPadMux(PIN_RED, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_RED, 1);
    PINCTRL_SetPadMux(PIN_GREEN, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_GREEN, 1);
    PINCTRL_SetPadMux(PIN_BLUE, IO_SOURCE_GENERAL);
    PINCTRL_SetPadPwmSel(PIN_BLUE, 1);

    setup_channel(PIN_RED   >> 1);
    setup_channel(PIN_GREEN >> 1);
    setup_channel(PIN_BLUE  >> 1);
    
    set_led_color(50, 50, 50);
}

#else

#error unknown or unsupported chip family

#endif

#endif


//-------------------------------------------------温度计驱动整理-------------------------------------------------

#if(BOARD_ID == BOARD_ING91881B_02_02_05)
    #include "bme280.h"

    uint8_t dev_addr = BME280_ADDR;
    struct bme280_dev bme280_data =
    {
        .intf_ptr = &dev_addr,
        .intf = BME280_I2C_INTF,
        .read = user_i2c_read,
        .write = user_i2c_write,
        .delay_us = user_delay_us,
        /* Recommended mode of operation: Indoor navigation */
        .settings =
        {
            .osr_h = BME280_OVERSAMPLING_1X,
            .osr_p = BME280_OVERSAMPLING_16X,
            .osr_t = BME280_OVERSAMPLING_2X,
            .filter = BME280_FILTER_COEFF_16,
            .standby_time = BME280_STANDBY_TIME_62_5_MS,
        },
    };
    struct bme280_data comp_data;
#elif(BOARD_ID == BOARD_ING91881B_02_02_06)
#error unknown or unsupported chip family.
#endif

void setup_env_sensor()
{
#if(BOARD_ID == BOARD_ING91881B_02_02_05)
    printf("sensor init...");
    if (bme280_init(&bme280_data) != BME280_OK)
        printf("failed\n");
    else
    {
        printf("OK\n");
        bme280_set_sensor_settings(BME280_ALL_SETTINGS_SEL, &bme280_data);
        bme280_set_sensor_mode(BME280_NORMAL_MODE, &bme280_data);
    }
#elif(BOARD_ID == BOARD_ING91881B_02_02_05)
#endif
}

double get_temperature()
{
#if (BOARD_ID == BOARD_ING91881B_02_02_05)
    if(bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data) < 0)
        return -1;
    return comp_data.temperature;

#elif (BOARD_ID == BOARD_ING91881B_02_02_06)
    
#include "peripheral_sysctrl.h"

#define I2C_PORT        I2C_PORT_0
#define MTS01B_OK           0
#define MTS01B_E_COMM_FAIL  1

    uint8_t cmd[2]={(uint8_t)(CONVERT_T >>8), (uint8_t)(CONVERT_T & 0xFF)};
    uint8_t reg_data[3] = {0}; 
    //uint8_t sta = MTS01B_E_COMM_FAIL;
    //读取温度数据
    uint8_t sta = i2c_read(I2C_PORT, 0x44, cmd, 2, reg_data, sizeof(reg_data));
    if(sta == MTS01B_E_COMM_FAIL){
        SYSCTRL_ResetBlock(SYSCTRL_Reset_APB_I2C0);
        SYSCTRL_ReleaseBlock(SYSCTRL_Reset_APB_I2C0);
        i2c_init(I2C_PORT);
        platform_printf("i2c rst \n");
    }else {
        return (double)(40 + ((reg_data[0] & 0x7F) >> 7) * (-1) * (~((reg_data[0] & 0x7F) - 1)));
    }
#endif
}

double get_humidity()
{
#if (BOARD_ID == BOARD_ING91881B_02_02_05)
    if(bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data) < 0)
        return -1;
    return comp_data.humidity;

#elif (BOARD_ID == BOARD_ING91881B_02_02_06)

#error unknown or unsupported chip family.

#endif
}

double get_pressure()
{
#if(BOARD_ID == BOARD_ING91881B_02_02_05)
    if(bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data) < 0)
        return -1;
    return comp_data.humidity;

#elif (BOARD_ID == BOARD_ING91881B_02_02_06)

#error unknown or unsupported chip family.

#endif
}


//-------------------------------------------------加速度计驱动整理-------------------------------------------------
//加速度计无需修改，直接使用即可
#ifdef ACCELEROMETER
#include "bma2x2.h"

static struct bma2x2_accel_data sample_xyz = {0};

extern s32 bma2x2_power_on(void);

void setup_accelerometer(void)
{
#ifndef SIMULATION
    printf("bma2x2_power_on...");
    if (bma2x2_power_on()==0)
        printf("success!!\n");
    else
        printf("faild!!\n");
#endif
}

void get_acc_xyz(float *x, float *y, float *z)
{
    bma2x2_read_accel_xyz(&sample_xyz);
    *x = sample_xyz.x;
    *y = sample_xyz.y;
    *z = sample_xyz.z;
}
#endif


//-------------------------------------------------蜂鸣器驱动整理-------------------------------------------------
#ifndef PIN_BUZZER
#define PIN_BUZZER 8
#endif

void setup_buzzer(void)
{
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_PWM));
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PINCTRL_SetGeneralPadMode(PIN_BUZZER, IO_MODE_PWM, 4, 0);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    PINCTRL_SetPadMux(PIN_BUZZER, IO_SOURCE_PWM6_B);
#else
    #error unknown or unsupported chip family
#endif
}

void set_buzzer_freq0(uint8_t channel_index, uint16_t freq)
{
    uint32_t pera = PWM_CLOCK_FREQ / freq;
    PWM_HaltCtrlEnable(channel_index, 1);
    PWM_Enable(channel_index, 0);
    if (freq > 0)
    {
        PWM_SetPeraThreshold(channel_index, pera);
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
        PWM_SetMultiDutyCycleCtrl(channel_index, 0);        // do not use multi duty cycles
#endif
        PWM_SetHighThreshold(channel_index, 0, pera >> 1);
        PWM_SetMode(channel_index, PWM_WORK_MODE_UP_WITHOUT_DIED_ZONE);
        PWM_SetMask(channel_index, 0, 0);
        PWM_Enable(channel_index, 1);
        PWM_HaltCtrlEnable(channel_index, 0);
    }
}

void set_buzzer_freq(uint16_t freq)
{
    set_buzzer_freq0(PIN_BUZZER >> 1, freq);
}
