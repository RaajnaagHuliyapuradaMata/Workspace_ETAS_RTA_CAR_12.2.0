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

#include"rba_FeeFs1x_Prv_Cfg.h"

#if (RBA_FEEFS1X_PRV_CFG_MAINTAIN != FALSE)
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_FeeFs1x_Prv_Rb_MaintainJob.h"
#include "rba_FeeFs1x_Prv_Rb_MaintainJobTypes.h"

#include "rba_FeeFs1x_Prv_BlockJob.h"
#include "rba_FeeFs1x_Prv_BlockJobTypes.h"
#include "rba_FeeFs1x_Prv_BC.h"
#include "Fee_Prv_Lib.h"

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
static rba_FeeFs1x_MtJob_JobData_tst            rba_FeeFs1x_MtJob_JobData_st;           // Maintain job working variable

static rba_FeeFs1x_MtJob_checkMt_tst            rba_FeeFs1x_MtJob_checkMt_st;           // Check maintain operation variable

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"


/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

// state functions for check maintain do state machine
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_MtJob_checkMtReqDo_chkSearchResultCompToCfg(rba_FeeFs1x_MtJob_checkMt_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_MtJob_checkMtReqDo_cmpCopies(rba_FeeFs1x_MtJob_checkMt_tst * fsm_pst);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 * rba_FeeFs1x_MtJob_cancel
 * Init function which is called during Fee_Init. Initialises all internal variables.
 * \param   none
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_MtJob_cancel(void)
{
    // initialise check maintain structure
    rba_FeeFs1x_MtJob_checkMt_st.state_en = rba_FeeFs1x_MtJob_checkMt_stm_idle_e;
}

/**
 *********************************************************************
 * rba_FeeFs1x_MtJob_initJob
 * function for passing the maintain job relevant data from the job acceptor function in BlockJob unit
 * \param   feeIndex_u16 : fee index of the block which has to be maintained
 * \retval  none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_MtJob_initJob(uint16 feeIndex_u16)
{
    rba_FeeFs1x_MtJob_JobData_st.feeIndex_u16 = feeIndex_u16;
}

/**
 *********************************************************************
 * rba_FeeFs1x_MtJob_checkMtReq
 * This function stores the search result of the previous step and sets the state machine to evaluate the results.
 *
 * \param   searchresult_en : result of the previously executed search
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_MtJob_checkMtReq(rba_FeeFs1x_Searcher_RetVal_ten searchresult_en)
{
    rba_FeeFs1x_MtJob_checkMt_st.state_en = rba_FeeFs1x_MtJob_checkMt_stm_chkSearchResultCompToCfg_e;
    rba_FeeFs1x_MtJob_checkMt_st.entry_b = TRUE;
    rba_FeeFs1x_MtJob_checkMt_st.retValSearch_en = searchresult_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_MtJob_checkMtReqDo
 * This function evaluates if the block copies are consistent and same. If not, it triggers execute action to
 * store the new copies.
 *
 * \return  job result indicating whether the actual maintain is required or not
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_JobOK_e   : actual maintain is not required, both copies are consistent and same
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_execute_e : actual maintain needs to be executed
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e   : job aborted due to internal errors like no valid copies found during search
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_MtJob_checkMtReqDo(void)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;

    switch(rba_FeeFs1x_MtJob_checkMt_st.state_en)
    {
        case rba_FeeFs1x_MtJob_checkMt_stm_chkSearchResultCompToCfg_e:
        {
            retVal_en = rba_FeeFs1x_MtJob_checkMtReqDo_chkSearchResultCompToCfg(&rba_FeeFs1x_MtJob_checkMt_st);
        }break;
        case rba_FeeFs1x_MtJob_checkMt_stm_cmpCopies_e:
        {
            retVal_en = rba_FeeFs1x_MtJob_checkMtReqDo_cmpCopies(&rba_FeeFs1x_MtJob_checkMt_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
        }break;
    }

    if(rba_FeeFs1x_BlockJob_ReturnType_pending_e != retVal_en)
    {
        // initialise check maintain structure
        rba_FeeFs1x_MtJob_checkMt_st.state_en = rba_FeeFs1x_MtJob_checkMt_stm_idle_e;
    }

    return  retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_MtJob_checkMtReqDo_chkSearchResultCompToCfg
 * statefunction of rba_FeeFs1x_MtJob_checkMtReqDo.
 * Function checks for the search result and checks if there was any change in the configuration
 * between the latest consistent copy in DFLASH and configuration.
 *
 * \return  job result to be returned by the statemachine
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e : job aborted due to internal errors like no copies found during search
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_MtJob_checkMtReqDo_chkSearchResultCompToCfg(rba_FeeFs1x_MtJob_checkMt_tst * fsm_pst)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;
    uint16                              confStatusByte_u16;
    boolean                             isMatchingConfig_b;

    if(rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e != fsm_pst->retValSearch_en)
    {
        /* maintain is only called, if double storage is configured (native and redundant)
           If single storage is configured, the job is already rejected on Fee level
           If only one copy is found, the redundant copy may be missing, so maintain shall be executed  */
        confStatusByte_u16 = Fee_Prv_ConfigGetBlockStatusBitsByBlockNr(rba_FeeFs1x_MtJob_JobData_st.feeIndex_u16, rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast);
        isMatchingConfig_b = rba_FeeFs1x_BC_compare2Config(
                                    rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e,      //BlockJob_Latest is initialized by Searcher
                                    confStatusByte_u16);

        if(isMatchingConfig_b)
        {
            if(rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e == fsm_pst->retValSearch_en)
            {
                // native copy is matching the config, but redundant copy is missing -> write redundant copy
                rba_FeeFs1x_MtJob_JobData_st.writeTwice_b = FALSE;
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_execute_e;
            }
            else
            {
                // configuration matches, check if data are same
                FEE_PRV_LIBSC(rba_FeeFs1x_MtJob_checkMt_stm_cmpCopies_e);
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
            }
        }
        else
        {
            // configurations are different, set the flag to indicate that the block has to be written twice and execute job has to be performed
            rba_FeeFs1x_MtJob_JobData_st.writeTwice_b = TRUE;
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_execute_e;
        }
    }
    else
    {
        // no copy is found, copy may not be recovered -> return an error
        retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_MtJob_checkMtReqDo_cmpCopies
 * statefunction of rba_FeeFs1x_MtJob_checkMtReqDo. Function triggers the byte by byte comparision for both the found copies.
 * Both the header and data are compared. Comparing header would also check if the 2 copies have different status flags.
 *
 * \return  job result to be returned by the statemachine
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e   : comparision ongoing
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_execute_e   : blocks are different, maintain has to be performed
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_JobOK_e     : blocks are same, no maintain to be performed
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e     : job aborted due to internal errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_MtJob_checkMtReqDo_cmpCopies(rba_FeeFs1x_MtJob_checkMt_tst * fsm_pst)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValCompare_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_BC_compareFls2Fls(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e, rba_FeeFs1x_BC_Cpy_BlockJobs_Redundant_e);
    }

    retValCompare_en = rba_FeeFs1x_BC_compareFls2FlsDo();

    switch (retValCompare_en)
    {
        case rba_FeeFs1x_Pending_e:
        {
            // compare is ongoing, schedule again
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
        }break;

        case rba_FeeFs1x_JobOK_e:
        {
            // compare job has concluded that the two copies are same including status flags
            // maintain is not required, let the statemachine return that the write job is finished with job ok
            // finalise the copies before exit
            rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
            rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_BlockJobs_Redundant_e);
            // finalizations successful
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_JobOK_e;

        }break;

        case rba_FeeFs1x_JobFailed_e:
        {
            // compare job has detected a difference between native and redundant DFLASH copy
            // --> execute the write only once
            rba_FeeFs1x_MtJob_JobData_st.writeTwice_b = FALSE;
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_execute_e;
        }break;

        default:
        {
            // return value not evaluated, because in any case an error is returned
            rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
            rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_BlockJobs_Redundant_e);

            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_MtJob_execMt
 * This function sets the state machine to execute the maintain job.
 *
 * \param   searchresult_en : result of the previously executed search (not used during execute step)
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_MtJob_execMt(rba_FeeFs1x_Searcher_RetVal_ten searchresult_en)
{
    // retValSearch is not required during execute maintain operation
    (void) searchresult_en;

    // do not destroy the copies in case of no NoFB
    rba_FeeFs1x_BlockJob_execWrMaintain(
            TRUE,        // isCopy1Destroyed_b
            TRUE,        // isCopy2Destroyed_b
            FALSE,       // isNoFB_b
            rba_FeeFs1x_MtJob_JobData_st.writeTwice_b
    );
}

/**
 *********************************************************************
 * rba_FeeFs1x_MtJob_execMt_NoFB
 * This function sets the state machine to execute the maintain job for no follback blocks.
 *
 * \param   searchresult_en : result of the previously executed search (not used during execute step)
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_MtJob_execMt_NoFB(rba_FeeFs1x_Searcher_RetVal_ten searchresult_en)
{
    boolean hasNoFBBitActive_b;
    boolean isCopy1Destroyed_b;
    boolean isCopy2Destroyed_b;

    if(rba_FeeFs1x_Prv_isNoFallbackActive())
    {
        switch(searchresult_en)
        {
            case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
            {
                // only one copy is found, this is the latest (copy2), so never access the redundant copy (copy1)
                // -> isCopy1Destroyed_b must be TRUE
                if(rba_FeeFs1x_MtJob_JobData_st.writeTwice_b)
                {
                    // 1 copy,   native copy config mismatch    -> init1,           write1, init2, destroy2, write2
                    hasNoFBBitActive_b =  rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);

                    /* do not try to write the noFallback marker (destroy state), if block is not a noFallback block
                       -> if block is a noFallback block, isCopy1Destroyed_b = TRUE else isCopy1Destroyed_b = FALSE
                       this could be the case, if formerly no noFallback blocks are reconfigured to noFallback blocks
                       an error will occur due to write access to already written last page of previous block */
                    isCopy2Destroyed_b = !hasNoFBBitActive_b;
                    isCopy1Destroyed_b = TRUE;
                }
                else
                {
                    // 1 copy,   native copy config OK          -> init1,          write1
                    isCopy1Destroyed_b = TRUE;
                    isCopy2Destroyed_b = TRUE;
                }
            }break;
            case rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e:
            {
                if(rba_FeeFs1x_MtJob_JobData_st.writeTwice_b)
                {
                    // 2 copies, native copy config mismatch    -> init1, destroy1, write1, init2, destroy2, write2
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
                }
                else
                {
                    // 2 copies, data mismatch                  -> init1, destroy1, write1
                    hasNoFBBitActive_b = rba_FeeFs1x_BC_getCpyNoFBBitActive(rba_FeeFs1x_BC_Cpy_BlockJobs_Redundant_e);
                    /* do not try to write the noFallback marker (destroy state), if block is not a noFallback block
                       -> if block is a noFallback block, isCopy1Destroyed_b = TRUE else isCopy1Destroyed_b = FALSE
                       this could be the case, if formerly no noFallback blocks are reconfigured to noFallback blocks
                       an error will occur due to write access to already written last page of previous block */
                    isCopy1Destroyed_b = !hasNoFBBitActive_b;
                    isCopy2Destroyed_b = TRUE;
                }
            }break;
            default:
            {
                isCopy1Destroyed_b = TRUE;
                isCopy2Destroyed_b = TRUE;
            }break;
        }

        rba_FeeFs1x_BlockJob_execWrMaintain(
                isCopy1Destroyed_b,
                isCopy2Destroyed_b,
                TRUE,       // isNoFB_b
                rba_FeeFs1x_MtJob_JobData_st.writeTwice_b
        );
    }
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif //FEE_RB_MAINTAIN

#endif

