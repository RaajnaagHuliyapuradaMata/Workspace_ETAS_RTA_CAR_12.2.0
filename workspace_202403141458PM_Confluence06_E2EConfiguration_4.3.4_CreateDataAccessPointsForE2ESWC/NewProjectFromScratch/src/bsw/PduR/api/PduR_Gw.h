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
/* Gateway support functions */
/**
\defgroup PDUR_GW_H    PDUR - Protocol Data Unit Router
*/
#ifndef PDUR_GW_H
#define PDUR_GW_H

#include "PduR_Cfg_Internal.h"
#include "PduR_Types.h"
#include "rba_BswSrv.h"


#ifdef PDUR_BUFFERPOOL_SUPPORT
#include "PduR_BufferPool.h"
#endif /* PDUR_BUFFERPOOL_SUPPORT */
#define PDUR_SESSION_IDLE        0x00    /* Default state, Reception has not yet started i.e PduR_< LoTp >StartOfReception() is not called */
#define PDUR_RX_INPROGRESS       0x01    /* Reception is started, waiting for PduR_< LoTp >CopyRxData() to be called */
#define PDUR_RX_FINISHED         0x02    /* Complete requested message is buffered in PduRTpBuffer */

#define PDUR_TX_IDLE             0x10    /* Default state, Transmission has not yet started i.e PduR_< LoTp >CopyTxData() is not called */
#define PDUR_TX_INPROGRESS       0x20    /* Enough data is available with PduR, waiting for PduR_< LoTp >CopyTxData() to be called  */

/* ------------------------------------------------------------------------ */
/**
 * @ingroup PDUR_GW_H
 */


/*INLINE function protypes*/
LOCAL_INLINE void PduR_InitializePduR_Gw_Tp_Session(PduR_TpSession_Dynamic * dyn, PduR_GT_TpPtr gwTpTablePtr);
LOCAL_INLINE PduLengthType PduR_UpdateBuffSizePtr(PduR_TpSession_Dynamic * dyn);
LOCAL_INLINE void PduR_InitializePduR_Gw_If_DynStruct(PduR_FIFO_If_dynamic * dyn, const PduR_FIFO_fixed * fifo);
LOCAL_INLINE void PduR_ResetPduR_FIFO_Tp_dynamic_Structure(PduR_TpSession_Dynamic * dyn, PduR_GT_TpPtr gwTpTablePtr);
LOCAL_INLINE void PduR_LoadActiveSession(PduR_TpSession_Dynamic * *SessionPtr, const PduR_TpConnection_Dynamic * ConnectionPtr);
LOCAL_INLINE void PduR_Prv_UpdatePduR_Gw_Tp_ReadInfoStruct(PduR_GT_TpPtr gwTpTablePtr_pcst, uint8 * bufferPtr_pu8);
/* Begin section for code
*/
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/*begin: gatewaying */

#if defined(PDUR_IFGATEWAY_SUPPORT) && (PDUR_IFGATEWAY_SUPPORT != STD_OFF)
/* IF layer gatewaying */
/**
 * @ingroup PDUR_GW_H
 *
 * Performs signature translation between RxIndication() and Transmit() during interface-to-interface gatewaying \n
 *\n
 * @param  In:      gwayId - gateway ID \n
 * @param  Out:  data - Pointer to rx SDU (buffer of received payload) \n
 *
 * @return None \n
 */
extern void PduR_GF_If_RxIndication_Func(PduIdType gwayId, const PduInfoType * data );

/**
 * @ingroup PDUR_GW_H
 *
 * Performs signature translation between RxIndication() and Transmit() during interface-to-interface gatewaying \n
 *\n
 * @param  In:      gwayId - gateway ID \n
 * @param  Out:  data - Pointer to rx SDU (buffer of received payload) \n
 *
 * @return None \n
 */
extern void PduR_GF_DDIf_FIFO_TxConfirmation_Func( PduIdType gwayId,Std_ReturnType result);

/**
 * @ingroup PDUR_GW_H
 *\n
 * Performs IF layer gatewaying for Trigger Transmit Tx Confirmation \n
 *\n
 * @param  In:      gwayId - gateway ID \n
 *
 * @return None \n
 */
extern void PduR_GF_TTIf_Buf_TxConfirmation_Func(  PduIdType gwayId,Std_ReturnType result);


/**
 * @ingroup PDUR_GW_H
 *\n
 * Performs IF layer gatewaying for FIFO Tx Confirmation \n
 *\n
 * @param  In:      gwayId - gateway ID \n
 *
 * @return None \n
 */
extern void PduR_GF_TTIf_FIFO_TxConfirmation_Func( PduIdType gwayId,Std_ReturnType result);


/**
 * @ingroup PDUR_GW_H
 *
 * Performs IF layer gatewaying for FIFO Direct transmit \n
 *\n
 * @param  In:      id - PDU ID type \n
 * @param  Out:     info - Pointer to Tx PDU \n
 *\n
 * @return          E_OK: if the request is accepted\n
 *                  E_NOT_OK: if the request is not accepted  just for testing \n
 */
extern Std_ReturnType PduR_GF_DDIf_FIFO_Transmit_Func(PduIdType id, const PduInfoType * info );
/**
 * @ingroup PDUR_GW_H
 *
 *  Performs IF layer gatewaying for Trigger transmit \n
 *\n
 * @param  In:      gwayId - PDU ID type \n
 * @param  Out:     info - Pointer to Tx PDU info \n
 *\n
 * @return          E_OK: if the request is accepted\n
 *                  E_NOT_OK: if the request is not accepted  just for testing \n
 *
 */
extern Std_ReturnType PduR_GF_TTIf_Buf_Transmit_Func( PduIdType gwayId, const PduInfoType * info );
/**
 * @ingroup PDUR_GW_H
 *
 * Performs IF layer gatewaying for FIFO Trigger transmit \n
 *
 * @param  In:     gwayId - PDU ID type \n
 * @param  Out:     info - Pointer to Tx PDU info \n
 *\n
 * @return          E_OK: if the request is accepted\n
 *                  E_NOT_OK: if the request is not accepted  just for testing \n
 *\n
 */
extern Std_ReturnType PduR_GF_TTIf_FIFO_Transmit_Func(PduIdType gwayId, const PduInfoType * info );

/**
 * @ingroup PDUR_GW_H
 *  This API Handle gatewaying between If layers where the destination layer uses Trigger Transmit data provision,
 *  and is configured to use a single entry buffer. \n
 *\n
 * @param  In:      gwayId  - GatewayID of FlexRay that is requested to be transmitted.\n
 * @param  In:      dataPtr - Pointer to place inside the transmit buffer of the L-PDU where data shall be copied to.\n
 */
extern Std_ReturnType PduR_GF_TTIf_Buf_TriggerTransmit_Func( PduIdType gwayId, PduInfoType * dataPtr);

/**
 * @ingroup PDUR_GW_H
 * This API Handle gatewaying between If layers where the destination layer
 *  uses Trigger Transmit data provision, and is configured to use  a FIFO
 *  Note that PDUR254 specifies that an item of state called TxIdx shall be
 *  associated with each FIFO. This is logically equivalent to the readPtr which
 *  we maintain for the FIFO anyway and so is not supplied separately.\n
 * \n
 * @param  In:     gwayId  - GatewayID that is requested to be transmitted. \n
 * @param  In:     dataPtr - Pointer to place inside the transmit buffer of the L-PDU where data shall be copied to. \n
 */
extern Std_ReturnType PduR_GF_TTIf_FIFO_TriggerTransmit_Func(PduIdType gwayId, PduInfoType * dataPtr);

/* Combined IF gateway and reception support */
# if defined(PDUR_COM_SUPPORT) && (PDUR_COM_SUPPORT != 0)
/**
 * @ingroup PDUR_GW_H
 *  This API is support function for combined gateway and reception. \n
 * \n
 * @param  In:         id   :ID of AUTOSAR COM I-PDU to be transmitted.\n
 * @param  Out:        info :A pointer to a structure with I-PDU related data that shall be transmitted
 *                           data length and pointer to I-SDU buffer.\n
 * @return              E_OK Transmit request has been accepted.\n
 */
extern Std_ReturnType PduR_GF_Com_Transmit_Func( PduIdType id, const PduInfoType * info);
# endif /* PDUR_COM_SUPPORT */

# if defined(PDUR_IPDUM_SUPPORT) && (PDUR_IPDUM_SUPPORT != 0)
/**
 * @ingroup PDUR_GW_H
 * This API is support function for combined gateway and reception. \n
 *
 * @param  In:          id  : ID of IPDUM I-PDU to be transmitted. \n
 * @param  Out:         info: A pointer to a structure with I-PDU related data that shall be transmitted:
 *                            data length and pointer to I-SDU buffer. \n
 * @return              E_OK: Transmit request has been accepted. \n
 */
extern Std_ReturnType PduR_GF_IpduM_Transmit_Func( PduIdType id, const PduInfoType * info);
# endif /* PDUR_IPDUM_SUPPORT */

# if defined(PDUR_SECOC_SUPPORT) && (PDUR_SECOC_SUPPORT != 0)
/**
 * @ingroup PDUR_GW_H
 * This API is support function for combined gateway and reception. \n
 *
 * @param  In:          id  : ID of SecOC I-PDU to be transmitted. \n
 * @param  Out:         info: A pointer to a structure with I-PDU related data that shall be transmitted:
 *                            data length and pointer to I-SDU buffer. \n
 * @return              E_OK: Transmit request has been accepted. \n
 */
extern Std_ReturnType PduR_GF_SecOC_Transmit_Func( PduIdType id, const PduInfoType * info);
# endif /* PDUR_SECOC_SUPPORT */

# if defined(PDUR_SD_SUPPORT) && (PDUR_SD_SUPPORT != 0)
/**
 * @ingroup PDUR_GW_H
 * This API is support function for combined gateway and reception. \n
 *
 * @param  In:          id  : ID of Sd I-PDU to be transmitted. \n
 * @param  Out:         info: A pointer to a structure with I-PDU related data that shall be transmitted:
 *                            data length and pointer to I-SDU buffer. \n
 * @return              E_OK: Transmit request has been accepted. \n
 */
extern Std_ReturnType PduR_GF_Sd_Transmit_Func( PduIdType id, const PduInfoType * info);
# endif /* PDUR_SD_SUPPORT */

#endif /* PDUR_IFGATEWAY_SUPPORT */


/**
 * @ingroup PDUR_GW_H
 *\n
 * Performs IF layer gatewaying for Direct transmission for Tx Confirmation \n
 *\n
 * @param  In:      gwayId - gateway ID \n
 *
 * @return None \n
 */
extern void PduR_GF_DDIf_NoBuf_TxConfirmation_Func(PduIdType gwayId,Std_ReturnType result);

/**
 * @ingroup PDUR_GW_H
 *  This API is support function to handle multicast from a gateway operation.\n
 * \n
 * @param  In:         id   :ID of I-PDU to be transmitted.\n
 * @param  Out:        info  :Pointer to pointer to PduInfoStructure containing SDU data pointer and
 *                           SDU length of a transmit buffer.\n
 */
extern Std_ReturnType PduR_GwMcTx_Transmit_Func(PduIdType id, const PduInfoType * info);

#if defined(PDUR_TPGATEWAY_SUPPORT) && (PDUR_TPGATEWAY_SUPPORT != STD_OFF)

/* Combined IF gateway and reception support */
/**
 * @ingroup PDUR_GW_H
 *  This API is support function for combined gateway and reception. \n
 * \n
 * @param  In:         id   :ID of AUTOSAR LDCOM I-PDU to be transmitted.\n
 * @param  Out:        info :A pointer to a structure with I-PDU related data that shall be transmitted
 *                           data length and pointer to I-SDU buffer.\n
 * @return              E_OK Transmit request has been accepted.\n
 */
extern Std_ReturnType PduR_GF_LdCom_Transmit_Func( PduIdType id, const PduInfoType * info);


/**
 * @ingroup PDUR_GW_H
 *  **************************************************************************************************
 * PduR_GF_Tp_StartOfReception - This function will be called by the transport protocol module at the start of
 *                               receiving an I-PDU.
 *                               The I-PDU might be fragmented into multiple N-PDUs (FF with one or more following CFs)
 *                               or might consist of a single N-PDU (SF). The service shall provide the currently
 *                               available maximum buffer size when invoked with TpSdulength equal to 0.
 *
 *
 * \param(in)       id                - Identification of the I-PDU.
 *                    info              - Pointer to a PduInfoType structure containing the payload data
 *                                           (without protocol information) and payload length of the first
 *                                           frame or single frame of a transport protocol I-PDU reception.
 *                                           Depending on the global parameter MetaDataLength, additional bytes
 *                                           containing MetaData (e.g. the CAN ID) are appended after the payload data.
 *                    TpSduLength          - Total length of the PDU to be received.
 *
 * \param(out)        bufferSizePtr      - Available receive buffer in the receiving module.
 *                                        This parameter will be used to compute the Block Size (BS)
 *                                        in the transport protocol module.
 *
 * \retval          BUFREQ_OK           -  Connection has been accepted. bufferSizePtr indicates the availablereceive buffer;
 *                                         reception is continued. If no buffer of the requested size is available,
 *                                         a receive buffer size of 0 shall be indicated by bufferSizePtr.
 *                    BUFREQ_E_NOT_OK   -  Connection has been rejected; reception is aborted. bufferSizePtr remains unchanged.
 *                    BUFREQ_E_OVFL     -  No buffer of the required length can be provided; reception is aborted.
 *                                         bufferSizePtr remains unchanged.
 *
 * \seealso
 * \usedresources
 **************************************************************************************************

 */
extern BufReq_ReturnType PduR_GF_Tp_StartOfReception_Func (  PduIdType gwId,
                                                                                      const PduInfoType * info,
                                                                                      PduLengthType TpSduLength,
                                                                                      PduLengthType * bufferSizePtr);

/**
 * @ingroup PDUR_GW_H
 *  **************************************************************************************************
 * PduR_GF_Tp_CopyRxData - This function is called when transport protocol module have data to copy to
 *                           the receiving module. Several calls may be made during one transportation of an I-PDU.
 *
 * \param(in)       PduIdType    gwId             - Identification of the received I-PDU.
 *                  pduInfoPtr   Info             - Pointer to the buffer (SduDataPtr), metadata (MetaDataPtr) and its
 *                                                  length (SduLength) containing the data to be copied by PDU Router
 *                                                  module in case of gateway or upper layer module in case of
 *                                                  reception.
 * \param(out)      PduLengthType* bufferSizePtr  - Available receive buffer after data has been copied.
 *
 * \retval          BUFREQ_OK: Buffer request accomplished successful.
 *                    BUFREQ_E_NOT_OK: Buffer request not successful. Buffer cannot be accessed.
 *                    BUFREQ_E_OVFL: No Buffer of the required length can be provided.
 * \seealso
 * \usedresources
 **************************************************************************************************

 */
extern BufReq_ReturnType PduR_GF_Tp_CopyRxData_Func (  PduIdType gwId,
                                                             const PduInfoType * info, PduLengthType * bufferSizePtr);
/**
 * @ingroup PDUR_GW_H
 **************************************************************************************************
 * PduR_GF_Tp_RxIndication - Called by the transport protocol module after an I-PDU has been received successfully
 *                              or when an error occurred. It is also used to confirm cancellation of an I-PDU.
 *
 * \param(in)       PduIdType    Id                 - Identification of the received I-PDU.
 *                  Std_ReturnType result        - Result of the reception.
 *
 * \param(out)      None
 *
 * \retval            None
 * \seealso
 * \usedresources
 **************************************************************************************************

 */

extern void PduR_GF_Tp_RxIndication_Func ( PduIdType gwId,
                                                       Std_ReturnType result);

/**
 * @ingroup PDUR_GW_H
 **************************************************************************************************
 * PduR_GF_Tp_CopyTxData - This function is called by the transport protocol module to query
 *                           the transmit data of an I-PDU segment. Each call to this function copies
 *                           the next part of the transmit data until TpDataState indicates TP_DATARETRY.
 *                           In this case the API restarts to copy the data beginning at the
 *                           location indicated by TpTxDataCnt.
 *
 * \param(in)       PduIdType    gwId             - Identification of the transmitted I-PDU.
 *                  pduInfoPtr   Info             - Provides destination buffer and the number of bytes to copy.
 *                                                    In case of gateway the PDU Router module will copy otherwise the source upper
 *                                                    layer module will copy the data. If no enough transmit data is available,
 *                                                    no data is copied. The transport protocol module will retry.
 *                                                    A size of copy size of 0 can be used to indicate state changes in the retry parameter.
 *                    RetryInfoType* retry          - This parameter is used to retransmit data because problems occurred in transmitting it the last time.
 *                                                    If the I-PDU is transmitted from a local module (e.g. DCM) the PDU router module will
 *                                                    just forward the parameter value without check. If the I-PDU is gatewayed from another bus the PDU Router
 *                                                    module will make the following interpretation: If the transmitted TP I-PDU does not support
 *                                                    the retry feature a NULL_PTR can be provided. Alternatively TpDataState can indicate TP_NORETRY.
 *                                                    Both indicate that the copied transmit data can be removed from the buffer after it has been copied.
 *                                                    If the retry feature is used by the Tx I-PDU, RetryInfoPtr must point to a valid RetryInfoType element.
 *                                                    If TpDataState indicates TP_CONFPENDING the previously copied data must remain in the TP buffer
 *                                                    to be available for error recovery. TP_DATACONF indicates that all data, that have been copied so far,
 *                                                    are confirmed and can be removed from the TP buffer. Data copied by this API call are excluded and will
 *                                                    be confirmed later. TP_DATARETRY indicates that this API call shall copy already copied data in order
 *                                                    to recover from an error. In this case TxTpDataCnt specifies the offset of the first byte to be copied by the API call.
 *
 * \param(out)      PduLengthType* availableDataPtr  - Indicates the remaining number of bytes that are available in the PduR Tx buffer.
 *                                                    AvailableTxDataCntPtr can be used by TP modules that support dynamic payload lengths
 *                                                    (e.g. Iso FrTp) to determine the size of the following CFs.
 *
 * \retval            BUFREQ_OK        : Data has been copied to the transmit buffer completely as requested.
 *                    BUFREQ_E_BUSY    : Request could not be fulfilled as the required amount of Tx data is not available.
 *                                      TP layer might retry later on. No data has been copied.
 *                    BUFREQ_E_NOT_OK    : Data has not been copied. Request failed.
 * \seealso
 * \usedresources
 **************************************************************************************************

 */

extern BufReq_ReturnType PduR_GF_Tp_CopyTxData_Func ( PduIdType gwId,
                                                               const PduInfoType * info,
                                                               const RetryInfoType * retry,
                                                               PduLengthType * availableDataPtr);
/**
 * @ingroup PDUR_GW_H
 *  **************************************************************************************************
 * PduR_GF_Tp_TxConfirmation - This function is called by a transport protocol module after
 *                                the I-PDU has been transmitted on its network, the result will reveal
 *                                if the transmission was successful or not.
 *
 * \param(in)    PduIdType  Id             - Identification of the transmitted I-PDU.
 *               Std_ReturnType result   - Result of the transmission of the I-PDU.
 *
 * \param(out)      None
 *
 * \retval          None
 *
 * \seealso
 * \usedresources
 **************************************************************************************************

 */

extern void PduR_GF_Tp_TxConfirmation_Func ( PduIdType gwId,
                                                          Std_ReturnType result);
#endif /* PDUR_TPGATEWAY_SUPPORT */

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"


LOCAL_INLINE void PduR_InitializePduR_Gw_Tp_Session(PduR_TpSession_Dynamic * dyn, PduR_GT_TpPtr gwTpTablePtr)
{
    /* Initilize the elements of the Dynamic Tp buffer Ram structure */
    dyn->bufferPtr = NULL_PTR;
    dyn->writePtr  = NULL_PTR;
#if defined(PDUR_MULTICAST_FROM_TP_SUPPORT) && (PDUR_MULTICAST_FROM_TP_SUPPORT != 0)
    dyn->infoPtr = NULL_PTR;
#endif
    dyn->bufferLength = 0;
#ifdef PDUR_STORE_AND_FORWARD
    dyn->remainingBuffLength = 0;
    dyn->RxChunkLength = 0;
    dyn->TxChunkLength = 0;
#endif
    dyn->Rx_Status = PDUR_SESSION_IDLE;
    dyn->Tx_Status = PDUR_TX_IDLE;
    dyn->Tx_E_OK_Count = 0;
    dyn->Rx_FifoInstanceUsed_u8 = 0;
    dyn->Tx_FifoInstanceUsed_u8 = 0;
    /*Check if PDUR_FIFO_TP_SUPPORT switch is enabled */
#ifdef PDUR_FIFO_TP_SUPPORT
    if(gwTpTablePtr->TpQueueDepth_u8 > 1u)
    {
        uint8 index_u8;
        for(index_u8 = 0; index_u8 < gwTpTablePtr->TpQueueDepth_u8; index_u8++)
        {
            dyn->SduLength_auo[index_u8]   = 0;
            dyn->numRxLength_auo[index_u8] = 0;
        }
    }
    else
#endif
    {
        dyn->SduLength_auo[0]   = 0;
        dyn->numRxLength_auo[0] = 0;
        (void)gwTpTablePtr;
    }
}

LOCAL_INLINE void PduR_InitializePduR_Gw_If_DynStruct(PduR_FIFO_If_dynamic * dyn, const PduR_FIFO_fixed * fifo)
{
    uint8 indexArr_8;
    /*Initilize the elements of the Dynamic Fifo If buffer Ram structure*/
    dyn->writePtr = fifo->begin;
    dyn->readPtr = fifo->begin;
    for(indexArr_8=0; indexArr_8 < fifo->nrElts; indexArr_8++)
    {
        dyn->actLen[indexArr_8] = 0;
    }

    dyn->used = 0;
    dyn->txConfPending = 0;
    dyn->readIndex = 0;
    dyn->writeIndex = 0;
}

LOCAL_INLINE void PduR_ResetPduR_FIFO_Tp_dynamic_Structure(PduR_TpSession_Dynamic * dyn, PduR_GT_TpPtr gwTpTablePtr)
{
    /* reset the status flag to Tx Idle and Rx Idle
     * First reset the Rx_Status as this is the first status check done in the callbacks */
    dyn->Rx_Status = PDUR_SESSION_IDLE;
    dyn->Tx_Status = PDUR_TX_IDLE;

    /* Clear the other elements of the Dynamic Ram structure */
    /* reset read and write pointer */
    dyn->readPtr = NULL_PTR;
    dyn->writePtr = NULL_PTR;
#if defined(PDUR_MULTICAST_FROM_TP_SUPPORT) && (PDUR_MULTICAST_FROM_TP_SUPPORT != 0)
    dyn->infoPtr = NULL_PTR;
#endif
    dyn->numTxLength = 0;
#ifdef PDUR_STORE_AND_FORWARD
    dyn->remainingBuffLength = 0;
    dyn->RxChunkLength = 0;
    dyn->TxChunkLength = 0;
#endif
    dyn->Tx_E_OK_Count = 0;
    dyn->Rx_FifoInstanceUsed_u8 = 0;
    dyn->Tx_FifoInstanceUsed_u8 = 0;
    /* Check if PDUR_FIFO_TP_SUPPORT is enabled*/
#ifdef PDUR_FIFO_TP_SUPPORT
    if(gwTpTablePtr->TpQueueDepth_u8 > 1u)
    {
        uint8 index_u8;
        for(index_u8 = 0; index_u8 < gwTpTablePtr->TpQueueDepth_u8; index_u8++)
        {
            dyn->SduLength_auo[index_u8]   = 0;
            dyn->numRxLength_auo[index_u8] = 0;
        }
    }
    else
#endif
    {
        dyn->SduLength_auo[0]   = 0;
        dyn->numRxLength_auo[0] = 0;
        (void)gwTpTablePtr;
    }

}

LOCAL_INLINE void PduR_LoadActiveSession(PduR_TpSession_Dynamic * *SessionPtr, const PduR_TpConnection_Dynamic * ConnectionPtr)
{
    /* FC_VariationPoint_START */
#ifdef PDUR_QUEUEING_SUPPORT
    /* Read into a local variable for the sake of automicity */
    uint8 activeSessionCount;
    activeSessionCount = ConnectionPtr->activeSessionCount;
    if(activeSessionCount == 1)
    {
        /* Fetch the RAM structure details  */
        *SessionPtr = (ConnectionPtr->primarySession);
    }

    else if(activeSessionCount == 2)
    {
        /* Fetch the RAM structure details  */
        *SessionPtr = (ConnectionPtr->secondarySession);
    }

    else
    {
        *SessionPtr = NULL_PTR;
    }
#else
    /* FC_VariationPoint_END */
    *SessionPtr = (ConnectionPtr->primarySession);
    /* FC_VariationPoint_START */
#endif
    /* FC_VariationPoint_END */

}
/* SOR- StartOfReception, TxConf - TxConfirmation
 * readPtrs are updating in SOR, because SOR can be called again for same Pdu.
 */
LOCAL_INLINE void PduR_Prv_UpdatePduR_Gw_Tp_ReadInfoStruct(PduR_GT_TpPtr gwTpTablePtr_pcst, uint8 * bufferPtr_pu8)
{
    /* check if GwTp RP is multicast */
    if(gwTpTablePtr_pcst->numOfLowerLayers > 1u)
    {
        PduIdType mcId_uo;
        mcId_uo = gwTpTablePtr_pcst->loId;
        /* In GwTP table entrys will be generated for each destination,
         * To Copy the correct Data in destination during PduR_LoTpCopyTxData api, readPtrs and numTxLength are updated for each destination. */
        while (gwTpTablePtr_pcst->loId == mcId_uo)
        {
			/* Check if multicast gateway involving upperlayer, for upper layer NULL_PTR is generated*/
            if(gwTpTablePtr_pcst->TpReadBuffInfo_pst != NULL_PTR)
            {
                gwTpTablePtr_pcst->TpReadBuffInfo_pst->readPtr_pu8 = bufferPtr_pu8;
                gwTpTablePtr_pcst->TpReadBuffInfo_pst->numTxLength_uo = 0;
            }
            gwTpTablePtr_pcst++;
        }
    }
    else
    {
        /* Do Nothing
         * To avoid warning*/
    }
}
/* MR12 RULE 8.13 VIOLATION: The member elements of dyn pointer is modified only if STORE_AND_FORWARD feature is enabled.
   There warning is senn only when this feature is disabled. */
LOCAL_INLINE PduLengthType PduR_UpdateBuffSizePtr(PduR_TpSession_Dynamic * dyn)
{
    PduLengthType  CurrentBufferLength_l;

#ifdef PDUR_STORE_AND_FORWARD
    /* Ensure CopyTxData() called if buffer is full
    * Update bufferSizePtr if RxDataChunkLength reached to bufferLength */
    if((dyn->RxChunkLength == dyn->bufferLength) && (((dyn->RxChunkLength - dyn->TxChunkLength) != 0u) ||
            (dyn->numRxLength_auo[dyn->Rx_FifoInstanceUsed_u8] == dyn->numTxLength)))
    {
        dyn->writePtr = dyn->bufferPtr;
        dyn->RxChunkLength = 0;
        dyn->remainingBuffLength = dyn->TxChunkLength;
    }
    /* 1st write operation is complete and reading is continue*/
    else if(dyn->TxChunkLength > dyn->RxChunkLength)
    {
        dyn->remainingBuffLength = dyn->TxChunkLength - dyn->RxChunkLength;
    }
    /*In case of first Time CopyRxData 1st write is going on*/
    else if((dyn->numRxLength_auo[dyn->Rx_FifoInstanceUsed_u8] == dyn->numTxLength) ||
                 (dyn->RxChunkLength > dyn->TxChunkLength))
    {
        dyn->remainingBuffLength = ((dyn->bufferLength - dyn->RxChunkLength) + dyn->TxChunkLength);
    }
    else
    {
        /*do nothing*
         * to remove Misra warning*/
    }
    CurrentBufferLength_l = dyn->remainingBuffLength;
#else
    /* Update outpointer with remaining free buffer length */
    CurrentBufferLength_l = dyn->bufferLength - dyn->numRxLength_auo[dyn->Rx_FifoInstanceUsed_u8];

#endif /* #ifdef PDUR_STORE_AND_FORWARD */

    return CurrentBufferLength_l;
}

/* Only enabled if Multicast from Tp support is enabled */
/* This macro has to be generated in PduR_Cfg.h if TP Multicast is required */
#if defined(PDUR_MULTICAST_FROM_TP_SUPPORT) && (PDUR_MULTICAST_FROM_TP_SUPPORT != 0)

# if defined(PDUR_DCM_SUPPORT) && (PDUR_DCM_SUPPORT != 0)
#  if defined(PDUR_TPGATEWAY_SUPPORT) && (PDUR_TPGATEWAY_SUPPORT != STD_OFF)
/* ------------------------------------------------------------------------ */
/* Begin section for code */
/* FC_VariationPoint_START */
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 * @ingroup PDUR_GW_H
 *  **************************************************************************************************
 * PduR_GF_TP_Dcm_Transmit_Func - This function is called when there is Tp gateway multicast configuration.
 *
 * \param(in)       PduIdType    id             - Identification of the transmitted I-PDU.
 *                  const PduInfoType * info          - A pointer to a structure with I-PDU related data that shall be
 *                                                      transmitted data length and pointer to I-SDU buffer.\n
 *
 * \param(out)      None
 *
 * \retval          Std_ReturnType              E_OK Transmit request has been accepted.\n
 *                                                 E_NOT_OK Transmit request has been rejected.\n
 * \seealso
 * \usedresources
 **************************************************************************************************
 */

extern Std_ReturnType PduR_GF_TP_Dcm_Transmit_Func( PduIdType id, const PduInfoType * info);

/*end */

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
/* FC_VariationPoint_END */
#  endif /* PDUR_TPGATEWAY_SUPPORT */
# endif /* PDUR_DCM_SUPPORT */

# if defined(PDUR_COM_SUPPORT) && (PDUR_COM_SUPPORT != 0)
#  if defined(PDUR_TPGATEWAY_SUPPORT) && (PDUR_TPGATEWAY_SUPPORT != STD_OFF)
/* ------------------------------------------------------------------------ */
/* Begin section for code */
/* FC_VariationPoint_START */
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 * @ingroup PDUR_GW_H
 *  **************************************************************************************************
 * PduR_GF_TP_Com_Transmit_Func - This function is called when there is Tp gateway multicast configuration.
 *
 * \param(in)       PduIdType    id             - Identification of the transmitted I-PDU.
 *                  const PduInfoType * info          - A pointer to a structure with I-PDU related data that shall be
 *                                                      transmitted data length and pointer to I-SDU buffer.\n
 *
 * \param(out)      None
 *
 * \retval          Std_ReturnType              E_OK Transmit request has been accepted.\n
 *                                                 E_NOT_OK Transmit request has been rejected.\n
 * \seealso
 * \usedresources
 **************************************************************************************************
 */

extern Std_ReturnType PduR_GF_TP_Com_Transmit_Func( PduIdType id, const PduInfoType * info);

/*end */

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
/* FC_VariationPoint_END */
#  endif /* PDUR_TPGATEWAY_SUPPORT */
# endif /* PDUR_COM_SUPPORT */

#endif /* PDUR_MULTICAST_FROM_TP_SUPPORT */

#if defined(PDUR_TPGATEWAY_SUPPORT) && (PDUR_TPGATEWAY_SUPPORT != STD_OFF)

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

extern BufReq_ReturnType PduR_GetTpBuffer(  const PduR_GT_Tp * gt,
                                                        PduLengthType TpSduLength);

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

# ifdef PDUR_BUFFERPOOL_SUPPORT
extern void PduR_ReleaseTpBuffer(  const PduR_GT_Tp * gt);
# endif /* PDUR_BUFFERPOOL_SUPPORT */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#endif /* PDUR_TPGATEWAY_SUPPORT */

#endif /* PDUR_GW_H */

