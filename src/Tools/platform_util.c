#include "platform_util.h"
#include "eflash.h"

const struct platform_info *platform_inspect2(uintptr_t binary_addr, int family)
{
    switch (family)
    {
        case INGCHIPS_FAMILY_918:
            return (const struct platform_info *)(binary_addr + 0xb0);
        case INGCHIPS_FAMILY_916:
            {
                uintptr_t vect_addr = io_read(binary_addr + EFLASH_SECTOR_SIZE);
                return (const struct platform_info *)(vect_addr + 0xfc);
            }

        default:
            return (const struct platform_info *)0;
    }
}

const struct platform_info *platform_inspect(uintptr_t binary_addr)
{
    return platform_inspect2(binary_addr, INGCHIPS_FAMILY);
}

uintptr_t platform_get_vector_table_address(void)
{
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    return io_read(FLASH_BASE + EFLASH_SECTOR_SIZE * 3);
#else
    return FLASH_BASE;
#endif
}
