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

//-------------------------------------------------RGB_LED drive sort-------------------------------------------------
#ifdef BOARD_RGB_LED

#ifndef PWM_LED

#ifndef PIN_RGB_LED
#define PIN_RGB_LED   GIO_GPIO_0
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

uint64_t GPIO_MASK = 1 << PIN_RGB_LED;

#define APB_BASE_test ((uint32_t)0x40000010UL)  //GPIO_DO
static void ws2881_write(uint32_t value)
{
    int8_t i;

    for( i = 0; i < 24; i++ )
    {
        uint32_t bit = value & ( 0x00800000 >> i);

        if (bit)
        {   
            GIO_SetBits(GPIO_MASK);
            GIO_ClearBits(GPIO_MASK);
        }
        else
        {       
            //This part cannot be replaced by GIO_SetBits(GPIO_MASK),otherwise 
            //it does not satisfy the timing requirement.
            uint32_t tmp2 = (*((__IO uint32_t *)APB_BASE_test))&(~0x1);    
            GIO_SetBits(GPIO_MASK);
            (*((__IO uint32_t *)APB_BASE_test)) = tmp2;

        }
    }
    delay(100 * 8);
}

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


//-------------------------------------------------Thermometer drive sort-------------------------------------------------
#ifdef BOARD_SENSOR_THERMO

#ifndef SIMULATION

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


    #include "peripheral_sysctrl.h"

    #define I2C_PORT        I2C_PORT_0
    #define MTS01B_OK           0
    #define MTS01B_E_COMM_FAIL  1

    /* Temperature measurement instruction of MTS01B */
    #define CONVERT_T   0xCC44

    uint8_t cmd[2]={(uint8_t)(CONVERT_T >>8), (uint8_t)(CONVERT_T & 0xFF)};
    uint8_t reg_data[3] = {0};

#endif

#endif


void setup_env_sensor()
{
#ifndef SIMULATION
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
    
    if(i2c_read(I2C_PORT, 0x44, cmd, 2, reg_data, sizeof(reg_data)) == MTS01B_E_COMM_FAIL)
        return 0.0;   
    return (float)((40 + ((reg_data[0] & 0x7F) >> 7) * (-1) * (~((reg_data[0] & 0x7F) - 1))) * 100);
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

//-------------------------------------------------accelerator driver sort-------------------------------------------------
#ifdef BOARD_SENSOR_ACCEL
#include "bma2x2.h"

typedef enum {
    RANGE_2G = 0x03, //±2g
    RANGE_4G = 0x05, //±4g
    RANGE_8G = 0x08, //±8g
    RANGE_16G = 0x0C, //±16g
}ACCEL_RANGE;

float accel_factor = 0.000;

#define ACC_SAMPLING_RATE    (50) 

static struct bma2x2_accel_data sample_xyz = {0};

extern s32 bma2x2_power_on(void);

static uint8_t sensor_range = 0;


// mg/LSB
float get_accel_mg_factor()
{
    bma2x2_get_range(&sensor_range);
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
#ifndef SIMULATION
    bma2x2_read_accel_xyz(&sample_xyz);
    float factor = get_accel_mg_factor();
    *x = sample_xyz.x * factor * 0.001;
    *y = sample_xyz.y * factor * 0.001;
    *z = sample_xyz.z * factor * 0.001;
#else
    static uint16_t sim_cnt = 0;
    sim_cnt++;
    sample_xyz->x = 0;
    sample_xyz->y = 0;
    sample_xyz->z = 0;
    if (sim_cnt < ACC_SAMPLING_RATE - 10)
        return;
    if (sim_cnt >= ACC_SAMPLING_RATE)
    {
        sim_cnt = 0;
        return;
    }

    if (sim_cnt < ACC_SAMPLING_RATE - 5)
        sample_xyz->x = (sim_cnt + 5 - (ACC_SAMPLING_RATE - 5)) * 200;
    else
        sample_xyz->x = (ACC_SAMPLING_RATE - sim_cnt) * 200;
#endif
}

#endif


//-------------------------------------------------buzzer driver sort-------------------------------------------------
#ifdef BOARD_SENSOR_BUZZER

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
#endif

//-------------------------------------------------key configuration-------------------------------------------------
#ifdef BOARD_KEY_CONFIG

const static uint8_t key_pins[] = {
    IO_NOT_A_PIN, GIO_GPIO_1, GIO_GPIO_5, GIO_GPIO_7,
    GIO_GPIO_4
};

#define ARRAY_LEN(x)    (sizeof(x)/sizeof(x[0]))

void setup_keyconfigure()
{
    for (int i = 0; i < ARRAY_LEN(key_pins); i++)
    {
        if (key_pins[i] == IO_NOT_A_PIN) continue;
        PINCTRL_SetPadMux(key_pins[i], IO_SOURCE_GPIO);
        GIO_SetDirection((GIO_Index_t)key_pins[i], GIO_DIR_INPUT);
        GIO_ConfigIntSource((GIO_Index_t)key_pins[i],
                            GIO_INT_EN_LOGIC_LOW_OR_FALLING_EDGE | GIO_INT_EN_LOGIC_HIGH_OR_RISING_EDGE,
                            GIO_INT_EDGE);
    }
}
#endif