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

#include "rba_FeeFs1x_Prv_BackgroundJob.h"
#include "rba_FeeFs1x_Prv_BackgroundJobTypes.h"

#include "rba_FeeFs1x_Prv_SectorEraseJob.h"
#include "rba_FeeFs1x_Prv_SectorReorg.h"
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


static rba_FeeFs1x_Background_data_tst rba_FeeFs1x_Background_data_st;


#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Background_idle(rba_FeeFs1x_Background_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Background_erase(rba_FeeFs1x_Background_data_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Background_softreorg(rba_FeeFs1x_Background_data_tst * fsm_pst);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 * rba_FeeFs1x_BackgroundJob_cancel
 * Initializes the backgroundJob unit
 * \param   void
 * \return  void
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_BackgroundJob_cancel(void)
{
    rba_FeeFs1x_Background_data_st.state_en = rba_FeeFs1x_Background_stm_idle_e;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Background
 * State machine for backgroundJob
 * \param   void
 * \return  MemIf_JobResultType
 * \retval  MEMIF_JOB_OK        : BackgroundJob successful
 * \retval  MEMIF_JOB_PENDING   : BackgroundJob is pending
 * \retval  MEMIF_JOB_FAILED    : BackgroundJob unsuccessful due to errors
 * \seealso
 * \usedresources
 *********************************************************************
 */
MemIf_JobResultType rba_FeeFs1x_Background(void)
{
    MemIf_JobResultType retVal_en;
    rba_FeeFs1x_RetVal_ten retValInner_en;

    switch(rba_FeeFs1x_Background_data_st.state_en)
    {
        case rba_FeeFs1x_Background_stm_idle_e:
        {
            // check whether a task is to be executed
            retValInner_en = rba_FeeFs1x_Background_idle(&rba_FeeFs1x_Background_data_st);
        }break;

        case rba_FeeFs1x_Background_stm_erase_e:
        {
            // erase is ongoing
            retValInner_en = rba_FeeFs1x_Background_erase(&rba_FeeFs1x_Background_data_st);
        }break;
        case rba_FeeFs1x_Background_stm_softreorg_e:
        {
            // soft reorg is ongoing
            retValInner_en = rba_FeeFs1x_Background_softreorg(&rba_FeeFs1x_Background_data_st);
        }break;
        default:
        {
            retValInner_en = rba_FeeFs1x_ErrorInternal_e;
        }break;
    }

    switch(retValInner_en)
    {
        case rba_FeeFs1x_JobOK_e:
        {
            retVal_en = MEMIF_JOB_OK;
        }break;
        case rba_FeeFs1x_Pending_e:
        {
            retVal_en = MEMIF_JOB_PENDING;
        }break;
        default:
        {
            retVal_en = MEMIF_JOB_FAILED;
        }break;
    }

    if(retVal_en != MEMIF_JOB_PENDING)
    {
        // reset the background statemachine on exit
        rba_FeeFs1x_Background_data_st.state_en = rba_FeeFs1x_Background_stm_idle_e;
    }

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Background_idle
 * state function when state is idle
 * \param   *fsm_pst : pointer to state variable struct
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_JobOK_e: no background job required
 * \retval  rba_FeeFs1x_Pending_e: erase or soft reorg in background to be executed
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Background_idle(rba_FeeFs1x_Background_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(rba_FeeFs1x_SectorEraseJob_isEraseRequired())
    {
        // erase in background to be executed
        FEE_PRV_LIBSC(rba_FeeFs1x_Background_stm_erase_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }
    else
    {
        // erase in background not required
        if(rba_FeeFs1x_Reorg_isSoftReorgRequired())
        {
            // soft reorg required
            FEE_PRV_LIBSC(rba_FeeFs1x_Background_stm_softreorg_e);
            retVal_en = rba_FeeFs1x_Pending_e;
        }
        else
        {
            // no background job required
            retVal_en = rba_FeeFs1x_JobOK_e;
        }
    }
    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Background_erase
 * state function performing the sector erase
 * \param   *fsm_pst : pointer to state variable struct
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_JobOK_e: job finished successful
 * \retval  rba_FeeFs1x_Pending_e: erase still ongoing
 * \retval  rba_FeeFs1x_JobFailed_e: an error occured
 * \retval  rba_FeeFs1x_ErrorInternal_e: something went wrong
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Background_erase(rba_FeeFs1x_Background_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        rba_FeeFs1x_SectorEraseJob_erase();
    }

    // cyclic call of Do fct
    retVal_en = rba_FeeFs1x_SectorEraseJob_eraseDo();

    return retVal_en;
}

/**
 *********************************************************************
 * rba_FeeFs1x_Background_softreorg
 * state function soft reorganisation
 * \param   *fsm_pst : pointer to state variable struct
 * \return  rba_FeeFs1x_RetVal_ten
 * \retval  rba_FeeFs1x_JobOK_e: job finished successful
 * \retval  rba_FeeFs1x_Pending_e: erase still ongoing
 * \retval  rba_FeeFs1x_JobFailed_e: restart required
 * \retval  rba_FeeFs1x_ErrorInternal_e: something went wrong
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_Background_softreorg(rba_FeeFs1x_Background_data_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        // Trigger the reorg
        rba_FeeFs1x_Reorg_reorganiseSector(TRUE);
    }

    //call the reorgDo during every statecall until done
    retVal_en = rba_FeeFs1x_Reorg_reorganiseSectorDo();

    return retVal_en;
}


#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

