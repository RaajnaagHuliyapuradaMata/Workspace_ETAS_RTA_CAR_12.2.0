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

/* Com_SendSignalGroupArrayWithMetaData() API shall only be available when the configuration switch
 * ComEnableSignalGroupArrayApi and ComMetaDataSupport are enabled */
#if defined (COM_TX_SIGNALGROUP_ARRAY) && defined(COM_METADATA_SUPPORT)

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

# define COM_START_SEC_CODE
# include "Com_MemMap.h"

/*
 **********************************************************************************************************************
 Function name    : Com_SendSignalGroupArrayWithMetaData
 Description      : Service updates the signal object identified by SignalId with the signal
                    referenced by the SignalDataPtr parameter and meta data provided via metadataPtr parameter.
 Parameter        : idSignalGroup_u16        -> Id of the signal group.
                  : signalGroupArrayPtr_pcu8 -> The pointer to the address where the application data is available.
                  : metaDataPtr_pcu8         -> The pointer provides meta data
 Return value     : E_OK/COM_SERVICE_NOT_AVAILABLE/COM_BUSY
 **********************************************************************************************************************
*/
uint8 Com_SendSignalGroupArrayWithMetaData(Com_SignalGroupIdType idSignalGroup_u16,
                                           const uint8 * signalGroupArrayPtr_pcu8,
                                           const uint8 * metaDataPtr_pcu8)
{
    Com_TxIpduCfg_tpcst             txIpduConstPtr_pcst;
    Com_TxIpduRam_tpst              txIpduRamPtr_pst;
    Com_TxSigGrpCfg_tpcst           txSigGrpConstPtr_pcst;
    Com_TxSigGrpArrayCfg_tpcst      txSigGrpArrayConstPtr_pcst;
    Com_IpduId_tuo                  idIpdu_uo;
    uint8                           constxSignalGrpFields_u8;
# ifdef COM_EffectiveSigGrpTOC
    uint8                           transferProperty_u8;
# endif
    uint8                           returnValue_u8;

    returnValue_u8 = COM_SERVICE_NOT_AVAILABLE;

#if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SendSignalGroupArrayWithMetaData, COM_E_UNINIT);
    }
    else if (signalGroupArrayPtr_pcu8 == NULL_PTR)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SendSignalGroupArrayWithMetaData, COM_E_PARAM_POINTER);
    }
    else if (!Com_Prv_IsValidTxSignalGroupId(idSignalGroup_u16))
    {
        COM_DET_REPORT_ERROR(COMServiceId_SendSignalGroupArrayWithMetaData, COM_E_PARAM);
    }
    else
#endif /* end of COM_PRV_ERROR_HANDLING */
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idSignalGroup_u16           = COM_GET_TXSIGNALGRP_ID(idSignalGroup_u16);

        txSigGrpConstPtr_pcst       = COM_GET_TXSIGGRP_CONSTDATA(idSignalGroup_u16);
        constxSignalGrpFields_u8    = txSigGrpConstPtr_pcst->txSignalGrpFields_u8;
        idIpdu_uo                   = txSigGrpConstPtr_pcst->idComIPdu_uo;
        txIpduConstPtr_pcst         = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);
        txSigGrpArrayConstPtr_pcst  = COM_GET_TXSIGGRP_ARRAY_CONSTDATA(txSigGrpConstPtr_pcst->idxSigGrpArray_uo);

        /* Proceed only if Tx-SignalGroup is enabled with Array Access
        * and
        * if the given I-PDU supports MetaData
        */
        /* MR12 RULE 13.5 VIOLATION: Below conditional statement access RAM, use of intermediate variables
        * to store and read here, may read obsolete value. Evaluation(or no evaluation) shall not impact the
        * system behaviour. Hence suppressed. */
        if (Com_GetValue(TXIPDU,_IS_METADATAPDU,txIpduConstPtr_pcst->txIPduFields_u16) &&
           Com_GetValue(TXSIGGRP,_ARRAYACCESS,constxSignalGrpFields_u8))
        {
            txIpduRamPtr_pst = txIpduConstPtr_pcst->ipduRam_pst;
            /* Proceed only if Large Data tranmission is not in Progress */
# ifdef COM_TX_TP_IPDUTYPE
            if ( Com_GetRamValue(TXIPDU,_LARGEDATAINPROG, txIpduRamPtr_pst->txFlags_u16) )
            {
                returnValue_u8 = COM_BUSY;
            }
            else
# endif /*#ifdef COM_TX_TP_IPDUTYPE*/
            {

# ifdef COM_EffectiveSigGrpTOC
                transferProperty_u8 = Com_GetValue(TXSIGGRP,_TP,constxSignalGrpFields_u8);

                if (COM_SIG_TP_IS_TRIGGERED_ON_CHANGE(transferProperty_u8))
                {
                    /* Unpack & check, if any of the GroupSignal data has changed whose transfer property is
                    * configured with TRIGGERED_ON_CHANGE */
                    Com_Prv_InternalSigGrpArrayTrigOnChange(idSignalGroup_u16, signalGroupArrayPtr_pcu8);
                }
                else
                {
                    /* Do-nothing */
                }
# endif /* #  ifdef COM_EffectiveSigGrpTOC */

                SchM_Enter_Com_TxIpduProtArea(SENDSIGNALGRP);

                /* The service Com_SendSignalGroupArray() shall copy the content designated by the
                * signalGroupArrayPtr_pcu8 to the associated I-PDU Buffer */
                Com_ByteCopy( ( uint8 * )( txIpduConstPtr_pcst->buffPtr_pu8 +
                                                                        txSigGrpArrayConstPtr_pcst->firstByteNo_uo ),
                        ( const uint8 * )( signalGroupArrayPtr_pcu8 ),
                              ( uint32  )( txSigGrpArrayConstPtr_pcst->length_uo ) );

                SchM_Exit_Com_TxIpduProtArea(SENDSIGNALGRP);

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

                returnValue_u8 = Com_Prv_InternalSendSignalGroup( idSignalGroup_u16 );
            }
        }
    }
    return returnValue_u8;
}

# define COM_STOP_SEC_CODE
# include "Com_MemMap.h"

#endif /* #if defined (COM_TX_SIGNALGROUP_ARRAY) && defined(COM_METADATA_SUPPORT) */

