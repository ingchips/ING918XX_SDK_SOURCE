#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include <string.h>

#define _override_malloc    pvPortMalloc
#define _override_free      vPortFree

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
