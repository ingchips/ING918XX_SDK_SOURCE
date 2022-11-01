#include "ingsoc.h"
#include "FreeRTOS.h"
#include "task.h"


/* RGB_LED灯驱动整理*/
#ifndef PWM_LED

#include "rgb_led.c"

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

typedef struct
{
    union
    {
        struct {uint8_t r, g, b;};
        uint32_t value;
    };
} rgb_t;

struct
{
    rgb_t rgb0, rgb1, cur;
    int dir;
} breathing_info = {};
    
static uint8_t value_step(uint8_t cur, uint8_t target, uint8_t step)
{
    int offset = (int)target - cur;
    if (offset > step)
    {
        return cur + step;
    }
    else if (offset < -(int)step)
    {
        return cur - step;
    }
    else
        return target;
}

static void breathing_task(void *pdata)
{
    #define STEP 2
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(30));
        if (breathing_info.rgb0.value == breathing_info.rgb1.value)
            continue;
        rgb_t *target = breathing_info.dir ? &breathing_info.rgb0 : &breathing_info.rgb1;
        breathing_info.cur.r = value_step(breathing_info.cur.r,
                                          target->r, STEP);
        breathing_info.cur.g = value_step(breathing_info.cur.g,
                                          target->g, STEP);
        breathing_info.cur.b = value_step(breathing_info.cur.b,
                                          target->b, STEP);
        set_rgb_led_color(breathing_info.cur.r, breathing_info.cur.g, breathing_info.cur.b);
        if (breathing_info.cur.value == breathing_info.rgb1.value)
            breathing_info.dir = 1;
        else if (breathing_info.cur.value == breathing_info.rgb0.value)
            breathing_info.dir = 0;
    }
}

void setup_rgb_breathing()
{
    xTaskCreate(breathing_task,
               "d",
               configMINIMAL_STACK_SIZE,
               NULL,
               (configMAX_PRIORITIES - 1),
               NULL);
}

void set_rbg_breathing(rgb_t rgb0, rgb_t rgb1)
{
    breathing_info.rgb0 = rgb0;
    breathing_info.rgb1 = rgb1;
    breathing_info.dir = 0;
    breathing_info.cur = breathing_info.rgb0;
    set_rgb_led_color(breathing_info.cur.r, breathing_info.cur.g, breathing_info.cur.b);
}


/* themometor 驱动整理 */
#include "bme280.h"

#define BME2800_SENSOR_CHIP     0
#define MTS001B_SENSOR_CHIP     1

#ifndef TEMP_SENSOR_CHIP    
#define TEMP_SENSOR_CHIP    BME2800_SENSOR_CHIP
#endif

void bme280_sensor_init(struct bme280_dev *dev)
{
    printf("sensor init...");
    if (bme280_init(dev) != BME280_OK)
        printf("failed\n");
    else
    {
        printf("OK\n");
        bme280_set_sensor_settings(BME280_ALL_SETTINGS_SEL, dev);
        bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);
    }
}

int8_t bme280_get_sensor_data3(uint8_t sensor_comp, struct bme280_data *comp_data, struct bme280_dev *dev)
{  
    return bme280_get_sensor_data(BME280_ALL, comp_data, dev);
}

double get_temperature(struct bme280_data *comp_data)
{
#if (TEMP_SENSOR_CHIP == BME2800_SENSOR_CHIP)

    return comp_data->temperature;

#elif (TEMP_SENSOR_CHIP == MTS001B_SENSOR_CHIP)

#error unknown or unsupported chip family

#endif
}

double get_humidity(struct bme280_data *comp_data)
{
#if (TEMP_SENSOR_CHIP == BME2800_SENSOR_CHIP)

    return comp_data->humidity;

#elif (TEMP_SENSOR_CHIP == MTS001B_SENSOR_CHIP)

#error unknown or unsupported chip family

#endif
}

double get_pressure(struct bme280_data *comp_data)
{
#if (TEMP_SENSOR_CHIP == BME2800_SENSOR_CHIP)

    return comp_data->humidity;

#elif (TEMP_SENSOR_CHIP == MTS001B_SENSOR_CHIP)

#error unknown or unsupported chip family

#endif
}


/* accelerometer 驱动整理 */