#include <stdint.h>
#include <string.h>
#include "kb_scan.h"
#include "ingsoc.h"
#include "peripheral_pinctrl.h"
#include "peripheral_gpio.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

// state changed event
// each bit represent the state of one key
//           COL[0]  COL[1] .... 
//             |       |
// ROW[0] ----K0------K1---------  K[RN -1]
//             |       |
// ROW[1] ---K[RN]----.....................
// ....        |       |
// ....   .................................
extern void kb_state_changed(const uint32_t old_state, const uint32_t new_state);

extern void kb_keep_pressed(const uint32_t keys);

GIO_Index_t scan_rows[] = {GIO_GPIO_6, GIO_GPIO_7,  GIO_GPIO_8,  GIO_GPIO_9};
GIO_Index_t scan_cols[] = {GIO_GPIO_5, GIO_GPIO_12, GIO_GPIO_14, GIO_GPIO_11};

#define KB_ROW_NUM (sizeof(scan_rows) / sizeof(scan_rows[0]))
#define KB_COL_NUM (sizeof(scan_cols) / sizeof(scan_cols[0]))

uint32_t kb_old_state = 0;
uint32_t kb_old_keep_pressed = 0;

uint8_t down_counter[KB_ROW_NUM * KB_COL_NUM] = {0};

void kb_init(void)
{

    int i;
    for (i = 0; i < KB_ROW_NUM; i++)
    {
        PINCTRL_SetPadMux(scan_rows[i], IO_SOURCE_GENERAL);
        GIO_SetDirection(scan_rows[i], GIO_DIR_OUTPUT);
        GIO_WriteValue(scan_rows[i], 0);
    }
    for (i = 0; i < KB_COL_NUM; i++)
    {
        PINCTRL_SetPadMux(scan_cols[i], IO_SOURCE_GENERAL);
        GIO_SetDirection(scan_cols[i], GIO_DIR_INPUT);
        PINCTRL_Pull(scan_cols[i], PINCTRL_PULL_DOWN);
    }
    memset(down_counter, 0, sizeof(down_counter));
}

void kb_update(void)
{
    uint32_t new_state = 0;    
    int row, col;
    uint32_t keep_pressed = 0;
    for (row = 0; row < KB_ROW_NUM; row++)
    {
        uint32_t value;
        GIO_WriteValue(scan_rows[row], 1);
        value = GIO_ReadAll();
        GIO_WriteValue(scan_rows[row], 0);
        for (col = 0; col < KB_COL_NUM; col++)
        {
            uint8_t id = row * KB_COL_NUM + col;
            if (value & (1 << scan_cols[col]))
            {
                new_state |=  1 << id;
                if (down_counter[id] < 255)
                {
                    down_counter[id]++;
                    if (down_counter[id] > KEEP_PRESSED_LIMIT)
                        keep_pressed |= 1 << id;
                }
            }
            else
                down_counter[id] = 0;
        }
    }

    if (new_state != kb_old_state)
    {
        kb_state_changed(kb_old_state, new_state);
        kb_old_state = new_state;
    }
    
    if (keep_pressed != kb_old_keep_pressed)
    {
        if (keep_pressed)
            kb_keep_pressed(keep_pressed);
        kb_old_keep_pressed = keep_pressed;
    }
}

#endif
