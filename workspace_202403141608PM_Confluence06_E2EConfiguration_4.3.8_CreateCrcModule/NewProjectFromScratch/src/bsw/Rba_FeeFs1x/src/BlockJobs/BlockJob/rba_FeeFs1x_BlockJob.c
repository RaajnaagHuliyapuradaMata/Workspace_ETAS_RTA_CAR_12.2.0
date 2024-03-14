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
#include "rba_FeeFs1x.h"
#include "rba_FeeFs1x_Prv_Cfg.h"

#include "rba_FeeFs1x_Prv_BlockJob.h"
#include "rba_FeeFs1x_Prv_BlockJobTypes.h"
#include "rba_FeeFs1x_Prv_Searcher.h"
#include "rba_FeeFs1x_Prv_SearcherTypes.h"

#include "Fee_Prv_Lib.h"
#include "Fee_Rb_Types.h"

#include "rba_FeeFs1x_Prv_BC.h"
#include "rba_FeeFs1x_Prv_CacheKwn.h"
#include "rba_FeeFs1x_Prv_CacheUnkwn.h"
#include "rba_FeeFs1x_Prv_ReadJob.h"
#include "rba_FeeFs1x_Prv_WriteJob.h"
#include "rba_FeeFs1x_Prv_InvalidateJob.h"
#include "rba_FeeFs1x_Prv_Rb_MaintainJob.h"
#include "rba_FeeFs1x_Prv_Utils.h"

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
 * Prototypes
 **********************************************************************************************************************
 */
#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

/* this function has to be declared here before initialization of rba_FeeFs1x_BlockJob_jobDoFunctions_apfct */
static rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_execWrDo(void);

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define FEE_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

RBA_FEEFS1X_VAR_SCOPE(rba_FeeFs1x_BlockJob_execWr_data_tst rba_FeeFs1x_BlockJob_execWr_data_st)
RBA_FEEFS1X_VAR_SCOPE(rba_FeeFs1x_BlockJobData_tst rba_FeeFs1x_BlockJobData_st)

# if(defined(RBA_FEEFS1X_PRV_CFG_DETAILED_BLK_INFO_API) && (TRUE == RBA_FEEFS1X_PRV_CFG_DETAILED_BLK_INFO_API))
static MemIf_Rb_DetailedBlkInfo_ten rba_FeeFs1x_BlockJob_ReadJobResultDetailed_aen[RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS];
# endif

static boolean rba_FeeFs1x_BlockJob_BlkDataCrcError_b;

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"



#define FEE_START_SEC_CONST_UNSPECIFIED
#include "Fee_MemMap.h"
// DO NOT CHANGE WITHOUT MATCHING THE ENUM rba_FeeFs1x_BlockJob_jobFunc_ten
static const rba_FeeFs1x_BlockJob_stateFunctionPointer_tpfct rba_FeeFs1x_BlockJob_jobFunctions_apfct[] =
{
        // read functionality
        rba_FeeFs1x_RdJob_checkRdReq                ,       // rba_FeeFs1x_BlockJob_jobFunc_checkRd_e = 0u,
        rba_FeeFs1x_RdJob_execRd                    ,       // rba_FeeFs1x_BlockJob_jobFunc_execRd_e

        // write functionality
        rba_FeeFs1x_WrJob_checkWrReq                ,       // rba_FeeFs1x_BlockJob_jobFunc_checkWr_e
        rba_FeeFs1x_WrJob_execWr_Native             ,       // rba_FeeFs1x_BlockJob_jobFunc_execWr_e
        //
        rba_FeeFs1x_WrJob_execWr_Red                ,       // rba_FeeFs1x_BlockJob_jobFunc_execWrRed_e
        //
        rba_FeeFs1x_WrJob_execWr_NoFBNative         ,       //  rba_FeeFs1x_BlockJob_jobFunc_execWrNoFB_e,
        rba_FeeFs1x_WrJob_execWr_NoFBRed            ,       //  rba_FeeFs1x_BlockJob_jobFunc_execWrNoFBRed_e,

        rba_FeeFs1x_WrJob_execWr_UnknownBlk         ,       //  rba_FeeFs1x_BlockJob_jobFunc_execWrUnknownBlk_e,

        // invalidate functionality                         //
        rba_FeeFs1x_InvJob_checkInvReq              ,       // rba_FeeFs1x_BlockJob_jobFunc_checkInv_e
        rba_FeeFs1x_InvJob_execInv_Native           ,       // rba_FeeFs1x_BlockJob_jobFunc_execInv_e

        rba_FeeFs1x_InvJob_execInv_Red              ,       // rba_FeeFs1x_BlockJob_jobFunc_execInvRed_e

        rba_FeeFs1x_InvJob_execInv_NoFBNative       ,       // rba_FeeFs1x_BlockJob_jobFunc_execInvNoFB_e,
        rba_FeeFs1x_InvJob_execInv_NoFBRed          ,       // rba_FeeFs1x_BlockJob_jobFunc_execInvNoFBRed_e,

        rba_FeeFs1x_InvJob_execInv_UnknownBlk       ,      //  rba_FeeFs1x_BlockJob_jobFunc_execInvUnknownBlk_e,

        // maintain functionality
        rba_FeeFs1x_MtJob_checkMtReq                ,       // rba_FeeFs1x_BlockJob_jobFunc_checkMt_e
        rba_FeeFs1x_MtJob_execMt                    ,       // rba_FeeFs1x_BlockJob_jobFunc_execMt_e

        rba_FeeFs1x_MtJob_execMt_NoFB                       // rba_FeeFs1x_BlockJob_jobFunc_execMtNoFB_e,
};
// DO NOT CHANGE WITHOUT MATCHING THE ENUM rba_FeeFs1x_BlockJob_jobDoFunc_ten
static const rba_FeeFs1x_BlockJob_stateDoFunctionPointer_tpfct rba_FeeFs1x_BlockJob_jobDoFunctions_apfct[] =
{
        // read functionality
        rba_FeeFs1x_RdJob_checkRdReqDo              ,        // rba_FeeFs1x_BlockJob_jobDoFunc_checkRdDo_e = 0u,
        rba_FeeFs1x_RdJob_execRdDo                  ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execRdDo_e

        // write functionality
        rba_FeeFs1x_WrJob_checkWrReqDo              ,        // rba_FeeFs1x_BlockJob_jobDoFunc_checkWrDo_e

        rba_FeeFs1x_BlockJob_execWrDo               ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execWrDo_e
        rba_FeeFs1x_BlockJob_execWrDo               ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execWrRedDo_e
        rba_FeeFs1x_BlockJob_execWrDo               ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execWrNoFBDo_e,
        rba_FeeFs1x_BlockJob_execWrDo               ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execWrNoFBRedDo_e,
        rba_FeeFs1x_BlockJob_execWrDo               ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execWrUnknownBlk_e,

        // invalidate functionality
        rba_FeeFs1x_InvJob_checkInvReqDo            ,        // rba_FeeFs1x_BlockJob_jobDoFunc_checkInvDo_e

        rba_FeeFs1x_BlockJob_execWrDo               ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execInvDo_e
        rba_FeeFs1x_BlockJob_execWrDo               ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execInvRedDo_e
        rba_FeeFs1x_BlockJob_execWrDo               ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execInvNoFBDo_e,
        rba_FeeFs1x_BlockJob_execWrDo               ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execInvNoFBRedDo_e,
        rba_FeeFs1x_BlockJob_execWrDo               ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execInvUnknownBlkDo_e,

        // maintain functionality
        rba_FeeFs1x_MtJob_checkMtReqDo              ,        // rba_FeeFs1x_BlockJob_jobDoFunc_checkMtDo_e

        rba_FeeFs1x_BlockJob_execWrDo               ,        // rba_FeeFs1x_BlockJob_jobDoFunc_execMtDo_e
        rba_FeeFs1x_BlockJob_execWrDo                        // rba_FeeFs1x_BlockJob_jobDoFunc_execMtNoFBDo_e
};

#define FEE_STOP_SEC_CONST_UNSPECIFIED
#include "Fee_MemMap.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJobDo_search(rba_FeeFs1x_BlockJobData_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJobDo_checkreq(rba_FeeFs1x_BlockJobData_tst * const fsm_pst);
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJobDo_execAction(rba_FeeFs1x_BlockJobData_tst * const fsm_pst);

/* execWr_Do functions */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_execWrDo_init(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_execWrDo_destroy(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_execWrDo_write(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_execWrDo_error(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BlockJob_execWrDo_init_initBC(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BlockJob_execWrDo_destroy_destroyBC(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BlockJob_execWrDo_write_writeBC(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BlockJob_execWrDo_write_invalidateBC(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BlockJob_execWrDo_write_maintainBC(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst);

LOCAL_INLINE MemIf_JobResultType rba_FeeFs1x_BlockJob_mapBlockJobRetToMemifRet(rba_FeeFs1x_BlockJob_ReturnType_ten bjRetVal_en);

static void rba_FeeFs1x_BlockJob_callJobFunc(rba_FeeFs1x_BlockJob_jobFunc_ten selectedFunc_en,
                                             rba_FeeFs1x_Searcher_RetVal_ten searchRetVal_en);

static rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_callJobDoFunc(
                                             rba_FeeFs1x_BlockJob_jobDoFunc_ten selectedFunc_en);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_jobDummy
 * Dummy function to fill non-existing entry in rba_FeeFs1x_BlockJob_jobFunctions_apfct
 *
 * \param   retValSearch_en : result of the previously executed search
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
#  if((FALSE == RBA_FEEFS1X_PRV_CFG_MAINTAIN)    || \
      (RBA_FEEFS1X_PRV_CFG_NR_RDNT_BLOCKS == 0u) || \
      ((FALSE == RBA_FEEFS1X_PRV_CFG_UNKNOWN_BLOCK_WRITE)&&(FALSE == RBA_FEEFS1X_PRV_CFG_UNKNOWN_SURVIVAL_BLOCK_WRITE)))
void rba_FeeFs1x_BlockJob_jobDummy(rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en)
{
    (void) retValSearch_en;
    return; /* Do nothing */
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_jobDoDummy
 * Dummy function to fill non-existing entry in rba_FeeFs1x_BlockJob_jobDoFunctions_apfct
 *
 * \return  job result
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e : job aborted due to internal error
 * \seealso
 * \usedresources
 *********************************************************************
 */
# if (FALSE == RBA_FEEFS1X_PRV_CFG_MAINTAIN)
rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_jobDoDummy(void)
{
    return rba_FeeFs1x_BlockJob_ReturnType_error_e; /* This function should never be called, return with error */
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_init
 * resets all internal variables of the BlockJob unit
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BlockJob_init(void)
{
# if(defined(RBA_FEEFS1X_PRV_CFG_DETAILED_BLK_INFO_API) && (TRUE == RBA_FEEFS1X_PRV_CFG_DETAILED_BLK_INFO_API))

    uint16_least idxBlk_qu16;

    for(idxBlk_qu16 = 0u; idxBlk_qu16 < RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS; idxBlk_qu16++)
    {
        rba_FeeFs1x_BlockJob_ReadJobResultDetailed_aen[idxBlk_qu16] = MEMIF_RB_DETAILED_BLK_INFO_INIT_E;
    }

# endif

    rba_FeeFs1x_BlockJob_cancel();
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_cancel
 * resets relevant internal variables of the BlockJob unit
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BlockJob_cancel(void)
{
    // reset internal variables
    rba_FeeFs1x_BlockJobData_st.state_en          = rba_FeeFs1x_BlockJob_stm_idle_e;
    rba_FeeFs1x_BlockJobData_st.nrDataBytDone_u16 = 0u;
    rba_FeeFs1x_BlockJob_execWr_data_st.state_en  = rba_FeeFs1x_BlockJob_execWr_stm_idle_e;
}
/**
 *********************************************************************
 * rba_FeeFs1x_BlockJobDo
 * This is the do function for BlockJob operation.
 * The function also updates the detailed block status information for known blocks.
 * \return  Returns how "well" the job is finished
 * \retval MEMIF_JOB_OK            : job executed correctly
 * \retval MEMIF_JOB_FAILED        : job caused an error
 * \retval MEMIF_JOB_PENDING       : job ongoing, continue during next mainfunction call
 * \retval MEMIF_JOB_CANCELED      :
 * \retval MEMIF_BLOCK_INCONSISTENT: blockjob(read) didn't find a block copy
 * \retval MEMIF_BLOCK_INVALID     : blockjob(read) found an explicit block invalidation marker
 * \seealso
 * \usedresources
 *********************************************************************
 */
MemIf_JobResultType rba_FeeFs1x_BlockJobDo(void)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten stInnr_en;
    MemIf_JobResultType                 stExt_en;

    switch(rba_FeeFs1x_BlockJobData_st.state_en)
    {
        case rba_FeeFs1x_BlockJob_stm_search_e:
        {
            stInnr_en = rba_FeeFs1x_BlockJobDo_search(&rba_FeeFs1x_BlockJobData_st);
            break;
        }
        case rba_FeeFs1x_BlockJob_stm_checkReq_e:
        {
            stInnr_en = rba_FeeFs1x_BlockJobDo_checkreq(&rba_FeeFs1x_BlockJobData_st);
            break;
        }
        case rba_FeeFs1x_BlockJob_stm_execJob_e:
        {
            stInnr_en = rba_FeeFs1x_BlockJobDo_execAction(&rba_FeeFs1x_BlockJobData_st);
            break;
        }
        default:
        {
            // unexpected state value
            stInnr_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
            break;
        }
    }

    if(rba_FeeFs1x_BlockJob_ReturnType_pending_e != stInnr_en)
    {
        rba_FeeFs1x_BlockJobData_st.state_en           = rba_FeeFs1x_BlockJob_stm_idle_e;

# if(defined(RBA_FEEFS1X_PRV_CFG_DETAILED_BLK_INFO_API) && (TRUE == RBA_FEEFS1X_PRV_CFG_DETAILED_BLK_INFO_API))
        if((rba_FeeFs1x_BlockJob_ReturnType_inconsistent_e == stInnr_en) &&
           (RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS > rba_FeeFs1x_BlockJobData_st.feeIndex_u16))
        {
            if(rba_FeeFs1x_BlockJob_BlkDataCrcError_b)
            {
                /* Block Data CRC of block is wrong */
                rba_FeeFs1x_BlockJob_ReadJobResultDetailed_aen[rba_FeeFs1x_BlockJobData_st.feeIndex_u16] =
                                                                    MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_DATA_E;
            }
            else
            {
                /* Block is not present/readable on the medium */
                rba_FeeFs1x_BlockJob_ReadJobResultDetailed_aen[rba_FeeFs1x_BlockJobData_st.feeIndex_u16] =
                                                                    MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_MISSING_E;
            }
        }
# endif
    }

    stExt_en = rba_FeeFs1x_BlockJob_mapBlockJobRetToMemifRet(stInnr_en);

    return stExt_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJobDo_search
 * statefunction of blockJobDo
 * \param   fsm_pst : reference to statemachine data
 * \return  value to be returned by statemachine
 * \retval rba_FeeFs1x_BlockJob_ReturnType_pending_e      : job is not yet finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_execute_e      : exec action has to be perfomed
 * \retval rba_FeeFs1x_BlockJob_ReturnType_JobOK_e        : data is verified and validated, job is finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_inconsistent_e : data is corrupted , job is finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_invalidated_e  :consistent block copy is invalidated, job finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_error_e        : error during processing, job aborted
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJobDo_search(rba_FeeFs1x_BlockJobData_tst * const fsm_pst)
{
    rba_FeeFs1x_Searcher_RetVal_ten retValSearch_en;
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        // depending on the required search type, call the respective search job init function
        switch(fsm_pst->idxSearchType_en)
        {
            case rba_FeeFs1x_BlockJob_searchType_F1C_e:
            {
                rba_FeeFs1x_Searcher_find1LatestConsistentCopy_Known(fsm_pst->feeIndex_u16);
                break;
            }
            case rba_FeeFs1x_BlockJob_searchType_F2C_e:
            {
                rba_FeeFs1x_Searcher_find2LatestCopies_Known(fsm_pst->feeIndex_u16);
                break;
            }
            case rba_FeeFs1x_BlockJob_searchType_F1UC_e:
            {
                if(rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e == fsm_pst->searchRetVal_en)
                {
                    rba_FeeFs1x_Searcher_findFirst1LatestUncheckedCopy_Known(fsm_pst->feeIndex_u16);
                }
                else
                {
                    rba_FeeFs1x_Searcher_findNext1LatestUncheckedCopy_Known(fsm_pst->feeIndex_u16);
                }
                break;
            }
            default:
            {
                /* MR12 DIR 16.4 VIOLATION: default statement should never be reached */
                break;
            }
        }
    }

    // Cyclic call of Do fct
    retValSearch_en = rba_FeeFs1x_Searcher_findCopiesDo();

    switch(retValSearch_en)
    {
        case rba_FeeFs1x_Searcher_RetVal_Pending_e:
        {
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
            break;
        }
        case rba_FeeFs1x_Searcher_RetVal_OneCopyFound_e:
        case rba_FeeFs1x_Searcher_RetVal_TwoCopiesFound_e:
        case rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e:
        {
            // Store the search result and switch to checking the execution necessity
            fsm_pst->searchRetVal_en = retValSearch_en;
            FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_stm_checkReq_e);
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;

            break;
        }
        case rba_FeeFs1x_Searcher_RetVal_ErrorExternal_e:
        {
            // in case of a flash read error, retries shall be executed.
            // Searcher does not make a distinction between read errors and other errors like OOB checks.
            // So on any error a retry will be done, although this would not be required
            if(fsm_pst->allowedRetries_u8 > 0)
            {
                // restart the search
                FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_stm_search_e);
                fsm_pst->allowedRetries_u8--;
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
            }
            else
            {
                // all retries failed - no copies found - continue with next step:
                // read/maintain -> exit in check state, write/invalidate -> write will be attempted.
                fsm_pst->searchRetVal_en = rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e;
                FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_stm_checkReq_e);
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
            }
            break;
        }
        case rba_FeeFs1x_Searcher_RetVal_ErrorInternal_e:
        default:
        {
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
            break;
        }
    }

    return retVal_en;

}
/**
 *********************************************************************
 * rba_FeeFs1x_BlockJobDo_checkreq
 * statefunction of blockJobDo
 * \param   fsm_pst : reference to statemachine data
 * \return  value to be returned by statemachine
 * \retval rba_FeeFs1x_BlockJob_ReturnType_pending_e      : job is not yet finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_execute_e      : exec action has to be perfomed
 * \retval rba_FeeFs1x_BlockJob_ReturnType_JobOK_e        : data is verified and validated, job is finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_inconsistent_e : data is corrupted , job is finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_invalidated_e  :consistent block copy is invalidated, job finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_error_e        : error during processing, job aborted
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJobDo_checkreq(rba_FeeFs1x_BlockJobData_tst * const fsm_pst)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retValInner_en;
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_BlockJob_callJobFunc(fsm_pst->idxCheckFunc_en , fsm_pst->searchRetVal_en);
    }

    // cyclic call of Do fct.
    retValInner_en = rba_FeeFs1x_BlockJob_callJobDoFunc(fsm_pst->idxCheckDoFunc_en);

    if(rba_FeeFs1x_BlockJob_ReturnType_execute_e == retValInner_en)
    {
        // if the check results in the necessity / possiblilty to execute the job action, switch to the respective state
        FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_stm_execJob_e);

        // change the return value to pending to signal that the activity needs more cycles
        retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
    }
    else
    {
        // All other return values are handled at state machine
        retVal_en = retValInner_en;
    }

    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_BlockJobDo_execAction
 * statefunction of blockJobDo
 * \param   fsm_pst : reference to statemachine data
 * \return  value to be returned by statemachine
 * \retval rba_FeeFs1x_BlockJob_ReturnType_pending_e      : job is not yet finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_execute_e      : exec action has to be perfomed
 * \retval rba_FeeFs1x_BlockJob_ReturnType_JobOK_e        : data is verified and validated, job is finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_inconsistent_e : data is corrupted , job is finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_invalidated_e  :consistent block copy is invalidated, job finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_error_e        : error during processing, job aborted
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJobDo_execAction(rba_FeeFs1x_BlockJobData_tst * const fsm_pst)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retValInner_en;
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_BlockJob_callJobFunc(fsm_pst->idxExecFunc_en , fsm_pst->searchRetVal_en);
    }

    // cyclic call of Do fct.
    retValInner_en = rba_FeeFs1x_BlockJob_callJobDoFunc(fsm_pst->idxExecDoFunc_en);

    if(rba_FeeFs1x_BlockJob_ReturnType_restart_e == retValInner_en)
    {
        // if the exec action got to the conclusion to restart the whole blockjob, restart from search
        FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_stm_search_e);
        retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
    }
    else
    {
        retVal_en = retValInner_en;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWr
 * prepare function for writing a copy, including the special case of writing an invalidated copy
 *
 * write a copy:
 * security level  actions
 *    redundant    1) if isCopy1Destroyed_b = TRUE and noFallback feature is activated, destroy the redundant copy
 *                 2) write new redundant copy from RAM to DFlash
 *                 3) if isCopy2Destroyed_b = TRUE and noFallback feature is activated, destroy the latest copy
 *                 4) write new latest copy; in fast mode, write new latest copy directly from RAM to DFlash
 *                                           in slow mode, copy the new redundant copy in DFlash to new latest copy
 *
 *    native       1) if isCopy2Destroyed_b = TRUE and noFallback feature is activated, destroy the latest copy
 *                 2) write new latest copy from RAM to DFlash
 *
 * invalidate a copy:
 * same actions as for write redundant and native, but copy has length = 0
 *
 * \param  dataCRC_u32
 * \param  jobType_en            writing or invalidating a copy
 * \param  isCpyRed_b            TRUE: redundant copy, FALSE: native copy
 * \param  isCopy1Destroyed_b    FALSE: red. copy has to be destroyed, TRUE: do not destroy the red. copy
 * \param  isCopy2Destroyed_b    FALSE: native copy has to be destroyed, TRUE: do not destroy the native copy
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BlockJob_execWr(
        uint32                                  dataCRC_u32,
        rba_FeeFs1x_BlockJob_execWr_jobType_ten jobType_en,
        boolean                                 isCpyRed_b,
        boolean                                 isCopy1Destroyed_b,
        boolean                                 isCopy2Destroyed_b
        )
{
    /* If chunk job is ongoing, initialization has already been performed -> go directly to the write state */
    if(rba_FeeFs1x_BlockJob_execWr_stm_write_e != rba_FeeFs1x_BlockJob_execWr_data_st.state_en)
    {
        rba_FeeFs1x_BlockJob_execWr_data_st.isCpyRed_b = isCpyRed_b;
        if(isCpyRed_b)
        {
            // use the part of the state machine, which is designed for writing the redundant copy
            // after the redundant copy is written, type switches to writing the native copy (natOrRed2Cpy)
            rba_FeeFs1x_BlockJob_execWr_data_st.type_en = rba_FeeFs1x_BlockJob_execWr_type_red1Cpy_e;
        }
        else
        {
            // use the part of the state machine, which is designed for writing the native copy
            rba_FeeFs1x_BlockJob_execWr_data_st.type_en = rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e;
        }

        rba_FeeFs1x_BlockJob_execWr_data_st.state_en = rba_FeeFs1x_BlockJob_execWr_stm_init_e;
        rba_FeeFs1x_BlockJob_execWr_data_st.dataCRC_u32 = dataCRC_u32;
        rba_FeeFs1x_BlockJob_execWr_data_st.jobType_en = jobType_en;
        rba_FeeFs1x_BlockJob_execWr_data_st.isCopy1Destroyed_b = isCopy1Destroyed_b;
        rba_FeeFs1x_BlockJob_execWr_data_st.isCopy2Destroyed_b = isCopy2Destroyed_b;
        // isNoFB_b is not initialized, it is only required for maintain, not for write or invalidate
    }

    rba_FeeFs1x_BlockJob_execWr_data_st.entry_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWrMaintain
 * prepare function for maintaining a copy
 *
 * NoFallback  writeTwice_b              copy 1                     copy 2
 *    TRUE        TRUE            latest -> new redundant       latest -> new latest
 *    TRUE        FALSE           latest -> new redundant
 *    FALSE       TRUE            latest -> new redundant       new redundant -> new latest
 *    FALSE       FALSE           latest -> new redundant
 *
 *    If isCopyxDestroyed_b = FALSE and noFallback feature is activated,
 *    copy x will be destroyed before writing the new copy
 *
 * \param  isCopy1Destroyed_b    FALSE: red. copy has to be destroyed, TRUE: do not destroy the red. copy
 * \param  isCopy2Destroyed_b    FALSE: native copy has to be destroyed, TRUE: do not destroy the native copy
 * \param  isNoFB_b              Is it a no fallback copy
 * \param  writeTwice_b          FALSE: write only the redundant copy, TRUE: write redundant and natice copy
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BlockJob_execWrMaintain(
        boolean isCopy1Destroyed_b,
        boolean isCopy2Destroyed_b,
        boolean isNoFB_b,
        boolean writeTwice_b
        )
{
    if(writeTwice_b)
    {
        // block configuration has changed: write native and redundant copy
        rba_FeeFs1x_BlockJob_execWr_data_st.type_en = rba_FeeFs1x_BlockJob_execWr_type_red1Cpy_e;

        /* Since two new copies are written, increment the number of retry attempts. In total each copy is tried =
         * one attempt + RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES reattempts. So +1 is added below. */
        rba_FeeFs1x_BlockJobData_st.allowedRetries_u8 += (RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES + 1u);
    }
    else
    {
        // block configuration has not changed, but only one copy was found in DFlash
        // write only the redundant copy
        rba_FeeFs1x_BlockJob_execWr_data_st.type_en = rba_FeeFs1x_BlockJob_execWr_type_onlyRedCpy_e;
    }

    // maintain: always a redundant copy
    rba_FeeFs1x_BlockJob_execWr_data_st.isCpyRed_b = TRUE;
    rba_FeeFs1x_BlockJob_execWr_data_st.jobType_en = rba_FeeFs1x_BlockJob_execWr_jobType_maintain;
    rba_FeeFs1x_BlockJob_execWr_data_st.isCopy1Destroyed_b = isCopy1Destroyed_b;
    rba_FeeFs1x_BlockJob_execWr_data_st.isCopy2Destroyed_b = isCopy2Destroyed_b;
    rba_FeeFs1x_BlockJob_execWr_data_st.isNoFB_b = isNoFB_b;
    rba_FeeFs1x_BlockJob_execWr_data_st.state_en = rba_FeeFs1x_BlockJob_execWr_stm_init_e;
    rba_FeeFs1x_BlockJob_execWr_data_st.entry_b = TRUE;
    // dataCRC_u32 not required for maintain -> not initialized
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWrDo
 * job do function for writing native and redundant (non-NoFallback) blocks
 * \return  job result
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_JobOK_e   : job finished
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_restart_e : restart required
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e   : job aborted due to internal errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
static rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_execWrDo(void)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;

    switch(rba_FeeFs1x_BlockJob_execWr_data_st.state_en)
    {
        case rba_FeeFs1x_BlockJob_execWr_stm_init_e:
        {
            retVal_en = rba_FeeFs1x_BlockJob_execWrDo_init(&rba_FeeFs1x_BlockJob_execWr_data_st);
        }break;
        case rba_FeeFs1x_BlockJob_execWr_stm_destroy_e:
        {
            retVal_en = rba_FeeFs1x_BlockJob_execWrDo_destroy(&rba_FeeFs1x_BlockJob_execWr_data_st);
        }break;
        case rba_FeeFs1x_BlockJob_execWr_stm_write_e:
        {
            retVal_en = rba_FeeFs1x_BlockJob_execWrDo_write(&rba_FeeFs1x_BlockJob_execWr_data_st);
        }break;
        case rba_FeeFs1x_BlockJob_execWr_stm_error_e:
        {
            retVal_en = rba_FeeFs1x_BlockJob_execWrDo_error(&rba_FeeFs1x_BlockJob_execWr_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
        }break;
    }

    if(rba_FeeFs1x_BlockJob_ReturnType_pending_e != retVal_en)
    {
        /* For chunk jobs, do not finalize the block copies when some more bytes are still to be programmed. */
        if((rba_FeeFs1x_BlockJob_ReturnType_JobOK_e != retVal_en) ||
           (rba_FeeFs1x_BlockJobData_st.nrDataBytDone_u16 >= rba_FeeFs1x_BlockJobData_st.nrDataBytTot_u16))
        {
            rba_FeeFs1x_BlockJob_execWr_data_st.state_en = rba_FeeFs1x_BlockJob_execWr_stm_idle_e;

            rba_FeeFs1x_BlockJobData_st.nrDataBytDone_u16 = 0u;

            // delete the BCs which were created in init copy and searcher to prevent unwanted further usage
            rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
            rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_BlockJobs_NewLatest_e);

            if(rba_FeeFs1x_BlockJob_execWr_data_st.isCpyRed_b)
            {
                rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_BlockJobs_Redundant_e);
                rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_BlockJobs_NewRedundant_e);
            }
        }
    }
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWrDo_init
 * statefunction of execWrDo
 * inits the first and the second new copy depending on object type
 * \param   *fsm_pst: pointer to struct containing the statemachine data
 * \return  job result
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_restart_e : restart required
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e : job aborted due to internal errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_execWrDo_init(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retValInitCpyDo_en;
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;

    retValInitCpyDo_en = rba_FeeFs1x_BlockJob_execWrDo_init_initBC(fsm_pst);

    // evaluate the initCpyWr result
    switch(retValInitCpyDo_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            if(rba_FeeFs1x_Prv_isNoFallbackActive())
            {
                if(rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e == fsm_pst->type_en)
                {
                    // native copy or second copy of a redundant block
                    if(fsm_pst->isCopy2Destroyed_b)
                    {
                        // No copy required to be destroyed, switch to write directly
                        FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_execWr_stm_write_e);
                    }
                    else
                    {
                        // old copy has to be destroyed
                        // another copy is present after destroying this one --> data loss prevented
                        FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_execWr_stm_destroy_e);
                    }
                }
                else
                {
                    // first copy of a redundant block is processed
                    if(fsm_pst->isCopy1Destroyed_b)
                    {
                        // No copy required to be destroyed, switch to write directly
                        FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_execWr_stm_write_e);
                    }
                    else
                    {
                        // old copy has to be destroyed
                        // another copy is present after destroying this one --> data loss prevented
                        FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_execWr_stm_destroy_e);
                    }
                }
            }
            else
            {
                // proceed with writing the copy, return pending
                FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_execWr_stm_write_e);
            }
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
        }break;
        case rba_FeeFs1x_JobFailed_e:
        {
            // restarting the blockjob from searching for old copies is required
            if((FEE_RB_WRITEJOB_SKIP_HARDREORG_WRITE_ONLY_E == rba_FeeFs1x_BlockJobData_st.jobMode_en) ||
               (FEE_RB_INVALIDATEJOB_SKIP_HARDREORG_WRITE_ONLY_E == rba_FeeFs1x_BlockJobData_st.jobMode_en))
            {
                // Memory full. no more writes possible till reorg is completed.
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
            }
            else
            {
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_restart_e;
            }
        }break;
        case rba_FeeFs1x_Pending_e:
        {
            // call same state during next cycle
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
        }break;
        case rba_FeeFs1x_ErrorExternal_e:
        {
            // error -> switch to retry handler
            FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_execWr_stm_error_e);
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
        }break;
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // unexpected return value-> abort the job
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWrDo_destroy
 * statefunction of execWrDo
 * destroys the first and the second new copy depending on object type
 * \param   *fsm_pst: pointer to struct containing the statemachine data
 * \return  job result
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e : job aborted due to internal errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_execWrDo_destroy(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;
    rba_FeeFs1x_RetVal_ten              retValInner_en;
    rba_FeeFs1x_BlockJob_ReturnType_ten retValTemp_en;
    Std_ReturnType                      retValCache_en = E_OK;

    if(rba_FeeFs1x_Prv_isNoFallbackActive())
    {
        retValInner_en = rba_FeeFs1x_BlockJob_execWrDo_destroy_destroyBC(fsm_pst);

        // evaluate the initCpyWr result
        switch(retValInner_en)
        {
            /* Continue with the next step on both success or failure of destroy operation.
               As the retry would have to be performed on same page only and would lead to further complexities of
               possible overprogramming of the page leading to flash sequence error on Renesas controllers. */
            case rba_FeeFs1x_ErrorExternal_e:
            case rba_FeeFs1x_JobOK_e:
            {
                if(rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e == fsm_pst->type_en)
                {
                    /* Native copy or second copy of a redundant block is handled. For NoFB Red Copy: After destroy and
                     * invalidation of first copy, 2nd copy will become a intermediate Red copy and moved from Nat to
                     * Red Cache. Therefore when destroying 2nd copy it will be in Red cache --> delete redundant cache
                     * entry. Otherwise delete the native cache entry */

                    /* Currently this state is not reached for unknown block write. But in future if the code is adapted
                     * to also handle unknown block, then, there could be an OOB access as
                     * rba_FeeFs1x_BlockJobData_st.feeIndex_u16 is set to 0xFFFF. Which means that there is a risk that
                     * the below code may be also called for unknown block. To avoid this the calls are made only when
                     * the index is valid. This check today is "dead code" but might protect future OOB access.
                     * */
                    if(RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS > rba_FeeFs1x_BlockJobData_st.feeIndex_u16)
                    {
                        if(fsm_pst->isCpyRed_b)
                        {
                            retValCache_en =
                                    rba_FeeFs1x_CacheKwn_invalidateCache_Red(rba_FeeFs1x_BlockJobData_st.feeIndex_u16);
                        }
                        else
                        {
                            rba_FeeFs1x_CacheKwn_invalidateCache(rba_FeeFs1x_BlockJobData_st.feeIndex_u16);
                        }
                    }

                    if(E_OK == retValCache_en)
                    {
                        /* copy2 is destroyed, so don't do it again, this could happen, if write2 fails and
                         * error handler is restarting from ini */
                        fsm_pst->isCopy2Destroyed_b = TRUE;
                        if(rba_FeeFs1x_BlockJob_execWr_jobType_maintain != fsm_pst->jobType_en)
                        {
                            // maintain will finalize this copy on state machine exit,
                            // because it is needed in state write, when only the redundant copy shall be written.
                            // for write and invalidate it may be finalized already here
                            rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
                        }

                        // proceed with writing the new copy
                        FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_execWr_stm_write_e);
                        retValTemp_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
                    }
                    else
                    {
                        retValTemp_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
                    }
                }
                else
                {
                    // first copy of a redundant copy is processed
                    Std_ReturnType retValCacheRed_en;
                    retValCacheRed_en =
                            rba_FeeFs1x_CacheKwn_invalidateCache_Red(rba_FeeFs1x_BlockJobData_st.feeIndex_u16);
                    if (E_OK == retValCacheRed_en)
                    {
                        fsm_pst->isCopy1Destroyed_b = TRUE;
                        // redundant copy is destroyed, so finalize also the BC object
                        rba_FeeFs1x_BC_finalizeCopy(rba_FeeFs1x_BC_Cpy_BlockJobs_Redundant_e);

                        // proceed with writing the new copy
                        FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_execWr_stm_write_e);
                        retValTemp_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
                    }
                    else
                    {
                        retValTemp_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
                    }
                }

                retVal_en = retValTemp_en;
            }break;

            case rba_FeeFs1x_Pending_e:
            {
                // call same state during next cycle
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
            }break;

            case rba_FeeFs1x_ErrorInternal_e:
            default:
            {
                // unexpected return value-> abort the job
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
            }break;
        }
    }
    else
    {
        retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
    }
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWrDo_write
 * statefunction of execWr_Do
 * writes the first and the second new copy depending on object type
 * \param   *fsm_pst: pointer to struct containing the statemachine data
 * \return  job result
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_JobOK_e : job finished
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e : job aborted due to internal errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_execWrDo_write(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValWr_en;
    rba_FeeFs1x_BlockJob_execWr_type_ten type_en;
    uint32 pageNr_u32;

    switch(fsm_pst->jobType_en)
    {
        case rba_FeeFs1x_BlockJob_execWr_jobType_invalidate:
        {
            retValWr_en = rba_FeeFs1x_BlockJob_execWrDo_write_invalidateBC(fsm_pst);
        }break;
        case rba_FeeFs1x_BlockJob_execWr_jobType_maintain:
        {
            retValWr_en = rba_FeeFs1x_BlockJob_execWrDo_write_maintainBC(fsm_pst);
        }break;
        case rba_FeeFs1x_BlockJob_execWr_jobType_write:
        default: // normal write
        {
            retValWr_en = rba_FeeFs1x_BlockJob_execWrDo_write_writeBC(fsm_pst);
        }break;
    }

    // evaluate the result of writing
    switch(retValWr_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            rba_FeeFs1x_BlockJobData_st.nrDataBytDone_u16 += rba_FeeFs1x_BlockJobData_st.nrDataBytCur_u16;

            // Check if all chunks are completed
            if(rba_FeeFs1x_BlockJobData_st.nrDataBytDone_u16 < rba_FeeFs1x_BlockJobData_st.nrDataBytTot_u16)
            {
                // writing of current chunk is successful
                retVal_en = rba_FeeFs1x_BlockJob_ReturnType_JobOK_e;
            }
            else
            {

                // writing the copy completed --> update the cache to complete the job
                rba_FeeFs1x_BC_Cpy_blockCopyObject_ten BlockCopyObj;

                type_en = fsm_pst->type_en;
                if(rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e == type_en)
                {
                    // native copy or second copy of a redundant block is processed
                    BlockCopyObj = rba_FeeFs1x_BC_Cpy_BlockJobs_NewLatest_e;
                }
                else
                {
                    // first copy of a redundant block is processed
                    BlockCopyObj = rba_FeeFs1x_BC_Cpy_BlockJobs_NewRedundant_e;
                }

                // Job finished successfully, update the cache to finalize the operation
                pageNr_u32 = rba_FeeFs1x_BC_getPageNo(BlockCopyObj);

                if(!rba_FeeFs1x_BlockJobData_st.isUnknownBlk_b)
                {
                    rba_FeeFs1x_CacheKwn_addNewerCopy(rba_FeeFs1x_BlockJobData_st.feeIndex_u16,
                            TRUE,       // After write, data is always validated
                            pageNr_u32,
                            fsm_pst->isCpyRed_b);
                }
                else
                {
                    // attempt to update the unknown cache only when the cache is built up. Otherwise, we will have to
                    // first build up the cache (to avoid unknown blocks in the oldest sector being skipped).
                    if(rba_FeeFs1x_Searcher_isUnknownBlkCacheBuilt())
                    {
                        // cache has been built up before, just update it with new entry
                        if(E_NOT_OK == rba_FeeFs1x_CacheUnkwn_addNewerCopyByPersID(
                                        rba_FeeFs1x_BlockJobData_st.persID_u16,
                                        TRUE,
                                        pageNr_u32,
                                        fsm_pst->isCpyRed_b) )
                        {
                            /* unknown cache could not be updated, because its full. It is also ok if the cache could
                             * not be updated, this block will be found in the next round of cache build up. For this
                             * important is to set the page limit where the cache got full and this has to be done only
                             * once (handled in the below function itself). */
                            rba_FeeFs1x_Searcher_checkUnknownBlkCacheFullAndSetReorgLimit(pageNr_u32-1u);
                        }
                    }
                    else
                    {
                        /* Unknown cache is not built up yet. To keep it simple, it is not required to build cache here.
                         * However, it is important that the unknown cache should be built up later, so reset the cache
                         * and this will trigger the build up of the cache from oldest sector. By skipping the cache
                         * build up here, the handling is similar to Sector overflow handling
                         * -> rba_FeeFs1x_Reorg_repairSectOverflowDo_cacheBuildup() */
                        rba_FeeFs1x_Searcher_resetSearchForUnknownCacheBuildup();
                    }
                }

                // writing of the redundant copy is finished */
                if(rba_FeeFs1x_BlockJob_execWr_type_red1Cpy_e == type_en)
                {
                    /* redundant block is written, switch to write the native copy */
                    fsm_pst->type_en = rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e;

                    FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_execWr_stm_init_e);

                    /* It is important that the retries is reset here again, because it might have high value due to
                     * initial value set in rba_FeeFs1x_Write() or rba_FeeFs1x_Invalidate().
                     * Important is to allow the same number of retries for each copy. */
                    if((rba_FeeFs1x_BlockJobData_st.isChunkJob_b) ||
                       (rba_FeeFs1x_BlockJobData_st.allowedRetries_u8 > RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES))
                    {
                        rba_FeeFs1x_BlockJobData_st.allowedRetries_u8  = RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES;
                    }
                    else
                    {
                        /* Total number of attempts for redundant block should not exceed
                         * 2 * (RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES + 1). Control reaching here means that some of
                         * the retries for the second copy were also attempted during first copy write. So do not change
                         * the remaining retries. */
                    }

                    // second copy will be written in one chunk
                    rba_FeeFs1x_BlockJobData_st.nrDataBytCur_u16 = rba_FeeFs1x_BlockJobData_st.nrDataBytTot_u16;

                    retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
                }
                else
                {
                    // all copies are written.
                    retVal_en = rba_FeeFs1x_BlockJob_ReturnType_JobOK_e;
                }
            }
        }break;
        case rba_FeeFs1x_ErrorExternal_e:
        {
            // error during writing, go to retry handler
            FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_execWr_stm_error_e);
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
        }break;
        case rba_FeeFs1x_Pending_e:
        {
            // state requires to be called again during next cycle
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
        }break;
        case rba_FeeFs1x_ErrorInternal_e:
        default:
        {
            // unexpected return value
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
        }break;
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWrDo_error
 * statefunction of execWr_Do
 * write retry handler
 * \param   *fsm_pst: pointer to struct containing the statemachine data
 * \return  job result
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_pending_e : ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_BlockJob_ReturnType_error_e : job aborted due to internal errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_execWrDo_error(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;

    // check whether a retry is allowed
    if(rba_FeeFs1x_BlockJobData_st.allowedRetries_u8 > 0u)
    {
        FEE_PRV_LIBSC(rba_FeeFs1x_BlockJob_execWr_stm_init_e);

        rba_FeeFs1x_BlockJobData_st.allowedRetries_u8--;
        // retry counter doesn't forbid another retry --> restart the write
        retVal_en = rba_FeeFs1x_BlockJob_ReturnType_pending_e;
    }
    else
    {
        /* Correct the return value when first copy was written successfully but second copy write failed.
         * It is partially successful but its better to return as PASSED, since the redundant copy can later be
         * healed during maintain api or reorg execution. */
        if((fsm_pst->isCpyRed_b) &&
           (rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e == fsm_pst->type_en))
        {
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_JobOK_e;
        }
        else
        {
            // retry maximum reached, exit the job with an error
            retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
        }
    }

    return retVal_en;
}

/*
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWr_InitBlockCopy
 * inits the block copy write action depending on job type
 * \param   *fsm_pst: pointer to struct containing the statemachine data
 * \return  Returns whether the prepare space operation was completed
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e         : prepare space was successfully completed
 * \retval  rba_FeeFs1x_JobFailed_e     : block copy was moved during sector reorg, return failure to restart block jobs
 * \retval  rba_FeeFs1x_ErrorInternal_e : Error during preperation of space
 * \retval  rba_FeeFs1x_ErrorExternal_e : processing error, prepare space wasn't completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BlockJob_execWrDo_init_initBC(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten                  retVal_en;
    rba_FeeFs1x_BC_Cpy_blockCopyObject_ten  copy_en;

    if(FEE_PRV_LIBENTRY)
    {
        if(rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e == fsm_pst->type_en)
        {
            /* handle the native or second copy of a redundant block */
            copy_en = rba_FeeFs1x_BC_Cpy_BlockJobs_NewLatest_e;
        }
        else
        {
            /* handle first copy of a redundant block */
            copy_en = rba_FeeFs1x_BC_Cpy_BlockJobs_NewRedundant_e;
        }

        /* It is important that the block copy is initialized with total block length
         * (rba_FeeFs1x_BlockJobData_st.nrDataBytTot_u16), since this will be used to create the required space for the
         * complete block and avoids problems when one of the chunks in between crosses over the HSR or leads to mem
         * full scenario. */
        rba_FeeFs1x_BC_initCopyWr(  copy_en,
                                    rba_FeeFs1x_BlockJobData_st.nrDataBytTot_u16,
                                    rba_FeeFs1x_BlockJobData_st.persID_u16,
                                    rba_FeeFs1x_BlockJobData_st.statusByte_u16,
                                    rba_FeeFs1x_BlockJobData_st.jobMode_en);

        rba_FeeFs1x_BlockJobData_st.nrDataBytDone_u16 = 0u;
    }

    // cyclic call of Do fct.
    retVal_en = rba_FeeFs1x_BC_initCopyWrDo();

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWrDo_destroy_destroyBC
 * Destroys a block copy depending on job type
 * \param   *fsm_pst: pointer to struct containing the statemachine data
 * \return  Returns status of the destroy operation
 * \retval  rba_FeeFs1x_Pending_e       : destory ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e         : block copy destroyed sucessfully
 * \retval  rba_FeeFs1x_JobFailed_e     : block copy was already destroyed
 * \retval  rba_FeeFs1x_ErrorInternal_e : Error during destorying of the block copy
 * \retval  rba_FeeFs1x_ErrorExternal_e : processing error, validation wasn't completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BlockJob_execWrDo_destroy_destroyBC(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        if(rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e == fsm_pst->type_en)
        {
            /* native copy to be handled */
            rba_FeeFs1x_BC_destroy(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e);
        }
        else
        {
            /* redundant copy to be handled */
            rba_FeeFs1x_BC_destroy(rba_FeeFs1x_BC_Cpy_BlockJobs_Redundant_e);
        }
    }

    // Cyclic call for the DO function
    retVal_en = rba_FeeFs1x_BC_destroyDo();

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWrDo_write_writeBC
 * Write a block copy
 * \param   *fsm_pst: pointer to struct containing the statemachine data
 * \return  Returns status of the invalidate/storage operation
 * \retval  rba_FeeFs1x_Pending_e       : Storage ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e         : Data stored sucessfully
 * \retval  rba_FeeFs1x_ErrorInternal_e : Error during storage of the data
 * \retval  rba_FeeFs1x_ErrorExternal_e : processing error, validation wasn't completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BlockJob_execWrDo_write_writeBC(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten                  retVal_en;
    rba_FeeFs1x_BlockJob_execWr_type_ten    type_en;
    boolean                                 isCpyRed_b, isChunkJob_b;
    Fee_Rb_JobMode_ten                      jobMode_en;
    uint32                                  dataCRC_u32;
    uint8 const *                           userbuffer_pcu8;
    uint16                                  nrDataBytCur_u16;

    type_en          = fsm_pst->type_en;
    isCpyRed_b       = fsm_pst->isCpyRed_b;
    jobMode_en       = rba_FeeFs1x_BlockJobData_st.jobMode_en;
    dataCRC_u32      = fsm_pst->dataCRC_u32;
    userbuffer_pcu8  = rba_FeeFs1x_BlockJobData_st.userbuffer_pcu8;
    isChunkJob_b     = rba_FeeFs1x_BlockJobData_st.isChunkJob_b;
    nrDataBytCur_u16 = rba_FeeFs1x_BlockJobData_st.nrDataBytCur_u16;

    if(FEE_PRV_LIBENTRY)
    {
        if(rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e == type_en)
        {
            /* second copy of a redundant block or a native copy */
            if(isCpyRed_b)
            {
                /* Handle the second copy of a redundant block */

                if((FEE_RB_ALLJOBS_ALLSTEPS_E == jobMode_en) || (isChunkJob_b))
                {
                    /* 1. In fast execution mode, write the latest copy directly from the user buffer without previously
                     * copying from flash to internal buffer and writing back from there.
                     * 2. During chunk wise write, the second copy should be written using the data from first copy.
                     * Since the full data is not available from the user buffer. */
                    rba_FeeFs1x_BC_wrFls2Fls(rba_FeeFs1x_BC_Cpy_BlockJobs_NewRedundant_e,
                                             rba_FeeFs1x_BC_Cpy_BlockJobs_NewLatest_e,
                                             FALSE,
                                             0x0,
                                             jobMode_en);
                }
                else
                {
                    rba_FeeFs1x_BC_wrRam2Fls(rba_FeeFs1x_BC_Cpy_BlockJobs_NewLatest_e,
                                             dataCRC_u32,
                                             userbuffer_pcu8,
                                             nrDataBytCur_u16,
                                             jobMode_en,
                                             FALSE);  // This is not a chunkwise job
                }
            }
            else
            {
                /* handle a native copy */
                rba_FeeFs1x_BC_wrRam2Fls(rba_FeeFs1x_BC_Cpy_BlockJobs_NewLatest_e,
                                         dataCRC_u32,
                                         userbuffer_pcu8,
                                         nrDataBytCur_u16,
                                         jobMode_en,
                                         isChunkJob_b);
            }
        }
        else
        {
            /* handle the first copy of a redundant block */
                rba_FeeFs1x_BC_wrRam2Fls(rba_FeeFs1x_BC_Cpy_BlockJobs_NewRedundant_e,
                                         dataCRC_u32,
                                         userbuffer_pcu8,
                                         nrDataBytCur_u16,
                                         jobMode_en,
                                         isChunkJob_b);
        }
    }

    // continue processing the writing
    retVal_en = rba_FeeFs1x_BC_wrData2FlsDo();

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWrDo_write_invalidateBC
 * Invalidate  a block copy
 * \param   *fsm_pst: pointer to struct containing the statemachine data
 * \return  Returns status of the invalidate/storage operation
 * \retval  rba_FeeFs1x_Pending_e       : Storage ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e         : Data stored sucessfully
 * \retval  rba_FeeFs1x_ErrorInternal_e : Error during storage of the data
 * \retval  rba_FeeFs1x_ErrorExternal_e : processing error, validation wasn't completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BlockJob_execWrDo_write_invalidateBC(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    Fee_Rb_JobMode_ten jobMode_en;

    jobMode_en = rba_FeeFs1x_BlockJobData_st.jobMode_en;

    if(FEE_PRV_LIBENTRY)
    {
        if(rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e == fsm_pst->type_en)
        {
            /* second copy of a redundant block or a native copy */
            rba_FeeFs1x_BC_invalidate(rba_FeeFs1x_BC_Cpy_BlockJobs_NewLatest_e, jobMode_en);
        }
        else
        {
            /* handle the first copy of a redundant block */
            rba_FeeFs1x_BC_invalidate(rba_FeeFs1x_BC_Cpy_BlockJobs_NewRedundant_e, jobMode_en);
        }
    }

    // continue processing the writing
    retVal_en = rba_FeeFs1x_BC_invalidateDo();

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_execWrDo_write_maintainBC
 * Maintain a block copy
 * \param   *fsm_pst: pointer to struct containing the statemachine data
 * \return  Returns status of the invalidate/storage operation
 * \retval  rba_FeeFs1x_Pending_e       : Storage ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_JobOK_e         : Data stored sucessfully
 * \retval  rba_FeeFs1x_ErrorInternal_e : Error during storage of the data
 * \retval  rba_FeeFs1x_ErrorExternal_e : processing error, validation wasn't completed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_BlockJob_execWrDo_write_maintainBC(rba_FeeFs1x_BlockJob_execWr_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(rba_FeeFs1x_Prv_isMaintainActive())
    {
        if(FEE_PRV_LIBENTRY)
        {
            uint8 statusByte_u8 = (uint8)(0xFFu & Fee_Prv_ConfigGetBlockStatusBitsByBlockNr(
                                                    rba_FeeFs1x_BlockJobData_st.feeIndex_u16,
                                                    rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast) );

            // case    NoFallback   config mismatch    write redundant copy (copy1)     write native copy (copy2)
            // 1          TRUE         TRUE            Latest -> New Redundant          Latest        -> New Latest
            // 2          TRUE        FALSE            Latest -> New Redundant
            // 3         FALSE         TRUE            Latest -> New Redundant          New Redundant -> New Latest
            // 4         FALSE        FALSE            Latest -> New Redundant

            if(rba_FeeFs1x_BlockJob_execWr_type_natOrRed2Cpy_e == fsm_pst->type_en)
            {
                // only the native copy shall be written or the second copy of a redundant block
                if(fsm_pst->isNoFB_b)
                {
                    // case 1
                    rba_FeeFs1x_BC_wrFls2Fls(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e,
                            rba_FeeFs1x_BC_Cpy_BlockJobs_NewLatest_e,
                            TRUE,
                            statusByte_u8,
                            FEE_RB_ALLJOBS_ALLSTEPS_E);
                }
                else
                {
                    // always execute in fast mode -> in Fee_Medium only the blank check before writing will be skipped
                    // case 3
                    rba_FeeFs1x_BC_wrFls2Fls(rba_FeeFs1x_BC_Cpy_BlockJobs_NewRedundant_e,
                            rba_FeeFs1x_BC_Cpy_BlockJobs_NewLatest_e,
                            FALSE,
                            0x0,
                            FEE_RB_WRITEJOB_WRITE_VERIFY_E);
                }
            }
            else
            {
                // case 2, 4
                // There is no newLatest copy, so the latest copy has to be taken as the source
                rba_FeeFs1x_BC_wrFls2Fls(rba_FeeFs1x_BC_Cpy_BlockJobs_Latest_e,
                                        rba_FeeFs1x_BC_Cpy_BlockJobs_NewRedundant_e,
                                        TRUE,
                                        statusByte_u8,
                                        FEE_RB_ALLJOBS_ALLSTEPS_E);
            }
        }

        retVal_en = rba_FeeFs1x_BC_wrData2FlsDo();
    }
    else
    {
        retVal_en = rba_FeeFs1x_ErrorInternal_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Read The function supports reading of known blocks only. the check for known or unknown is already done
 * in FeeHl so not performed here again. The API should be called only for known blocks.
 * \attention: Function should be called only with valid parameter value!
 *
 * \param orderStruct_pcst: struct containing the data collected by the Fee external service APIs
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Read(Fee_Prv_JobDesc_tst const * orderStruct_pcst)
{
    /* not initialized (because unused for both normal and chunk-wise read jobs):
     * userbuffer_pcu8, persID_u16, nrDataBytTot_u16, nrDataBytCur_u16, statusByte_u16, isUnknownBlk_b, isChunkJob_b */

    // Normal job order, or first chunk of a chunk-wise job order?
    if(Fee_Prv_JobIsFirstPart(orderStruct_pcst))
    {
        // reset the retry counter
        rba_FeeFs1x_BlockJobData_st.allowedRetries_u8 = RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES;

        // set the check and exec functions
        rba_FeeFs1x_BlockJobData_st.idxCheckDoFunc_en = rba_FeeFs1x_BlockJob_jobDoFunc_checkRdDo_e;
        rba_FeeFs1x_BlockJobData_st.idxCheckFunc_en   = rba_FeeFs1x_BlockJob_jobFunc_checkRd_e;
        rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en  = rba_FeeFs1x_BlockJob_jobDoFunc_execRdDo_e;
        rba_FeeFs1x_BlockJobData_st.idxExecFunc_en    = rba_FeeFs1x_BlockJob_jobFunc_execRd_e;

        // only search the latest consistent copy for reading
        rba_FeeFs1x_BlockJobData_st.idxSearchType_en  = rba_FeeFs1x_BlockJob_searchType_F1UC_e;

        // initialize the searcher return value
        rba_FeeFs1x_BlockJobData_st.searchRetVal_en   = rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e;

        // store the requested job details
        rba_FeeFs1x_BlockJobData_st.feeIndex_u16      = orderStruct_pcst->blockNumber_u16;
        rba_FeeFs1x_BlockJobData_st.jobMode_en        = orderStruct_pcst->jobMode_en;

        // switch to search state to enable the job
        rba_FeeFs1x_BlockJobData_st.state_en          = rba_FeeFs1x_BlockJob_stm_search_e;

# if(defined(RBA_FEEFS1X_PRV_CFG_DETAILED_BLK_INFO_API) && (TRUE == RBA_FEEFS1X_PRV_CFG_DETAILED_BLK_INFO_API))
        /* reset detailed block info. For unknown block read (which will be supported in future),
         * the feeIndex_u16 would be 0xFFFF. Skip the detailed block info for such blocks. */
        if(RBA_FEEFS1X_PRV_CFG_NR_OF_BLOCKS > rba_FeeFs1x_BlockJobData_st.feeIndex_u16)
        {
            rba_FeeFs1x_BlockJob_ReadJobResultDetailed_aen[rba_FeeFs1x_BlockJobData_st.feeIndex_u16] =
                                                            MEMIF_RB_DETAILED_BLK_INFO_INIT_E;
        }
# endif
        rba_FeeFs1x_BlockJob_BlkDataCrcError_b        = FALSE;
    }
    else
    {
        /* Unlike for a chunk-wise write with rba_FeeFs1x_Write, a retry on error is permitted for a chunk-wise read
         * with rba_FeeFs1x_Read. Reason: The scope of the retry is the search, which is done before reading the first
         * chunk, or after a CRC mismatch has been detected for the last chunk. When working on the first chunk, a
         * retry from scratch is always permitted. And after a CRC mismatch has been detected for the last chunk, there
         * is no access of the user buffer anymore, i.e. a retry of the search only is okay. */

        // go directly to read the block
        rba_FeeFs1x_BlockJobData_st.state_en          = rba_FeeFs1x_BlockJob_stm_execJob_e;
    }

    // pass the relevant jobdata to the unit RdJob
    rba_FeeFs1x_RdJob_initJob(orderStruct_pcst);

    rba_FeeFs1x_BlockJobData_st.entry_b               = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Invalidate The function supports invalidation of both known and unknown blocks.
 * \attention: Function should be called only with valid parameter value!
 * \param   orderStruct_pcst : struct containing the data collected by the Fee external service APIs
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Invalidate(Fee_Prv_JobDesc_tst const * orderStruct_pcst)
{
    // reset the retry counter
    rba_FeeFs1x_BlockJobData_st.allowedRetries_u8 = RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES;

    // For redundant block also add the number of retries for the second copy
    if(0u != (orderStruct_pcst->statusFlag_u16 & RBA_FEEFS1X_PRV_CFG_DOUBLESTORAGE_BITS))
    {
        /* In total each copy is tried = one attempt + RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES reattempts.
         * So +1 is added below. */
        rba_FeeFs1x_BlockJobData_st.allowedRetries_u8 += (RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES + 1u);
    }

    if(!orderStruct_pcst->isUnknownBlk_b)
    {
        // normal write for the latest consistent copy
        if(!Fee_Prv_ConfigIsBlockDoubleStorageActiveByBlockNr(  orderStruct_pcst->blockNumber_u16,
                                                                RBA_FEEFS1X_PRV_CFG_DOUBLESTORAGE_BITS,
                                                                rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast) )
        {
            if(!Fee_Prv_ConfigIsBlockNoFallbackActiveByBlockNr( orderStruct_pcst->blockNumber_u16,
                                                                RBA_FEEFS1X_PRV_CFG_NOFALLBACK_BITS,
                                                                rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast) )
            {
                // native non-noFallback block

                // set the exec functions
                rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en   = rba_FeeFs1x_BlockJob_jobDoFunc_execInvDo_e;
                rba_FeeFs1x_BlockJobData_st.idxExecFunc_en     = rba_FeeFs1x_BlockJob_jobFunc_execInv_e;

                // only search the latest consistent copy for invalidating
                rba_FeeFs1x_BlockJobData_st.idxSearchType_en   = rba_FeeFs1x_BlockJob_searchType_F1C_e;
            }
            else
            {
                // native noFallback block

                // set the exec functions
                rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en   = rba_FeeFs1x_BlockJob_jobDoFunc_execInvNoFBDo_e;
                rba_FeeFs1x_BlockJobData_st.idxExecFunc_en     = rba_FeeFs1x_BlockJob_jobFunc_execInvNoFB_e;

                // only search the latest consistent copy for invalidating
                rba_FeeFs1x_BlockJobData_st.idxSearchType_en   = rba_FeeFs1x_BlockJob_searchType_F1C_e;
            }
        }
        else
        {
            if(!Fee_Prv_ConfigIsBlockNoFallbackActiveByBlockNr( orderStruct_pcst->blockNumber_u16,
                                                                RBA_FEEFS1X_PRV_CFG_NOFALLBACK_BITS,
                                                                rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast) )
            {
                // redundant non-NoFallback block

                // set the exec functions
                rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en  = rba_FeeFs1x_BlockJob_jobDoFunc_execInvRedDo_e;
                rba_FeeFs1x_BlockJobData_st.idxExecFunc_en    = rba_FeeFs1x_BlockJob_jobFunc_execInvRed_e;

                // only search the latest consistent copy for reading
                // (redundancy check for the old copies is not executed)
                rba_FeeFs1x_BlockJobData_st.idxSearchType_en   = rba_FeeFs1x_BlockJob_searchType_F1C_e;
            }
            else
            {
                // redundant NoFallback block

                // set the exec functions
                rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en  = rba_FeeFs1x_BlockJob_jobDoFunc_execInvNoFBRedDo_e;
                rba_FeeFs1x_BlockJobData_st.idxExecFunc_en    = rba_FeeFs1x_BlockJob_jobFunc_execInvNoFBRed_e;

                // search for both latest consistent  and redundant copies
                // (redundant copy is supposed to get destroyed)
                rba_FeeFs1x_BlockJobData_st.idxSearchType_en   = rba_FeeFs1x_BlockJob_searchType_F2C_e;
            }
        }
    }
    else
    {
        // set the exec functions for the unknown block handling
        rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en   = rba_FeeFs1x_BlockJob_jobDoFunc_execInvUnknownBlkDo_e;
        rba_FeeFs1x_BlockJobData_st.idxExecFunc_en     = rba_FeeFs1x_BlockJob_jobDoFunc_execInvUnknownBlk_e;

        // Search is not used, anyway set it to default value
        rba_FeeFs1x_BlockJobData_st.idxSearchType_en   = rba_FeeFs1x_BlockJob_searchType_F1C_e;

        // store details of the unknown block
    }

    // set the check functions
    rba_FeeFs1x_BlockJobData_st.idxCheckDoFunc_en  = rba_FeeFs1x_BlockJob_jobDoFunc_checkInvDo_e;
    rba_FeeFs1x_BlockJobData_st.idxCheckFunc_en    = rba_FeeFs1x_BlockJob_jobFunc_checkInv_e;

    rba_FeeFs1x_BlockJobData_st.userbuffer_pcu8 = NULL_PTR;

    // store the requested job details
    rba_FeeFs1x_BlockJobData_st.feeIndex_u16        = orderStruct_pcst->blockNumber_u16;
    rba_FeeFs1x_BlockJobData_st.jobMode_en          = orderStruct_pcst->jobMode_en;
    rba_FeeFs1x_BlockJobData_st.isUnknownBlk_b      = orderStruct_pcst->isUnknownBlk_b;
    rba_FeeFs1x_BlockJobData_st.persID_u16          = orderStruct_pcst->idPers_u16;
    rba_FeeFs1x_BlockJobData_st.nrDataBytTot_u16    = RBA_FEEFS1X_BLOCKJOB_DATA_LENGTH_0;
    rba_FeeFs1x_BlockJobData_st.nrDataBytCur_u16    = RBA_FEEFS1X_BLOCKJOB_DATA_LENGTH_0;
    rba_FeeFs1x_BlockJobData_st.isChunkJob_b        = FALSE;
    rba_FeeFs1x_BlockJobData_st.statusByte_u16      = orderStruct_pcst->statusFlag_u16;

    // in fast mode search and check if invalidation required is skipped:
    // if initialized with "no copy found", no destruction of old noFallback copies is performed
    rba_FeeFs1x_BlockJobData_st.searchRetVal_en = rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e;

    // pass the relevant jobdata to the unit InvJob
    rba_FeeFs1x_InvJob_initJob(orderStruct_pcst);

    // switch to search state to enable the job
    rba_FeeFs1x_BlockJobData_st.entry_b = TRUE;

    if((FEE_RB_ALLJOBS_ALLSTEPS_E == rba_FeeFs1x_BlockJobData_st.jobMode_en) && (!orderStruct_pcst->isUnknownBlk_b))
    {
        // start with the search for already existent copies
        rba_FeeFs1x_BlockJobData_st.state_en = rba_FeeFs1x_BlockJob_stm_search_e;
    }
    else
    {
        /* in all other modes or job is for unknown block (which needs to be written chunk wise),
         * search for existent copies is skipped -> directly execute*/
        rba_FeeFs1x_BlockJobData_st.state_en = rba_FeeFs1x_BlockJob_stm_execJob_e;
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_Write The function supports writing of both known and unknown blocks.
 * \attention: Function should be called only with valid parameter value!
 * \param   orderStruct_pcst : struct containing the data collected by the Fee external service APIs
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Write(Fee_Prv_JobDesc_tst const * orderStruct_pcst)
{
    // Normal job order, or first chunk of a chunk-wise job order?
    if(0u == rba_FeeFs1x_BlockJobData_st.nrDataBytDone_u16)
    {
        // reset the retry counter
        rba_FeeFs1x_BlockJobData_st.allowedRetries_u8 = RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES;

        // For redundant block also add the number of retries for the second copy
        if(0u != (orderStruct_pcst->statusFlag_u16 & RBA_FEEFS1X_PRV_CFG_DOUBLESTORAGE_BITS))
        {
            /* In total each copy is tried = one attempt + RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES reattempts.
             * So +1 is added below. */
            rba_FeeFs1x_BlockJobData_st.allowedRetries_u8 += (RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES + 1u);
        }

        // store details of the order
        // check if the block is a known or unknown block
        if(!orderStruct_pcst->isUnknownBlk_b)
        {
            // normal write for the latest consistent copy
            if(!Fee_Prv_ConfigIsBlockDoubleStorageActiveByBlockNr(  orderStruct_pcst->blockNumber_u16,
                                                                    RBA_FEEFS1X_PRV_CFG_DOUBLESTORAGE_BITS,
                                                                    rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast) )
            {
                if(!Fee_Prv_ConfigIsBlockNoFallbackActiveByBlockNr( orderStruct_pcst->blockNumber_u16,
                                                                    RBA_FEEFS1X_PRV_CFG_NOFALLBACK_BITS,
                                                                    rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast) )
                {
                    // native non-noFallback block

                    // set the exec functions
                    rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en   = rba_FeeFs1x_BlockJob_jobDoFunc_execWrDo_e;
                    rba_FeeFs1x_BlockJobData_st.idxExecFunc_en     = rba_FeeFs1x_BlockJob_jobFunc_execWr_e;

                    // only search the latest consistent copy for reading
                    rba_FeeFs1x_BlockJobData_st.idxSearchType_en   = rba_FeeFs1x_BlockJob_searchType_F1C_e;
                }
                else
                {
                    // native NoFallback block

                    // set the exec functions
                    // exec functions differ from non-noFallback path
                    rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en   = rba_FeeFs1x_BlockJob_jobDoFunc_execWrNoFBDo_e;
                    rba_FeeFs1x_BlockJobData_st.idxExecFunc_en     = rba_FeeFs1x_BlockJob_jobFunc_execWrNoFB_e;

                    // only search the latest consistent copy for reading
                    rba_FeeFs1x_BlockJobData_st.idxSearchType_en   = rba_FeeFs1x_BlockJob_searchType_F1C_e;
                }
            }
            else
            {
                // redundant block

                if(!Fee_Prv_ConfigIsBlockNoFallbackActiveByBlockNr( orderStruct_pcst->blockNumber_u16,
                                                                    RBA_FEEFS1X_PRV_CFG_NOFALLBACK_BITS,
                                                                    rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast) )
                {
                    // redundant non-NoFallback block

                    // set the exec functions
                    rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en   = rba_FeeFs1x_BlockJob_jobDoFunc_execWrRedDo_e;
                    rba_FeeFs1x_BlockJobData_st.idxExecFunc_en     = rba_FeeFs1x_BlockJob_jobFunc_execWrRed_e;

                    // only search the latest consistent copy for reading
                    // (redundancy check for the old copies is not executed)
                    rba_FeeFs1x_BlockJobData_st.idxSearchType_en   = rba_FeeFs1x_BlockJob_searchType_F1C_e;
                }
                else
                {
                    // redundant NoFallback-block

                    // set the exec functions
                    // use NoFB specific exec function
                    rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en   = rba_FeeFs1x_BlockJob_jobDoFunc_execWrNoFBRedDo_e;
                    rba_FeeFs1x_BlockJobData_st.idxExecFunc_en     = rba_FeeFs1x_BlockJob_jobFunc_execWrNoFBRed_e;

                    // search for both latest consistent  and redundant copies
                    // (redundant copy is supposed to get destroyed)
                    rba_FeeFs1x_BlockJobData_st.idxSearchType_en   = rba_FeeFs1x_BlockJob_searchType_F2C_e;
                }
            }
        }
        else
        {
            // set the exec functions for the unknown block handling
            rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en   = rba_FeeFs1x_BlockJob_jobFunc_execWrUnknownBlkDo_e;
            rba_FeeFs1x_BlockJobData_st.idxExecFunc_en     = rba_FeeFs1x_BlockJob_jobFunc_execWrUnknownBlk_e;

            // Search is not used, anyway set it to default value
            rba_FeeFs1x_BlockJobData_st.idxSearchType_en   = rba_FeeFs1x_BlockJob_searchType_F1C_e;
        }

        // set the check functions
        rba_FeeFs1x_BlockJobData_st.idxCheckDoFunc_en  = rba_FeeFs1x_BlockJob_jobDoFunc_checkWrDo_e;
        rba_FeeFs1x_BlockJobData_st.idxCheckFunc_en    = rba_FeeFs1x_BlockJob_jobFunc_checkWr_e;

        // store the requested job details
        rba_FeeFs1x_BlockJobData_st.feeIndex_u16      = orderStruct_pcst->blockNumber_u16;
        rba_FeeFs1x_BlockJobData_st.jobMode_en        = orderStruct_pcst->jobMode_en;
        rba_FeeFs1x_BlockJobData_st.isUnknownBlk_b    = orderStruct_pcst->isUnknownBlk_b;
        rba_FeeFs1x_BlockJobData_st.isChunkJob_b      = orderStruct_pcst->isChunkJob_b;
        rba_FeeFs1x_BlockJobData_st.persID_u16        = orderStruct_pcst->idPers_u16;
        rba_FeeFs1x_BlockJobData_st.nrDataBytTot_u16  = orderStruct_pcst->nrBytTot_u16;
        rba_FeeFs1x_BlockJobData_st.statusByte_u16    = orderStruct_pcst->statusFlag_u16;

        // in fast mode search is skipped: if initialized with "no copy found" write will always be executed
        rba_FeeFs1x_BlockJobData_st.searchRetVal_en = rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e;

        // pass the relevant jobdata to the unit WrJob
        rba_FeeFs1x_WrJob_initJob(orderStruct_pcst);

        if(rba_FeeFs1x_BlockJobData_st.isChunkJob_b)
        {
            /* For chunk wise job, skip the search (as this could be for known block write) and
             * also it is not possible to calculate crc from user buffer (as complete data might not be available),
             * instead go directly to store the data */
            rba_FeeFs1x_BlockJobData_st.state_en = rba_FeeFs1x_BlockJob_stm_execJob_e;
        }
        else if(FEE_RB_ALLJOBS_ALLSTEPS_E == rba_FeeFs1x_BlockJobData_st.jobMode_en)
        {
            // start with the search for already existent copies
            rba_FeeFs1x_BlockJobData_st.state_en = rba_FeeFs1x_BlockJob_stm_search_e;
        }
        else
        {
            // in fast mode, search for existent copies and data comparison between user data
            // and existent copy is skipped. in check requested only the CRC calculation has to be executed
            rba_FeeFs1x_BlockJobData_st.state_en = rba_FeeFs1x_BlockJob_stm_checkReq_e;
        }
    }
    else
    {
        /* Chunk operation was already started and this is the next chunk. The variable still holds the old values
         * just update the new parameters (chunk length and buffer pointer). */

        // Since only partial information is available, there cannot be a retry on error.
        rba_FeeFs1x_BlockJobData_st.allowedRetries_u8 = 0u;

        // go directly to write the block
        rba_FeeFs1x_BlockJobData_st.state_en          = rba_FeeFs1x_BlockJob_stm_execJob_e;
    }

    rba_FeeFs1x_BlockJobData_st.userbuffer_pcu8       = orderStruct_pcst->bfr_pcu8;
    rba_FeeFs1x_BlockJobData_st.nrDataBytCur_u16      = orderStruct_pcst->length_u16;
    rba_FeeFs1x_BlockJobData_st.entry_b               = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Rb_Maintain The function supports maintain of known blocks only. the check for known or unknown is
 * already done in FeeHl so not performed here again. The API should be called only for known blocks.
 * \attention: Function should be called only with valid parameter value!
 * \param   orderStruct_pcst : struct containing the data collected by the Fee external service APIs
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_Rb_Maintain(Fee_Prv_JobDesc_tst const * orderStruct_pcst)
{
    if(rba_FeeFs1x_Prv_isMaintainActive())
    {
        // reset the retry counter
        rba_FeeFs1x_BlockJobData_st.allowedRetries_u8 = RBA_FEEFS1X_PRV_MAX_RD_WR_COMP_RETRIES;

        if(!Fee_Prv_ConfigIsBlockNoFallbackActiveByBlockNr( orderStruct_pcst->blockNumber_u16,
                                                            RBA_FEEFS1X_PRV_CFG_NOFALLBACK_BITS,
                                                            rba_FeeFs1x_Prv_Cfg_BlockPropertiesTable_ast) )
        {
            // non-NoFallback block

            // set the exec functions
            rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en   = rba_FeeFs1x_BlockJob_jobDoFunc_execMtDo_e;
            rba_FeeFs1x_BlockJobData_st.idxExecFunc_en     = rba_FeeFs1x_BlockJob_jobFunc_execMt_e;
        }
        else
        {
            // noFallback block

            // set the exec functions
            // Select the NoFallback Exec functions
            rba_FeeFs1x_BlockJobData_st.idxExecDoFunc_en   = rba_FeeFs1x_BlockJob_jobDoFunc_execMtNoFBDo_e;
            rba_FeeFs1x_BlockJobData_st.idxExecFunc_en     = rba_FeeFs1x_BlockJob_jobFunc_execMtNoFB_e;
        }

        // set the check functions
        rba_FeeFs1x_BlockJobData_st.idxCheckDoFunc_en  = rba_FeeFs1x_BlockJob_jobDoFunc_checkMtDo_e;
        rba_FeeFs1x_BlockJobData_st.idxCheckFunc_en    = rba_FeeFs1x_BlockJob_jobFunc_checkMt_e;

        // search for both the copies
        rba_FeeFs1x_BlockJobData_st.idxSearchType_en   = rba_FeeFs1x_BlockJob_searchType_F2C_e;

        // initialize the searcher return value
        rba_FeeFs1x_BlockJobData_st.searchRetVal_en = rba_FeeFs1x_Searcher_RetVal_NoCopyFound_e;

        // store the requested index and job mode
        rba_FeeFs1x_BlockJobData_st.feeIndex_u16     = orderStruct_pcst->blockNumber_u16;
        rba_FeeFs1x_BlockJobData_st.jobMode_en       = orderStruct_pcst->jobMode_en;
        rba_FeeFs1x_BlockJobData_st.persID_u16       = orderStruct_pcst->idPers_u16;
        rba_FeeFs1x_BlockJobData_st.nrDataBytTot_u16 = orderStruct_pcst->length_u16;
        rba_FeeFs1x_BlockJobData_st.nrDataBytCur_u16 = orderStruct_pcst->length_u16;    // block written in one chunk
        rba_FeeFs1x_BlockJobData_st.isChunkJob_b     = FALSE;
        rba_FeeFs1x_BlockJobData_st.statusByte_u16   = orderStruct_pcst->statusFlag_u16;

        //pass the relevant jobdata to the unit MaintainJob
        rba_FeeFs1x_MtJob_initJob(rba_FeeFs1x_BlockJobData_st.feeIndex_u16);

        // switch to search state to enable the job
        rba_FeeFs1x_BlockJobData_st.entry_b            = TRUE;
        rba_FeeFs1x_BlockJobData_st.state_en           = rba_FeeFs1x_BlockJob_stm_search_e;
    }
    else
    {
        // maintain called but deactivated or maintain called with no maintain job request
        // the idle state in blockjobDo causes an error-return
        rba_FeeFs1x_BlockJobData_st.state_en = rba_FeeFs1x_BlockJob_stm_idle_e;
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_GetDetailedBlkInfo
 * returns the detailed block info after reading the respective block
 * \attention: Function should be called only with valid parameter value!
 * \param   blockNumber_u16 : block number
 * \return  detailed block info
 * \retval MEMIF_RB_DETAILED_BLK_INFO_NOT_AVAILABLE_E       : Detailed block info not configured
 * \retval MEMIF_RB_DETAILED_BLK_INFO_INIT_E                : Detailed block info not available
 * \retval MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_MISSING_E: Block inconsitent: valid block (header) wasn't found
 * \retval MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_DATA_E   : Block inconsitent: data CRC of block is wrong
 * \seealso
 * \usedresources
 *********************************************************************
 */
MemIf_Rb_DetailedBlkInfo_ten rba_FeeFs1x_GetDetailedBlkInfo(uint16 blockNumber_u16)
{
# if(defined(RBA_FEEFS1X_PRV_CFG_DETAILED_BLK_INFO_API) && (TRUE == RBA_FEEFS1X_PRV_CFG_DETAILED_BLK_INFO_API))
    return rba_FeeFs1x_BlockJob_ReadJobResultDetailed_aen[blockNumber_u16];
# else
    (void)blockNumber_u16;
    return MEMIF_RB_DETAILED_BLK_INFO_NOT_AVAILABLE_E;
# endif
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_ReportBlkDataCrcError
 * reports block data CRC error
 * \seealso rba_FeeFs1x_BlockJob_IsBlkDataCrcError
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BlockJob_ReportBlkDataCrcError(void)
{
    rba_FeeFs1x_BlockJob_BlkDataCrcError_b = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_IsBlkDataCrcError
 * \return Detection of block data CRC error during last/ongoing read job
 * \retval TRUE : Block data CRC error detected
 * \retval FALSE: No block data CRC error detected
 * \seealso rba_FeeFs1x_BlockJob_ReportBlkDataCrcError
 * \usedresources
 *********************************************************************
 */
boolean rba_FeeFs1x_BlockJob_IsBlkDataCrcError(void)
{
    return rba_FeeFs1x_BlockJob_BlkDataCrcError_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_mapBlockJobRetToMemifRet
 * translates the internal return value to the external Memif return value
 * \param   bjRetVal_en : internal return value to be translated
 * \return  mapped internal result to the external one
 * \retval MEMIF_JOB_OK            : job executed correctly
 * \retval MEMIF_JOB_FAILED        : job caused an error
 * \retval MEMIF_JOB_PENDING       : job ongoing, continue during next mainfunction call
 * \retval MEMIF_JOB_CANCELED      :
 * \retval MEMIF_BLOCK_INCONSISTENT: blockjob(read) didn't find a block copy
 * \retval MEMIF_BLOCK_INVALID     : blockjob(read) found an explicit block invalidation marker
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE MemIf_JobResultType rba_FeeFs1x_BlockJob_mapBlockJobRetToMemifRet(rba_FeeFs1x_BlockJob_ReturnType_ten bjRetVal_en)
{
    MemIf_JobResultType retVal_en;
    switch (bjRetVal_en)
    {
        case rba_FeeFs1x_BlockJob_ReturnType_pending_e:
        {
            retVal_en = MEMIF_JOB_PENDING;
            break;
        }
        case rba_FeeFs1x_BlockJob_ReturnType_JobOK_e:
        {
            retVal_en = MEMIF_JOB_OK;
            break;
        }
        case rba_FeeFs1x_BlockJob_ReturnType_inconsistent_e:
        {
            retVal_en = MEMIF_BLOCK_INCONSISTENT;
            break;
        }
        case rba_FeeFs1x_BlockJob_ReturnType_invalidated_e:
        {
            retVal_en = MEMIF_BLOCK_INVALID;
            break;
        }
        // state execute must not occur on exit of the statemachine, must be already handled by check function
        case rba_FeeFs1x_BlockJob_ReturnType_execute_e:
        case rba_FeeFs1x_BlockJob_ReturnType_error_e:
        default:
        {
            retVal_en = MEMIF_JOB_FAILED;
            break;
        }
    }

    return retVal_en;

}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_callJobFunc
 * encapsulating function for calling the specific jobfunction protected against floating pointers by
 * only allowing inbound indize on the constant jobFunc-functionpointer array
 * \attention: Function should be called only with valid parameter value!
 * \param   selectedFunc_en : index/name of the function to be called out of the jobFunctions-
 * array
 * \param   searchRetVal_en : parameter to be passed to the called function
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
static void rba_FeeFs1x_BlockJob_callJobFunc(
        rba_FeeFs1x_BlockJob_jobFunc_ten selectedFunc_en ,
        rba_FeeFs1x_Searcher_RetVal_ten searchRetVal_en)
{
    // inbound / defined function pointer
    // call the defined function
    rba_FeeFs1x_BlockJob_jobFunctions_apfct[selectedFunc_en](searchRetVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_BlockJob_callJobDoFunc
 * encapsulating function for calling the specific jobDofunction protected against floating pointers by
 * only allowing inbound indize on the constant jobDoFunc-functionpointer array
 * \param   selectedFunc_en : index/name of the function to be called out of the jobDoFunctions-
 * array
 * \return  job result
 * \retval rba_FeeFs1x_BlockJob_ReturnType_pending_e      : job is not yet finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_execute_e      : exec action has to be perfomed
 * \retval rba_FeeFs1x_BlockJob_ReturnType_JobOK_e        : data is verified and validated, job is finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_inconsistent_e : data is corrupted , job is finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_invalidated_e  :consistent block copy is invalidated, job finished
 * \retval rba_FeeFs1x_BlockJob_ReturnType_error_e        : error during processing, job aborted
 * \seealso
 * \usedresources
 *********************************************************************
 */
static rba_FeeFs1x_BlockJob_ReturnType_ten rba_FeeFs1x_BlockJob_callJobDoFunc(
        rba_FeeFs1x_BlockJob_jobDoFunc_ten selectedFunc_en)
{
    rba_FeeFs1x_BlockJob_ReturnType_ten retVal_en;
    if(rba_FeeFs1x_BlockJob_jobDoFunc_Max_e > selectedFunc_en)
    {
        retVal_en = rba_FeeFs1x_BlockJob_jobDoFunctions_apfct[selectedFunc_en]();
    }
    else
    {
        retVal_en = rba_FeeFs1x_BlockJob_ReturnType_error_e;
    }

    return retVal_en;
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

