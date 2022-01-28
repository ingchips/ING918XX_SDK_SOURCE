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

void rf_enable_powerboost(void)
{
#warning WIP: rf_enable_powerboost
}

#endif
