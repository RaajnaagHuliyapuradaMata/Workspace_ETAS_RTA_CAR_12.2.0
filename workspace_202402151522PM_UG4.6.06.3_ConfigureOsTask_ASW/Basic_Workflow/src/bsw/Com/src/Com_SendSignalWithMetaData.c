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
 Function name    : Com_SendSignalWithMetaData
 Description      : Service updates the signal object identified by SignalId with the signal
                    referenced by the SignalDataPtr parameter and meta data provided via metadataPtr parameter.
 Parameter        : idSignal_u16      -> Id of the signal.
                  : signalDataPtr_pcv -> The pointer to the address where the application data is available.
                  : metaDataPtr_pcu8  -> The pointer provides meta data
 Return value     : E_OK/COM_SERVICE_NOT_AVAILABLE/COM_BUSY
 **********************************************************************************************************************
*/
uint8 Com_SendSignalWithMetaData(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv,
                                                                const uint8 * metaDataPtr_pcu8)
{
    Com_TxIpduCfg_tpcst         txIpduConstPtr_pcst;
    Com_TxSigCfg_tpcst          txSigConstPtr_pcst;
    Com_TxIpduRam_tpst          txIpduRamPtr_pst;  /* Local pointer to Tx IPdu static configuration */
#ifdef COM_TX_SIGNALGROUP
    Com_TxGrpSigCfg_tpcst       txGrpSigConstPtr_pcst;
    Com_TxSigGrpCfg_tpcst       txSigGrpConstPtr_pcst;
#endif
    Com_IpduId_tuo              idIpdu_uo;
    uint8                       status_u8;        /* Return status */

    status_u8 = COM_SERVICE_NOT_AVAILABLE;

#if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SendSignalWithMetaData, COM_E_UNINIT);
    }
    else if (signalDataPtr_pcv == NULL_PTR)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SendSignalWithMetaData, COM_E_PARAM_POINTER);
    }
    else
#endif /* end of COM_PRV_ERROR_HANDLING */
    if (Com_Prv_IsValidTxSignalId(idSignal_u16))
    {
        /* Convert external HandleId to internal HandleId for Tx Signal
        * IMP : before accessing the tables, it is required to convert the IDs
        * For pre-compile , conversion is not required. Access Macro will return the same ID,
        * which is passed to it */
        idSignal_u16        = COM_GET_TXSIGNAL_ID(idSignal_u16);

        txSigConstPtr_pcst  = COM_GET_TXSIG_CONSTDATA(idSignal_u16);
        idIpdu_uo           = txSigConstPtr_pcst->idComIPdu_uo;
        txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);

        /* Check if signal is a part of metadata I-PDU and if the given I-PDU supports MetaData */
        if (Com_GetValue(TXIPDU,_IS_METADATAPDU,txIpduConstPtr_pcst->txIPduFields_u16))
        {
            txIpduRamPtr_pst = txIpduConstPtr_pcst->ipduRam_pst;

            /* Proceed only if Large Data tranmission is not in Progress */
#ifdef COM_TX_TP_IPDUTYPE
            if (Com_GetRamValue(TXIPDU,_LARGEDATAINPROG,txIpduRamPtr_pst->txFlags_u16))
            {
                /* The TxIPdu transmission of large Data IPdu is in progress, hence no signal update is allowed.
                * Until the transmission is completed. */
                 status_u8 = COM_BUSY;
            }
            else
#endif /*#ifdef COM_TX_TP_IPDUTYPE*/
            {
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

                status_u8 = Com_Prv_InternalSendSignal(idSignal_u16,signalDataPtr_pcv);
            }
        }
    }
#ifdef COM_TX_SIGNALGROUP
    else if (Com_Prv_IsValidTxGroupSignalId(idSignal_u16))
    {
        /* Convert external HandleId to internal HandleId for Tx Signal
        * IMP : before accessing the tables, it is required to convert the IDs
        * For pre-compile , conversion is not required. Access Macro will return the same ID,
        * which is passed to it */
        idSignal_u16    = COM_GET_TXGRPSIGNAL_ID(idSignal_u16);

        txGrpSigConstPtr_pcst   = COM_GET_TXGRPSIG_CONSTDATA(idSignal_u16);
        txSigGrpConstPtr_pcst   = COM_GET_TXSIGGRP_CONSTDATA(txGrpSigConstPtr_pcst->idSigGrp_uo);
        idIpdu_uo               = txSigGrpConstPtr_pcst->idComIPdu_uo;
        txIpduConstPtr_pcst     = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);

        /* Check if signal is a part of metadata I-PDU and if the given I-PDU supports MetaData */
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
        }

        Com_Prv_UpdateShadowSignal(idSignal_u16, signalDataPtr_pcv);
        status_u8       = E_OK;
    }
#endif /* #ifdef COM_TX_SIGNALGROUP */
    else
    {
        COM_DET_REPORT_ERROR(COMServiceId_SendSignalWithMetaData, COM_E_PARAM);
    }

    return status_u8;
}

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* #ifdef COM_METADATA_SUPPORT */

