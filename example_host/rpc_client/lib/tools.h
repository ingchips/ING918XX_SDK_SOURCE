#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void sleep_ms(uint32_t ms);
uint64_t get_sys_ms_time(void);

class Timing
{
    public:
        uint32_t Diff() // difference in ms
        {
            uint64_t now = get_sys_ms_time();
            uint32_t r = now - last;
            last = now;
            return r;
        }
    private:
        uint32_t last;
};

#ifdef __cplusplus
}
#endif
