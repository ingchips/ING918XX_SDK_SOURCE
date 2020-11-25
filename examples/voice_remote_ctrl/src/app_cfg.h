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

#define EFFECTIVE_SAMPLING_RATE  16000

#define OVER_SAMPLING        1                      // must be 2^n
#define OVER_SAMPLING_MASK   (OVER_SAMPLING - 1)

#define SAMPLING_RATE        (EFFECTIVE_SAMPLING_RATE * OVER_SAMPLING)

#endif
