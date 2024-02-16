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

#ifndef COM_H
#define COM_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
/* TRACE[SWS_Com_00609][SWS_Com_00865] - ComStack_Types.h, Std_Types.h */
#include "ComStack_Types.h"
#include "Com_Cfg.h"
#include "Com_PBcfg.h"

#if(COM_ENABLE_INTER_MODULE_CHECKS)
/* [SWS_BSW_00036] Inter Module Checks - ComStack_Types.h */
# if (!defined(COMTYPE_AR_RELEASE_MAJOR_VERSION) || (COMTYPE_AR_RELEASE_MAJOR_VERSION != COM_AR_RELEASE_MAJOR_VERSION))
#  error "AUTOSAR major version undefined or mismatched"
# endif
# if (!defined(COMTYPE_AR_RELEASE_MINOR_VERSION) || (COMTYPE_AR_RELEASE_MINOR_VERSION != COM_AR_RELEASE_MINOR_VERSION))
#  error "AUTOSAR minor version undefined or mismatched"
# endif
#endif /* #if(COM_ENABLE_INTER_MODULE_CHECKS) */

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/* DET Error ID's */

/**
 * @ingroup COM_H
 *
 * TRACE[SWS_Com_91014]
 * This macro represents the Error Id for the event when API service is called with wrong parameter. \n
 */
#define COM_E_PARAM                                                 0x01
/**
 * @ingroup COM_H
 *
 * TRACE[SWS_Com_91014]
 * This macro represents the Error Id for the event when API service is called before COM module is uninitialised. \n
 */
#define COM_E_UNINIT                                                0x02
/**
 * @ingroup COM_H
 *
 * TRACE[SWS_Com_91014]
 * This macro represents the Error Id for the event when API service is called with NULL pointer. \n
 */
#define COM_E_PARAM_POINTER                                         0x03
/**
 * @ingroup COM_H
 *
 * TRACE[SWS_Com_91014]
 * This macro represents the Error Id for the event when API service is called with Invalid configuration set. \n
 */
#define COM_E_INIT_FAILED                                           0x04
/**
 * @ingroup COM_H
 *
 * TRACE[SWS_Com_91015]
 * This macro represents runtime error code when the Transmission request was skipped \n
 */
#define COM_E_SKIPPED_TRANSMISSION                                  0x05
/**
 * @ingroup COM_H
 *
 * This macro represents the Error Id in case of not permitted value is updated by caller. \n
 * [non-autosar]: created for internal checks
 */
#define COM_E_INVALID_VALUE                                         0xF0
/**
 * @ingroup COM_H
 *
 * TRACE[SWS_Com_00865]
 * This macro represents the Return code for the event when the service is currently not available. \n
 */
#define COM_SERVICE_NOT_AVAILABLE                                   0x80
/**
 * @ingroup COM_H
 *
 * TRACE[SWS_Com_00865]
 * This macro represents the Return code for the event when the buffer of a large I-PDU is locked. \n
 */
#define COM_BUSY                                                    0x81
/**
 * @ingroup COM_H
 *
 * TRACE[SWS_Com_00442]
 * This macro represents the identifier of the index based instance of COM module. \n
 */
#define COM_INSTANCE_ID                                             0u

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define COM_START_SEC_CODE
#include "Com_MemMap.h"

/* START: API declarations */

/**
 * @ingroup COM_H
 *
 * This API initializes the Com Interface.\n
 *
 * @param in       config_pcst - Pointer to Com module's configuration data.\n
 *
 * @Param None \n
 * @return None \n
 */
/* TRACE[SWS_Com_00432] Com_Init() */
extern void Com_Init(const Com_ConfigType * config_pcst);

/**
  * @ingroup COM_H
  *
  * This API deinitializes the Com Interface.\n
  *
  * @param None \n
  * @return None \n
  */
/* TRACE[SWS_Com_00130] Com_DeInit() */
extern void Com_DeInit(void);
/* TRACE[SWS_Com_91001] Com_IpduGroupStart() */
extern void Com_IpduGroupStart(Com_IpduGroupIdType idIpduGrp_u16, boolean initialize_b);
/* TRACE[SWS_Com_91002] Com_IpduGroupStop() */
extern void Com_IpduGroupStop(Com_IpduGroupIdType idIpduGrp_u16);
/* TRACE[SWS_Com_91003] Com_DisableReceptionDM() */
extern void Com_DisableReceptionDM(Com_IpduGroupIdType idIpduGrp_u16);
/* TRACE[SWS_Com_91004] Com_EnableReceptionDM() */
extern void Com_EnableReceptionDM(Com_IpduGroupIdType idIpduGrp_u16);

/**
 * @ingroup COM_H
 *
 * Service to get the status of the AUTOSAR COM module.\n
 *
 * @return       COM_INIT :the AUTOSAR COM module is initialized and usable.\n
 * @return       COM_UNINIT:AUTOSAR COM module is not initialized and not usable \n
 */
/* TRACE[SWS_Com_00194] Com_GetStatus() */
extern Com_StatusType Com_GetStatus(void);

/* TRACE[SWS_Com_00426] This function shall be pre compile time configurable On/Off by the configuration
    parameter Com_VERSION_INFO_API */
/**
 * @ingroup COM_H
 *
 * Service for obtaining the Version info.\n
 * @param out       versioninfo_pst - Pointer to where to store the version information of this module.\n
 *
 * @return None \n
 */
extern void Com_GetVersionInfo(Std_VersionInfoType * versioninfo_pst);

/**
 * @ingroup COM_H
 *
 * Service updates the signal object identified by SignalId with the signal referenced by the SignalDataPtr parameter\n
 *
 * @param in       idSignal_u16 - Id of the signal.\n
 * @param in       signalDataPtr_pcv - The pointer to the address where the application data is available.\n
 * @return         E_OK - Service has been accepted.\n
 *                    COM_SERVICE_NOT_AVAILABLE - Corresponding I-PDU group was stopped.\n
 */
extern uint8 Com_SendSignal(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv);
/* Service updates the signal object identified by SignalId with the signal referenced by the SignalDataPtr parameter
 * and meta data is provided by metadataptr */
extern uint8 Com_SendSignalWithMetaData(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv,
                                        const uint8 * metaDataPtr_pcu8);
/* Com_SendDynSignalWithMetaData api updates the signal object identified by SignalId with the signal referenced by
 * the SignalDataPtr parameter and meta data is provided by metadataptr */
extern uint8 Com_SendDynSignalWithMetaData(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv,
                                                     uint16 length_u16, const uint8 * metaDataPtr_pcu8);
/* Service copies the content of the associated shadow buffer to the associated I-PDU buffer and meta data is provided
 * by the metadataptr. Prior to this call, all group signals should be updated in the shadow buffer by the call of
 * Com_SendSignal.*/
extern uint8 Com_SendSignalGroupWithMetaData(Com_SignalGroupIdType idSignalGroup_u16, const uint8 * metaDataPtr_pcu8);
/* Service copies the content of the provided SignalGroupArrayPtr to the associated I-PDU.
 * The provided data shall correspond to the array representation of the signal group.
 * The meta data is provided by metadataptr */
extern uint8 Com_SendSignalGroupArrayWithMetaData(Com_SignalGroupIdType idSignalGroup_u16,
                   const uint8 * signalGroupArrayPtr_pcu8, const uint8 * metaDataPtr_pcu8);

/**
 * @ingroup COM_H
 *
 * Service for receiving the signal, This API copies the data to the application buffer.\n
 *
 * @param in       idSignal_u16 - Id of the signal.\n
 * @param in       signalDataPtr_pv - The pointer to the address where the application data is available.\n
 * @return         E_OK - Service has been accepted.\n
 *                 COM_SERVICE_NOT_AVAILABLE - Corresponding I-PDU group was stopped.\n
 */
extern uint8 Com_ReceiveSignal(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv);

/**
 * @ingroup COM_H
 *
 * Service for Invalidating the signal.\n
 *
 * @param in       idSignal_u16 - Id of the signal.\n
 * @return         E_OK - Service has been accepted.\n
 *                 COM_SERVICE_NOT_AVAILABLE - Corresponding I-PDU group was stopped,
 *                 no ComSignalDataInvalidValue is configured for the given signalId
 *                 or service fails due to development error.\n
 */
extern uint8 Com_InvalidateSignal(Com_SignalIdType idSignal_u16);
#ifdef COM_TX_SIGNALGROUP

/**
 * @ingroup COM_H
 *
 * Service copies the content of the associated shadow buffer to the associated I-PDU buffer.Prior to this call,
   all group signals should be updated in the shadow buffer by the call of Com_SendSignal.\n
 *
 * @param in       idSignalGroup_u16 - Id of the SignalGroup.\n
 * @return         E_OK - Service has been accepted.\n
 *                 COM_SERVICE_NOT_AVAILABLE - Corresponding I-PDU group was stopped.\n
 */
extern uint8 Com_SendSignalGroup(Com_SignalGroupIdType idSignalGroup_u16);

/**
 * @ingroup COM_H
 *
 * Service for Invalidating the Signal Group.\n
 *
 * @param in       idSignalGroup_u16 - Id of the Signal Group.\n
 * @return         E_OK - Service has been accepted.\n
 *                 COM_SERVICE_NOT_AVAILABLE - Corresponding I-PDU group was stopped,
 *                 no ComSignalDataInvalidValue is configured for any of the group signals
 *                 or service fails due to development error\n
 */
extern uint8 Com_InvalidateSignalGroup(Com_SignalGroupIdType idSignalGroup_u16);
#endif /* #ifdef COM_TX_SIGNALGROUP */

#ifdef COM_RX_SIGNALGROUP

/**
 * @ingroup COM_H
 *
 * Service used to copy the SignalGroup content from IpduBuffer to Shadowbuffer.\n
 *
 * @param in       idSignalGroup_u16 - Id of the Signal Group.\n
 * @return         E_OK - Service has been accepted.\n
 *                 COM_SERVICE_NOT_AVAILABLE - Corresponding I-PDU group was stopped.\n
 */
/* TRACE[SWS_Com_00201] Com_ReceiveSignalGroup() */
extern uint8 Com_ReceiveSignalGroup(Com_SignalGroupIdType idSignalGroup_u16);

#endif /* #ifdef COM_RX_SIGNALGROUP */

/**
 * @ingroup COM_H
 *
 * Service updates the signal object identified by SignalId with the signal referenced by the SignalDataPtr parameter,
 * with length referenced by length parameter.\n
 * @param in       idSignal_u16         - Id of the signal.\n
 * @param in       signalDataPtr_pcv - The pointer to the address where the application data is available.\n
 * @param in       length_u16         - The Length of the signal to be transmitted.\n
 * @return         E_OK - Service has been accepted.\n
 *                 COM_SERVICE_NOT_AVAILABLE - Corresponding I-PDU group was stopped.\n
 *                 E_NOT_OK    - if the requested length is greater than maximum configured length.\n
 */
extern uint8 Com_SendDynSignal(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv, uint16 length_u16);

/**
 * @ingroup COM_H
 *
 * Service for receiving the signal, This API copies the data to the application buffer, with length in LengthPtr.\n
 *
 * @param in       idSignal_u16     - Id of the signal.\n
 * @param in       signalDataPtr_pv - The pointer to the address where the application data is available.\n
 * @param in/out   lengthPtr_pu16   - maximum length that could be received \n
 *                 LengthPtr - length of the dynamic length signal \n
 * @return         E_OK - Service has been accepted.\n
 *                 COM_SERVICE_NOT_AVAILABLE - Corresponding I-PDU group was stopped.\n
 *                 E_NOT_OK - if application requests for length less than the received length\n
 */
extern uint8 Com_ReceiveDynSignal(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv, uint16 * lengthPtr_pu16);

/**
 * @ingroup COM_H
 *
 * Service copies the content of the provided SignalGroupArrayPtr to the associated I-PDU.
 * The provided data shall correspond to the array representation of the signal group.
 *
 * @param in       idSignalGroup_u16         - Id of signal group to be sent \n
 *                 *signalGroupArrayPtr_pcu8 - Reference to the signal group array \n
 *
 * @return         E_OK                      : service has been accepted
 *                 COM_SERVICE_NOT_AVAILABLE : corresponding I-PDU group was stopped
 *                                              (or service failed due to development error)
 *                 COM_BUSY                  : in case the TP-Buffer is locked for large data types handling\n
 */
extern uint8 Com_SendSignalGroupArray(Com_SignalGroupIdType idSignalGroup_u16, const uint8 *signalGroupArrayPtr_pcu8);

/**
 * @ingroup COM_H
 *
 * Service copies the received signal group array representation from the PDU to the location designated by
 * SignalGroupArrayPtr. \n
 *
 * @param in    : idSignalGroup_u16        - Id of signal group to be sent \n
 * @param out   : *signalGroupArrayPtr_pu8 - reference to the location where the received signal group array
 *                                       shall be stored \n
 *
 * @return      : E_OK                      : service has been accepted
 *                COM_SERVICE_NOT_AVAILABLE : corresponding I-PDU group was stopped
 *                                            (or service failed due to development error)
 *                COM_BUSY                  : in case the TP-Buffer is locked for large data types handling\n
 */
extern uint8 Com_ReceiveSignalGroupArray(Com_SignalGroupIdType idSignalGroup_u16, uint8 *signalGroupArrayPtr_pu8);

/**
 * @ingroup COM_H
 *
 * Service used within the Ipdu-Callout. The I-PDU with the given ID is triggered for transmission.\n
 *
 * @param in       idPdu_uo - Id of the Tx Ipdu.\n
 *
 * @return None \n
 */
extern void Com_TriggerIPDUSend(PduIdType idPdu_uo);
/* Service is triggered to transmit I-PDU with Metadata. */
extern Std_ReturnType Com_TriggerIPDUSendWithMetaData(PduIdType idPdu_uo, uint8 * metaData_pu8);
/* Service for receiving the signal with metadata value, this API copies the data to the application buffer and
 * received metadata is stored at the position given by metadataptr */
extern uint8 Com_ReceiveSignalWithMetaData(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv,
                                           uint8 * metaDataPtr_pu8);
/* Service for receiving the dynamic signal/group signal with metadata value, this API copies the data to the
 * application buffer and received metadata is stored at the position given by metadataptr */
extern uint8 Com_ReceiveDynSignalWithMetaData(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv,
                                                      uint16 * lengthPtr_pu16, uint8 * metaDataPtr_pu8);
/* Service used to copy the SignalGroup content from IpduBuffer to Shadowbuffer and received metadata is stored at
 * the position given by metadataptr */
extern uint8 Com_ReceiveSignalGroupWithMetaData(Com_SignalGroupIdType idSignalGroup_u16, uint8* metaDataPtr_pu8);
/* Service used to copy the received signalgroup array representation from PDU to the location specified by
 * signalGroupArrayPtr and received metadata is stored at the position given by metadataptr */
extern uint8 Com_ReceiveSignalGroupArrayWithMetaData(Com_SignalGroupIdType idSignalGroup_u16,
                                                     uint8 * signalGroupArrayPtr_pu8, uint8* metaDataPtr_pu8);

/**
 * @ingroup COM_H
 *
 * Service called by the lower layer when an AUTOSAR COM I-PDU shall be transmitted. Copies the contents of
 * I-PDU transmit buffer to the L-PDU buffer given by PduInfoPtr.\n
 *
 * @param in       idTxPdu_uo         - Id of the Tx Ipdu.\n
 * @param in       pduInfoPtr_pst     - Pointer to the address where the COM module shall copy the Ipdu data and
 *                                      length information.\n
 * @return         E_OK - SDU has been copied and SduLength indicates the number of copied bytes. \n
                   E_NOT_OK - No SDU has been copied. PduInfoPtr must not be used since it may contain a\n
                              NULL pointer or point to invalid data.
 */
extern Std_ReturnType Com_TriggerTransmit(PduIdType idTxPdu_uo, PduInfoType * pduInfoPtr_pst);

/**
 * @ingroup COM_H
 *
 * Service called by the lower layer after an I-PDU has been received.\n
 *
 * @param in       idRxPdu_uo         - Id of the received Ipdu.\n
 * @param in       pduInfoPtr_pcst     - Contains the length of the received I-PDU and a pointer to a buffer
 *                                       containing the I-PDU.\n
 *
 * @return None \n
 */
extern void Com_RxIndication(PduIdType idRxPdu_uo, const PduInfoType * pduInfoPtr_pcst);

/**
 * @ingroup COM_H
 *
 * Service called by the lower layer after the PDU has been transmitted on the network.\n
 *
 * @param in       idTxPdu_uo - Id of the Tx Ipdu.\n
 *
 * @return None \n
 */
extern void Com_TxConfirmation(PduIdType idTxPdu_uo,Std_ReturnType result_u8);

/* FC_VariationPoint_START */
/**
* @ingroup COM_H
*
* The below function is used to change the Periodicity of all the Tx frames .\n
*
* @param in      commonPeriod_u16 - New period (interms of multiples of Com_mainFunctionTx() raster time)\n
*
* @return None \n
*/
extern void Com_ReduceBusload(uint16 commonPeriod_u16);

/**
* @ingroup COM_H
*
* The below function is used to change the period of all the Tx frames to a old period from common TX period, set by \n
* function Com_ReduceBusload().\n
*
* @return None \n
*/
extern void Com_RestoreBusload(void);
/* FC_VariationPoint_END */

/* FC_VariationPoint_START */
/* CC specific function */
 /**
 * @ingroup COM_H
 *
 * this function checks whether the Tx frame is scheduled to be sent in the coming cycle.\n
 *
 * @param in       idTxPdu_uo - Tx ipdu ID.\n
 * @param callerTaskCycleInMs_u16 , this parameter is the value of caller's task cycle in ms.
 * i.e : if the Caller is in 1ms, then the ComCallerTaskCycle = 1
 * @return None \n
 */
extern boolean Com_IsTxScheduled(PduIdType idTxPdu_uo, uint16 callerTaskCycleInMs_u16);
/* FC_VariationPoint_END */

/* FC_VariationPoint_START */
/**
 * @ingroup COM_H
 *
 *  Service for Reading the length of the received IPdu.\n
 *
 * @param in       idRxPdu_uo             - Id of the reception I-PDU.\n
 * @param out      rxIPduLengthPtr_puo     - Memory address to update received PduLength.\n
 *
 * @return         E_OK - Service has been accepted.\n
 * @return         COM_SERVICE_NOT_AVAILABLE- Development error, or IPduGroup is stopped.\n
 */
extern uint8 Com_ReadRxIPduLength(PduIdType idRxPdu_uo, PduLengthType * rxIPduLengthPtr_puo);
/* FC_VariationPoint_END */

/**
 * @ingroup COM_H
 *
 * The service Com_SwitchIpduTxMode sets the transmission mode of the I-PDU referenced by PduId to Mode.\n
 *
 * @param in       idPdu_uo - Id of the I-PDU of which the transmission mode shall be changed.\n
 * @param in       mode_b     - The transmission mode that shall be set.\n
 *
 * @return None \n
 */
extern void Com_SwitchIpduTxMode(PduIdType idPdu_uo, boolean mode_b);

/**
 * @ingroup COM_H
 *
 * The service COM_ProvideRxIpduStatus returns the status of Rx I-PDU referenced by PduId.\n
 *
 * @param in       idPdu_uo - Id of the I-PDU of which the IPDU status is required.\n
 *
 * @return boolean - the service returns TRUE,  if the Ipdu is STARTED,\n
 *                                  returns FALSE, if the Ipdu is STOPPED.\n
 */
extern boolean Com_ProvideRxIpduStatus(PduIdType idPdu_uo);

/**
 * @ingroup COM_H
 *
 * The service COM_ProvideTxIpduStatus returns the status of Tx I-PDU referenced by PduId.\n
 *
 * @param in       idPdu_uo - Id of the I-PDU of which the IPDU status is required.\n
 *
 * @return boolean - the service returns TRUE,  if the Ipdu is STARTED,\n
 *                                  returns FALSE, if the Ipdu is STOPPED.\n
 */
extern boolean Com_ProvideTxIpduStatus(PduIdType idPdu_uo);
extern BufReq_ReturnType Com_StartOfReception(PduIdType idComRxPdu_uo, const PduInfoType* pduInfoPtr_pcst,
                                              PduLengthType tpSduLength_uo, PduLengthType* rxBufferSizePtr_puo);
extern BufReq_ReturnType Com_CopyRxData(PduIdType idPdu_uo,const PduInfoType* pduInfoPtr_pcst,
                                        PduLengthType* rxBufferSizePtr_puo);
extern BufReq_ReturnType Com_CopyTxData(PduIdType idPdu_uo, const PduInfoType* pduInfoPtr_pcst,
                                        const RetryInfoType* retryInfoPtr_pcst, PduLengthType* availableDataPtr_puo);
extern void Com_TpRxIndication(PduIdType idPdu_uo, Std_ReturnType result_u8);
extern void Com_TpTxConfirmation(PduIdType idPdu_uo, Std_ReturnType result_u8);

/* FC_VariationPoint_START */
extern void Com_SetTxIPduControlViaRbaNdsEcuVariant(PduIdType idIpdu_uo, boolean ipduStatus_b);
extern void Com_SetRxIPduControlViaRbaNdsEcuVariant(PduIdType idIpdu_uo, boolean ipduStatus_b);
extern Std_ReturnType Com_SetRxIPduTimeoutTicks(PduIdType idRxPdu_uo, uint16 timeoutTicks_u16);
/* FC_VariationPoint_END */

/* END: API declarations */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* #ifndef COM_H */

