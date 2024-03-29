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
/** Handle ProvideTxBuffer invocation for Tp layer gatewaying,
  * for the situation when using a FIFO
  */

#include "PduR_Prv.h"
#if defined(PDUR_TPGATEWAY_SUPPORT) && (PDUR_TPGATEWAY_SUPPORT != STD_OFF)
#include "PduR_Gw.h"
#ifdef PDUR_BUFFERPOOL_SUPPORT
#include "PduR_BufferPool.h"
#endif /* PDUR_BUFFERPOOL_SUPPORT */

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_GF_Tp_CopyTxData_Func - This function is called by the transport protocol module to query
 *                                the transmit data of an I-PDU segment. Each call to this function copies
 *                                the next part of the transmit data until TpDataState indicates TP_DATARETRY.
 *                                In this case the API restarts to copy the data beginning at the
 *                                location indicated by TpTxDataCnt.
 *
 * \param(in)       PduIdType    gwId             - Identification of the transmitted I-PDU.
 *                  pduInfoPtr   Info             - Provides destination buffer and the number of bytes to copy.
 *                                                  In case of gateway the PDU Router module will copy otherwise the
 *                                                  source upper layer module will copy the data. If no enough transmit
 *                                                  data is available, no data is copied. The transport protocol module
 *                                                  will retry. A size of copy size of 0 can be used to indicate state
 *                                                  changes in the retry parameter.
 *                    RetryInfoType* retry        - This parameter is used to retransmit data because problems occurred
 *                                                  in transmitting it the last time. If the I-PDU is transmitted from
 *                                                  a local module (e.g. DCM) the PDU router module will just forward
 *                                                  the parameter value without check. If the I-PDU is gatewayed from
 *                                                  another bus the PDU Router module will make the following
 *                                                  interpretation: If the transmitted TP I-PDU does not support the
 *                                                  retry feature a NULL_PTR can be provided.
 *                                                  It indicates that the copied transmit data can be removed from the
 *                                                  buffer after it has been copied.
 *                                                  If the retry feature is used by the Tx I-PDU,
 *                                                  RetryInfoPtr must point to a valid RetryInfoType element.
 *                                                  If TpDataState indicates TP_CONFPENDING the previously copied data
 *                                                  must remain in the TP buffer to be available for error recovery.
 *                                                  TP_DATACONF indicates that all data, that have been copied so far,
 *                                                  are confirmed and can be removed from the TP buffer.
 *                                                  Data copied by this API call are excluded and will be confirmed
 *                                                  later.
 *                                                  TP_DATARETRY indicates that this API call shall copy already
 *                                                  copied data in order to recover from an error.
 *                                                  In this case TxTpDataCnt specifies the offset of the first byte
 *                                                  to be copied by the API call.
 *
 * \param(out)      PduLengthType* availableDataPtr  - Indicates the remaining number of bytes that are available
 *                                                     in the PduR Tx buffer. AvailableTxDataCntPtr can be used by
 *                                                     TP modules that support dynamic payload lengths
 *                                                     (e.g. Iso FrTp) to determine the size of the following CFs.
 *
 * \retval            BUFREQ_OK        : Data has been copied to the transmit buffer completely as requested.
 *                    BUFREQ_E_BUSY    : Request could not be fulfilled as the required amount of Tx data is not
 *                                       available. TP layer might retry later on. No data has been copied.
 *                    BUFREQ_E_NOT_OK  : Data has not been copied. Request failed.
 * \seealso
 * \usedresources
 **************************************************************************************************
 */

/* MR12 RULE 8.13 VIOLATION: A pointer parameter in a function prototype should be declared as pointer to const if the
 * pointer is not used to modify the addressed object.
 * But AUTOSAR SWS specification signature is as below, Hence suppressed */
BufReq_ReturnType PduR_GF_Tp_CopyTxData_Func(PduIdType gwId,
        const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr)
{
    /* Start: Variable declaration */
    PduR_GT_TpPtr      gt;
    /* Fetch the fixed Buffer details */
    const PduR_FIFO_Tp_fixed * fifo;
    /* Fetch the RAM structure details  */
    PduR_TpConnDynPtr ConnectionPtr;
    PduR_TpSessDynPtr  SessionPtr;
    BufReq_ReturnType rtn;
    /* End: Variable declaration */

    /* Start: Variable Initialization */
    gt = PDUR_GW_TP_BASE + gwId;
    fifo = (gt->GW_Tp_buffer);
    ConnectionPtr = fifo->connectionTable;
    /* Tx is always on primary session */
    SessionPtr = ConnectionPtr->primarySession;
    rtn = BUFREQ_OK;
    /* End: Variable Initialization */
    /* Update SessionPtr readPtr and numTxLength for multicast lower layer dests */
        if(gt->numOfLowerLayers > 1u)
        {
            /* Update readPtr and numTxLength in sessionPtr for each destination*/
            SessionPtr->readPtr = gt->TpReadBuffInfo_pst->readPtr_pu8;
            SessionPtr->numTxLength = gt->TpReadBuffInfo_pst->numTxLength_uo;
         }

    /* Check for RX and TX status of Session Ptr as both are expected to be in IN_PROGRESS state */
    /* Check if requested length is greater than complete message length requested
     * in PduR_<LoTp>StartOfReception call */
    if ((SessionPtr->Rx_Status == PDUR_SESSION_IDLE) ||(SessionPtr->Tx_Status == PDUR_TX_IDLE) ||
            (info->SduLength > (SessionPtr->SduLength_auo[SessionPtr->Tx_FifoInstanceUsed_u8] -
                    SessionPtr->numTxLength)))
    {
        /*RxIndication was received with negative result*/
        return  BUFREQ_E_NOT_OK;
    }

    if(info->SduLength == 0u)
    {
        /* Dest LoTp has requested with sdulength = 0, update the out parameter,availableDataPtr,
         * with available data in PduRTpbuffer i.e Received data - Transmitted data.
         * If availableDataPtr is 0 then return BUFREQ_E_BUSY.
         */
        *(availableDataPtr) =
                (SessionPtr->numRxLength_auo[SessionPtr->Tx_FifoInstanceUsed_u8] - SessionPtr->numTxLength);
        if(*(availableDataPtr) != 0u)
        {
            return BUFREQ_OK;
        }
        else
        {
            return BUFREQ_E_BUSY;
        }
    }

    /* check if the received data is more than requested data */
    if(SessionPtr->numRxLength_auo[SessionPtr->Tx_FifoInstanceUsed_u8] >= (info->SduLength + SessionPtr->numTxLength))
    {
        /*Check if Retry is a Null Pointer or the TpDataState is TP_DATACONF */
        if((retry == NULL_PTR) || (retry->TpDataState == TP_DATACONF))
        {
            /* unicast gateway case and multicast gateway case with single frames where the buffer pointers are
             * not updated, only the data is copied and available data pointer is made 0*/
            /* check if the received data is equal to the requested data */
            if(info->SduLength == SessionPtr->SduLength_auo[SessionPtr->Tx_FifoInstanceUsed_u8])
            {

                /* Copy the data into the transmit buffer and update the read pointer */
                rba_BswSrv_MemCopy8(info->SduDataPtr,SessionPtr->readPtr,info->SduLength);

                /* Copy the metadata into the buffer */
                if((SessionPtr->metaDataPtr != NULL_PTR) && (info->MetaDataPtr != NULL_PTR))
                {
                    rba_BswSrv_MemCopy8(info->MetaDataPtr,SessionPtr->metaDataPtr,gt->MetaDataInfo_pst->metaDataBufLen);
                }


                /* Indicates the remaining number of bytes that are available in the PduR Tp buffer*/
                *(availableDataPtr) = 0;

            }   /* end of - if(info->SduLength == SessionPtr->SduLength_auo[SessionPtr->Tx_FifoInstanceUsed_u8]) */

            else
            {
                /* unicast gateway case and multicast gateway case with multiple frames where the buffer
                 * read pointer is updated, data is copied, transmitted length is
                 * updated and the available data pointer is updated*/

#ifdef PDUR_STORE_AND_FORWARD

                PduLengthType availableDataLength_l, txChunkTotalLength_uo;
                /* For singlecast multiframe and multicast singleframe RP, always numTxLength_uo >= TxChunkLength_uo.
                 * StoreNFwd feature for multiframe multicast gateway [parallel reception and transmission] RP is not supported.
                 * And In multicast multiframe gateway TxChunkLength can be more than numTxLength_uo.
                 * As it should enter to the code where read pointer is reset below code is added */
                if(SessionPtr->numTxLength > SessionPtr->TxChunkLength)
                {
                    txChunkTotalLength_uo = SessionPtr->TxChunkLength;
                }
                else
                {
                    txChunkTotalLength_uo = SessionPtr->numTxLength;
                }
                if(txChunkTotalLength_uo == SessionPtr->bufferLength)
                {
                    /* readPtr reaches to BufferLength
                     * reset the readPtr */
                    SessionPtr->readPtr = SessionPtr->bufferPtr;
                    SessionPtr->TxChunkLength = 0;

                }
                availableDataLength_l = SessionPtr->bufferLength - SessionPtr->TxChunkLength;
                if(info->SduLength > availableDataLength_l)
                {
                    /* Requested length cross the bufferLength
                     * Copy the data into destination buffrer till bufferlength*/
                    rba_BswSrv_MemCopy8(info->SduDataPtr,SessionPtr->readPtr,availableDataLength_l);
                    /* Reset the readPtr and transmitted chunk length */
                    SessionPtr->readPtr = SessionPtr->bufferPtr;
                    SessionPtr->TxChunkLength = 0;
                    /*Copy rest of bytes into destination buffer*/
                    rba_BswSrv_MemCopy8((info->SduDataPtr + availableDataLength_l),
                            SessionPtr->readPtr, (info->SduLength - availableDataLength_l));
                    /* Increment the readPtr */
                    SessionPtr->readPtr += (info->SduLength - availableDataLength_l);
                    /*Increment the transmitted data chunk Length */
                    SessionPtr->TxChunkLength += (info->SduLength - availableDataLength_l);

                }
                else
                {
                    /* Copy the data into the transmit buffer */
                    rba_BswSrv_MemCopy8(info->SduDataPtr,SessionPtr->readPtr,info->SduLength);
                    /* Increment the readPtr */
                    SessionPtr->readPtr += info->SduLength;
                    /*Increment the transmitted data chunk Length */
                    SessionPtr->TxChunkLength += info->SduLength;
                }

    #else
                /* Copy the data into the transmit buffer and update the read pointer */
                rba_BswSrv_MemCopy8(info->SduDataPtr,SessionPtr->readPtr,info->SduLength);
                SessionPtr->readPtr += info->SduLength;

                /* Copy the metadata into the buffer */
                if((SessionPtr->metaDataPtr != NULL_PTR) && (info->MetaDataPtr != NULL_PTR))
                {
                    rba_BswSrv_MemCopy8(info->MetaDataPtr,SessionPtr->metaDataPtr,gt->MetaDataInfo_pst->metaDataBufLen);
                }
    #endif

                /*Increment the total transmitted data Length */
                SessionPtr->numTxLength += info->SduLength;
                /* update the remaining number of bytes that are available in the PduR Tp buffer */
                *(availableDataPtr) =
                        SessionPtr->numRxLength_auo[SessionPtr->Tx_FifoInstanceUsed_u8] - SessionPtr->numTxLength;

            }
            /* Updated readPtr and numTxLength in GwTp for multicast lower layer destination*/
            if(gt->numOfLowerLayers > 1u)
            {
                gt->TpReadBuffInfo_pst->readPtr_pu8 = SessionPtr->readPtr;
                gt->TpReadBuffInfo_pst->numTxLength_uo = SessionPtr->numTxLength;
            }
       }   /* end of - if((retry == NULL_PTR) || (retry->TpDataState == TP_DATACONF)) */
       else
        {
            /* Because Dest LoTp has requested retransmission with state other than TP_DATACONF */
            /* PDU Router shall immediately stop further processing of this I-PDU */
            rtn = BUFREQ_E_NOT_OK;
        }
    }   /* end of - if(SessionPtr->numRxLength_auo[SessionPtr->Tx_FifoInstanceUsed_u8] >=
         *      (info->SduLength + SessionPtr->numTxLength)) */
    else
    {
        /* Reception in progress */
        rtn = BUFREQ_E_BUSY;

    }
    return rtn;
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#endif /* PDUR_TPGATEWAY_SUPPORT */

