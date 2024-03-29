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
 Function name    : Com_ReceiveSignal
 Description      : Service for receiving the signal, This API copies the data to the application buffer and copy the
                    metadata into position specified by the MetaDataPtr parameter
 Parameter        : idSignal_u16        -> Id of the Signal
                  : signalDataPtr_pv    -> Pointer to the address where signal data needs to be copied.
                  : metaDataPtr_pu8     -> Pointer to the address where metadata data needs to be copied.
 Return value     : E_OK/COM_SERVICE_NOT_AVAILABLE
 **********************************************************************************************************************
*/
/* MR12 RULE 8.13 VIOLATION: A pointer parameter(signalDataPtr_pv) in a function prototype should be declared as
*  pointer to const if the pointer is not used to modify the addressed object. But AUTOSAR SWS specification signature
*  is as below, hence suppressed */
uint8 Com_ReceiveSignalWithMetaData(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv, uint8 * metaDataPtr_pu8)
{

    /* Local pointer holds the address of Rx signal static configuration */
    Com_RxSigCfg_tpcst          rxSigConstPtr_pcst;
    Com_RxIpduCfg_tpcst         rxIpduConstPtr_pcst;
#ifdef COM_RX_SIGNALGROUP
    Com_RxGrpSigCfg_tpcst       rxGrpSigConstPtr_pcst;
#endif
    Com_IpduId_tuo              idIpdu_uo;
    uint8                       status_u8;  /* Return status */
    /* Local variable holds the signal type */
    uint8                       sigType_u8;

    status_u8 = COM_SERVICE_NOT_AVAILABLE;

# if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReceiveSignalWithMetaData, COM_E_UNINIT);
    }
    else if ((signalDataPtr_pv == NULL_PTR) || (metaDataPtr_pu8 == NULL_PTR))
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReceiveSignalWithMetaData, COM_E_PARAM_POINTER);
    }
    else
# endif /* end of COM_PRV_ERROR_HANDLING */
    if (Com_Prv_IsValidRxSignalId(idSignal_u16))
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idSignal_u16        = COM_GET_RXSIGNAL_ID(idSignal_u16);

        rxSigConstPtr_pcst  = COM_GET_RXSIG_CONSTDATA(idSignal_u16);
        idIpdu_uo           = rxSigConstPtr_pcst->idComIPdu_uo;
        rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo);
        sigType_u8          = Com_GetValue(GEN,_TYPE,rxSigConstPtr_pcst->general_u8);

        /* If IPDU Group is Started, then update the status variable */
        if (Com_Prv_CheckRxIPduStatus((PduIdType)idIpdu_uo))
        {
            status_u8 = E_OK;
        }

        if ((sigType_u8 % COM_TWO) == COM_ZERO)
        {
            /* Copy Data of Unsigned Signal Types */
            Com_Prv_CopyData_Sig_UnsignedType(idSignal_u16, signalDataPtr_pv);
        }
        else
        {
            /* Copy Data of Signed Signal Types */
            Com_Prv_CopyData_Sig_SignedType(idSignal_u16, signalDataPtr_pv);
        }

        /* Check if received signal is part of metadata I-PDU */
        if (Com_GetValue(RXIPDU,_IS_METADATAPDU,rxIpduConstPtr_pcst->rxIPduFields_u8))
        {
            SchM_Enter_Com_RxIpduProtArea(RECEIVE_METADATA);

            /* Copy the given RxMetaData internal buffer to Metadata pointer */
            Com_ByteCopy(metaDataPtr_pu8,
                        (COM_GET_RXMETADATA(idIpdu_uo)->rxMetaDataValue_pau8),
                         COM_GET_RXMETADATA(idIpdu_uo)->rxMetaDataLength_u8);

            SchM_Exit_Com_RxIpduProtArea(RECEIVE_METADATA);
        }
    }
#ifdef COM_RX_SIGNALGROUP
    else if (Com_Prv_IsValidRxGroupSignalId(idSignal_u16))
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idSignal_u16    = COM_GET_RXGRPSIGNAL_ID(idSignal_u16);

        rxGrpSigConstPtr_pcst   = COM_GET_RXGRPSIG_CONSTDATA(idSignal_u16);
        idIpdu_uo               = COM_GET_RXSIGGRP_CONSTDATA(rxGrpSigConstPtr_pcst->idSigGrp_uo)->idComIPdu_uo;
        rxIpduConstPtr_pcst     = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo);

        /* Check if received signal group is part of metadata I-PDU */
        if (Com_GetValue(RXIPDU,_IS_METADATAPDU,rxIpduConstPtr_pcst->rxIPduFields_u8))
        {
            SchM_Enter_Com_RxIpduProtArea(RECEIVE_METADATA);

            /* Copy the given RxMetaData internal buffer to Metadata pointer */
            Com_ByteCopy(metaDataPtr_pu8,
                        (COM_GET_RXMETADATA(idIpdu_uo)->rxMetaDataValue_pau8),
                         COM_GET_RXMETADATA(idIpdu_uo)->rxMetaDataLength_u8);

            SchM_Exit_Com_RxIpduProtArea(RECEIVE_METADATA);
        }

        Com_Prv_ReceiveShadowSignal(idSignal_u16, signalDataPtr_pv);
        status_u8 = E_OK;
    }
#endif
    else
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReceiveSignalWithMetaData, COM_E_PARAM);
    }

    return(status_u8);
}

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* #ifdef COM_METADATA_SUPPORT */

