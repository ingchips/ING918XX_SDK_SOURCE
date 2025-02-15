/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2023 Huawei Device Co., Ltd. All rights reserved.
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

#ifndef _LOS_ARCH_H
#define _LOS_ARCH_H

#include "los_config.h"
#include "los_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if defined(__ICCARM__) || defined(__CC_ARM)
STATIC INLINE UINTPTR ArchSpGet(VOID)
{
    UINTPTR sp;
    __asm("mov %0, sp" : "=r" (sp));
    return sp;
}

STATIC INLINE UINTPTR ArchPspGet(VOID)
{
    UINTPTR psp;
    __asm("mrs %0, psp" : "=r" (psp));
    return psp;
}

STATIC INLINE UINTPTR ArchMspGet(VOID)
{
    UINTPTR msp;
    __asm("mrs %0, msp" : "=r" (msp));
    return msp;
}

#define ARCH_LR_GET()                           \
({                                              \
    UINTPTR lr;                                 \
    __asm("mov %0, lr" : "=r" (lr));            \
    (lr);                                       \
})
#define ArchLRGet ARCH_LR_GET
#elif defined(__CLANG_ARM) || defined(__GNUC__)
STATIC INLINE UINTPTR ArchSpGet(VOID)
{
    UINTPTR sp;
    __asm volatile("mov %0, sp" : "=r" (sp));
    return sp;
}

STATIC INLINE UINTPTR ArchPspGet(VOID)
{
    UINTPTR psp;
    __asm volatile("mrs %0, psp" : "=r" (psp));
    return psp;
}

STATIC INLINE UINTPTR ArchMspGet(VOID)
{
    UINTPTR msp;
    __asm volatile("mrs %0, msp" : "=r" (msp));
    return msp;
}

#define ARCH_LR_GET()                           \
({                                              \
    UINTPTR lr;                                 \
    __asm volatile("mov %0, lr" : "=r" (lr));   \
    (lr);                                       \
})
#define ArchLRGet ARCH_LR_GET
#else
/* Other platforms to be improved  */
#endif

VOID ArchInit(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_ARCH_H */

