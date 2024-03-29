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
#include "BswM_Prv.h"
#include "BswM_ComM.h"


/*---------------------------------------------------------------------------------------*/
/*- Definition of Global Functions                                                      -*/
/*---------------------------------------------------------------------------------------*/


#if (defined(BSWM_COMM_ENABLED) && (BSWM_COMM_ENABLED == TRUE))

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

/*****************************************************************************************
* Function name :   BswM_ComM_CurrentMode(NetworkHandleType Network, ComM_ModeType RequestedMode)
* Description   :   ComM function call to indicate the current state of communication channel.
* Parameter     :   Network, RequestedMode
* Return value  :   void
* Remarks       :   The corresponding configuration container for this API is BswMComMIndication.
*****************************************************************************************/
void BswM_ComM_CurrentMode
(
    NetworkHandleType Network,
    ComM_ModeType RequestedMode
)
{
#if (defined(BSWM_NO_OF_MRP_COMM_INDICATION) && (BSWM_NO_OF_MRP_COMM_INDICATION > 0))
    boolean flgDetChecksPassed_b = FALSE;
    uint16 idx_u16 = 0;
    BswM_Cfg_MRPComMIndicationType_tst dataComM_st;
    boolean flgNetworkIdValid = FALSE;

#if ( BSWM_DEBUG == TRUE )
    /*BswM_Prv_ctrInterrupt_u8 used for counting how many times BswM is getting interrupted*/
    BswM_Prv_ctrInterrupt_u8++;
#endif /* ( BSWM_DEBUG == TRUE ) */

    /* Is BswM initialized ? */
    if ( FALSE == BswM_Prv_isModuleInitialised_b )
    {
        /* DET if the BSW Mode Manager is not initialized.*/
        BSWM_DET_REPORT_ERROR(BSWM_API_ID_COMM_CM, BSWM_E_NO_INIT);
    }
    else
    {
        /* Check whether passed ComM Channel-Id is valid and get corresponding index */
        flgNetworkIdValid = BswM_Prv_GetComMMRPIndex_b(Network, &idx_u16);

        if(FALSE == flgNetworkIdValid)
        {
            /* DET if the Passed Channel-id is not within the valid range.*/
            BSWM_DET_REPORT_ERROR(BSWM_API_ID_COMM_CM, BSWM_E_REQ_USER_OUT_OF_RANGE);
        }
        else if((RequestedMode != COMM_NO_COMMUNICATION) &&
                (RequestedMode != COMM_SILENT_COMMUNICATION) &&
                (RequestedMode != COMM_FULL_COMMUNICATION))
        {
            /* DET if the Current State is not within the valid range.*/
            BSWM_DET_REPORT_ERROR(BSWM_API_ID_COMM_CM, BSWM_E_REQ_MODE_OUT_OF_RANGE);
        }
        else
        {
            flgDetChecksPassed_b = TRUE;
        }

        if(FALSE != flgDetChecksPassed_b)
        {
            /* Check if no other request is in progress */
            if(FALSE != BswM_Prv_ReqProcessOrQueue_b(BSWM_MRP_COMM_IND, Network, idx_u16, (uint16)RequestedMode))
            {
                /* Process any requests that were delayed before this request */
                if (FALSE != BswM_Prv_isReqstDelayed_b)
                {
                    BswM_Prv_ProcessDelayedReqst();
                }

                /*Update RAM buffer with current mode values*/
                BswM_Cfg_ComMIndicationModeInfo_ast[idx_u16].isValidModePresent_b = TRUE;

                BswM_Cfg_ComMIndicationModeInfo_ast[idx_u16].dataMode_u8 = RequestedMode;

                dataComM_st = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComM_ast[idx_u16];

                BswM_Prv_RuleEval(dataComM_st.dataReqProcessing_en, BSWM_MRP_COMM_IND, Network, dataComM_st.adrRulesRef_pu16, dataComM_st.nrAssociatedRules_u16);

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
    BswM_Prv_InterruptHandler(BSWM_API_ID_COMM_CM);
#endif /* ( BSWM_DEBUG == TRUE ) */

#else
    /*To avoid compiler warning of unused parameters*/
    (void)Network;
    (void)RequestedMode;
#endif
    return;
}


/*****************************************************************************************
* Function name :   BswM_ComM_CurrentPNCMode(PNCHandleType PNC, ComM_PncModeType CurrentPncMode)
* Description   :   ComM Pnc function call to indicate the current state of Partial Network Cluster channel.
* Parameter     :   PNC, CurrentPncMode
* Return value  :   void
* Remarks       :   The corresponding configuration container for this API is BswMComMPncRequest.
*****************************************************************************************/
void BswM_ComM_CurrentPNCMode
(
    PNCHandleType PNC,
    ComM_PncModeType CurrentPncMode
)
{
#if (defined(BSWM_NO_OF_MRP_COMMPNCS_REQUEST) && (BSWM_NO_OF_MRP_COMMPNCS_REQUEST > 0))
    boolean flgDetChecksPassed_b = FALSE;
    uint16 idx_u16 = 0;
    BswM_Cfg_MRPComMPncRequestType_tst dataComMPnc_st;
    boolean flgPNCValid = FALSE;

#if ( BSWM_DEBUG == TRUE )
    /*BswM_Prv_ctrInterrupt_u8 used for counting how many times BswM is getting interrupted*/
    BswM_Prv_ctrInterrupt_u8++;
#endif /* ( BSWM_DEBUG == TRUE ) */

    /* Is BswM initialized ? */
    if ( FALSE == BswM_Prv_isModuleInitialised_b )
    {
        /* DET if the BSW Mode Manager is not initialized.*/
        BSWM_DET_REPORT_ERROR(BSWM_API_ID_CUR_PNC_MODE, BSWM_E_NO_INIT);
    }
    else
    {
        /* Check whether passed PNC is valid and get corresponding index */
        flgPNCValid = BswM_Prv_GetCurrentPncMRPIndex_b(PNC, &idx_u16);

         if((CurrentPncMode != COMM_PNC_REQUESTED) &&
                (CurrentPncMode != COMM_PNC_READY_SLEEP) &&
                (CurrentPncMode != COMM_PNC_PREPARE_SLEEP) &&
                (CurrentPncMode != COMM_PNC_NO_COMMUNICATION))
        {
            /* DET if the Current State is not within the valid range.*/
            BSWM_DET_REPORT_ERROR(BSWM_API_ID_CUR_PNC_MODE, BSWM_E_REQ_MODE_OUT_OF_RANGE);
        }
        else if(FALSE != flgPNCValid)
        {
            flgDetChecksPassed_b = TRUE;
        }
        else
        {
            //do nothing
        }

        if(FALSE != flgDetChecksPassed_b)
        {
            /* Check if no other request is in progress */
            if(FALSE != BswM_Prv_ReqProcessOrQueue_b(BSWM_MRP_COMM_PNC_REQST, PNC, idx_u16, (uint16)CurrentPncMode))
            {
                /* Process any requests that were delayed before this request */
                if (FALSE != BswM_Prv_isReqstDelayed_b)
                {
                    BswM_Prv_ProcessDelayedReqst();
                }

                /*Update RAM buffer with current mode values*/
                BswM_Cfg_ComMPncRequestModeInfo_ast[idx_u16].isValidModePresent_b = TRUE;

                BswM_Cfg_ComMPncRequestModeInfo_ast[idx_u16].dataMode_en = CurrentPncMode;

                dataComMPnc_st = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComMPnc_ast[idx_u16];

                BswM_Prv_RuleEval(dataComMPnc_st.dataReqProcessing_en, BSWM_MRP_COMM_PNC_REQST, PNC, dataComMPnc_st.adrRulesRef_pu16, dataComMPnc_st.nrAssociatedRules_u16);

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
    BswM_Prv_InterruptHandler(BSWM_API_ID_CUR_PNC_MODE);
#endif /* ( BSWM_DEBUG == TRUE ) */

#else
    /*To avoid compiler warning of unused parameters*/
    (void)PNC;
    (void)CurrentPncMode;
#endif
    return;
}


/*****************************************************************************************
* Function name :   BswM_ComM_InitiateReset(void)
* Description   :   ComM Initiate Reset function call initiates the reset.
* Parameter     :   void
* Return value  :   void
* Remarks       :   The corresponding configuration container for this API is BswMComMInitiateReset.
*****************************************************************************************/
void BswM_ComM_InitiateReset
(
    void
)
{
#if (defined(BSWM_NO_OF_ERP_COMM_INITIATE_RESET) && (BSWM_NO_OF_ERP_COMM_INITIATE_RESET > 0))
    uint16 idx_u16 = BSWM_ERP_WITH_NO_CHANNEL_SOURCE;
    BswM_Cfg_ERPComMInitiateResetType_tst dataComMInitReset_st;

#if ( BSWM_DEBUG == TRUE )
    /*BswM_Prv_ctrInterrupt_u8 used for counting how many times BswM is getting interrupted*/
    BswM_Prv_ctrInterrupt_u8++;
#endif /* ( BSWM_DEBUG == TRUE ) */

    /* Is BswM initialized ? */
    if ( FALSE == BswM_Prv_isModuleInitialised_b )
    {
        /* DET if the BSW Mode Manager is not initialized.*/
        BSWM_DET_REPORT_ERROR(BSWM_API_ID_COMM_INIT_RST, BSWM_E_NO_INIT);
    }
    else
    {
        /* Check if no other request is in progress */
        if(FALSE != BswM_Prv_ReqProcessOrQueue_b(BSWM_ERP_COMM_INITIATE_RESET, BSWM_ERP_WITH_NO_CHANNEL_SOURCE, idx_u16, (uint16)BSWM_NO_SOURCE_STATUS_TRUE))
        {
            /* Process any requests that were delayed before this request */
            if (FALSE != BswM_Prv_isReqstDelayed_b)
            {
                BswM_Prv_ProcessDelayedReqst();
            }

            /*Update RAM buffer with current mode values*/
            BswM_Cfg_ComMInitiateResetModeInfo_ast[idx_u16].isValidModePresent_b = TRUE;

            BswM_Cfg_ComMInitiateResetModeInfo_ast[idx_u16].dataMode_en = BSWM_EVENT_IS_SET;

            dataComMInitReset_st = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataComMReset_st[idx_u16];

            BswM_Prv_RuleEval(dataComMInitReset_st.dataReqProcessing_en, BSWM_ERP_COMM_INITIATE_RESET, BSWM_ERP_WITH_NO_CHANNEL_SOURCE, dataComMInitReset_st.adrRulesRef_pu16, dataComMInitReset_st.nrAssociatedRules_u16);

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

#if ( BSWM_DEBUG == TRUE )
    BswM_Prv_InterruptHandler(BSWM_API_ID_COMM_INIT_RST);
#endif /* ( BSWM_DEBUG == TRUE ) */

#endif
    return;
}


#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif

/**********************************************************************************************************************
 * End of File
 **********************************************************************************************************************/

