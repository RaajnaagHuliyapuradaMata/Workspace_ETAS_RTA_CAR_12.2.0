/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2017, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef CANIF_CBK_H
#define CANIF_CBK_H

/***************************************************************************************************
**                      Include Section                                                           **
***************************************************************************************************/

#include "CanIf_Cfg.h"

#if (THIRD_PARTY_CANTRCV_DRIVER_INTEGRATION == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON)
#include "CanIf_Integn.h"
#endif

/***************************************************************************************************
**                                   Global Callback Function Prototypes                          **
***************************************************************************************************/


#define CANIF_START_SEC_CODE
#include "CanIf_MemMap.h"

/* Declaration of CanIf_RxIndication() API */
extern void CANIF_RXINDICATION_FNAME(const Can_HwType * Mailbox,
        const PduInfoType * PduInfoPtr);



/* Declaration of CanIf_TrcvModeIndication() API */
#if CANIF_CFG_TRCV_DRV_SUPPORT==STD_ON
extern void CANIF_TRCVMODEINDICATION_FNAME( uint8 TransceiverId,
        CanTrcv_TrcvModeType TransceiverMode);
#endif

/* Declaration of CanIf_ClearTrcvWufFlagIndication() API */
#if(CANIF_PUBLIC_PN_SUPPORT == STD_ON)
void CANIF_CLEARTRCVWUFFLAGINDICATION_FNAME( uint8 TransceiverId);

/* Declaration of CanIf_CheckTrcvWakeFlagIndication() API */
void CANIF_CHECKTRCVWAKEFLAGINDICATION_FNAME( uint8 TransceiverId);

/* Declaration of CanIf_ConfirmPnAvailability() API */
void CANIF_CONFIRMPNAVAILABILITY_FNAME( uint8 TransceiverId);
#endif

/* Declaration of CanIf_TxConfirmation() API */
extern void CANIF_TXCONFIRMATION_FNAME(PduIdType CanTxPduId );

/* Declaration of CanIf_TriggerTransmit() API */
extern Std_ReturnType CANIF_TRIGGERTRANSMIT_FNAME(PduIdType TxPduId, PduInfoType* PduInfoPtr);

/* Declaration of CanIf_ControllerBusOff() API */
extern void CANIF_CONTROLLERBUSOFF_FNAME(uint8 ControllerId );

/* Declaration of CanIf_ControllerModeIndication() API */
extern void CANIF_CONTROLLERMODEINDICATION_FNAME( uint8 ControllerId, Can_ControllerStateType ControllerMode);

/* Declaration of CanIf_ControllerErrorPassive() API */
extern void CANIF_CONTROLLERERRORPASSIVE_FNAME( uint8 ControllerId);
#define CANIF_STOP_SEC_CODE
#include "CanIf_MemMap.h"

#endif   /* CANIF_CBK_H */


