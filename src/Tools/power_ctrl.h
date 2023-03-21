#ifndef _power_ctrl_h
#define _power_ctrl_h

#ifdef __cplusplus
extern "C" {
#endif

// WARNING: This library is ONLY for ING918XX

typedef enum
{
    POWER_CTRL_MODE_SAFE = 1,       // power consumption is reduced a little bit
                                    // System might be more stable with this mode
                                    // under extreme conditions (high temperature, etc).
    POWER_CTRL_MODE_BALANCED = 0,   // power consumption is significantly reduced,
                                    // RF performance is better than AGGRESSIVE
    POWER_CTRL_MODE_AGGRESSIVE = 2, // power consumption is significantly reduced,
                                    // RF performance might be degraded
    POWER_CTRL_MODE_MAX,
} power_ctrl_mode_t;

// Call this funciton in `app_main`
void power_ctrl_init(power_ctrl_mode_t mode);

// Call this function in event `PLATFORM_CB_EVT_QUERY_DEEP_SLEEP_ALLOWED`
// when deep sleep is allowed
void power_ctrl_before_deep_sleep(void);

// Call this function in event `PLATFORM_CB_EVT_ON_DEEP_SLEEP_WAKEUP`
// Note: Call this before configuring peripherals.
void power_ctrl_deep_sleep_wakeup(void);

// Recommend to call this function before disabling power saving for better RF performance
// Note: This is EXPERIMENTAL.
void power_ctrl_before_disable_power_saving(void);

#ifdef __cplusplus
}
#endif

#endif

