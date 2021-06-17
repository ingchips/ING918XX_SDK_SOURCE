#ifndef _power_ctrl_h
#define _power_ctrl_h

#ifdef __cplusplus
extern "C" {
#endif

    
// Call this funciton in `app_main`
void power_ctrl_init(void);

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

