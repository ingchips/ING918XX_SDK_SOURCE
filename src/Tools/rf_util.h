#ifndef _rf_util_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable Tx power boost mode
 *
 * The maximum Tx power is given by the last element of `power_mapping` in
 * `rf_util.c`.
*/
void rf_enable_powerboost(void);

#ifdef __cplusplus
}
#endif

#endif
