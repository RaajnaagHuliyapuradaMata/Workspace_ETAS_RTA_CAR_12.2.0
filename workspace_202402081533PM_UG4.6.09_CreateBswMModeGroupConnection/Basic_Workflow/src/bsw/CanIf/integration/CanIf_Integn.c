/*
* *********************************************************************************************************************
* This is a template file.
* It defines integration functions which are necessary to be implemented.
* The integrator must complete the templates before deploying software containing functions defined in this file.
* Once templates have been completed, the integrator should delete the "removal part"
* Note: The integrator is responsible for updates made to this file.
*
* To suppress following error message, define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT
* during compiler invocation (e.g. -DNOT_READY_FOR_TESTING_OR_DEPLOYMENT)
* The suppression of the error only allows the user to proceed with the building phase.
* *********************************************************************************************************************
*/

/*---- Removal start ----*/
/* Hint for the integrator: remove everything between Removal start and Removal end after proper implementation */

#ifndef NOT_READY_FOR_TESTING_OR_DEPLOYMENT
# error The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#else
# warning The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#endif /* NOT_READY_FOR_TESTING_OR_DEPLOYMENT */
/*---- Removal end ----*/

/* IMPORTANT: This template file is necessary only when 3rd party CanDriver or CanTrcvDriver need to be integrated with CanIf. */
/* In case of not using 3rd Party driver, the usage of CanIf_Integn.c_tpl and CanIf_Integn.h_tpl are not required. */

/* This file contains the integration code that allows the calling of a 3rd party driver that is based on older Autosar version like AR4.2.2 (adaptation of calls) */
/* Hint: Please update this file based on the underlying driver that need to be integrated. For example, if no switch is needed, then the API prototypes related to switch can be removed. */

#include "CanIf_Integn.h"
/* BSWEXT-637 */
#include "CanIf_Cbk.h"

#define CANIF_START_SEC_CODE
#include "CanIf_MemMap.h"

/* -------------------------------------- */
/* CanIf callbacks                        */
/* Can -> Integn -> CanIf                */
/* -------------------------------------- */

void CanIf_RxIndication(const Can_HwType * Mailbox,
        const PduInfoType * PduInfoPtr)
{
    CanIf_Integn_RxIndication(Mailbox,PduInfoPtr);
}

#if CANIF_CFG_TRCV_DRV_SUPPORT==STD_ON
void CanIf_TrcvModeIndication(uint8 TransceiverId,
        CanTrcv_TrcvModeType TransceiverMode)
{
    CanIf_Integn_TrcvModeIndication(TransceiverId,TransceiverMode);
}
#endif

#if(CANIF_PUBLIC_PN_SUPPORT == STD_ON)
void CanIf_ClearTrcvWufFlagIndication(uint8 TransceiverId)
{
    CanIf_Integn_ClearTrcvWufFlagIndication(TransceiverId);
}

void CanIf_CheckTrcvWakeFlagIndication(uint8 TransceiverId)
{
    CanIf_Integn_CheckTrcvWakeFlagIndication(TransceiverId);
}

void CanIf_ConfirmPnAvailability(uint8 TransceiverId)
{
    CanIf_Integn_ConfirmPnAvailability(TransceiverId);
}
#endif

void CanIf_TxConfirmation(PduIdType CanTxPduId)
{
    CanIf_Integn_TxConfirmation(CanTxPduId);
}

#if (CANIF_TRIGGERTRANSMIT_SUPPORT == STD_ON)
Std_ReturnType CanIf_TriggerTransmit(PduIdType TxPduId, PduInfoType* PduInfoPtr)
{
    return CanIf_Integn_TriggerTransmit(TxPduId,PduInfoPtr);
}
#endif

void CanIf_ControllerBusOff(uint8 ControllerId)
{
    CanIf_Integn_ControllerBusOff(ControllerId);
}

void CanIf_ControllerModeIndication(uint8 ControllerId, Can_ControllerStateType ControllerMode)
{
    CanIf_Integn_ControllerModeIndication(ControllerId,ControllerMode);
}

void CanIf_ControllerErrorPassive(uint8 ControllerId)
{
    CanIf_Integn_ControllerErrorPassive(ControllerId);
}

/* ------------------------------------*/
/* Can related APIs                    */
/* CanIf -> Integn -> Can Driver       */

/* Can Driver API calls ie CanIf -> Integn -> Can Driver */
#if (CANIF_SET_BAUDRATE_API == STD_ON)
Std_ReturnType Can_Integn_SetBaudrate(uint8 Controller_u8, uint16 BaudRateConfigID_u16)
{
    return Can_SetBaudrate(Controller_u8,BaudRateConfigID_u16);
}
#endif

Std_ReturnType Can_Integn_SetControllerMode (uint8 Controller_u8, Can_ControllerStateType Transition_en)
{
    return Can_SetControllerMode(Controller_u8, Transition_en);
}

Std_ReturnType Can_Integn_GetControllerMode(uint8 Controller, Can_ControllerStateType* ControllerModePtr)
{
    return Can_GetControllerMode(Controller, ControllerModePtr);
}

Std_ReturnType Can_Integn_Write (Can_HwHandleType Hth, const Can_PduType* PduInfo)
{
    return Can_Write(Hth, PduInfo);
}

#if (CANIF_GLOBAL_TIME_SUPPORT == STD_ON)
Std_ReturnType Can_Integn_GetCurrentTime(uint8 ControllerId, Can_TimeStampType* timeStampPtr)
{
    return Can_GetCurrentTime(ControllerId, timeStampPtr);
}

void Can_Integn_EnableEgressTimeStamp(Can_HwHandleType Hth)
{
    Can_EnableEgressTimeStamp(Hth);
}

Std_ReturnType Can_Integn_GetEgressTimeStamp(PduIdType TxPduId, Can_HwHandleType Hth, Can_TimeStampType* timeStampPtr)
{
    return Can_GetEgressTimeStamp(TxPduId, Hth, timeStampPtr);
}

Std_ReturnType Can_Integn_GetIngressTimeStamp(Can_HwHandleType Hrh, Can_TimeStampType* timeStampPtr)
{
    return Can_GetIngressTimeStamp(Hrh, timeStampPtr);
}
#endif

/* -------------------------------------- */
/* CanTrcv related APIs                    */
/* CanIf -> Integn -> CanTrcv             */
/* -------------------------------------- */

/* Call adaptation : convert arguments if required and forward call to third party driver module */

#if ((CANIF_CFG_TRCV_DRV_SUPPORT == STD_ON) && (THIRD_PARTY_CANTRCV_DRIVER_INTEGRATION == STD_ON))
Std_ReturnType CanTrcv_Integn_SetOpMode(uint8 Transceiver, CanTrcv_TrcvModeType OpMode)
{
    return CanTrcv_SetOpMode(Transceiver, OpMode);
}

Std_ReturnType CanTrcv_Integn_GetOpMode(uint8 Transceiver, CanTrcv_TrcvModeType * OpMode)
{
    return CanTrcv_GetOpMode(Transceiver, OpMode);
}

Std_ReturnType CanTrcv_Integn_GetBusWuReason(uint8 Transceiver, CanTrcv_TrcvWakeupReasonType * reason)
{
    return CanTrcv_GetBusWuReason(Transceiver, reason);
}

Std_ReturnType CanTrcv_Integn_SetWakeupMode(uint8 Transceiver, CanTrcv_TrcvWakeupModeType TrcvWakeupMode)
{
    return CanTrcv_SetWakeupMode(Transceiver, TrcvWakeupMode);
}

#if((CANIF_PUBLIC_PN_SUPPORT == STD_ON) && (CANIF_CFG_TRCV_PN_SUPPORT == STD_ON))
Std_ReturnType CanTrcv_Integn_ClearTrcvWufFlag(uint8 Transceiver)
{
    return CanTrcv_ClearTrcvWufFlag(Transceiver);
}
#endif

#if (CANIF_PUBLIC_PN_SUPPORT == STD_ON)
Std_ReturnType CanTrcv_Integn_CheckWakeFlag(uint8 Transceiver)
{
    return CanTrcv_CheckWakeFlag(Transceiver);
}
#endif

#endif

#define CANIF_STOP_SEC_CODE
#include "CanIf_MemMap.h"
