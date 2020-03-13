#ifndef DISP_TASK_H
#define DISP_TASK_H

#include <stdint.h>

#define DISP_LOGO       0
#define DISP_TIME       1
#define DISP_PED        2
#define DISP_HR         3
#define DISP_CONNECTED      4
#define DISP_DISCONNECTED   5

void disp_init(void);

void disp_item(uint8_t item);

#endif
