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

#ifndef COM_PRV_H
#define COM_PRV_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Com.h"
#include "Com_Cfg_Internal.h"
#include "Com_Cfg_SchM.h"
#include "Com_Prv_Types.h"
#include "rba_BswSrv.h"

#if (COM_CONFIGURATION_VARIANT == COM_VARIANT_PRE_COMPILE)
#include "Com_PBcfg_Common.h"
#include "Com_PBcfg_Variant.h"
#endif

#if (COM_CONFIGURATION_USE_DET == STD_ON) || defined(COM_TX_TP_IPDUTYPE)
#include "Det.h"
#if(COM_ENABLE_INTER_MODULE_CHECKS)
/* [SWS_BSW_00036] Inter Module Checks */
# if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != COM_AR_RELEASE_MAJOR_VERSION))
#  error "AUTOSAR major version undefined or mismatched"
# endif
# if (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != COM_AR_RELEASE_MINOR_VERSION))
#  error "AUTOSAR minor version undefined or mismatched"
# endif
#endif /* #if(COM_ENABLE_INTER_MODULE_CHECKS) */
#endif /* #if (COM_CONFIGURATION_USE_DET == STD_ON) || defined(COM_TX_TP_IPDUTYPE) */

#ifdef COM_ARSRVLIB_SUPPORT
#include "Bfx.h"
#if(COM_ENABLE_INTER_MODULE_CHECKS)
/* [SWS_BSW_00036] Inter Module Checks */
# if (!defined(BFX_AR_RELEASE_MAJOR_VERSION) || (BFX_AR_RELEASE_MAJOR_VERSION != COM_AR_RELEASE_MAJOR_VERSION))
#  error "AUTOSAR major version undefined or mismatched"
# endif
# if (!defined(BFX_AR_RELEASE_MINOR_VERSION) || (BFX_AR_RELEASE_MINOR_VERSION != COM_AR_RELEASE_MINOR_VERSION))
#  error "AUTOSAR minor version undefined or mismatched"
# endif
#endif /* #if(COM_ENABLE_INTER_MODULE_CHECKS) */
#endif /* #ifndef COM_ARSRVLIB_SUPPORT */

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/
/**
 * @ingroup COM_PRV_H
 * Macro to convert boolean to Unsigned value
 */
#define COM_BOOL_TO_UNSIGNED(BoolValue)                 ((BoolValue) ? (COM_SET) : (COM_RESET))
/**
 * @ingroup COM_PRV_H
 * Macro to convert unsigned to bool value
 */
#define COM_UNSIGNED_TO_BOOL(unsignedValue)             (((unsignedValue) != COM_RESET) ? (COM_TRUE) : (COM_FALSE))

#define COM_PRV_NOT_FROM_IPDU_GRP_START                 (COM_TRUE)
#define COM_PRV_FROM_IPDU_GRP_START                     (COM_FALSE)

/**
 * @ingroup COM_PRV_H
 * Macro that returns size of Float-32 in bytes
 */
#define COM_F32_SIZE_IN_BYTES                           (4u)
/**
 * @ingroup COM_PRV_H
 * Macro that returns size of Float-64 in bytes
 */
#define COM_F64_SIZE_IN_BYTES                           (8u)

/**
 * @ingroup COM_PRV_H
 *
 *  Endianes type: Little endian
 */
#define COM_LITTLEENDIAN                                (0u)
/**
 * @ingroup COM_PRV_H
 *
 *  Endianes type: Big Endian
 */
#define COM_BIGENDIAN                                   (1u)
/**
 * @ingroup COM_PRV_H
 *
 * Endianes type:  Opaque
 */
#define COM_OPAQUE                                      (0u)

/**
 * @ingroup COM_PRV_H
 *
 * Ipdu processing: Immediate
 */
#define COM_IMMEDIATE                                   (FALSE)
/**
 * @ingroup COM_PRV_H
 *
 * Ipdu processing:  Deffered
 */
#define COM_DEFERRED                                    (TRUE)

/**
 * @ingroup COM_PRV_H
 *
 * Signal Property: Triggered on Change
 */
#define COM_TRIGGERED_ON_CHANGE                         (0u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Property: Triggered on Change Without Repetition
 */
#define COM_TRIGGERED_ON_CHANGE_WITHOUT_REPETITION      (1u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Property: Pending
 */
#define COM_PENDING                                     (2u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Property: Triggered Without Repetition
 */
#define COM_TRIGGERED_WITHOUT_REPETITION                (3u)
/**
 * @ingroup COM_PRV_H
 *
 *  Signal Property: Triggered
 */
#define COM_TRIGGERED                                   (4u)

/* Returns true only if signal transfer property is configured as
 * COM_TRIGGERED or
 * COM_TRIGGERED_WITHOUT_REPETITION */
#define COM_SIG_TP_IS_TRIGGERED(TransProp)              ((TransProp) > COM_PENDING)

/* Returns true only if signal transfer property is configured as
 * COM_TRIGGERED_ON_CHANGE or
 * COM_TRIGGERED_ON_CHANGE_WITHOUT_REPETITION */
#define COM_SIG_TP_IS_TRIGGERED_ON_CHANGE(TransProp)    ((TransProp) < COM_PENDING)

/* Returns true only if signal transfer property is configured as
 * COM_TRIGGERED_WITHOUT_REPETITION or
 * COM_TRIGGERED_ON_CHANGE_WITHOUT_REPETITION */
#define COM_SIG_TP_IS_WITHOUT_REPETITION(TransProp)     (((TransProp) & 0x1u) != 0u)

/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status RESET
 */
#define COM_RESET                                       (0u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status SET
 */
#define COM_SET                                         (1u)

/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status value DISCARD, for signal group
 * Action taken : Received Signal group data will be discarded
 */
#define COM_SIGGRP_DISCARD                              (0u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status value ACCEPT, for signal group
 * Action taken : Received Signal group data will be stored into internal buffer
 */
#define COM_SIGGRP_ACCEPT                               (1u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status value REPLACE_TO_INIT, for signal group
 * Action taken : The Signal-group or group-signal buffer is filled with init value
 */
#define COM_SIGGRP_REPLACE_TO_INIT                      (2u)

/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status STOP
 */
#define COM_STOP                                        (FALSE)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status START
 */
#define COM_START                                       (TRUE)

/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status ACCEPT
 */
#define COM_ACCEPT                                      (TRUE)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status REJECT
 */
#define COM_REJECT                                      (FALSE)

/* Defines are arranged to optimise runtime during checks */
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Tx Ipdu mode status DIRECT
 */
#define COM_TXMODE_DIRECT                               (0x00u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Tx Ipdu mode status MIXED
 */
#define COM_TXMODE_MIXED                                (0x01u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Tx Ipdu mode status NONE
 */
#define COM_TXMODE_NONE                                 (0x02u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Tx Ipdu mode status PERIODIC
 */
#define COM_TXMODE_PERIODIC                             (0x03u)

/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Clear Update bit status - TRANSMIT
 */
#define COM_CLRUPDATEBIT_TRANSMIT                       (1u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Clear Update bit status - TRIGGERTRANSMIT
 */
#define COM_CLRUPDATEBIT_TRIGGERTRANSMIT                (2u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Clear Update bit status - CONFIRMATION
 */
#define COM_CLRUPDATEBIT_CONFIRMATION                   (3u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Clear Update bit status - NOT SET
 */
#define COM_CLRUPDATEBIT_NONE                           (0u)

/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status - TRUE
 */
#define COM_TRUE                                        (TRUE)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status - FALSE
 */
#define COM_FALSE                                       (FALSE)

/**
 * @ingroup COM_PRV_H
 *
 *  Invalid value/Timeout actions: None
 */
#define COM_NONE                                        (0u)
/**
 * @ingroup COM_PRV_H
 *
 * Invalid value/Timeout actions: Replace
 */
#define COM_REPLACE                                     (1u)
/**
 * @ingroup COM_PRV_H
 *
 * Invalid value actions:  Notify
 */
#define COM_NOTIFY                                      (2u)
/**
 * @ingroup COM_PRV_H
 *
 * Timeout actions: Substitute
 */
#define COM_SUBSTITUTE                                  (2u)

/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent Flag status - Bit is not used/Unused Index
 */
#define COM_NOT_USED                                    (0u)


#define COM_MAXIMUM_GWSYNC_REPITIONS                    (2u)

/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code instead of 0u
 */
#define COM_ZERO                                        (0u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code instead of 1u
 */
#define COM_ONE                                         (1u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code with Modulus operator
 */
#define COM_TWO                                         (2u)


/* START: Signal Type */

/*
 * VERY IMPORTANT : Do not change the following hash defines.
 * These values are used in the code, to optimise the handling.
 * 1. All the odd values are used by signed types
 */
/**
 * @ingroup COM_PRV_H
 *
 *  Signal Type UINT8
 */
#define COM_UINT8                                       (0x00u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Type SINT8
 */
#define COM_SINT8                                       (0x01u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Type UINT16
 */
#define COM_UINT16                                      (0x02u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Type SINT16
 */
#define COM_SINT16                                      (0x03u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Type UINT32
 */
#define COM_UINT32                                      (0x04u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Type SINT32
 */
#define COM_SINT32                                      (0x05u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Type UINT64
 */
#define COM_UINT64                                      (0x10u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Type SINT64
 */
#define COM_SINT64                                      (0x11u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Type BOOLEAN
 */
#define COM_BOOLEAN                                     (0x06u)

/**
 * @ingroup COM_PRV_H
 *
 * NOTE : Signal Type COM_UNDEFTYPE, All the signed data types have '1' as LSB.
 * All the signed data types have special operation based on above check, Be care full in Adding a new Data type.
 */
#define COM_UNDEFTYPE                                   (0x07)

/**
 * @ingroup COM_PRV_H
 *
 * Signal Type UINT8_N
 */
#define COM_UINT8_N                                     (0x08u)
/**
 * @ingroup COM_PRV_H
 *
 * Signal Type COM_UINT8_DYN
 */
#define COM_UINT8_DYN                                   (0x0Au)
/* FC_VariationPoint_START */
/**
 * @ingroup COM_PRV_H
 *
 * Signal Type COM_FLOAT32
 */
#define COM_FLOAT32                                     (0x0Cu)

/**
 * @ingroup COM_PRV_H
 *
 * Signal Type COM_FLOAT64
 */
#define COM_FLOAT64                                     (0x0Eu)
/* FC_VariationPoint_END */
/* END: Signal Type */

/*
 * return true in case signal type is of COM_UINT64 or COM_SINT64
 */
#define COM_SIG_IS_INT64_TYPE(type)                     (((type) & 0x1Eu) == (0x10u))
/*
 * return true in case signal type signed type COM_SINT8, COM_SINT16, COM_SINT32, COM_SINT64
 */
#define COM_SIG_IS_SIGNED_TYPE(type)                    (((type) & 0x1u) != 0x0u)

/* START: Signal Filter Type */

/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent signal filter algorithm ALWAYS
 */
#define COM_ALWAYS                                      (0u)  /*If filter is not configured it should be COM_ALWAYS */
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent signal filter algorithm NEVER
 */
#define COM_NEVER                                       (1u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent signal filter algorithm MASKED_NEW_EQUALS_X
 */
#define COM_MASKED_NEW_EQUALS_X                         (2u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent signal filter algorithm MASKED_NEW_DIFFERS_X
 */
#define COM_MASKED_NEW_DIFFERS_X                        (3u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent signal filter algorithm MASKED_NEW_DIFFERS_MASKED_OLD
 */
#define COM_MASKED_NEW_DIFFERS_MASKED_OLD               (4u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent signal filter algorithm NEW_IS_WITHIN_POS
 */
#define COM_NEW_IS_WITHIN_POS                           (5u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent signal filter algorithm NEW_IS_WITHIN_NEG
 */
#define COM_NEW_IS_WITHIN_NEG                           (6u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent signal filter algorithm NEW_IS_OUTSIDE_POS
 */
#define COM_NEW_IS_OUTSIDE_POS                          (7u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent signal filter algorithm NEW_IS_OUTSIDE_NEG
 */
#define COM_NEW_IS_OUTSIDE_NEG                          (8u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent signal filter algorithm ONE_EVERY_N
 */
#define COM_ONE_EVERY_N                                 (9u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent signal filter algorithm is not configured
 */
#define COM_NOTCONFIGURED                               (10u)

/* END: Signal Filter Type */

/* START: TMS calculation for IPDU */
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent IPdu has configured mode state TRUE and FALSE
 */
#define COM_MODE_VALID                                  (0u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent IPdu has configured mode state TRUE
 */
#define COM_TRUEMODE_VALID                              (1u)
/**
 * @ingroup COM_PRV_H
 *
 * Macro used in code to represent IPdu has configured mode state FALSE
 */
#define COM_FALSEMODE_VALID                             (2u)

/* END: TMS calculation for IPDU */

/**
 * @ingroup COM_PRV_H
 *
 * Macro is used to initialize instance of 'Com_SendIpduInfo' structure in Com Code
 */
#define COM_CLEAR_SENDIPDU_FLAGS                        {0u,0u,0u,0u,0u,0u,0u}

/**
 * @ingroup COM_PRV_H
 *
 * Used to indicate that rx-signals are not-configured with update bit and timeout
 */
#define COM_UPDATEBIT_NOT_APPLICABLE                    (0x00u)
/**
 * @ingroup COM_PRV_H
 *
 * Used to indicate that rx-signals are configured with update bit and timeout
 */
#define COM_UPDATEBIT_APPLICABLE                        (0x01u)
/**
 * @ingroup COM_PRV_H
 *
 * Used to know if rx-signal(-group) is received with update bit
 */
#define COM_UPDATEBIT_SET                               (0x02u)

/* Returns (2^Bit_Size) for provided 'Bit_Size' */
#define COM_GET_BASE_2_POWER_X(BIT_SIZE)                (((uint16)1u) << ((uint16)(BIT_SIZE)))

/* Returns maximum unsigned value for provided 'Bit_Size' */
#define COM_MAX_U_VALUE_OF_X_BITS(BIT_SIZE)             (COM_GET_BASE_2_POWER_X(BIT_SIZE) - 1u)

/**
 * @ingroup COM_PRV_H
 *
 * This macro has to be passed with '_LATESTMODE'
 *
 * below macro is equivalent to
 * ((tx_mode == COM_TXMODE_PERIODIC) || (tx_mode == COM_TXMODE_MIXED))
 */
#define COM_TX_MODE_IS_CYCLIC(TX_MODE)                  (((TX_MODE) & 0x1u) > COM_ZERO)

/**
 * @ingroup COM_PRV_H
 *
 * This macro has to be passed with '_LATESTMODE'
 *
 * below macro is equivalent to
 * ((tx_mode == COM_TXMODE_DIRECT) || (tx_mode == COM_TXMODE_MIXED))
 */
#define COM_TX_MODE_IS_EVENT(TX_MODE)                   ((TX_MODE) < COM_TXMODE_NONE)

/* START: Com_ServiceIdType - Service Id's   */

/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_Init(). \n
 */
#define COMServiceId_Init                                           0x01
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_DeInit(). \n
 */
#define COMServiceId_Deinit                                         0x02

/* This macro represents the service ID for the API Com_IpduGroupStart() */
#define COMServiceId_IpduGroupStart                                 0x03
/* This macro represents the service ID for the API Com_IpduGroupStop() */
#define COMServiceId_IpduGroupStop                                  0x04
/* This macro represents the service ID for the API Com_DisableReceptionDM() */
#define COMServiceId_DisableReceptionDM                             0x05
/* This macro represents the service ID for the API Com_EnableReceptionDM() */
#define COMServiceId_EnableReceptionDM                              0x06

/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_GetStatus(). \n
 */
#define COMServiceId_GetStatus                                      0x07
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_GetVersionInfo(). \n
 */
#define COMServiceId_GetVersionInfo                                 0x09

/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_SendSignal(). \n
 */
#define COMServiceId_SendSignal                                     0x0A
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_ReceiveSignal(). \n
 */
#define COMServiceId_ReceiveSignal                                  0x0B

/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_ReceiveDynSignal(). \n
 */
#define COMServiceId_ReceiveDynSignal                               0x22

/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_SendDynSignal(). \n
 */
#define COMServiceId_SendDynSignal                                  0x21
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_SendSignalGroup(). \n
 */
#define COMServiceId_SendSignalGroup                                0x0D
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_ReceiveSignalGroup(). \n
 */
#define COMServiceId_ReceiveSignalGroup                             0x0E
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_InvalidateSignal(). \n
 */
#define COMServiceId_InvalidateSignal                               0x10
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_InvalidateSignalGroup(). \n
 */
#define COMServiceId_InvalidateSignalGroup                          0x1B
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_TriggerIpduSend(). \n
 */
#define COMServiceId_TriggerIpduSend                                0x17
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_MainFunctionRx(). \n
 */
#define COMServiceId_MainFunctionRx                                 0x18
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_MainFunctionTx(). \n
 */
#define COMServiceId_MainFunctionTx                                 0x19
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_TxConfirmation(). \n
 */
#define COMServiceId_TxConfirmation                                 0x40
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_TriggerTransmit(). \n
 */
#define COMServiceId_TriggerTransmit                                0x41
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_RxIndication(). \n
 */
#define COMServiceId_RxIndication                                   0x42

/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_SwitchIpduTxMode(). \n
 */
#define COMServiceId_SwitchIpduTxMode                               0x27
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_StartOfReception(). \n
 */
#define COMServiceId_StartOfReception                               0x25
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_CopyRxData(). \n
 */
#define COMServiceId_CopyRxData                                     0x44
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_CopyTxData(). \n
 */
#define COMServiceId_CopyTxData                                     0x43
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_TpRxIndication(). \n
 */
#define COMServiceId_TpRxIndication                                 0x1E
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_TpTxConfirmation(). \n
 */
#define COMServiceId_TpTxConfirmation                               0x26
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_TriggerIPDUSendWithMetaData() \n
 */
#define COMServiceId_TriggerIPDUSendWithMetaData                    0x28
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_SendSignalGroupArray() \n
 */
#define COMServiceId_SendSignalGroupArray                           0x23
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_ReceiveSignalGroupArray() \n
 */
#define COMServiceId_ReceiveSignalGroupArray                        0x24
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the RxIpduCallout \n
 */
#define COMServiceId_RxIpduCallout                                  0x2A
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the TxIpduCallout \n
 */
#define COMServiceId_TxIpduCallout                                  0x29

/* FC_VariationPoint_START */
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_SetRxIPduControlViaRbaNdsEcuVariant()
 */
#ifdef COM_RXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT
#define COMServiceId_SetRxIPduControlViaRbaNdsEcuVariant            0x91
#endif /* #ifdef COM_RXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT */

/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_SetTxIPduControlViaRbaNdsEcuVariant()
 */
#ifdef COM_TXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT
#define COMServiceId_SetTxIPduControlViaRbaNdsEcuVariant            0x92
#endif /* #ifdef COM_TXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT */
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_SetRxIPduTimeoutTicks() \n
 */
#define COMServiceId_SetRxIPduTimeoutTicks                          0x93

/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_ReadRxIPduLength()
 * This is some random number assigned to this API \n
 */
#define COMServiceId_ReadRxIPduLength                               0x95u
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_ProvideRxIpduStatus() \n
 */
#define COMServiceId_ProvideRxIpduStatus                            0x96
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_ProvideTxIpduStatus() \n
 */
#define COMServiceId_ProvideTxIpduStatus                            0x97
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_IsTxScheduled() \n
 */
#define COMServiceId_IsTxScheduled                                  0x98
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_ReduceBusload() \n
 */
#define COMServiceId_ReduceBusload                                  0x99
/**
 * @ingroup COM_PRV_H
 *
 * This macro represents the service ID for the API Com_RestoreBusload() \n
 */
#define COMServiceId_RestoreBusload                                 0x9A

/* FC_VariationPoint_END */
/* This macro represents the service ID for the API Com_ReceiveSignalWithMetaData() */
#define COMServiceId_ReceiveSignalWithMetaData                      0x49
/* This macro represents the service ID for the API Com_ReceiveDynSignalWithMetaData() */
#define COMServiceId_ReceiveDynSignalWithMetaData                   0x4A
/* This macro represents the service ID for the API Com_ReceiveSignalGroupWithMetaData() */
#define COMServiceId_ReceiveSignalGroupWithMetaData                 0x4B
/* This macro represents the service ID for the API Com_ReceiveSignalGroupArrayWithMetaData() */
#define COMServiceId_ReceiveSignalGroupArrayWithMetaData            0x4C
/* This macro represents the service ID for the API Com_SendSignalWithMetaData() */
#define COMServiceId_SendSignalWithMetaData                         0x4D
/* This macro represents the service ID for the API Com_SendDynSignalWithMetaData() */
#define COMServiceId_SendDynSignalWithMetaData                      0x4E
/* This macro represents the service ID for the API Com_SendSignalGroupWithMetaData() */
#define COMServiceId_SendSignalGroupWithMetaData                    0x4F
/* This macro represents the service ID for the API Com_SendSignalGroupArrayWithMetaData() */
#define COMServiceId_SendSignalGroupArrayWithMetaData               0x50

/* End : Com_ServiceIdType - Service Id's   */

/**
 * @ingroup COM_PRV_H
 *
 *  This Macro copies source address to destination address
 */
#define COM_MEMCOPY(xDest_pv,  xSrc_pcv, numBytes_u32)              \
             (void)rba_BswSrv_MemCopy((void*)(xDest_pv),     (const void*)(xSrc_pcv),  (numBytes_u32));

/**
 * @ingroup COM_PRV_H
 *
 *  This Macro compares the two memory location and returns values
 */
#define COM_MEMCOMPARE(xSrc1_pcv, xSrc2_pcv, numBytes_u32)              \
             rba_BswSrv_MemCompare((const void*)(xSrc1_pcv), (const void*)(xSrc2_pcv), (numBytes_u32))

/* Unpack counter value into Ipdu buffer */
#ifndef COM_ARSRVLIB_SUPPORT

# define COM_UNPACK_CNTR_FROM_IPDU_BUFF(Data, StartPos, Size)             \
             (uint8)(((uint8)Data >> (uint8)StartPos) & ((uint8)COM_MAX_U_VALUE_OF_X_BITS(Size)))

#else
# define COM_UNPACK_CNTR_FROM_IPDU_BUFF(Data, StartPos, Size)             \
             Bfx_GetBits_u8u8u8_u8(Data, StartPos, Size)
#endif /* #ifndef COM_ARSRVLIB_SUPPORT */

/* Pack counter value into Ipdu buffer */
#ifndef COM_ARSRVLIB_SUPPORT
# define COM_PACK_CNTR_INTO_IPDU_BUFF(DataPtr, StartPos, Size, Counter)   \
            *((uint8 *)DataPtr) = \
            ((*((uint8 *)DataPtr) & (uint8)(~((uint8)((uint8)COM_MAX_U_VALUE_OF_X_BITS(Size)) << (uint8)StartPos))) |\
            (uint8)((uint8)Counter << (uint8)StartPos))

#else
# define COM_PACK_CNTR_INTO_IPDU_BUFF(DataPtr, StartPos, Size, Counter)   \
            Bfx_PutBits_u8u8u8u8(DataPtr, StartPos, Size, Counter)
#endif /* #ifndef COM_ARSRVLIB_SUPPORT */

/* Compute next counter value with respect to counter wrap-around */
#define COM_GET_CNTR_WRAP_AROUND_VALUE(Counter, MaxValue_u16)    (uint8)(((uint16)(Counter)) % ((uint16)(MaxValue_u16)))

#define COM_DET_REPORT_RUNTIME_ERROR(ApiId, ErrorId)  (void)Det_ReportRuntimeError(COM_MODULE_ID, COM_INSTANCE_ID, (ApiId), (ErrorId));

#if(COM_CONFIGURATION_USE_DET == STD_ON)

#define COM_DET_REPORT_ERROR(ApiId, ErrorId)  (void)Det_ReportError(COM_MODULE_ID, COM_INSTANCE_ID, (ApiId), (ErrorId));

#else

#define COM_DET_REPORT_ERROR(ApiId, ErrorId)

#endif /* #if(COM_CONFIGURATION_USE_DET == STD_ON) */

/**
 * @ingroup COM_PRV_H
 *
 * Macro to perform sign extension
 */
#define COM_SIGN_EXTEND_TO_32_BIT(Value,Size)    ((Value) | (COM_MAX_U32_VALUE << ((Size) - 1u)))
#define COM_SIGN_EXTEND_TO_64_BIT(Value,Size)    ((Value) | (COM_MAX_U64_VALUE << ((Size) - 1u)))

/*Macros for Bit-fields for RAM variables to Set the value*/
/* FC_VariationPoint_START */
/*
typedef struct
{
    uint16 PDUSTATUS:1;         [Bit0] Flag gives the old status of the Pdu Activation status
    uint16 CONFIR:1;            [Bit1] Used to check whether Confirmation for the Tx frame has been received or not
    uint16 TICKTXTO:1;          [Bit2] Flag which tells whether Timer was started/Stopped
    uint16 MDT:1;               [Bit3] Flag which tells whether Minimum delay time has not completed
    uint16 EVENTTRIG:1;         [Bit4] Flag which stores the information whether IPdu is transmitted in event
                                triggered call from Com_SendSignal/Com_SendDynSignal/Com_SendSignalGroup/
                                Com_SendSignalGroupArray/Com_TriggerIpduSend/Com_TriggerIpduSendWithMetadata/
                                Com_SwitchIpduTxMode
    uint16 ISGWTRIGGRD:1;       [Bit5] Flag which stores if there was a Trigger on the Gw Pdu
    uint16 ISTOENABLED:1;       [Bit6] Flag which stores information if the Timeout timer has to be started for
                                Ipdu.
    uint16 LARGEDATAINPROG:1;   [Bit7] Flag is set, if the Tp Pdu for large data pdu handling is in progress
    uint16 GWIPDUTMSCHANGED:1;  [Bit8] Flag is set, if the Gw-IPDU's Tranmission Mode has changed after its last
                                    transmission from Com
    uint16 RETRYFAILEDTXREQ:1;  [Bit9] Flag is set, if any failed Tx-Requests are present
    uint16 METADATA:1;          [Bit10] Flag remains set, until application provided metadata is transmitted to the
                                    lower layer(In case of ComTxIPduDeferredProcessing)
    uint16 MDTLOAD:1;           [Bit11] Flag which tells whether to load MDT/not to load MDT
    uint16 TXGWIPDU:1;          [Bit12] Flag which tells whether to transmit Tx IPDU from MainFunctionTx
    uint16 Not_Used:3;          Un-used bits
} Com_TxFlagsType;
*/
/* FC_VariationPoint_END */

/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the PDU activation status of Tx IPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_PDUSTATUS(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFFFEu)) | (uint16)(BitVal))

#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
 * Argument 1: pointer to a 16-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */

#define Com_SetRamValue_TXIPDU_PDUSTATUS(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),0u,(boolean)(BitVal))
#endif

/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the PDU activation status of Tx IPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_PDUSTATUS(ByteValue)         ((uint16)(ByteValue) & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_TXIPDU_PDUSTATUS(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),0u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro packs the status of Tx confirmation of a Tx Ipdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_CONFIR(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFFFDu)) | (uint16)((BitVal) << 1u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_TXIPDU_CONFIR(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),1u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the status of Tx confirmation of a Tx Ipdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_CONFIR(ByteValue)         ((uint16)((ByteValue) >> 1u) & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_TXIPDU_CONFIR(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),1u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro packs the Time out status flag for Tx IPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_TICKTXTO(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFFFBu)) | (uint16)((BitVal) << 2u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_TXIPDU_TICKTXTO(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),2u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Time out status flag for Tx IPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_TICKTXTO(ByteValue)         ((uint16)((ByteValue) >> 2u) & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_TXIPDU_TICKTXTO(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),2u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the MDT flag for Tx IPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_MDT(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFFF7u)) | (uint16)((BitVal) << 3u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_TXIPDU_MDT(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),3u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the MDT flag for Tx IPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_MDT(ByteValue)         ((uint16)((ByteValue) >> 3u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */

#define Com_GetRamValue_TXIPDU_MDT(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),3u)
#endif


 /**
 * @ingroup COM_PRV_H
 *
 *  This Macro packs the Tx value of the flag which stores the information whether IPdu is transmitted in event triggered\n
 *  call from Com_SendSignal/Com_SendSignalGroup/Com_SendDynSignal/Com_TriggerIpduSend
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_EVENTTRIG(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFFEFu)) | (uint16)((BitVal) << 4u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_TXIPDU_EVENTTRIG(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),4u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Tx value of the flag which stores the information whether IPdu is \n
 *  transmitted in event triggered call from Com_SendSignal/Com_SendSignalGroup/Com_SendDynSignal/Com_TriggerIpduSend
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_EVENTTRIG(ByteValue)         ((uint16)((ByteValue) >> 4u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_TXIPDU_EVENTTRIG(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),4u)
#endif

/**
* @ingroup COM_PRV_H
*
*  This Macro packs the flag value which stores the information whether a Gw IPdu is triggered for event transmission\n
*  The trigger can happen in Com_MainFunctionTx and Com_SendSignal/Com_SendSignalGroup
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_ISGWTRIGGRD(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFFDFu)) | (uint16)((BitVal) << 5u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
* Argument 1: pointer to a 8-bit variable, where a bit should be modified
* Argument 2: position of the bit in the byte
* Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
* */
#define Com_SetRamValue_TXIPDU_ISGWTRIGGRD(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),5u,(boolean)(BitVal))
#endif


/**
* @ingroup COM_PRV_H
*
*   This Macro unpacks and returns the Tx value of the flag which stores the information whether IPdu is transmitted in event triggered\n
*  call by application OR Com_MainFunctionTx()
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_ISGWTRIGGRD(ByteValue)         ((uint16)((ByteValue) >> 5u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
* Argument 1    : 8-bit Base variable, where should be read from.
* Argument 2    : bit position from, where the bit shall be read
* Return value    : Returns the bit value
* */
#define Com_GetRamValue_TXIPDU_ISGWTRIGGRD(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),5u)
#endif

/**
* @ingroup COM_PRV_H
*
*  This Macro packs the flag value which stores the information whether a Gw signal which triggered IPdu transmission has configured timeout\n
*  The trigger can happen in Com_MainFunctionTx and Com_SendSignal/Com_SendSignalGroup
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_ISTOENABLED(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFFBFu)) | (uint16)((BitVal) << 6u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
* Argument 1: pointer to a 8-bit variable, where a bit should be modified
* Argument 2: position of the bit in the byte
* Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
* */
#define Com_SetRamValue_TXIPDU_ISTOENABLED(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),6u,(boolean)(BitVal))
#endif


/**
* @ingroup COM_PRV_H
*
*  This Macro unpacks and returns the flag value which stores the information whether a Gw signal which triggered IPdu transmission has configured timeout\n
*  The trigger can happen in Com_MainFunctionTx and Com_SendSignal/Com_SendSignalGroup
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_ISTOENABLED(ByteValue)         ((uint16)((ByteValue) >> 6u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
* Argument 1    : 8-bit Base variable, where should be read from.
* Argument 2    : bit position from, where the bit shall be read
* Return value    : Returns the bit value
* */
#define Com_GetRamValue_TXIPDU_ISTOENABLED(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),6u)
#endif


/**
* @ingroup COM_PRV_H
*
*  This Macro packs the flag value which stores the information
* whether given Tx-IPDU of 'TP' type buffer is locked
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_LARGEDATAINPROG(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFF7Fu)) | (uint16)((BitVal) << 7u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
* Argument 1: pointer to a 8-bit variable, where a bit should be modified
* Argument 2: position of the bit in the byte
* Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
* */
#define Com_SetRamValue_TXIPDU_LARGEDATAINPROG(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),7u,(boolean)(BitVal))
#endif

/**
* @ingroup COM_PRV_H
*
*  This Macro unpacks and returns the flag value which stores the information
* whether given Tx-IPDU of 'TP' type buffer is locked
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_LARGEDATAINPROG(ByteValue)        ((uint16)((ByteValue) >> 7u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
* Argument 1    : 8-bit Base variable, where should be read from.
* Argument 2    : bit position from, where the bit shall be read
* Return value  : Returns the bit value
* */
#define Com_GetRamValue_TXIPDU_LARGEDATAINPROG(ByteValue)        Bfx_GetBit_u16u8_u8((ByteValue),7u)
#endif

/**
* @ingroup COM_PRV_H
*
* This Macro packs the flag value which stores the information whether TMS has changed after
* its last tranmission
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_GWIPDUTMSCHANGED(ByteValue,BitVal)  (ByteValue) = (((ByteValue) & ((uint16)0xFEFF)) | \
                                                                                               (uint16)((BitVal) << 8u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
* Argument 1: pointer to a 8-bit variable, where a bit should be modified
* Argument 2: position of the bit in the byte
* Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
* */
#define Com_SetRamValue_TXIPDU_GWIPDUTMSCHANGED(ByteValue,BitVal)  Bfx_PutBit_u16u8u8(&(ByteValue),8u,(boolean)(BitVal))
#endif

/**
* @ingroup COM_PRV_H
*
* This Macro unpacks and returns the flag value which stores the information whether TMS has changed after
* its last tranmission
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_GWIPDUTMSCHANGED(ByteValue)         ((uint16)((ByteValue) >> 8u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
* Argument 1    : 8-bit Base variable, where should be read from.
* Argument 2    : bit position from, where the bit shall be read
* Return value  : Returns the bit value
* */
#define Com_GetRamValue_TXIPDU_GWIPDUTMSCHANGED(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),8u)
#endif

/**
* @ingroup COM_PRV_H
*
* Macro to store failed Tx-IPdu requests, to be retried in the next Com_MainFunctionTx()
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_RETRYFAILEDTXREQ(ByteValue,BitVal)  (ByteValue) = (((ByteValue) & ((uint16)0xFDFFu)) | \
                                                                                              (uint16)((BitVal) << 9u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
* Argument 1: pointer to a 8-bit variable, where a bit should be modified
* Argument 2: position of the bit in the byte
* Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
* */
#define Com_SetRamValue_TXIPDU_RETRYFAILEDTXREQ(ByteValue,BitVal)  Bfx_PutBit_u16u8u8(&(ByteValue),9u,(boolean)(BitVal))
#endif

/**
* @ingroup COM_PRV_H
*
* Macro to unpack and check for any failed Tx-IPdu requests, to be retried in the Com_MainFunctionTx()
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_RETRYFAILEDTXREQ(ByteValue)         ((uint16)((ByteValue) >> 9u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
* Argument 1    : 8-bit Base variable, where should be read from.
* Argument 2    : bit position from, where the bit shall be read
* Return value  : Returns the bit value
* */
#define Com_GetRamValue_TXIPDU_RETRYFAILEDTXREQ(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),9u)
#endif

/**
* @ingroup COM_PRV_H
*
* Macro to store if application provided metadata is updated in TxIpdu buffer
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_METADATA_REQUEST(ByteValue,BitVal) (ByteValue) = (((ByteValue) & ((uint16)0xFBFF)) | \
                                                                                          (uint16)((BitVal) << 10u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
* Argument 1: pointer to a 8-bit variable, where a bit should be modified
* Argument 2: position of the bit in the byte
* Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
* */
#define Com_SetRamValue_TXIPDU_METADATA_REQUEST(ByteValue,BitVal) Bfx_PutBit_u16u8u8(&(ByteValue),10u,(boolean)(BitVal))
#endif

/**
* @ingroup COM_PRV_H
*
* Macro to unpack and check if application provided metadata is updated in TxIpdu buffer
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_METADATA_REQUEST(ByteValue)        ((uint16)((ByteValue) >> 10u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
* Argument 1    : 8-bit Base variable, where should be read from.
* Argument 2    : bit position from, where the bit shall be read
* Return value  : Returns the bit value
* */
#define Com_GetRamValue_TXIPDU_METADATA_REQUEST(ByteValue)        Bfx_GetBit_u16u8_u8((ByteValue),10u)
#endif

/**
* @ingroup COM_PRV_H
*
* Macro to load MDT
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_MDTLOAD(ByteValue,BitVal) (ByteValue) = (((ByteValue) & ((uint16)0xF7FF)) | \
                                                                                          (uint16)((BitVal) << 11u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
* Argument 1: pointer to a 16-bit variable, where a bit should be modified
* Argument 2: position of the bit in the byte
* Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
* */
#define Com_SetRamValue_TXIPDU_MDTLOAD(ByteValue,BitVal) Bfx_PutBit_u16u8u8(&(ByteValue),11u,(boolean)(BitVal))
#endif

/**
* @ingroup COM_PRV_H
*
* Macro to load MDT
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_MDTLOAD(ByteValue)        ((uint16)((ByteValue) >> 11u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
* Argument 1    : 8-bit Base variable, where should be read from.
* Argument 2    : bit position from, where the bit shall be read
* Return value  : Returns the bit value
* */
#define Com_GetRamValue_TXIPDU_MDTLOAD(ByteValue)        Bfx_GetBit_u16u8_u8((ByteValue),11u)
#endif
/**
* @ingroup COM_PRV_H
*
* Macro to transmit Tx IPDU from cyclic part of MainFunctionTx
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_GWIPDU_TXREQ(ByteValue,BitVal) (ByteValue) = (((ByteValue) & ((uint16)0xEFFF)) | \
                                                                                          (uint16)((BitVal) << 12u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
* Argument 1: pointer to a 16-bit variable, where a bit should be modified
* Argument 2: position of the bit in the byte
* Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
* */
#define Com_SetRamValue_TXIPDU_GWIPDU_TXREQ(ByteValue,BitVal) Bfx_PutBit_u16u8u8(&(ByteValue),12u,(boolean)(BitVal))
#endif

/**
* @ingroup COM_PRV_H
*
* Macro to transmit Tx IPDU from cyclic part of MainFunctionTx
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_GWIPDU_TXREQ(ByteValue)        ((uint16)((ByteValue) >> 12u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
* Argument 1    : 8-bit Base variable, where should be read from.
* Argument 2    : bit position from, where the bit shall be read
* Return value  : Returns the bit value
* */
#define Com_GetRamValue_TXIPDU_GWIPDU_TXREQ(ByteValue)        Bfx_GetBit_u16u8_u8((ByteValue),12u)
#endif


/* FC_VariationPoint_START */
/*
typedef struct
{
    uint16 PduStatus:1;              [Bit0] Flag gives the old status of the Pdu Activation status
    uint16 Pdu_DMStatus:1;           [Bit1] Flag used to Indicate Ipdu based reception deadline monitoring status
    uint16 RxIndicationFlag:1;       [Bit2] Flag used to Indicate that Ipdu was received
    uint16 IsRxGwIpduProcessed:1;    [Bit3] Flag used to Indicate if the Gw Ipdu is processed after its reception
    uint16 rxTPIPduFlag:1;           [Bit4] Flag used to Indicate the reception of TP Rx Ipdu
    uint16 IsFirstReception:1;       [Bit5] Flag used in case of first reception of IPdu configured with counter
    uint16 IsInvalid:1;              [Bit6] Flag used in case large-data received IPDU has to be considered as
                                    'invalid'
    uint16 reloadRxTicks:1;          [Bit7] Flag used in case 'new' Rx IPDU deadline value is updated via
                                            Com_SetRxIPduTimeoutTicks
    uint16 GWSIG_UPDATED:1;          [Bit8] Flag is set if any contained Gw signal buffer is updated
    uint16 unused:7;

} Com_RxFlagsType;
 */
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the PDU activation status of Rx IPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXIPDU_PDUSTATUS(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFFFEu)) | (uint16)(BitVal))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
 * Argument 1: pointer to a 16-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXIPDU_PDUSTATUS(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),0u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the PDU activation status of Rx IPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXIPDU_PDUSTATUS(ByteValue)         ((uint16)(ByteValue) & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_RXIPDU_PDUSTATUS(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),0u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro packs the Dead line monitoring Activation status for RxIPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXIPDU_DMSTATUS(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFFFDu)) | (uint16)((BitVal) << 1u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
 * Argument 1: pointer to a 16-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXIPDU_DMSTATUS(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),1u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns  the Dead line monitoring Activation status for RxIPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXIPDU_DMSTATUS(ByteValue)         ((uint16)((ByteValue) >> 1u) & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_RXIPDU_DMSTATUS(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),1u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 *  This Macro packs the Rx indication status for RxIPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXIPDU_INDICATION(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFFFBu)) | (uint16)((BitVal) << 2u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
 * Argument 1: pointer to a 16-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXIPDU_INDICATION(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),2u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns  the Rx indication status for RxIPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXIPDU_INDICATION(ByteValue)         ((uint16)((ByteValue) >> 2u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_RXIPDU_INDICATION(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),2u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 *  This Macro packs the processed status for Gw Rx Ipdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXIPDU_GWPDU_TOBEPROCESSED(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint16)0xFFF7u)) | (uint16)((BitVal) << 3u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
 * Argument 1: pointer to a 16-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXIPDU_GWPDU_TOBEPROCESSED(ByteValue,BitVal)         Bfx_PutBit_u16u8u8(&(ByteValue),3u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the processed status for Gw Rx Ipdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXIPDU_GWPDU_TOBEPROCESSED(ByteValue)         ((uint16)((ByteValue) >> 3u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_RXIPDU_GWPDU_TOBEPROCESSED(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),3u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 *  This Macro packs the processed status for Large data TP Ipdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXIPDU_LARGEDATAINPROG(ByteValue,BitVal)        (ByteValue) = (((ByteValue) & ((uint16)0xFFEFu)) | (uint16)((BitVal) << 4u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
 * Argument 1: pointer to a 16-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXIPDU_LARGEDATAINPROG(ByteValue,BitVal)        Bfx_PutBit_u16u8u8(&(ByteValue),4u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the processed status for Large data TP Ipdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXIPDU_LARGEDATAINPROG(ByteValue)       ((uint16)((ByteValue) >> 4u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 16-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#define Com_GetRamValue_RXIPDU_LARGEDATAINPROG(ByteValue)       Bfx_GetBit_u16u8_u8((ByteValue),4u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 *  This macro is used to set whenever RxIPdu is started from Com_Init API or Com_IpduGroupStart API,
 *  before its first reception
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXIPDU_IS_FIRST_RECEPTION(ByteValue,BitVal)    (ByteValue) = (((ByteValue) & ((uint16)0xFFDFu)) | (uint16)((BitVal) << 5u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
 * Argument 1: pointer to a 16-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXIPDU_IS_FIRST_RECEPTION(ByteValue,BitVal)    Bfx_PutBit_u16u8u8(&(ByteValue),5u,(boolean)(BitVal))
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This macro is used to know if RxIPdu is recently started through Com_Init API or Com_IpduGroupStart API,
 * before its first reception
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXIPDU_IS_FIRST_RECEPTION(ByteValue)    ((uint16)((ByteValue) >> 5u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 16-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#define Com_GetRamValue_RXIPDU_IS_FIRST_RECEPTION(ByteValue)    Bfx_GetBit_u16u8_u8((ByteValue),5u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This macro is used to set if previously received large-IPDU has to be treated incorrect, as if all signals of the
 * received large I-PDU exactly like invalidated signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXIPDU_IS_INVALID(ByteValue,BitVal)    (ByteValue) = (((ByteValue) & ((uint16)0xFFBFu)) | (uint16)((BitVal) << 6u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
 * Argument 1: pointer to a 16-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXIPDU_IS_INVALID(ByteValue,BitVal)    Bfx_PutBit_u16u8u8(&(ByteValue),6u,(boolean)(BitVal))
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This macro is used to know if previously received large-IPDU has to be treated incorrect, as if all signals of the
 * received large I-PDU exactly like invalidated signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXIPDU_IS_INVALID(ByteValue)    ((uint16)((ByteValue) >> 6u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 16-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#define Com_GetRamValue_RXIPDU_IS_INVALID(ByteValue)    Bfx_GetBit_u16u8_u8((ByteValue),6u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This macro is used to set if new Rx IPDU based deadline value is provided via Com_SetRxIPduTimeoutTicks
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXIPDU_RELOAD_DM_TIMER(ByteValue,BitVal)    \
                                        (ByteValue) = (((ByteValue) & ((uint16)0xFF7Fu)) | (uint16)((BitVal) << 7u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
 * Argument 1: pointer to a 16-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXIPDU_RELOAD_DM_TIMER(ByteValue,BitVal)  Bfx_PutBit_u16u8u8(&(ByteValue),7u,(boolean)(BitVal))
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This macro is used to know if Rx IPDU based deadline timer has to be reloaded in MainFunctionRx
 * with ticks provided via Com_SetRxIPduTimeoutTicks
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXIPDU_RELOAD_DM_TIMER(ByteValue)    ((uint16)((ByteValue) >> 7u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 16-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#define Com_GetRamValue_RXIPDU_RELOAD_DM_TIMER(ByteValue)    Bfx_GetBit_u16u8_u8((ByteValue),7u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This macro is used to set if new Rx IPDU based deadline value is provided via Com_SetRxIPduTimeoutTicks
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXIPDU_GWSIG_UPDATED(ByteValue,BitVal)    \
                                        (ByteValue) = (((ByteValue) & ((uint16)0xFEFFu)) | (uint16)((BitVal) << 8u))
#else
/* Bfx_PutBit_u16u8u8 : Sets or clears a bit at a given position BitPn in the 16-bit variable Data.
 * Argument 1: pointer to a 16-bit variable, where a bit should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXIPDU_GWSIG_UPDATED(ByteValue,BitVal)    Bfx_PutBit_u16u8u8(&(ByteValue),8u,(boolean)(BitVal))
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This macro is used to know if Rx IPDU based deadline timer has to be reloaded in MainFunctionRx
 * with ticks provided via Com_SetRxIPduTimeoutTicks
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXIPDU_GWSIG_UPDATED(ByteValue)    ((uint16)((ByteValue) >> 8u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 16-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#define Com_GetRamValue_RXIPDU_GWSIG_UPDATED(ByteValue)    Bfx_GetBit_u16u8_u8((ByteValue),8u)
#endif


/* FC_VariationPoint_START */
/*
typedef struct
{
    uint8 LatestTransMode:2;    Stores the Latest Transmission Mode of the IPDU
    uint8 TxModeState:1;        Stores the Transmission mode State i.e TRUE/FALSE
    uint8 MixedPhaseShift:1;    MixedPhaseShift status. If this is SET, then current mode is MixedPhaseShift
    uint8 Not_Used:4;           Not used Bit field
}Com_TransModeType;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 *  This Macro packs Tx Ipdu Latest mode
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_LATESTMODE(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFCu)) | (uint8)(BitVal))
#else
/* Bfx_PutBits_u8u8u8u8 : puts the bits, as per the bit pattern starting from a given position till the size in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bits should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: length of the bits
 * Argument 4: value of bits, bit pattern to be written
 * */
#define Com_SetRamValue_TXIPDU_LATESTMODE(ByteValue,BitVal)         Bfx_PutBits_u8u8u8u8(&(ByteValue),0u,2u,(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro returns Tx Ipdu Latest mode
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_LATESTMODE(ByteValue)         ((uint8)((ByteValue)) & (uint8)(0x03u))
#else
/* Bfx_GetBits_u8u8u8_u8 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value    : Returns the bit pattern
 * */
#define Com_GetRamValue_TXIPDU_LATESTMODE(ByteValue)         Bfx_GetBits_u8u8u8_u8((ByteValue),0u,2u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 *  This Macro packs Tx Ipdu State(current)
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_TXMODESTATE(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFBu)) | (uint8)((BitVal) << 2u))
#else
/* Bfx_PutBits_u8u8u8u8 : puts the bits, as per the bit pattern starting from a given position till the size in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bits should be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: length of the bits
 * Argument 4: value of bits, bit pattern to be written
 * */
#define Com_SetRamValue_TXIPDU_TXMODESTATE(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),2u,(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro returns Tx Ipdu Tx mode state(current)
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_TXMODESTATE(ByteValue)         ((uint8)((ByteValue) >> 2u) & (uint8)(0x01u))
#else
/* Bfx_GetBits_u8u8u8_u8 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value    : Returns the bit pattern
 * */
#define Com_GetRamValue_TXIPDU_TXMODESTATE(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),2u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs MIXEDPHASESHIFT status of the tx Ipdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXIPDU_MIXEDPHASESHIFT(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xF7u)) | (uint8)((BitVal) << 3u))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_TXIPDU_MIXEDPHASESHIFT(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),3u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro returns true, if the Tx-IPdu current mode is selected as MIXEDPHASESHIFT
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXIPDU_MIXEDPHASESHIFT(ByteValue)         ((uint8)((ByteValue) >> 3u) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_TXIPDU_MIXEDPHASESHIFT(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),3u)
#endif


/* FC_VariationPoint_START */

/*
 typedef struct
{
    uint8 TrigOnChange:1;    Flag is set to indicate that SignalGroup is triggered
    uint8 IsGwUpdated:1;     Flag is set to indicate that Gw-Tx-SignalGroup is updated
    uint8 Not_Used:6;        Not used Bit field

}Com_TxSignalGrpFlagType_tst;

 */
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 *  This Macro packs Triggered_on_Change status of Tx Signal Group
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXSIGGRP_TRIGONCHNG(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFEu)) | (uint8)(BitVal))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_TXSIGGRP_TRIGONCHNG(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),0u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns Triggered_on_Change status of Tx Signal Group
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXSIGGRP_TRIGONCHNG(ByteValue)         ((uint8)(ByteValue) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_TXSIGGRP_TRIGONCHNG(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),0u)
#endif


#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXSIGGRP_ISGWUPDTD(ByteValue,BitVal)       (ByteValue) = (((ByteValue) & ((uint8)0xFDu)) | (uint8)((BitVal) << 1u))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_TXSIGGRP_ISGWUPDTD(ByteValue,BitVal)       Bfx_PutBit_u8u8u8(&(ByteValue),1u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns Triggered_on_Change status of Tx Signal Group
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXSIGGRP_ISGWUPDTD(ByteValue)      ((uint8)((ByteValue) >> 1u) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 8-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#define Com_GetRamValue_TXSIGGRP_ISGWUPDTD(ByteValue)      Bfx_GetBit_u8u8_u8((ByteValue),1u)
#endif

/* FC_VariationPoint_START */
/*
typedef struct
{
    uint8 SigGrpNotify_Flag:1;      Flagged to provide notification callback
    uint8 Not_Used:7;               Not used Bit field

}Com_RxSignalGrpFlagType_tst;
*/
/* FC_VariationPoint_END */

/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the SignalGroup Notify flag for Rx Signal Groups
 */

#ifdef COM_PRV_ENABLECONFIGINTERFACES
/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the Notify flag for Rx Signal Groups
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXSIGGRPCFG_SIGNOTIF(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFEu)) | (uint8)(BitVal))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXSIGGRPCFG_SIGNOTIF(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),0u,(boolean)(BitVal))
#endif
#endif /* end of #ifdef COM_PRV_ENABLECONFIGINTERFACES */
/* FC_VariationPoint_START */
/*
typedef struct
{
    uint8 SigGrpNotify_Flag:1;      Flagged to provide notification callback
    uint8 SigGrpFillBit:1           Flagged to know whether the Rx-SigGrp is to be gatewayed
    uint8 RxDMFilteringPassed:1     TRACE[SWS_Com_00793] After the RxTimeouttimer elapses, the first reception of
                                    the signal shall not be filtered if it is of MASKED_NEW_DIFFERS_MASKED_OLD type
    uint8 Not_Used:5;               Not used Bit field

}Com_RxSignalGrpFlagType_tst;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the Notify flag for Rx Signal Groups
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXSIGGRP_SIGNOTIF(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFEu)) | (uint8)(BitVal))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXSIGGRP_SIGNOTIF(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),0u,(boolean)(BitVal))
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns  the Notify flag for Rx Signal Groups
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXSIGGRP_SIGNOTIF(ByteValue)         ((uint8)(ByteValue) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_RXSIGGRP_SIGNOTIF(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),0u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the fill bit status for Rx Signal Groups
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXSIGGRP_SIGNALFILLBIT(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFDu)) | (uint8)(BitVal << 1u))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXSIGGRP_SIGNALFILLBIT(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),1u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns  the fill bit status for Rx Signal Groups
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXSIGGRP_SIGNALFILLBIT(ByteValue)        ((uint8)((ByteValue) >> 1u) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 8-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#define Com_GetRamValue_RXSIGGRP_SIGNALFILLBIT(ByteValue)        Bfx_GetBit_u8u8_u8((ByteValue),1u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the Rx deadline monitoring flag to ignore filter processing for Rx SignalGroups
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXSIGGRP_DMFILTERING(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFBu)) | (uint8)(BitVal << 2u))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXSIGGRP_DMFILTERING(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),2u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns Rx deadline monitoring flag to ignore filter processing for Rx Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXSIGGRP_DMFILTERING(ByteValue)         ((uint8)((ByteValue) >> 2u) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_RXSIGGRP_DMFILTERING(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),2u)
#endif


/*
 typedef struct
{
    uint8 SigTxChangeMode:1;
    uint8 Not_Used:7;               Not used Bit field

}Com_TxGrpSignalFlagType_tst;
 */
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 *  This Macro packs the SigTxChangeMode status of a Tx Group Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXGRPSIG_SIGCHNGMODE(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFEu)) | (uint8)(BitVal))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_TXGRPSIG_SIGCHNGMODE(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),0u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the SigTxChangeMode status  of a Tx Group Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXGRPSIG_SIGCHNGMODE(ByteValue)         ((uint8)(ByteValue) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_TXGRPSIG_SIGCHNGMODE(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),0u)
#endif

/* FC_VariationPoint_START */
/*
typedef struct
{
    uint8 SigTxChangeMode:1;
    uint8 Not_Used:7;               Not used Bit field

} Com_TxSignalFlagType_tst;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the SigTxChangeMode status of a Tx Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_TXSIG_SIGCHNGMODE(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFEu)) | (uint8)(BitVal))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_TXSIG_SIGCHNGMODE(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),0u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the SigTxChangeMode status  of a Tx Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_TXSIG_SIGCHNGMODE(ByteValue)         ((uint8)(ByteValue) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_TXSIG_SIGCHNGMODE(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),0u)
#endif

/* FC_VariationPoint_START */
/*
typedef struct
{
    uint8 SigNotify_Flag:1;         Flag used for Conditional notification
    uint8 Not_Used:7;               Not used Bit field
} Com_RxSigGeneralType;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the Signal Notify flag for Rx Signals
 */
#ifdef COM_PRV_ENABLECONFIGINTERFACES
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXSIGCFG_SIGNOTIF(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFEu)) | (uint8)(BitVal))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXSIGCFG_SIGNOTIF(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),0u,(boolean)(BitVal))
#endif
#endif  /* end of #ifdef COM_PRV_ENABLECONFIGINTERFACES */

/* FC_VariationPoint_START */
/*
typedef struct
{
    uint8 SigNotify_Flag:1;         Flag used for Conditional notification
    uint8 Sig_FillBit:1;            Flag used to know the status of Signal buffer
                                    COM_TRUE = Signal buffer updated
                                    COM_FALSE = Signal buffer not updated
    uint8 RxDMFilteringPassed:1;    TRACE[SWS_Com_00793] After the RxTimeouttimer elapses, the first reception of
                                    the signal shall not be filtered if it is of MASKED_NEW_DIFFERS_MASKED_OLD type
    uint8 Not_Used:5;               Not used Bit field
}Com_RxSignalFlagType_tst;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the Notify flag for Rx Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXSIG_SIGNOTIF(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFEu)) | (uint8)(BitVal))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXSIG_SIGNOTIF(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),0u,(boolean)(BitVal))
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns  the Notify flag for Rx Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXSIG_SIGNOTIF(ByteValue)         ((uint8)(ByteValue) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_RXSIG_SIGNOTIF(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),0u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the Signal filled status flag for Rx Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXSIG_SIGNALFILLBIT(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFDu)) | (uint8)(BitVal << 1u))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXSIG_SIGNALFILLBIT(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),1u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns Signal filled status flag for Rx Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXSIG_SIGNALFILLBIT(ByteValue)         ((uint8)((ByteValue) >> 1u) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_RXSIG_SIGNALFILLBIT(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),1u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro packs the Rx deadline monitoring flag to ignore filter processing for Rx Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_SetRamValue_RXSIG_DMFILTERING(ByteValue,BitVal)         (ByteValue) = (((ByteValue) & ((uint8)0xFBu)) | (uint8)(BitVal << 2u))
#else
/* Bfx_PutBit_u8u8u8 : Sets or clears a bit at a given position BitPn in the 8-bit variable Data.
 * Argument 1: pointer to a 8-bit variable, where a bit schould be modified
 * Argument 2: position of the bit in the byte
 * Argument 3: defines the new bit, 0 clears the bit and 1 sets the bit
 * */
#define Com_SetRamValue_RXSIG_DMFILTERING(ByteValue,BitVal)         Bfx_PutBit_u8u8u8(&(ByteValue),2u,(boolean)(BitVal))
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns Rx deadline monitoring flag to ignore filter processing for Rx Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetRamValue_RXSIG_DMFILTERING(ByteValue)         ((uint8)((ByteValue) >> 2u) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetRamValue_RXSIG_DMFILTERING(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),2u)
#endif

/* FC_VariationPoint_START */
/* These Set of Macros return the CONST BitField values */

/*
    typedef struct
    {
        uint16 Signal_Processing:1;         [Bit0] Signal Processing of each signal within the IPDU(DEFERRED(1)/IMMEDIATE(0))
        uint16 TMSCalclation:2;             [Bit2-Bit1] Transmission mode status
                                                COM_MODE_VALID      (0)  If both the modes are configured
                                                COM_TRUEMODE_VALID  (1)  If TRUE mode is configured
                                                COM_FALSEMODE_VALID (2)  If FALSE mode is configured
                                                COM_MODE_INVALID    (3)  If both the modes are not configured
        uint16 NoneModeTimeOut:1;           [Bit3] Set, if Deadline monitoring for the IPdu for TX mode none
        uint16 ClearUpdBit:2;               [Bit5-Bit4] Status of where to clear update bit
                                                TRANSMIT        (1)
                                                TRIGGERTRANSMIT (2)
                                                CONFIRMATION    (3)
        uint16 FilterEvalReq:1;             [Bit6] Set, if Filter evaluation required
        uint16 IsDynPdu:1;                  [Bit7] Set, if the Ipdu is of Dynamic type
        uint16 IsGwDestPdu:1;               [Bit8] Set, if the Ipdu has Gw destination signal
        uint16 IsCalloutFrmTrigTrans:1;     [Bit9] Set, if the callout is from Com_TriggerTransmit
        uint16 IsLargeDataPdu:1;            [Bit10] Set, if the IPdu is of Large data TP IPdu
        uint16 isCancelTransmitSupported:1; [Bit11] Set, if the IPdu supports CancelTransmit
        uint16 ipduPartOfIpduGrp:1;         [Bit12] Set, if the Ipdu is referring any IpduGroup
        uint16 defaultTMSStatus:1;          [Bit13] Set, if Ipdu is not referring to any IpduGroup, and
                                                 the filter evaluation during the configuration time resulted in true
        uint16 Is_MetaDataPdu:1;            [Bit14] Set, if IPDU is metadata pdu
        uint16 Not_Used:1;                  Unused bitfields, reserved for future use
    } Com_TxIpduFlagType;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns Signal Processing of the Tx and Rx IPDU
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXIPDU_SIGPROC(ByteValue)         ((uint16)(ByteValue) & ((uint16)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXIPDU_SIGPROC(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),0u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns TMS value of the Tx IPDU
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXIPDU_TMS(ByteValue)         ((uint16)((ByteValue) >> 1u) & ((uint16)0x03u))
#else
/* Bfx_GetBits_u8u8u8_u8 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value    : Returns the bit pattern
 * */
#define Com_GetValue_TXIPDU_TMS(ByteValue)         Bfx_GetBits_u16u8u8_u16((ByteValue),1u,2u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns None mode Dm status of the Tx IPDU
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXIPDU_NONETOSTATUS(ByteValue)         ((uint16)((ByteValue) >> 3u) & ((uint16)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXIPDU_NONETOSTATUS(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),3u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns ClearUpdateBit status of a Tx IPdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXIPDU_CLRUPDBIT(ByteValue)         ((uint16)((ByteValue) >> 4u) & ((uint16)0x03u))
#else
/* Bfx_GetBits_u8u8u8_u8 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value    : Returns the bit pattern
 * */
#define Com_GetValue_TXIPDU_CLRUPDBIT(ByteValue)         Bfx_GetBits_u16u8u8_u16((ByteValue),4u,2u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns TMS value of the Tx IPDU
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXIPDU_FILTEREVALREQ(ByteValue)         ((uint16)((ByteValue) >> 6u) & ((uint16)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXIPDU_FILTEREVALREQ(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),6u)
#endif


/**
* @ingroup COM_PRV_H
*
*   This Macro unpacks and returns the Tx value of the flag which stores the information whether IPdu is transmitted in event triggered\n
*  call by application OR Com_MainFunctionTx()
*/
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXIPDU_ISDYNIPDU(ByteValue)         ((uint16)((ByteValue) >> 7u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
* Argument 1    : 8-bit Base variable, where should be read from.
* Argument 2    : bit position from, where the bit shall be read
* Return value    : Returns the bit value
* */
#define Com_GetValue_TXIPDU_ISDYNIPDU(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),7u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the information if the Tx IPDU is Gw Pdu (has Gw signals)
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXIPDU_ISGWPDU(ByteValue)         ((uint16)((ByteValue) >> 8u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXIPDU_ISGWPDU(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),8u)
#endif
/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the information if the Tx IPDU is Gw Pdu (has Gw signals)
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXIPDU_ISCALLOUTFRMTRIGTRANS(ByteValue)         ((uint16)((ByteValue) >> 9u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXIPDU_ISCALLOUTFRMTRIGTRANS(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),9u)
#endif
/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the information if the Tx IPDU is Large data pdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXIPDU_ISLARGEDATAPDU(ByteValue)        ((uint16)((ByteValue) >> 10u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 8-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#define Com_GetValue_TXIPDU_ISLARGEDATAPDU(ByteValue)        Bfx_GetBit_u16u8_u8((ByteValue),10u)
#endif
/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the information if the Tx IPDU is Large data pdu
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXIPDU_ISCANCELTRANSMITSUPPORTED(ByteValue)        ((uint16)((ByteValue) >> 11u)  & (uint16)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 8-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#define Com_GetValue_TXIPDU_ISCANCELTRANSMITSUPPORTED(ByteValue)        Bfx_GetBit_u16u8_u8((ByteValue),11u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns boolean if the Ipdu is referring any IpduGroup
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_TXIPDU_PDUWITHPDUGRP(ByteValue)  ( ( uint16 )( ( ByteValue ) >> 12u ) & ( ( uint16 )0x01u ) )
# else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value  : Returns the bit value
 * */
#  define Com_GetValue_TXIPDU_PDUWITHPDUGRP(ByteValue)  Bfx_GetBit_u16u8_u8( ( ByteValue ), 12u )
# endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns boolean of Default TMS status for the IPdu
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_TXIPDU_DEFAULTTMSSTATUS(ByteValue)  ( ( uint16 )( ( ByteValue ) >> 13u ) & ( ( uint16 )0x01u ) )
# else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value  : Returns the bit value
 * */
#  define Com_GetValue_TXIPDU_DEFAULTTMSSTATUS(ByteValue)  Bfx_GetBit_u16u8_u8( ( ByteValue ), 13u )
# endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns boolean if pdu contain metadata
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_TXIPDU_IS_METADATAPDU(ByteValue)  ( ( uint16 )( ( ByteValue ) >> 14u ) & ( ( uint16 )0x01u ) )
# else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value  : Returns the bit value
 * */
#  define Com_GetValue_TXIPDU_IS_METADATAPDU(ByteValue)  Bfx_GetBit_u16u8_u8( ( ByteValue ), 14u )
# endif

/*
    typedef struct
    {
        uint8 sigAckCbk:1;                  [Bit0] set, if signal-notification/"ACK"-callback is configured
        uint8 sigGrpAckCbk:1;               [Bit1] set, if sigGrp-notification/"ACK"-callback is configured
        uint8 sigToutCbk:1;                 [Bit2] set, if signal-timeoutNotification/"TOUT"-callback is configured
        uint8 sigGrpToutCbk:1;              [Bit3] set, if sigGrp-timeoutNotification/"TOUT"-callback is configured
        uint8 sigErrCbk:1;                  [Bit4] set, if signal-errorNotification/"ERR"-callback is configured
        uint8 sigGrpErrCbk:1;               [Bit5] set, if sigGrp-errorNotification/"ERR"-callback is configured
        uint8 Not_Used:2;                   Unused bitfields
    } Com_TxIpduVarFields_u8;
*/
/**
 * This Macro unpacks and returns 'true',
 * if TxIPdu contains at least one signal with signal-notification/"ACK"-callback is configured
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_TXIPDUVAR_IS_SIG_ACKCBK(ByteValue)       (((uint8)(ByteValue)) & ((uint8)0x01u))
# else
#  define Com_GetValue_TXIPDUVAR_IS_SIG_ACKCBK(ByteValue)       Bfx_GetBit_u8u8_u8((ByteValue), 0u)
# endif
/**
 * This Macro unpacks and returns 'true',
 * if TxIPdu contains at least one signal with signal-notification/"ACK"-callback is configured
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_TXIPDUVAR_IS_SIGGRP_ACKCBK(ByteValue)    (((uint8)((ByteValue) >> 1u)) & ((uint8)0x01u))
# else
#  define Com_GetValue_TXIPDUVAR_IS_SIGGRP_ACKCBK(ByteValue)    Bfx_GetBit_u8u8_u8((ByteValue), 1u)
# endif
/*
 * This Macro unpacks and returns 'true' if TX IPDU contains at least one signal or signal-group with ACK callback
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_TXIPDUVAR_IS_ACKCBK(ByteValue)            ((((uint8)(ByteValue)) & ((uint8)0x03u)) > 0u)
#else
# define Com_GetValue_TXIPDUVAR_IS_ACKCBK(ByteValue)            (Bfx_GetBits_u8u8u8_u8((ByteValue),0u,2u) > 0u)
#endif
/**
 * This Macro unpacks and returns 'true',
 * if TxIPdu contains at least one signal with signal-timeoutNotification/"TOUT"-callback is configured
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_TXIPDUVAR_IS_SIG_TOUTCBK(ByteValue)       (((uint8)((ByteValue) >> 2u)) & ((uint8)0x01u))
# else
#  define Com_GetValue_TXIPDUVAR_IS_SIG_TOUTCBK(ByteValue)       Bfx_GetBit_u8u8_u8((ByteValue), 2u)
# endif
/**
 * This Macro unpacks and returns 'true',
 * if TxIPdu contains at least one signal with signal-timeoutNotification/"TOUT"-callback is configured
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_TXIPDUVAR_IS_SIGGRP_TOUTCBK(ByteValue)    (((uint8)((ByteValue) >> 3u)) & ((uint8)0x01u))
# else
#  define Com_GetValue_TXIPDUVAR_IS_SIGGRP_TOUTCBK(ByteValue)    Bfx_GetBit_u8u8_u8((ByteValue), 3u)
# endif
/*
 * This Macro unpacks and returns 'true' if TX IPDU contains at least one signal or signal-group with ERR callback
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_TXIPDUVAR_IS_TOUTCBK(ByteValue)            ((((uint8)((ByteValue) >> 2u)) & ((uint8)0x03u)) > 0u)
#else
# define Com_GetValue_TXIPDUVAR_IS_TOUTCBK(ByteValue)            (Bfx_GetBits_u8u8u8_u8((ByteValue),2u,2u) > 0u)
#endif

/**
 * This Macro unpacks and returns 'true',
 * if TxIPdu contains at least one signal with signal-errorNotification/"ERR"-callback is configured
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_TXIPDUVAR_IS_SIG_ERRCBK(ByteValue)       (((uint8)((ByteValue) >> 4u)) & ((uint8)0x01u))
# else
#  define Com_GetValue_TXIPDUVAR_IS_SIG_ERRCBK(ByteValue)       Bfx_GetBit_u8u8_u8((ByteValue), 4u)
# endif
/**
 * This Macro unpacks and returns 'true',
 * if TxIPdu contains at least one signal with signal-errorNotification/"ERR"-callback is configured
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_TXIPDUVAR_IS_SIGGRP_ERRCBK(ByteValue)    (((uint8)((ByteValue) >> 5u)) & ((uint8)0x01u))
# else
#  define Com_GetValue_TXIPDUVAR_IS_SIGGRP_ERRCBK(ByteValue)    Bfx_GetBit_u8u8_u8((ByteValue), 5u)
# endif
/*
 * This Macro unpacks and returns 'true' if TX IPDU contains at least one signal or signal-group with ERR callback
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_TXIPDUVAR_IS_ERRCBK(ByteValue)            ((((uint8)((ByteValue) >> 4u)) & ((uint8)0x03u)) > 0u)
#else
# define Com_GetValue_TXIPDUVAR_IS_ERRCBK(ByteValue)            (Bfx_GetBits_u8u8u8_u8((ByteValue),4u,2u) > 0u)
#endif


/* FC_VariationPoint_START */
/*
    typedef struct
    {
        uint8 Signal_Processing:1;  [Bit0] Signal Processing of each signal within the IPDU
                                     COM_TRUE  : COM_DEFERRED  IPdu Processing
                                     COM_FALSE : COM_IMMEDIATE IPdu Processing
        uint8 Notify_Cbk:1;         [Bit1] Set, If any of the Signal within the Ipdu has Notification cbk configured
        uint8 IsRxGwIPdu:1;         [Bit2] Set, if RxIpdu has any data to be Gatewayed
        uint8 IpduPartOfIpduGrp:1;  [Bit3] Set, if the Ipdu is referring any IpduGroup
        uint8 IS_TP_TYPE:1;         [Bit4] Set, if IPDU type is TP otherwise 0 for NORMAL
        uint8 TP_INV_CFG:1;         [Bit5] Set, if TP IPDU contains signal(group) with configured invalidAction
        uint8 Is_MetaDataPdu:1;     [Bit6] Set, if IPDU is metadata pdu
        uint8 Not_Used:1;           Not used Bit field

    } Com_RxIpduFlagType;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns Signal Processing of the Tx and Rx IPDU
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXIPDU_SIGPROC(ByteValue)              ((uint8)(ByteValue) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_RXIPDU_SIGPROC(ByteValue)              Bfx_GetBit_u8u8_u8((ByteValue),0u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns Notification Cbk status of the Rx IPDU
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXIPDU_NOTIFCBK(ByteValue)             ((uint8)((ByteValue) >> 1u) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_RXIPDU_NOTIFCBK(ByteValue)             Bfx_GetBit_u8u8_u8((ByteValue),1u)
#endif

#if defined (COM_SIGNALGATEWAY) || defined (COM_SIGNALGROUPGATEWAY)
/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the information if the Rx IPDU is Gw Pdu (has Gw signals or Gw Group signals)
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_RXIPDU_ISGWPDU(ByteValue)            ((uint8)((ByteValue) >> 2u)  & (uint8)(0x01u))
# else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1         : 8-bit Base variable, where should be read from.
 * Argument 2         : bit position from, where the bit shall be read
 * Return value       : Returns the bit value
 * */
#  define Com_GetValue_RXIPDU_ISGWPDU(ByteValue)            Bfx_GetBit_u8u8_u8((ByteValue),2u)
# endif /* #ifndef COM_ARSRVLIB_SUPPORT */
#endif /* #if defined (COM_SIGNALGATEWAY) || defined (COM_SIGNALGROUPGATEWAY) */

/* @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns boolean if the Ipdu is referring any IpduGroup
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_RXIPDU_PDUWITHPDUGRP(ByteValue)       ((uint8)((ByteValue) >> 3u) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
# define Com_GetValue_RXIPDU_PDUWITHPDUGRP(ByteValue)       Bfx_GetBit_u8u8_u8((ByteValue),3u)
#endif

/* @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns 'true' if IPDU is configured as TP otherwise 'false' for NORMAL type
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_RXIPDU_IS_TP_TYPE(ByteValue)          ((uint8)((ByteValue) >> 4u) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
# define Com_GetValue_RXIPDU_IS_TP_TYPE(ByteValue)          Bfx_GetBit_u8u8_u8((ByteValue),4u)
#endif

/* @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns 'true' if TP IPDU contains signal(group) with configured invalidAction
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_RXIPDU_TP_INV_CFG(ByteValue)         ((uint8)((ByteValue) >> 5u) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
# define Com_GetValue_RXIPDU_TP_INV_CFG(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),5u)
#endif

/* @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns 'true' if IPDU contains Rx metadata
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_RXIPDU_IS_METADATAPDU(ByteValue)         ((uint8)((ByteValue) >> 6u) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
# define Com_GetValue_RXIPDU_IS_METADATAPDU(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),6u)
#endif


/*
    typedef struct
    {
        uint8 sigAckCbk:1;                  [Bit0] set, if signal-notification/"ACK"-callback is configured
        uint8 sigGrpAckCbk:1;               [Bit1] set, if sigGrp-notification/"ACK"-callback is configured
        uint8 sigToutCbk:1;                 [Bit2] set, if signal-notification/"TOUT"-callback is configured
        uint8 sigGrpToutCbk:1;              [Bit3] set, if sigGrp-notification/"TOUT"-callback is configured
        uint8 Not_Used:4;                   Unused bitfields
    } Com_RxIpduVarFields_u8;
*/
/*
 * This Macro unpacks and returns 'true' if RX IPDU contains at least one signal with ACK callback
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_RXIPDUVAR_IS_SIG_ACKCBK(ByteValue)        (((uint8)(ByteValue)) & ((uint8)0x01u))
#else
# define Com_GetValue_RXIPDUVAR_IS_SIG_ACKCBK(ByteValue)        Bfx_GetBit_u8u8_u8((ByteValue),0u)
#endif
/*
 * This Macro unpacks and returns 'true' if RX IPDU contains at least one signal-group with ACK callback
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_RXIPDUVAR_IS_SIGGRP_ACKCBK(ByteValue)     (((uint8)((ByteValue) >> 1u)) & ((uint8)0x01u))
#else
# define Com_GetValue_RXIPDUVAR_IS_SIGGRP_ACKCBK(ByteValue)     Bfx_GetBit_u8u8_u8((ByteValue),1u)
#endif
/*
 * This Macro unpacks and returns 'true' if RX IPDU contains at least one signal or signal-group with ACK callback
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_RXIPDUVAR_IS_ACKCBK(ByteValue)            ((((uint8)(ByteValue)) & ((uint8)0x03u)) > 0u)
#else
# define Com_GetValue_RXIPDUVAR_IS_ACKCBK(ByteValue)            (Bfx_GetBits_u8u8u8_u8((ByteValue),0u,2u) > 0u)
#endif
/*
 * This Macro unpacks and returns 'true' if RX IPDU contains at least one signal with timeout callback
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_RXIPDUVAR_IS_SIG_TOUTCBK(ByteValue)        (((uint8)((ByteValue) >> 2u)) & ((uint8)0x01u))
#else
# define Com_GetValue_RXIPDUVAR_IS_SIG_TOUTCBK(ByteValue)        Bfx_GetBit_u8u8_u8((ByteValue),2u)
#endif
/*
 * This Macro unpacks and returns 'true' if RX IPDU contains at least one signal-group with timeout callback
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_RXIPDUVAR_IS_SIGGRP_TOUTCBK(ByteValue)     (((uint8)((ByteValue) >> 3u)) & ((uint8)0x01u))
#else
# define Com_GetValue_RXIPDUVAR_IS_SIGGRP_TOUTCBK(ByteValue)     Bfx_GetBit_u8u8_u8((ByteValue),3u)
#endif
/*
 * This Macro unpacks and returns 'true' if RX IPDU contains at least one signal or signal-group with timeout callback
 */
#ifndef COM_ARSRVLIB_SUPPORT
# define Com_GetValue_RXIPDUVAR_IS_TOUTCBK(ByteValue)            ((((uint8)((ByteValue) >> 2u)) & ((uint8)0x03u)) > 0u)
#else
# define Com_GetValue_RXIPDUVAR_IS_TOUTCBK(ByteValue)            (Bfx_GetBits_u8u8u8_u8((ByteValue),0u,2u) > 0u)
#endif


/* FC_VariationPoint_START */
/*
    typedef struct
    {
        uint16 DataInvalidType:2;    [Bit1-Bit0] Data Invalid Action
        uint16 FilterAlgorithm:4;    [Bit5-Bit2] Filter Algorithm
        uint16 DataTimeoutType:2;    [Bit7-Bit6] DataTimeout Action
        uint16 IsGwSignal:1;         [Bit8] Is gateway signal
        uint16 unused:7;
    } Com_RxSigPropType;
*/
/* FC_VariationPoint_END */

/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Rx Signal Invalid Action
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXSIG_INVACTION(ByteValue)         (((uint16)(ByteValue)) & ((uint16)0x03u))
#else
/* Bfx_GetBits_u16u8u8_u16 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value    : Returns the bit pattern
 * */
#define Com_GetValue_RXSIG_INVACTION(ByteValue)         Bfx_GetBits_u16u8u8_u16((ByteValue),0u,2u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Signal Filter algorithm of a RX Signal
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXSIG_FILTRALG(ByteValue)         ((uint16)((ByteValue) >> 2u) & (uint16)(0x0Fu))
#else
/* Bfx_GetBits_u16u8u8_u16 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value    : Returns the bit pattern
 * */
#define Com_GetValue_RXSIG_FILTRALG(ByteValue)         Bfx_GetBits_u16u8u8_u16((ByteValue),2u,4u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Rx Signal or Signal group Timeout Action
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXSIG_TOACTION(ByteValue)         (((uint16)((ByteValue) >> 6u)) & ((uint16)0x03u))
#else
/* Bfx_GetBits_u16u8u8_u16 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value    : Returns the bit pattern
 * */
#define Com_GetValue_RXSIG_TOACTION(ByteValue)         Bfx_GetBits_u16u8u8_u16((ByteValue),6u,2u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the
 * COM_TRUE = If Rx signal is ComGw Src signal
 * COM_FALSE = If Rx signal is not ComGw Src signal
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXSIG_ISGWSIG(ByteValue)         (((uint16)((ByteValue) >> 8u)) & ((uint16)0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_RXSIG_ISGWSIG(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),8u)
#endif

/* FC_VariationPoint_START */
/*
    typedef struct
    {
        uint16 TransProp       : 3;    [Bit2-Bit0] Transfer Property of signal
        uint16 FilterAlgorithm : 4;    [Bit6-Bit3] Filter Algorithm of signal
        uint16 DataInvalidType : 1;    [Bit7] DataInvalid Action
        uint16 TimeOutEnabled  : 1;    [Bit8] Check If time out is configured
    } Com_TxSigPropType;
 */
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Transfer property of a Tx Signal
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXSIG_TP(ByteValue)         ((uint16)(ByteValue) & (uint16)(0x07u))
#else
/* Bfx_GetBits_u16u8u8_u16 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value  : Selected bitfield sequence as uint16
 * */
#define Com_GetValue_TXSIG_TP(ByteValue)          Bfx_GetBits_u16u8u8_u16((ByteValue),0u,3u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Signal Filter algorithm of a Tx/RX Signal
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXSIG_FILTRALG(ByteValue)         ((uint16)((ByteValue) >> 3u) & (uint16)(0x0Fu))
#else
/* Bfx_GetBits_u16u8u8_u16 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 16-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value  : Selected bitfield sequence as uint16
 * */
#define Com_GetValue_TXSIG_FILTRALG(ByteValue)         Bfx_GetBits_u16u8u8_u16((ByteValue),3u,4u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns Invalidation status for a Tx Signal
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXSIG_INVACTION(ByteValue)         ((uint16)((ByteValue) >> 7u) & ((uint16)0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 16-bit variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value boolean  : Returns the bit value from Data at position BitPn
 * */
#define Com_GetValue_TXSIG_INVACTION(ByteValue)        Bfx_GetBit_u16u8_u8((ByteValue),7u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Tx Signal or Signal group Timeout enable status
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXSIG_TOENABLED(ByteValue)         (((uint16)((ByteValue) >> 8u)) & ((uint16)0x01u))
#else
/* Bfx_GetBit_u16u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXSIG_TOENABLED(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),8u)
#endif

/* FC_VariationPoint_START */
/*
    typedef struct
    {
         uint8 TransProp     :3;     [Bit2-Bit0] Transfer Property of a SignalGroup
         uint8 TimeOutEnabled:1;     [Bit3] is time out is configured for the signal group
         uint8 FilterEvalReq :1;     [Bit4] filter configured for anyone of its group signal
         uint8 UpdBitConf    :1;     [Bit5] is update bit configured for signal group
         uint8 ArrayAccess   :1;     [Bit6] is "Array Access" enabled for signal group
         uint8 Not_Used      :1;     Unused Bit
    } Com_TxSigGrpPropType;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Transfer property of a Tx Signal Group
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXSIGGRP_TP(ByteValue)         ((uint8)(ByteValue) & (uint8)(0x07u))
#else
/* Bfx_GetBits_u8u8u8_u8 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value  : Returns the bit pattern
 * */
#define Com_GetValue_TXSIGGRP_TP(ByteValue)         Bfx_GetBits_u8u8u8_u8((ByteValue),0u,3u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Tx Signal group Timeout enable status
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXSIGGRP_TOENABLED(ByteValue)         (((uint8)((ByteValue) >> 3u)) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXSIGGRP_TOENABLED(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),3u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Tx Signal group Filter Evaluation required status
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXSIGGRP_FILTEREVALREQ(ByteValue)         (((uint8)((ByteValue) >> 4u)) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXSIGGRP_FILTEREVALREQ(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),4u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Tx Signal group UpdateBit configured status
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXSIGGRP_UPDBITCONF(ByteValue)         (((uint8)((ByteValue) >> 5u)) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXSIGGRP_UPDBITCONF(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),5u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Tx Signal group Array Access enable status
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXSIGGRP_ARRAYACCESS(ByteValue)         (((uint8)((ByteValue) >> 6u)) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value  : Returns the bit value
 * */
#define Com_GetValue_TXSIGGRP_ARRAYACCESS(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),6u)
#endif


/* FC_VariationPoint_START */
/*
    typedef struct
    {
        uint8 DataInvalidType:2; [Bit1-Bit0] DataInvalid Action
        uint8 DataTimeoutType:2; [Bit3-Bit2] DataTimeout Action
        uint8 UpdBitConf:1;      [Bit4] Is UpdateBit set
        uint8 ArrayAccess:1;     [Bit5] Is SignalGroup Array Access enabled
        uint8 FilterEvalReq:1;   [Bit6] Is Filter processing required at any of its GroupSignal
        uint8 IsGwSignal:1;      [Bit7] SignalGroup Gateway
    } Com_RxSigGrpPropertyType;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Rx Signal Invalid Action
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXSIGGRP_INVACTION(ByteValue)         (((uint8)(ByteValue)) & ((uint8)0x03u))
#else
/* Bfx_GetBits_u8u8u8_u8 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value    : Returns the bit pattern
 * */
#define Com_GetValue_RXSIGGRP_INVACTION(ByteValue)         Bfx_GetBits_u8u8u8_u8((ByteValue),0u,2u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Rx Signal group Timeout Action
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXSIGGRP_TOACTION(ByteValue)         (((uint8)((ByteValue) >> 2u)) & ((uint8)0x03u))
#else
/* Bfx_GetBits_u8u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value    : Returns the bit pattern
 * */
#define Com_GetValue_RXSIGGRP_TOACTION(ByteValue)         Bfx_GetBits_u8u8u8_u8((ByteValue),2u,2u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Rx Signal group UpdateBit configured status
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXSIGGRP_UPDBITCONF(ByteValue)       (((uint8)((ByteValue) >> 4u)) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 8-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#define Com_GetValue_RXSIGGRP_UPDBITCONF(ByteValue)       Bfx_GetBit_u8u8_u8((ByteValue),4u)
#endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Rx Signal group Array Access enabled status
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_RXSIGGRP_ARRAYACCESS(ByteValue)       (((uint8)((ByteValue) >> 5u)) & ((uint8)0x01u))
# else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 8-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#  define Com_GetValue_RXSIGGRP_ARRAYACCESS(ByteValue)       Bfx_GetBit_u8u8_u8((ByteValue),5u)
# endif

/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Rx Signal group Filter Evaluation required status
 */
# ifndef COM_ARSRVLIB_SUPPORT
#  define Com_GetValue_RXSIGGRP_FILTEREVALREQ(ByteValue)         (((uint8)((ByteValue) >> 6u)) & ((uint8)0x01u))
# else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value  : Returns the bit value
 * */
#  define Com_GetValue_RXSIGGRP_FILTEREVALREQ(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),6u)
# endif

/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Rx Signal group gateway enabled status
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXSIGGRP_ISGWSIGGRP(ByteValue)       (((uint8)((ByteValue) >> 7u)) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1   : 8-bit Base variable, where should be read from.
 * Argument 2   : bit position from, where the bit shall be read
 * Return value : Returns the bit value
 * */
#define Com_GetValue_RXSIGGRP_ISGWSIGGRP(ByteValue)       Bfx_GetBit_u8u8_u8((ByteValue),7u)
#endif

/* FC_VariationPoint_START */
/*
    typedef struct
    {
        uint8 Type:5;        [Bit4-Bit0] DataType
        uint8 Endianess:1;   [Bit5] Endianess
        uint8 UpdBitConf:1;  [Bit6] IsUpdateBitConfigured
        uint8 Not_Used:1;
    } Com_GeneralType;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Signal Type of a Tx/RX Signals and Group Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_GEN_TYPE(ByteValue)         ((uint8)(ByteValue) & (uint8)(0x1Fu))
#else
/* Bfx_GetBits_u8u8u8_u8 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value  : Returns the bit pattern
 * */
#define Com_GetValue_GEN_TYPE(ByteValue)         Bfx_GetBits_u8u8u8_u8((ByteValue),0u,5u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Signal Endianess of a Tx/Rx Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_GEN_ENDIANESS(ByteValue)         ((uint8)((ByteValue) >> 5u) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value  : Returns the bit value
 * */
#define Com_GetValue_GEN_ENDIANESS(ByteValue)         (Bfx_GetBit_u8u8_u8((ByteValue),5u) ? \
                                                       COM_BIGENDIAN : COM_LITTLEENDIAN)
#endif

/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Signal Endianess of a Tx/RX Signals and Group Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_GEN_UPDBITCONF(ByteValue)         ((uint8)((ByteValue) >> 6u) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value  : Returns the bit value
 * */
#define Com_GetValue_GEN_UPDBITCONF(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),6u)
#endif


/* FC_VariationPoint_START */
/*
    typedef struct
    {
        uint8 Type:5;           [Bit4-Bit0] DataType
        uint8 Endianess:1;      [Bit5] Endianess of Grp signal
        uint8 isTrigOnChange:1; [Bit6] does group signal contributes to the evaluation of signal group transfer property
        uint8 isInvalidCfg:1;   [Bit7] Indicates if the group signal has configured invalid value

    } Com_TxGrpSigPropertyType;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Signal Type of a Tx/RX Signals and Group Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXGRPSIG_TYPE(ByteValue)         ((uint16)(ByteValue) & (uint16)(0x1Fu))
#else
/* Bfx_GetBits_u8u8u8_u8 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value    : Returns the bit pattern
 * */
#define Com_GetValue_TXGRPSIG_TYPE(ByteValue)         Bfx_GetBits_u16u8u8_u16((ByteValue),0u,5u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns the Signal Endianess of a Tx Group Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXGRPSIG_ENDIANESS(ByteValue)         ((uint16)((ByteValue) >> 5u) & (uint16)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXGRPSIG_ENDIANESS(ByteValue)         (Bfx_GetBit_u16u8_u8((ByteValue),5u) ? \
                                                            COM_BIGENDIAN : COM_LITTLEENDIAN)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Tx Group Signal Transfer property
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXGRPSIG_IS_TOC(ByteValue)         ((uint16)((ByteValue) >> 6u) & (uint16)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXGRPSIG_IS_TOC(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),6u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 *  This Macro unpacks and returns Invalidation status for a Tx Group signal
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_TXGRPSIG_INVACTION(ByteValue)         ((uint16)((ByteValue) >> 7u) & ((uint16)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value    : Returns the bit value
 * */
#define Com_GetValue_TXGRPSIG_INVACTION(ByteValue)         Bfx_GetBit_u16u8_u8((ByteValue),7u)
#endif


/* FC_VariationPoint_START */
/*
    typedef struct
    {
        uint8 Type:5;               [Bit4-Bit0] DataType
        uint8 Endianess:1;          [Bit5] Endianess of grp signal
        uint8 GrpSigInvalid:1;      [Bit6] Has group signal configured invalid value
        uint8 unused:1;
    } Com_RxGrpSigPropertyType;
*/
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Signal Type of a Tx/RX Signals and Group Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXGRPSIG_TYPE(ByteValue)         ((uint8)(ByteValue) & (uint8)(0x1Fu))
#else
/* Bfx_GetBits_u8u8u8_u8 : Returns a sequence of the variable Data in form of a bitfield.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : Position of the first bit to be read out
 * Argument 3    : Length of the section, which should be read.
 * Return value  : Returns the bit pattern
 * */
#define Com_GetValue_RXGRPSIG_TYPE(ByteValue)         Bfx_GetBits_u8u8u8_u8((ByteValue),0u,5u)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns the Signal Endianess of a Rx Group Signals
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXGRPSIG_ENDIANESS(ByteValue)         ((uint8)((ByteValue) >> 5u) & (uint8)(0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value  : Returns the bit value
 * */
#define Com_GetValue_RXGRPSIG_ENDIANESS(ByteValue)         (Bfx_GetBit_u8u8_u8((ByteValue),5u) ? \
                                                            COM_BIGENDIAN : COM_LITTLEENDIAN)
#endif


/**
 * @ingroup COM_PRV_H
 *
 * This Macro unpacks and returns Invalidation status for a Tx Group signal
 */
#ifndef COM_ARSRVLIB_SUPPORT
#define Com_GetValue_RXGRPSIG_INVACTION(ByteValue)         ((uint8)((ByteValue) >> 6u) & ((uint8)0x01u))
#else
/* Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
 * Argument 1    : 8-bit Base variable, where should be read from.
 * Argument 2    : bit position from, where the bit shall be read
 * Return value  : Returns the bit value
 * */
#define Com_GetValue_RXGRPSIG_INVACTION(ByteValue)         Bfx_GetBit_u8u8_u8((ByteValue),6u)
#endif


#define Com_SetUpdateBitValue(PduPtr,UpdBitPos,ByteNo)                                                          \
        /* Bfx_SetBit_u8u8 : Sets a bit at a defined position BitPn.
         * Argument 1    : pointer to the 8-bit variable, to be modified.
         * Argument 2    : position to be set
         * (UpdBitPos - (Com_Bitposition_tuo)(ByteNo * 8)) =  Get the position of update bit in the byte
         * */                                                                                                   \
        Com_Setbit((uint8 *)((PduPtr)->buffPtr_pu8 + (ByteNo)),(uint8)((UpdBitPos) - (Com_Bitposition_tuo)((ByteNo) * 8u)));

#define Com_ClearUpdateBitValue(PduPtr,UpdBitPos,ByteNo)                                                        \
            /* Bfx_ClrBit_u8u8 : Clears a bit at a defined position BitPn.
             * Argument 1    : pointer to the 8-bit variable, to be modified.
             * Argument 2    : position to be cleared
             * (UpdBitPos - (Com_Bitposition_tuo)(ByteNo * 8)) ->  Get the position of update bit in the byte
             * */                                                                                               \
        Com_Clearbit((uint8 *)((PduPtr)->buffPtr_pu8 + (ByteNo)),(uint8)((UpdBitPos) - (Com_Bitposition_tuo)((ByteNo) * 8u)));

#define Com_GetUpdateBitValue(dataPtr,UpdBitPos,ByteNo)                                                         \
        /* Bfx_GetBit_u8u8_u8 : Bfx_GetBit_u8u8_u8 : Gets a bit at the variable Data at a described position BitPn.
             * Argument 1    : 8-bit Base variable, where should be read from.
             * ((dataPtr) + (ByteNo)) -> Data pointer pointing to the byte with update bit position
             * Argument 2    : bit position from, where the bit shall be read
             * ((UpdBitPos) - (Com_Bitposition_tuo)((ByteNo) * 8)))-> Get the position of update bit in the byte
             * Return value    : Returns the bit value
             * */                                                                                               \
        Com_Getbit(*((uint8 *)((dataPtr) + (ByteNo))),(uint8)((UpdBitPos) - (Com_Bitposition_tuo)((ByteNo) * 8u)))

#ifdef COM_ARSRVLIB_SUPPORT
#define Com_Clearbit(ByteValue, Bitpos) Bfx_ClrBit_u8u8(ByteValue,Bitpos)
#else
#define Com_Clearbit(ByteValue, Bitpos) (*ByteValue) = ((uint8)(*ByteValue) & (~((uint8)(1u << (Bitpos)))))
#endif /* #ifdef COM_ARSRVLIB_SUPPORT */

#ifdef COM_ARSRVLIB_SUPPORT
#define Com_Setbit(ByteValue, Bitpos) Bfx_SetBit_u8u8(ByteValue,Bitpos)
#else
#define Com_Setbit(ByteValue, Bitpos) (*ByteValue) = ((uint8)(*ByteValue) | ((uint8)(1u << (Bitpos))))
#endif /* #ifdef COM_ARSRVLIB_SUPPORT */

#ifdef COM_ARSRVLIB_SUPPORT
#define Com_Getbit(ByteValue, Bitpos) Bfx_GetBit_u8u8_u8(ByteValue,Bitpos)
#else
#define Com_Getbit(ByteValue, Bitpos) (((ByteValue) & ((uint8)(1u << (Bitpos)))) != 0u)
#endif /* #ifdef COM_ARSRVLIB_SUPPORT */

/**
 * @ingroup COM_PRV_H
 *
 * Macro to concatenate ELEMENTTYPE,PROP with Com_GetValue_ and pass ByteValue
 */
#define     Com_GetValue(ELEMENTTYPE,PROP,ByteValue)            Com_GetValue_##ELEMENTTYPE##PROP(ByteValue)
/**
 * @ingroup COM_PRV_H
 *
 * Macro to concatenate ELEMENTTYPE,PROP with Com_GetRamValue_ and pass ByteValue
 */
#define     Com_GetRamValue(ELEMENTTYPE,PROP,ByteValue)         Com_GetRamValue_##ELEMENTTYPE##PROP(ByteValue)
/**
 * @ingroup COM_PRV_H
 *
 * Macro to concatenate ELEMENTTYPE,PROP with Com_SetRamValue_ and pass ByteValue,Value
 */
#define     Com_SetRamValue(ELEMENTTYPE,PROP,ByteValue,Value)   Com_SetRamValue_##ELEMENTTYPE##PROP(ByteValue,Value)


 /**
  * @ingroup COM_PRV_H
  *
  * Macro to check if Rx Ipdu deadline monitoring status is Started /stopped
  */
#define COM_CheckRxIPduDMStatus(IpduId) \
            (Com_GetRamValue(RXIPDU,_DMSTATUS,COM_GET_RX_IPDU_CONSTDATA(IpduId)->ipduRam_pst->rxFlags_u16) != COM_STOP)

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define COM_START_SEC_CODE
#include "Com_MemMap.h"

#if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE)

void Com_Prv_TxIPduAckCallbacks(Com_IpduId_tuo idIpdu_uo);
void Com_Prv_TxIPduErrCallbacks(Com_IpduId_tuo idIpdu_uo);
void Com_Prv_TxIPduToutCallbacks(Com_IpduId_tuo idIpdu_uo);
void Com_Prv_RxIPduToutCallbacks(Com_IpduId_tuo idIpdu_uo);

#endif /* #if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE) */

 /* FC_VariationPoint_START */
#ifdef COM_RX_IPDUCONTROL_VIA_CALIBRATION
 /**
  * @ingroup COM_PRV_H
  *
  *  Service check if the Calibration variable value is set     \n
  *
  *  @param  idIpdu_uo - Id of the Tx Ipdu   \n
  *
  *  @return Boolean : Status value of the calibration variable \n
  */
boolean Com_Prv_CheckRxIPduCalibrationVarStatus(PduIdType idIpdu_uo);

#endif /* #ifdef COM_RX_IPDUCONTROL_VIA_CALIBRATION */

#ifdef COM_TX_IPDUCONTROL_VIA_CALIBRATION
 /**
  * @ingroup COM_PRV_H
  *
  *  Service check if the Calibration variable value is set     \n
  *
  *  @param  idIpdu_uo - Id of the Rx Ipdu   \n
  *
  *  @return Boolean : Status value of the calibration variable \n
  */
boolean Com_Prv_CheckTxIPduCalibrationVarStatus(PduIdType idIpdu_uo);

#endif /* #ifdef COM_TX_IPDUCONTROL_VIA_CALIBRATION */

#ifdef COM_TXPDU_TIMEPERIOD_VIA_CALIBRATION
uint16 Com_Prv_GetTxIPduCyclePeriod(PduIdType idIpdu_uo);
#endif /* #ifdef COM_TXPDU_TIMEPERIOD_VIA_CALIBRATION */

#ifdef COM_RXPDU_FIRSTTIMEOUT_VIA_CALIBRATION
uint16 Com_Prv_GetRxIPduFirstTimeoutCalibration(PduIdType idIpdu_uo);
#endif /* #ifdef COM_RXPDU_FIRSTTIMEOUT_VIA_CALIBRATION */

#ifdef COM_RXPDU_TIMEOUT_VIA_CALIBRATION
uint16 Com_Prv_GetRxIPduTimeoutCalibration(PduIdType idIpdu_uo);
#endif /* #ifdef COM_RXPDU_TIMEOUT_VIA_CALIBRATION */

#ifdef COM_TXPDU_TIMEOFFSET_VIA_CALIBRATION
uint16 Com_Prv_GetTxIPduTimeOffsetCalibration(PduIdType idIpdu_uo);
#endif /* #ifdef COM_TXPDU_TIMEOFFSET_VIA_CALIBRATION */

#ifdef COM_TXPDU_NUMOFREPETITION_VIA_CALIBRATION
uint8 Com_Prv_GetTxIPduNumberOfRepetitionCalibration(PduIdType idIpdu_uo);
#endif /* #ifdef COM_TXPDU_NUMOFREPETITION_VIA_CALIBRATION */

/* FC_VariationPoint_END */

 /**
  * @ingroup COM_PRV_H
  *
  *  Service for copying the bytes from source to destination              \n
  *
  *  @param  dest_pu8   - Pointer to Destination           \n
  *  @param  src_pcu8   - Pointer to source \n
  *  @param  len_u32    - No.of bytes   \n
  *
  *  @return None \n
  */
void Com_ByteCopy( uint8 * dest_pu8, const uint8 * src_pcu8, uint32 len_u32 );
/**
 * @ingroup COM_PRV_H
 *
 *  Internal function used to copy the data for unsigned signal type              \n
 *
 *  @param  idSignal_u16        - signal id           \n
 *  @param  signalDataPtr_pv    - Pointer to the the signal data \n
 *
 *  @return None \n
 */
void Com_Prv_CopyData_Sig_UnsignedType(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv);
/**
 * @ingroup COM_PRV_H
 *
 *  Internal function used to copy the data for signed signal type              \n
 *
 *  @param  idSignal_u16        - signal id           \n
 *  @param  signalDataPtr_pv    - Pointer to the the signal data \n
 *
 *  @return None \n
 */
void Com_Prv_CopyData_Sig_SignedType(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv);
/**
 * @ingroup COM_PRV_H
 *
 *  Internal function used to copy SignalGroup content from IpduBuffer to Shadowbuffer              \n
 *
 *  @param  idSignalGroup_u16   - signal group id           \n
 *
 *  @return None \n
 */
void Com_Prv_ReceiveSignalGroup(Com_SignalGroupIdType idSignalGroup_u16);
/**
 * @ingroup COM_PRV_H
 *
 *  Internal function used for packing the signal into Ipdubuffer              \n
 *
 *  @param  endianess_u8 - Little Endian/Big Endian           \n
 *  @param  bitPos_uo    - Position of the signal in the ipdu \n
 *  @param  bitSize_uo   - Size of the signal   \n
 *  @param  srcBuf_uo    - Buffer where signal data is available \n
 *  @param  destBuf_pu8  - Pointer to buffer where the signal data needs to be packed \n
 *
 *  @return None \n
 */
void Com_Prv_PackSignal( uint8 endianess_u8, Com_Bitposition_tuo  bitPos_uo, Com_Bitsize_tuo bitSize_uo, Com_SigMax_tuo srcBuf_uo,
                                                                                                   uint8 * destBuf_pu8 );
/* FC_VariationPoint_START */
/**
 * @ingroup COM_PRV_H
 *
 *  Internal function used for packing the float64 signal/group signal into Ipdubuffer              \n
 *
 *  @param  Endianess - Little Endian/Big Endian           \n
 *  @param  Bit_Pos -   Position of the signal in the ipdu \n
 *  @param  Src_Buf -   Buffer where signal data is available \n
 *  @param  Dest_Buf -  ponter to buffer where the signal data needs to be packed \n
 *
 *  @return None \n
 */
#if (defined(COM_TXSIG_FLOAT64SUPP) || defined(COM_TXGRPSIG_FLOAT64SUPP) ||\
    (defined(COM_RX_SIGNALGROUP_ARRAY) && defined(COM_RXGRPSIG_FLOAT64SUPP)))
void Com_Prv_PackFloatSignal( uint8 endianess_u8, Com_Bitposition_tuo bitPos_uo, float64 src_f64, uint8 * destBuf_pu8 );
#endif
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 *  Internal function used for unpacking the signal from Ipdu Buffer              \n
 *
 *  @param  endianess_u8  - Little Endian/Big Endian           \n
 *  @param  bitPos_uo     - Position of the signal in the ipdu \n
 *  @param  bitSize_uo    - Size of the signal   \n
 *  @param  srcBuf_pcu8   - Buffer where signal data is available \n
 *  @param  sigType_u8    - signal type used for sign extension of negative values \n
 *
 *  @return None \n
 */
Com_SigMax_tuo Com_Prv_UnpackSignal(
                            uint8                       endianess_u8,
                            Com_Bitposition_tuo         bitPos_uo,
                            Com_Bitsize_tuo             bitSize_uo,
                            const uint8 *               srcBuf_pcu8,
                            uint8                       sigType_u8
                               );

/* FC_VariationPoint_START */
/**
 * @ingroup COM_PRV_H
 *
 *  Internal function used for unpacking the signal from Ipdu Buffer              \n
 *
 *  @param  endianess_u8 - Little Endian/Big Endian           \n
 *  @param  bitPos_uo    - Position of the signal in the ipdu \n
 *  @param  srcBuf_pcu8  - Buffer where signal data is available \n
 *
 *  @return float64 \n
 */
#if defined (COM_RXSIG_FLOAT64SUPP ) || defined(COM_RXGRPSIG_FLOAT64SUPP)
float64 Com_Prv_UnPackFloatSignal(uint8 endianess_u8, Com_Bitposition_tuo bitPos_uo, const uint8 * srcBuf_pcu8);
#endif /* #if defined (COM_RXSIG_FLOAT64SUPP ) || defined(COM_RXGRPSIG_FLOAT64SUPP) */
/* FC_VariationPoint_END */
/**
 * @ingroup COM_PRV_H
 *
 *  Internal function used for Transmitting Tx Ipdu's              \n
 *
 *  @param  idTxPdu_uo- Id of the Tx ipdu,Timeouts           \n
 *
 *  @return None \n
 */
void Com_Prv_SendIpdu(PduIdType idTxPdu_uo, Com_SendIpduInfo_tst sendIpduFlag_st);
/**
 * @ingroup COM_PRV_H
 *
 *  Internal function used for Transmission Mode Selector              \n
 *
 *  @param   idTxIpdu_uo- Id of the Tx ipdu           \n
 *
 *  @return None \n
 */
void Com_Prv_TxChangeMode(Com_IpduId_tuo idTxIpdu_uo);

#ifdef COM_TxFilters
/**
 * @ingroup COM_PRV_H
 *
 *  This function evaluates the current TMS of the IPDU, based on the corresponding signal which is updated.   \n
 *
 *  @param   idSignal_u16 - Id of the signal           \n
 *  @return  COM_TRUE
 *  @return  COM_FALSE
 */
boolean Com_Prv_SigTxChangeMode(Com_SignalIdType idSignal_u16, uint32 txNewVal_u32);

#ifdef COM_TX_SIGNALGROUP
/**
 * @ingroup COM_PRV_H
 *
 *  This function evaluates the current TMS of the IPDU, based on the corresponding signal group which is updated.   \n
 *  @param   idSigGroup_u16 - Id of the signal Group          \n
 *  @return  COM_TRUE
 *  @return  COM_FALSE
 */
boolean Com_Prv_SigGrpTxChangeMode(Com_SignalGroupIdType idSigGroup_u16);

#endif /* #ifdef COM_TX_SIGNALGROUP */

#endif /* #ifdef COM_TxFilters */

/**
 * @ingroup COM_PRV_H
 *
 *  Internal function to process rx-signal's in the received Pdu \n
 *
 *  @param   idRxPdu_uo    - received Pdu-Id \n
 *  @param   pduInfoPtr_pcst - received Pdu info pointer \n
 *
 *  @return None \n
 */
void Com_Prv_ProcessSignal(PduIdType idRxPdu_uo, const PduInfoType * pduInfoPtr_pcst);

/**
 * @ingroup COM_PRV_H
 *
 *  Internal function to process rx-signal groups's in the received Pdu \n
 *
 *  @param   idRxPdu_uo    - received Pdu-Id \n
 *  @param   pduInfoPtr_pcst - received Pdu info pointer \n
 *
 *  @return None \n
 */
#ifdef COM_RX_SIGNALGROUP

void Com_Prv_ProcessSignalGroup(PduIdType idRxPdu_uo, const PduInfoType * pduInfoPtr_pcst);

#endif /* #ifdef COM_RX_SIGNALGROUP */

#ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
/* Service for receiving the dynamic signal, this API copies the data to the application buffer */
extern uint8  Com_Prv_ReceiveDynSignal(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv, uint16 * lengthPtr_pu16);
#endif

#ifdef COM_RX_SIGNALGROUP

/**
 * @ingroup COM_PRV_H
 *
 *  Internal function to receiving the shadow Signal \n
 *
 *  @param   idSignal_u16      - Id of the signal \n
 *  @param   signalDataPtr_pv  - Pointer to the the signal data \n
 *
 *  @return None \n
 */
void Com_Prv_ReceiveShadowSignal(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv);
/**
 * @ingroup COM_PRV_INL_H
 *
 * Function to initialize Rx-signal group with initial values
 */

void Com_Prv_RxReplaceSigGrp(
                        Com_RxSigGrpCfg_tpcst   rxSigGrpConstPtr_pcst,
                        Com_MainFunc_tuo        idRxMainFunc_uo,
                        uint8                   replaceAction_u8
                            );

#ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
/* Service for receiving the dynamic group signal, this API copies the data to the application buffer */
extern uint8 Com_Prv_ReceiveDynGroupSignal(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv, uint16 * lengthPtr_pu16);
#endif

#endif /* #ifdef COM_RX_SIGNALGROUP */

#ifdef COM_TX_DYNAMIC_SIGNAL_SUPPORT
extern uint8 Com_Prv_SendDynSignal(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv, uint16 length_u16);
#endif

#ifdef COM_TX_SIGNALGROUP
    /**
     * @ingroup COM_PRV_H
     *
     *  Internal function used for copying the Signal Group data from the shadow buffer to IPDU buffer       \n
     *
     *  @param  idSignalGrp_u16 - Id of the SignalGroup       \n
     *
     *  @return None \n
     */
void Com_Prv_ShadowBuffToIpduBuff(Com_SignalGroupIdType idSignalGrp_u16);

#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
extern uint8 Com_Prv_CopyDynGroupSignal(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv, uint16 length_u16);
#endif

#endif

# ifdef COM_EffectiveSigGrpTOC
/**
 * @ingroup COM_PRV_H
 *
 *  Service if GroupSignal data has changed from the last update             \n
 *
 *  @param   idSignalGroup_u16 - Id of the Signal group Id         \n
 *
 *  @return None \n
 */
void Com_Prv_InternalSigGrpArrayTrigOnChange(
                                             Com_SignalGroupIdType idSignalGroup_u16,
                                             const uint8 * signalGroupArrayPtr_pcu8
                                            );

# endif /* # ifdef COM_EffectiveSigGrpTOC */

#ifdef COM_TX_SIGNALGROUP
/**
 * @ingroup COM_PRV_H
 *
 *  Internal function used a group signal with the data, referenced by SignalDataPtr       \n
 *
 *  @param  idSignal_u16        - Id of the GroupSignal       \n
 *  @param  signalDataPtr_pcv   - Pointer to the address where application data is available       \n
 *
 *  @return None \n
 */
void Com_Prv_UpdateShadowSignal(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv);

#endif /* #ifdef COM_TX_SIGNALGROUP */

#ifdef COM_FILTERS
/**
 * @ingroup COM_PRV_H
 *
 *  Service used for filtering the signal depending on the conditions \n
 *
 *  @param  algorithm_u8  - Filter type \n
 *  @param  newVal_u32    - New value for filtering \n
 *  @param  idxFilter_uo  - Filter Index \n
 *  @param  notFromIpduGrpStart_b - Whether to increment occurrence for One_Every_N filters \n
 *
 *  @return   COM_TRUE
 *  @return   COM_FALSE
 */
boolean Com_Prv_Filter(uint32 newVal_u32, uint8 algorithm_u8, Com_Filter_tuo idxFilter_uo, boolean notFromIpduGrpStart_b);

#endif /* #ifdef COM_FILTERS */


/**
 * @ingroup COM_PRV_H
 *
 *  Service called by the lower layer after an I-PDU has been received.      \n
 *
 *  @param  idRxPdu_uo    - Id of the Rx ipdu       \n
 *  @param  pduInfoPtr_pcst - Pointer to the buffer where the received data is stored       \n
 *
 *  @return None \n
 */
void Com_Prv_ProcessRxIPdu(PduIdType idRxPdu_uo, const PduInfoType * pduInfoPtr_pcst);

/**
 * @ingroup COM_PRV_H
 *
 *  Internal function used for clearing the update bits of signals and signal groups in the IPdu      \n
 *
 *  @param  txIpduConstPtr_pcst - Pointer to Tx ipdu static configuration      \n
 *  @param  Buff_Add - Pointer to the buffer where the received data is stored       \n
 *
 *  @return None \n
 */
void Com_Prv_ClearUpdateBits(Com_TxIpduCfg_tpcst txIpduConstPtr_pcst);

#if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
# ifdef COM_ENABLE_MAINFUNCTION_RX

extern void Com_Prv_RxToGwFifo(Com_IpduId_tuo idIpdu_uo);

# endif /* end of COM_ENABLE_MAINFUNCTION_RX */

# ifdef COM_ENABLE_MAINFUNCTION_TX

extern void Com_Prv_GwFifoToTx(Com_MainFunc_tuo idTxMainFunc_uo);
extern void Com_Prv_PrepareGwTxIpdu(Com_IpduId_tuo idIpdu_uo);

# endif /* end of COM_ENABLE_MAINFUNCTION_TX */
#endif /* end of defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY) */

void Com_Prv_InternalTxConfirmation(PduIdType idTxPdu_uo);

#if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE)
extern void ComAppl_IncompatibleGenerator(void);
#endif

extern uint8 Com_Prv_InternalSendSignal(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv );
extern uint8 Com_Prv_InternalSendSignalGroup(Com_SignalGroupIdType idSignalGroup_u16);
#ifdef COM_TxGrpSigInvalid
extern uint8 Com_Prv_InternalInvalidateShadowSignal(Com_SignalIdType idSignal_u16);
#endif

#ifdef COM_ENABLE_MAINFUNCTION_RX
extern void Com_Prv_InternalMainFunctionRx(Com_MainFunc_tuo idRxMainFunc_uo );
#endif

extern void Com_Prv_InternalMainFunctionTx(Com_MainFunc_tuo idTxMainFunc_uo );

#if defined(COM_RxIPduTimeout) || defined(COM_RxSigUpdateTimeout) || defined(COM_RxSigGrpUpdateTimeout)
extern boolean Com_Prv_EnableRxDeadlineMonitoring(Com_IpduId_tuo idIpdu_uo);
#endif

#if defined(COM_RxIPduNotification) || defined(COM_RxSignalNotify) || defined(COM_RxSignalGrpNotify)
extern void Com_Prv_InvokeRxNotifications( PduIdType idRxPdu_uo );
#endif

#if defined(COM_RxIPduCallOuts) || defined(COM_RX_IPDUCOUNTER)
extern boolean Com_Prv_IsValidRxIpdu(PduIdType idPdu_uo, const PduInfoType * pduInfoPtr_pcst);
#endif

#ifdef COM_RXIPDU_TP_INVALIDATION
extern void Com_Prv_ExecuteRxInvalidActions(PduIdType idPdu_uo);
#endif

#if defined(COM_RxSigInvalid) || defined (COM_RxFilters)
extern boolean Com_Prv_ValidateRxSignal(Com_SignalIdType idSignal_u16, void * rxSrcPtr_pv);
#endif

#ifdef COM_RxFilters
extern boolean Com_Prv_EvaluateRxSigFilter(Com_SignalIdType idSignal_u16, Com_SigMax_tuo rxSigVal_uo);
#endif

extern Com_SigMax_tuo Com_Prv_RxReplaceSignal(
                                        Com_RxSigCfg_tpcst  rxSigConstPtr_pcst,
                                        Com_MainFunc_tuo    idRxMainFunc_uo,
                                        uint8               replaceAction_u8
                                             );

#ifdef COM_RxSigInvalid
extern boolean Com_Prv_IsRxSigReplacedToInit(Com_SignalIdType  idSignal_u16);
#endif

extern void Com_Prv_DisableReceptionDM(Com_IpduGroupIdType idIpduGrp_u16);
extern void Com_Prv_EnableReceptionDM(Com_IpduGroupIdType idIpduGrp_u16);

extern void Com_Prv_TxIPduStart(Com_IpduId_tuo idIpdu_uo, boolean initialize_b);
extern void Com_Prv_TxIPduStop(Com_IpduId_tuo idIpdu_uo);
extern void Com_Prv_RxIPduStart(Com_IpduId_tuo idIpdu_uo, boolean initialize_b);

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"


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

/*  START:  Global Variables declaration */

/* FC_VariationPoint_START */
#if (COM_COMMON_TX_PERIOD == STD_ON)
/**
 * @ingroup COM_PRV_H
 *
 * Value of reduced period
 */
#define COM_START_SEC_VAR_CLEARED_16
#include "Com_MemMap.h"
extern uint16 Com_ReducedPeriod_u16;
#define COM_STOP_SEC_VAR_CLEARED_16
#include "Com_MemMap.h"
#endif /* COM_COMMON_TX_PERIOD */
/* FC_VariationPoint_END */


#define COM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"

/**
 * @ingroup COM_PRV_H
 *
 * COM module status flag
 */
extern Com_StatusType Com_InitStatus_en;

#if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE)
/**
 * @ingroup COM_PRV_H
 *
 * Internal pointer to config structure
 */
extern const Com_ConfigData_tst *   Com_Prv_Config_pcst;
extern const Com_CommonData_tst *   Com_Prv_Common_pcst;
extern const Com_VariantData_tst *  Com_Prv_Variant_pcst;

#endif

#define COM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Com_MemMap.h"

/**
 * @ingroup COM_PRV_H
 *
 * Structure which contains Tx mode NONE related values
 */
#define COM_START_SEC_CONST_UNSPECIFIED
#include "Com_MemMap.h"
extern const Com_TransModeInfo_tst Com_NONE_TransModeInfo_cst;
#define COM_STOP_SEC_CONST_UNSPECIFIED
#include "Com_MemMap.h"

/*  END:  Global Variables declaration */

/* macro to get Rx SignalGroup ID offset */
#define COM_GET_RXSIGNALGRP_ID_OFFSET                         (COM_GET_NUM_RX_SIGNALS)
/* macro to get Tx SignalGroup ID offset */
#define COM_GET_TXSIGNALGRP_ID_OFFSET                         (COM_GET_NUM_TX_SIGNALS)
/* macro to get Rx GroupSignal ID offset */
#define COM_GET_RXGRPSIGNAL_ID_OFFSET                         (COM_GET_NUM_RX_SIGNALS)
/* macro to get Tx GroupSignal ID offset */
#define COM_GET_TXGRPSIGNAL_ID_OFFSET                         (COM_GET_NUM_TX_SIGNALS)

/* START: Access MACROS */
#if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST TxIpdu data when post build variant is selected
 */
#define COM_GET_TX_IPDU_CONSTDATA(COMPDUID)                (&Com_Prv_Config_pcst->txIpduCfg_pcast[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get variant specific CFG of TxIpdu when post-build variant is selected
 */
#define COM_GET_TX_IPDU_VAR_CFG(COMPDUID)                  (&Com_Prv_Variant_pcst->txIpduVarCfg_pcast[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
* Access macro to get internal HandleId from external HandleId , for Tx IPdus
 */
#define COM_GET_TX_IPDU_ID(COMPDUID)                       (Com_Prv_Config_pcst->txIpduMappingCfg_pcauo[COMPDUID])
/* ComIPduHandleId */
#define COM_GET_TXIPDU_HANDLEID(COMPDUID)                  (COM_GET_TX_IPDU_CONSTDATA(COMPDUID)->idHandle_u16)
/* size from current active PB variant configuration structure */
#define COM_GET_TXIPDU_SIZE(COMPDUID)                      \
                                                (Com_Prv_Variant_pcst->txIpduVarCfg_pcast[COMPDUID].size_uo)
#define COM_GET_TXIPDU_PDURID(COMPDUID)                    \
                                                (Com_Prv_Variant_pcst->txIpduVarCfg_pcast[COMPDUID].idPduRSrcPdu_uo)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST RxIpdu data when post build variant is selected
 */
#define COM_GET_RX_IPDU_CONSTDATA(COMPDUID)                (&Com_Prv_Config_pcst->rxIpduCfg_pcast[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get variant specific CFG of RxIpdu when post-build variant is selected
 */
#define COM_GET_RX_IPDU_VAR_CFG(COMPDUID)                  (&Com_Prv_Variant_pcst->rxIpduVarCfg_pcast[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
* Access macro to get internal HandleId from external HandleId , for Rx IPdus
 */
#define COM_GET_RX_IPDU_ID(COMPDUID)                       (Com_Prv_Config_pcst->rxIpduMappingCfg_pcauo[COMPDUID])
/* ComIPduHandleId */
#define COM_GET_RXIPDU_HANDLEID(COMPDUID)                  (COM_GET_RX_IPDU_CONSTDATA(COMPDUID)->idHandle_u16)
/* size from current active PB variant configuration structure */
#define COM_GET_RXIPDU_SIZE(COMPDUID)                      (Com_Prv_Variant_pcst->rxIpduVarCfg_pcast[COMPDUID].size_uo)
/* Fetching Rx metadata */
#define COM_GET_RXMETADATA(COMPDUID)                       (&Com_Prv_Config_pcst->rxMetaData_pcast[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST IpduGroup when post build variant is selected
 */
#define COM_GET_IPDUGRP_CONSTDATA(COMPDUID)                &(Com_Prv_Config_pcst->ipduGrpCfg_pcast[COMPDUID])
 /**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId for Ipdu group, when post build variant is selected
 */
#define COM_GET_IPDUGRP_ID(COMPDUID)                       (Com_Prv_Config_pcst->ipduGrpMappingCfg_pcau16[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST IpduGroupIpduRef when post build variant is selected
 */
#define COM_GET_IPDUGRP_IPDUREF_CONSTDATA(COMPDUID)        &(Com_Prv_Config_pcst->ipduGrpIpduIdCfg_pcauo[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST RxSignal when post build variant is selected
 */
#define COM_GET_RXSIG_CONSTDATA(COMSIGID)                  &(Com_Prv_Config_pcst->rxSigCfg_pcast[COMSIGID])
/**
 * Access macro to get user-callback data, for Rx Signal in post-build variant
 */
#define COM_GET_RXSIG_VAR_CFG(ID)                          &(Com_Prv_Variant_pcst->rxSigVarCfg_pcast[ID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Rx Signals
 */
#define COM_GET_RXSIGNAL_ID(COMSIGID)                      (Com_Prv_Config_pcst->rxSignalMappingCfg_pcau16[COMSIGID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST TxSignal when post build variant is selected
 */
#define COM_GET_TXSIG_CONSTDATA(COMSIGID)                  (&Com_Prv_Config_pcst->txSigCfg_pcast[COMSIGID])
/**
 * Access macro to get user-callback data, for Tx Signal in post-build variant
 */
#define COM_GET_TXSIG_VAR_CFG(ID)                           (&Com_Prv_Variant_pcst->txSigVarCfg_pcast[ID])
 /**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Tx Signals
 */
#define COM_GET_TXSIGNAL_ID(COMSIGID)                      (Com_Prv_Config_pcst->txSignalMappingCfg_pcau16[COMSIGID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST TxSignalGroup when post build variant is selected
 */
#define COM_GET_TXSIGGRP_CONSTDATA(COMSIGGRPID)            (&Com_Prv_Config_pcst->txSigGrpCfg_pcast[COMSIGGRPID])
/**
 * Access macro to get user-callback data, for Tx Signal Groups in post-build variant
 */
#define COM_GET_TXSIGGRP_VAR_CFG(ID)                        (&Com_Prv_Variant_pcst->txSigGrpVarCfg_pcast[ID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Tx Signal Groups
 */
#define COM_GET_TXSIGNALGRP_ID(COMSIGGRPID)                \
    (Com_Prv_Config_pcst->txSignalGrpMappingCfg_pcau16[(COMSIGGRPID) - COM_GET_TXSIGNALGRP_ID_OFFSET])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST RxSignalGroup when post build variant is selected
 */
#define COM_GET_RXSIGGRP_CONSTDATA(COMSIGGRPID)            (&Com_Prv_Config_pcst->rxSigGrpCfg_pcast[COMSIGGRPID])
/**
 * Access macro to get user-callback data, for Rx Signal Groups in post-build variant
 */
#define COM_GET_RXSIGGRP_VAR_CFG(ID)                        (&Com_Prv_Variant_pcst->rxSigGrpVarCfg_pcast[ID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Rx Signal Groups
 */
#define COM_GET_RXSIGNALGRP_ID(COMSIGGRPID)                \
    (Com_Prv_Config_pcst->rxSignalGrpMappingCfg_pcau16[(COMSIGGRPID) - COM_GET_RXSIGNALGRP_ID_OFFSET])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST TxGroupSignal when post build variant is selected
 */
#define COM_GET_TXGRPSIG_CONSTDATA(COMGRPSIGID)            &(Com_Prv_Config_pcst->txGrpSigCfg_pcast[COMGRPSIGID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Tx Group Signals
 */
#define COM_GET_TXGRPSIGNAL_ID(COMGRPSIGID)                \
    (Com_Prv_Config_pcst->txGrpSignalMappingCfg_pcau16[(COMGRPSIGID) - COM_GET_TXGRPSIGNAL_ID_OFFSET])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST RxGroupSignal when post build variant is selected
 */
#define COM_GET_RXGRPSIG_CONSTDATA(COMGRPSIGID)            &(Com_Prv_Config_pcst->rxGrpSigCfg_pcast[COMGRPSIGID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Rx Group Signals
 */
#define COM_GET_RXGRPSIGNAL_ID(COMGRPSIGID)                \
    (Com_Prv_Config_pcst->rxGrpSignalMappingCfg_pcau16[(COMGRPSIGID) - COM_GET_RXGRPSIGNAL_ID_OFFSET])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST Gateway Signal when post build variant is selected
 */
#define COM_GET_GWMAPSIG_CONSTDATA(IDX_GWMAP)              (&Com_Prv_Config_pcst->gwMapSigCfg_pcast[IDX_GWMAP])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST Gateway Signal Fifo mapping when post build variant is selected
 */
#define COM_GET_GWMAPSIGFIFO_CONSTDATA(IDX_FIFO)           (&Com_Prv_Config_pcst->gwMapSigFifoCfg_pcast[IDX_FIFO])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST Gateway Signal destination mapping when post build variant is selected
 */
#define COM_GET_GWMAPSIGID_CONSTDATA(IDX_DEST)             (&Com_Prv_Config_pcst->gwMapSigIdCfg_pcast[IDX_DEST])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Total number of IpduGroups configured when post build variant is selected
 */
#define COM_GET_NUM_TOTAL_IPDU_GRP                         (Com_Prv_Config_pcst->numOfIpduGroup_u16)

/* Access macro to get total number of Rx IpduGroups configured when post build variant is selected */
#define COM_GET_NUM_TOTAL_RX_IPDU_GRP                      (Com_Prv_Config_pcst->numOfRxIPduGroup_u16)
/* Access macro to get total number of Tx IpduGroups configured when post build variant is selected */
#define COM_GET_NUM_TOTAL_TX_IPDU_GRP                      (Com_Prv_Config_pcst->numOfTxIPduGroup_u16)

#if defined ( COM_TX_SIGNALGROUP_ARRAY )
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Tx SignalGroup Array Constant Structure, when post-build variant is selected
 */
# define COM_GET_TXSIGGRP_ARRAY_CONSTDATA(SIGGRP_INDEX)    &(Com_Prv_Config_pcst->txSigGrpArrayCfg_pcast[(SIGGRP_INDEX)])
#endif

#if defined ( COM_RX_SIGNALGROUP_ARRAY )
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Rx SignalGroup Array Constant Structure, when post-build variant is selected
 */
# define COM_GET_RXSIGGRP_ARRAY_CONSTDATA(SIGGRP_INDEX)    &(Com_Prv_Config_pcst->rxSigGrpArrayCfg_pcast[(SIGGRP_INDEX)])
#endif

#if defined(COM_TXSIG_FLOAT64SUPP ) || defined(COM_TXGRPSIG_FLOAT64SUPP) || defined(COM_RXGRPSIG_FLOAT64SUPP) || defined(COM_RXSIG_FLOAT64SUPP)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Init Value from the FloatValArray when post build variant is selected
 */
#define COM_GET_FLOAT_ARRAY_VALUE(val)                ((Com_Prv_Common_pcst->float64SigValuesCfg_pcaf64)[(val)])
#endif

#if defined (COM_TXSIG_INT64 ) || defined(COM_RXSIG_INT64) || defined(COM_TXGRPSIG_INT64) || defined(COM_RXGRPSIG_INT64)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get signal init/invalid value for uint64/sint64 types when post build variant is selected
 */
#define COM_GET_INT64_SIG_VALUE(index)                ((Com_Prv_Common_pcst->int64SigValuesCfg_pcau64)[(index)])
#endif

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get signal init/invalid value for uint8[n] types when post build variant is selected
 */
#define COM_GET_BYTE_ARRAY_SIG_VAL(index)             ((Com_Prv_Common_pcst->byteArraySigValCfg_pcast)[(index)])

#if defined (COM_F_MASKEDNEWEQUALSX ) || defined(COM_F_MASKEDNEWDIFFERSX)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Init Value from the Com_MaskX when post build variant is selected
 */
#define COM_GET_MASK_X(FilterIndex)                   (Com_Prv_Config_pcst->maskXCfg_pcast[FilterIndex])
#endif /* #if defined (COM_F_MASKEDNEWEQUALSX ) || defined(COM_F_MASKEDNEWDIFFERSX) */

#ifdef COM_F_MASKEDNEWDIFFERSOLD
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Init Value from the Com_Mask when post build variant is selected
 */
#define COM_GET_FMASKOLD(FilterIndex)                 (Com_Prv_Config_pcst->maskOldCfg_pcast[FilterIndex])
#endif /* #ifdef COM_F_MASKEDNEWDIFFERSOLD */

#if defined (COM_F_NEWISWITHIN_POS) || defined(COM_F_NEWISOUTSIDE_POS)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Init Value from the Com_POSMinMax when post build variant is selected
 */
#define COM_GET_POSMINMAX(FilterIndex)                      (Com_Prv_Config_pcst->posMinMaxCfg_pcast[FilterIndex])
#endif /* #if defined (COM_F_NEWISWITHIN_POS) || defined(COM_F_NEWISOUTSIDE_POS) */

#if defined (COM_F_NEWISWITHIN_NEG) || defined(COM_F_NEWISOUTSIDE_NEG)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Init Value from the Com_NEGMinMax when post build variant is selected
 */
#define COM_GET_NEGMINMAX(FilterIndex)                      (Com_Prv_Config_pcst->negMinMaxCfg_pcast[FilterIndex])
#endif /* #if defined (COM_F_NEWISWITHIN_NEG) || defined(COM_F_NEWISOUTSIDE_NEG) */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get ComMainFunctionTx configuration, when post build variant is selected
 */
#define COM_GET_MAINFUNCTX_CFG(TX_MF_ID)                    (&Com_Prv_Config_pcst->mainFuncTxCfg_pcast[TX_MF_ID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get ComMainFunctionRx configuration, when post build variant is selected
 */
#define COM_GET_MAINFUNCRX_CFG(RX_MF_ID)                    (&Com_Prv_Config_pcst->mainFuncRxCfg_pcast[RX_MF_ID])

#ifdef COM_TX_IPDUCOUNTER
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST TxIpdu counter data, when post build variant is selected
 */
#define COM_GET_TX_IPDU_CNTR_CONSTDATA(INDEX)               &(Com_Prv_Config_pcst->txIpduCntrCfg_pcast[(INDEX)])
#endif

#ifdef COM_RX_IPDUCOUNTER
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST RxIpdu counter data, when post build variant is selected
 */
#define COM_GET_RX_IPDU_CNTR_CONSTDATA(INDEX)               &(Com_Prv_Config_pcst->rxIpduCntrCfg_pcast[(INDEX)])
#endif

#ifdef COM_F_ONEEVERYN
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST array which holds the OneEveryN filter configuration in PB variant
 */
#define COM_GET_ONEEVERYN_CONST(FILTER_INDEX)               (Com_Prv_Config_pcst->oneEveryNCfg_pcast[FILTER_INDEX])

#endif /* #ifdef COM_F_ONEEVERYN */

/* RAM Variables access macros */
#ifdef COM_F_MASKEDNEWDIFFERSOLD
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds the signals old value during PB variant
 */
#define COM_GET_F_OLDVALUE(FILTER_INDEX)                    (*(Com_Prv_Config_pcst->maskOldCfg_pcast[FILTER_INDEX].fOldVal_pu32))
#endif /* #ifdef COM_F_MASKEDNEWDIFFERSOLD */

#if defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds the signals/signal groups old value for
 * triggered on change transfer property signals/signal groups during PB variant
 */
#define COM_GET_OLDVALTOC(MF_ID, OLDVAL_INDEX)     (Com_Prv_Common_pcst->txRamBufCfg_pcast[MF_ID].oldValueTOC_pauo[OLDVAL_INDEX])
#endif /* #if defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC) */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type uint8 buffer in PB variant
 */
#define COM_GET_SIGTYPEUINT8_BUFF(MF_ID,SIGNAL_INDEX)       (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].sigBuf_pau8[SIGNAL_INDEX])
#ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type uint8_dyn buffer in PB variant
 */
#define COM_GET_SIGTYPEUINT8DYN_BUFF(MF_ID,SIGNAL_INDEX)    (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].sigDynBuf_pau8[SIGNAL_INDEX])
#endif /* #ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT */
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type uint16 buffer in PB variant
 */
#define COM_GET_SIGTYPEUINT16_BUFF(MF_ID,SIGNAL_INDEX)      (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].sigBuf_pau16[SIGNAL_INDEX])

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type uint32 buffer in PB variant
 */
#define COM_GET_SIGTYPEUINT32_BUFF(MF_ID,SIGNAL_INDEX)      (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].sigBuf_pau32[SIGNAL_INDEX])

#ifdef COM_RXSIG_INT64
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type uint64 buffer in PB variant
 */
#define COM_GET_SIGTYPEUINT64_BUFF(MF_ID,SIGNAL_INDEX)      (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].sigBuf_pau64[SIGNAL_INDEX])
#endif /* #ifdef COM_RXSIG_INT64 */

#ifdef COM_RXSIG_FLOAT64SUPP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type float64 buffer in PB variant
 */
#define COM_GET_SIGTYPEFLOAT64_BUFF(MF_ID,SIGNAL_INDEX)     (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].sigBuf_paf64[SIGNAL_INDEX])
#endif /* #ifdef COM_RXSIG_FLOAT64SUPP */

#ifdef COM_RX_SIGNALGROUP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint8 buffer in PB variant
 */
#define COM_GET_RXGRPSIGTYPEUINT8_BUFF(MF_ID,GRPSIGNAL_INDEX)    (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].rxGrpSigBuf_pau8[GRPSIGNAL_INDEX])

#ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint8_dyn buffer in PB variant
 */
#define COM_GET_RXGRPSIGTYPEUINT8DYN_BUFF(MF_ID,GRPSIGNAL_INDEX)     (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].rxGrpSigDynBuf_pau8[GRPSIGNAL_INDEX])
#endif/* #ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint16 buffer in PB variant
 */
#define COM_GET_RXGRPSIGTYPEUINT16_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].rxGrpSigBuf_pau16[GRPSIGNAL_INDEX])

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint32 buffer in PB variant
 */
#define COM_GET_RXGRPSIGTYPEUINT32_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].rxGrpSigBuf_pau32[GRPSIGNAL_INDEX])

#ifdef COM_RXGRPSIG_INT64
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint64 buffer in PB variant
 */
#define COM_GET_RXGRPSIGTYPEUINT64_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].rxGrpSigBuf_pau64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_RXGRPSIG_INT64 */

#ifdef COM_RXGRPSIG_FLOAT64SUPP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type float64 buffer in PB variant
 */
#define COM_GET_RXGRPSIGTYPEFLOAT64_BUFF(MF_ID,GRPSIGNAL_INDEX)  (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].rxGrpSigBuf_paf64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_RXGRPSIG_FLOAT64SUPP */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Second Rx GroupSignal type uint8 buffer in PB variant
 */
#define COM_GET_SECRXGRPSIGTYPEU8_BUFF(MF_ID,GRPSIGNAL_INDEX)    (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].secondRxGrpSigBuf_pau8[GRPSIGNAL_INDEX])

#ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint8_dyn buffer in PB variant
 */
#define COM_GET_SECRXGRPSIGTYPEUINT8DYN_BUFF(MF_ID,GRPSIGNAL_INDEX)     (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].secondRxGrpSigDynBuf_pau8[GRPSIGNAL_INDEX])
#endif/* #ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Second Rx GroupSignal type uint16 buffer in PB variant
 */
#define COM_GET_SECRXGRPSIGTYPEU16_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].secondRxGrpSigBuf_pau16[GRPSIGNAL_INDEX])

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Second Rx GroupSignal type uint32 buffer in PB variant
 */
#define COM_GET_SECRXGRPSIGTYPEU32_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].secondRxGrpSigBuf_pau32[GRPSIGNAL_INDEX])

#ifdef COM_RXGRPSIG_INT64
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Second Rx GroupSignal type uint64 buffer in PB variant
 */
#define COM_GET_SECRXGRPSIGTYPEU64_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].secondRxGrpSigBuf_pau64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_RXGRPSIG_INT64 */

#ifdef COM_RXGRPSIG_FLOAT64SUPP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Second Rx GroupSignal type float64 buffer in PB variant
 */
#define COM_GET_SECRXGRPSIGTYPEFLOAT64_BUFF(MF_ID,GRPSIGNAL_INDEX) (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].secondRxGrpSigBuf_paf64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_RXGRPSIG_FLOAT64SUPP */

#ifdef COM_RX_SIGNALGROUP_ARRAY
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx SignalGroup of Array type buffer in PB variant
 */
# define COM_GET_RXSIGGRP_ARRAY_BUFF(MF_ID,SIGGRP_INDEX)        (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].rxSigGrpArrayBuf_pau8[(SIGGRP_INDEX)])
#endif

#endif/* #ifdef COM_RX_SIGNALGROUP */

#ifdef COM_TX_SIGNALGROUP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type uint8 buffer in PB variant
 */
#define COM_GET_TXGRPSIGTYPEU8_BUFF(MF_ID,GRPSIGNAL_INDEX)       \
        (Com_Prv_Common_pcst->txRamBufCfg_pcast[MF_ID].txGrpSigBuf_pau8[GRPSIGNAL_INDEX])

#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type uint8_Dyn buffer in PB variant
 */
#define COM_GET_TXGRPSIGTYPEUINT8DYN_BUFF(MF_ID,GRPSIGNAL_INDEX)              \
        (Com_Prv_Common_pcst->txRamBufCfg_pcast[MF_ID].txGrpSigDynBuf_pau8[GRPSIGNAL_INDEX])
#endif /* COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type uint16 buffer in PB variant
 */
#define COM_GET_TXGRPSIGTYPEU16_BUFF(MF_ID,GRPSIGNAL_INDEX)      \
        (Com_Prv_Common_pcst->txRamBufCfg_pcast[MF_ID].txGrpSigBuf_pau16[GRPSIGNAL_INDEX])

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type uint32 buffer in PB variant
 */
#define COM_GET_TXGRPSIGTYPEU32_BUFF(MF_ID,GRPSIGNAL_INDEX)      \
        (Com_Prv_Common_pcst->txRamBufCfg_pcast[MF_ID].txGrpSigBuf_pau32[GRPSIGNAL_INDEX])

#ifdef COM_TXGRPSIG_INT64
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type uint64 buffer in PC variant
 */
#define COM_GET_TXGRPSIGTYPEU64_BUFF(MF_ID,GRPSIGNAL_INDEX)      \
        (Com_Prv_Common_pcst->txRamBufCfg_pcast[MF_ID].txGrpSigBuf_pau64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_TXGRPSIG_INT64 */

#ifdef COM_TXGRPSIG_FLOAT64SUPP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type float64 buffer in PB variant
 */
#define COM_GET_TXGRPSIGTYPEFLOAT64_BUFF(MF_ID,GRPSIGNAL_INDEX)  \
        (Com_Prv_Common_pcst->txRamBufCfg_pcast[MF_ID].txGrpSigBuf_paf64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_TXGRPSIG_FLOAT64SUPP */

#endif/* #ifdef COM_TX_SIGNALGROUP */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get TX RAM buffers in post-build variant
 */
#define COM_GET_TXRAM_BUF(MF_ID)                            (Com_Prv_Common_pcst->txRamBufCfg_pcast[MF_ID])

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds structure for deferred buffer in PB variant
 */
#define COM_GET_DEFERREDBUFFER(MF_ID)                      (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].deferredBuf_pau8)

#ifdef COM_METADATA_SUPPORT
/* Fetching Rx metadata in PB variant */
#define COM_GET_RXMETADATADEFERREDBUFFER(MF_ID)            (Com_Prv_Common_pcst->rxRamBufCfg_pcast[MF_ID].rxMetaDataDeferrefdBuf_pau8)
#endif /* #ifdef COM_METADATA_SUPPORT */

#define COM_GET_NUM_RX_SIGNALS                             (Com_Prv_Config_pcst->numOfRxSignals_u16)

#define COM_GET_NUM_TX_SIGNALS                             (Com_Prv_Config_pcst->numOfTxSignals_u16)

#define COM_GET_NUM_TX_SIGNALGRP                           (Com_Prv_Config_pcst->numOfTxSignalGroup_u16)

#define COM_GET_NUM_RX_SIGNALGRP                           (Com_Prv_Config_pcst->numOfRxSignalGroup_u16)

#define COM_GET_NUM_TX_GRPSIGNALS                          (Com_Prv_Config_pcst->numOfTxGroupSignal_uo)

#define COM_GET_NUM_RX_GRPSIGNALS                          (Com_Prv_Config_pcst->numOfRxGroupSignal_uo)

#define COM_GET_NUM_TX_IPDU                                (Com_Prv_Config_pcst->numOfTxIpdu_uo)

#define COM_GET_NUM_RX_IPDU                                (Com_Prv_Config_pcst->numOfRxIpdu_uo)

#define COM_GET_INT32_SIG_VALUE(INDEX)                     (Com_Prv_Common_pcst->int32SigValuesCfg_pcau32[(INDEX)])

#define COM_IS_VALID_RXIPDU_ID(COMRXIPDUID)                   \
    (((COMRXIPDUID) < Com_Prv_Common_pcst->numOfRxIpdusInAllVariants_u16) && (COM_GET_RX_IPDU_ID(COMRXIPDUID) < COM_GET_NUM_RX_IPDU))

#define COM_IS_VALID_TXIPDU_ID(COMTXIPDUID)                   \
    (((COMTXIPDUID) < Com_Prv_Common_pcst->numOfTxIpdusInAllVariants_u16) && (COM_GET_TX_IPDU_ID(COMTXIPDUID) < COM_GET_NUM_TX_IPDU))

/* access macro to fetch (maxRxSignalGroupId + 1),
 * equivalent to num of RxSignals + num of RxSignalGroups in all variants */
#define COM_GET_RXSIGNALGRP_ID_MAX_OUT_BOUND               (Com_Prv_Common_pcst->rxSignalGrpIdMaxOutBound_u16)

#define COM_IS_VALID_RXSIGNALGRP_ID(COMSIGGRPID)              \
    (((COMSIGGRPID) >= COM_GET_RXSIGNALGRP_ID_OFFSET) && ((COMSIGGRPID) < COM_GET_RXSIGNALGRP_ID_MAX_OUT_BOUND) && \
     (COM_GET_RXSIGNALGRP_ID(COMSIGGRPID) < COM_GET_NUM_RX_SIGNALGRP))

/* access macro to fetch (maxTxSignalGroupId + 1),
 * equivalent to num of TxSignals + num of TxSignalGroups in all variants */
#define COM_GET_TXSIGNALGRP_ID_MAX_OUT_BOUND               (Com_Prv_Common_pcst->txSignalGrpIdMaxOutBound_u16)

#define COM_IS_VALID_TXSIGNALGRP_ID(COMSIGGRPID)              \
    (((COMSIGGRPID) >= COM_GET_TXSIGNALGRP_ID_OFFSET) && ((COMSIGGRPID) < COM_GET_TXSIGNALGRP_ID_MAX_OUT_BOUND) && \
     (COM_GET_TXSIGNALGRP_ID(COMSIGGRPID) < COM_GET_NUM_TX_SIGNALGRP))

#define COM_IS_VALID_IPDUGRP_ID(COMPDUID)                     \
    (((COMPDUID) < Com_Prv_Common_pcst->numOfIPduGroupsInAllVariants_u16) && (COM_GET_IPDUGRP_ID(COMPDUID) < COM_GET_NUM_TOTAL_IPDU_GRP))

#define COM_IS_VALID_RXIPDUGRP_ID(COMRXIPDUGRPID)             \
    (((COMRXIPDUGRPID) < Com_Prv_Config_pcst->numOfRxIPduGroup_u16) && (COM_GET_IPDUGRP_ID(COMRXIPDUGRPID) < COM_GET_NUM_TOTAL_RX_IPDU_GRP))

#define COM_IS_VALID_TXIPDUGRP_ID(COMTXIPDUGRPID)             \
    (((COMTXIPDUGRPID) >= COM_GET_NUM_TOTAL_RX_IPDU_GRP) &&   \
    (((COMTXIPDUGRPID) < (Com_Prv_Config_pcst->numOfTxIPduGroup_u16 + Com_Prv_Config_pcst->numOfRxIPduGroup_u16))) && \
    (COM_GET_IPDUGRP_ID(COMTXIPDUGRPID) < (COM_GET_NUM_TOTAL_TX_IPDU_GRP + COM_GET_NUM_TOTAL_RX_IPDU_GRP)))

#define COM_IS_VALID_TXSIGNAL_ID(COMTXSIGID)                  \
    (((COMTXSIGID) < Com_Prv_Common_pcst->numOfTxSignalsInAllVariants_u16) && (COM_GET_TXSIGNAL_ID(COMTXSIGID) < COM_GET_NUM_TX_SIGNALS))

#define COM_IS_VALID_RXSIGNAL_ID(COMRXSIGID)                  \
    (((COMRXSIGID) < Com_Prv_Common_pcst->numOfRxSignalsInAllVariants_u16) && (COM_GET_RXSIGNAL_ID(COMRXSIGID) < COM_GET_NUM_RX_SIGNALS))

/* access macro to fetch (maxTxGroupSignalId + 1),
 * equivalent to num of TxSignals + num of TxGroupSignals in all variants */
#define COM_GET_TXGRPSIGNAL_ID_MAX_OUT_BOUND                  (Com_Prv_Common_pcst->txGrpSignalIdMaxOutBound_u16)

#define COM_IS_VALID_TX_GRPSIGNAL_ID(COMGRPSIGID)             \
    (((COMGRPSIGID) >= COM_GET_TXGRPSIGNAL_ID_OFFSET) && ((COMGRPSIGID) < COM_GET_TXGRPSIGNAL_ID_MAX_OUT_BOUND) \
            && (COM_GET_TXGRPSIGNAL_ID(COMGRPSIGID) < COM_GET_NUM_TX_GRPSIGNALS))

/* access macro to fetch (maxRxGroupSignalId + 1),
 * equivalent to num of RxSignals + num of RxGroupSignals in all variants */
#define COM_GET_RXGRPSIGNAL_ID_MAX_OUT_BOUND                  (Com_Prv_Common_pcst->rxGrpSignalIdMaxOutBound_u16)

#define COM_IS_VALID_RX_GRPSIGNAL_ID(COMGRPSIGID)             \
    (((COMGRPSIGID) >= COM_GET_RXGRPSIGNAL_ID_OFFSET) && ((COMGRPSIGID) < COM_GET_RXGRPSIGNAL_ID_MAX_OUT_BOUND) \
            && (COM_GET_RXGRPSIGNAL_ID(COMGRPSIGID) < COM_GET_NUM_RX_GRPSIGNALS))

#ifdef COM_F_ONEEVERYN
#define COM_GET_NUM_SIG_GRPSIG_WITH_ONE_EVERY_N_FILTER     (Com_Prv_Config_pcst->numOfOneEveryNSigAndGrpSig_u8)
#endif /* #ifdef COM_F_ONEEVERYN */

#else
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST TxIpdu data when pre-compile variant is selected
 */
#define COM_GET_TX_IPDU_CONSTDATA(COMPDUID)                (&Com_Prv_xTxIpduCfg_acst[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get variant specific CFG of TxIpdu when pre-compile variant is selected
 */
#define COM_GET_TX_IPDU_VAR_CFG(COMPDUID)                 (&Com_Prv_xTxIpduVarCfg_acst[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Tx IPdus
 * For pre-compile variant no mapping required, both the values will be same
 */
#define COM_GET_TX_IPDU_ID(COMPDUID)                       (COMPDUID)
/* ComIPduHandleId */
#define COM_GET_TXIPDU_HANDLEID(COMPDUID)                  (COMPDUID)
/* size from configuration structures */
#define COM_GET_TXIPDU_SIZE(COMPDUID)                      (COM_GET_TX_IPDU_CONSTDATA(COMPDUID)->size_uo)
#define COM_GET_TXIPDU_PDURID(COMPDUID)                    (COM_GET_TX_IPDU_CONSTDATA(COMPDUID)->idPduRSrcPdu_uo)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST RxIpdu data when pre-compile variant is selected
 */
#define COM_GET_RX_IPDU_CONSTDATA(COMPDUID)                (&Com_Prv_xRxIpduCfg_acst[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get variant specific CFG of RxIpdu when pre-compile variant is selected
 */
#define COM_GET_RX_IPDU_VAR_CFG(COMPDUID)                  (&Com_Prv_xRxIpduVarCfg_acst[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Rx IPdus
 * For pre-compile variant no mapping required, both the values will be same
 */
#define COM_GET_RX_IPDU_ID(COMPDUID)                       (COMPDUID)
/* ComIPduHandleId */
#define COM_GET_RXIPDU_HANDLEID(COMPDUID)                  (COMPDUID)
/* size from configuration structures */
#define COM_GET_RXIPDU_SIZE(COMPDUID)                      (COM_GET_RX_IPDU_CONSTDATA(COMPDUID)->size_uo)
/* Fetching Rx metadata */
#define COM_GET_RXMETADATA(COMPDUID)                       (COM_GET_RX_IPDU_CONSTDATA(COMPDUID)->rxMetaDataPtr_pcst)

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST IpduGroup when pre-compile variant is selected
 */
#define COM_GET_IPDUGRP_CONSTDATA(COMPDUID)                (&Com_Prv_xIpduGrpCfg_acst[COMPDUID])
 /**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId for Ipdu group
 * For pre-compile variant no mapping required, both the values will be same
 */
#define COM_GET_IPDUGRP_ID(COMPDUID)                       (COMPDUID)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST IpduGroupIpduRef when pre-compile variant is selected
 */
#define COM_GET_IPDUGRP_IPDUREF_CONSTDATA(COMPDUID)        (&Com_Prv_xIPduGrp_IpduRefCfg_acuo[COMPDUID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST RxSignal when pre-compile variant is selected
 */
#define COM_GET_RXSIG_CONSTDATA(COMSIGID)                  (&Com_Prv_xRxSigCfg_acst[COMSIGID])
/**
 * Access macro to get user-callback data, for Rx Signal in pre-compile variant
 */
#define COM_GET_RXSIG_VAR_CFG(ID)                          (&Com_Prv_xRxSigVarCfg_acst[ID])
  /**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Rx Signals
 * For pre-compile variant no mapping required, both the values will be same
 */
 #define COM_GET_RXSIGNAL_ID(COMSIGID)                     (COMSIGID)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST TxSignal when pre-compile variant is selected
 */
#define COM_GET_TXSIG_CONSTDATA(COMSIGID)                  (&Com_Prv_xTxSigCfg_acst[COMSIGID])
/**
 * Access macro to get user-callback data, for Tx Signal in pre-compile variant
 */
#define COM_GET_TXSIG_VAR_CFG(ID)                           (&Com_Prv_xTxSigVarCfg_acst[ID])
 /**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Tx Signals
 * For pre-compile variant no mapping required, both the values will be same
 */
#define COM_GET_TXSIGNAL_ID(COMSIGID)                      (COMSIGID)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST TxSignalGroup when pre-compile variant is selected
 */
#define COM_GET_TXSIGGRP_CONSTDATA(COMSIGGRPID)            (&Com_Prv_xTxSigGrpCfg_acst[COMSIGGRPID])
/**
 * Access macro to get user-callback data, for Tx Signal Groups in pre-compile variant
 */
#define COM_GET_TXSIGGRP_VAR_CFG(ID)                        (&Com_Prv_xTxSigGrpVarCfg_acst[ID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Tx Signals Group
 * For pre-compile variant, SignalGroup ID is created with an offset value(eq to num of Tx Signals),
 * so, SignalGroup ID is reduced by an offset value.
 */
#define COM_GET_TXSIGNALGRP_ID(COMSIGGRPID)                ((COMSIGGRPID) - COM_GET_TXSIGNALGRP_ID_OFFSET)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST RxSignalGroup when pre-compile variant is selected
 */
#define COM_GET_RXSIGGRP_CONSTDATA(COMSIGGRPID)            (&Com_Prv_xRxSigGrpCfg_acst[COMSIGGRPID])
/**
 * Access macro to get user-callback data, for Rx Signal Groups in pre-compile variant
 */
#define COM_GET_RXSIGGRP_VAR_CFG(ID)                        (&Com_Prv_xRxSigGrpVarCfg_acst[ID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Rx Signals Group
 * For pre-compile variant, SignalGroup ID is created with an offset value(eq to num of Rx Signals),
 * so, SignalGroup ID is reduced by an offset value.
 */
#define COM_GET_RXSIGNALGRP_ID(COMSIGGRPID)                ((COMSIGGRPID) - COM_GET_RXSIGNALGRP_ID_OFFSET)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST TxGroupSignal when pre-compile variant is selected
 */
#define COM_GET_TXGRPSIG_CONSTDATA(COMGRPSIGID)            (&Com_Prv_xTxGrpSigCfg_acst[COMGRPSIGID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Tx Group Signals
 * For pre-compile variant, GroupSignal ID is created with an offset value(eq to num of Signals),
 * so, GroupSignal ID is reduced by an offset value.
 */
#define COM_GET_TXGRPSIGNAL_ID(COMGRPSIGID)                ((COMGRPSIGID) - COM_GET_TXGRPSIGNAL_ID_OFFSET)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST RxGroupSignal when pre-compile variant is selected
 */
#define COM_GET_RXGRPSIG_CONSTDATA(COMGRPSIGID)            (&Com_Prv_xRxGrpSigCfg_acst[COMGRPSIGID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get internal HandleId from external HandleId , for Rx Group Signals
 * For pre-compile variant, GroupSignal ID is created with an offset value(eq to num of Signals),
 * so, GroupSignal ID is reduced by an offset value.
 */
#define COM_GET_RXGRPSIGNAL_ID(COMGRPSIGID)                ((COMGRPSIGID) - COM_GET_RXGRPSIGNAL_ID_OFFSET)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST Gateway Signal when pre-compile variant is selected
 */
#define COM_GET_GWMAPSIG_CONSTDATA(IDX_GWMAP)              (&Com_Prv_xGwMapSigCfg_acst[IDX_GWMAP])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST Gateway Signal Fifo mapping when pre-compile variant is selected
 */
#define COM_GET_GWMAPSIGFIFO_CONSTDATA(IDX_FIFO)           (&Com_Prv_xGwMapSigFifoCfg_acst[IDX_FIFO])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST Gateway Signal destination mapping when pre-compile variant is selected
 */
#define COM_GET_GWMAPSIGID_CONSTDATA(IDX_DEST)             (&Com_Prv_xGwMapSigIdCfg_acst[IDX_DEST])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Total number of IpduGroups configured when pre-compile variant is selected
 */
#define COM_GET_NUM_TOTAL_IPDU_GRP                         COM_NUM_TOTAL_IPDU_GRP

/* Access macro to get total number of Rx IpduGroups configured when pre-compile variant is selected */
#define COM_GET_NUM_TOTAL_RX_IPDU_GRP                      COM_NUM_TOTAL_RX_IPDU_GRP

/* Access macro to get total number of Tx IpduGroups configured when pre-compile variant is selected */
#define COM_GET_NUM_TOTAL_TX_IPDU_GRP                      COM_NUM_TOTAL_TX_IPDU_GRP

#if defined ( COM_TX_SIGNALGROUP_ARRAY )
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Tx SignalGroup Array Constant Structure, when pre-compile variant is selected
 */
# define COM_GET_TXSIGGRP_ARRAY_CONSTDATA(SIGGRP_INDEX)    (&Com_Prv_xTxSigGrpArrayCfg_acst[(SIGGRP_INDEX)])
#endif

#if defined ( COM_RX_SIGNALGROUP_ARRAY )
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Rx SignalGroup Array Constant Structure, when pre-compile variant is selected
 */
# define COM_GET_RXSIGGRP_ARRAY_CONSTDATA(SIGGRP_INDEX)    (&Com_Prv_xRxSigGrpArrayCfg_acst[(SIGGRP_INDEX)])
#endif

#if defined(COM_TXSIG_FLOAT64SUPP ) || defined(COM_TXGRPSIG_FLOAT64SUPP) || defined(COM_RXGRPSIG_FLOAT64SUPP) || defined(COM_RXSIG_FLOAT64SUPP)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Init Value from the FloatValArray when pre-compile variant is selected
 */
#define COM_GET_FLOAT_ARRAY_VALUE(val)                     (Com_Float64SigValues_acf64[(val)])
#endif

#if defined (COM_TXSIG_INT64 ) || defined(COM_RXSIG_INT64) || defined(COM_TXGRPSIG_INT64) || defined(COM_RXGRPSIG_INT64)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get signal init/invalid value for uint64/sint64 types when pre-compile variant is selected
 */
#define COM_GET_INT64_SIG_VALUE(index)                     (Com_Int64SigValues_acu64[(index)])
#endif

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get signal init/invalid value for uint8[n] types when pre-compile variant is selected
 */
#define COM_GET_BYTE_ARRAY_SIG_VAL(index)                  (Com_ByteArraySigValues_acst[(index)])

#if defined (COM_F_MASKEDNEWEQUALSX ) || defined(COM_F_MASKEDNEWDIFFERSX)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Init Value from the Com_MaskX when PC variant is selected
 */
#define COM_GET_MASK_X(FilterIndex)                        (Com_MaskX_acst[FilterIndex])
#endif /* #if defined (COM_F_MASKEDNEWEQUALSX ) || defined(COM_F_MASKEDNEWDIFFERSX) */

#ifdef COM_F_MASKEDNEWDIFFERSOLD
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Init Value from the Com_Mask when PC variant is selected
 */
#define COM_GET_FMASKOLD(FilterIndex)                      (Com_FMaskOld_acst[FilterIndex])
#endif /* #ifdef COM_F_MASKEDNEWDIFFERSOLD */

#if defined (COM_F_NEWISWITHIN_POS) || defined(COM_F_NEWISOUTSIDE_POS)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Init Value from the Com_POSMinMax when PC variant is selected
 */
#define COM_GET_POSMINMAX(FilterIndex)                     (Com_POSMinMax_acst[FilterIndex])
#endif /* #if defined (COM_F_NEWISWITHIN_POS) || defined(COM_F_NEWISOUTSIDE_POS) */

#if defined (COM_F_NEWISWITHIN_NEG) || defined(COM_F_NEWISOUTSIDE_NEG)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get Init Value from the Com_NEGMinMax when PC variant is selected
 */
#define COM_GET_NEGMINMAX(FilterIndex)                     (Com_NEGMinMax_acst[FilterIndex])
#endif /* #if defined (COM_F_NEWISWITHIN_NEG) || defined(COM_F_NEWISOUTSIDE_NEG) */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get ComMainFunctionTx configuration, when PC variant is selected
 */
#define COM_GET_MAINFUNCTX_CFG(TX_MF_ID)                    (&Com_MainFuncTxCfg_acst[TX_MF_ID])
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get ComMainFunctionRx configuration, when PC variant is selected
 */
#define COM_GET_MAINFUNCRX_CFG(RX_MF_ID)                    (&Com_MainFuncRxCfg_acst[RX_MF_ID])

#ifdef COM_TX_IPDUCOUNTER
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST TxIpdu counter data when PC variant is selected
 */
#define COM_GET_TX_IPDU_CNTR_CONSTDATA(TX_CNTR_IDX)        &(Com_Prv_xTxIpduCntrCfg_acst[(TX_CNTR_IDX)])
#endif

#ifdef COM_RX_IPDUCOUNTER
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST RxIpdu counter data when PC variant is selected
 */
#define COM_GET_RX_IPDU_CNTR_CONSTDATA(RX_CNTR_IDX)        &(Com_Prv_xRxIpduCntrCfg_acst[(RX_CNTR_IDX)])
#endif

#define COM_GET_INT32_SIG_VALUE(UNIQUE_INDEX)    (Com_Prv_Int32SigValues_acu32[(UNIQUE_INDEX)])

#ifdef COM_F_ONEEVERYN
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get CONST array which holds the OneEveryN filter configuration in PC variant
 */
#define COM_GET_ONEEVERYN_CONST(FILTER_INDEX)              (Com_OneEveryN_Const_acst[FILTER_INDEX])

#endif /* #ifdef COM_F_ONEEVERYN */

#ifdef COM_F_MASKEDNEWDIFFERSOLD
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds the signals old value during PC variant
 */
#define COM_GET_F_OLDVALUE(FILTER_INDEX)                   (*(Com_FMaskOld_acst[FILTER_INDEX].fOldVal_pu32))
#endif /* #ifdef COM_F_MASKEDNEWDIFFERSOLD */

#if defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC)
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds the signals/signal groups old value for
 * triggered on change transfer property signals/signal groups during PB variant
 */
#define COM_GET_OLDVALTOC(MF_ID, OLDVAL_INDEX)         (Com_Prv_xTxRamBuf_acst[MF_ID].oldValueTOC_pauo[OLDVAL_INDEX])
#endif /* #if defined(COM_EffectiveSigTOC) || defined(COM_EffectiveSigGrpTOC) */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type uint8 buffer in PC variant
 */
#define COM_GET_SIGTYPEUINT8_BUFF(MF_ID,SIGNAL_INDEX)            (Com_Prv_xRxRamBuf_acst[MF_ID].sigBuf_pau8[SIGNAL_INDEX])
#ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type uint8_dyn buffer in PC variant
 */
#define COM_GET_SIGTYPEUINT8DYN_BUFF(MF_ID,SIGNAL_INDEX)         (Com_Prv_xRxRamBuf_acst[MF_ID].sigDynBuf_pau8[SIGNAL_INDEX])
#endif/* #ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT */
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type uint16 buffer in PC variant
 */
#define COM_GET_SIGTYPEUINT16_BUFF(MF_ID,SIGNAL_INDEX)           (Com_Prv_xRxRamBuf_acst[MF_ID].sigBuf_pau16[SIGNAL_INDEX])

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type uint32 buffer in PC variant
 */
#define COM_GET_SIGTYPEUINT32_BUFF(MF_ID,SIGNAL_INDEX)           (Com_Prv_xRxRamBuf_acst[MF_ID].sigBuf_pau32[SIGNAL_INDEX])

#ifdef COM_RXSIG_INT64
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type uint64 buffer in PC variant
 */
#define COM_GET_SIGTYPEUINT64_BUFF(MF_ID,SIGNAL_INDEX)           (Com_Prv_xRxRamBuf_acst[MF_ID].sigBuf_pau64[SIGNAL_INDEX])
#endif /* #ifdef COM_RXSIG_INT64 */

#ifdef COM_RXSIG_FLOAT64SUPP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Signal type float64 buffer in PC variant
 */
#define COM_GET_SIGTYPEFLOAT64_BUFF(MF_ID,SIGNAL_INDEX)          (Com_Prv_xRxRamBuf_acst[MF_ID].sigBuf_paf64[SIGNAL_INDEX])
#endif /* #ifdef COM_RXSIG_FLOAT64SUPP */

#ifdef COM_RX_SIGNALGROUP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint8 buffer in PC variant
 */
#define COM_GET_RXGRPSIGTYPEUINT8_BUFF(MF_ID,GRPSIGNAL_INDEX)    (Com_Prv_xRxRamBuf_acst[MF_ID].rxGrpSigBuf_pau8[GRPSIGNAL_INDEX])

#ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint8_dyn buffer in PC variant
 */
#define COM_GET_RXGRPSIGTYPEUINT8DYN_BUFF(MF_ID,GRPSIGNAL_INDEX)    (Com_Prv_xRxRamBuf_acst[MF_ID].rxGrpSigDynBuf_pau8[GRPSIGNAL_INDEX])
#endif/* #ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint16 buffer in PC variant
 */
#define COM_GET_RXGRPSIGTYPEUINT16_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_xRxRamBuf_acst[MF_ID].rxGrpSigBuf_pau16[GRPSIGNAL_INDEX])

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint32 buffer in PC variant
 */
#define COM_GET_RXGRPSIGTYPEUINT32_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_xRxRamBuf_acst[MF_ID].rxGrpSigBuf_pau32[GRPSIGNAL_INDEX])

#ifdef COM_RXGRPSIG_INT64
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint64 buffer in PC variant
 */
#define COM_GET_RXGRPSIGTYPEUINT64_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_xRxRamBuf_acst[MF_ID].rxGrpSigBuf_pau64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_RXGRPSIG_INT64 */

#ifdef COM_RXGRPSIG_FLOAT64SUPP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type float64 buffer in PC variant
 */
#define COM_GET_RXGRPSIGTYPEFLOAT64_BUFF(MF_ID,GRPSIGNAL_INDEX)  (Com_Prv_xRxRamBuf_acst[MF_ID].rxGrpSigBuf_paf64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_RXGRPSIG_FLOAT64SUPP */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Second Rx GroupSignal type uint8 buffer in PC variant
 */
#define COM_GET_SECRXGRPSIGTYPEU8_BUFF(MF_ID,GRPSIGNAL_INDEX)    (Com_Prv_xRxRamBuf_acst[MF_ID].secondRxGrpSigBuf_pau8[GRPSIGNAL_INDEX])

#ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx GroupSignal type uint8_dyn buffer in PC variant
 */
#define COM_GET_SECRXGRPSIGTYPEUINT8DYN_BUFF(MF_ID,GRPSIGNAL_INDEX)     (Com_Prv_xRxRamBuf_acst[MF_ID].secondRxGrpSigDynBuf_pau8[GRPSIGNAL_INDEX])
#endif/* #ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Second Rx GroupSignal type uint16 buffer in PC variant
 */
#define COM_GET_SECRXGRPSIGTYPEU16_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_xRxRamBuf_acst[MF_ID].secondRxGrpSigBuf_pau16[GRPSIGNAL_INDEX])

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Second Rx GroupSignal type uint32 buffer in PC variant
 */
#define COM_GET_SECRXGRPSIGTYPEU32_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_xRxRamBuf_acst[MF_ID].secondRxGrpSigBuf_pau32[GRPSIGNAL_INDEX])

#ifdef COM_RXGRPSIG_INT64
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Second Rx GroupSignal type uint64 buffer in PC variant
 */
#define COM_GET_SECRXGRPSIGTYPEU64_BUFF(MF_ID,GRPSIGNAL_INDEX)   (Com_Prv_xRxRamBuf_acst[MF_ID].secondRxGrpSigBuf_pau64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_RXGRPSIG_INT64 */

#ifdef COM_RXGRPSIG_FLOAT64SUPP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Second Rx GroupSignal type float64 buffer in PC variant
 */
#define COM_GET_SECRXGRPSIGTYPEFLOAT64_BUFF(MF_ID,GRPSIGNAL_INDEX) (Com_Prv_xRxRamBuf_acst[MF_ID].secondRxGrpSigBuf_paf64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_RXGRPSIG_FLOAT64SUPP */

#ifdef COM_RX_SIGNALGROUP_ARRAY
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Rx SignalGroup of Array type buffer in PC variant
 */
# define COM_GET_RXSIGGRP_ARRAY_BUFF(MF_ID,SIGGRP_INDEX)           (Com_Prv_xRxRamBuf_acst[MF_ID].rxSigGrpArrayBuf_pau8[SIGGRP_INDEX])
#endif

#endif/* #ifdef COM_RX_SIGNALGROUP */

#ifdef COM_TX_SIGNALGROUP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type uint8 buffer in PC variant
 */
#define COM_GET_TXGRPSIGTYPEU8_BUFF(MF_ID,GRPSIGNAL_INDEX)              \
        (Com_Prv_xTxRamBuf_acst[MF_ID].txGrpSigBuf_pau8[GRPSIGNAL_INDEX])

#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type uint8_Dyn buffer in PC variant
 */
#define COM_GET_TXGRPSIGTYPEUINT8DYN_BUFF(MF_ID,GRPSIGNAL_INDEX)              \
        (Com_Prv_xTxRamBuf_acst[MF_ID].txGrpSigDynBuf_pau8[GRPSIGNAL_INDEX])
#endif /* COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type uint16 buffer in PC variant
 */
#define COM_GET_TXGRPSIGTYPEU16_BUFF(MF_ID,GRPSIGNAL_INDEX)             \
        (Com_Prv_xTxRamBuf_acst[MF_ID].txGrpSigBuf_pau16[GRPSIGNAL_INDEX])

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type uint32 buffer in PC variant
 */
#define COM_GET_TXGRPSIGTYPEU32_BUFF(MF_ID,GRPSIGNAL_INDEX)             \
        (Com_Prv_xTxRamBuf_acst[MF_ID].txGrpSigBuf_pau32[GRPSIGNAL_INDEX])

#ifdef COM_TXGRPSIG_INT64
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type uint64 buffer in PC variant
 */
#define COM_GET_TXGRPSIGTYPEU64_BUFF(MF_ID,GRPSIGNAL_INDEX)             \
        (Com_Prv_xTxRamBuf_acst[MF_ID].txGrpSigBuf_pau64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_TXGRPSIG_INT64 */

#ifdef COM_TXGRPSIG_FLOAT64SUPP
/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds Tx GroupSignal type float64 buffer in PC variant
 */
#define COM_GET_TXGRPSIGTYPEFLOAT64_BUFF(MF_ID,GRPSIGNAL_INDEX)         \
        (Com_Prv_xTxRamBuf_acst[MF_ID].txGrpSigBuf_paf64[GRPSIGNAL_INDEX])
#endif /* #ifdef COM_TXGRPSIG_FLOAT64SUPP */

#endif/* #ifdef COM_TX_SIGNALGROUP */

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get TX RAM buffers in pre-compile variant
 */
#define COM_GET_TXRAM_BUF(MF_ID)                            (Com_Prv_xTxRamBuf_acst[MF_ID])

/**
 * @ingroup COM_PRV_H
 *
 * Access macro to get RAM array which holds structure for deferred buffer in PC variant
 */
#define COM_GET_DEFERREDBUFFER(MF_ID)                      (Com_Prv_xRxRamBuf_acst[MF_ID].deferredBuf_pau8)

#ifdef COM_METADATA_SUPPORT
/* Fetching Rx metadata in PC variant */
#define COM_GET_RXMETADATADEFERREDBUFFER(MF_ID)            (Com_Prv_xRxRamBuf_acst[MF_ID].rxMetaDataDeferrefdBuf_pau8)
#endif /* #ifdef COM_METADATA_SUPPORT */

#define COM_GET_NUM_RX_SIGNALS                             (COM_NUM_RX_SIGNALS)

#define COM_GET_NUM_TX_SIGNALS                             (COM_NUM_TX_SIGNALS)

#define COM_GET_NUM_TX_SIGNALGRP                           (COM_NUM_TX_SIGNALGRP)

#define COM_GET_NUM_RX_SIGNALGRP                           (COM_NUM_RX_SIGNALGRP)

#define COM_GET_NUM_TX_GRPSIGNALS                          (COM_NUM_TX_GRPSIGNALS)

#define COM_GET_NUM_RX_GRPSIGNALS                          (COM_NUM_RX_GRPSIGNALS)

#define COM_GET_NUM_TX_IPDU                                (COM_NUM_TX_IPDU)

#define COM_GET_NUM_RX_IPDU                                (COM_NUM_RX_IPDU)

#ifdef COM_F_ONEEVERYN
#define COM_GET_NUM_SIG_GRPSIG_WITH_ONE_EVERY_N_FILTER     (COM_NUM_OF_SIG_GRPSIG_WITH_ONE_EVERY_N)
#endif /* #ifdef COM_F_ONEEVERYN */

#define COM_IS_VALID_RXIPDU_ID(COMRXIPDUID)                   ((COMRXIPDUID) < COM_GET_NUM_RX_IPDU)

#define COM_IS_VALID_TXIPDU_ID(COMTXIPDUID)                   ((COMTXIPDUID) < COM_GET_NUM_TX_IPDU)

#define COM_GET_RXSIGNALGRP_ID_MAX_OUT_BOUND                  (COM_RXSIGNALGRP_ID_MAX_OUT_BOUND)

#define COM_IS_VALID_RXSIGNALGRP_ID(COMSIGGRPID)              \
    (((COMSIGGRPID) >= COM_GET_RXSIGNALGRP_ID_OFFSET) && ((COMSIGGRPID) < COM_GET_RXSIGNALGRP_ID_MAX_OUT_BOUND))

#define COM_GET_TXSIGNALGRP_ID_MAX_OUT_BOUND                  (COM_TXSIGNALGRP_ID_MAX_OUT_BOUND)

#define COM_IS_VALID_TXSIGNALGRP_ID(COMSIGGRPID)              \
    (((COMSIGGRPID) >= COM_GET_TXSIGNALGRP_ID_OFFSET) && ((COMSIGGRPID) < COM_GET_TXSIGNALGRP_ID_MAX_OUT_BOUND))

#define COM_IS_VALID_IPDUGRP_ID(COMPDUID)                     ((COMPDUID)  < COM_GET_NUM_TOTAL_IPDU_GRP)

#define COM_IS_VALID_RXIPDUGRP_ID(COMRXIPDUGRPID)             ((COMRXIPDUGRPID) < COM_GET_NUM_TOTAL_RX_IPDU_GRP)

#define COM_IS_VALID_TXIPDUGRP_ID(COMTXIPDUGRPID)               \
        (((COMTXIPDUGRPID) >= COM_GET_NUM_TOTAL_RX_IPDU_GRP) && \
         ((COMTXIPDUGRPID) < (COM_GET_NUM_TOTAL_RX_IPDU_GRP + COM_GET_NUM_TOTAL_TX_IPDU_GRP)))

#define COM_IS_VALID_TXSIGNAL_ID(COMTXSIGID)                  ((COMTXSIGID) < COM_GET_NUM_TX_SIGNALS)
#define COM_IS_VALID_RXSIGNAL_ID(COMRXSIGID)                  ((COMRXSIGID) < COM_GET_NUM_RX_SIGNALS)

/* access macro to fetch (maxTxGroupSignalId + 1),
 * equivalent to num of TxSignals + num of TxGroupSignals in all variants */
#define COM_GET_TXGRPSIGNAL_ID_MAX_OUT_BOUND                  COM_TXGRPSIGNAL_ID_MAX_OUT_BOUND

#define COM_IS_VALID_TX_GRPSIGNAL_ID(COMGRPSIGID)             \
    (((COMGRPSIGID) >= COM_GET_TXGRPSIGNAL_ID_OFFSET) && ((COMGRPSIGID) < COM_GET_TXGRPSIGNAL_ID_MAX_OUT_BOUND))

/* access macro to fetch (maxRxGroupSignalId + 1),
 * equivalent to num of RxSignals + num of RxGroupSignals in all variants */
#define COM_GET_RXGRPSIGNAL_ID_MAX_OUT_BOUND                  COM_RXGRPSIGNAL_ID_MAX_OUT_BOUND

#define COM_IS_VALID_RX_GRPSIGNAL_ID(COMGRPSIGID)              \
    (((COMGRPSIGID) >= COM_GET_RXGRPSIGNAL_ID_OFFSET) && ((COMGRPSIGID) < COM_GET_RXGRPSIGNAL_ID_MAX_OUT_BOUND))

#endif /* #if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE) */

/* Access macro for ComEnablePduBasedLocks
 * If the project needs Pdu based locking mechanism, then enable the parameter ComGeneral-> ComEnablePduBasedLocks
 * Else regualar locks shall be used in the code */
#ifdef COM_PDUBASEDLOCKSENABLED

#define Com_Get_SchM_Enter_Com_RxIPduProcess             SchM_Enter_Com_RxIPduProcess();
#define Com_Get_SchM_Exit_Com_RxIPduProcess              SchM_Exit_Com_RxIPduProcess();

#define Com_Get_SchM_Enter_Com_RxIndication              SchM_Enter_Com_RxIndication();
#define Com_Get_SchM_Exit_Com_RxIndication               SchM_Exit_Com_RxIndication();

#define Com_Get_SchM_Enter_Com_TxIPdu                    SchM_Enter_Com_TxIPdu();
#define Com_Get_SchM_Exit_Com_TxIPdu                     SchM_Exit_Com_TxIPdu();

#define Com_Get_SchM_Enter_Com_RxPduBuffer
#define Com_Get_SchM_Exit_Com_RxPduBuffer

#define Com_Get_SchM_Enter_Com_RxSigBuff(APINAME)
#define Com_Get_SchM_Exit_Com_RxSigBuff(APINAME)

#ifdef COM_RX_SIGNALGROUP_ARRAY
#define Com_Get_SchM_Enter_Com_RxSigGrpArrayBuff
#define Com_Get_SchM_Exit_Com_RxSigGrpArrayBuff
#endif /* #ifdef COM_RX_SIGNALGROUP_ARRAY */

#define Com_Get_SchM_Enter_Com_RxGrpSigSecBuff
#define Com_Get_SchM_Exit_Com_RxGrpSigSecBuff

#define Com_Get_SchM_Enter_Com_RxSigToTicks
#define Com_Get_SchM_Exit_Com_RxSigToTicks

#define Com_Get_SchM_Enter_Com_RxSigGrpToTicks
#define Com_Get_SchM_Exit_Com_RxSigGrpToTicks

#define Com_Get_SchM_Enter_Com_RxSigDynBuff
#define Com_Get_SchM_Exit_Com_RxSigDynBuff

#define Com_Get_SchM_Enter_Com_TxIpduProtArea(APINAME)
#define Com_Get_SchM_Exit_Com_TxIpduProtArea(APINAME)

#define Com_Get_SchM_Enter_Com_TxGrpSigBuffer
#define Com_Get_SchM_Exit_Com_TxGrpSigBuffer

#ifdef COM_METADATA_SUPPORT
#define Com_Get_SchM_Enter_Com_RxIPduMetaData
#define Com_Get_SchM_Exit_Com_RxIPduMetaData

#define Com_Get_SchM_Enter_Com_TxIPduMetaData
#define Com_Get_SchM_Exit_Com_TxIPduMetaData
#endif

#else

#define Com_Get_SchM_Enter_Com_RxIPduProcess
#define Com_Get_SchM_Exit_Com_RxIPduProcess

#define Com_Get_SchM_Enter_Com_RxIndication
#define Com_Get_SchM_Exit_Com_RxIndication

#define Com_Get_SchM_Enter_Com_TxIPdu
#define Com_Get_SchM_Exit_Com_TxIPdu

#define Com_Get_SchM_Enter_Com_RxPduBuffer               SchM_Enter_Com_RxPduBuffer();
#define Com_Get_SchM_Exit_Com_RxPduBuffer                SchM_Exit_Com_RxPduBuffer();

#define Com_Get_SchM_Enter_Com_RxSigBuff(APINAME)        SchM_Enter_Com_RxSigBuff(APINAME);
#define Com_Get_SchM_Exit_Com_RxSigBuff(APINAME)         SchM_Exit_Com_RxSigBuff(APINAME);

#ifdef COM_RX_SIGNALGROUP_ARRAY
#define Com_Get_SchM_Enter_Com_RxSigGrpArrayBuff         SchM_Enter_Com_RxSigGrpArrayBuff();
#define Com_Get_SchM_Exit_Com_RxSigGrpArrayBuff          SchM_Exit_Com_RxSigGrpArrayBuff();
#endif /* #ifdef COM_RX_SIGNALGROUP_ARRAY */

#define Com_Get_SchM_Enter_Com_RxGrpSigSecBuff           SchM_Enter_Com_RxGrpSigSecBuff();
#define Com_Get_SchM_Exit_Com_RxGrpSigSecBuff            SchM_Exit_Com_RxGrpSigSecBuff();

#define Com_Get_SchM_Enter_Com_RxSigToTicks              SchM_Enter_Com_RxSigToTicks();
#define Com_Get_SchM_Exit_Com_RxSigToTicks               SchM_Exit_Com_RxSigToTicks();

#define Com_Get_SchM_Enter_Com_RxSigGrpToTicks           SchM_Enter_Com_RxSigGrpToTicks();
#define Com_Get_SchM_Exit_Com_RxSigGrpToTicks            SchM_Exit_Com_RxSigGrpToTicks();

#define Com_Get_SchM_Enter_Com_RxSigDynBuff              SchM_Enter_Com_RxSigDynBuff();
#define Com_Get_SchM_Exit_Com_RxSigDynBuff               SchM_Exit_Com_RxSigDynBuff();

#define Com_Get_SchM_Enter_Com_TxIpduProtArea(APINAME)   SchM_Enter_Com_TxIpduProtArea(APINAME);
#define Com_Get_SchM_Exit_Com_TxIpduProtArea(APINAME)    SchM_Exit_Com_TxIpduProtArea(APINAME);

#define Com_Get_SchM_Enter_Com_TxGrpSigBuffer            SchM_Enter_Com_TxGrpSigBuffer();
#define Com_Get_SchM_Exit_Com_TxGrpSigBuffer             SchM_Exit_Com_TxGrpSigBuffer();

#ifdef COM_METADATA_SUPPORT
#define Com_Get_SchM_Enter_Com_RxIPduMetaData            SchM_Enter_Com_RxIPdu_RECEIVE_METADATA();
#define Com_Get_SchM_Exit_Com_RxIPduMetaData             SchM_Exit_Com_RxIPdu_RECEIVE_METADATA();

#define Com_Get_SchM_Enter_Com_TxIPduMetaData            SchM_Enter_Com_TxIPdu_TRANSMIT_METADATA();
#define Com_Get_SchM_Exit_Com_TxIPduMetaData             SchM_Exit_Com_TxIPdu_TRANSMIT_METADATA();
#endif

#endif /* #ifdef COM_PDUBASEDLOCKSENABLED */


/* Access macros for calibration */

#ifdef COM_RxIPduTimeout
/* FC_VariationPoint_START */
#ifdef COM_RXPDU_TIMEOUT_VIA_CALIBRATION
/* Call function to get the calibration value RxIPduTimeout */
#define COM_GET_RXIPDU_TIMEOUT(idRxPdu, rxIpduConstPtr_pcst)             Com_Prv_GetRxIPduTimeoutCalibration((PduIdType)(idRxPdu))
#else
/* FC_VariationPoint_END */
/* Get RxIPduTimeout */
#define COM_GET_RXIPDU_TIMEOUT(idRxPdu, rxIpduConstPtr_pcst)             ((rxIpduConstPtr_pcst)->timeout_u16)
/* FC_VariationPoint_START */
#endif /* #ifdef COM_RXPDU_TIMEOUT_VIA_CALIBRATION */
/* FC_VariationPoint_END */
#endif /* #ifdef COM_RxIPduTimeout */

/* FC_VariationPoint_START */
#ifdef COM_TXPDU_TIMEOFFSET_VIA_CALIBRATION
/* Call function to get the calibration value TxIPduTimeOffset */
#define COM_GET_TXIPDU_TIMEOFFSET(idTxPdu, currentTxModePtr_pcst)        Com_Prv_GetTxIPduTimeOffsetCalibration((PduIdType)(idTxPdu))
#else
/* FC_VariationPoint_END */
/* Get TxIPduTimeOffset */
#define COM_GET_TXIPDU_TIMEOFFSET(idTxPdu, currentTxModePtr_pcst)        ((currentTxModePtr_pcst)->timeOffset_u16)
/* FC_VariationPoint_START */
#endif /* COM_TXPDU_TIMEOFFSET_VIA_CALIBRATION */
/* FC_VariationPoint_END */

/* FC_VariationPoint_START */
#ifdef COM_TXPDU_TIMEPERIOD_VIA_CALIBRATION
/* Call function to get the calibration value TxIPduTimePeriod */
#define COM_GET_TXIPDU_TIMEPERIOD(idTxPdu, currentTxModePtr_pcst)        Com_Prv_GetTxIPduCyclePeriod((PduIdType)(idTxPdu))
#elif (COM_COMMON_TX_PERIOD == STD_ON)
/* Get TxIPduTimePeriod */
#define COM_GET_TXIPDU_TIMEPERIOD(idTxPdu, currentTxModePtr_pcst)        ((Com_ReducedPeriod_u16 == COM_ZERO)? \
                                                                         ((currentTxModePtr_pcst)->timePeriod_u16) : Com_ReducedPeriod_u16)
#else
/* FC_VariationPoint_END */
/* Get TxIPduTimePeriod */
#define COM_GET_TXIPDU_TIMEPERIOD(idTxPdu, currentTxModePtr_pcst)        ((currentTxModePtr_pcst)->timePeriod_u16)
/* FC_VariationPoint_START */
#endif /* #ifdef COM_TXPDU_TIMEPERIOD_VIA_CALIBRATION */
/* FC_VariationPoint_END */

/* FC_VariationPoint_START */
#ifdef COM_TXPDU_NUMOFREPETITION_VIA_CALIBRATION
/* Call function to get the calibration value TxIPduNumOfRepetition */
#define COM_GET_TXIPDU_NUMOFREPETITION(idTxPdu, currentTxModePtr_pcst)   Com_Prv_GetTxIPduNumberOfRepetitionCalibration((PduIdType)(idTxPdu))
#else
/* FC_VariationPoint_END */
/* Get TxIPduNumOfRepetition */
#define COM_GET_TXIPDU_NUMOFREPETITION(idTxPdu, currentTxModePtr_pcst)   ((currentTxModePtr_pcst)->numOfRepetitions_u8)
/* FC_VariationPoint_START */
#endif /* #define COM_TXPDU_NUMOFREPETITION_VIA_CALIBRATION */
/* FC_VariationPoint_END */

/* END: Access MACROS */

#endif /* COM_PRV_H */
