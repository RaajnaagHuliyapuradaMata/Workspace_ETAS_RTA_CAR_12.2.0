/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
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
/* Function definition and SoAd related headers available only if SoAd is enabled in the project  */
#if (defined(BSWM_RB_SOAD_ENABLED) && (BSWM_RB_SOAD_ENABLED == TRUE))
#include "BswM_SoAd.h"
#include "SoAd.h"

/*---------------------------------------------------------------------------------------*/
/*- Definition of Global Functions                                                       -*/
/*---------------------------------------------------------------------------------------*/


#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

/*****************************************************************************************
* Function name :BswM_SoAd_SoConModeChg(SoAd_SoConIdType SoConId, SoAd_SoConModeType State)
* Description   :Function called by SoAd(Socket Adaptor) to notify state change of a socket connection.
* Parameter     :SoConId: The socket connection index.
*                State  : State The state of the SoAd socket connection.
* Return value  :None
* Remarks       :The corresponding configuration container for this API is BswMSoAdSoConModeChg.
*****************************************************************************************/
void BswM_SoAd_SoConModeChg
(
        SoAd_SoConIdType SoConId,
        SoAd_SoConModeType State
)
{
#if (defined(BSWM_NO_OF_MRP_SOAD_SO_CON_MODE_CHG) && (BSWM_NO_OF_MRP_SOAD_SO_CON_MODE_CHG > 0 ))
    uint16 idx_u16 = 0;
    BswM_Cfg_MRPSoAdSoConModeChgType_tst dataSoAdSoConModeChg_st;
    boolean flgSoAdSoConModeChgValid = FALSE;
    boolean flgDetChecksPassed_b = FALSE;

#if ( BSWM_DEBUG == TRUE )

    /*BswM_Prv_ctrInterrupt_u8 used for counting how many times BswM is getting interrupted*/
    BswM_Prv_ctrInterrupt_u8++;

#endif /* ( BSWM_DEBUG == TRUE ) */

    /* Is BswM initialized ? */
    if ( FALSE == BswM_Prv_isModuleInitialised_b )
    {
        /* DET if the BSW Mode Manager is not initialized.*/
        BSWM_DET_REPORT_ERROR(BSWM_API_ID_SOADSOCONMODECHG, BSWM_E_NO_INIT);
    }
    else
    {
        /* Check whether passed  SoConId is valid and get corresponding index */
        flgSoAdSoConModeChgValid = BswM_Prv_GetSoAdSoConModeChgMRPIndex_b(SoConId, &idx_u16);

        if(FALSE !=flgSoAdSoConModeChgValid)
        {
            if( (State != SOAD_SOCON_OFFLINE) && (State != SOAD_SOCON_RECONNECT) && (State != SOAD_SOCON_ONLINE))
            {
                /* DET if the Current State is not within the valid range.*/
                BSWM_DET_REPORT_ERROR(BSWM_API_ID_SOADSOCONMODECHG, BSWM_E_REQ_MODE_OUT_OF_RANGE);
            }
            else
            {
                flgDetChecksPassed_b = TRUE;
            }
        }

        if(FALSE != flgDetChecksPassed_b)
        {
            /* Check if no other request is in progress */
            if(FALSE != BswM_Prv_ReqProcessOrQueue_b(BSWM_MRP_SOAD_SO_CON_MODE_CHG, SoConId, idx_u16, (uint16)State))
            {
                /* Process any requests that were delayed before this request */
                if (FALSE != BswM_Prv_isReqstDelayed_b)
                {
                    BswM_Prv_ProcessDelayedReqst();
                }

                /*Update RAM buffer with current mode values*/
                  BswM_Cfg_SoAdSoConModeChgInfo_ast[idx_u16].isValidModePresent_b = TRUE;

                  BswM_Cfg_SoAdSoConModeChgInfo_ast[idx_u16].dataMode_en = State;

                  /* Update SoAd MRP type definitions*/
                  dataSoAdSoConModeChg_st = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataSoAdSoConModeChg_ast[idx_u16];

                  /* Call rule evaluation function with the data related to corresponding SoAd MRP */
                  BswM_Prv_RuleEval(dataSoAdSoConModeChg_st.dataReqProcessing_en, BSWM_MRP_SOAD_SO_CON_MODE_CHG, SoConId, dataSoAdSoConModeChg_st.adrRulesRef_pu16, dataSoAdSoConModeChg_st.nrAssociatedRules_u16);

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
    BswM_Prv_InterruptHandler(BSWM_API_ID_SOADSOCONMODECHG);
#endif /* ( BSWM_DEBUG == TRUE ) */
#else
    /*To avoid compiler warning of unused parameters*/
    (void)SoConId;
    (void)State;

#endif /* (BSWM_NO_OF_MRP_SOAD_SO_CON_MODE_CHG > 0) */
    return;
}

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif /* BSWM_RB_SOAD_ENABLED */

/**********************************************************************************************************************
 * End of source file
 **********************************************************************************************************************/
