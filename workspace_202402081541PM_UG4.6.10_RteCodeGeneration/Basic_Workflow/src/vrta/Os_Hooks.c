#include "Os.h"
#include "Main_Cfg.h"
#include "target.h"
#include "Rte_Intl.h"
#include "Vrta_Cfg.h"

/* This is called during StartOS() and before RTA-OS starts the scheduler.
 * It is the safest place to enable interrupt sources that have been initialized
 * before StartOS() was called. */
FUNC(void, OS_APPL_CODE) StartupHook(void) {
    TargetEnableInterrupts();
}

FUNC(void, OS_APPL_CODE) ShutdownHook(StatusType Error) {
    /* OS has shut down */
}

FUNC(boolean,OS_APPL_CODE) Os_Cbk_Idle(void) {
    while(!vrtaIsAppFinished()) {
        vrtaIsIdle(5);
    }
    vrtaTerminate();
    return TRUE;
}

FUNC(Os_StopwatchTickType, OS_APPL_CODE) Os_Cbk_GetStopwatch( void ) {
    return (Os_StopwatchTickType) (1000 * TargetReadClock());
}

FUNC(AccessType, OS_APPL_CODE) Os_Cbk_CheckMemoryAccess(ApplicationType Application, TaskType TaskID, ISRType ISRID, MemoryStartAddressType Address, MemorySizeType Size) {
    (void)Application;
    (void)TaskID;
    (void)ISRID;
    (void)Address;
    (void)Size;
    return (AccessType)(0U);
}
