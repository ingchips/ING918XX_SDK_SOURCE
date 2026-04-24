#include <stdio.h>
#include <string.h>
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "port_gen_os_driver.h"
#include "target_config.h"
#include "trace.h"

#ifdef DETECT_KEY
#error Sorry: feature DETECT_KEY not ported
#endif

#include "../../../examples/peripheral_console/src/main_shared.c"

const char welcome_msg[] = "Built with LiteOS (" HW_LITEOS_KERNEL_VERSION_STRING ")";

extern const gen_os_driver_t *os_impl_get_driver(void);

uintptr_t app_main()
{
    _app_main();

    return (uintptr_t)os_impl_get_driver();
}
