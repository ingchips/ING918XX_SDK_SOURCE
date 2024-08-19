#include "platform_api.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

void rf_enable_powerboost(void)
{
    // ING918 is already in power boosted mode
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#include "peripheral_sysctrl.h"

static const uint32_t rf_data[] = {
#include "rf_powerboost_916.dat"
};

static const int16_t power_mapping[] = {
-6337, -2533, -1978, -1633, -1378, -1200, -1033, -941, -837, -733, -651, -573, \
-498, -430, -372, -316, -261, -212, -167, -123, -78, -44, -12, 23, 57, \
88, 114, 141, 169, 196, 219, 241, 261, 281, 300, 319, 336, 354, 370, 386, \
401, 415, 426, 439, 452, 464, 473, 482, 494, 503, 511, 521, 530, 538, 544, \
553, 558, 566, 573, 578, 586, 592, 597, 603};

void rf_enable_powerboost(void)
{
    platform_set_rf_init_data(rf_data);
    platform_set_rf_power_mapping(power_mapping);

    SYSCTRL_SetBuckDCDCOutput(SYSCTRL_BUCK_DCDC_OUTPUT_2V000);
    SYSCTRL_SetLDOOutputRF(SYSCTRL_LDO_RF_OUTPUT_1V800);
}

#endif
