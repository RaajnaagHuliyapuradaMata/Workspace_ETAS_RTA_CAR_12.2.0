/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2014, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#include "PduR.h"
#include "PduR_Prv.h"
#if defined(PDUR_IFGATEWAY_SUPPORT) && (PDUR_IFGATEWAY_SUPPORT != STD_OFF)
#include "PduR_Gw.h"

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_GF_TTIf_Buf_TriggerTransmit_Func -   Handle gatewaying between If layers where the destination layer
 *            uses Trigger Transmit data provision, and is configured to use  a single entry buffer
 *
 * \param           PduIdType gwayId - GatewayID of FlexRay that is requested to be transmitted.
 *                  uint8 *  dataPtr - Pointer to place inside the transmit buffer of the L-PDU where data shall be
 *                                     copied to.
 *
 * \retval          None
 * \seealso
 * \usedresources
 **************************************************************************************************
 */

/* The signature needs to be compatible with <Up>_TriggerTransmit */
Std_ReturnType PduR_GF_TTIf_Buf_TriggerTransmit_Func( PduIdType gwayId,
                                                               PduInfoType * dataPtr )
{
    const PduR_GT_If * gt;
    PduR_Buf_If_dynamic  *dyn;
    PduInfoType * infoBuf;
    uint16 i;
    Std_ReturnType retVal;
    gt      = PDUR_GW_IF_BASE + gwayId;
    /* MR12 RULE 11.5, 11.8 VIOLATION : 'pointer-to-void' is being cast to a 'pointer-to-object'
     * type and this conversion will not result in any misalignment.
     * Hence Supressed.*/
    dyn     =  (PduR_Buf_If_dynamic*) gt->GW_If_buffer;
    infoBuf = dyn->gwBuf;
    retVal = E_NOT_OK;

    SchM_Enter_PduR_LockIFGatewayTriggerTxNoNest();

    /*  [SWS_PduR_00819] : Check the lower layer buffer size,
     *  if lower layer buffer size is less than the stored Pdu Data length then return E_NOT_OK
     *  lowerLayer Buffer Length >= Stored PduData Length  */
    if(dataPtr->SduLength >= infoBuf->SduLength)
    {
        for(i = 0 ; i < infoBuf->SduLength ; i++)
        {
            *((dataPtr->SduDataPtr)+i) = *((infoBuf->SduDataPtr)+i);
        }
        dataPtr->SduLength = infoBuf->SduLength;
        retVal = E_OK;
    }
    /* If metadataPtr is NULL_PTR and metadataTyperef is not configured, then this will not be copied */
    if((gt->metaDataIFInfo_pst != NULL_PTR) && (dataPtr->MetaDataPtr != NULL_PTR))
    {
        for(i = 0 ; i < gt->metaDataIFInfo_pst->metaDataBufLen ; i++)
        {
            *((dataPtr->MetaDataPtr)+i) = *((infoBuf->MetaDataPtr)+i);
        }
    }

    /* Set the transmission status as idle, So PduR can accept the next reception */
    dyn->Tx_Status = PDUR_TX_IDLE;
    SchM_Exit_PduR_LockIFGatewayTriggerTxNoNest();
    return retVal;
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_IFGATEWAY_SUPPORT */

