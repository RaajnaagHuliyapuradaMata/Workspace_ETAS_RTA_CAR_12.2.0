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
#include "BswM_EthIf.h"
#include "BswM_Prv.h"

/*---------------------------------------------------------------------------------------*/
/*- Definition of Global Functions                                                      -*/
/*---------------------------------------------------------------------------------------*/

#if (defined(BSWM_ETHIF_ENABLED) && (BSWM_ETHIF_ENABLED == TRUE))

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"
/*****************************************************************************************
* Function name :   BswM_EthIf_PortGroupLinkStateChg(EthIf_SwitchPortGroupIdxType PortGroupIdx, EthTrcv_LinkStateType PortGroupState)
* Description   :   EthIf function call to indicate the current state of EthIf SwitchPortGroup.
* Parameter     :   PortGroupIdx, PortGroupState
* Return value  :   void
* Remarks       :   The corresponding configuration container for this API is BswMEthIfPortGroupLinkStateChg.
*****************************************************************************************/
void BswM_EthIf_PortGroupLinkStateChg
(
    EthIf_SwitchPortGroupIdxType PortGroupIdx,
    EthTrcv_LinkStateType PortGroupState
)
{
#if (defined(BSWM_NO_OF_MRP_ETHIF_PORTFGROUP) && (BSWM_NO_OF_MRP_ETHIF_PORTFGROUP > 0))
    boolean flgDetChecksPassed_b = FALSE;
    uint16 idx_u16 = 0;
    BswM_Cfg_MRPEthIfPortGroupType_tst dataEthIfPortGroup_ast;
    boolean flgPortGroupIdValid_b = FALSE;

#if ( BSWM_DEBUG == TRUE )
    /*BswM_Prv_ctrInterrupt_u8 used for counting how many times BswM is getting interrupted*/
    BswM_Prv_ctrInterrupt_u8++;
#endif /* ( BSWM_DEBUG == TRUE ) */

    /* Is BswM initialized ? */
    if ( FALSE == BswM_Prv_isModuleInitialised_b )
    {
        /* DET if the BSW Mode Manager is not initialized.*/
        BSWM_DET_REPORT_ERROR(BSWM_API_ID_ETHIFGRPSTATE, BSWM_E_NO_INIT);
    }
    else
    {
        /* Check whether passed EthIf PortGroup-Id is valid and get corresponding index */
        flgPortGroupIdValid_b = BswM_Prv_GetEthIfMRPIndex_b(PortGroupIdx, &idx_u16);

        if(FALSE == flgPortGroupIdValid_b)
        {
            /* Do-nothig and return without any action */
        }
        else if((PortGroupState != ETHTRCV_LINK_STATE_DOWN) &&
                (PortGroupState != ETHTRCV_LINK_STATE_ACTIVE))
        {
            /* DET if the Current State is not within the valid range.*/
            BSWM_DET_REPORT_ERROR(BSWM_API_ID_ETHIFGRPSTATE, BSWM_E_REQ_MODE_OUT_OF_RANGE);
        }
        else
        {
            flgDetChecksPassed_b = TRUE;
        }

        if(FALSE != flgDetChecksPassed_b)
        {
            /* Check if no other request is in progress */
            if(FALSE != BswM_Prv_ReqProcessOrQueue_b(BSWM_MRP_ETHIF_PORTFGROUP, (uint16)PortGroupIdx, idx_u16, (uint16)PortGroupState))
            {
                /* Process any requests that were delayed before this request */
                if (FALSE != BswM_Prv_isReqstDelayed_b)
                {
                    BswM_Prv_ProcessDelayedReqst();
                }

                /*Update RAM buffer with current mode values*/
                BswM_Cfg_EthIfPortGroupStateInfo_ast[idx_u16].isValidModePresent_b = TRUE;

                BswM_Cfg_EthIfPortGroupStateInfo_ast[idx_u16].dataMode_en = PortGroupState;

                dataEthIfPortGroup_ast = BswM_Prv_adrSelectedConfig_pcst->dataModeArbitration_st.dataModeRequestPorts_st.dataEthIfPortGroup_ast[idx_u16];

                BswM_Prv_RuleEval(dataEthIfPortGroup_ast.dataReqProcessing_en, BSWM_MRP_ETHIF_PORTFGROUP, (uint16)PortGroupIdx, dataEthIfPortGroup_ast.adrRulesRef_pu16, dataEthIfPortGroup_ast.nrAssociatedRules_u16);

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
    BswM_Prv_InterruptHandler(BSWM_API_ID_ETHIFGRPSTATE);
#endif /* ( BSWM_DEBUG == TRUE ) */

#else
    /*To avoid compiler warning of unused parameters*/
    (void)PortGroupIdx;
    (void)PortGroupState;
#endif  /* BSWM_NO_OF_MRP_ETHIF_PORTFGROUP */

}

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif /* BSWM_ETHIF_ENABLED */

/**********************************************************************************************************************
 * End of source file
 **********************************************************************************************************************/

