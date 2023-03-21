#include "platform_api.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)

static const uint32_t rf_data[] = {
#include "rf_powerboost.dat"
};

static const int16_t power_mapping[] = {
-6337, -2528, -1936, -1593, -1354, -1164, -1011, -882, -770, -670, \
-583, -506, -439, -373, -315, -260, -211, -161, -116, -73, -35, 3, \
38, 72, 100, 130, 159, 186, 209, 234, 257, 279, 300, 322, 342, 360, \
378, 395, 410, 426, 441, 455, 468, 482, 493, 505, 516, 527, 533, 544, \
554, 563, 571, 581, 589, 596, 603, 611, 618, 625, 631, 637, 644, 650};

void rf_enable_powerboost(void)
{
    platform_set_rf_init_data(rf_data);
    platform_set_rf_power_mapping(power_mapping);
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
