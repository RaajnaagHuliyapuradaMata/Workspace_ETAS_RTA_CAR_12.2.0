/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2018, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"


#if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_FeeFs1x_Prv_Cfg.h"
#include "rba_FeeFs1x_Prv_InvalidateJob.h"

#include "rba_FeeFs1x_Prv_BlockJob.h"
#include "rba_FeeFs1x_Prv_BlockJobTypes.h"
#include "rba_FeeFs1x_Prv_BC.h"

#include "Fee_Prv_Job.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */

#define FEE_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

static rba_FeeFs1x_Searcher_RetVal_ten  rba_FeeFs1x_InvJob_searchReturnValue_en;

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

#define FEE_START_SEC_VAR_CLEARED_16
#include "Fee_MemMap.h"

static uint16                           rba_FeeFs1x_InvJob_statusByte_u16;

#define FEE_STOP_SEC_VAR_CLEARED_16
#include "Fee_MemMap.h"


/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 * rba_FeeFs1x_InvJob_cancel
 * Init function which is called during Fee_Init. Initialises all internal variables.
 * \param   none
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_InvJob_cancel(void)
{
    rba_FeeFs1x_InvJob_searchReturnValue_en = rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e;
}

/**
 *********************************************************************
 * rba_FeeFs1x_InvJob_initJob
 * function for passing the relevant data from the job acceptor function in BlockJob unit
 * \param   orderStruct_pcst : details of the job order
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_InvJob_initJob(Fee_Prv_JobDesc_tst const * orderStruct_pcst)
{
    rba_FeeFs1x_InvJob_statusByte_u16 = orderStruct_pcst->statusFlag_u16;

    // Avoid warning of variable not being used when feature is deactivated.
    (void) rba_FeeFs1x_InvJob_statusByte_u16;
}

/**
 *********************************************************************
 * initializing the check for an invalidation request
 * \param   searchResult_en : search result from previously called BlockJobDo_search
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_InvJob_checkInvReq(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    rba_FeeFs1x_InvJob_searchReturnValue_en = retValSearch_en;
}

/**
 *********************************************************************
 * asynchronous part of the "check an invalidation request"
 * \param   void
 * \return  result of the checking for necessity / possiblility of invalidating this block
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_JobOK_e        : copy is invalidated
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_execute_e      : execute the invalidation
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e        : internal error leading to job abort
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_InvJob_checkInvReqDo(void)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;

    switch(rba_FeeFs1x_InvJob_searchReturnValue_en)
    {
        case rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e:
        case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
        {
            if(rba_FeeFs1x_BC_getCpyInvBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e))
            {
                // copy is already invalidated, so return OK and do nothing
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_JobOK_e;
            }
            else
            {
                // copy is not yet invalidated, so execute the invalidation
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_execute_e;
            }
            break;
        }
        case rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e:
        {
            // If no copies are found, write a new invalidated header
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_execute_e;
            break;
        }
        default:
        {
            // If not explicitly 0 or 1 copy is found by the searcher, the return value is unexpected --> error
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
            break;
        }
    }
    return (retVal_en);
}

/**
 *********************************************************************
 * prepare function for writing a native invalidated copy to the flash
 * \param   retValSearch_en : search result from searching for an already present block copy
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_InvJob_execInv_Native(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    // retValSearch is not required during a native copy write
    (void) retValSearch_en;

    rba_FeeFs1x_BlockJob_execWr(
            0u,          // data CRC
            rba_FeeFs1x_BlockJob_execWr_jobType_invalidate,
            FALSE,       // native copy
            TRUE,        // isCopy1Destroyed_b
            TRUE         // isCopy2Destroyed_b
            );
}

/**
 *********************************************************************
 * rba_FeeFs1x_InvJob_execInv_Red
 * prepare function for writing a redundant invalidated copy to the flash
 * \param   retValSearch_en : result of the previously executed search
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
# if(RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
void rba_FeeFs1x_InvJob_execInv_Red(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    // retValSearch is not required during a redundant copy write
    (void) retValSearch_en;


    rba_FeeFs1x_BlockJob_execWr(
            0u,          // data CRC
            rba_FeeFs1x_BlockJob_execWr_jobType_invalidate,
            TRUE,        // redundant copy
            TRUE,        // isCopy1Destroyed_b
            TRUE         // isCopy2Destroyed_b
            );
}
# endif
/**
 *********************************************************************
 * rba_FeeFs1x_InvJob_execInv_NoFBNative
 * job init function for invalidating native NoFB copy
 * \param   retValSearch_en : result of the previously executed search
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_InvJob_execInv_NoFBNative(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    boolean hasNoFBBitActive_b;
    boolean isCopy2Destroyed_b;

    if(rba_FeeFs1x_Prv_isNoFallbackActive())
    {
        if(rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e == retValSearch_en)
        {
            /* block is a noFallback block, but no copy is found, so set native copy to already destroyed. */
            isCopy2Destroyed_b = TRUE;
        }
        else
        {
            hasNoFBBitActive_b = rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
            /* One copy is found.
               - If found copy is a nofallback block (hasNoFBBitActive_b == TRUE)
                   -> destroy the copy
               - If found copy is a no nofallback block (hasNoFBBitActive_b == FALSE)
                   -> eventually block was reconfigured from a no NoFB block to a NoFB block
                      If the block was formerly a no NoFB block, writing the destroy marker would
                      lead to a write access to already written last page of the previous block
                      A write error would occur
                      -> Do not write the destroy marker */
            isCopy2Destroyed_b = !hasNoFBBitActive_b;
        }

        rba_FeeFs1x_BlockJob_execWr(
                0u,          // data CRC
                rba_FeeFs1x_BlockJob_execWr_jobType_invalidate,
                FALSE,                        // native copy
                TRUE,                         // isCopy1Destroyed_b
                isCopy2Destroyed_b
                );
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_InvJob_execInv_NoFBRed
 * job init function for invalidating a redundant noFallback copy
 * \param   retValSearch_en : result of the previously executed search
 * \seealso
 * \usedresources
 *********************************************************************
 */
# if(RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
void rba_FeeFs1x_InvJob_execInv_NoFBRed(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    boolean hasNoFBBitActive_b;
    boolean isCopy1Destroyed_b;
    boolean isCopy2Destroyed_b;

    if(rba_FeeFs1x_Prv_isNoFallbackActive())
    {
        /* default: already destroyed */
        isCopy1Destroyed_b = TRUE;
        isCopy2Destroyed_b = TRUE;

        switch(retValSearch_en)
        {
            case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
            {
                hasNoFBBitActive_b = rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
                /* One copy is found.
                   - If found copy is a nofallback block (hasNoFBBitActive_b == TRUE)
                       -> destroy the copy
                   - If found copy is a no nofallback block (hasNoFBBitActive_b == FALSE)
                       -> eventually block was reconfigured from a no NoFB block to a NoFB block
                          If the block was formerly a no NoFB block, writing the destroy marker would
                          lead to a write access to already written last page of the previous block
                          A write error would occur
                          -> Do not write the destroy marker */
                isCopy2Destroyed_b = !hasNoFBBitActive_b;
            }break;

            case rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e:
            {
                hasNoFBBitActive_b = rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Redundant_e);
                /* Two copies found.
                    - If found redundant copy is a nofallback block (hasNoFBBitActive_b == TRUE)
                       -> destroy the copy
                   - If found redundant copy is a no nofallback block (hasNoFBBitActive_b == FALSE)
                       -> eventually block was reconfigured from a no NoFB block to a NoFB block
                          If the block was formerly a no NoFB block, writing the destroy marker would
                          lead to a write access to already written last page of the previous block
                          A write error would occur
                          -> Do not write the destroy marker */
                isCopy1Destroyed_b = !hasNoFBBitActive_b;

                hasNoFBBitActive_b = rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
                /* Two copies found.
                     - If found native copy is a nofallback block (hasNoFBBitActive_b == TRUE)
                        -> destroy the copy
                    - If found native copy is a no nofallback block (hasNoFBBitActive_b == FALSE)
                        -> eventually block was reconfigured from a no NoFB block to a NoFB block
                           If the block was formerly a no NoFB block, writing the destroy marker would
                           lead to a write access to already written last page of the previous block
                           A write error would occur
                           -> Do not write the destroy marker */
                isCopy2Destroyed_b = !hasNoFBBitActive_b;
            }break;

            // case no copy found: take the default values for isCopyDestroyed_b
            case rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e:
            default:
            {
                /* MR12 DIR 16.4 VIOLATION: default statement should never be reached */
                break;
            }
        }

        rba_FeeFs1x_BlockJob_execWr(
                0u,          // data CRC
                rba_FeeFs1x_BlockJob_execWr_jobType_invalidate,
                TRUE,        // redundant copy
                isCopy1Destroyed_b,
                isCopy2Destroyed_b
                );
    }
}
# endif

# if((TRUE == RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLOCK_WRITE) || (TRUE == RBA_FEEFS1X_PRV_CFG_UNKNOWN_SURVIVAL_BLOCK_WRITE))
/**
 *********************************************************************
 * prepare function for invalidating a unknown block
 * \param   retValSearch_en : search result from searching for an already present block copy
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_InvJob_execInv_UnknownBlk(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    boolean isRed_b = FALSE;

    // retValSearch is not required during a unknown block copy write
    (void) retValSearch_en;

    // check if the unknown block is redundant
    if(0u != (rba_FeeFs1x_InvJob_statusByte_u16 & RBA_FEEFS1X_PRV_CFG_DOUBLESTORAGE_BITS))
    {
        isRed_b = TRUE;
    }

    rba_FeeFs1x_BlockJob_execWr(
            0u,          // data CRC
            rba_FeeFs1x_BlockJob_execWr_jobType_invalidate,
            isRed_b,     // native or redundant copy
            TRUE,        // to keep implementation simple, the old nofallback copies are not invalidated.
            TRUE         // to keep implementation simple, the old nofallback copies are not invalidated.
            );
}
# endif

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

