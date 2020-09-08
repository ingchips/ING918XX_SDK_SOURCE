#ifndef APP_CFG_H
#define APP_CFG_H

#define BOARD_DEV               0
#define BOARD_REM               1

#define APP_ING                 0
#define APP_ANDROID             1
#define APP_MIBOXS              2
#define APP_CUSTOMER            3

#define ADC_CHANNEL_ID          4

#ifndef BOARD
#define BOARD                   BOARD_DEV
#endif

#ifndef APP_TYPE
#define APP_TYPE                APP_ING
#endif

#if (APP_TYPE != APP_ING)
#define HAS_KB
#endif

#if (BOARD == BOARD_DEV)
#define ADC_CHANNEL_ID          4
#else
#define ADC_CHANNEL_ID          4
#endif

#endif
