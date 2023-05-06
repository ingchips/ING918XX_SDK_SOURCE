#include <string.h>

#if (HEAP_OVERRIDE_TYPE == 0)

#include "platform_api.h"
#include "port_gen_os_driver.h"

#define GEN_OS          ((const gen_os_driver_t *)platform_get_gen_os_driver())

#define _override_malloc    GEN_OS->malloc
#define _override_free      GEN_OS->free

#elif (HEAP_OVERRIDE_TYPE == 1)

// IMPORTANT: `malloc`, `free`, etc, CAN'T be used in `app_main`. They are available after
//            `PLATFORM_CB_EVT_PROFILE_INIT` callback is invoked.

#include "ll_api.h"

#define _override_malloc    ll_malloc
#define _override_free      ll_free

#endif

void * malloc(size_t s)
{
    return _override_malloc(s);
}

void free(void * p)
{
    _override_free(p);
}

void * calloc(size_t n, size_t elem)
{
    void * mem = _override_malloc(n * elem);
    if (mem) memset(mem, 0, n * elem);
    return mem;
}

void * realloc(void * p, size_t s)
{
    void * mem = NULL;
    if (s > 0)
    {
        mem = _override_malloc(s);
        if (mem && p) memcpy(mem, p, s);
    }
    if (p) _override_free(p);
    return mem;
}

