#include "platform_util.h"

const struct platform_info *platform_inspect(uintptr_t binary_addr)
{
    return (const struct platform_info *)(binary_addr + 0xb0);
}
