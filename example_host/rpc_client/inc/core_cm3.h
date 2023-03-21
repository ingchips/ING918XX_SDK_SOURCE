#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#define __IO volatile

#define SCB_ICSR_VECTACTIVE_Msk 1

struct tagSCB
{
    uint32_t ICSR;
};


extern struct tagSCB __dummy;

#define SCB     (&__dummy)

#ifdef __cplusplus
}
#endif
