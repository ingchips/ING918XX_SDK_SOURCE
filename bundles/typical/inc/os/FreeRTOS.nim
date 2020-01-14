type
  TickType_t* = uint32
  portCHAR* = char
  portFLOAT* = cfloat
  portDOUBLE* = cdouble
  portLONG* = clong
  portSHORT* = cshort
  portSTACK_TYPE* = uint32
  portBASE_TYPE* = clong
  configSTACK_DEPTH_TYPE* = uint16
  StackType_t* = portSTACK_TYPE
  BaseType_t* = clong
  UBaseType_t* = culong

const
  configSYSTICK_CLOCK_HZ* = 32768
  configCPU_CLOCK_HZ* = (cast[culong](48000000))
  configTICK_RATE_HZ* = (cast[TickType_t](1000))
  configMAX_PRIORITIES* = (15)
  configMINIMAL_STACK_SIZE* = (cast[cushort](128))
  configMAX_TASK_NAME_LEN* = (16)


const
  pdFALSE* = (cast[UBaseType_t](0))
  pdTRUE* = (cast[UBaseType_t](1))
  pdPASS* = (pdTRUE)
  pdFAIL* = (pdFALSE)
  errQUEUE_EMPTY* = (cast[BaseType_t](0))
  errQUEUE_FULL* = (cast[BaseType_t](0))

  portMAX_DELAY* = 0xffffffffu32

template pdMS_TO_TICKS*(xTimeInMs: int32): TickType_t =
    cast[TickType_t]((cast[uint32](xTimeInMs) * cast[uint32](configTICK_RATE_HZ)) div 1000)

type
  List_t* {.importc: "List_t", header: "FreeRTOSNim.h", bycopy.} = object


##
##  Used internally only.
##

type
  TimeOut_t* {.importc: "TimeOut_t", header: "FreeRTOSNim.h", bycopy.} = object
    xOverflowCount* {.importc: "xOverflowCount".}: BaseType_t
    xTimeOnEntering* {.importc: "xTimeOnEntering".}: TickType_t


##  Possible return values for eTaskConfirmSleepModeStatus().

type
  eSleepModeStatus* {.size: sizeof(cint).} = enum
    eAbortSleep = 0,            ##  A task has been made ready or a context switch pended since portSUPPORESS_TICKS_AND_SLEEP() was called - abort entering a sleep mode.
    eStandardSleep,           ##  Enter a sleep mode that will not last any longer than the expected idle time.
    eNoTasksWaitingTimeout    ##  No tasks are waiting for a timeout so it is safe to enter a sleep mode that can only be exited by an external interrupt.


type
  tskTaskControlBlock* {.importc: "tskTaskControlBlock", header: "FreeRTOSNim.h",
                        bycopy.} = object


##  The old naming convention is used to prevent breaking kernel aware debuggers.

type
  TaskHandle_t* = ptr tskTaskControlBlock

## *
##  Type by which software timers are referenced.  For example, a call to
##  xTimerCreate() returns an TimerHandle_t variable that can then be used to
##  reference the subject timer in calls to other software timer API functions
##  (for example, xTimerStart(), xTimerReset(), etc.).
##

type
  tmrTimerControl* {.importc: "struct tmrTimerControl", header: "FreeRTOSNim.h", bycopy.} = object


##  The old naming convention is used to prevent breaking kernel aware debuggers.

type
  TimerHandle_t* = ptr tmrTimerControl

## *
##  Type by which queues are referenced.  For example, a call to xQueueCreate()
##  returns an QueueHandle_t variable that can then be used as a parameter to
##  xQueueSend(), xQueueReceive(), etc.
##

type
  QueueDefinition* {.importc: "QueueDefinition", header: "FreeRTOSNim.h", bycopy.} = object


##  Using old naming convention so as not to break kernel aware debuggers.

type
  QueueHandle_t* = ptr QueueDefinition

## *
##  Type by which queue sets are referenced.  For example, a call to
##  xQueueCreateSet() returns an xQueueSet variable that can then be used as a
##  parameter to xQueueSelectFromSet(), xQueueAddToSet(), etc.
##

type
  QueueSetHandle_t* = ptr QueueDefinition

## *
##  Queue sets can contain both queues and semaphores, so the
##  QueueSetMemberHandle_t is defined as a type to be used where a parameter or
##  return value can be either an QueueHandle_t or an SemaphoreHandle_t.
##

type
  QueueSetMemberHandle_t* = ptr QueueDefinition

##
##  Defines the prototype to which task functions must conform.  Defined in this
##  file to ensure the type is known before portable.h is included.
##

type
  TaskFunction_t* = proc (a1: pointer) {.noconv.}

##
##  Defines the prototype to which timer callback functions must conform.
##

type
  TimerCallbackFunction_t* = proc (xTimer: TimerHandle_t) {.noconv.}

##
##  In line with software engineering best practice, especially when supplying a
##  library that is likely to change in future versions, FreeRTOS implements a
##  strict data hiding policy.  This means the software timer structure used
##  internally by FreeRTOS is not accessible to application code.  However, if
##  the application writer wants to statically allocate the memory required to
##  create a software timer then the size of the queue object needs to be know.
##  The StaticTimer_t structure below is provided for this purpose.  Its sizes
##  and alignment requirements are guaranteed to match those of the genuine
##  structure, no matter which architecture is being used, and no matter how the
##  values in FreeRTOSConfig.h are set.  Its contents are somewhat obfuscated in
##  the hope users will recognise that it would be unwise to make direct use of
##  the structure members.
##

const
  STATIC_TIMER_SIZE* = 44
  STATIC_QUEUE_SIZE* = 76
  STATIC_TASK_SIZE* = 84

type
  StaticQueue_t* {.importc: "StaticQueue_t", header: "FreeRTOSNim.h", bycopy.} = object
    dummy* {.importc: "dummy".}: array[STATIC_QUEUE_SIZE, uint8]

  StaticTask_t* {.importc: "StaticTask_t", header: "FreeRTOSNim.h", bycopy.} = object
    dummy* {.importc: "dummy".}: array[STATIC_TASK_SIZE, uint8]

  StaticTimer_t* {.importc: "StaticTimer_t", header: "FreeRTOSNim.h", bycopy.} = object
    dummy* {.importc: "dummy".}: array[STATIC_TIMER_SIZE, uint8]


proc xQueueGenericSend*(xQueue: QueueHandle_t; pvItemToQueue: pointer;
                       xTicksToWait: TickType_t; xCopyPosition: BaseType_t): BaseType_t {.
    importc: "xQueueGenericSend", header: "FreeRTOSNim.h".}
proc xQueueReceive*(xQueue: QueueHandle_t; pvBuffer: pointer;
                   xTicksToWait: TickType_t): BaseType_t {.
    importc: "xQueueReceive", header: "FreeRTOSNim.h".}
proc xQueueGenericSendFromISR*(xQueue: QueueHandle_t; pvItemToQueue: pointer;
                              pxHigherPriorityTaskWoken: ptr BaseType_t;
                              xCopyPosition: BaseType_t): BaseType_t {.
    importc: "xQueueGenericSendFromISR", header: "FreeRTOSNim.h".}
proc xQueueGiveFromISR*(xQueue: QueueHandle_t;
                       pxHigherPriorityTaskWoken: ptr BaseType_t): BaseType_t {.
    importc: "xQueueGiveFromISR", header: "FreeRTOSNim.h".}
proc xQueueSemaphoreTake*(xQueue: QueueHandle_t; xTicksToWait: TickType_t): BaseType_t {.
    importc: "xQueueSemaphoreTake", header: "FreeRTOSNim.h".}
proc xQueueGenericCreate*(uxQueueLength: UBaseType_t; uxItemSize: UBaseType_t;
                         ucQueueType: uint8): QueueHandle_t {.
    importc: "xQueueGenericCreate", header: "FreeRTOSNim.h".}
proc xQueueGenericCreateStatic*(uxQueueLength: UBaseType_t;
                               uxItemSize: UBaseType_t;
                               pucQueueStorage: ptr uint8;
                               pxStaticQueue: ptr StaticQueue_t; ucQueueType: uint8): QueueHandle_t {.
    importc: "xQueueGenericCreateStatic", header: "FreeRTOSNim.h".}
proc vQueueWaitForMessageRestricted*(xQueue: QueueHandle_t;
                                    xTicksToWait: TickType_t;
                                    xWaitIndefinitely: BaseType_t) {.
    importc: "vQueueWaitForMessageRestricted", header: "FreeRTOSNim.h".}
proc xQueueGenericReset*(xQueue: QueueHandle_t; xNewQueue: BaseType_t): BaseType_t {.
    importc: "xQueueGenericReset", header: "FreeRTOSNim.h".}
proc xTaskCreate*(pxTaskCode: TaskFunction_t; pcName: cstring;
                 usStackDepth: configSTACK_DEPTH_TYPE; pvParameters: pointer;
                 uxPriority: UBaseType_t; pxCreatedTask: ptr TaskHandle_t): BaseType_t {.
    importc: "xTaskCreate", header: "FreeRTOSNim.h".}
  ## lint !e971 Unqualified char types are allowed for strings and single characters only.
proc xTaskCreateStatic*(pxTaskCode: TaskFunction_t; pcName: cstring;
                       ulStackDepth: uint32; pvParameters: pointer;
                       uxPriority: UBaseType_t; puxStackBuffer: ptr StackType_t;
                       pxTaskBuffer: ptr StaticTask_t): TaskHandle_t {.
    importc: "xTaskCreateStatic", header: "FreeRTOSNim.h".}
  ## lint !e971 Unqualified char types are allowed for strings and single characters only.
proc vTaskDelay*(xTicksToDelay: TickType_t) {.importc: "vTaskDelay",
    header: "FreeRTOSNim.h".}
proc vTaskStartScheduler*() {.importc: "vTaskStartScheduler",
                            header: "FreeRTOSNim.h".}
proc vTaskSuspendAll*() {.importc: "vTaskSuspendAll", header: "FreeRTOSNim.h".}
proc xTaskResumeAll*(): BaseType_t {.importc: "xTaskResumeAll",
                                  header: "FreeRTOSNim.h".}
proc xTaskGetTickCount*(): TickType_t {.importc: "xTaskGetTickCount",
                                     header: "FreeRTOSNim.h".}
proc xTaskIncrementTick*(): BaseType_t {.importc: "xTaskIncrementTick",
                                      header: "FreeRTOSNim.h".}
proc vTaskPlaceOnEventList*(pxEventList: ptr List_t; xTicksToWait: TickType_t) {.
    importc: "vTaskPlaceOnEventList", header: "FreeRTOSNim.h".}
proc vTaskPlaceOnEventListRestricted*(pxEventList: ptr List_t;
                                     xTicksToWait: TickType_t;
                                     xWaitIndefinitely: BaseType_t) {.
    importc: "vTaskPlaceOnEventListRestricted", header: "FreeRTOSNim.h".}
proc xTaskRemoveFromEventList*(pxEventList: ptr List_t): BaseType_t {.
    importc: "xTaskRemoveFromEventList", header: "FreeRTOSNim.h".}
proc vTaskSwitchContext*() {.importc: "vTaskSwitchContext", header: "FreeRTOSNim.h".}
proc xTaskCheckForTimeOut*(pxTimeOut: ptr TimeOut_t; pxTicksToWait: ptr TickType_t): BaseType_t {.
    importc: "xTaskCheckForTimeOut", header: "FreeRTOSNim.h".}
proc vTaskMissedYield*() {.importc: "vTaskMissedYield", header: "FreeRTOSNim.h".}
proc xTaskGetSchedulerState*(): BaseType_t {.importc: "xTaskGetSchedulerState",
    header: "FreeRTOSNim.h".}
proc xTaskPriorityInherit*(pxMutexHolder: TaskHandle_t): BaseType_t {.
    importc: "xTaskPriorityInherit", header: "FreeRTOSNim.h".}
proc xTaskPriorityDisinherit*(pxMutexHolder: TaskHandle_t): BaseType_t {.
    importc: "xTaskPriorityDisinherit", header: "FreeRTOSNim.h".}
proc vTaskPriorityDisinheritAfterTimeout*(pxMutexHolder: TaskHandle_t;
    uxHighestPriorityWaitingTask: UBaseType_t) {.
    importc: "vTaskPriorityDisinheritAfterTimeout", header: "FreeRTOSNim.h".}
proc vTaskStepTick*(xTicksToJump: TickType_t) {.importc: "vTaskStepTick",
    header: "FreeRTOSNim.h".}
proc eTaskConfirmSleepModeStatus*(): eSleepModeStatus {.
    importc: "eTaskConfirmSleepModeStatus", header: "FreeRTOSNim.h".}
proc pvTaskIncrementMutexHeldCount*(): TaskHandle_t {.
    importc: "pvTaskIncrementMutexHeldCount", header: "FreeRTOSNim.h".}
proc vTaskInternalSetTimeOutState*(pxTimeOut: ptr TimeOut_t) {.
    importc: "vTaskInternalSetTimeOutState", header: "FreeRTOSNim.h".}
proc xTimerCreate*(pcTimerName: cstring; xTimerPeriodInTicks: TickType_t;
                  uxAutoReload: UBaseType_t; pvTimerID: pointer;
                  pxCallbackFunction: TimerCallbackFunction_t): TimerHandle_t {.
    importc: "xTimerCreate", header: "FreeRTOSNim.h".}
  ## lint !e971 Unqualified char types are allowed for strings and single characters only.
proc xTimerCreateStatic*(pcTimerName: cstring; xTimerPeriodInTicks: TickType_t;
                        uxAutoReload: UBaseType_t; pvTimerID: pointer;
                        pxCallbackFunction: TimerCallbackFunction_t;
                        pxTimerBuffer: ptr StaticTimer_t): TimerHandle_t {.
    importc: "xTimerCreateStatic", header: "FreeRTOSNim.h".}
  ## lint !e971 Unqualified char types are allowed for strings and single characters only.
proc pvTimerGetTimerID*(xTimer: TimerHandle_t): pointer {.
    importc: "pvTimerGetTimerID", header: "FreeRTOSNim.h".}
proc xTimerCreateTimerTask*(): BaseType_t {.importc: "xTimerCreateTimerTask",
    header: "FreeRTOSNim.h".}
proc xTimerGenericCommand*(xTimer: TimerHandle_t; xCommandID: BaseType_t;
                          xOptionalValue: TickType_t;
                          pxHigherPriorityTaskWoken: ptr BaseType_t;
                          xTicksToWait: TickType_t): BaseType_t {.
    importc: "xTimerGenericCommand", header: "FreeRTOSNim.h".}
proc pxPortInitialiseStack*(pxTopOfStack: ptr StackType_t; pxCode: TaskFunction_t;
                           pvParameters: pointer): ptr StackType_t {.
    importc: "pxPortInitialiseStack", header: "FreeRTOSNim.h".}
proc pvPortMalloc*(xSize: csize): pointer {.importc: "pvPortMalloc",
                                        header: "FreeRTOSNim.h".}
proc vPortFree*(pv: pointer) {.importc: "vPortFree", header: "FreeRTOSNim.h".}
proc xPortStartScheduler*(): BaseType_t {.importc: "xPortStartScheduler",
                                       header: "FreeRTOSNim.h".}

const tmrCOMMAND_START = cast[BaseType_t](1)
const tmrCOMMAND_RESET = cast[BaseType_t](2)
const tmrCOMMAND_STOP = cast[BaseType_t](3)

template xTimerReset*(xTimer: TimerHandle_t, xTicksToWait: uint32): BaseType_t =
  xTimerGenericCommand(xTimer, tmrCOMMAND_RESET, xTaskGetTickCount(), nil, xTicksToWait)

template xTimerStop*(xTimer: TimerHandle_t, xTicksToWait: uint32): BaseType_t =
  xTimerGenericCommand(xTimer, tmrCOMMAND_STOP, 0, nil, xTicksToWait)