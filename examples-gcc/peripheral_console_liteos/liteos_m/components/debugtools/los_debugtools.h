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

#ifndef _LOS_DEBUGTOOLS_H
#define _LOS_DEBUGTOOLS_H

#include "los_config.h"
#include "los_task.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


/* StackDump print format */
#define PRINT_PER_ROW  4

/* SchedTrace records number */
#define TRACE_NUM   200

/* sched trace info, users can add their own */
typedef struct {
    UINT32 newTaskID;
    CHAR newTaskName[LOS_TASK_NAMELEN];
    UINT32 runTaskID;
    CHAR runTaskName[LOS_TASK_NAMELEN];
} SchedTraceInfo;

/* SchedTrace record callback */
typedef void (*SchedTraceRecordCB)(LosTaskCB *newTask, LosTaskCB *runTask);

/* SchedTrace show callback, buf is overwrite ringbuf, max amount of storage is TRACE_NUM */
typedef void (*SchedTraceShowCB)(SchedTraceInfo *ringBuf, UINT32 count);

/* Shell callback */
extern UINT32 OsShellCmdStackDump(INT32 argc, const CHAR **argv);
extern UINT32 OsShellCmdHwiDump(INT32 argc, const CHAR **argv);
extern UINT32 OsShellCmdSchedTrace(INT32 argc, const CHAR **argv);

/* Other module callback */
extern VOID OsSchedTraceRecord(LosTaskCB *newTask, LosTaskCB *runTask);

/* External interface */
/* dump stack by task id, can be called at any time */
extern VOID LOS_TaskStackDump(UINT32 taskID);

/* register sched trace handle, If not registered, the default fun will be used */
extern VOID LOS_SchedTraceHandleRegister(SchedTraceRecordCB recordCB, SchedTraceShowCB showCB);
/* start sched trace, will alloc buf, and start write to buf when sched */
extern VOID LOS_SchedTraceStart(VOID);
/* stop sched trace, will stop record and free buf */
extern VOID LOS_SchedTraceStop(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
