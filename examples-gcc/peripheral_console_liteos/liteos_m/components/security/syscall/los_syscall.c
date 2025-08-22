/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd. All rights reserved.
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

#define GNU_SOURCE
#include "los_syscall.h"
#include "los_context.h"
#include "los_task.h"
#include "los_debug.h"
#include "unistd.h"
#include "errno.h"

#define SYS_CALL_NUM_LIMIT    (__NR_syscallend + 1)
#define SYS_CALL_NUM_REG_OFFSET 7
#define NARG_BITS       4
#define NARG_MASK       0x0F
#define NARG_PER_BYTE   2

typedef UINT32 (*SyscallFun1)(UINT32);
typedef UINT32 (*SyscallFun2)(UINT32, UINT32);
typedef UINT32 (*SyscallFun3)(UINT32, UINT32, UINT32);
typedef UINT32 (*SyscallFun4)(UINT32, UINT32, UINT32, UINT32);
typedef UINT32 (*SyscallFun5)(UINT32, UINT32, UINT32, UINT32, UINT32);
typedef UINT32 (*SyscallFun7)(UINT32, UINT32, UINT32, UINT32, UINT32, UINT32, UINT32);

static UINTPTR g_syscallHandle[SYS_CALL_NUM_LIMIT] = {0};
static UINT8 g_syscallNArgs[(SYS_CALL_NUM_LIMIT + 1) / NARG_PER_BYTE] = {0};

void OsSyscallHandleInit(void)
{
#define SYSCALL_HAND_DEF(id, fun, rType, nArg)                                             \
    if ((id) < SYS_CALL_NUM_LIMIT) {                                                       \
        g_syscallHandle[(id)] = (UINTPTR)(fun);                                            \
        g_syscallNArgs[(id) / NARG_PER_BYTE] |= ((id) & 1) ? (nArg) << NARG_BITS : (nArg); \
    }                                                                                      \

#include "syscall_lookup.h"
#undef SYSCALL_HAND_DEF
}

/* The SYSCALL ID is in R7 on entry. Parameters follow in R0..R6 */
VOID OsSyscallHandle(UINT32 *args)
{
    UINT32 ret;
    UINT8 nArgs;
    UINTPTR handle;
    UINT32 svcNum = (UINT32)args[SYS_CALL_NUM_REG_OFFSET];

    if (svcNum >= SYS_CALL_NUM_LIMIT) {
        PRINT_ERR("Syscall ID: error %d !!!\n", svcNum);
        return;
    }

    handle = g_syscallHandle[svcNum];
    nArgs = g_syscallNArgs[svcNum / NARG_PER_BYTE]; /* 4bit per nargs */
    nArgs = (svcNum & 1) ? (nArgs >> NARG_BITS) : (nArgs & NARG_MASK);
    if ((handle == 0) || (nArgs > ARG_NUM_7)) {
        PRINT_ERR("Unsupported syscall ID: %d nArgs: %d\n", svcNum, nArgs);
        args[ARG_NUM_0] = -ENOSYS;
        return;
    }

    switch (nArgs) {
        case ARG_NUM_0:
        case ARG_NUM_1:
            ret = (*(SyscallFun1)handle)(args[ARG_NUM_0]);
            break;
        case ARG_NUM_2:
            ret = (*(SyscallFun2)handle)(args[ARG_NUM_0], args[ARG_NUM_1]);
            break;
        case ARG_NUM_3:
            ret = (*(SyscallFun3)handle)(args[ARG_NUM_0], args[ARG_NUM_1], args[ARG_NUM_2]);
            break;
        case ARG_NUM_4:
            ret = (*(SyscallFun4)handle)(args[ARG_NUM_0], args[ARG_NUM_1], args[ARG_NUM_2], args[ARG_NUM_3]);
            break;
        case ARG_NUM_5:
            ret = (*(SyscallFun5)handle)(args[ARG_NUM_0], args[ARG_NUM_1], args[ARG_NUM_2], args[ARG_NUM_3], \
                                         args[ARG_NUM_4]);
            break;
        default:
            ret = (*(SyscallFun7)handle)(args[ARG_NUM_0], args[ARG_NUM_1], args[ARG_NUM_2], args[ARG_NUM_3], \
                                         args[ARG_NUM_4], args[ARG_NUM_5], args[ARG_NUM_6]);
    }

    args[ARG_NUM_0] = ret;

    return;
}