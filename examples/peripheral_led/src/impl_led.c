#include "ingsoc.h"
#include "FreeRTOS.h"
#include "task.h"

#include "board.h"

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
