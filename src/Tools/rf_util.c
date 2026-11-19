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
    // CW is already in power boosted mode;
    // this function will enable power boost for data packets.
    platform_set_rf_init_data(rf_data);
    platform_set_rf_power_mapping(power_mapping);
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

#include "peripheral_sysctrl.h"

static const uint32_t rf_data[] = {
#include "rf_powerboost_916.dat"
};

static const int16_t power_mapping[] = {
    -6140, -2336, -1781, -1436, -1181, -1003, -836, -744, -640, -536, -454, -376, -301, -233, -175, -119,
    -64,   -15,   30,    74,    119,   153,   185,  220,  254,  285,  311,  338,  366,  393,  416,  438,
    458,   478,   497,   516,   533,   551,   567,  583,  598,  612,  623,  636,  649,  661,  670,  679,
    691,   700,   708,   718,   727,   735,   741,  750,  755,  763,  770,  775,  783,  789,  794,  800};

void rf_enable_powerboost(void)
{
    platform_set_rf_init_data(rf_data);
    platform_set_rf_power_mapping(power_mapping);

    SYSCTRL_SetBuckDCDCOutput(SYSCTRL_BUCK_DCDC_OUTPUT_2V000);
    SYSCTRL_SetLDOOutputRF(SYSCTRL_LDO_RF_OUTPUT_1V800);
}

#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_20)

#include "peripheral_sysctrl.h"

static const uint32_t rf_data[] = {
#include "rf_powerboost_916.dat"
};


// If set value is 0, use LDO mode.
#define USE_DCDC_MODE 1

static const int16_t power_mapping[] = {
    -6140, -2336, -1781, -1436, -1181, -1003, -836, -744, -640, -536, -454, -376, -301, -233, -175, -119,
    -64,   -15,   30,    74,    119,   153,   185,  220,  254,  285,  311,  338,  366,  393,  416,  438,
    458,   478,   497,   516,   533,   551,   567,  583,  598,  612,  623,  636,  649,  661,  670,  679,
    691,   700,   708,   718,   727,   735,   741,  750,  755,  763,  770,  775,  783,  789,  794,  800};

void rf_enable_powerboost(void)
{
    platform_set_rf_init_data(rf_data);
    platform_set_rf_power_mapping(power_mapping);

#if USE_DCDC_MODE
    SYSCTRL_SetBuckDCDCOutput(SYSCTRL_BUCK_DCDC_OUTPUT_1V800);
    SYSCTRL_EnableBuckDCDC(1);
    SYSCTRL_EnableDCDCMode(1);
#else
    SYSCTRL_EnableDCDCMode(0);
#endif
}

#endif
