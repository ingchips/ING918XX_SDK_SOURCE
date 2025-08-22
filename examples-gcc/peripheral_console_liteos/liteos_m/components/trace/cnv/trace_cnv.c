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

#include "trace_cnv.h"
#include "los_trace.h"
#include "los_task.h"
#include "los_sem.h"
#include "los_mux.h"
#include "los_queue.h"
#include "los_event.h"
#include "los_swtmr.h"
#include "los_hook.h"

STATIC VOID LOS_TraceMemInit(VOID *pool, UINT32 size)
{
    LOS_TRACE(MEM_INFO_REQ, pool);
}

STATIC VOID LOS_TraceMemAlloc(VOID *pool, VOID *ptr, UINT32 size)
{
    LOS_TRACE(MEM_ALLOC, pool, (UINTPTR)ptr, size);
}

STATIC VOID LOS_TraceMemFree(VOID *pool, VOID *ptr)
{
    LOS_TRACE(MEM_FREE, pool, (UINTPTR)ptr);
}

STATIC VOID LOS_TraceMemRealloc(VOID *pool, VOID *ptr, UINT32 size)
{
    LOS_TRACE(MEM_REALLOC, pool, (UINTPTR)ptr, size);
}

STATIC VOID LOS_TraceMemAllocAlign(VOID *pool, VOID *ptr, UINT32 size, UINT32 boundary)
{
    LOS_TRACE(MEM_ALLOC_ALIGN, pool, (UINTPTR)ptr, size, boundary);
}

STATIC VOID LOS_TraceEventInit(PEVENT_CB_S eventCB)
{
    LOS_TRACE(EVENT_CREATE, (UINTPTR)eventCB);
}

STATIC VOID LOS_TraceEventRead(PEVENT_CB_S eventCB, UINT32 eventMask, UINT32 mode, UINT32 timeout)
{
    LOS_TRACE(EVENT_READ, (UINTPTR)eventCB, eventCB->uwEventID, eventMask, mode, timeout);
}

STATIC VOID LOS_TraceEventWrite(PEVENT_CB_S eventCB, UINT32 events)
{
    LOS_TRACE(EVENT_WRITE, (UINTPTR)eventCB, eventCB->uwEventID, events);
}

STATIC VOID LOS_TraceEventClear(PEVENT_CB_S eventCB, UINT32 events)
{
    LOS_TRACE(EVENT_CLEAR, (UINTPTR)eventCB, eventCB->uwEventID, events);
}

STATIC VOID LOS_TraceEventDestroy(PEVENT_CB_S eventCB)
{
    LOS_TRACE(EVENT_DELETE, (UINTPTR)eventCB, LOS_OK);
}

STATIC VOID LOS_TraceQueueCreate(const LosQueueCB *queueCB)
{
    LOS_TRACE(QUEUE_CREATE, queueCB->queueID, queueCB->queueLen, queueCB->queueSize - sizeof(UINT32),
                (UINTPTR)queueCB, 0);
}

STATIC VOID LOS_TraceQueueRW(const LosQueueCB *queueCB, UINT32 operateType,
                    UINT32 bufferSize, UINT32 timeout)
{
    LOS_TRACE(QUEUE_RW, queueCB->queueID, queueCB->queueSize, bufferSize, operateType,
                queueCB->readWriteableCnt[OS_QUEUE_READ], queueCB->readWriteableCnt[OS_QUEUE_WRITE], timeout);
}

STATIC VOID LOS_TraceQueueDelete(const LosQueueCB *queueCB)
{
    LOS_TRACE(QUEUE_DELETE, queueCB->queueID, queueCB->queueState, queueCB->readWriteableCnt[OS_QUEUE_READ]);
}

STATIC VOID LOS_TraceSemCreate(const LosSemCB *semCB)
{
    LOS_TRACE(SEM_CREATE, semCB->semID, 0, semCB->semCount);
}

STATIC VOID LOS_TraceSemPost(const LosSemCB *semCB, const LosTaskCB *resumedTask)
{
    (VOID)resumedTask;
    LOS_TRACE(SEM_POST, semCB->semID, 0, semCB->semCount);
}

STATIC VOID LOS_TraceSemPend(const LosSemCB *semCB, const LosTaskCB *runningTask, UINT32 timeout)
{
    (VOID)runningTask;
    LOS_TRACE(SEM_PEND, semCB->semID, semCB->semCount, timeout);
}

STATIC VOID LOS_TraceSemDelete(const LosSemCB *semCB)
{
    LOS_TRACE(SEM_DELETE, semCB->semID, LOS_OK);
}

STATIC VOID LOS_TraceMuxCreate(const LosMuxCB *muxCB)
{
    LOS_TRACE(MUX_CREATE, muxCB->muxID);
}

STATIC VOID LOS_TraceMuxPost(const LosMuxCB *muxCB)
{
    LOS_TRACE(MUX_POST, muxCB->muxID, muxCB->muxCount,
                (muxCB->owner == NULL) ? 0xffffffff : muxCB->owner->taskID);
}

STATIC VOID LOS_TraceMuxPend(const LosMuxCB *muxCB, UINT32 timeout)
{
    LOS_TRACE(MUX_PEND, muxCB->muxID, muxCB->muxCount,
                (muxCB->owner == NULL) ? 0xffffffff : muxCB->owner->taskID, timeout);
}

STATIC VOID LOS_TraceMuxDelete(const LosMuxCB *muxCB)
{
    LOS_TRACE(MUX_DELETE, muxCB->muxID, muxCB->muxStat, muxCB->muxCount,
                (muxCB->owner == NULL) ? 0xffffffff : muxCB->owner->taskID);
}

STATIC VOID LOS_TraceTaskCreate(const LosTaskCB *taskCB)
{
    LOS_TRACE(TASK_CREATE, taskCB->taskID, taskCB->taskStatus, taskCB->priority);
}

STATIC VOID LOS_TraceTaskPriModify(const LosTaskCB *taskCB, UINT32 prio)
{
    LOS_TRACE(TASK_PRIOSET, taskCB->taskID, taskCB->taskStatus, taskCB->priority, prio);
}

STATIC VOID LOS_TraceTaskDelete(const LosTaskCB *taskCB)
{
    LOS_TRACE(TASK_DELETE, taskCB->taskID, taskCB->taskStatus, (UINTPTR)taskCB->stackPointer);
}

STATIC VOID LOS_TraceTaskSwitchedIn(VOID)
{
    LosTaskCB *newTask = g_losTask.newTask;
    LosTaskCB *runTask = g_losTask.runTask;
    LOS_TRACE(TASK_SWITCH, newTask->taskID, runTask->priority, runTask->taskStatus,
        newTask->priority, newTask->taskStatus);
}

STATIC VOID LOS_TraceTaskResume(const LosTaskCB *taskCB)
{
    LOS_TRACE(TASK_RESUME, taskCB->taskID, taskCB->taskStatus, taskCB->priority);
}

STATIC VOID LOS_TraceTaskSuspend(const LosTaskCB *taskCB)
{
    LOS_TRACE(TASK_SUSPEND, taskCB->taskID, taskCB->taskStatus, g_losTask.runTask->taskID);
}

STATIC VOID LOS_TraceIsrEnter(UINT32 hwiNum)
{
    LOS_TRACE(HWI_RESPONSE_IN, hwiNum);
}

STATIC VOID LOS_TraceIsrExit(UINT32 hwiNum)
{
    LOS_TRACE(HWI_RESPONSE_OUT, hwiNum);
}

STATIC VOID LOS_TraceSwtmrCreate(const SWTMR_CTRL_S *swtmr)
{
    LOS_TRACE(SWTMR_CREATE, swtmr->usTimerID);
}

STATIC VOID LOS_TraceSwtmrDelete(const SWTMR_CTRL_S *swtmr)
{
    LOS_TRACE(SWTMR_DELETE, swtmr->usTimerID);
}

STATIC VOID LOS_TraceSwtmrExpired(const SWTMR_CTRL_S *swtmr)
{
    LOS_TRACE(SWTMR_EXPIRED, swtmr->usTimerID);
}

STATIC VOID LOS_TraceSwtmrStart(const SWTMR_CTRL_S *swtmr)
{
    LOS_TRACE(SWTMR_START, swtmr->usTimerID, swtmr->ucMode, swtmr->uwInterval);
}

STATIC VOID LOS_TraceSwtmrStop(const SWTMR_CTRL_S *swtmr)
{
    LOS_TRACE(SWTMR_STOP, swtmr->usTimerID);
}

VOID OsTraceCnvInit(VOID)
{
    LOS_HookReg(LOS_HOOK_TYPE_MEM_ALLOC, LOS_TraceMemAlloc);
    LOS_HookReg(LOS_HOOK_TYPE_MEM_FREE, LOS_TraceMemFree);
    LOS_HookReg(LOS_HOOK_TYPE_MEM_INIT, LOS_TraceMemInit);
    LOS_HookReg(LOS_HOOK_TYPE_MEM_REALLOC, LOS_TraceMemRealloc);
    LOS_HookReg(LOS_HOOK_TYPE_MEM_ALLOCALIGN, LOS_TraceMemAllocAlign);
    LOS_HookReg(LOS_HOOK_TYPE_EVENT_INIT, LOS_TraceEventInit);
    LOS_HookReg(LOS_HOOK_TYPE_EVENT_READ, LOS_TraceEventRead);
    LOS_HookReg(LOS_HOOK_TYPE_EVENT_WRITE, LOS_TraceEventWrite);
    LOS_HookReg(LOS_HOOK_TYPE_EVENT_CLEAR, LOS_TraceEventClear);
    LOS_HookReg(LOS_HOOK_TYPE_EVENT_DESTROY, LOS_TraceEventDestroy);
    LOS_HookReg(LOS_HOOK_TYPE_QUEUE_CREATE, LOS_TraceQueueCreate);
    LOS_HookReg(LOS_HOOK_TYPE_QUEUE_DELETE, LOS_TraceQueueDelete);
    LOS_HookReg(LOS_HOOK_TYPE_QUEUE_READ, LOS_TraceQueueRW);
    LOS_HookReg(LOS_HOOK_TYPE_QUEUE_READ_COPY, LOS_TraceQueueRW);
    LOS_HookReg(LOS_HOOK_TYPE_QUEUE_WRITE, LOS_TraceQueueRW);
    LOS_HookReg(LOS_HOOK_TYPE_QUEUE_WRITE_COPY, LOS_TraceQueueRW);
    LOS_HookReg(LOS_HOOK_TYPE_SEM_CREATE, LOS_TraceSemCreate);
    LOS_HookReg(LOS_HOOK_TYPE_SEM_DELETE, LOS_TraceSemDelete);
    LOS_HookReg(LOS_HOOK_TYPE_SEM_POST, LOS_TraceSemPost);
    LOS_HookReg(LOS_HOOK_TYPE_SEM_PEND, LOS_TraceSemPend);
    LOS_HookReg(LOS_HOOK_TYPE_MUX_CREATE, LOS_TraceMuxCreate);
    LOS_HookReg(LOS_HOOK_TYPE_MUX_POST, LOS_TraceMuxPost);
    LOS_HookReg(LOS_HOOK_TYPE_MUX_PEND, LOS_TraceMuxPend);
    LOS_HookReg(LOS_HOOK_TYPE_MUX_DELETE, LOS_TraceMuxDelete);
    LOS_HookReg(LOS_HOOK_TYPE_TASK_PRIMODIFY, LOS_TraceTaskPriModify);
    LOS_HookReg(LOS_HOOK_TYPE_TASK_DELETE, LOS_TraceTaskDelete);
    LOS_HookReg(LOS_HOOK_TYPE_TASK_CREATE, LOS_TraceTaskCreate);
    LOS_HookReg(LOS_HOOK_TYPE_TASK_SWITCHEDIN, LOS_TraceTaskSwitchedIn);
    LOS_HookReg(LOS_HOOK_TYPE_MOVEDTASKTOREADYSTATE, LOS_TraceTaskResume);
    LOS_HookReg(LOS_HOOK_TYPE_MOVEDTASKTOSUSPENDEDLIST, LOS_TraceTaskSuspend);
    LOS_HookReg(LOS_HOOK_TYPE_ISR_ENTER, LOS_TraceIsrEnter);
    LOS_HookReg(LOS_HOOK_TYPE_ISR_EXIT, LOS_TraceIsrExit);
    LOS_HookReg(LOS_HOOK_TYPE_SWTMR_CREATE, LOS_TraceSwtmrCreate);
    LOS_HookReg(LOS_HOOK_TYPE_SWTMR_DELETE, LOS_TraceSwtmrDelete);
    LOS_HookReg(LOS_HOOK_TYPE_SWTMR_EXPIRED, LOS_TraceSwtmrExpired);
    LOS_HookReg(LOS_HOOK_TYPE_SWTMR_START, LOS_TraceSwtmrStart);
    LOS_HookReg(LOS_HOOK_TYPE_SWTMR_STOP, LOS_TraceSwtmrStop);
}
