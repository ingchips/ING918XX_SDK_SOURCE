#include "platform_util.h"

const struct platform_info *platform_inspect2(uintptr_t binary_addr, int family)
{
    switch (family)
    {
        case INGCHIPS_FAMILY_918:
            return (const struct platform_info *)(binary_addr + 0xb0);
        case INGCHIPS_FAMILY_916:
            return (const struct platform_info *)(binary_addr + 0xfc);
        default:
            return (const struct platform_info *)0;
    }
}

const struct platform_info *platform_inspect(uintptr_t binary_addr)
{
    return platform_inspect2(binary_addr, INGCHIPS_FAMILY);
}
