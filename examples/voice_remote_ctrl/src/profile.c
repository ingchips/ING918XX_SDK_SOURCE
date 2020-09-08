#include "app_cfg.h"

#if (APP_TYPE == APP_ING)
#include "profile_ing.c"
#elif (APP_TYPE == APP_ANDROID)
#include "profile_android.c"
#elif (APP_TYPE == APP_MIBOXS)
#include "profile_miboxs.c"
#elif (APP_TYPE == APP_CUSTOMER)
#include "profile_customer.c"
#endif
