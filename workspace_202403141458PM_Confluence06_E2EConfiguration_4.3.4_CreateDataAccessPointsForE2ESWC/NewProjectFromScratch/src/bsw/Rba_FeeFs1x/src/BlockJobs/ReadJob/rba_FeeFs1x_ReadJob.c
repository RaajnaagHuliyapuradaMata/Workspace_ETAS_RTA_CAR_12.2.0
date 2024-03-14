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
#include "rba_FeeFs1x_Prv_ReadJob.h"
#include "rba_FeeFs1x_Prv_ReadJobTypes.h"

#include "rba_FeeFs1x_Prv_BlockJob.h"
#include "rba_FeeFs1x_Prv_BlockJobTypes.h"
#include "rba_FeeFs1x_Prv_BC.h"
#include "rba_FeeFs1x_Prv_CacheKwn.h"
#include "rba_FeeFs1x_Prv.h"

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


static rba_FeeFs1x_RdJob_JobData_tst rba_FeeFs1x_RdJob_JobData_st;

static rba_FeeFs1x_Searcher_RetVal_ten rba_FeeFs1x_RdJob_searchReturnValue_en;


#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

/**
 *********************************************************************
 * rba_FeeFs1x_RdJob_checkRdReq
 * Function matching the blockjob function typedef for state init functions for passing parameters
 * to an asynchronous job. This specific checkRdReq initializes a read job
 * \param   retValSearch_en : defines how many copies were found during the search
 * \return  Job accepted
 * \retval  E_OK : job accepted, checkRdReqDo should be called now
 * \retval  E_NOT_OK : job not accepted, don't call checkRdReqDo
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_RdJob_checkRdReq(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    rba_FeeFs1x_RdJob_searchReturnValue_en = retValSearch_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_RdJob_checkRdReqDo
 * asynchronous part of the checkRdReq function

 * \return  result of the checking for necessity/ possiblility of reading this block
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_inconsistent_e : no copy with valid data found
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_invalidated_e : valid copy explicitly invalidated found
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_execute_e : read can be executed
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e : internal error leading to job abort
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_RdJob_checkRdReqDo(void)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;
    switch(rba_FeeFs1x_RdJob_searchReturnValue_en)
    {
        case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
        {
            // Inconsistent block data detected for any but the first chunk of a chunk-wise read request?
            if( rba_FeeFs1x_BlockJob_IsBlkDataCrcError()  &&
                rba_FeeFs1x_RdJob_JobData_st.isChunkJob_b &&
                (rba_FeeFs1x_RdJob_JobData_st.offset_u16 > 0u) )
            {
                /* Don't restart with data from a redundant or older block copy, since we don't have access to
                 * the complete chunk user buffer. We have return control to the chunk user for an external
                 * retry from scratch. */
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_inconsistent_e;
            }
            else
            {
                boolean hasInvBitSet_b = FALSE;
                hasInvBitSet_b = rba_FeeFs1x_BC_getCpyInvBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
                if(hasInvBitSet_b)
                {
                    // invalidated copy
                    retVal_en = rba_FeeFs1x_BlockJob_ReturnType_invalidated_e;
                }
                else
                {
                    // not invalidated copy --> read allowed
                    retVal_en = rba_FeeFs1x_BlockJob_ReturnType_execute_e;
                }
            }
            break;
        }
        case rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e:
        {
            // If no copies are found, the copy is inconsistent
            /* Currently this state is not reached for unknown blocks. But in future when unknown block read is
             * supported then, there could be an OOB access as rba_FeeFs1x_RdJob_JobData_st.feeIndex_u16 = 0xFFFF.
             * Which means that there is a risk that the below code may be also called for unknown block. To avoid this
             * the calls are made only when the index is valid. This check today is "dead code" but might protect future
             * OOB access.
             * */
            if(RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS > rba_FeeFs1x_RdJob_JobData_st.feeIndex_u16)
            {
                rba_FeeFs1x_CacheKwn_invalidateCache(rba_FeeFs1x_RdJob_JobData_st.feeIndex_u16);
            }
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_inconsistent_e;
            break;
        }
        default:
        {
            // If not explicitly 0 or 1 copy is found by the searcher, the return value is unexpected --> error
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
            break;
        }
    }
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_RdJob_execRd
 * \param   retValSearch_en : defines how many copies were found during the search
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_RdJob_execRd(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    rba_FeeFs1x_RdJob_searchReturnValue_en = retValSearch_en;

    // start the BC read
    rba_FeeFs1x_BC_readVldtFls2Ram( rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e,
                                    rba_FeeFs1x_RdJob_JobData_st.length_u16,
                                    rba_FeeFs1x_RdJob_JobData_st.offset_u16,
                                    rba_FeeFs1x_RdJob_JobData_st.userbuffer_pu8,
                                    rba_FeeFs1x_RdJob_JobData_st.isChunkJob_b);
}

/**
 *********************************************************************
 * rba_FeeFs1x_RdJob_execRdDo
 * \return  read finished successfully?
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_JobOK_e : read finished
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e : job aborted due to internal errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_RdJob_execRdDo(void)
{
    rba_FeeFs1x_RetVal_ten retValRd_en;
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;
    boolean isFinalizeCopy_b = FALSE;

    retValRd_en = rba_FeeFs1x_BC_readVldtFls2RamDo();

    switch(retValRd_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            /* job finished successfully
             * For chunk jobs, do neither set the cache valid nor finalize the block copies
             * when some more bytes are still to be read */
            if( !rba_FeeFs1x_RdJob_JobData_st.isChunkJob_b || ( rba_FeeFs1x_RdJob_JobData_st.blkLen_u16 <=
                                (rba_FeeFs1x_RdJob_JobData_st.offset_u16 + rba_FeeFs1x_RdJob_JobData_st.length_u16)) )
            {
                /* Currently this state is not reached for unknown blocks. But in future when unknown block read is
                 * supported then, there could be an OOB access as rba_FeeFs1x_RdJob_JobData_st.feeIndex_u16 = 0xFFFF.
                 * Which means that there is a risk that the below code may be also called for unknown block. To avoid
                 * this the calls are made only when the index is valid. This check today is "dead code" but might
                 * protect future OOB access.
                 * */
                if(RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS > rba_FeeFs1x_RdJob_JobData_st.feeIndex_u16)
                {
                    rba_FeeFs1x_CacheKwn_setDataValid(rba_FeeFs1x_RdJob_JobData_st.feeIndex_u16);
                }

                isFinalizeCopy_b = TRUE;
            }

            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_JobOK_e;
            break;
        }
        case rba_FeeFs1x_Pending_e:
        {
            // read needs to be called during the next cycle
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
            break;
        }
        case rba_FeeFs1x_JobFailed_e:
        {
            // block data is inconsistent
            rba_FeeFs1x_BlockJob_ReportBlkDataCrcError();

            isFinalizeCopy_b = TRUE;

            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_restart_e;
            break;
        }
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            isFinalizeCopy_b = TRUE;

            // Error during read or unexpected return value
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
            break;
        }
    }

    // finalize the copies before exit
    if(isFinalizeCopy_b)
    {
        rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_RdJob_cancel
 * resets the RdJob internal variables
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_RdJob_cancel(void)
{
    rba_FeeFs1x_RdJob_searchReturnValue_en = rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e;
}

/**
 *********************************************************************
 * rba_FeeFs1x_RdJob_initJob
 * function for passing the read relevant data for the (re-)initialization of a read job from the job acceptor
 * function in BlockJob unit
 * \param   orderStruct_pcst : struct containing the data collected by the Fee external service APIs
 * \seealso
 * \usedresources
 * \attention   This function is also called for the restart of an ongoing chunk-wise read job.
 *********************************************************************
 */
void rba_FeeFs1x_RdJob_initJob(Fee_Prv_JobDesc_tst const * orderStruct_pcst)
{
    /* The below three structure members must be set for a normal job, or the first chunk of a chunk-wise job.
     * For subsequent chunks, there is no harm to update them again (with the same values - kept by FeeHl). */
    rba_FeeFs1x_RdJob_JobData_st.feeIndex_u16   = orderStruct_pcst->blockNumber_u16;
    rba_FeeFs1x_RdJob_JobData_st.blkLen_u16     = orderStruct_pcst->nrBytTot_u16;
    rba_FeeFs1x_RdJob_JobData_st.isChunkJob_b   = orderStruct_pcst->isChunkJob_b;

    // The below three structure members must be updated for subsequent chunks.
    rba_FeeFs1x_RdJob_JobData_st.length_u16     = orderStruct_pcst->length_u16;
    rba_FeeFs1x_RdJob_JobData_st.offset_u16     = orderStruct_pcst->offset_u16;
    rba_FeeFs1x_RdJob_JobData_st.userbuffer_pu8 = orderStruct_pcst->bfr_pu8;
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

