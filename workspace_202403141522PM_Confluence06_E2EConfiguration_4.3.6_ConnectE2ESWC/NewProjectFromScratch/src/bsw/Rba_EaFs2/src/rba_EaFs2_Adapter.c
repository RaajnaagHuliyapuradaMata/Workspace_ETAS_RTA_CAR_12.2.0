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

#include "Ea.h"

#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))

#include "Ea_Prv.h"
#include "Ea_Cfg_SchM.h"

#if ((defined(EA_RB_ENTER_STOP_MODE) && (EA_RB_ENTER_STOP_MODE == TRUE)))

#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

rba_EaFs2_Prv_StopMode_ten  rba_EaFs2_Prv_StopMode_en;

#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"


#define EA_START_SEC_CODE
#include "Ea_MemMap.h"
/**
 *********************************************************************
 * Ea_Rb_EnterStopMode():
 *
 * This function shall be called by rba_FeeAddOnAS to stop the
 * Ea User request and Internal operation.
 *
 * \param    None
 * \return   Std_ReturnType   E_NOT_OK : Request Failed
 *                            E_OK : Request Success
 * \retval
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType Ea_Rb_EnterStopMode(void)
{
    Std_ReturnType result_en = E_NOT_OK;

    if(E_OK == Ea_Rb_CheckInit(EA_SID_RB_ENTERSTOPMODE))
    {

        if(RBA_EAFS2_PRV_STOPMODE_NOTREQUESTED == rba_EaFs2_Prv_StopMode_en)
        {
            /* Disable Interrupt */
            SchM_Enter_Ea_Order();

            if(EA_RB_NO_ORDER_E == Ea_Prv_Order_st.OrderType_en)
            {
                Ea_Prv_Order_st.OrderType_en = EA_PRV_STOP_MODE_E;
            }
            rba_EaFs2_Prv_StopMode_en = RBA_EAFS2_PRV_STOPMODE_INPROCESS;

            /* Enable Interrupt */
            SchM_Exit_Ea_Order();
        }

        result_en = E_OK;
    }
    return(result_en);
}

/**
 *********************************************************************
 * Ea_Rb_GetAdapterJobResult : Function call Inidactes that ReadAll is completed
 *
 * Functions which initiates the background operations in a power cycle
 *
 * \param    (void)
 * Return value:
 * MemIf_JobResultType - MEMIF_JOB_OK:      The last job has been finished successfully.
 *                       MEMIF_JOB_PENDING: The last job is waiting for execution or
 *                                          currently being executed.
 *                       MEMIF_JOB_FAILED:  The last job has not been finished
 *                                          successfully (it failed).
 *
 *********************************************************************
 */
MemIf_JobResultType Ea_Rb_GetAdapterJobResult(void)
{
    MemIf_JobResultType xRetVal = MEMIF_JOB_OK;

        /* [EA134]:
         * If development error detection for the module Ea is enabled: the function Ea_GetJobResult shall check that
         * the module has been initialized. If this check fails, the function Ea_GetJobResult shall raise the development
         * error EA_E_UNINIT and return MEMIF_JOB_FAILED to the caller without performing the requested function.
         * */
        if(Ea_Rb_CheckInit(EA_SID_RB_GETADAPTERJOBRESULT) != E_OK)
        {
            xRetVal = MEMIF_JOB_FAILED;
        }
        else
        {
            if(RBA_EAFS2_PRV_STOPMODE_INPROCESS == rba_EaFs2_Prv_StopMode_en)
            {
                xRetVal = MEMIF_JOB_PENDING;
            }

        }

    return (xRetVal);
}


/**
 *********************************************************************
 * rba_EaFs2_Prv_StopModeActivate :
 *
 * Function Activates the Stop Mode Feature.
 *
 * \param    (void)
 * \return   (void)
 *********************************************************************
 */

void rba_EaFs2_Prv_StopModeActivate(void)
{
    /* Disabling Interrupt not necessary.Analysis done */
    rba_EaFs2_Prv_StopMode_en = RBA_EAFS2_PRV_STOPMODE_ACTIVATED;
#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG == FALSE)
    if(EA_RB_MIGRATION_FORCEDRUNNING == Ea_Rb_MigrationStaus_u8)
    {
        rba_EaFs2_Prv_UnallocBlkSpaceCheck();
    }
#endif
}


#if(FALSE != EA_PRV_DEBUGGING)
/**
 *********************************************************************
 * rba_EaFs2_Prv_Initialize_Adapter():
 *
 * This function used as initialzation for global variable
 *
 * \param    None
 *
 * \retval
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_EaFs2_Prv_Initialize_Adapter(void)
{
    rba_EaFs2_Prv_StopMode_en = RBA_EAFS2_PRV_STOPMODE_NOTREQUESTED;
}
#endif

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"
#endif /* (EA_RB_ENTER_STOP_MODE != FALSE) */
#endif /* (EA_RB_CFG_SELECTED_FS == 2) */
