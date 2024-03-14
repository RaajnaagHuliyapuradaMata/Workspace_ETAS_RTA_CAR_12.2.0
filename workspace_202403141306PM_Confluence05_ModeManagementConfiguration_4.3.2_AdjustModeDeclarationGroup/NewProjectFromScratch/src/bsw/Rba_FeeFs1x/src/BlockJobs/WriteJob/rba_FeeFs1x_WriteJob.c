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

#include "rba_FeeFs1x_Prv_WriteJob.h"
#include "rba_FeeFs1x_Prv_WriteJobTypes.h"

#include "rba_FeeFs1x_Prv_BlockJob.h"
#include "rba_FeeFs1x_Prv_BlockJobTypes.h"
#include "rba_FeeFs1x_Prv_BC.h"
#include "rba_FeeFs1x_Prv_Utils.h"

#include "Fee_Prv_Lib.h"
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

static rba_FeeFs1x_WrJob_JobData_tst             rba_FeeFs1x_WrJob_JobData_st;

static rba_FeeFs1x_WrJob_checkWr_data_tst        rba_FeeFs1x_WrJob_checkWr_data_st;

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"


/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_WrJob_checkWrReqDo_calcUserCRC(rba_FeeFs1x_WrJob_checkWr_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_WrJob_checkWrReqDo_comp2Config(rba_FeeFs1x_WrJob_checkWr_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_WrJob_checkWrReqDo_comp2DFLASH(rba_FeeFs1x_WrJob_checkWr_data_tst * fsm_pst);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 * rba_FeeFs1x_WrJob_cancel
 * Init function which is called during Fee_Init. Initialises all internal variables.
 * \param   none
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_WrJob_cancel(void)
{
    rba_FeeFs1x_WrJob_JobData_st.dataCRC_u32 = 0u;

    rba_FeeFs1x_WrJob_checkWr_data_st.state_en = rba_FeeFs1x_WrJob_checkWr_stm_idle_e;
}

/**
 *********************************************************************
 * rba_FeeFs1x_WrJob_initJob
 * function for passing the read relevant data from the job acceptor function in BlockJob unit
 * \param   orderStruct_pcst : details of the job order
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_WrJob_initJob(Fee_Prv_JobDesc_tst const * orderStruct_pcst)
{
    rba_FeeFs1x_WrJob_JobData_st.userbuffer_pcu8 = orderStruct_pcst->bfr_pcu8;
    rba_FeeFs1x_WrJob_JobData_st.feeIndex_u16 = orderStruct_pcst->blockNumber_u16;
    rba_FeeFs1x_WrJob_JobData_st.persID_u16 = orderStruct_pcst->idPers_u16;
    rba_FeeFs1x_WrJob_JobData_st.blkLen_u16 = orderStruct_pcst->length_u16;
    rba_FeeFs1x_WrJob_JobData_st.statusByte_u16 = orderStruct_pcst->statusFlag_u16;
    rba_FeeFs1x_WrJob_JobData_st.isUnknownBlk_b = orderStruct_pcst->isUnknownBlk_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_WrJob_checkWrReq
 * initializing the check for a write request
 * \param   retValSearch_en : result of the previously executed search
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_WrJob_checkWrReq(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    rba_FeeFs1x_WrJob_checkWr_data_st.retValSearch_en = retValSearch_en;
    rba_FeeFs1x_WrJob_checkWr_data_st.state_en = rba_FeeFs1x_WrJob_checkWr_stm_calcCRC_e;
    rba_FeeFs1x_WrJob_checkWr_data_st.entry_b = TRUE;
    rba_FeeFs1x_WrJob_checkWr_data_st.calcedBuffCRC_u32 = 0u;
}

/**
 *********************************************************************
 * rba_FeeFs1x_WrJob_checkWrReqDo
 * \return  job result indicating whether the actual write is required
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_JobOK_e   : actual write is not required, the latest copy matches the current buffer
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_execute_e : actual write needs to be executed
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e   : job aborted due to internal errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_WrJob_checkWrReqDo(void)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;
    switch(rba_FeeFs1x_WrJob_checkWr_data_st.state_en)
    {
        case rba_FeeFs1x_WrJob_checkWr_stm_calcCRC_e:
        {
            retVal_en = rba_FeeFs1x_WrJob_checkWrReqDo_calcUserCRC(&rba_FeeFs1x_WrJob_checkWr_data_st);
        }break;
        case rba_FeeFs1x_WrJob_checkWr_stm_compToConfig_e:
        {
            retVal_en = rba_FeeFs1x_WrJob_checkWrReqDo_comp2Config(&rba_FeeFs1x_WrJob_checkWr_data_st);
        }break;
        case rba_FeeFs1x_WrJob_checkWr_stm_compToDFLASH_e:
        {
            retVal_en = rba_FeeFs1x_WrJob_checkWrReqDo_comp2DFLASH(&rba_FeeFs1x_WrJob_checkWr_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
        }break;
    }

    if(retVal_en != rba_FeeFs1x_BlockJob_ReturnType_pending_e)
    {
        rba_FeeFs1x_WrJob_checkWr_data_st.state_en = rba_FeeFs1x_WrJob_checkWr_stm_idle_e;
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_WrJob_checkWrReq_NativeDo_calcUserCRC
 * statefunction of rba_FeeFs1x_WrJob_checkWrReq_NativeDo
 * \param   fsm_pst : statemachine data pointer
 * \return  job result to be returned by the statemachine
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_execute_e : actual write needs to be executed
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e   : job aborted due to internal errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_WrJob_checkWrReqDo_calcUserCRC(rba_FeeFs1x_WrJob_checkWr_data_tst * fsm_pst)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValCRC_en;

    if(FEE_PRV_LIBENTRY)
    {
        uint16 blkLength_u16;
        uint16 statusByte_u16;
        uint16 persID_u16;
        uint16 hdrCRC_u16;
        uint16 feeIndex_u16;

        // get the persistent ID, length and status byte from config
        if(!rba_FeeFs1x_WrJob_JobData_st.isUnknownBlk_b)
        {
            feeIndex_u16 = rba_FeeFs1x_WrJob_JobData_st.feeIndex_u16;
            persID_u16    = Fee_Prv_ConfigGetBlockPersistentIdByBlockNr(feeIndex_u16, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);
            blkLength_u16 = Fee_Prv_ConfigGetBlockLengthByBlockNr(feeIndex_u16, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);
            statusByte_u16 = Fee_Prv_ConfigGetBlockStatusBitsByBlockNr(feeIndex_u16, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);
        }
        else
        {
            // for unknown blocks, pick the information from the job order itself
            persID_u16 = rba_FeeFs1x_WrJob_JobData_st.persID_u16;
            blkLength_u16 = rba_FeeFs1x_WrJob_JobData_st.blkLen_u16;
            statusByte_u16 = rba_FeeFs1x_WrJob_JobData_st.statusByte_u16;
        }

        // calculate the initial CRC value for the current request
        hdrCRC_u16 = rba_FeeFs1x_BC_calcBlkHdrCRC(persID_u16, blkLength_u16, statusByte_u16);
        fsm_pst->calcedBuffCRC_u32 = ((uint32)hdrCRC_u16) ^ RBA_FEEFS1X_MAXUINT32;


        // start the data CRC calculation over the user buffer
        rba_FeeFs1x_Utils_computeUserBufferCrc(rba_FeeFs1x_WrJob_JobData_st.userbuffer_pcu8,
                                               blkLength_u16,
                                               &(fsm_pst->calcedBuffCRC_u32));
    }

    // proceed Cyclic calling the CRC do function for finishing the job
    retValCRC_en = rba_FeeFs1x_Utils_computeUserBufferCrcDo();

    // Evaluate the result of the CRC function
    switch (retValCRC_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            // CRC function needs more calls to finish
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;

        }break;
        case rba_FeeFs1x_JobOK_e:
        {
            // CRC calculation finished

            // copy the calculated CRC to the writejob structure for potential execWrites after checkWrReq
            rba_FeeFs1x_WrJob_JobData_st.dataCRC_u32 = fsm_pst->calcedBuffCRC_u32;

            // switch to next state
            switch(fsm_pst->retValSearch_en)
            {
                case rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e:
                {
                    retVal_en = rba_FeeFs1x_BlockJob_ReturnType_execute_e;
                }break;
                case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
                case rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e:
                {
                    FEE_PRV_LIBSC(rba_FeeFs1x_WrJob_checkWr_stm_compToConfig_e);
                    retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
                }break;
                default:
                {
                    retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
                }break;
            }


        }break;
        default:
        {
            // unexpected return value, e.g. errors during CRC calculation
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_WrJob_checkWrReq_NativeDo_comp2Config
 * statefunction of rba_FeeFs1x_WrJob_checkWrReq_NativeDo
 * \param   fsm_pst : statemachine data pointer
 * \return  job result to be returned by the statemachine
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_execute_e : actual write needs to be executed
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_WrJob_checkWrReqDo_comp2Config(rba_FeeFs1x_WrJob_checkWr_data_tst * fsm_pst)
{
    uint32 dataCRCInDFLASH_u32;
    boolean isMatchingConfig_b;
    boolean isInvBitSet_b;
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;
    uint16  confStatusByte_u16;

    // no libentry pattern used, because the state is supposed to only be called once

    confStatusByte_u16 = Fee_Prv_ConfigGetBlockStatusBitsByBlockNr(rba_FeeFs1x_WrJob_JobData_st.feeIndex_u16, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);

    dataCRCInDFLASH_u32 = rba_FeeFs1x_BC_getDataCRC(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
    isMatchingConfig_b = rba_FeeFs1x_BC_compare2Config(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e, confStatusByte_u16);
    isInvBitSet_b = rba_FeeFs1x_BC_getCpyInvBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);

    // extraction of data from BC successful

    // compare the user buffer data CRC to the one stored along with the DFLASH copy
    if( (fsm_pst->calcedBuffCRC_u32 == dataCRCInDFLASH_u32) &&
            (isMatchingConfig_b) && (!isInvBitSet_b) )
    {
        // The latest consistent copy in DFLASH matches the config and the CRC matches the one
        //calced over the user buffer.
        // -->To determine the necessity of writing, a manual data comparison is required
        FEE_PRV_LIBSC(rba_FeeFs1x_WrJob_checkWr_stm_compToDFLASH_e);
        retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
    }
    else
    {
        // data CRC differs or the DFLASH copy has status bits not matching the current config
        // --> executing the job is required
        retVal_en = rba_FeeFs1x_BlockJob_ReturnType_execute_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_WrJob_checkWrReq_NativeDo_comp2DFLASH
 * statefunction of rba_FeeFs1x_WrJob_checkWrReq_NativeDo
 * \param   fsm_pst : statemachine data pointer
 * \return  job result to be returned by the statemachine
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_JobOK_e : actual write is not required, the latest copy matches the current buffer
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_execute_e : actual write needs to be executed
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e : job aborted due to internal errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_WrJob_checkWrReqDo_comp2DFLASH(rba_FeeFs1x_WrJob_checkWr_data_tst * fsm_pst)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValCompare_en;
    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_BC_compareBuffer2Copy(rba_FeeFs1x_WrJob_JobData_st.userbuffer_pcu8, rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
    }

    // cyclic call of Do fct.
    retValCompare_en = rba_FeeFs1x_BC_compareBuffer2CopyDo();

    switch (retValCompare_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            // compare is ongoing, schedule again
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
        }break;

        case rba_FeeFs1x_JobOK_e:
        {
            // compare job has concluded that the user buffer is matching the DFLASH copy bytewise
            // --> write is not required, let the statemachine return that the write job is finished with job ok
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_JobOK_e;
        }break;

        case rba_FeeFs1x_JobFailed_e:
        {
            // compare job has detected a difference between user buffer and DFLASH copy
            // --> execute the write
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_execute_e;
        }break;

        default:
        {
            // unexpected return value; abor the job
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * prepare function for writing a native copy to the flash
 * \param   retValSearch_en : search result from searching for an already present block copy
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_WrJob_execWr_Native(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    // retValSearch is not required during a native copy write
    (void) retValSearch_en;

    rba_FeeFs1x_BlockJob_execWr(
            rba_FeeFs1x_WrJob_JobData_st.dataCRC_u32,
            rba_FeeFs1x_BlockJob_execWr_jobType_write,
            FALSE,       // native copy
            TRUE,        // isCopy1Destroyed_b
            TRUE         // isCopy2Destroyed_b
            );
}

/**
 *********************************************************************
 * rba_FeeFs1x_WrJob_execWr_Red
 * prepare function for writing a redundant copy to the flash
 * \param   retValSearch_en : result of the previously executed search
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
# if(RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
void rba_FeeFs1x_WrJob_execWr_Red(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    // retValSearch is not required during a redundant copy write
    (void) retValSearch_en;

    rba_FeeFs1x_BlockJob_execWr(
            rba_FeeFs1x_WrJob_JobData_st.dataCRC_u32,
            rba_FeeFs1x_BlockJob_execWr_jobType_write,
            TRUE,        // redundant copy
            TRUE,        // isCopy1Destroyed_b
            TRUE         // isCopy2Destroyed_b
    );
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_WrJob_execWr_NoFBNative
 * job init function for writing a native NoFB copy
 * \param   retValSearch_en : result of the previously executed search
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_WrJob_execWr_NoFBNative(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    boolean hasNoFBBitActive_b;
    boolean isCopy2Destroyed_b;

    if(rba_FeeFs1x_Prv_isNoFallbackActive())
    {
        if(rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e == retValSearch_en)
        {
            isCopy2Destroyed_b = TRUE;
        }
        else
        {
            hasNoFBBitActive_b = rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
            /* do not try to write the noFallback marker (destroy state), if block is not a noFallback block
               -> if block is a noFallback block, isCopy2Destroyed_b = TRUE else isCopy2Destroyed_b = FALSE
               this could be the case, if formerly no noFallback blocks are reconfigured to noFallback blocks
               an error will occur due to write access to already written last page of previous block */
            isCopy2Destroyed_b = !hasNoFBBitActive_b;
        }

        rba_FeeFs1x_BlockJob_execWr(
                rba_FeeFs1x_WrJob_JobData_st.dataCRC_u32,
                rba_FeeFs1x_BlockJob_execWr_jobType_write,
                FALSE,              // native copy
                TRUE,               // isCopy1Destroyed_b
                isCopy2Destroyed_b
                );
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_WrJob_execWr_NoFBRed
 * job init function for writing a redundant noFallback copy
 * \param   retValSearch_en : result of the previously executed search
 * \seealso
 * \usedresources
 *********************************************************************
 */
# if(RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS != 0u)
void rba_FeeFs1x_WrJob_execWr_NoFBRed(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
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
                /* do not try to write the noFallback marker (destroy state), if block is not a noFallback block
                   -> if block is a noFallback block, isCopy2Destroyed_b = TRUE else isCopy2Destroyed_b = FALSE
                   this could be the case, if formerly no noFallback blocks are reconfigured to noFallback blocks
                   an error will occur due to write access to already written last page of previous block */
                isCopy2Destroyed_b = !hasNoFBBitActive_b;
            }break;

            case rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e:
            {
                hasNoFBBitActive_b = rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Redundant_e);
                /* do not try to write the noFallback marker (destroy state), if block is not a noFallback block
                   -> if block is a noFallback block, isCopy1Destroyed_b = TRUE else isCopy1Destroyed_b = FALSE
                   this could be the case, if formerly no noFallback blocks are reconfigured to noFallback blocks
                   an error will occur due to write access to already written last page of previous block */
                isCopy1Destroyed_b = !hasNoFBBitActive_b;

                hasNoFBBitActive_b = rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
                /* do not try to write the noFallback marker (destroy state), if block is not a noFallback block
                   -> if block is a noFallback block, isCopy2Destroyed_b = TRUE else isCopy2Destroyed_b = FALSE
                   this could be the case, if formerly no noFallback blocks are reconfigured to noFallback blocks
                   an error will occur due to write access to already written last page of previous block */
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
                rba_FeeFs1x_WrJob_JobData_st.dataCRC_u32,
                rba_FeeFs1x_BlockJob_execWr_jobType_write,
                TRUE,               // redundant copy
                isCopy1Destroyed_b,
                isCopy2Destroyed_b
                );
    }
}
# endif

# if((TRUE == RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLOCK_WRITE) || (TRUE == RBA_FEEFS1X_PRV_CFG_UNKNOWN_SURVIVAL_BLOCK_WRITE))
/**
 *********************************************************************
 * prepare function for writing a unknown block
 * \param   retValSearch_en : search result from searching for an already present block copy
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_WrJob_execWr_UnknownBlk(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    boolean isRed_b = FALSE;

    // retValSearch is not required during a unknown block copy write
    (void) retValSearch_en;

    // check if the unknown block is redundant
    if(0u != (rba_FeeFs1x_WrJob_JobData_st.statusByte_u16 & RBA_FEEFS1X_PRV_CFG_DOUBLESTORAGE_BITS))
    {
        isRed_b = TRUE;
    }

    rba_FeeFs1x_BlockJob_execWr(
            rba_FeeFs1x_WrJob_JobData_st.dataCRC_u32,
            rba_FeeFs1x_BlockJob_execWr_jobType_write,
            isRed_b,     // native or redundant copy
            TRUE,        // to keep implementation simple, the old nofallback copies are not invalidated.
            TRUE         // to keep implementation simple, the old nofallback copies are not invalidated.
            );
}
# endif


#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

