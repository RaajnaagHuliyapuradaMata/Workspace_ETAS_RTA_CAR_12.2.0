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

#if (defined(COM_RX_SIGNALGROUP) && defined(COM_METADATA_SUPPORT))

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
 Function name    : Com_ReceiveSignalGroupWithMetaData
 Description      : Service used to copy the SignalGroup content from IpduBuffer to Shadowbuffer and copy the metadata
                    into position specified by the MetaDataPtr parameter
 Parameter        : idSignalGroup_u16 -> Id of the SignalGroup
                  : metaDataPtr_pu8   -> Pointer to the address where metadata data needs to be copied.
 Return value     : E_OK/COM_SERVICE_NOT_AVAILABLE
 **********************************************************************************************************************
*/
/* MR12 RULE 8.13 VIOLATION: A pointer parameter(signalDataPtr_pv) in a function prototype should be declared as
* pointer to const if the pointer is not used to modify the addressed object. But AUTOSAR SWS specification signature
* is as below, hence suppressed */
uint8 Com_ReceiveSignalGroupWithMetaData(Com_SignalGroupIdType idSignalGroup_u16, uint8* metaDataPtr_pu8)
{

    Com_RxSigGrpCfg_tpcst       rxSigGrpConstPtr_pcst;
    Com_RxIpduCfg_tpcst         rxIpduConstPtr_pcst;
    Com_IpduId_tuo              idIpdu_uo;
    uint8                       status_u8;

    status_u8 = COM_SERVICE_NOT_AVAILABLE;

# if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReceiveSignalGroupWithMetaData, COM_E_UNINIT);
    }
    else if (metaDataPtr_pu8 == NULL_PTR)
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReceiveSignalGroupWithMetaData, COM_E_PARAM_POINTER);
    }
    else if (!Com_Prv_IsValidRxSignalGroupId(idSignalGroup_u16))
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReceiveSignalGroupWithMetaData, COM_E_PARAM);
    }
    else
# endif /* end of COM_PRV_ERROR_HANDLING */
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idSignalGroup_u16     = COM_GET_RXSIGNALGRP_ID(idSignalGroup_u16);

        rxSigGrpConstPtr_pcst = COM_GET_RXSIGGRP_CONSTDATA(idSignalGroup_u16);
        idIpdu_uo             = rxSigGrpConstPtr_pcst->idComIPdu_uo;
        rxIpduConstPtr_pcst   = COM_GET_RX_IPDU_CONSTDATA(rxSigGrpConstPtr_pcst->idComIPdu_uo);

        /* If IPDU Group is Started, then update the status variable */
        if (Com_Prv_CheckRxIPduStatus((PduIdType)idIpdu_uo))
        {
            status_u8 = E_OK;
        }

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

        Com_Prv_ReceiveSignalGroup(idSignalGroup_u16);

    }
    return (status_u8);
}

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* #if (defined(COM_RX_SIGNALGROUP) && defined(COM_METADATA_SUPPORT)) */

