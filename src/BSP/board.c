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

#if ((BOARD_ID == BOARD_ING91881B_02_02_04) || (BOARD_ID == BOARD_ING91881B_02_02_05) || (BOARD_ID == BOARD_ING91881B_02_02_06))
#if (INGCHIPS_FAMILY != INGCHIPS_FAMILY_918)
    #error INGCHIPS_FAMILY conflicts with BOARD_ID
#endif
#elif ((BOARD_ID == BOARD_DB682AC1A) || (BOARD_ID == BOARD_DB72C8K1A))
#if (INGCHIPS_FAMILY != INGCHIPS_FAMILY_916)
    #error INGCHIPS_FAMILY conflicts with BOARD_ID
#endif
#endif

#ifndef PIN_RGB_LED
#if ((BOARD_ID == BOARD_ING91881B_02_02_04) || (BOARD_ID == BOARD_ING91881B_02_02_05) || (BOARD_ID == BOARD_ING91881B_02_02_06))
#define PIN_RGB_LED   GIO_GPIO_0
#elif (BOARD_ID == BOARD_DB682AC1A)
#define PIN_RGB_LED   GIO_GPIO_6
#elif (BOARD_ID == BOARD_DB72C8K1A)
#define PIN_RGB_LED   GIO_GPIO_5
#endif
#endif

//-------------------------------------------------RGB_LED drive sort-------------------------------------------------
#ifdef BOARD_USE_RGB_LED

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
        __NOP();
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

#elif ((BOARD_ID == BOARD_ING91881B_02_02_06) || (BOARD_ID == BOARD_DB682AC1A))

#define PWM_LED_CHANNEL     0
#define PWM_LED_FREQ    1000000

static void ws2881_write(uint32_t value)
{
    int8_t i;

    for( i = 0; i < 24; i++ )
    {
        uint32_t bit = value & ( 0x00800000 >> i);

        if (bit)
        {
            PWM_SetupSingle(PWM_LED_CHANNEL, 600); // T1H = ~600NS
        }
        else
        {
            PWM_SetupSingle(PWM_LED_CHANNEL, 300); // T0H = ~800NS
        }
    }
    delay(100 * 8);
}

void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t cmd = (0x3a << 24) | (b << 16) | (r << 8) | g;

    ws2881_write(cmd);
}

#elif (BOARD_ID == BOARD_DB72C8K1A)

static void PWM_SinglePulseInit(const uint8_t channel_index, uint8_t is_out_a)
{
    PWM_Enable(channel_index, 0);
    PWM_SetPeraThreshold(channel_index, 0);
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PWM_SetMultiDutyCycleCtrl(channel_index, 0);        // do not use multi duty cycles
#endif
    PWM_SetHighThreshold(channel_index, 0, 0);
    PWM_SetMode(channel_index, PWM_WORK_MODE_SINGLE_WITHOUT_DIED_ZONE);
    if (is_out_a)
        PWM_SetMask(channel_index, 0, 1);
    else
        PWM_SetMask(channel_index, 1, 0);
    if (!is_out_a)
        PWM_SetInvertOutput(channel_index, 0, 1);
}

static void PWM_SinglePulseSend(const uint8_t channel_index, const uint32_t pulse_width)
{
    uint32_t pera = PWM_CLOCK_FREQ / (1000000000 / pulse_width);
    PWM_Enable(channel_index, 0);
    PWM_SetPeraThreshold(channel_index, pera);
    PWM_Enable(channel_index, 1);
}

#define PWM_LED_CHANNEL     0
#define PWM_IO_SOURCE       IO_SOURCE_PWM0_B

static void ws2881_write(uint32_t value)
{
    int8_t i;

    for( i = 0; i < 24; i++ )
    {
        uint32_t bit = value & ( 0x00800000 >> i);

        if (bit) // T_DUTY: ~3.7US ( T_DUTY  = T0H+T0L = T1H+T1L )
        {
            PWM_SinglePulseSend(PWM_LED_CHANNEL, 800); // T1H = ~800NS
        }
        else
        {
            PWM_SinglePulseSend(PWM_LED_CHANNEL, 300); // T0H = ~300NS
        }
    }
    delay(100 * 8); // RES(>280US) = ~230US
}

void set_rgb_led_color(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t cmd = (b << 16) | (r << 8) | g;

    ws2881_write(cmd);
}

#endif

void setup_rgb_led()
{
#if(BOARD_ID == BOARD_ING91881B_02_02_05)
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_GPIO0));
    PINCTRL_SetPadMux(PIN_RGB_LED, IO_SOURCE_GPIO);

    GIO_SetDirection(PIN_RGB_LED, GIO_DIR_OUTPUT);
    GIO_WriteValue(PIN_RGB_LED, 0);

#elif(BOARD_ID == BOARD_ING91881B_02_02_06)
    SYSCTRL_ClearClkGateMulti( (1 << SYSCTRL_ClkGate_APB_PinCtrl)
                             | (1 << SYSCTRL_ClkGate_APB_PWM));
    PINCTRL_SetGeneralPadMode(PIN_RGB_LED, IO_MODE_PWM, 0, 0);
#elif(BOARD_ID == BOARD_DB682AC1A)
    SYSCTRL_ClearClkGateMulti( (1 << SYSCTRL_ClkGate_APB_PinCtrl)
                             | (1 << SYSCTRL_ClkGate_APB_PWM));
    SYSCTRL_SelectPWMClk(SYSCTRL_CLK_SLOW_DIV_1);
    PINCTRL_SetPadMux(PIN_RGB_LED, IO_SOURCE_PWM0_A);
#elif(BOARD_ID == BOARD_DB72C8K1A)
    SYSCTRL_ClearClkGateMulti( (1 << SYSCTRL_ClkGate_APB_PinCtrl)
                             | (1 << SYSCTRL_ClkGate_APB_PWM));
    SYSCTRL_SelectPWMClk(SYSCTRL_CLK_SLOW_DIV_1);
    PINCTRL_SetPadMux(PIN_RGB_LED, PWM_IO_SOURCE);
    PWM_SinglePulseInit(PWM_LED_CHANNEL, (PWM_IO_SOURCE==IO_SOURCE_PWM0_A) );
#else
    #error unknown or unsupported board type
#endif
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

void reboot_i2c_module()
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    SYSCTRL_ResetBlock(SYSCTRL_Reset_APB_I2C0);
    SYSCTRL_ReleaseBlock(SYSCTRL_Reset_APB_I2C0);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    SYSCTRL_ResetBlock(SYSCTRL_ITEM_APB_I2C0);
    SYSCTRL_ReleaseBlock(SYSCTRL_ITEM_APB_I2C0);
#endif
    i2c_init(I2C_PORT);
}

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

#elif ((BOARD_ID == BOARD_ING91881B_02_02_06) || (BOARD_ID == BOARD_DB682AC1A))

#endif

void setup_env_sensor()
{
#if(BOARD_ID == BOARD_ING91881B_02_02_05)

    printf("sensor BME280 init...");
    if (bme280_init(&bme280_data) != BME280_OK)
    {
        printf("failed\n");
        reboot_i2c_module();
    }
    else
    {
        printf("OK\n");
        bme280_set_sensor_settings(BME280_ALL_SETTINGS_SEL, &bme280_data);
        bme280_set_sensor_mode(BME280_NORMAL_MODE, &bme280_data);
    }

#elif ((BOARD_ID == BOARD_ING91881B_02_02_06) || (BOARD_ID == BOARD_DB682AC1A))

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
#elif ((BOARD_ID == BOARD_ING91881B_02_02_06) || (BOARD_ID == BOARD_DB682AC1A))
    #define MTS01B_OK           0
    #define MTS01B_E_COMM_FAIL  1

    int symbolbit, sensor_status;
    int try_cnt = 5;
    uint16_t temperature_2bytes; //16bit temperature data
    /* Temperature measurement instruction of MTS01B */
    #define CONVERT_T   0xCC44
    uint8_t cmd[2] = {(uint8_t)(CONVERT_T >> 8), (uint8_t)(CONVERT_T & 0xFF)};
    uint8_t reg_data[3] = {0};

    while(try_cnt--)
    {
        sensor_status = i2c_read(I2C_PORT, 0x44, cmd, 2, reg_data, 3);
        if(sensor_status != MTS01B_OK)
            reboot_i2c_module();
        else
            break;
    }

    if(try_cnt == 0 && sensor_status != MTS01B_OK)
    {
        printf("failed!\n");
        return 0.0;
    }

    temperature_2bytes = (reg_data[0] << 8) | reg_data[1];
    symbolbit = reg_data[0] >> 7;

    return (float)(4000 + (((symbolbit == 0 ? (temperature_2bytes & 0X7fff) : ((-1) * ((~(temperature_2bytes - 1)) & 0xffff))) * 100) >> 8));
#endif
}

float get_humidity()
{
#if (BOARD_ID == BOARD_ING91881B_02_02_05)
    if(bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data) < 0)
        return 0.0;
    return comp_data.humidity;

#elif ((BOARD_ID == BOARD_ING91881B_02_02_06) || (BOARD_ID == BOARD_DB682AC1A))
    return 0.0;
#endif
}

float get_pressure()
{
#if(BOARD_ID == BOARD_ING91881B_02_02_05)
    if(bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_data) < 0)
        return 0.0;
    return comp_data.humidity;

#elif ((BOARD_ID == BOARD_ING91881B_02_02_06) || (BOARD_ID == BOARD_DB682AC1A))
    return 0.0;
#endif
}

uint16_t get_thermo_addr()
{
#if (BOARD_ID == BOARD_ING91881B_02_02_05)
    return 0x76;
#elif ((BOARD_ID == BOARD_ING91881B_02_02_06) || (BOARD_ID == BOARD_DB682AC1A))
    return 0x44;
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

uint16_t get_thermo_addr()
{
    return 0;
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
#elif ((BOARD_ID == BOARD_ING91881B_02_02_06) || (BOARD_ID == BOARD_DB682AC1A))
    return sensor_range == RANGE_2G ? 0.98 :
                sensor_range == RANGE_4G ? 1.95 :  3.91;
#endif
}

void setup_accelerometer(void)
{
    uint8_t sensor_range = 0;
#if (BOARD_ID == BOARD_ING91881B_02_02_05)
    printf("bma2x2_power_on...");
#elif ((BOARD_ID == BOARD_ING91881B_02_02_06) || (BOARD_ID == BOARD_DB682AC1A))
    printf("stk8ba58_power_on...");
#endif
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

uint16_t get_acc_addr()
{
    return 0x18;
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

uint16_t get_acc_addr()
{
    return 0;
}
#endif

//-------------------------------------------------buzzer driver sort-------------------------------------------------
#ifdef BOARD_USE_BUZZER

#define BUZZ_PIN        GIO_GPIO_8

void setup_buzzer()
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    SYSCTRL_ClearClkGateMulti((1 << SYSCTRL_ClkGate_APB_PWM));
    PINCTRL_SetGeneralPadMode(BUZZ_PIN, IO_MODE_PWM, 4, 0);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    SYSCTRL_ClearClkGateMulti( (1 << SYSCTRL_ClkGate_APB_PinCtrl)
                                    | (1 << SYSCTRL_ClkGate_APB_PWM));
    SYSCTRL_SelectPWMClk(SYSCTRL_CLK_32k);
    PINCTRL_SetPadMux(BUZZ_PIN, IO_SOURCE_PWM0_B);
#else
    #error unknown or unsupported chip family
#endif
}

void set_buzzer_freq(uint16_t freq)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PWM_SetupSimple(BUZZ_PIN >> 1, freq, 50);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    PWM_SetupSimple(0, freq, 50);
#endif
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

#if ((BOARD_ID == BOARD_ING91881B_02_02_04) || (BOARD_ID == BOARD_ING91881B_02_02_05) || (BOARD_ID == BOARD_ING91881B_02_02_06))
    const static GIO_Index_t key_pins[] = {
        GIO_GPIO_1, GIO_GPIO_5, GIO_GPIO_7, GIO_GPIO_4
    };
#elif (BOARD_ID ==  BOARD_DB682AC1A)
    const static GIO_Index_t key_pins[] = {
        GIO_GPIO_6, GIO_GPIO_10, GIO_GPIO_11, GIO_GPIO_9
    };
#endif

#define ARRAY_LEN(x)    (sizeof(x) / sizeof(x[0]))

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
