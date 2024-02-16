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
/* Support for PduR_Comxxx()  */


#include "PduR_Prv.h"
/* FC_FileVariationPoint_START */

/* Only enabled if Multicast from Tp support is enabled */
/* This macro has to be generated in PduR_Cfg.h if TP Multicast is required */
#if defined(PDUR_MULTICAST_FROM_TP_SUPPORT) && (PDUR_MULTICAST_FROM_TP_SUPPORT != 0)

#if defined(PDUR_COM_SUPPORT) && (PDUR_COM_SUPPORT != 0)
#if defined(PDUR_TPGATEWAY_SUPPORT) && (PDUR_TPGATEWAY_SUPPORT != STD_OFF)

#include "PduR_Com.h"
#include "PduR_Gw.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_GF_TP_Com_Transmit
 *      Handle "transmission" to Com (TP) as part of a combined gateway and reception operation
 *
 * \param           PduIdType id         :ID of I-PDU to be transmitted.
 *                  PduInfoType info     :Pointer to pointer to PduInfoStructure containing SDU data pointer and
 *                                        SDU length of a transmit buffer.
 *
 * \Re_Val          E_OK, E_NOT_OK
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_GF_TP_Com_Transmit_Func( PduIdType id,
                                                             const PduInfoType * info)
{
    /* Start: Variable declaration */
    PduLengthType bufferSizePtr;
    PduLengthType TpSduLength;
    BufReq_ReturnType bufReq_rtn;
    Std_ReturnType Ret_Val;
    PduR_GT_TpPtr      gt;
    /* Fetch the fixed Buffer details */
    const PduR_FIFO_Tp_fixed * fifo;
    /* Fetch the RAM structure details  */
    PduR_TpSessDynPtr  SessionPtr;
    PduR_TpConnDynPtr ConnectionPtr;
     /* End: Variable declaration */

     /* Start: Variable Initialization */
    gt = PDUR_GW_TP_BASE;
    while(id != gt->upId)
    {
        gt++;
    }
     fifo = (gt->GW_Tp_buffer);
     ConnectionPtr = fifo->connectionTable;
     SessionPtr = ConnectionPtr->primarySession;
     TpSduLength = SessionPtr->SduLength_auo[0];
     bufReq_rtn = BUFREQ_E_NOT_OK;
     Ret_Val = E_NOT_OK;
    /* End: Variable Initialization */

     /*  In case of gatewaying, when a successful RxIndication is received by PduR from the
      *  lower layer, the module shall initiate a reception session for a configured upper layer destination:
      *  Com_StartOfReception, Com_CopyRxData, and Com_RxIndication will be called in this order. */
     if(SessionPtr->infoPtr != NULL_PTR)
     {
         /* Initiate Reception with stored data during CopyRxData*
          * As multiframes is not supported in COM during multicast Gateway*
          * So for the single-frame info will be same at the time of StartOfReception() and CopyRxData()*/
         bufReq_rtn = Com_StartOfReception(id, info, TpSduLength, &bufferSizePtr);
     }
     else
     {
         /*Initiate reception with  NULL_PTR*/
         bufReq_rtn = Com_StartOfReception(id, SessionPtr->infoPtr, TpSduLength, &bufferSizePtr);
     }
     /*  If Com_StartOfReception for a multicast TP gatewaying returns with an error,
      *  the PduR shall stop the upper layer reception without further interaction with the upper layer */
    if(bufReq_rtn != BUFREQ_OK)
    {
        /*StartofReception returns with error */
        Ret_Val = E_NOT_OK;
    }
    /*  When Com_StartOfReception returns BUFREQ_OK, but the available buffer is too
     *  small to receive the whole message, the PduR shall call Com_RxIndication with result = E_NOT_OK */
    else if(bufferSizePtr < TpSduLength)
    {
        /* Available buffer is too small to receive the whole message */
        Com_TpRxIndication(id,E_NOT_OK);
    }
    else
    {
        /* When Com_CopyRxData does not return BUFREQ_OK, the PduR
         * shall call Com_TpRxIndication with result = E_NOT_OK. */
        /*Call the upper layer*/
        bufReq_rtn = Com_CopyRxData(id, info, &bufferSizePtr);
        if(bufReq_rtn != BUFREQ_OK)
        {
            /* <UpTp>_CopyRxData returns an error */
            Com_TpRxIndication(id,E_NOT_OK);
        }
        else
        {
            /* <UpTp>_StartOfReception and <UpTp>_CopyRxData are successful */
            Com_TpRxIndication(id,E_OK);
            Ret_Val = E_OK;
        }
    }
    return Ret_Val;
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#endif /* PDUR_TPGATEWAY_SUPPORT */
#endif /* PDUR_COM_SUPPORT */

#endif /* PDUR_MULTICAST_FROM_TP_SUPPORT */
/* FC_FileVariationPoint_END */
