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


#include "rba_FeeFs1x_Prv_TriggerReorgJob.h"
#include "rba_FeeFs1x_Prv_TriggerReorgJobTypes.h"


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


static rba_FeeFs1x_TriggerReorg_tst rba_FeeFs1x_TriggerReorg_st;


#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"
/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
 */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_TriggerReorgDo_reorg(rba_FeeFs1x_TriggerReorg_tst * fsm_pst);
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_TriggerReorgDo_erase(rba_FeeFs1x_TriggerReorg_tst * fsm_pst);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 *********************************************************************
 * rba_FeeFs1x_TriggerReorgJob_cancel
 * job init function for trigggering a force reorganisation of logical sector 0, to initialise all variables and state machines
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_TriggerReorgJob_cancel(void)
{
    if(rba_FeeFs1x_Prv_isTriggerReorgActive())
    {
        rba_FeeFs1x_TriggerReorg_st.state_en = rba_FeeFs1x_TriggerReorg_stm_idle_e;
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_TriggerReorg
 * job init function for trigggering a force reorganisation of logical sector 0
 * \return  none
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_FeeFs1x_TriggerReorg(void)
{
    if(rba_FeeFs1x_Prv_isTriggerReorgActive())
    {
        rba_FeeFs1x_TriggerReorg_st.state_en = rba_FeeFs1x_TriggerReorg_stm_reorg_e;
        rba_FeeFs1x_TriggerReorg_st.entry_b  = TRUE;
    }
}

/**
 *********************************************************************
 * rba_FeeFs1x_TriggerReorgDo
 * jobDo function for trigggering a force reorganisation of logical sector 0
 * \return  is the job accepted?
 * \retval  MEMIF_JOB_PENDING    : job ongoing, call the function again
 * \retval  MEMIF_JOB_OK         : trigger sector reorg job was successfully completed
 * \retval  MEMIF_JOB_FAILED     : sector reorg job failed
 * \seealso
 * \usedresources
 *********************************************************************
 */
MemIf_JobResultType rba_FeeFs1x_TriggerReorgDo(void)
{
    MemIf_JobResultType retVal_en;
    rba_FeeFs1x_RetVal_ten innerRetVal_en;

    if(rba_FeeFs1x_Prv_isTriggerReorgActive())
    {
        switch(rba_FeeFs1x_TriggerReorg_st.state_en)
        {
            case rba_FeeFs1x_TriggerReorg_stm_reorg_e:
            {
                innerRetVal_en = rba_FeeFs1x_TriggerReorgDo_reorg(&rba_FeeFs1x_TriggerReorg_st);
            }break;

            case rba_FeeFs1x_TriggerReorg_stm_erase_e:
            {
                innerRetVal_en = rba_FeeFs1x_TriggerReorgDo_erase(&rba_FeeFs1x_TriggerReorg_st);
            }break;

            default:
            {
                innerRetVal_en = rba_FeeFs1x_ErrorInternal_e;
            }break;
        }

        switch(innerRetVal_en)
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
            rba_FeeFs1x_TriggerReorg_st.state_en = rba_FeeFs1x_TriggerReorg_stm_idle_e;
        }
    }
    else
    {
        retVal_en = MEMIF_JOB_FAILED;
    }

    return (retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_TriggerReorgDo_reorg
 * state function of trigger reorg job. the state triggers forced reorganisation of logical sector 0. This reorg operation
 * is not interruptable by new API request i.e., MEMIF_JOB_PENDING is returned till the reorganisation is completed.
 *
 * \return  is the job accepted?
 * \retval  rba_FeeFs1x_JobOK_e: job finished successful
 * \retval  rba_FeeFs1x_JobFailed_e: trigger sector reorg job failed
 * \retval  rba_FeeFs1x_Pending_e: job ongoing, call the function again
 * \retval  rba_FeeFs1x_ErrorInternal_e: sector reorg job failed, internal error
 * \retval  rba_FeeFs1x_ErrorExternal_e: sector reorg job failed, external error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_TriggerReorgDo_reorg(rba_FeeFs1x_TriggerReorg_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retValReorg_en;
    rba_FeeFs1x_RetVal_ten retVal_en;

    if(FEE_PRV_LIBENTRY)
    {
        // Trigger the reorg
        rba_FeeFs1x_Reorg_reorganiseSector(FALSE);
    }

    //call the reorgDo during every statecall until done
    retValReorg_en = rba_FeeFs1x_Reorg_reorganiseSectorDo();

    // map the reorg result to memif
    if(rba_FeeFs1x_JobOK_e == retValReorg_en)
    {
        // switch to next state to erase the sector, since the job is not yet over, return pending
        FEE_PRV_LIBSC(rba_FeeFs1x_TriggerReorg_stm_erase_e);
        retVal_en = rba_FeeFs1x_Pending_e;
    }
    else
    {
        retVal_en = retValReorg_en;
    }

    return(retVal_en);
}

/**
 *********************************************************************
 * rba_FeeFs1x_TriggerReorgDo_erase
 * state function of trigger reorg job. the state triggers sector erase for just completed sector reorg.
 *
 * \return  is the job accepted?
 * \retval  rba_FeeFs1x_JobOK_e: job finished successful
 * \retval  rba_FeeFs1x_JobFailed_e: trigger sector reorg job failed
 * \retval  rba_FeeFs1x_Pending_e: job ongoing, call the function again
 * \retval  rba_FeeFs1x_ErrorInternal_e: sector reorg job failed, internal error
 * \retval  rba_FeeFs1x_ErrorExternal_e: sector reorg job failed, external error
 * \seealso
 * \usedresources
 *********************************************************************
 */
LOCAL_INLINE rba_FeeFs1x_RetVal_ten rba_FeeFs1x_TriggerReorgDo_erase(rba_FeeFs1x_TriggerReorg_tst * fsm_pst)
{
    rba_FeeFs1x_RetVal_ten retValReorg_en;

    if(FEE_PRV_LIBENTRY)
    {
        // Trigger sector erase
        rba_FeeFs1x_SectorEraseJob_erase();
    }

    //call the eraseDo during every statecall until done
    retValReorg_en = rba_FeeFs1x_SectorEraseJob_eraseDo();

    if(rba_FeeFs1x_JobOK_e == retValReorg_en)
    {
        // switch state to idle
        FEE_PRV_LIBSC(rba_FeeFs1x_TriggerReorg_stm_idle_e);
    }

    return(retValReorg_en);
}

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

