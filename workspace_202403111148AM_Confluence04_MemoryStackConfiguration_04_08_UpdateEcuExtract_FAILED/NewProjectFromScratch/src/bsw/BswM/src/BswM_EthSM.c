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
#include "BswM_EthSM.h"
#include "BswM_Prv.h"

/*---------------------------------------------------------------------------------------*/
/*- Definition of Global Functions                                                      -*/
/*---------------------------------------------------------------------------------------*/

#if (defined(BSWM_ETHSM_ENABLED) && (BSWM_ETHSM_ENABLED == TRUE))

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

/*****************************************************************************************
* Function name :   BswM_EthSM_CurrentState(NetworkHandleType Network, EthSM_NetworkModeStateType CurrentState)
* Description   :   Function called by EthSM to indicate its current state.
* Parameter     :   Network, CurrentState
* Return value  :   void
* Remarks       :   The corresponding configuration container for this API is BswMEthSMIndication.
*****************************************************************************************/
void BswM_EthSM_CurrentState
(
    NetworkHandleType Network,
    EthSM_NetworkModeStateType CurrentState
)
{
#if (defined(BSWM_NO_OF_MRP_ETHSM_INDICATION) && (BSWM_NO_OF_MRP_ETHSM_INDICATION > 0))
    boolean flgDetChecksPassed_b = FALSE;
    uint16 idx_u16 = 0;
    BswM_Cfg_MRPEthSMType_tst dataEthSM_st;
    boolean flgNetworkIdValid = FALSE;

#if ( BSWM_DEBUG == TRUE )
    /*BswM_Prv_ctrInterrupt_u8 used for counting how many times BswM is getting interrupted*/
    BswM_Prv_ctrInterrupt_u8++;
#endif /* ( BSWM_DEBUG == TRUE ) */

    /* Is BswM initialized ? */
    if ( FALSE == BswM_Prv_isModuleInitialised_b )
    {
        /* DET if the BSW Mode Manager is not initialized.*/
        BSWM_DET_REPORT_ERROR(BSWM_API_ID_ETHSM_CS, BSWM_E_NO_INIT);
    }
    else
    {
        /* Check whether passed CanSM Channel-Id is valid and get corresponding index */
        flgNetworkIdValid = BswM_Prv_GetEthSMMRPIndex_b(Network, &idx_u16);

        if(FALSE == flgNetworkIdValid)
        {
            /* DET if the Passed Channel-id is not within the valid range.*/
            BSWM_DET_REPORT_ERROR(BSWM_API_ID_ETHSM_CS, BSWM_E_REQ_USER_OUT_OF_RANGE);
        }
        else if((CurrentState != ETHSM_STATE_OFFLINE) &&
                (CurrentState != ETHSM_STATE_WAIT_TRCVLINK) &&
                (CurrentState != ETHSM_STATE_WAIT_ONLINE) &&
                (CurrentState != ETHSM_STATE_ONLINE) &&
                (CurrentState != ETHSM_STATE_ONHOLD) &&
                (CurrentState != ETHSM_STATE_WAIT_OFFLINE))
        {
            /* DET if the Current State is not within the valid range.*/
            BSWM_DET_REPORT_ERROR(BSWM_API_ID_ETHSM_CS, BSWM_E_REQ_MODE_OUT_OF_RANGE);
        }
        else
        {
            flgDetChecksPassed_b = TRUE;
        }

        if(FALSE != flgDetChecksPassed_b)
        {
            /* Check if no other request is in progress */
            if(FALSE != BswM_Prv_ReqProcessOrQueue_b(BSWM_MRP_ETHSM_IND, Network, idx_u16, (uint16)CurrentState))
            {
                /* Process any requests that were delayed before this request */
                if (FALSE != BswM_Prv_isReqstDelayed_b)
                {
                    BswM_Prv_ProcessDelayedReqst();
                }

                /*Update RAM buffer with current mode values*/
                BswM_Cfg_EthSMIndicationModeInfo_ast[idx_u16].isValidModePresent_b = TRUE;

                BswM_Cfg_EthSMIndicationModeInfo_ast[idx_u16].dataMode_en = CurrentState;

                dataEthSM_st = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEthSM_ast[idx_u16];

                BswM_Prv_RuleEval(dataEthSM_st.dataReqProcessing_en, BSWM_MRP_ETHSM_IND, Network, dataEthSM_st.adrRulesRef_pu16, dataEthSM_st.nrAssociatedRules_u16);

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
    BswM_Prv_InterruptHandler(BSWM_API_ID_ETHSM_CS);
#endif /* ( BSWM_DEBUG == TRUE ) */

#else
    /*To avoid compiler warning of unused parameters*/
    (void)Network;
    (void)CurrentState;
#endif  /* BSWM_NO_OF_MRP_ETHSM_INDICATION */
    return;
}

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif
/**********************************************************************************************************************
 * End of source file
 **********************************************************************************************************************/
