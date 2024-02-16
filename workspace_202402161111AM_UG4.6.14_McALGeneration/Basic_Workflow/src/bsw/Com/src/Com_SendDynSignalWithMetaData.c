/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2020, all rights reserved
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

#ifdef COM_METADATA_SUPPORT

#if defined(COM_TX_DYNAMIC_SIGNAL_SUPPORT) || defined(COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT)

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

#ifdef COM_TX_DYNAMIC_SIGNAL_SUPPORT
LOCAL_INLINE uint8 Com_Prv_SendDynSignalWithMetaData(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv,
                                                                 uint16 length_u16, const uint8 * metaDataPtr_pcu8);
#endif

#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
LOCAL_INLINE uint8 Com_Prv_CopyDynGroupSignalWithMetaData(Com_SignalIdType idSignal_u16,
                                                          const void * signalDataPtr_pcv,
                                                          uint16 length_u16, const uint8 * metaDataPtr_pcu8);
#endif

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
 Function name    : Com_SendDynSignalWithMetaData
 Description      : Service updates the signal object identified by SignalId with the signal
                    referenced by the SignalDataPtr parameter and length of the dynamic length signal
 Parameter        : idSignal_u16        -> Id of the signal.
                  : signalDataPtr_pcv   -> The pointer to the address where the application data is available.
                  : length_u16          -> Length of the dynamic length signal
                  : metaDataPtr_pcu8    -> Pointer to the meta data of the signal
 Return value     : E_OK/COM_SERVICE_NOT_AVAILABLE/E_NOT_OK/COM_BUSY
 **********************************************************************************************************************
*/
uint8 Com_SendDynSignalWithMetaData(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv, uint16 length_u16,
                                                                                      const uint8 * metaDataPtr_pcu8)
{
#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
    Com_TxGrpSigCfg_tpcst       txGrpSigConstPtr_pcst;
#endif
#ifdef COM_TX_DYNAMIC_SIGNAL_SUPPORT
    Com_TxSigCfg_tpcst          txSigConstPtr_pcst;
# ifdef COM_TX_TP_IPDUTYPE
    Com_TxIpduRam_tpst          txIpduRamPtr_pst;
    Com_IpduId_tuo              idIpdu_uo;
# endif
#endif
    uint16                      type_u16;
    uint8                       status_u8;
    /* Initialize variable */
    status_u8 = COM_SERVICE_NOT_AVAILABLE;

# if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SendDynSignalWithMetaData, COM_E_UNINIT);
    }
    else if (signalDataPtr_pcv == NULL_PTR)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SendDynSignalWithMetaData, COM_E_PARAM_POINTER);
    }
    else
# endif /* end of COM_PRV_ERROR_HANDLING */
#ifdef COM_TX_DYNAMIC_SIGNAL_SUPPORT
    /* Check for dynamic signal */
    if (Com_Prv_IsValidTxSignalId(idSignal_u16))
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idSignal_u16        = COM_GET_TXSIGNAL_ID(idSignal_u16);
        txSigConstPtr_pcst  = COM_GET_TXSIG_CONSTDATA(idSignal_u16);
        type_u16            = Com_GetValue(GEN,_TYPE,txSigConstPtr_pcst->general_u8);

# ifdef COM_TX_TP_IPDUTYPE
        idIpdu_uo           = txSigConstPtr_pcst->idComIPdu_uo;
        txIpduRamPtr_pst    = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo)->ipduRam_pst;

        /* Check whether the large data pdu flag has been reset */
        if (Com_GetRamValue(TXIPDU,_LARGEDATAINPROG,txIpduRamPtr_pst->txFlags_u16))
        {
            status_u8 = COM_BUSY;
        }
        else
# endif /*#ifdef COM_TX_TP_IPDUTYPE*/
        {
            /* If the application requests with length greater than the maximum length of the signal */
            if ((length_u16 <= txSigConstPtr_pcst->bitSize_uo) && (COM_UINT8_DYN == type_u16))
            {
                status_u8 = Com_Prv_SendDynSignalWithMetaData(idSignal_u16, signalDataPtr_pcv, length_u16,
                                                                                                metaDataPtr_pcu8);
            }
            else
            {
                status_u8 = E_NOT_OK;
            }
        }
    }
    else
#endif /* #ifdef COM_TX_DYNAMIC_SIGNAL_SUPPORT */
#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
    /* Check for dynamic group signal */
    if (Com_Prv_IsValidTxGroupSignalId(idSignal_u16))
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idSignal_u16            = COM_GET_TXGRPSIGNAL_ID(idSignal_u16);
        txGrpSigConstPtr_pcst   = COM_GET_TXGRPSIG_CONSTDATA(idSignal_u16);

        type_u16                = Com_GetValue(TXGRPSIG,_TYPE,txGrpSigConstPtr_pcst->txGrpSigFields_u8);

        /* If the application requests with length greater than the maximum length of the signal */
        if ((length_u16 <= txGrpSigConstPtr_pcst->bitSize_uo) && (COM_UINT8_DYN == type_u16))
        {
            status_u8 = Com_Prv_CopyDynGroupSignalWithMetaData(idSignal_u16, signalDataPtr_pcv, length_u16,
                                                                                                metaDataPtr_pcu8);
        }
        else
        {
            status_u8 = E_NOT_OK;
        }
    }
    else
#endif /* #ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT */
    {
        /* Dynamic signal/dynamic group signal id is not exist,
        * hence return value is COM_SERVICE_NOT_AVAILABLE */
        COM_DET_REPORT_ERROR(COMServiceId_SendDynSignalWithMetaData, COM_E_PARAM);
    }

    return status_u8;
}


#ifdef COM_TX_DYNAMIC_SIGNAL_SUPPORT
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_SendDynSignalWithMetaData
 Description      : Service updates the signal object identified by SignalId with the signal
                    referenced by the SignalDataPtr parameter and length of the dynamic length signal
 Parameter        : idSignal_u16        -> Id of the signal.
                  : signalDataPtr_pcv   -> The pointer to the address where the application data is available.
                  : length_u16          -> Length of the dynamic length signal
                  : metaDataPtr_pcu8    -> Pointer to the meta data of the signal
 Return value     : E_OK/COM_SERVICE_NOT_AVAILABLE
 **********************************************************************************************************************
*/
LOCAL_INLINE uint8 Com_Prv_SendDynSignalWithMetaData(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv,
                                                                 uint16 length_u16, const uint8 * metaDataPtr_pcu8)
{

    Com_TxIpduCfg_tpcst         txIpduConstPtr_pcst;
    Com_TxSigCfg_tpcst          txSigConstPtr_pcst;
    Com_TxIpduRam_tpst          txIpduRamPtr_pst;  /* Local pointer to Tx IPdu static configuration */
    Com_IpduId_tuo              idIpdu_uo;
    uint8                       status_u8;         /* Return status */

    status_u8           = E_OK;

    txSigConstPtr_pcst  = COM_GET_TXSIG_CONSTDATA(idSignal_u16);
    idIpdu_uo           = txSigConstPtr_pcst->idComIPdu_uo;
    txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);

    /* Check if signal is a part of metadata I-PDU */
    if (Com_GetValue(TXIPDU,_IS_METADATAPDU,txIpduConstPtr_pcst->txIPduFields_u16))
    {
        txIpduRamPtr_pst = txIpduConstPtr_pcst->ipduRam_pst;

        if (NULL_PTR != metaDataPtr_pcu8)
        {
            /* Lock the given TxIPdu buffer from updation in other API's */
            SchM_Enter_Com_TxIpduProtArea(TRANSMIT_METADATA);

            /* Copy the given MetaData into Tx metadata Buffer */
            Com_ByteCopy( (txIpduConstPtr_pcst->txMetaDataPtr_pcst->txMetaDataValue_pau8),
                          ((const uint8 * )metaDataPtr_pcu8),
                           txIpduConstPtr_pcst->txMetaDataPtr_pcst->txMetaDataLength_u8);

            /* Set flag, that is to be used in Com_Prv_SendIpdu to retain caller metadata */
            Com_SetRamValue(TXIPDU,_METADATA_REQUEST,txIpduRamPtr_pst->txFlags_u16,COM_TRUE);

            /* Release-Lock for the given TxIPdu buffer updation in other API's */
            SchM_Exit_Com_TxIpduProtArea(TRANSMIT_METADATA);
        }

        status_u8 = Com_Prv_SendDynSignal( idSignal_u16, signalDataPtr_pcv, length_u16);
    }

    return status_u8;
}
#endif /* #ifdef COM_TX_DYNAMIC_SIGNAL_SUPPORT */


#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_CopyDynGroupSignalWithMetaData
 Description      : Service updates the signal object identified by SignalId with the signal
                    referenced by the SignalDataPtr parameter and length of the dynamic length signal
 Parameter        : idSignal_u16        -> Id of the signal.
                  : signalDataPtr_pcv   -> The pointer to the address where the application data is available.
                  : length_u16          -> Length of the dynamic length signal
                  : metaDataPtr_pcu8    -> Pointer to the meta data of the signal
 Return value     : E_OK/E_NOT_OK
 **********************************************************************************************************************
*/
LOCAL_INLINE uint8 Com_Prv_CopyDynGroupSignalWithMetaData(Com_SignalIdType idSignal_u16,
                                                          const void * signalDataPtr_pcv,
                                                          uint16 length_u16, const uint8 * metaDataPtr_pcu8)
{

    /* Local pointer to hold the Tx ipdu status information */
    Com_TxIpduRam_tpst              txIpduRamPtr_pst;
    Com_TxIpduCfg_tpcst             txIpduConstPtr_pcst;
    Com_TxSigGrpCfg_tpcst           txSigGrpConstPtr_pcst;
    Com_TxGrpSigCfg_tpcst           txGrpSigConstPtr_pcst;
    Com_IpduId_tuo                  idIpdu_uo;
    uint8                           status_u8;

    txGrpSigConstPtr_pcst   = COM_GET_TXGRPSIG_CONSTDATA(idSignal_u16);
    txSigGrpConstPtr_pcst   = COM_GET_TXSIGGRP_CONSTDATA(txGrpSigConstPtr_pcst->idSigGrp_uo);
    idIpdu_uo               = txSigGrpConstPtr_pcst->idComIPdu_uo;
    txIpduConstPtr_pcst     = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);
    status_u8               = E_NOT_OK;

    /* Check if signal is a part of metadata I-PDU */
    if (Com_GetValue(TXIPDU,_IS_METADATAPDU,txIpduConstPtr_pcst->txIPduFields_u16))
    {
        txIpduRamPtr_pst = txIpduConstPtr_pcst->ipduRam_pst;

        if (NULL_PTR != metaDataPtr_pcu8)
        {
            /* Lock the given TxIPdu buffer from updation in other API's */
            SchM_Enter_Com_TxIpduProtArea(TRANSMIT_METADATA);

            /* Copy the given MetaData into Tx metadata Buffer */
            Com_ByteCopy( (txIpduConstPtr_pcst->txMetaDataPtr_pcst->txMetaDataValue_pau8),
                          ((const uint8 * )metaDataPtr_pcu8),
                           txIpduConstPtr_pcst->txMetaDataPtr_pcst->txMetaDataLength_u8);

            /* Set flag, that is to be used in Com_Prv_SendIpdu to retain caller metadata */
            Com_SetRamValue(TXIPDU,_METADATA_REQUEST,txIpduRamPtr_pst->txFlags_u16,COM_TRUE);

            /* Release-Lock for the given TxIPdu buffer updation in other API's */
            SchM_Exit_Com_TxIpduProtArea(TRANSMIT_METADATA);
        }

        status_u8 = Com_Prv_CopyDynGroupSignal(idSignal_u16, signalDataPtr_pcv, length_u16);
    }

    return status_u8;
}
#endif /* #ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* #if defined(COM_TX_DYNAMIC_SIGNAL_SUPPORT) || defined(COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT) */

#endif /* #ifdef COM_METADATA_SUPPORT */

