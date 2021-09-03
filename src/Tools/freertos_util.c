/*
 * Some functions are adapted from FreeRTOS Kernel.
 * 
 * See license below.
 * 
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
 
#include "freertos_util.h"
#include "platform_api.h"
#include "ingsoc.h"
#include "FreeRTOS.h"
#include "task.h"

void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement )
{
TickType_t xTimeToWake;
BaseType_t xAlreadyYielded, xShouldDelay = pdFALSE;

    vTaskSuspendAll();
    {
        /* Minor optimisation.  The tick count cannot change in this
        block. */
        const TickType_t xConstTickCount = xTaskGetTickCount();

        /* Generate the tick time at which the task wants to wake. */
        xTimeToWake = *pxPreviousWakeTime + xTimeIncrement;

        if( xConstTickCount < *pxPreviousWakeTime )
        {
            /* The tick count has overflowed since this function was
            lasted called.  In this case the only time we should ever
            actually delay is if the wake time has also	overflowed,
            and the wake time is greater than the tick time.  When this
            is the case it is as if neither time had overflowed. */
            if( ( xTimeToWake < *pxPreviousWakeTime ) && ( xTimeToWake > xConstTickCount ) )
            {
                xShouldDelay = pdTRUE;
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            /* The tick time has not overflowed.  In this case we will
            delay if either the wake time has overflowed, and/or the
            tick time is less than the wake time. */
            if( ( xTimeToWake < *pxPreviousWakeTime ) || ( xTimeToWake > xConstTickCount ) )
            {
                xShouldDelay = pdTRUE;
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }

        /* Update the wake time ready for the next call. */
        *pxPreviousWakeTime = xTimeToWake;

        if( xShouldDelay != pdFALSE )
        {
            xTaskResumeAll();
            vTaskDelay( xTimeToWake - xConstTickCount );
        }
        else
        {
            xAlreadyYielded = xTaskResumeAll();
        }
    }

    /* Force a reschedule if xTaskResumeAll has not already done so, we may
    have put ourselves to sleep. */
    if( xAlreadyYielded == pdFALSE )
    {
        portYIELD_WITHIN_API();
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
}

#define CALI_TO_Q15(cali) (((uint64_t)(1000 * 65536) << 15) / (cali) / (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ))

static uint32_t tick_per_ms_q15 = CALI_TO_Q15(0x1e8480);

TickType_t AccurateMS_TO_TICKS( uint32_t xTimeInMs )
{
    uint64_t t = xTimeInMs;
    t *= tick_per_ms_q15;
    TickType_t r = (TickType_t)(t >> 15);
    return t & 0x8000 ? r + 1 : r;
}

void vUpdateTicksClockFrequency(void)
{
    uint32_t cali = platform_read_info(PLATFORM_INFO_32K_CALI_VALUE);
    tick_per_ms_q15 = CALI_TO_Q15(cali);
}
