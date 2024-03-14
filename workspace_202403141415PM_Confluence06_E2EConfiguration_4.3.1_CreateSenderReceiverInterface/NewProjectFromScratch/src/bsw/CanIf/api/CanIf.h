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

#ifndef CANIF_H
#define CANIF_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

/* Type definitions of CAN Interface */
#include "CanIf_Types.h"

/* Global Callback Function Prototypes of CAN Interface */
#include "CanIf_Can.h"

/* CanIf configuration header file */
#include "CanIf_Cfg.h"

#if(CANIF_CONFIGURATION_VARIANT == CANIF_CFG_VAR_PBS)
/* CanIf configuration header file */
#include "CanIf_PBcfg.h"
#endif

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/




/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/



/*This hold the current config set details*/
#define CANIF_START_SEC_VAR_FAST_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"
extern const CanIf_ConfigType * CanIf_Prv_ConfigSet_tpst;
#define CANIF_STOP_SEC_VAR_FAST_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"


/* Declaration of CanIf_Init() API */
#define CANIF_START_SEC_CODE
#include "CanIf_MemMap.h"

extern void CanIf_Init(const CanIf_ConfigType * ConfigPtr);




/* Declaration of CanIf_SetControllerMode() API */
extern Std_ReturnType CanIf_SetControllerMode(uint8 ControllerId, Can_ControllerStateType ControllerMode);


#if (CANIF_LITE_CONFIGURATION == STD_OFF)
/* Declaration of CanIf_GetControllerMode() API */
extern Std_ReturnType CanIf_GetControllerMode(uint8 ControllerId, Can_ControllerStateType * ControllerModePtr);
#endif

/* Declaration of CanIf_Transmit() API */
extern Std_ReturnType CanIf_Transmit(PduIdType CanIfTxSduId,
        const PduInfoType * CanIfTxInfoPtr);


/* Declaration of CanIf_CancelTransmit() API */
#if (STD_ON == CANIF_PUBLIC_CANCEL_TRANSMIT_SUPPORT)
extern Std_ReturnType CanIf_CancelTransmit(PduIdType CanIfTxSduId);
#endif

/* Declaration of CanIf_SetPduMode() API */
extern Std_ReturnType CanIf_SetPduMode(uint8 ControllerId,
        CanIf_PduModeType PduModeRequest);

/* Declaration of CanIf_DeInit() API */
extern void CanIf_DeInit(const CanIf_ConfigType * ConfigPtr);

/* Declaration of CanIf_GetPduMode() API */
extern Std_ReturnType CanIf_GetPduMode(
                                                         uint8 ControllerId,
                                                         CanIf_PduModeType * PduModePtr
                                                        );

#if ((CANIF_PUBLIC_READRXPDU_DATA_API == STD_ON) && (CANIF_CFG_READRXPDU_DATA_IN_RXPDUS == STD_ON) && (CANIF_CFG_RX_FEATURE_ENABLED== STD_ON))
/* Declaration of CanIf_ReadRxPduData() API */
extern Std_ReturnType CanIf_ReadRxPduData(PduIdType CanIfRxSduId,
                                                        PduInfoType * CanIfRxInfoPtr);
#endif


#if ((CANIF_READTXPDU_NOTIFY_STATUS_API != STD_OFF) && (CANIF_CFG_TX_FEATURE_ENABLED== STD_ON))
/* Declaration of CanIf_ReadTxNotifStatus() API */
extern CanIf_NotifStatusType CanIf_ReadTxNotifStatus(
                                                                      PduIdType CanIfTxSduId
                                                                      );
#endif

#if ((CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API != STD_OFF) && (CANIF_CFG_RX_FEATURE_ENABLED== STD_ON))
/* Declaration of CanIf_ReadRxNotifStatus() API */
extern CanIf_NotifStatusType CanIf_ReadRxNotifStatus(PduIdType CanIfRxSduId);
#endif

#if (CANIF_VERSIONINFO_API != STD_OFF)
/* Declaration of CanIf_GetVersionInfo() API */
extern void CanIf_GetVersionInfo (Std_VersionInfoType * VersionInfo);
#endif

#if (CANIF_PUBLIC_SETDYNAMICTXID_API != STD_OFF)
/* Declaration of CanIf_SetDynamicTxId() API */
extern void CanIf_SetDynamicTxId(
                                                  PduIdType CanTxPduId,
                                                  Can_IdType CanId
                                                   );
#endif

/* Declaration of CanIf_ResetDynamicTxId() API */
extern void CanIf_ResetDynamicTxId(PduIdType CanTxPduId);

#if ((CANIF_WAKEUP_SUPPORT != STD_OFF)&&(CANIF_CFG_TRCV_WAKEUP_SUPPORT == STD_ON))
/* Declaration of CanIf_CheckWakeup() API */
extern Std_ReturnType CanIf_CheckWakeup (EcuM_WakeupSourceType WakeupSource);
#endif

#if ((CANIF_CFG_TRCV_WAKEUP_SUPPORT == STD_ON) && (CANIF_PUBLIC_WAKEUP_CHECK_VALIDATION_SUPPORT == STD_ON))
/* Declaration of CanIf_CheckValidation() API */
extern Std_ReturnType CanIf_CheckValidation (EcuM_WakeupSourceType WakeupSource);
#endif

/* Declaration of CanIf_SetTrcvMode() API */
#if CANIF_CFG_TRCV_DRV_SUPPORT ==STD_ON
extern Std_ReturnType CanIf_SetTrcvMode(
                                            uint8 TransceiverId,
                                            CanTrcv_TrcvModeType TransceiverMode
                                                          );
#endif

#if CANIF_CFG_TRCV_DRV_SUPPORT ==STD_ON
/* Declaration of CanIf_GetTrcvMode() API */
extern Std_ReturnType CanIf_GetTrcvMode(
                                            CanTrcv_TrcvModeType * TransceiverModePtr,
                                            uint8 TransceiverId
                                                           );
#endif

#if CANIF_CFG_TRCV_DRV_SUPPORT!=STD_OFF
/* Declaration of CanIf_GetTrcvWakeupReason() API */
extern Std_ReturnType CanIf_GetTrcvWakeupReason(
                                            uint8 TransceiverId,
                                            CanTrcv_TrcvWakeupReasonType * TrcvWuReasonPtr
                                                                 );
/* Declaration of CanIf_SetTrcvWakeupMode() API */
extern Std_ReturnType CanIf_SetTrcvWakeupMode(
                                                uint8 TransceiverId,
                                                CanTrcv_TrcvWakeupModeType TrcvWakeupMode
);
#endif

#if(CANIF_CFG_RX_FEATURE_ENABLED== STD_ON)
/* Declaration of CanIf_ReturnRxPduId() API */
extern Std_ReturnType CanIf_ReturnRxPduId(
                                                     PduIdType CanIfRxTargetPduId,
                                                    PduIdType * CanIfRxPduIdPtr
                                                    );
#endif

#if(CANIF_CFG_TX_FEATURE_ENABLED== STD_ON)
/* Declaration of CanIf_ReturnTxPduId() API */
extern Std_ReturnType CanIf_ReturnTxPduId(
                                                     PduIdType CanIfTxTargetPduId,
                                                    PduIdType * CanIfTxPduIdPtr
                                                    );
#endif



#if (CANIF_SET_BAUDRATE_API == STD_ON)
/* Declaration of CanIf_SetBaudrate() API */
extern Std_ReturnType CanIf_SetBaudrate(
                                                uint8 ControllerId,
                                                const uint16 BaudRateConfigID
                                                );
#endif


#if (CANIF_PUBLIC_TXCONFIRM_POLLING_SUPPORT != STD_OFF)

/* Declaration of CanIf_GetTxConfirmationState() API */
extern CanIf_NotifStatusType CanIf_GetTxConfirmationState(
                                       uint8 ControllerId
                                               );
#endif


#if((CANIF_PUBLIC_PN_SUPPORT == STD_ON) && (CANIF_CFG_TRCV_DRV_SUPPORT==STD_ON))

/* Declaration of CanIf_ClearTrcvWufFlag() API */
extern Std_ReturnType CanIf_ClearTrcvWufFlag(
                                              uint8 TransceiverId
                                                               );
/* Declaration of CanIf_CheckTrcvWakeFlag() API */
extern Std_ReturnType CanIf_CheckTrcvWakeFlag(
                                              uint8 TransceiverId
                                                               );
#endif

#if ((CANIF_RB_READMSGID_API == STD_ON) && (CANIF_CFG_TX_FEATURE_ENABLED== STD_ON))
/* Declaration of CanIf_ReadTxMsgId() API */
extern uint32 CanIf_ReadTxMsgId(
                                                  uint16 msgHandleTx_u16
                                                 );
#endif

#if ((CANIF_RB_READMSGID_API == STD_ON) && (CANIF_CFG_RX_FEATURE_ENABLED== STD_ON))
/* Declaration of CanIf_ReadRxMsgId() API */
extern uint32 CanIf_ReadRxMsgId(uint16 msgHandleRx_u16);
#endif

#if(CANIF_CFG_TX_FEATURE_ENABLED== STD_ON)
/* Declaration of CanIf_Rb_ReadTxPduCanId() API */
extern Std_ReturnType CanIf_Rb_ReadTxPduCanId(
                                                    PduIdType CanIfTxPduId,
                                                    uint32 * TxPduCanId);
#endif

#if(CANIF_CFG_RX_FEATURE_ENABLED== STD_ON)
/* Declaration of CanIf_Rb_ReadRxPduCanId() API */
extern Std_ReturnType CanIf_Rb_ReadRxPduCanId( PduIdType CanIfRxPduId,
        uint32 * RxPduCanId);
#endif
#if ((CANIF_RB_CHANGERXPDUID_API != STD_OFF) && (CANIF_CFG_RX_FEATURE_ENABLED== STD_ON))
/* Declaration of CanIf_Rb_ChangeRxPduCanId() API */
extern Std_ReturnType CanIf_Rb_ChangeRxPduCanId(PduIdType CanRxPduId, Can_IdType CanId);
#endif

#define CANIF_STOP_SEC_CODE
#include "CanIf_MemMap.h"

/* Declaration of CanIf_Get_DirectHw_Info() API */
#if ((CANIF_DIRECT_HW_WRITE == STD_ON)&&(CANIF_CFG_TX_FEATURE_ENABLED== STD_ON))
#define CANIF_START_SEC_CODE_FAST
#include "CanIf_MemMap.h"
extern Std_ReturnType CanIf_Get_DirectHw_Info (PduIdType CanTxPduId,
        rba_CanDirectHWInfo_o * hw_info_po);

/* Declaration of CanIf_DirectHw_Write() API */
extern Std_ReturnType CanIf_DirectHw_Write(const uint32 * p_tx_data_buff_u32
        , uint32 data_len_u32, const rba_CanDirectHWInfo_o * hw_info_po);

#define CANIF_STOP_SEC_CODE_FAST
#include "CanIf_MemMap.h"
#endif

#if (CANIF_GLOBAL_TIME_SUPPORT == STD_ON)
#define CANIF_START_SEC_CODE
#include "CanIf_MemMap.h"

/* Declaration of CanIf_GetCurrentTime() API */
extern Std_ReturnType CanIf_GetCurrentTime(uint8 ControllerId, Can_TimeStampType* timeStampPtr);

/* Declaration of CanIf_EnableEgressTimeStamp() API */
extern void CanIf_EnableEgressTimeStamp(PduIdType TxPduId);

/* Declaration of CanIf_GetEgressTimeStamp() API */
extern Std_ReturnType CanIf_GetEgressTimeStamp(PduIdType TxPduId,Can_TimeStampType* timeStampPtr);

/* Declaration of CanIf_GetIngressTimeStamp() API */
extern Std_ReturnType CanIf_GetIngressTimeStamp (PduIdType RxPduId, Can_TimeStampType* timeStampPtr);

#define CANIF_STOP_SEC_CODE
#include "CanIf_MemMap.h"
#endif
/*
 **********************************************************************************************************************
 * Global Variables                                                                                                   *
 **********************************************************************************************************************
*/

#endif   /* CANIF_H */
