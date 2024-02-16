/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2015, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Com_Prv.h"
#include "Com_Prv_Inl.h"


#ifdef COM_RX_TP_IPDUTYPE

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/

#define COM_START_SEC_CODE
#include "Com_MemMap.h"

/*
 **********************************************************************************************************************
 Function name    : Com_StartOfReception
 Description      : Com_StartOfReception initiates reception of the TP I-PDU with ID ComRxPduId.
                    It returns the size of the internal receive buffer for the I-PDU via RxBufferSizePtr.
 Parameter        : idComRxPdu_uo,pduInfoPtr_pcst,tpSduLength_uo,rxBufferSizePtr_puo(out)
 Return value     : BufReq_ReturnType
 **********************************************************************************************************************
*/
BufReq_ReturnType Com_StartOfReception(
                                PduIdType               idComRxPdu_uo,
                                const PduInfoType*      pduInfoPtr_pcst,
                                PduLengthType           tpSduLength_uo,
                                PduLengthType*          rxBufferSizePtr_puo
                                      )
{
    /* Start: Variable declarations */
    Com_RxIpduCfg_tpcst         rxIpduConstPtr_pcst;
    /* Start: Variable declarations */
    Com_RxIpduRam_tpst          rxIpduRamPtr_pst;
    BufReq_ReturnType           returnValue_en;
    /* End: Variable declarations */

    /* Initialize variable */
    /* Return with BUFREQ_E_NOT_OK if DET occurs or Ipdu group containing this IPDU is not started */
    returnValue_en = BUFREQ_E_NOT_OK;

# if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_StartOfReception, COM_E_UNINIT);
    }
    else if (rxBufferSizePtr_puo == NULL_PTR)
    {
        COM_DET_REPORT_ERROR(COMServiceId_StartOfReception, COM_E_PARAM_POINTER);
    }
    else if (!Com_Prv_IsValidRxIpduId(idComRxPdu_uo))
    {
        COM_DET_REPORT_ERROR(COMServiceId_StartOfReception, COM_E_PARAM);
    }
    else
# endif /* end of COM_PRV_ERROR_HANDLING */
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        *  to internal Id which is generated through configuration
        *  If PC variant is selected, then no mapping table will be used. */
        idComRxPdu_uo       = COM_GET_RX_IPDU_ID(idComRxPdu_uo);

        rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(idComRxPdu_uo);
        rxIpduRamPtr_pst    = rxIpduConstPtr_pcst->ipduRam_pst;

        /* Check if the Ipdu group containing this IPDU is started */
        if (Com_Prv_CheckRxIPduStatus(idComRxPdu_uo))
        {
            PduLengthType rxIPduLengthCfg_uo;

            rxIPduLengthCfg_uo = COM_GET_RXIPDU_SIZE(idComRxPdu_uo);

            /* Check if the reception requests fits Ipdu size,
            * else return overflow as we cannot accomodate this */
            if (tpSduLength_uo <= rxIPduLengthCfg_uo)
            {
                /* As all the checks are passed, then we accept this reception and
                * set flag which will be monitored in CopyRxData calls */
                Com_SetRamValue(RXIPDU,_LARGEDATAINPROG,rxIpduRamPtr_pst->rxFlags_u16,COM_TRUE);
                /* Initialise the received length RAM variable to zero here,
                * This length variable will be incremented in the Com_CopyRxData API */
                rxIpduRamPtr_pst->rxTPIPduLength_uo = COM_ZERO;

                /* Update the TP with Ipdu size for buffer size out parameter */
                *rxBufferSizePtr_puo = rxIPduLengthCfg_uo;

# if defined(COM_METADATA_SUPPORT)

                /* TRACE SWS_Com_00691:
                * If neither first/single frame data nor MetaData are available, this parameter is set to NULL_PTR.
                * Hence check if pduInfoPtr_pcst is not NULL_PTR */

                /* MR12 RULE 13.5 VIOLATION: Below conditional statement access RAM, use of intermediate variables
                * to store and read here, may read obsolete value. Evaluation(or no evaluation) shall not impact the
                * system behaviour. Hence suppressed. */
                if ((pduInfoPtr_pcst != NULL_PTR)
                    && (pduInfoPtr_pcst->MetaDataPtr != NULL_PTR)
                    && Com_GetValue(RXIPDU,_IS_METADATAPDU,rxIpduConstPtr_pcst->rxIPduFields_u8))
                {

                    SchM_Enter_Com_RxIpduProtArea(RECEIVE_METADATA);

                    /* Copy the given MetaData into RxMetaData internal buffer */
                    Com_ByteCopy(&rxIpduRamPtr_pst->rxMetadataBuf_au8[0],
                                 (uint8*)(pduInfoPtr_pcst->MetaDataPtr),
                                 COM_GET_RXMETADATA(idComRxPdu_uo)->rxMetaDataLength_u8);

                    SchM_Exit_Com_RxIpduProtArea(RECEIVE_METADATA);
                }
# endif /* #if defined(COM_METADATA_SUPPORT) */

                /* conditionally used variable(s) is type-cast to 'void' to avoid warnings,
                 * they are intentionally not placed under compiler switch statement,
                 * so that these lines are always available for compilation*/
                (void)pduInfoPtr_pcst;
                /* Positive condition hence status as BUFREQ_OK */
                returnValue_en = BUFREQ_OK;
            }
            else
            {
                /* return with BUFREQ_E_OVFL */
                returnValue_en = BUFREQ_E_OVFL;
            }
        }
    }

    return returnValue_en;
}

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* COM_RX_TP_IPDUTYPE */

