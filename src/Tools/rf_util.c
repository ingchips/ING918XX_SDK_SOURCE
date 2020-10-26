#include "platform_api.h"

const uint32_t rf_data[] = {
#include "rf_powerboost.dat"
};

void rf_enable_powerboost(void)
{
    platform_set_rf_init_data(rf_data);
}
