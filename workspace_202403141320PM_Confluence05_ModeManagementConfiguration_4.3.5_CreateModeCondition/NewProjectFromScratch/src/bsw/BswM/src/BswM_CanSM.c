/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2016, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/*---------------------------------------------------------------------------------------*/
/*- Include files                                                                       -*/
/*---------------------------------------------------------------------------------------*/
#include "BswM.h"
#include "BswM_CanSM.h"
#include "BswM_Prv.h"

/*---------------------------------------------------------------------------------------*/
/*- Definition of Global Functions                                                      -*/
/*---------------------------------------------------------------------------------------*/

#if (defined(BSWM_CANSM_ENABLED) && (BSWM_CANSM_ENABLED == TRUE))

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"
/*****************************************************************************************
* Function name :   BswM_CanSM_CurrentState(NetworkHandleType Network, CanSM_BswMCurrentStateType CurrentState)
* Description   :   CanSM function call to indicate the current state of communication channel.
* Parameter     :   Network, CurrentState
* Return value  :   void
* Remarks       :   The corresponding configuration container for this API is BswMCanSMIndication.
*****************************************************************************************/
void BswM_CanSM_CurrentState
(
    NetworkHandleType Network,
    CanSM_BswMCurrentStateType CurrentState
)
{
#if (defined(BSWM_NO_OF_MRP_CANSM_INDICATION) && (BSWM_NO_OF_MRP_CANSM_INDICATION > 0))
    boolean flgDetChecksPassed_b = FALSE;
    uint16 idx_u16 = 0;
    BswM_Cfg_MRPCanSMIndicationType_tst dataCanSM_st;
    boolean flgNetworkIdValid = FALSE;

#if ( BSWM_DEBUG == TRUE )
    /*BswM_Prv_ctrInterrupt_u8 used for counting how many times BswM is getting interrupted*/
    BswM_Prv_ctrInterrupt_u8++;
#endif /* ( BSWM_DEBUG == TRUE ) */

    /* Is BswM initialized ? */
    if ( FALSE == BswM_Prv_isModuleInitialised_b )
    {
        /* DET if the BSW Mode Manager is not initialized.*/
        BSWM_DET_REPORT_ERROR(BSWM_API_ID_CANSM_CS, BSWM_E_NO_INIT);
    }
    else
    {
        /* Check whether passed CanSM Channel-Id is valid and get corresponding index */
        flgNetworkIdValid = BswM_Prv_GetCanSMMRPIndex_b(Network, &idx_u16);

        if(FALSE == flgNetworkIdValid)
        {
            /* DET if the Passed Channel-id is not within the valid range.*/
            BSWM_DET_REPORT_ERROR(BSWM_API_ID_CANSM_CS, BSWM_E_REQ_USER_OUT_OF_RANGE);
        }
        else if((CurrentState != CANSM_BSWM_NO_COMMUNICATION) &&
                (CurrentState != CANSM_BSWM_SILENT_COMMUNICATION) &&
                (CurrentState != CANSM_BSWM_FULL_COMMUNICATION) &&
                (CurrentState != CANSM_BSWM_BUS_OFF) &&
                (CurrentState != CANSM_BSWM_CHANGE_BAUDRATE))
        {
            /* DET if the Current State is not within the valid range.*/
            BSWM_DET_REPORT_ERROR(BSWM_API_ID_CANSM_CS, BSWM_E_REQ_MODE_OUT_OF_RANGE);
        }
        else
        {
            flgDetChecksPassed_b = TRUE;
        }

        if(FALSE != flgDetChecksPassed_b)
        {
            /* Check if no other request is in progress */
            if(FALSE != BswM_Prv_ReqProcessOrQueue_b(BSWM_MRP_CANSM_IND, Network, idx_u16, (uint16)CurrentState))
            {
                /* Process any requests that were delayed before this request */
                if (FALSE != BswM_Prv_isReqstDelayed_b)
                {
                    BswM_Prv_ProcessDelayedReqst();
                }

                /*Update RAM buffer with current mode values*/
                BswM_Cfg_CanSMIndicationModeInfo_ast[idx_u16].isValidModePresent_b = TRUE;

                BswM_Cfg_CanSMIndicationModeInfo_ast[idx_u16].dataMode_en = CurrentState;

                dataCanSM_st = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataCanSM_ast[idx_u16];

                BswM_Prv_RuleEval(dataCanSM_st.dataReqProcessing_en, BSWM_MRP_CANSM_IND, Network, dataCanSM_st.adrRulesRef_pu16, dataCanSM_st.nrAssociatedRules_u16);

                /* Check Whether deferred request processing was delayed */
                if( FALSE != BswM_Prv_flgDelayDeferredReqst_b )
                {
                    /* Call the deferred request processing function */
                    BswM_Prv_ProcessDeferredReqst();
                    /*indicate that delayed deferred requests were serviced*/
                    BswM_Prv_flgDelayDeferredReqst_b = FALSE;
                }

                /* Reset request in progress flag */
                BswM_Prv_flgNewReqstProgress_b = FALSE;

            }
       }
    }

#if ( BSWM_DEBUG == TRUE )
    BswM_Prv_InterruptHandler(BSWM_API_ID_CANSM_CS);
#endif /* ( BSWM_DEBUG == TRUE ) */

#else
    /*To avoid compiler warning of unused parameters*/
    (void)Network;
    (void)CurrentState;
#endif /* (BSWM_NO_OF_MRP_CANSM_INDICATION > 0) */
    return;
}

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif

/**********************************************************************************************************************
 * End of source file
 **********************************************************************************************************************/

