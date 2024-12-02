/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**@defgroup los_config System configuration items
 * @ingroup kernel
 */

#ifndef _TARGET_CONFIG_H
#define _TARGET_CONFIG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */
#include "ingsoc.h"

/*=============================================================================
                                        System clock module configuration
=============================================================================*/
#if 0
#define OS_SYS_CLOCK                                        112000000
#define LOSCFG_BASE_CORE_TICK_PER_SECOND                    (1024UL)
#else
#define OS_SYS_CLOCK                                        32768
#define LOSCFG_BASE_CORE_TICK_PER_SECOND                    (1024UL)
#endif
#define LOSCFG_BASE_CORE_TICK_PER_SECOND_MINI               (2000UL)
#define LOSCFG_BASE_CORE_TICK_HW_TIME                       0
#define LOSCFG_BASE_CORE_TICK_WTIMER                        0
#define LOSCFG_BASE_CORE_TICK_RESPONSE_MAX                  OS_SYS_CLOCK/LOSCFG_BASE_CORE_TICK_PER_SECOND

/*=============================================================================
                                        Hardware interrupt module configuration
=============================================================================*/
#define LOSCFG_PLATFORM_HWI                                 0
#define LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT                 0
#define LOSCFG_PLATFORM_HWI_LIMIT                           128
/*=============================================================================
                                       Task module configuration
=============================================================================*/
#define LOSCFG_BASE_CORE_TSK_LIMIT                          16
#define LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE             (256 * 8)
#define LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE                 (128 * 4)
#define LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE                (LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE)
#define LOSCFG_BASE_CORE_TIMESLICE                          1
#define LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT                  1000
#define LOSCFG_SYS_HEAP_SIZE                                0x5000
/*=============================================================================
                                       Semaphore module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_SEM                                 1
#define LOSCFG_BASE_IPC_SEM_LIMIT                           5
/*=============================================================================
                                       Mutex module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_MUX                                 1
#define LOSCFG_BASE_IPC_MUX_LIMIT                           5
/*=============================================================================
                                       Queue module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_QUEUE                               1
#define LOSCFG_BASE_IPC_QUEUE_LIMIT                         5
/*=============================================================================
                                       Software timer module configuration
=============================================================================*/
#define LOSCFG_BASE_CORE_SWTMR                              1
#define LOSCFG_BASE_CORE_SWTMR_ALIGN                        0
#define LOSCFG_BASE_CORE_SWTMR_LIMIT                        5
/*=============================================================================
                                       Memory module configuration
=============================================================================*/
#define LOSCFG_MEM_MUL_POOL                                 0
#define OS_SYS_MEM_NUM                                      0
/*=============================================================================
                                       Exception module configuration
=============================================================================*/
#define LOSCFG_PLATFORM_EXC                                 0
/* =============================================================================
                                       printf module configuration
============================================================================= */
#define LOSCFG_KERNEL_PRINTF                                0

#define LOSCFG_BASE_CORE_SCHED_SLEEP                        0

#define LOSCFG_SHELL_STACK_SIZE                             4096


/**
 * The Version number of Public
 */
#define MAJ_V                                   5
#define MIN_V                                   0
#define REL_V                                   0

/**
 * The release candidate version number
 */
#define EXTRA_V                                 0

#define VERSION_NUM(a, b, c)                    (((a) << 16) | ((b) << 8) | (c))
#define HW_LITEOS_OPEN_VERSION_NUM              VERSION_NUM(MAJ_V, MIN_V, REL_V)

#define STRINGIFY_1(x)                          #x
#define STRINGIFY(x)                            STRINGIFY_1(x)

#define HW_LITEOS_OPEN_VERSION_STRING           STRINGIFY(MAJ_V) "." STRINGIFY(MIN_V) "." STRINGIFY(REL_V)
#if (EXTRA_V != 0)
#define HW_LITEOS_KERNEL_VERSION_STRING         HW_LITEOS_OPEN_VERSION_STRING "-rc" STRINGIFY(EXTRA_V)
#else
#define HW_LITEOS_KERNEL_VERSION_STRING         HW_LITEOS_OPEN_VERSION_STRING
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _TARGET_CONFIG_H */
