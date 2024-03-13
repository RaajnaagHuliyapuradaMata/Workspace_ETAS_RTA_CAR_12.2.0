/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/*
 ***************************************************************************************************
 * Spinlock implementation for Systems with up to 4 cores
 * assumptions:
 *      All locks which are set in a process are released before the end of the process.
 *      Interrupts are not suspended at the end of a process.
 *
 * features:
 *      Nesting with itself is allowed for all locks.
 *      Lock nesting counter for first lock is 2. Lock nesting counter = 1 is only used for arbitration.
 *      Activation of "nonest" lock is allowed even if "common" lock is active.
 *      Invalid nesting is detected.
 *
 ***************************************************************************************************
 */

/*
 *********************************************************************
 * Includes
 *********************************************************************
 */
#include "rba_BswSrv.h"
#ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT


#include "rba_BswSrv_Prv.h"

/*
 *********************************************************************
 * Defines
 *********************************************************************
 */


/*
 *********************************************************************
 * Variables
 *********************************************************************
 */

/* locks are only implemented, if more than one core is present */
#if (RBA_BSWSRV_NUM_USED_CORE > 1)
#  define RBA_BSWSRV_START_SEC_VAR_INIT_UNSPECIFIED
#  include "rba_BswSrv_MemMap.h"
rba_BswSrv_Lock_to rba_BswSrv_LockCommon_st;
#  define RBA_BSWSRV_STOP_SEC_VAR_INIT_UNSPECIFIED
#  include "rba_BswSrv_MemMap.h"
#endif

/* without CPU instanced memory at the same address */
#  define RBA_BSWSRV_START_SEC_VAR_INIT_UNSPECIFIED
#  include "rba_BswSrv_MemMap.h"
rba_BswSrv_CoreData_tst rba_BswSrv_CoreData_ast[RBA_BSWSRV_MAX_PHYS_CORE];
#  define RBA_BSWSRV_STOP_SEC_VAR_INIT_UNSPECIFIED
#  include "rba_BswSrv_MemMap.h"

/*
 *********************************************************************
 * Function prototypes
 *********************************************************************
 */
/* locks are only implemented, if more than one core is present */
#if (RBA_BSWSRV_NUM_USED_CORE > 1)
LOCAL_INLINE void rba_BswSrv_GetLockInternal(rba_BswSrv_CoreData_tst* CoreData_pst,
        rba_BswSrv_Lock_to* Lock_pst,
        rba_BswSrv_Lock_to* volatile * CoreActiveLock_pvpst);
LOCAL_INLINE void rba_BswSrv_ReleaseLockInternal(rba_BswSrv_CoreData_tst* CoreData_pst,
        rba_BswSrv_Lock_to* Lock_pst,
        rba_BswSrv_Lock_to* volatile * CoreActiveLock_pvpst);
#endif

LOCAL_INLINE void rba_BswSrv_PrvSuspendCoreLocalIntInline(rba_BswSrv_CoreData_tst* CoreData_pst);
LOCAL_INLINE void rba_BswSrv_PrvResumeCoreLocalIntInline(rba_BswSrv_CoreData_tst* CoreData_pst, uint32 stCheck_u32);


/*
 *********************************************************************
 * Functions
 *********************************************************************
 */

#define RBA_BSWSRV_START_SEC_CODE
#include "rba_BswSrv_MemMap.h"
/*
 *********************************************************************
 * rba_BswSrv_PrvTryToGetSimpleLock - Try to get a SimpleLock without interrupt suspend.
 *********************************************************************
 */
uint8_least rba_BswSrv_PrvTryToGetSimpleLock(rba_BswSrv_SimpleLock_to* Lock_pst)
{
    /* add RBA_BSWSRV_MAX_PHYS_CORE to Core number to distinguish between NoNestLock and SimpleLock */
    uint32 RequestedCoreLock_u32 = (0x81u + RBA_BSWSRV_MAX_PHYS_CORE) + RBA_BSWSRV_CFG_GETPHYSCORE();
    uint32 Lock_u32;

    /* Atomic set word if zero */
    Lock_u32 = rba_BswSrv_PrvTryToGetLock(&(Lock_pst->Prv_CoreLock_vu32), RequestedCoreLock_u32);

    if(Lock_u32 == 0u)
    {
        /* lock was received */
        return 1;
    }

    /* Check range of Core number to exclude parallel use by non-SimpleLock. */
    if (((Lock_u32 - (0x81u + RBA_BSWSRV_MAX_PHYS_CORE)) >= RBA_BSWSRV_MAX_PHYS_CORE) && (Lock_u32 != 0x80u))
    {
        /* Error: getting spinlock is not possible, as lock is active for a non-SimpleLock! */
        RBA_BSWSRV_CFG_MCU_PERFORM_RESET;
    }

    /* lock was not received */
    return 0;
}

/*
 *********************************************************************
 * rba_BswSrv_PrvReleaseSimpleLock - release a SimpleLock without interrupt suspend.
 *********************************************************************
 */
void rba_BswSrv_PrvReleaseSimpleLock(rba_BswSrv_SimpleLock_to* Lock_pst)
{
    /* add RBA_BSWSRV_MAX_PHYS_CORE to Core number to distinguish between NoNestLock and SimpleLock */
    uint32 RequestedCoreLock_u32 = (0x81u + RBA_BSWSRV_MAX_PHYS_CORE) + RBA_BSWSRV_CFG_GETPHYSCORE();

    if(Lock_pst->Prv_CoreLock_vu32 == RequestedCoreLock_u32)
    {
        /* release lock */
        rba_BswSrv_PrvWriteLock(&(Lock_pst->Prv_CoreLock_vu32), 0u);
    }
    else
    {
        /* Error: release of lock is not possible, as lock is not active for current core */
        RBA_BSWSRV_CFG_MCU_PERFORM_RESET;
    }
}

/*!
 ***************************************************************************************************
 * \brief Suspend CPU interrupts
 *
 * This function stores the state of the global CPU interrupt enable flag and disables the interrupts.
 *
 * \param[in]       void
 * \return          void
 ***************************************************************************************************
 */
void rba_BswSrv_PrvSuspendCoreLocalInt(void)
{
    rba_BswSrv_CoreData_tst* CoreData_pst = &rba_BswSrv_CoreData_ast[RBA_BSWSRV_CFG_GETPHYSCORE()];

    rba_BswSrv_PrvSuspendCoreLocalIntInline(CoreData_pst);
#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_CORE
    CoreData_pst->Log_st.ctIntSuspend_u16++;
#endif
}

/*!
 ***************************************************************************************************
 * \brief Suspend CPU interrupts inline
 *
 * This function stores the state of the global CPU interrupt enable flag and disables the interrupts.
 *
 * \param[in]       void
 * \return          void
 ***************************************************************************************************
 */
LOCAL_INLINE void rba_BswSrv_PrvSuspendCoreLocalIntInline(rba_BswSrv_CoreData_tst* CoreData_pst)
{
    /* read and set interrupt disable state  */
    uint32 ctSuspend_u32 = CoreData_pst->ctIntSuspLvl_u8;
    if (ctSuspend_u32 == 0u)
    {

		RBA_BSWSRV_CFG_DISABLE_INTERRUPT

#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_CORE
        CoreData_pst->Log_st.adLastSuspend_u32 = rba_BswSrv_GetReturnAddress();
        if(CoreData_pst->stLogEnable_u32 > 0u)
        {
            /* interrupt was enabled before: remember interrupt suspend time */
            CoreData_pst->Log_st.tiCurrIntSuspend_u32 = RBA_BSWSRV_CFG_GET_SYSTICKS();
        }
#endif
    }

    /* increment interrupt suspend level counter */
    CoreData_pst->ctIntSuspLvl_u8 = (uint8)(ctSuspend_u32 + 1u);
}

/*!
 ***************************************************************************************************
 * \brief Resume CPU interrupt status
 *
 * This function restores the state of the CPU interrupt system.
 *
 * \param[in]       void
 * \return          void
 ***************************************************************************************************
 */
void rba_BswSrv_PrvResumeCoreLocalInt(void)
{
    rba_BswSrv_CoreData_tst* CoreData_pst = &rba_BswSrv_CoreData_ast[RBA_BSWSRV_CFG_GETPHYSCORE()];

    rba_BswSrv_PrvResumeCoreLocalIntInline(CoreData_pst, 1u);
}

/*!
 ***************************************************************************************************
 * \brief Resume CPU interrupt status
 *
 * This function restores the state of the CPU interrupt system.
 *
 * \param[in]       rba_BswSrv_CoreData_tst* CoreData_pst
 * \param[in]       uint32 stCheck_u32 - This is typically a fix value which is evaluated at compile time.
 * \return          void
 ***************************************************************************************************
 */
LOCAL_INLINE void rba_BswSrv_PrvResumeCoreLocalIntInline(rba_BswSrv_CoreData_tst* CoreData_pst, uint32 stCheck_u32)
{
    uint32 ctPrevIntSuspLvl_u32 = CoreData_pst->ctIntSuspLvl_u8;

    /* decrement interrupt suspend level counter as common action before conditional re-enabling interrupts */
    CoreData_pst->ctIntSuspLvl_u8 = (uint8)(ctPrevIntSuspLvl_u32 - 1u);

    if (ctPrevIntSuspLvl_u32 == 1u)
    {
#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_CORE
        CoreData_pst->Log_st.adLastSuspend_u32 = 0;
        if((stCheck_u32 != 0u) && (CoreData_pst->stLogEnable_u32 > 0u))
        {
            uint32 tiInterruptSuspend_u32;

            /* measure current suspend time */
            tiInterruptSuspend_u32 = RBA_BSWSRV_CFG_GET_SYSTICKS() - CoreData_pst->Log_st.tiCurrIntSuspend_u32;
            CoreData_pst->Log_st.tiPrevSuspend_u32 = tiInterruptSuspend_u32;

            /* check if current value is new maximum */
#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_CORE_TOP10
            if(tiInterruptSuspend_u32 > CoreData_pst->Log_st.tiMaxIntSuspend_au32[0])
            {
                uint32 ct_u32;
                uint32 idxTime_u32 = 0;
                uint32 idxAdr_u32 = 0;
                uint32 adrReturn_u32 = rba_BswSrv_GetReturnAddress();

                /* search for index of entries */
                for(ct_u32 = 1u; ct_u32 < 10u; ct_u32++)
                {
                    /* get highest index with shorter lock time */
                    if(tiInterruptSuspend_u32 > CoreData_pst->Log_st.tiMaxIntSuspend_au32[ct_u32])
                    {
                        idxTime_u32 = ct_u32;
                    }
                    /* get index with same address */
                    if(adrReturn_u32 == CoreData_pst->Log_st.adMaxIntSuspend_au32[ct_u32])
                    {
                        idxAdr_u32 = ct_u32;
                    }
                }

                /* shift data of lower suspend times */
                for(ct_u32 = idxAdr_u32; (ct_u32 < idxTime_u32) && (ct_u32 < 9u); ct_u32++)
                {
                    /*MR12 RULE 1.3 VIOLATION: This warning is not critical and there is no lost data, since the element being accessed is calculated within bounds of an array. It needs for developer to process data.*/
                    CoreData_pst->Log_st.tiMaxIntSuspend_au32[ct_u32] = CoreData_pst->Log_st.tiMaxIntSuspend_au32[ct_u32 + 1u];
                    CoreData_pst->Log_st.adMaxIntSuspend_au32[ct_u32] = CoreData_pst->Log_st.adMaxIntSuspend_au32[ct_u32 + 1u];
                }

                /* Write new data */
                if(idxAdr_u32 <= idxTime_u32)
                {
                    CoreData_pst->Log_st.tiMaxIntSuspend_au32[idxTime_u32] = tiInterruptSuspend_u32;
                    CoreData_pst->Log_st.adMaxIntSuspend_au32[idxTime_u32] = adrReturn_u32;
                }
            }
#else
            if(tiInterruptSuspend_u32 > CoreData_pst->Log_st.tiMaxIntSuspend_u32)
            {
                uint32 adrReturn_u32 = rba_BswSrv_GetReturnAddress();

                /* check, if calling function has to be ignored */
                if (adrReturn_u32 != CoreData_pst->Log_st.adMaxIntSuspendIgnore_u32)
                {
                    /* store disable time */
                    CoreData_pst->Log_st.tiMaxIntSuspend_u32 = tiInterruptSuspend_u32;
                    /* store return address of calling function for tracking purposes */
                    CoreData_pst->Log_st.adMaxIntSuspend_u32 = adrReturn_u32;
                }
            }
#endif
        }
#endif

        RBA_BSWSRV_CFG_ENABLE_INTERRUPT  /* reenable interrupts */

    }
    else if((stCheck_u32 != 0u) && (ctPrevIntSuspLvl_u32 == 0u))
    {
        /* Error: interrupt suspend counter underflow */
        RBA_BSWSRV_CFG_MCU_PERFORM_RESET;
    }
    else
    {
        /* no action required */
    }
}


/* locks are only implemented, if more than one core is present */
#if (RBA_BSWSRV_NUM_USED_CORE > 1)
/*
 *********************************************************************
 * GetLockCommon - Get common spinlock.
 *
 * Must not be called, if a nonest lock is already active for the
 * current core.
 *********************************************************************
 */
void rba_BswSrv_PrvGetLockCommon(void)
{
    rba_BswSrv_CoreData_tst* CoreData_pst = &rba_BswSrv_CoreData_ast[RBA_BSWSRV_CFG_GETPHYSCORE()];

    /* nesting with a nonest lock is not allowed */
    if(CoreData_pst->ActiveNonestLock_vpst == NULL_PTR)
    {
        rba_BswSrv_GetLockInternal(CoreData_pst, &rba_BswSrv_LockCommon_st, NULL_PTR);
    }
    else
    {
        /* Error: getting common lock is not allowed because a nonest lock is active */
        RBA_BSWSRV_CFG_MCU_PERFORM_RESET;
    }
}

/*
 *********************************************************************
 * ReleaseLockCommon - Release common spinlock.
 *
 *
 *********************************************************************
 */
void rba_BswSrv_PrvReleaseLockCommon(void)
{
    rba_BswSrv_CoreData_tst* CoreData_pst = &rba_BswSrv_CoreData_ast[RBA_BSWSRV_CFG_GETPHYSCORE()];

    /* nesting with a nonest lock is not allowed */
    if(CoreData_pst->ActiveNonestLock_vpst == NULL_PTR)
    {
        rba_BswSrv_ReleaseLockInternal(CoreData_pst, &rba_BswSrv_LockCommon_st, NULL_PTR);
    }
    else
    {
        /* Error: release of common lock is not allowed because a nonest lock is active */
        RBA_BSWSRV_CFG_MCU_PERFORM_RESET;
    }
}

/*
 *********************************************************************
 * GetLockNonest - Get "nonest" spinlock.
 *
 * Must not be called, if a another "nonest" lock is already active for
 * the current core.
 *********************************************************************
 */
void rba_BswSrv_PrvGetLockNonest(rba_BswSrv_Lock_to* Lock_pst)
{
    rba_BswSrv_CoreData_tst* CoreData_pst = &rba_BswSrv_CoreData_ast[RBA_BSWSRV_CFG_GETPHYSCORE()];

    rba_BswSrv_Lock_to* ActiveNonestLock_pst = CoreData_pst->ActiveNonestLock_vpst;

    /* only nesting with the common lock and the same lock is allowed */
    if((ActiveNonestLock_pst == Lock_pst) ||
            (ActiveNonestLock_pst == NULL_PTR))
    {
        rba_BswSrv_GetLockInternal(CoreData_pst, Lock_pst, &(CoreData_pst->ActiveNonestLock_vpst));
    }
    else
    {
        /* Error: getting nonest lock is not allowed because a different nonest lock is already active */
        RBA_BSWSRV_CFG_MCU_PERFORM_RESET;
    }
}

/*
 *********************************************************************
 * ReleaseLockNonest - Release "nonest" spinlock.
 *
 *
 *********************************************************************
 */
void rba_BswSrv_PrvReleaseLockNonest(rba_BswSrv_Lock_to* Lock_pst)
{
    rba_BswSrv_CoreData_tst* CoreData_pst = &rba_BswSrv_CoreData_ast[RBA_BSWSRV_CFG_GETPHYSCORE()];

    /* current lock to be released must match the active nonest lock */
    if(CoreData_pst->ActiveNonestLock_vpst == Lock_pst)
    {
        rba_BswSrv_ReleaseLockInternal(CoreData_pst, Lock_pst, &(CoreData_pst->ActiveNonestLock_vpst));
    }
    else
    {
        /* Error: release of nonest lock is not allowed because a different nonest lock is already active */
        RBA_BSWSRV_CFG_MCU_PERFORM_RESET;
    }
}


/*
 *********************************************************************
 * GetLockInternal - Internal implementation of spinlock.
 *
 *
 *********************************************************************
 */
LOCAL_INLINE void rba_BswSrv_GetLockInternal(rba_BswSrv_CoreData_tst* CoreData_pst,
        rba_BswSrv_Lock_to* Lock_pst,
        rba_BswSrv_Lock_to* volatile * CoreActiveLock_pvpst)

{
#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_LOCK
    uint32 ctLockWait_u32 = 0;
#endif

    /* loop until lock is available or already set */
    do
    {
        /* suspend interrupts unconditionally as this is the time critical path */
        rba_BswSrv_PrvSuspendCoreLocalIntInline(CoreData_pst);
        {
            uint32 RequestedCoreLock_u32 = 0x81u + RBA_BSWSRV_CFG_GETPHYSCORE();
            uint32 CurrentCoreLock_u32;

            /* Try to get lock for current core. Includes atomic read of Lock variable */
            CurrentCoreLock_u32 = rba_BswSrv_PrvTryToGetLock(&(Lock_pst->Prv_CoreLock_vu32), RequestedCoreLock_u32);

            /* lock was not yet set before */
            if (CurrentCoreLock_u32 == 0u)
            {
                /* nesting counter bits are cleared in RequestedCoreLock_u32 */

                /* check is removed by compiler, as result is known at compile time of inlined code */
                if(CoreActiveLock_pvpst != NULL_PTR)
                {
                    /* Remember lock to be able to detect nesting violations */
                    *CoreActiveLock_pvpst = Lock_pst;
                }

#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_CORE
                /* read-modify-write of core specific data is possible at this point as interrupts are disabled */
                CoreData_pst->Log_st.ctLockUse_u16++;
#endif
#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_LOCK
                /* read-modify-write of lock specific data is possible at this point as lock is already active */
                Lock_pst->Prv_adActivation_u32 = rba_BswSrv_GetReturnAddress();
                Lock_pst->Prv_ctLockUse_vu16++;
                if(ctLockWait_u32 > 0u) /* conditional write for better traceability */
                {
                    Lock_pst->Prv_ctLockWait_vu16 += (uint16)ctLockWait_u32;
                }
#endif
                return; /* Lock is set */
            }

            /* Check if lock is already set for current core */
            else if ((CurrentCoreLock_u32 & 0xFFu) == RequestedCoreLock_u32)
            {
                /* Lock is already set for current core: update lock counter. */
                CurrentCoreLock_u32 += 0x100u;
                rba_BswSrv_PrvWriteLock(&(Lock_pst->Prv_CoreLock_vu32), CurrentCoreLock_u32);

#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_CORE
                /* Write marker to identify self nesting at lock analysis by data trace, use some available value to minimize runtime */
                CoreData_pst->Log_st.stLockNest_u32 = CurrentCoreLock_u32;
#endif
#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_LOCK
                Lock_pst->Prv_ctLockNest_vu16++;
#endif
                return; /* Lock is set */
            }
            /* Check range of Core number to exclude memory corruption. */
            else if ((CurrentCoreLock_u32 & 0x7Fu) > RBA_BSWSRV_MAX_PHYS_CORE)
            {

                /* Error: getting spinlock is not possible, as lock is active for invalid lock id! */
                RBA_BSWSRV_CFG_MCU_PERFORM_RESET;
            }
            else
            {
                /* no action required as lock is hold by other core */
            }

#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_CORE
            /* read-modify-write of core specific data is possible at this point as interrupts are disabled */
            CoreData_pst->Log_st.ctLockWait_u16++;
#endif
        }

        rba_BswSrv_PrvResumeCoreLocalIntInline(CoreData_pst, 0u); /* no checks are required, as interrupts were suspended before in the same function */

#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_LOCK
        ctLockWait_u32++;
#endif
    } while(TRUE);
}


/*
 *********************************************************************
 * ReleaseLockInternal - Internal implementation of spinlock.
 *
 * use inline function to keep information about caller
 *********************************************************************
 */
LOCAL_INLINE void rba_BswSrv_ReleaseLockInternal(rba_BswSrv_CoreData_tst* CoreData_pst,
        rba_BswSrv_Lock_to* Lock_pst,
        rba_BswSrv_Lock_to* volatile * CoreActiveLock_pvpst)
{
    uint32 PhysCore_u32        = RBA_BSWSRV_CFG_GETPHYSCORE();
    uint32 CurrentCoreLock_u32 = Lock_pst->Prv_CoreLock_vu32;

    if((CurrentCoreLock_u32 & 0xFFu) == (PhysCore_u32 + 0x81u))
    {
        /* lock is active for current core */

        if ((CurrentCoreLock_u32 >> 8u) == 0u)  /* only compare nesting counter */
        {
            /* lock isn't nested */
#if RBA_BSWSRV_CFG_LOG_LEVEL >= RBA_BSWSRV_PRV_LOG_LOCK
            Lock_pst->Prv_adActivation_u32 = 0u;
#endif

            /* check is removed by compiler, as result is known at compile time of inlined code */
            if(CoreActiveLock_pvpst != NULL_PTR)
            {
                /* remember lock release to be able to detect nesting violations */
                *CoreActiveLock_pvpst = NULL_PTR;
            }

            /* lock variable: release lock */
            CurrentCoreLock_u32 = 0u;
        }
        else
        {

            /* lock variable: lock is still active as it was activated several times */
            CurrentCoreLock_u32 -= 0x100u;
        }
        /* write lock variable */
        rba_BswSrv_PrvWriteLock(&(Lock_pst->Prv_CoreLock_vu32), CurrentCoreLock_u32);

        /* use inline function to keep information about calling function */
        rba_BswSrv_PrvResumeCoreLocalIntInline(CoreData_pst, 1u);
    }
    else
    {
        /* Error: release of lock is not possible, as lock is not active for current core */
        RBA_BSWSRV_CFG_MCU_PERFORM_RESET;
    }
}


/* locks are only implemented, if there is more than one core */
/* end: #if (RBA_BSWSRV_NUM_USED_CORE > 1) */
#endif

#define RBA_BSWSRV_STOP_SEC_CODE
#include "rba_BswSrv_MemMap.h"

/* end #ifndef RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT */
#endif

/*<RBHead>
 ***********************************************************************************************************************
 * List of changes
 *
 * $History$
 *
 ***********************************************************************************************************************
</RBHead>*/
