#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "../../peripheral_console/src/profile.h"
#include "../../peripheral_console/src/key_detector.h"

extern const void *os_impl_get_driver(void);

#include "../../peripheral_console/src/main_shared.c"

int app_main()
{
    _app_main();

    return (uintptr_t)os_impl_get_driver();
}

const char welcome_msg[] = "Built with FreeRTOS (10.2.1)";
