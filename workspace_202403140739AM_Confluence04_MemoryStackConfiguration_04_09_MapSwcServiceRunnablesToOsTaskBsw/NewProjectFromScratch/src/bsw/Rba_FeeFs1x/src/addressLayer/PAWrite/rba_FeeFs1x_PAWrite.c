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

#include "Fee_Prv_Cfg.h"


#if(defined(RBA_FEEFS1X_PRV_CFG_ENABLED) && (TRUE == RBA_FEEFS1X_PRV_CFG_ENABLED))

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "rba_FeeFs1x_Prv_Cfg.h"

#include "rba_FeeFs1x_Prv_PAWrite.h"
#include "rba_FeeFs1x_Prv_PAWriteTypes.h"

#include "rba_FeeFs1x_Prv_PAMap.h"
#include "rba_FeeFs1x_Prv_Sector.h"
#include "rba_FeeFs1x_Prv_SectorReorg.h"

#include "Fee_Prv_Lib.h"
#include "Fee_Prv_Medium.h"
#include "Fee_Rb_Types.h"

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


static rba_FeeFs1x_PAWrite_prepareSpace_data_tst      rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_maxNr_e];
static rba_FeeFs1x_PAWrite_write_data_tst             rba_FeeFs1x_PAWrite_write_data_st;
static rba_FeeFs1x_PAWrite_copyFls2Fls_data_tst       rba_FeeFs1x_PAWrite_copyFls2Fls_data_st;


#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_prepSectSt(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_checkHdrFit(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_fillSectEnd(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_switchActive(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_checkMem(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_fillSectorForHSR(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_execHSR(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst);

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_writeDo_limitChunkToSect(rba_FeeFs1x_PAWrite_write_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_writeDo_writeChunk(rba_FeeFs1x_PAWrite_write_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_writeDo_switchActiveSect(rba_FeeFs1x_PAWrite_write_data_tst * fsm_pst);


LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_copyFls2FlsDo_limit(rba_FeeFs1x_PAWrite_copyFls2Fls_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_copyFls2FlsDo_copy(rba_FeeFs1x_PAWrite_copyFls2Fls_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_copyFls2FlsDo_copy_exit(
        rba_FeeFs1x_PAWrite_copyFls2Fls_data_tst * fsm_pst,
        MemIf_JobResultType retValInner_en,
        boolean failedInternal_b);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_copyFls2FlsDo_switch(rba_FeeFs1x_PAWrite_copyFls2Fls_data_tst * fsm_pst);
/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 * initialization
 *
 * \param   void:
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */

void rba_FeeFs1x_PAWrite_cancel(void)
{
    rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_jobReq_e].state_en = rba_FeeFs1x_PAWrite_prepareSpace_stm_idle_e;
    rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_reorg_e].state_en = rba_FeeFs1x_PAWrite_prepareSpace_stm_idle_e;
    rba_FeeFs1x_PAWrite_write_data_st.state_en  = rba_FeeFs1x_PAWrite_write_stm_idle_e;

    rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.state_en = rba_FeeFs1x_PAWrite_copyFls2Fls_stm_idle_e;

}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_reorgPrepareSpace
 * Job init function for triggering a prepareSpace job from reorg
 * \param   noDataPages_u32 : amount of data pages requested to be stored within a following write request
 * \param   noHdrPages_u8 :   amount of header pages the block to be stored has to have in one sector(no sector overlap)
 * \param   startPageForRequest_pu32 : pointer to the variable the resulting allowed write page of prepSpace gets returned to
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PAWrite_reorgPrepareSpace(uint32 noDataPages_u32, uint8 noHdrPages_u8, uint32* startPageForRequest_pu32)
{

    {
        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_reorg_e].noDataPages_u32 = noDataPages_u32;
        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_reorg_e].noHdrPages_u8   = noHdrPages_u8;
        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_reorg_e].startPageForRequest_pu32 = startPageForRequest_pu32;
        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_reorg_e].isHSRCheckRequired_b = FALSE;
        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_reorg_e].entry_b = TRUE;
        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_reorg_e].state_en = rba_FeeFs1x_PAWrite_prepareSpace_stm_prepSectSt_e;
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_jobReqPrepareSpace
 * Job init function for triggering a prepareSpace from a usual write job
 * \param noDataPages_u32          : number of data pages requested to be stored within a following write request
 * \param noHdrPages_u8            : number of header pages to be stored (no sector overlap)
 * \param startPageForRequest_pu32 : pointer where the resulting allowed write page of prepSpace gets returned to
 * \param isHSRCheckRequired_b     : TRUE => check for HSR and trigger reorg when HSR limit is reached
 *                                   FALSE => skip HSR check and allow writes to happen beyond HSR limit as well
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PAWrite_jobReqPrepareSpace(uint32 noDataPages_u32, uint8 noHdrPages_u8, uint32* startPageForRequest_pu32, Fee_Rb_JobMode_ten jobMode_en)
{

        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_jobReq_e].noDataPages_u32 = noDataPages_u32;
        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_jobReq_e].noHdrPages_u8   = noHdrPages_u8;
        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_jobReq_e].startPageForRequest_pu32 = startPageForRequest_pu32;
        if((FEE_RB_WRITEJOB_SKIP_HARDREORG_WRITE_ONLY_E == jobMode_en) || (FEE_RB_INVALIDATEJOB_SKIP_HARDREORG_WRITE_ONLY_E == jobMode_en))
        {
            rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_jobReq_e].isHSRCheckRequired_b = FALSE;
        }
        else
        {
            rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_jobReq_e].isHSRCheckRequired_b = TRUE;
        }
        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_jobReq_e].entry_b = TRUE;
        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[rba_FeeFs1x_PAWrite_jobReq_e].state_en = rba_FeeFs1x_PAWrite_prepareSpace_stm_prepSectSt_e;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_prepareSpaceDo
 * statemachine for preparing memory space in case of block transfer during reorganizations or for writing of new blocks
 * rba_FeeFs1x_PAWrite_prepareSpace is the function to be called before writing.
 *
 * It performs
 *  - state correction of the sector the write page is in
 *       In case of an ongoing reorg this is required for force reorg handling
 *  - for writing blocks: HSR check is done and if required executes the HSR
 *  - for reorganizations: Memory full check is done and execution of robust sector handling
 *  - switching to the next sector, if the header doesn't fit into the current one
 *
 * \attention: Function should be called only with valid parameter value!
 * \param   jobType_en                   : type of job, e.g write job or a reorganization
 *
 * \return  returns whether the prepare space function completed successfully
 * \retval  rba_FeeFs1x_JobOK_e          : prepare space done, startPageForRequest_pu32 usable for starting the write
 * \retval  rba_FeeFs1x_JobFailed_e      : prepare space done, retrigger is required
 * \retval  rba_FeeFs1x_Pending_e        : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e  : internal processing error
 * \retval  rba_FeeFs1x_ErrorExternal_e  : error accessing external components(e.g. Fls)
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo(rba_FeeFs1x_PAWrite_jobType_ten jobType_en)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(rba_FeeFs1x_PAWrite_prepareSpace_data_ast[jobType_en].state_en)
    {
        case rba_FeeFs1x_PAWrite_prepareSpace_stm_prepSectSt_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_prepareSpaceDo_prepSectSt(&rba_FeeFs1x_PAWrite_prepareSpace_data_ast[jobType_en]);
        }break;
        case rba_FeeFs1x_PAWrite_prepareSpace_stm_checkHdrFit_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_prepareSpaceDo_checkHdrFit(&rba_FeeFs1x_PAWrite_prepareSpace_data_ast[jobType_en]);
        }break;
        case rba_FeeFs1x_PAWrite_prepareSpace_stm_fillSectEnd_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_prepareSpaceDo_fillSectEnd(&rba_FeeFs1x_PAWrite_prepareSpace_data_ast[jobType_en]);
        }break;
        case rba_FeeFs1x_PAWrite_prepareSpace_stm_switchActive_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_prepareSpaceDo_switchActive(&rba_FeeFs1x_PAWrite_prepareSpace_data_ast[jobType_en]);
        }break;
        case rba_FeeFs1x_PAWrite_prepareSpace_stm_checkMem_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_prepareSpaceDo_checkMem(&rba_FeeFs1x_PAWrite_prepareSpace_data_ast[jobType_en]);
        }break;
        case rba_FeeFs1x_PAWrite_prepareSpace_stm_fillSectorForHSR_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_prepareSpaceDo_fillSectorForHSR(&rba_FeeFs1x_PAWrite_prepareSpace_data_ast[jobType_en]);
        }break;
        case rba_FeeFs1x_PAWrite_prepareSpace_stm_execHSR_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_prepareSpaceDo_execHSR(&rba_FeeFs1x_PAWrite_prepareSpace_data_ast[jobType_en]);
        }break;
        default:
        {
            // unexpected state
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        // as soon as the job is not pendign any more, reset the job data
        rba_FeeFs1x_PAWrite_prepareSpace_data_ast[jobType_en].state_en = rba_FeeFs1x_PAWrite_prepareSpace_stm_idle_e;
    }

    return  retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_prepareSpaceDo_prepSectSt
 * statefunction of prepareSpaceDo
 * \param   fsm_pst : pointer to the statemachine data
 * \return  return value to be returned by the  statemachine
 * \retval  rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval   rba_FeeFs1x_ErrorExternal_e: error during use of external component( Fls or CRC)
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_prepSectSt(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValPrepSectStates_en;

    if(FEE_PRV_LIBENTRY)
    {
        if(E_OK == rba_FeeFs1x_PAMap_prepareSectorStatesOnCurrWrPage())
        {
            retValPrepSectStates_en = rba_FeeFs1x_PAMap_prepareSectorStatesDo();
        }
        else
        {
            retValPrepSectStates_en = rba_FeeFs1x_JobFailed_e;
        }
    }
    else
    {
        retValPrepSectStates_en = rba_FeeFs1x_PAMap_prepareSectorStatesDo();
    }

    switch(retValPrepSectStates_en)
    {
        case rba_FeeFs1x_Pending_e:
            // Fls errors during processing
        case rba_FeeFs1x_ErrorExternal_e:
        {
            retVal_en = retValPrepSectStates_en;
        }break;

        case rba_FeeFs1x_JobOK_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_checkHdrFit_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;

        case rba_FeeFs1x_JobFailed_e:
        {
            // acceptor function failed, eventually flash is full
            // check, if in case of a normal job a sector reorg has to be performed
            // or in case of a sector reorg JobFailed has to be returned
            FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_checkMem_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        default:
        {
            // unexpected return value OR error returned
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_prepareSpaceDo_checkHdrFit
 * statefunction of prepareSpaceDo
 * \param   fsm_pst : pointer to the statemachine data
 * \return  return value to be returned by the  statemachine
 * \retval  rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval   rba_FeeFs1x_ErrorExternal_e: error during use of external component( Fls or CRC)
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_checkHdrFit(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    uint32 pageNoLastPageInSect_u32;
    uint32 pageNoLastHdrPage_u32;

    uint32 currWrPage_u32;

    uint8 logSectIdxWrPage_u8;

    // -- Detect the last page of the sector the write page is in --

    // get the current write page
    currWrPage_u32 = rba_FeeFs1x_PAMap_getCurrWrPage();

    // get the logical sector index of the write page
    // return of rba_FeeFs1x_PAWrite_prepareSpaceDo_checkHdrFit already checked in prepare sector state
    // will never come with return E_NOT_OK here
    (void) rba_FeeFs1x_PAMap_getLogIdxByPage(currWrPage_u32, &logSectIdxWrPage_u8);

    pageNoLastPageInSect_u32= rba_FeeFs1x_Sector_getLogEndPage(logSectIdxWrPage_u8);

    pageNoLastHdrPage_u32 = currWrPage_u32 + fsm_pst->noHdrPages_u8 - 1u;

    // compare the last header page with the last page of the sector (the current write page is in)
    if(pageNoLastHdrPage_u32 <= pageNoLastPageInSect_u32)
    {
        // block header is fitting into the sector of the current write page
        FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_checkMem_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }
    else
    {
        // block header would overlap the sector boundary
        // --> fill the sector end and afterwards switch the active sector
        FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_fillSectEnd_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_prepareSpaceDo_fillSectEnd
 * statefunction of prepareSpaceDo
 * \param   fsm_pst : pointer to the statemachine data
 * \return  return value to be returned by the  statemachine
 * \retval  rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval   rba_FeeFs1x_JobFailed_e    : memory is full
 * \retval   rba_FeeFs1x_ErrorExternal_e: error during use of external component( Fls or CRC)
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_fillSectEnd(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValFill_en;

    // state entry?
    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_PAMap_fillSectorEnd();
    }

    // Cyclic call the do-function of the job
    retValFill_en =  rba_FeeFs1x_PAMap_fillSectorEndDo();

    // evaluate the job result
    switch (retValFill_en)
    {
        // if the job needs more cycles to complete, return pending to get called again during next cycle
        case rba_FeeFs1x_Pending_e:
        // on error, hand over the error to the caller and stop the state machine
        case rba_FeeFs1x_ErrorExternal_e:
        {
            retVal_en = retValFill_en;
        }break;
        case rba_FeeFs1x_JobOK_e:
        {
            // after filling the actual sector, the current write page points to the first page of the next sector
            // check, if this sector is available
            if(rba_FeeFs1x_Sector_isMemFull(rba_FeeFs1x_PAMap_getCurrWrPage()))
            {
                if(fsm_pst->isHSRCheckRequired_b)
                {
                    // sector number out of range, switch to check memory
                    FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_checkMem_e);
                    retVal_en = rba_FeeFs1x_Pending_e;
                 }
                else
                {
                    retVal_en = rba_FeeFs1x_JobFailed_e;
                }
            }
            else
            {
                /* sector is available, switch to next sector is feasible */
                FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_switchActive_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }
        }break;
        default:
        {
            // on error or unexpected return value, return error to the caller and stop the state machine
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_prepareSpaceDo_switchActive
 * statefunction of prepareSpaceDo
 * \param   fsm_pst : pointer to the statemachine data
 * \return  return value to be returned by the  statemachine
 * \retval  rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e         : internal processing error
 * \retval   rba_FeeFs1x_ErrorExternal_e: error during use of external component( Fls or CRC)
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_switchActive(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValSwitch_en;

    // state entry?
    if(FEE_PRV_LIBENTRY)
    {
        // at this point of time the current write page is already in the sector,
        // where data has to be written. So use the sector switch based on the current write page
        if(E_OK == rba_FeeFs1x_PAMap_prepareSectorStatesOnCurrWrPage())
        {
            // if job is accepted, trigger the first jobDo
            retValSwitch_en = rba_FeeFs1x_PAMap_prepareSectorStatesDo();
        }
        else
        {
            // if job is rejected, exit the statemachine with an internal processing error
            retValSwitch_en = rba_FeeFs1x_ErrorInternal_e;
        }

    }
    else
    {
        // on non-first calls, trigger the do-function of the job
        retValSwitch_en = rba_FeeFs1x_PAMap_prepareSectorStatesDo();
    }

    // evaluate the job result
    switch (retValSwitch_en)
    {
        // if the job needs more cycles to complete, return pending to get called again during next cycle
        case rba_FeeFs1x_Pending_e:
        // on error, hand over the error to the caller and stop the state machine
        case rba_FeeFs1x_ErrorExternal_e:
        {
            retVal_en = retValSwitch_en;
        }break;
        case rba_FeeFs1x_JobOK_e:
        {
            // as soon as the state's job is finished, transit to next state
            FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_checkMem_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        default:
        {
            // on error or unexpected return value, return error to the caller and stop the state machine
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_prepareSpaceDo_checkMem
 * statefunction of prepareSpaceDo
 * \param   fsm_pst : pointer to the statemachine data
 * \return  return value to be returned by the  statemachine
 * \retval  rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e         : internal processing error
 * \retval   rba_FeeFs1x_ErrorExternal_e: error during use of external component( Fls or CRC)
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_checkMem(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    uint32 lastPageOfBlock_u32;
    uint32 currWrPage_u32;

    // calculate the page number the last page of the block would get starting from current write address
    currWrPage_u32 = rba_FeeFs1x_PAMap_getCurrWrPage();
    lastPageOfBlock_u32 = fsm_pst->noHdrPages_u8 + fsm_pst->noDataPages_u32 + currWrPage_u32;

    if (fsm_pst->isHSRCheckRequired_b)
    {
        if(rba_FeeFs1x_Reorg_checkHSRNecessity(lastPageOfBlock_u32))
        {
            // preparing the HSR state
            FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_fillSectorForHSR_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }
        else
        {
            // HSR is not required, exit the prepare space, the write is now allowed at the returned page
            *(fsm_pst->startPageForRequest_pu32) = currWrPage_u32;

            // exit the job with JobOK
            retVal_en = rba_FeeFs1x_JobOK_e;
        }
    }
    else
    {
        // check if this last page is leading to mem full, trigger robust sector handling
        if(rba_FeeFs1x_Sector_isMemFull(lastPageOfBlock_u32))
        {
            // Robust sector handling required
            retVal_en = rba_FeeFs1x_JobFailed_e;
        }
        else
        {
            // return the new current write page after (check and execute if required) switching active sectors and executing HSR
            *(fsm_pst->startPageForRequest_pu32) = currWrPage_u32;

            // exit the job with JobOK
            retVal_en = rba_FeeFs1x_JobOK_e;
        }
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_prepareSpaceDo_fillSectorForHSR
 * statefunction of prepareSpaceDo
 * \param   fsm_pst : pointer to the statemachine data
 * \return  return value to be returned by the  statemachine
 * \retval  rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e         : internal processing error
 * \retval   rba_FeeFs1x_ErrorExternal_e: error during use of external component( Fls or CRC)
 * \seealso
 * \usedresources
 *********************************************************************
 */
# if (RBA_FEEFS1X_PRV_CFG_NR_FLASH_BANKS_AVAILABLE == 2u)
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_fillSectorForHSR(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    // on two-sector emulation, it has to be prevented that the currently active sector gets used by the reorg.
    // Thus, the sector end gets filled as soon as an HSR need is detected.
    // But the filling is required only when the current write page is still in the 0th sector
    uint32 currWrPage_u32;

    currWrPage_u32 = rba_FeeFs1x_PAMap_getCurrWrPage();

    /* Check if logical sector 0 has to be filled up before starting with transfer of blocks.
     * Enter the state in case the sector 0 filling is ongoing (indicated when fsm_pst->entry_b = FALSE),
     * since write page gets incremented even before the write is completed (to support cancel feature). */
    if((RBA_FEEFS1X_PRV_CFG_LOGL_PAGES_PER_SECTOR > currWrPage_u32) ||
       (!fsm_pst->entry_b))
    {
        rba_FeeFs1x_RetVal_ten retValSwitch_en;
        // state entry?
        if(FEE_PRV_LIBENTRY)
        {
            // first call of the state --> init the state's job
            rba_FeeFs1x_PAMap_fillSectorEnd();
        }

        // Cyclic call the do-function of the job
        retValSwitch_en = rba_FeeFs1x_PAMap_fillSectorEndDo();

        // evaluate the job result
        switch (retValSwitch_en)
        {
            case rba_FeeFs1x_Pending_e:
                // if the job needs more cycles to complete, return pending to get called again during next cycle
            case rba_FeeFs1x_ErrorExternal_e:
                // on error, hand over the error to the caller and stop the state machine
            {
                retVal_en = retValSwitch_en;
            }break;
            case rba_FeeFs1x_JobOK_e:
            {
                // as soon as the state's job is finished, transit to next state
                FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_execHSR_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }break;
            default:
            {
                // on error or unexpected return value, return error to the caller and stop the state machine
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
            }break;
        }
    }
    else
    {
        // do nothing and proceed with next state
        FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_execHSR_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }

    return retVal_en;
}
# else
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_fillSectorForHSR(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst)
{
    // For more than two sectors do nothing and proceed with HSR state
    FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_execHSR_e);
    return (rba_FeeFs1x_Pending_e);
}
# endif

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_prepareSpaceDo_execHSR
 * statefunction of prepareSpaceDo
 * \param   fsm_pst : pointer to the statemachine data
 * \return  return value to be returned by the  statemachine
 * \retval  rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval  rba_FeeFs1x_JobFailed_e     : job finished successfully but restart is required
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e : internal processing error
 * \retval   rba_FeeFs1x_ErrorExternal_e: error during use of external component( Fls or CRC)
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_prepareSpaceDo_execHSR(rba_FeeFs1x_PAWrite_prepareSpace_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValReorg_en;
    // state entry?
    if(FEE_PRV_LIBENTRY)
    {
        // first call of the state --> init the state's job
        rba_FeeFs1x_Reorg_reorganiseSector(FALSE);
    }

    // trigger the do-function of the job
    retValReorg_en = rba_FeeFs1x_Reorg_reorganiseSectorDo();

    // evaluate the job result
    switch(retValReorg_en)
    {
        case rba_FeeFs1x_JobFailed_e:
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_Pending_e:
        {
            retVal_en = retValReorg_en;
        }break;
        case rba_FeeFs1x_JobOK_e:
        {
            // after a single reorg, restart the prepSpace checks for the current write request again
            FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_prepareSpace_stm_prepSectSt_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        default:
        {
            // on error or any unexpected return value, return error
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}


/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_copyFls2Fls
 * \param   source_startPage_u32: first page the noOfPages shall be copied from
 * \param   noOfPages_u32       : amount of pages to be copies
 * \param   dest_startPage_u32  : page in DFLASH the pages from source shall be copied to
 * \param   updateCurrWrPage_b  : if set, the job updates the write page after completion
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PAWrite_copyFls2Fls(uint32 source_startPage_u32, uint32 noOfPages_u32, uint32 dest_startPage_u32 , boolean updateCurrWrPage_b)
{
    uint8 writeSector_u8;
    uint8 activeSector_u8;

    // get the current write page for checking the validity of the request length
    (void) rba_FeeFs1x_PAMap_getLogIdxByPage(dest_startPage_u32 , &writeSector_u8);

    // Last page should be less than CurrWrPage
    // inbound --> job accepted
    //store the parameters of the job
    rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.noOfPagesRemaining_u32  = noOfPages_u32;
    rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.noOfPagesDone_u32       = 0u;

    rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.nextChunkSourcePage_u32 = source_startPage_u32;
    rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.nextChunkDestPage_u32   = dest_startPage_u32;

    rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.currChunkSourcePage_u32 = 0u;
    rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.currChunkDestPage_u32   = 0u;

    rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.updateCurrWrPage_b      = updateCurrWrPage_b;

    activeSector_u8 = rba_FeeFs1x_Sector_getActiveSector();
    // set the state to the first one to be processed
    if(writeSector_u8 > activeSector_u8)
    {
        // if the start page of the data to be written is not in the actual sector, then first switch the sector
        rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.state_en = rba_FeeFs1x_PAWrite_copyFls2Fls_stm_switch_e;
    }
    else
    {
        // if the start page of the data to be written is in the actual sector start writing the data
        rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.state_en =  rba_FeeFs1x_PAWrite_copyFls2Fls_stm_limit_e;
    }
    rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.entry_b  = TRUE;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_copyFls2FlsDo
 * \return  job result
 * \retval  rba_FeeFs1x_JobOK_e,       : job passed
 * \retval  rba_FeeFs1x_Pending_e,     : job ongoing, call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e: Fs1x internal error
 * \retval  rba_FeeFs1x_ErrorExternal_e: error during read or write
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_copyFls2FlsDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch (rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.state_en)
    {
        case rba_FeeFs1x_PAWrite_copyFls2Fls_stm_limit_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_copyFls2FlsDo_limit(&rba_FeeFs1x_PAWrite_copyFls2Fls_data_st);
        }break;
        case rba_FeeFs1x_PAWrite_copyFls2Fls_stm_copy_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_copyFls2FlsDo_copy(&rba_FeeFs1x_PAWrite_copyFls2Fls_data_st);
        }break;
        case rba_FeeFs1x_PAWrite_copyFls2Fls_stm_switch_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_copyFls2FlsDo_switch(&rba_FeeFs1x_PAWrite_copyFls2Fls_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_PAWrite_copyFls2Fls_data_st.state_en = rba_FeeFs1x_PAWrite_copyFls2Fls_stm_idle_e;
    }
    return  retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_copyFls2FlsDo_limit
 * statefunction of copyFls2FlsDo
 * \return  job result
 * \retval  rba_FeeFs1x_JobOK_e,       : return job passed
 * \retval  rba_FeeFs1x_Pending_e,     : job ongoing, call the statemachine again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e: return Fs1x internal error
 * \retval  rba_FeeFs1x_ErrorExternal_e: return external error_error during read or write
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_copyFls2FlsDo_limit(rba_FeeFs1x_PAWrite_copyFls2Fls_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    uint32 maxChunkSizeSrc_u32;
    uint32 maxChunkSizeDest_u32;

    maxChunkSizeSrc_u32 = rba_FeeFs1x_PAMap_LimitRequestToSectorBoundaries(fsm_pst->nextChunkSourcePage_u32,
                                                        0u,
                                                        (fsm_pst->noOfPagesRemaining_u32 * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE));

    maxChunkSizeDest_u32 = rba_FeeFs1x_PAMap_LimitRequestToSectorBoundaries(fsm_pst->nextChunkDestPage_u32,
                                                        0u,
                                                        (fsm_pst->noOfPagesRemaining_u32 * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE));

    // store the current amount of pages and
    // determine the stronger limiter
    if(maxChunkSizeDest_u32 <= maxChunkSizeSrc_u32)
    {
        fsm_pst->currChunkIsLimitedByDest_b = TRUE;
        fsm_pst->currChunkNoOfPages_u32     = maxChunkSizeDest_u32 / RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;
    }
    else
    {
        fsm_pst->currChunkIsLimitedByDest_b = FALSE;
        fsm_pst->currChunkNoOfPages_u32     = maxChunkSizeSrc_u32 / RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;
    }

    // store the start pages of the current chunk
    fsm_pst->currChunkSourcePage_u32 = fsm_pst->nextChunkSourcePage_u32;
    fsm_pst->currChunkDestPage_u32   = fsm_pst->nextChunkDestPage_u32;

    // prepare the next chunk start pages
    fsm_pst->nextChunkSourcePage_u32 = fsm_pst->currChunkSourcePage_u32 + fsm_pst->currChunkNoOfPages_u32;
    fsm_pst->nextChunkDestPage_u32   = fsm_pst->currChunkDestPage_u32   + fsm_pst->currChunkNoOfPages_u32;

    // switch the state to copy the chunk
    FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_copyFls2Fls_stm_copy_e);
    retVal_en = rba_FeeFs1x_Pending_e;

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_copyFls2FlsDo_copy
 * statefunction of copyFls2FlsDo
 * \return  job result
 * \retval  rba_FeeFs1x_JobOK_e,       : return job passed
 * \retval  rba_FeeFs1x_Pending_e,     : job ongoing, call the statemachine again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e: return Fs1x internal error
 * \retval  rba_FeeFs1x_ErrorExternal_e: return external error_error during read or write
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_copyFls2FlsDo_copy(rba_FeeFs1x_PAWrite_copyFls2Fls_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    MemIf_JobResultType retValInner_en;

    if(FEE_PRV_LIBENTRY)
    {
        uint32 destAdr_u32;
        uint32 srcAdr_u32;
        uint32 length_u32;

        destAdr_u32 = rba_FeeFs1x_PAMap_getAddressByPage(fsm_pst->currChunkDestPage_u32);
        srcAdr_u32  = rba_FeeFs1x_PAMap_getAddressByPage(fsm_pst->currChunkSourcePage_u32);

        length_u32 = fsm_pst->currChunkNoOfPages_u32 * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE;

        /* Update the current write page if required ->
         *  On error, retry should not be done on this area, so does not matter if the write was successful or not.*/
        if(fsm_pst->updateCurrWrPage_b)
        {
            rba_FeeFs1x_PAMap_setCurrWrPage(fsm_pst->nextChunkDestPage_u32);
        }

        // use Fee_medium copy functionality which is skipping the blank check before copying.
        // blank check before copying is not required
        Fee_Prv_MediumCopy(Fee_Rb_DeviceName, srcAdr_u32, 0u, destAdr_u32, 0u, length_u32, FEE_PRV_MEDIUM_SEQ_COPY_COMP_E);
    }

    /* Cyclic call the Do function */
    retValInner_en = Fee_Prv_MediumCopyDo(Fee_Rb_DeviceName);

    // job exit evaluation split into subfunction due to HIS-metrics
    retVal_en = rba_FeeFs1x_PAWrite_copyFls2FlsDo_copy_exit(fsm_pst , retValInner_en , FALSE);

    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_copyFls2FlsDo_copy_exit
 * subfunction of rba_FeeFs1x_PAWrite_copyFls2FlsDo_copy
 * \param   fsm_pst: statefunction pointer to perform the exit-statetransition
 * \param   retValInner_en: return value of the copy function
 * \param   failedInternal_b: set to true if a jobFailed was set during job init state
 * \return  job result to be returned by the state
 * \retval  rba_FeeFs1x_JobOK_e,       : return job passed
 * \retval  rba_FeeFs1x_Pending_e,     : job ongoing, call the statemachine again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e: return Fs1x internal error
 * \retval  rba_FeeFs1x_ErrorExternal_e: return external error_error during read or write
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_copyFls2FlsDo_copy_exit(
        rba_FeeFs1x_PAWrite_copyFls2Fls_data_tst * fsm_pst,
        MemIf_JobResultType retValInner_en,
        boolean failedInternal_b)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch(retValInner_en)
    {
        case MEMIF_JOB_PENDING:
        {
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case MEMIF_JOB_OK:
        {
            // update the byte counters
            fsm_pst->noOfPagesDone_u32      += fsm_pst->currChunkNoOfPages_u32;
            fsm_pst->noOfPagesRemaining_u32 -= fsm_pst->currChunkNoOfPages_u32;

            if(fsm_pst->noOfPagesRemaining_u32 > 0u)
            {
                // another chunk is pending

                if(fsm_pst->currChunkIsLimitedByDest_b)
                {
                    // if the current limiter was the destination, switch the active sector
                    FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_copyFls2Fls_stm_switch_e);
                }
                else
                {
                    // if the current limiter was (only) the source, limit again
                    FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_copyFls2Fls_stm_limit_e);
                }

                retVal_en = rba_FeeFs1x_Pending_e;
            }
            else
            {
                // job is finished
                retVal_en = rba_FeeFs1x_JobOK_e;
            }

        }break;
        case MEMIF_JOB_FAILED:
        {
            if(failedInternal_b)
            {
                retVal_en = rba_FeeFs1x_ErrorInternal_e;
            }
            else
            {
                // mediumDo returned error
                retVal_en = rba_FeeFs1x_ErrorExternal_e;
            }
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_copyFls2FlsDo_switch
 * statefunction of copyFls2FlsDo
 * \return  job result
 * \retval  rba_FeeFs1x_JobOK_e,       : return job passed
 * \retval  rba_FeeFs1x_Pending_e,     : job ongoing, call the statemachine again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e: return Fs1x internal error
 * \retval  rba_FeeFs1x_ErrorExternal_e: return external error_error during read or write
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_copyFls2FlsDo_switch(rba_FeeFs1x_PAWrite_copyFls2Fls_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValInner_en;

    if(FEE_PRV_LIBENTRY)
    {
        uint32 lastPageOfNewBlock_u32;
        lastPageOfNewBlock_u32 = rba_FeeFs1x_PAMap_getLastPageNrOfChunk(
                fsm_pst->nextChunkDestPage_u32,
                0u,
                (uint16)(fsm_pst->noOfPagesRemaining_u32 * RBA_FEEFS1X_PRV_CFG_LOGL_PAGE_SIZE)
        );

        rba_FeeFs1x_PAMap_prepareSectorStates(lastPageOfNewBlock_u32 + 1u);
    }

    /* Cyclic call the Do-function */
    retValInner_en = rba_FeeFs1x_PAMap_prepareSectorStatesDo();

    switch(retValInner_en)
    {
        case rba_FeeFs1x_Pending_e:
        case rba_FeeFs1x_ErrorExternal_e:
        case rba_FeeFs1x_ErrorInternal_e:
        {
            retVal_en = retValInner_en;
        }break;
        case rba_FeeFs1x_JobOK_e:
        {
            FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_copyFls2Fls_stm_limit_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}




/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_write
 * \param   startPage_u32 : page the write request shall start programm
 * \param   nrBytes_u32   : amount of bytes to be written
 * \param   sourcebuffer_pcu8 : buffer to the RAM location the data source is stored at
 * \param   updateCurrWrPage_b : is the currWrPage to be updated during the write? For new copies, do it.
 *                                  E.g. for noFallback destroy markers, don't
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_PAWrite_write(uint32 startPage_u32, uint32 nrBytes_u32 , uint8 const * sourcebuffer_pcu8 , boolean updateCurrWrPage_b, Fee_Rb_JobMode_ten jobMode_en)
{
    uint8 writeSector_u8;
    uint8 activeSector_u8;

    (void) rba_FeeFs1x_PAMap_getLogIdxByPage(startPage_u32 , &writeSector_u8);

    activeSector_u8 = rba_FeeFs1x_Sector_getActiveSector();
    // set the state to the first one to be processed
    if(writeSector_u8 > activeSector_u8)
    {
        // if the start page of the data to be written is not in the actual sector, then first switch the sector
        rba_FeeFs1x_PAWrite_write_data_st.state_en = rba_FeeFs1x_PAWrite_write_stm_switchActiveSect_e;
    }
    else
    {
        // if the start page of the data to be written is in the actual sector start writing the data
        rba_FeeFs1x_PAWrite_write_data_st.state_en =  rba_FeeFs1x_PAWrite_write_stm_limitChunkToSect_e;
    }
    rba_FeeFs1x_PAWrite_write_data_st.entry_b  = TRUE;

    if ((FEE_RB_WRITEJOB_WRITE_ONLY_E == jobMode_en) || (FEE_RB_INVALIDATEJOB_WRITE_ONLY_E == jobMode_en))
    {
        // in the write only mode skip all steps and just write the block
        rba_FeeFs1x_PAWrite_write_data_st.writeSequence_en = FEE_PRV_MEDIUM_SEQ_WRITE_ONLY_E;
    }
    else if((FEE_RB_WRITEJOB_WRITE_VERIFY_E == jobMode_en) || (FEE_RB_INVALIDATEJOB_WRITE_VERIFY_E == jobMode_en))
    {
        // in write verify mode do not copy data to an internal buffer and skip the blank check but peform verification after write
        rba_FeeFs1x_PAWrite_write_data_st.writeSequence_en = FEE_PRV_MEDIUM_SEQ_WRITE_COMP_E;
    }
    else
    {
        // in default mode (and also any other write job mode which is not supported by Fee1x), perform all the steps
        rba_FeeFs1x_PAWrite_write_data_st.writeSequence_en = FEE_PRV_MEDIUM_SEQ_WRITE_BLNKCHK_COPY_COMP_E;
    }

    rba_FeeFs1x_PAWrite_write_data_st.startPage_u32 = startPage_u32;
    rba_FeeFs1x_PAWrite_write_data_st.nrBytesRemaining_u32 = nrBytes_u32;
    rba_FeeFs1x_PAWrite_write_data_st.nrBytesDone_u32 = 0uL;
    rba_FeeFs1x_PAWrite_write_data_st.sourcebuffer_pcu8 = sourcebuffer_pcu8;

    rba_FeeFs1x_PAWrite_write_data_st.currChunkNrBytes_u32 = 0uL;
    rba_FeeFs1x_PAWrite_write_data_st.nextChunkPage_u32 = startPage_u32;
    rba_FeeFs1x_PAWrite_write_data_st.currChunkPage_u32 = startPage_u32 - 1u;

    rba_FeeFs1x_PAWrite_write_data_st.updateCurrWrPage_b = updateCurrWrPage_b;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_writeDo
 * job Do function of rba_FeeFs1x_PAWrite_write
 * \return  job result
 * \retval rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval rba_FeeFs1x_Pending_e       : call job again during next cycle
 * \retval rba_FeeFs1x_ErrorInternal_e         : internal processing error
 * \retval rba_FeeFs1x_ErrorExternal_e : error during usage of other component (e.g. Fls)
 * \seealso
 * \usedresources
 *********************************************************************
 */
rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_writeDo(void)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    switch (rba_FeeFs1x_PAWrite_write_data_st.state_en)
    {
        case rba_FeeFs1x_PAWrite_write_stm_limitChunkToSect_e :
        {
            retVal_en = rba_FeeFs1x_PAWrite_writeDo_limitChunkToSect(&rba_FeeFs1x_PAWrite_write_data_st);
        }break;
        case rba_FeeFs1x_PAWrite_write_stm_writeChunk_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_writeDo_writeChunk(&rba_FeeFs1x_PAWrite_write_data_st);
        }break;
        case rba_FeeFs1x_PAWrite_write_stm_switchActiveSect_e:
        {
            retVal_en = rba_FeeFs1x_PAWrite_writeDo_switchActiveSect(&rba_FeeFs1x_PAWrite_write_data_st);
        }break;
        default:
        {
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    if(retVal_en != rba_FeeFs1x_Pending_e)
    {
        rba_FeeFs1x_PAWrite_write_data_st.state_en  = rba_FeeFs1x_PAWrite_write_stm_idle_e;
    }

    return  retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_writeDo_limitChunkToSect
 * statefunction of writeDo
 * \param   fsm_pst : pointer to the statemachine data
 * \return  return value to be returned by the  statemachine
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_writeDo_limitChunkToSect(rba_FeeFs1x_PAWrite_write_data_tst * fsm_pst)
{
    uint32 allowedNrBytesInChunk_u32;

    // get the byte limit of the next chunk
    allowedNrBytesInChunk_u32 = rba_FeeFs1x_PAMap_LimitRequestToSectorBoundaries(fsm_pst->nextChunkPage_u32, 0uL, fsm_pst->nrBytesRemaining_u32);

    // store the current chunk data for triggering the mediumWrite
    fsm_pst->currChunkNrBytes_u32 = allowedNrBytesInChunk_u32;
    fsm_pst->currChunkPage_u32 = fsm_pst->nextChunkPage_u32;

    // calculate the next chunk page starting allowedNrBytesInChunk_u32 bytes behind the current chunk page
    fsm_pst->nextChunkPage_u32 = 1u + rba_FeeFs1x_PAMap_getLastPageNrOfChunk
            (
                    fsm_pst->currChunkPage_u32,
                    0u,
                    (uint16)allowedNrBytesInChunk_u32
            );

    // transit into the next state
    FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_write_stm_writeChunk_e);

    return rba_FeeFs1x_Pending_e;
}

/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_writeDo_writeChunk
 * statefunction of writeDo
 * \param   fsm_pst : pointer to the statemachine data
 * \return  return value to be returned by the  statemachine
 * \retval  rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e         : internal processing error
 * \retval   rba_FeeFs1x_ErrorExternal_e: error during use of external component( Fls or CRC)
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_writeDo_writeChunk(rba_FeeFs1x_PAWrite_write_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    MemIf_JobResultType retValMedium_en;

    // state entry?
    if(FEE_PRV_LIBENTRY)
    {
        uint32 addressOfCurrPage_u32;
        // translate the current page into an address
        addressOfCurrPage_u32 = rba_FeeFs1x_PAMap_getAddressByPage(fsm_pst->currChunkPage_u32);

        /* Update the current write page if required ->
         *  On error, retry should not be done on this area, so does not matter if the write was successful or not.*/
        if(fsm_pst->updateCurrWrPage_b)
        {
            rba_FeeFs1x_PAMap_setCurrWrPage(fsm_pst->nextChunkPage_u32);
        }

        Fee_Prv_MediumWrite(Fee_Rb_DeviceName,
                addressOfCurrPage_u32,
                0u,
                (void const * )&(fsm_pst->sourcebuffer_pcu8[fsm_pst->nrBytesDone_u32]),
                fsm_pst->currChunkNrBytes_u32,
                fsm_pst->writeSequence_en);
    }

    // execute the mediumWriteDo
    retValMedium_en = Fee_Prv_MediumWriteDo(Fee_Rb_DeviceName);

    // evaluate the job result
    switch(retValMedium_en)
    {
        case MEMIF_JOB_PENDING:
        {
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        case MEMIF_JOB_OK:
        {
            // current chunk is finished, update the byte counters
            fsm_pst->nrBytesDone_u32      += fsm_pst->currChunkNrBytes_u32;
            fsm_pst->nrBytesRemaining_u32 -= fsm_pst->currChunkNrBytes_u32;

            // check whether the current chunk was the last
            if (fsm_pst->nrBytesRemaining_u32 == 0u)
            {
                retVal_en = rba_FeeFs1x_JobOK_e;
            }
            else
            {
                // sector boundary reached and more bytes are to be written
                // --> switch the active sector and process the remaining bytes
                FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_write_stm_switchActiveSect_e);
                retVal_en = rba_FeeFs1x_Pending_e;
            }
        }break;
        case MEMIF_JOB_FAILED:
        {
            // Fls didn't accept the job or returned failed
            retVal_en = rba_FeeFs1x_ErrorExternal_e;

        }break;
        default:
        {
            // on error or any unexpected return value, return error
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }
    return retVal_en;
}
/**
 *********************************************************************
 * rba_FeeFs1x_PAWrite_writeDo_switchActiveSect
 * statefunction of writeDo
 * \param   fsm_pst : pointer to the statemachine data
 * \return  return value to be returned by the  statemachine
 * \retval  rba_FeeFs1x_JobOK_e         : job finished successfully
 * \retval  rba_FeeFs1x_Pending_e       : call again during next cycle
 * \retval  rba_FeeFs1x_ErrorInternal_e         : internal processing error
 * \retval   rba_FeeFs1x_ErrorExternal_e: error during use of external component( Fls or CRC)
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_PAWrite_writeDo_switchActiveSect(rba_FeeFs1x_PAWrite_write_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;
    rba_FeeFs1x_RetVal_ten retValSwitch_en;

    // state entry?
    if(FEE_PRV_LIBENTRY)
    {
        uint32 lastPageOfBlock_u32;
        // first call of the state --> init the state's job
        // calculate the last page of the current block

        lastPageOfBlock_u32 = rba_FeeFs1x_PAMap_getLastPageNrOfChunk
                (
                        fsm_pst->nextChunkPage_u32,
                        0u ,
                        (uint16)fsm_pst->nrBytesRemaining_u32
                );

        rba_FeeFs1x_PAMap_prepareSectorStates(lastPageOfBlock_u32 + 1u);
    }

    // trigger the do-function of the job
    retValSwitch_en = rba_FeeFs1x_PAMap_prepareSectorStatesDo();

    // evaluate the job result
    switch (retValSwitch_en)
    {
        case rba_FeeFs1x_Pending_e:
            // if the job needs more cycles to complete, return pending to get called again during next cycle
        case rba_FeeFs1x_ErrorExternal_e:
            // on error, hand over the error to the caller and stop the state machine
        {
            retVal_en = retValSwitch_en;
        }break;
        case rba_FeeFs1x_JobOK_e:
        {
            // as soon as the state's job is finished, transit to next state
            FEE_PRV_LIBSC(rba_FeeFs1x_PAWrite_write_stm_limitChunkToSect_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }break;
        default:
        {
            // on error or unexpected return value, return error to the caller and stop the state machine
            retVal_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    return retVal_en;
}


#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

