#include <stdio.h>
#include <string.h>
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "profile.h"

extern const void *os_impl_get_driver(void);

#include "../../peripheral_console/src/main_shared.c"

uintptr_t app_main()
{
    _app_main();

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    // make sure that RAM does not exceed 0x20004000
    // then, we can power off the unused blocks
    SYSCTRL_SelectMemoryBlocks(SYSCTRL_RESERVED_MEM_BLOCKS);
#endif

    return (uintptr_t)os_impl_get_driver();
}

const char welcome_msg[] = "Built without RTOS";
