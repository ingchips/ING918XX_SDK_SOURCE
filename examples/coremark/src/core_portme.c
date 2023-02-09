/*
Copyright 2018 Embedded Microprocessor Benchmark Consortium (EEMBC)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Original Author: Shay Gal-on
*/

#include <stdio.h>
#include <stdlib.h>
#include "coremark.h"
#include "ingsoc.h"

#if VALIDATION_RUN
	volatile ee_s32 seed1_volatile=0x3415;
	volatile ee_s32 seed2_volatile=0x3415;
	volatile ee_s32 seed3_volatile=0x66;
#endif
#if PERFORMANCE_RUN
	volatile ee_s32 seed1_volatile=0x0;
	volatile ee_s32 seed2_volatile=0x0;
	volatile ee_s32 seed3_volatile=0x66;
#endif
#if PROFILE_RUN
	volatile ee_s32 seed1_volatile=0x8;
	volatile ee_s32 seed2_volatile=0x8;
	volatile ee_s32 seed3_volatile=0x8;
#endif
	volatile ee_s32 seed4_volatile=ITERATIONS;
	volatile ee_s32 seed5_volatile=0;

#include <time.h>

uint32_t SystemCoreClock =
#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
    PLL_CLK_FREQ
#else
    0
#endif
;

static volatile unsigned int systick_overflows = 0;

extern void SysTick_Handler(void)
{
   systick_overflows++;
}

static void reset_cycle_counter(void)
{
    /* Set the reload value and clear the current value. */
    SysTick->LOAD = 0x00ffffff;
    SysTick->VAL = 0;
    /* Reset the overflow counter */
    systick_overflows = 0;
}

static void start_cycle_counter(void)
{
    /* Enable the SysTick timer and enable the SysTick overflow interrupt */
    SysTick->CTRL |=
        (SysTick_CTRL_CLKSOURCE_Msk |
        SysTick_CTRL_ENABLE_Msk |
        SysTick_CTRL_TICKINT_Msk);
}

static uint64_t get_cycle_counter(void)
{
    unsigned int overflows = systick_overflows;
    /* A systick overflow might happen here */
    unsigned int systick_count = SysTick->VAL;
    /* check if it did and reload the low bit if it did */
    unsigned int new_overflows = systick_overflows;
    if (overflows != new_overflows)
    {
        /* This suffices as long as there is no chance that a second
        overflow can happen because new_overflows was read */
        systick_count = SysTick->VAL;
        overflows = new_overflows;
    }
    /* Recall that the SysTick counter counts down. */
    return (((uint64_t)overflows << 0x18) + (0x00FFFFFF - systick_count));
}

clock_t clock(void)
{
    return (clock_t) ((get_cycle_counter() * CLOCKS_PER_SEC) / SystemCoreClock);
}

void start_time(void)
{
	reset_cycle_counter();
    start_cycle_counter();
}

clock_t stop_timer;

void stop_time(void)
{
	stop_timer = clock();
}

CORE_TICKS get_time(void) {
	return stop_timer;
}

secs_ret time_in_secs(CORE_TICKS ticks) {
    return ((secs_ret)ticks) / CLOCKS_PER_SEC;
}

extern void setup_peripherals(void);

extern int app_main(void);
extern uint32_t __Vectors;

void portable_init(core_portable *p, int *argc, char *argv[])
{
    SCB->VTOR = (uint32_t)&__Vectors;

    app_main();

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)
    SystemCoreClock = SYSCTRL_GetHClk();
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#else
    #error unknown or unsupported chip family
#endif

    __enable_irq();

	p->portable_id=1;
    printf("\n"
           "============================\n"
           "*      Run Coremark        *\n"
           "*    CPU @ %10dHz    *\n"
           "*     Wait about 30s...    *\n"
           "============================\n\n", SystemCoreClock);
}

void portable_fini(core_portable *p)
{
	p->portable_id=0;
}

ee_u32 default_num_contexts=1;
