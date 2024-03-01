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
#include "PduR_Prv.h"
#if defined(PDUR_IFGATEWAY_SUPPORT) && (PDUR_IFGATEWAY_SUPPORT != STD_OFF)
#include "PduR_Gw.h"

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_GF_If_RxIndication_Func
 *    Performs signature translation between RxIndication() and Transmit() during interface-to-interface gatewaying.
 *    The signature needs to be compatible with <Up>_RxIndication
 *
 * \param           PduIdType Id    : ID of gateway PDU that has been received.
 *                  const uint8 *ptr: Pointer to rx SDU (buffer of received payload)
 *
 * \retval          None
 * \seealso         PDUR195
 * \usedresources
 **************************************************************************************************
 */

void PduR_GF_If_RxIndication_Func( PduIdType gwayId, const PduInfoType * data)
{
    const PduR_GT_IfTx * gt = PDUR_GW_IF_TX_BASE + gwayId;
    PduInfoType pduInfo_uo;

    /* Transmit function may point to one of:
     * - PduR_GwMcTx_Transmit_Func() for multicast
     * - Lower layer, for DD non-FIFO transmission.
     * - PduR_GF_DDIf_FIFO_Transmit_Func() for DD-FIFO gateway.
     * - PduR_GF_TTIf_FIFO_Transmit_Func() for TT-FIFO gateway.
     * - PduR_GF_TTIf_Buf_Transmit_Func() for single-buffer TT gateway. */
    /*Ignore the result of this function call. */
    if(data != NULL_PTR)
    {
        pduInfo_uo.SduDataPtr = data->SduDataPtr;
        pduInfo_uo.MetaDataPtr = data->MetaDataPtr;

        /* Check If RoutingPath is direct singlecast gateway then only check if received sdulength is greater than
         * configured Pdulength of DestPdu then forward configured destPdu length else forward received length */
        if((gt->gwType_u8 == PduR_GwNonBuffer) && (data->SduLength > gt->destPduLength_uo))
        {
            pduInfo_uo.SduLength = gt->destPduLength_uo;
        }
        else
        {
            pduInfo_uo.SduLength = data->SduLength;
        }
        #if(PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)
        (void)PDUR_BASE_RPTBL(loTxTablePtr)[(gt->transmitID)].PduR_loTransmitFunc(gt->loId, &pduInfo_uo);
        #else
        (void)PduR_loTransmitTable[(gt->transmitID)].PduR_loTransmitFunc(gt->loId, &pduInfo_uo);
        #endif
    }
    else
    {
        (void)PduR_loTransmitTable[(gt->transmitID)].PduR_loTransmitFunc(gt->loId, data);
    }
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#endif /* PDUR_IFGATEWAY_SUPPORT */

