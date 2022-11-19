#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"

#include "peripheral_pwm.h"
#include "peripheral_sysctrl.h"
#include "peripheral_pinctrl.h"

#include "platform_api.h"

//-------------------------------------------------RGB_LED驱动整理-------------------------------------------------
#ifdef BOARD_SENSOR_RGBLED

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

//#include "peripheral_pwm.h"

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

#endif


//-------------------------------------------------温度计驱动整理-------------------------------------------------
#ifdef BOARD_SENSOR_THERMO

#ifndef I2C_PORT
#define I2C_PORT        I2C_PORT_0
#endif
#include "iic.h"

#if(BOARD_ID == BOARD_ING91881B_02_02_05)

#include "bme280.h"

#define BME280_ADDR     BME280_I2C_ADDR_PRIM

BME280_INTF_RET_TYPE user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    return i2c_read(I2C_PORT, BME280_ADDR, &reg_addr, 1, reg_data, len) == 0 ? BME280_OK : BME280_E_COMM_FAIL;
}

BME280_INTF_RET_TYPE user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len,
                                                    void *intf_ptr)
{
    uint8_t data[len + 1];
    data[0] = reg_addr;
    memcpy(data + 1, reg_data, len);
    return i2c_write(I2C_PORT, BME280_ADDR, data, sizeof(data)) == 0 ? BME280_OK : BME280_E_COMM_FAIL;
}

void user_delay_us(uint32_t period, void *intf_ptr)
{
    uint32_t ms = (period + 999) / 1000;
    vTaskDelay(pdMS_TO_TICKS(ms));
}

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
//#error unknown or unsupported chip family.
#endif

void setup_env_sensor()
{
#if(BOARD_ID == BOARD_ING91881B_02_02_05)
    printf("sensor BME280 init...");
    if (bme280_init(&bme280_data) != BME280_OK)
        printf("failed\n");
    else
    {
        printf("OK\n");
        bme280_set_sensor_settings(BME280_ALL_SETTINGS_SEL, &bme280_data);
        bme280_set_sensor_mode(BME280_NORMAL_MODE, &bme280_data);
    }
#elif(BOARD_ID == BOARD_ING91881B_02_02_06)
    printf("sensor MTS01B init...");
#endif
}

float get_temperature()
{
#ifndef SIMULATION
#if (BOARD_ID == BOARD_ING91881B_02_02_05)


    if (bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data) < 0)
        return 0.0;
    return comp_data.temperature;
#elif (BOARD_ID == BOARD_ING91881B_02_02_06)
    
#include "peripheral_sysctrl.h"

#define I2C_PORT        I2C_PORT_0
#define MTS01B_OK           0
#define MTS01B_E_COMM_FAIL  1

    uint8_t cmd[2]={(uint8_t)(CONVERT_T >>8), (uint8_t)(CONVERT_T & 0xFF)};
    uint8_t reg_data[3] = {0}; 
    //uint8_t sta = MTS01B_E_COMM_FAIL;
    //Read temperature data
    uint8_t sta = i2c_read(I2C_PORT, 0x44, cmd, 2, reg_data, sizeof(reg_data));
    if(sta == MTS01B_E_COMM_FAIL){
        SYSCTRL_ResetBlock(SYSCTRL_Reset_APB_I2C0);
        SYSCTRL_ReleaseBlock(SYSCTRL_Reset_APB_I2C0);
        i2c_init(I2C_PORT);
        platform_printf("i2c rst \n");
    }else {
        platform_printf("[%x][%x][%x]\n", reg_data[0], reg_data[1], reg_data[2]);
        return (float)((40 + ((reg_data[0] & 0x7F) >> 7) * (-1) * (~((reg_data[0] & 0x7F) - 1))) * 100);
    }
#endif
#else
    return rand() & 0x1f;
#endif
}

float get_humidity()
{
#ifndef SIMULATION
#if (BOARD_ID == BOARD_ING91881B_02_02_05)
    if(bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data) < 0)
        return 0.0;
    return comp_data.humidity;

#elif (BOARD_ID == BOARD_ING91881B_02_02_06)
    return 0.0;
#endif
#else
    return rand() & 0x1f;
#endif
}

float get_pressure()
{
#ifndef SIMULATION
#if(BOARD_ID == BOARD_ING91881B_02_02_05)
    if(bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data) < 0)
        return 0.0;
    return comp_data.humidity;

#elif (BOARD_ID == BOARD_ING91881B_02_02_06)
    return 0.0;
#endif
#else
    return rand() & 0x1f;
#endif
}

#endif

//该如何调整，是否存在适合的驱动文件
//-------------------------------------------------加速度计驱动整理-------------------------------------------------
#ifdef BOARD_SENSOR_ACCEL
#include "bma2x2.h"

static struct bma2x2_accel_data sample_xyz = {0};

extern s32 bma2x2_power_on(void);

void setup_accelerometer(void)
{
    printf("bma2x2_power_on...");
    if (bma2x2_power_on()==0)
        printf("success!!\n");
    else
        printf("faild!!\n");
}

void get_acc_xyz(float *x, float *y, float *z)
{
    bma2x2_read_accel_xyz(&sample_xyz);
    *x = sample_xyz.x;
    *y = sample_xyz.y;
    *z = sample_xyz.z;
}

#endif

//OK
//-------------------------------------------------蜂鸣器驱动整理-------------------------------------------------
#ifdef BOARD_SENSOR_BUZZER
#if (BOARD_ID == BOARD_ING91881B_02_02_04)
#define BUZZ_PIN        GIO_GPIO_8
#elif (BOARD_ID == BOARD_ING91881B_02_02_05)
#define BUZZ_PIN        GIO_GPIO_8
#elif (BOARD_ID == BOARD_ING91881B_02_02_06)
#define BUZZ_PIN        GIO_GPIO_8
#endif
void setup_buzzer()
{

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PINCTRL_SetGeneralPadMode(BUZZ_PIN, IO_MODE_PWM, 4, 0);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    PINCTRL_SetPadMux(BUZZ_PIN, IO_SOURCE_PWM6_B);
#else
    #error unknown or unsupported chip family
#endif
}

void set_buzzer_freq(uint32_t freq)
{
    PWM_SetupSimple(BUZZ_PIN >> 1, freq, 50);
}

#endif



//-------------------------------------------------按键配置驱动整理-------------------------------------------------