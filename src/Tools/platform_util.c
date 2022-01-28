#include "platform_util.h"

const struct platform_info *platform_inspect(uintptr_t binary_addr)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    return (const struct platform_info *)(binary_addr + 0xb0);
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    return (const struct platform_info *)(binary_addr + 0xbc);
#else
    #error unknow chip family
#endif
}
