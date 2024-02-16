/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/


#ifndef RBA_BSWSRV_PRV_H
#define RBA_BSWSRV_PRV_H

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

/* define for RBA_BSWSRV_CFG_LOG_LEVEL */
#define RBA_BSWSRV_PRV_LOG_NONE          0
#define RBA_BSWSRV_PRV_LOG_CORE          1
#define RBA_BSWSRV_PRV_LOG_CORE_TOP10    2
#define RBA_BSWSRV_PRV_LOG_LOCK          3


/*
 *********************************************************************
 * Type definition
 *********************************************************************
 */
#ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT

#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_CORE
typedef struct
{
    uint32 adLastSuspend_u32;           /* return address of interrupt suspend or lock,  not cleared at interrupt enable */
    uint32 tiPrevSuspend_u32;           /* TIM0 duration of interrupt suspend or lock */
    #if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_CORE_TOP10
    uint32 tiMaxIntSuspend_au32[10];    /* sorted list of maximum TIM0 duration of interrupt suspend or lock */
    uint32 adMaxIntSuspend_au32[10];    /* return address of interrupt resume/lock relase for tiMaxIntSuspend_au32 */
    #else
    uint32 tiMaxIntSuspend_u32;         /* maximum TIM0 duration of interrupt suspend or lock */
    uint32 adMaxIntSuspend_u32;         /* return address of interrupt resume/lock relase for tiMaxIntSuspend_u32 */
    uint32 adMaxIntSuspendIgnore_u32;   /* this return address is ignored at calculation of maximum TIM0 duration */
    #endif
    uint32 tiCurrIntSuspend_u32;        /* contains TIM0-time of interrupt suspend or lock */
    uint16 ctIntSuspend_u16;            /* counter for interrupt suspend (does not contain call by lock) */
    uint16 ctIntSuspendRef_u16;         /* reference counter for interrupt suspend,  set to ctIntSuspend_u16 by rba_BswSrv_LockMeasure_10ms_Proc() */
    #if (RBA_BSWSRV_NUM_USED_CORE > 1)
    uint16 ctLockUse_u16;               /* counter for lock usage (does not contain self-nesting or wait) */
    uint16 ctLockUseRef_u16;            /* reference counter for lock usage,  set to ctLockUse_u16 by rba_BswSrv_LockMeasure_10ms_Proc() */
    uint16 ctLockWait_u16;              /* counter for lock wait loops */
    uint16 ctLockWaitRef_u16;           /* reference counter for lock wait loops,  set to ctLockWait_u16 by rba_BswSrv_LockMeasure_10ms_Proc() */
    uint32 stLockNest_u32;              /* marker written at lock self-nesting */
    #endif
} rba_BswSrv_Log_tst;
#endif

typedef struct
{
    #if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_CORE
    uint32 stLogEnable_u32;                             /* set this to != 0 to enable time measurement */
    rba_BswSrv_Log_tst              Log_st;
    #endif
    #if (RBA_BSWSRV_NUM_USED_CORE > 1)
    rba_BswSrv_Lock_to* volatile    ActiveNonestLock_vpst;
    #endif
    uint8                           ctIntSuspLvl_u8;
} rba_BswSrv_CoreData_tst;

/*
 ***************************************************************************************************
 * Variable declarations
 ***************************************************************************************************
 */
#define RBA_BSWSRV_START_SEC_VAR_INIT_UNSPECIFIED
#include "rba_BswSrv_MemMap.h"
extern rba_BswSrv_Lock_to rba_BswSrv_LockCommon_st;
#define RBA_BSWSRV_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "rba_BswSrv_MemMap.h"

/* without CPU instanced memory at the same address */
#  define RBA_BSWSRV_START_SEC_VAR_INIT_UNSPECIFIED
#  include "rba_BswSrv_MemMap.h"
extern rba_BswSrv_CoreData_tst rba_BswSrv_CoreData_ast[RBA_BSWSRV_MAX_PHYS_CORE];
#  define RBA_BSWSRV_STOP_SEC_VAR_INIT_UNSPECIFIED
#  include "rba_BswSrv_MemMap.h"

/*
 ***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
 */

LOCAL_INLINE uint32 rba_BswSrv_GetReturnAddress(void);
LOCAL_INLINE uint32 rba_BswSrv_PrvTryToGetLock(volatile uint32* LockVar_pvu32,  uint32 RequestedLock_u32);
LOCAL_INLINE void rba_BswSrv_PrvWriteLock(volatile uint32* LockVar_pvu32,  uint32 LockValue_u32);

/*
 ***************************************************************************************************
 * Inline functions
 ***************************************************************************************************
 */

/*
 *********************************************************************
 * rba_BswSrv_GetReturnAddress - Get return address of calling function.
 *
 * use inline function to keep information about calling function
 *********************************************************************
 */
LOCAL_INLINE uint32 rba_BswSrv_GetReturnAddress(void)
{
    RBA_BSWSRV_CFG_GET_RETURN_ADDRESS

}

/*********************************************************************
 * rba_BswSrv_PrvTryToGetLock - atomic test and set.
 *
 * Test and set is atomic,  but information whether this was successful is not atomic on JDP.
 * Therefore it is necessary to store the origin of the lock!
 *********************************************************************
 */
/* MR12 RULE 2.7 VIOLATION: The warning is uncritical and non-removable since the parameter is used in assembly code.
                            Approved by MISRA2012_SUPPORT_TEAM. */
LOCAL_INLINE uint32 rba_BswSrv_PrvTryToGetLock(volatile uint32* LockVar_pvu32,  uint32 RequestedLock_u32)
{
    RBA_BSWSRV_CFG_PRV_TRY_TO_LOCK

}


/*********************************************************************
 * rba_BswSrv_PrvWriteLock - Store to allow atomic test and Set.
 *********************************************************************
 */
/* MR12 RULE 2.7 VIOLATION: The warning is uncritical and non-removable since the parameter is used in assembly code.
                            Approved by MISRA2012_SUPPORT_TEAM. */
LOCAL_INLINE void rba_BswSrv_PrvWriteLock(volatile uint32* LockVar_pvu32,  uint32 LockValue_u32)
{
    RBA_BSWSRV_CFG_PRV_WRITE_LOCK
}

/* end of #ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT */
#endif

/* end of #ifndef RBA_BSWSRV_PRV_H */
#endif

