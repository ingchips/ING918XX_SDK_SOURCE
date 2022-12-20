#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"

#include "peripheral_pwm.h"
#include "peripheral_sysctrl.h"
#include "peripheral_pinctrl.h"
#include "peripheral_gpio.h"

#include "platform_api.h"

#define rand() platform_rand()

//-------------------------------------------------RGB_LED drive sort-------------------------------------------------
#ifdef BOARD_USE_RGB_LED

#ifndef PIN_RGB_LED
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
#define PIN_RGB_LED   GIO_GPIO_5
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#define PIN_RGB_LED   GIO_GPIO_0
#endif
#endif

#define LED_TLC59731    0
#define LED_WS2881      1

//default old driver:tlc59731
#ifndef RGB_LED
#define RGB_LED     LED_WS2881
#endif

// CPU clok: PLL_CLK_FREQ  48000000
// 1 cycle = 21ns
// 48 cycles per us
// Tcycle = 2us --> ~100 cycles
static void delay(int cycles)
{
    int i;
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    cycles *= SYSCTRL_GetHClk() / 48000000;
#endif
    for (i = 0; i < cycles; i++)
    {
#ifdef __GNUC__
        __asm("NOP");
#else
        __nop();
#endif
    }
}

#if(BOARD_ID == BOARD_ING91881B_02_02_05)

static void tlc59731_write(uint32_t value)
{
    int8_t i;

    #define pulse()                     \
        { GIO_WriteValue(PIN_RGB_LED, 1);   \
        delay(1);                       \
        GIO_WriteValue(PIN_RGB_LED, 0); } while (0)

    for( i = 0; i < 32; i++ )
    {
        uint32_t bit = value & ( 0x80000000 >> i );
        pulse();

        if (bit)
        {
            delay(10);
            pulse();
            delay(78);
        }
        else
            delay(90);
    }
    delay(100 * 8);
}

void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t cmd = (0x3a << 24) | (b << 16) | (r << 8) | g;

    tlc59731_write(cmd);
}

#elif(BOARD_ID == BOARD_ING91881B_02_02_06)

#define GPIO_MASK (1 << PIN_RGB_LED)
#define PIN_PWM_LED     GIO_GPIO_5
static void ws2881_write(uint32_t value)
{

    int8_t i;

    for( i = 0; i < 24; i++ )
    {
        uint32_t bit = value & ( 0x00800000 >> i);

        if (bit){
            // GIO_SetBits(GPIO_MASK);
            // GIO_ClearBits(GPIO_MASK);
            PWM_SetupSimple(PIN_PWM_LED >> 1, 100000, 50);
        } else {
            PWM_SetupSimple(PIN_PWM_LED >> 1, 100000, 50);
            // GIO_SetQuicPulse(GPIO_MASK);
        }
    }
    printf("finished!\n");
    delay(100 * 8);
}

void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t cmd = (0x3a << 24) | (r << 16) | (g << 8) | b;

    ws2881_write(cmd);
}

#endif

void setup_rgb_led()
{
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_GPIO0));
    PINCTRL_SetPadMux(PIN_RGB_LED, IO_SOURCE_GPIO);

    GIO_SetDirection(PIN_RGB_LED, GIO_DIR_OUTPUT);
    GIO_WriteValue(PIN_RGB_LED, 0);

    set_rgb_led_color(50, 50, 50);
}

#else

void setup_rgb_led() {}
void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b) {}

#endif


//-------------------------------------------------Thermometer drive sort-------------------------------------------------
#ifdef BOARD_USE_THERMO

#ifndef I2C_PORT
#define I2C_PORT        I2C_PORT_0
#endif

#include "iic.h"

#if (BOARD_ID == BOARD_ING91881B_02_02_05)

#define BME280_ADDR     BME280_I2C_ADDR_PRIM
#define BME280_INTF_RET_TYPE        int8_t

#include "bme280.c"

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

static uint8_t dev_addr = BME280_ADDR;

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

static struct bme280_data comp_data;

#elif (BOARD_ID == BOARD_ING91881B_02_02_06)

    #define I2C_PORT        I2C_PORT_0
    #define MTS01B_OK           0
    #define MTS01B_E_COMM_FAIL  1

    /* Temperature measurement instruction of MTS01B */
    #define CONVERT_T   0xCC44

    uint8_t cmd[2]={(uint8_t)(CONVERT_T >>8), (uint8_t)(CONVERT_T & 0xFF)};
    uint8_t reg_data[3] = {0};

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
    printf("OK\n");

#endif
}

float get_temperature()
{
#if (BOARD_ID == BOARD_ING91881B_02_02_05)

    if (bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data) < 0)
        return 0.0;
    return comp_data.temperature;
#elif (BOARD_ID == BOARD_ING91881B_02_02_06)

    if(i2c_read(I2C_PORT, 0x44, cmd, 2, reg_data, sizeof(reg_data)) == MTS01B_E_COMM_FAIL)
        return 0.0;
    return (float)((40 + ((reg_data[0] & 0x7F) >> 7) * (-1) * (~((reg_data[0] & 0x7F) - 1))) * 100);
#endif
}

float get_humidity()
{
#if (BOARD_ID == BOARD_ING91881B_02_02_05)
    if(bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data) < 0)
        return 0.0;
    return comp_data.humidity;

#elif (BOARD_ID == BOARD_ING91881B_02_02_06)
    return 0.0;
#endif
}

float get_pressure()
{
#if(BOARD_ID == BOARD_ING91881B_02_02_05)
    if(bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data) < 0)
        return 0.0;
    return comp_data.humidity;

#elif (BOARD_ID == BOARD_ING91881B_02_02_06)
    return 0.0;
#endif
}

#else

void setup_env_sensor() {}

float get_temperature()
{
    return (rand() & 0x1f) * 100;
}

float get_humidity()
{
    return 0.0;
}

float get_pressure()
{
    return 0.0;
}

#endif

//-------------------------------------------------accelerator driver sort-------------------------------------------------
#ifdef BOARD_USE_ACCEL
#include "bma2x2.c"
#include "bma2x2_support.c"

typedef enum {
    RANGE_2G = 0x03, //±2g
    RANGE_4G = 0x05, //±4g
    RANGE_8G = 0x08, //±8g
    RANGE_16G = 0x0C, //±16g
} ACCEL_RANGE;

#define ACC_SAMPLING_RATE    (50)

static struct bma2x2_accel_data sample_xyz = {0};

extern s32 bma2x2_power_on(void);

static float mg_factor = 0.0;

// mg/LSB
float get_accel_mg_factor(uint8_t sensor_range)
{
#if (BOARD_ID == BOARD_ING91881B_02_02_05)
    return sensor_range == RANGE_2G ? 0.224 :
                sensor_range == RANGE_4G ? 0.488 :
                    sensor_range == RANGE_8G ? 0.977 : 1.953;
#elif (BOARD_ID == BOARD_ING91881B_02_02_06)
    return sensor_range == RANGE_2G ? 0.98 :
                sensor_range == RANGE_4G ? 1.95 :  3.91;
#endif
}

void setup_accelerometer(void)
{
    uint8_t sensor_range = 0;
    printf("bma2x2_power_on...");
    if (bma2x2_power_on()==0)
        printf("success!!\n");
    else
        printf("faild!!\n");
    bma2x2_get_range(&sensor_range);
    mg_factor = get_accel_mg_factor(sensor_range);
}

void get_acc_xyz(float *x, float *y, float *z)
{
    bma2x2_read_accel_xyz(&sample_xyz);
    *x = sample_xyz.x * mg_factor * 0.001;
    *y = sample_xyz.y * mg_factor * 0.001;
    *z = sample_xyz.z * mg_factor * 0.001;
}

#else

void setup_accelerometer(void)
{
}

void get_acc_xyz(float *x, float *y, float *z)
{
    *x = rand() & 0xf;
    *y = rand() & 0xf;
    *z = rand() & 0xf;
}
#endif

//-------------------------------------------------buzzer driver sort-------------------------------------------------
#ifdef BOARD_USE_BUZZER

#define BUZZ_PIN        GIO_GPIO_8

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

void set_buzzer_freq(uint16_t freq)
{
    PWM_SetupSimple(BUZZ_PIN >> 1, freq, 50);
}

#else

void setup_buzzer()
{
}

void set_buzzer_freq(uint16_t freq)
{
}

#endif

//-------------------------------------------------key configuration-------------------------------------------------
#ifdef BOARD_USE_KEYS

const static GIO_Index_t key_pins[] = {
    GIO_GPIO_1, GIO_GPIO_5, GIO_GPIO_7, GIO_GPIO_4
};

#define ARRAY_LEN(x)    (sizeof(x)/sizeof(x[0]))

void setup_keys()
{
    int i;
    for (i = 0; i < ARRAY_LEN(key_pins); i++)
    {
        PINCTRL_SetPadMux(key_pins[i], IO_SOURCE_GPIO);
        GIO_SetDirection(key_pins[i], GIO_DIR_INPUT);
        GIO_ConfigIntSource((GIO_Index_t)key_pins[i],
                            GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                            GIO_INT_EDGE);
    }
}
#else
void setup_keys()
{
}
#endif
