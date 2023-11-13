#include <stdio.h>
#include <string.h>
#include "../../peripheral_console/src/profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "rtthread.h"
#include "port_gen_os_driver.h"

#ifdef DETECT_KEY
#error Sorry: feature DETECT_KEY not ported
#endif

#if (INGCHIPS_FAMILY != INGCHIPS_FAMILY_918)
#error WIP: Porting to ING916
#endif

extern const gen_os_driver_t *os_impl_get_driver(void);

#include "../../peripheral_console/src/main_shared.c"

int app_main()
{
    _app_main();

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    platform_config(PLATFORM_CFG_DEEP_SLEEP_TIME_REDUCTION, 4500);
#endif

    platform_config(PLATFORM_CFG_POWER_SAVING, 0);

    return (uintptr_t)os_impl_get_driver();
}

#define expand2(X) #X
#define expand(X) expand2(X)

const char welcome_msg[] = "Built with RT-Thread (" expand(RT_VERSION) "." expand(RT_SUBVERSION) "." expand(RT_REVISION) ")";
