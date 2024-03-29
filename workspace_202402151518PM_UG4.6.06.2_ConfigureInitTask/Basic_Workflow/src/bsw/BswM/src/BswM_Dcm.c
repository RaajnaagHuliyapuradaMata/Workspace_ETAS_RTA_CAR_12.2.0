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
#include "BswM_Dcm.h"
#include "BswM_Prv.h"

/*---------------------------------------------------------------------------------------*/
/*- Definition of Global Functions                                                      -*/
/*---------------------------------------------------------------------------------------*/

#if (defined(BSWM_DCM_ENABLED) && (BSWM_DCM_ENABLED == TRUE))

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"
/*****************************************************************************************
* Function name :   BswM_Dcm_ApplicationUpdated(void)
* Description   :   This function is called by the DCM in order to report an updated application.
* Parameter     :   void
* Return value  :   void
* Remarks       :   The corresponding configuration container for this API is BswMDcmApplicationUpdatedIndication.
*****************************************************************************************/
void BswM_Dcm_ApplicationUpdated
(
    void
)
{
#if (defined(BSWM_NO_OF_ERP_DCM_APP_UPDATE_INDICATION) && (BSWM_NO_OF_ERP_DCM_APP_UPDATE_INDICATION > 0))
    uint16 idx_u16 = BSWM_ERP_WITH_NO_CHANNEL_SOURCE;
    BswM_Cfg_ERPDcmAppIndicateType_tst dataDcmAppUpdate_st;

#if ( BSWM_DEBUG == TRUE )
    /*BswM_Prv_ctrInterrupt_u8 used for counting how many times BswM is getting interrupted*/
    BswM_Prv_ctrInterrupt_u8++;
#endif /* ( BSWM_DEBUG == TRUE ) */

    /* Is BswM initialized ? */
    if ( FALSE == BswM_Prv_isModuleInitialised_b )
    {
        /* DET if the BSW Mode Manager is not initialized.*/
        BSWM_DET_REPORT_ERROR(BSWM_API_ID_DCM_APP_UPDATE, BSWM_E_NO_INIT);
    }
    else
    {
        /* Check if no other request is in progress */
        if(FALSE != BswM_Prv_ReqProcessOrQueue_b(BSWM_ERP_DCM_APPL_UPDATE_IND, BSWM_ERP_WITH_NO_CHANNEL_SOURCE, idx_u16, (uint16)BSWM_NO_SOURCE_STATUS_TRUE))
        {
            /* Process any requests that were delayed before this request */
            if (FALSE != BswM_Prv_isReqstDelayed_b)
            {
                BswM_Prv_ProcessDelayedReqst();
            }

            /*Update RAM buffer with current mode values*/
            BswM_Cfg_DcmApplicationUpdatedindicationModeInfo_ast[idx_u16].isValidModePresent_b = TRUE;

            BswM_Cfg_DcmApplicationUpdatedindicationModeInfo_ast[idx_u16].dataMode_en = BSWM_EVENT_IS_SET;

            dataDcmAppUpdate_st = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataDcmAppUpdateInd_st[idx_u16];

            BswM_Prv_RuleEval(dataDcmAppUpdate_st.dataReqProcessing_en, BSWM_ERP_DCM_APPL_UPDATE_IND, BSWM_ERP_WITH_NO_CHANNEL_SOURCE, dataDcmAppUpdate_st.adrRulesRef_pu16, dataDcmAppUpdate_st.nrAssociatedRules_u16);

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
    BswM_Prv_InterruptHandler(BSWM_API_ID_DCM_APP_UPDATE);
#endif /* ( BSWM_DEBUG == TRUE ) */

#endif /* BSWM_NO_OF_MRP_DCM_APP_UPDATE_INDICATION */
    return;
}


/*****************************************************************************************
* Function name :   BswM_Dcm_CommunicationMode_CurrentState(NetworkHandleType Network, Dcm_CommunicationModeType RequestedMode)
* Description   :   Function called by DCM to inform the BswM about the current state of the communication mode.
* Parameter     :   Network, RequestedMode
* Return value  :   void
* Remarks       :   The corresponding configuration container for this API is BswMDcmComModeRequest.
*****************************************************************************************/
void BswM_Dcm_CommunicationMode_CurrentState
(
    NetworkHandleType Network,
    Dcm_CommunicationModeType RequestedMode
)
{
#if (defined(BSWM_NO_OF_MRP_DCM_COM_MODE_REQ) && (BSWM_NO_OF_MRP_DCM_COM_MODE_REQ > 0))
    boolean flgDetChecksPassed_b = FALSE;
    uint16 idx_u16 = 0;
    BswM_Cfg_MRPDcmComType_tst dataDcmCom_st;
    boolean flgDcmComModeValid = FALSE;

#if ( BSWM_DEBUG == TRUE )
    /*BswM_Prv_ctrInterrupt_u8 used for counting how many times BswM is getting interrupted*/
    BswM_Prv_ctrInterrupt_u8++;
#endif /* ( BSWM_DEBUG == TRUE ) */

    /* Is BswM initialized ? */
    if ( FALSE == BswM_Prv_isModuleInitialised_b )
    {
        /* DET if the BSW Mode Manager is not initialized.*/
        BSWM_DET_REPORT_ERROR(BSWM_API_ID_DCM_COMMODE_CS, BSWM_E_NO_INIT);
    }
    else
    {
        /* Check whether passed Network is valid and get corresponding index */
        flgDcmComModeValid = BswM_Prv_GetDcmComModeRequestMRPIndex_b(Network, &idx_u16);

        if(FALSE == flgDcmComModeValid)
        {
            /* DET if the Passed Channel-id is not within the valid range.*/
            BSWM_DET_REPORT_ERROR(BSWM_API_ID_DCM_COMMODE_CS, BSWM_E_REQ_USER_OUT_OF_RANGE);
        }
        else if((RequestedMode != DCM_ENABLE_RX_TX_NORM) &&
                (RequestedMode != DCM_ENABLE_RX_DISABLE_TX_NORM) &&
                (RequestedMode != DCM_DISABLE_RX_ENABLE_TX_NORM) &&
                (RequestedMode != DCM_DISABLE_RX_TX_NORMAL) &&
                (RequestedMode != DCM_ENABLE_RX_TX_NM) &&
                (RequestedMode != DCM_ENABLE_RX_DISABLE_TX_NM) &&
                (RequestedMode != DCM_DISABLE_RX_ENABLE_TX_NM) &&
                (RequestedMode != DCM_DISABLE_RX_TX_NM) &&
                (RequestedMode != DCM_ENABLE_RX_TX_NORM_NM) &&
                (RequestedMode != DCM_ENABLE_RX_DISABLE_TX_NORM_NM) &&
                (RequestedMode != DCM_DISABLE_RX_ENABLE_TX_NORM_NM) &&
                (RequestedMode != DCM_DISABLE_RX_TX_NORM_NM))
        {
            /* DET if the Requested Mode is not within the valid range.*/
            BSWM_DET_REPORT_ERROR(BSWM_API_ID_DCM_COMMODE_CS, BSWM_E_REQ_MODE_OUT_OF_RANGE);
        }
        else
        {
            flgDetChecksPassed_b = TRUE;
        }

        if(FALSE != flgDetChecksPassed_b)
        {
            /* Check if no other request is in progress */
            if(FALSE != BswM_Prv_ReqProcessOrQueue_b(BSWM_MRP_DCM_COM_MODE_REQST, Network, idx_u16, (uint16)RequestedMode))
            {
                /* Process any requests that were delayed before this request */
                if (FALSE != BswM_Prv_isReqstDelayed_b)
                {
                    BswM_Prv_ProcessDelayedReqst();
                }

                /*Update RAM buffer with current mode values*/
                BswM_Cfg_DcmComModeRequestModeInfo_ast[idx_u16].isValidModePresent_b = TRUE;

                BswM_Cfg_DcmComModeRequestModeInfo_ast[idx_u16].dataMode_u8 = RequestedMode;

                dataDcmCom_st = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataDcmCom_ast[idx_u16];

                BswM_Prv_RuleEval(dataDcmCom_st.dataReqProcessing_en, BSWM_MRP_DCM_COM_MODE_REQST, Network, dataDcmCom_st.adrRulesRef_pu16, dataDcmCom_st.nrAssociatedRules_u16);

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
    BswM_Prv_InterruptHandler(BSWM_API_ID_DCM_COMMODE_CS);
#endif /* ( BSWM_DEBUG == TRUE ) */

#else
    /*To avoid compiler warning of unused parameters*/
    (void)Network;
    (void)RequestedMode;
    return;
#endif /* BSWM_NO_OF_MRP_DCM_COM_MODE_REQ */
}

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif
/**********************************************************************************************************************
 * End of source file
 **********************************************************************************************************************/

