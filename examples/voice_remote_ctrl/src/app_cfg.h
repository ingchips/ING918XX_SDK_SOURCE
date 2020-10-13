#ifndef APP_CFG_H
#define APP_CFG_H

#define BOARD_DEV               0
#define BOARD_REM               1

#define ADC_CHANNEL_ID          4

#ifndef BOARD
#define BOARD                   BOARD_DEV
#endif

#if (BOARD == BOARD_REM)
#define HAS_KB
#endif

#if (BOARD == BOARD_DEV)
#define ADC_CHANNEL_ID          4
#else
#define ADC_CHANNEL_ID          4
#endif

#endif
