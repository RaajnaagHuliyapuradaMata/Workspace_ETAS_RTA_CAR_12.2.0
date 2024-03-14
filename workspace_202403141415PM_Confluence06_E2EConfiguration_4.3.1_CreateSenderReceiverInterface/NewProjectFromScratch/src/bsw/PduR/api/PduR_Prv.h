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

#ifndef PDUR_PRV_H
#define PDUR_PRV_H

#include "PduR.h"
#include "PduR_Cfg_Internal.h"
#include "PduR_Cfg_SchM.h"


#if defined(PDUR_DET_MODULE_AVAILABLE)
#include "Det.h"   /* For Det_ReportError() */
#if(PDUR_ENABLE_INTER_MODULE_CHECKS)
/* Inter Modular Check */
#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || \
     (DET_AR_RELEASE_MAJOR_VERSION != PDUR_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(DET_AR_RELEASE_MINOR_VERSION) || \
     (DET_AR_RELEASE_MINOR_VERSION != PDUR_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched"
#endif /* #if(PDUR_ENABLE_INTER_MODULE_CHECKS) */
#endif
#endif

#include "PduR_Types.h"
#include "PduR_Cfg_SymbolicNames.h"

#if defined(PDUR_ZERO_COST_OPERATION) && (PDUR_ZERO_COST_OPERATION != STD_OFF)
#include "PduR_ZeroCost_Cfg.h"
#endif

/**
 * @ingroup PDUR_H
 *
 * magic number - Required only for configuration\n
 */
#define PDUR_CONFIGTYPE_MAGIC      (0x025fe8a7uL)

/* ------------------------------------------------------------------------ */


/* PduR_LayerType */
/* Allocate each layer a separate bit, so that a PduR_LayerType
 * can be treated as a collection of flags. */
/**
 * @ingroup PDUR_H
 *
 * Allocate CanTp layer a separate bit, so that a PduR_LayerType_CanTp can be treated as flag.
 */
#define PDUR_LAYER_CANTP           ((uint8)0x01)
/**
 * @ingroup PDUR_H
 *
 * Allocate FrTp layer a separate bit, so that a PduR_LayerType_FrTp can be treated as flag.
 */
#define PDUR_LAYER_FRTP            ((uint8)0x02)
/**
 * @ingroup PDUR_H
 *
 * Allocate LinTp layer a separate bit, so that a PduR_LayerType_LinTp can be treated as flag.
 */
#define PDUR_LAYER_LINTP           ((uint8)0x04)
/**
 * @ingroup PDUR_H
 *
 * Allocate CanIf layer a separate bit, so that a PduR_LayerType_CanIf can be treated as flag.
 */
#define PDUR_LAYER_CANIF           ((uint8)0x10)
/**
 * @ingroup PDUR_H
 *
 * Allocate FrIf layer a separate bit, so that a PduR_LayerType_FrIf can be treated as flag.
 */
#define PDUR_LAYER_FRIF            ((uint8)0x20)
/**
 * @ingroup PDUR_H
 *
 * Allocate LinIf layer a separate bit, so that a PduR_LayerType_LinIf can be treated as flag.
 */
#define PDUR_LAYER_LINIF           ((uint8)0x40)
/**
 * @ingroup PDUR_H
 *
 * Allocate a separate bit, so that a PduR_Multicast can be treated as flag.Only used for TP/FIFO routes
 */
#define PDUR_MULTICAST             ((uint8)0x80)    /* Only used for TP/FIFO routes */
/**
 * @ingroup PDUR_H
 *
 * Allocate IpduM Lo layer a separate bit, so that a PduR_LayerType_IpduMLo can be treated as flag.
 */
#define PDUR_LAYER_IPDUMLO         ((uint8)0x08)

/**
 * @ingroup PDUR_H
 *
 * Allocate rba_J1939FdTp layer a separate bit, so that a PduR_LayerType_rba_J1939FdTp can be treated as flag.
 */

#define PDUR_LAYER_RBA_J1939FDTP   ((uint8)0x08)

/**
 * @ingroup PDUR_H
 *
 * Allocate J1939Tp layer a separate bit, so that a PduR_LayerType_J1939Tp can be treated as flag.
 */

#define PDUR_LAYER_J1939TP         ((uint8)0x08)

/* ------------------------------------------------------------------------ */

/**
 * @ingroup PDUR_H
 *
 * Event IDs
 * typedef enum {
 *    PDUR_EVENT_RXINDICATION = 0x01,
 *   PDUR_EVENT_TRANSMITCONFIRMATION ,
 *   PDUR_EVENT_COPYTXDATA,
 *   PDUR_EVENT_COPYRXDATA ,
 *   PDUR_EVENT_TRANSMIT,
 *   PDUR_EVENT_CANCELTRANSMIT,
 *   PDUR_EVENT_TRIGGERTRANSMIT
 *    } PduR_EventType;
 */
typedef enum {
    PDUR_EVENT_RXINDICATION = 0x01,
    PDUR_EVENT_TRANSMITCONFIRMATION ,
    PDUR_EVENT_COPYTXDATA,
    PDUR_EVENT_COPYRXDATA ,
    PDUR_EVENT_TRANSMIT,
    PDUR_EVENT_CANCELTRANSMIT,
    PDUR_EVENT_TRIGGERTRANSMIT
} PduR_EventType;

/* ------------------------------------------------------------------- */
/**
 * @ingroup PDUR_PRV_H
 *
 * Checking for PDUR init state
 * */
#define  PDUR_CHECK_STATE_VOID(instance_id, sid)                     \
   if ( PduR_State == PDUR_UNINIT )                     \
   {                                                    \
      PDUR_REPORT_ERROR((instance_id), (sid), PDUR_E_INVALID_REQUEST); \
      return;                                           \
   }

/**
 * @ingroup PDUR_PRV_H
 *
 * Returns the state and if the PDU Router has not been initialized (PDUR_UNINIT state) all services except PduR_Init() will
 * report the error PDUR_E_INVALID_REQUEST through the Development Error Tracer (DET) when called
 */

#define  PDUR_CHECK_STATE_RET(instance_id, sid, ret)                  \
   if ( PduR_State == PDUR_UNINIT )                     \
   {                                                    \
      PDUR_REPORT_ERROR((instance_id), (sid), PDUR_E_INVALID_REQUEST); \
      return (ret);                                       \
   }

/**
 * @ingroup PDUR_PRV_H
 *
 * Checks for invalid pointer.If the pointer is invalid, PDUR_E_NULL_POINTER is reported via DET
 */
#define  PDUR_CHECK_PTR_RET(instance_id, sid, ptr, ret)                      \
   if ( (ptr) == NULL_PTR )                                                \
   {                                                            \
      PDUR_REPORT_ERROR((instance_id), (sid), PDUR_E_NULL_POINTER);        \
      return (ret);                                               \
   }

/**
 * @ingroup PDUR_PRV_H
 *
 * checking for NULL or void pointer
 * */

#define  PDUR_CHECK_PTR_VOID(instance_id, sid, ptr)                          \
   if ( (ptr) == NULL_PTR)                                                \
   {                                                            \
      PDUR_REPORT_ERROR((instance_id), (sid), PDUR_E_NULL_POINTER);        \
      return;                                                   \
   }


/**
 * @ingroup PDUR_PRV_H
 *
 * report RunTime Error
 */
#if defined(PDUR_DET_MODULE_AVAILABLE)
#define PDUR_REPORT_RUNTIME_ERROR(instanceID, serviceId, errorId) (void)Det_ReportRuntimeError(PDUR_MODULE_ID, instanceID , serviceId, errorId)
#else
#define PDUR_REPORT_RUNTIME_ERROR(instanceID, serviceId, errorId) do {} while(0)
#endif
#if defined(PDUR_DEV_ERROR_DETECT) && (PDUR_DEV_ERROR_DETECT != STD_OFF) && defined(PDUR_DET_MODULE_AVAILABLE)
/**
 * @ingroup PDUR_PRV_H
 *
 * Instance id is added - Autosar Release2.1
 */
#define PDUR_REPORT_ERROR(instanceID, serviceId, errorId) (void)Det_ReportError(PDUR_MODULE_ID, instanceID , serviceId, errorId)  

#else


/**
 * @ingroup PDUR_PRV_H
 *
 * To remove MISRA warning
 */
#define PDUR_REPORT_ERROR(instanceID, serviceId, errorId) do {} while(0) /* To remove MISRA warning */
#endif


#if (PDUR_DSM_REINT_SUPPORT != STD_OFF)
/* ------------------------------------------------------------------------ */
/* Begin a set of variable definitions */


#define PDUR_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "PduR_MemMap.h"

extern PduR_DsmReInitType PduR_DsmReInitStruct;

#define PDUR_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "PduR_MemMap.h"
#endif

/**
 * @ingroup PDUR_H
 *
 * PduR_Base: Access to configuration  \n
 *
 */
/* ------------------------------------------------------------------------ */
/* Begin a set of variable definitions */

#define PDUR_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "PduR_MemMap.h"

extern const PduR_PBConfigType * PduR_Base;

/* ------------------------------------------------------------------------ */
/* End section for variable */

#define PDUR_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "PduR_MemMap.h"

#if(PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)

/* ------------------------------------------------------------------------ */
/* Begin section for variables */
# define PDUR_START_SEC_VAR_CLEARED_UNSPECIFIED
# include "PduR_MemMap.h"

extern const PduR_RPTablesType * PduR_FunctionPtrs;

/* ------------------------------------------------------------------------ */
/* End section for variables */
# define PDUR_STOP_SEC_VAR_CLEARED_UNSPECIFIED
# include "PduR_MemMap.h"

/**
 * @ingroup PDUR_H
 *
 * This macro is used to point to any table in PduR_RoutingPathTables \n
 */
/* MR12 RULE 20.7 VIOLATION: Putting 's' in (), throws compilation error. So cannot remove MISRA warning. */
#define PDUR_BASE_RPTBL(s)        (PduR_FunctionPtrs->s)

#endif

#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED

#include "PduR_MemMap.h"

#if(PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_PRE_COMPILE)

extern const PduR_PBConfigType PduR_GlobalPBConfig;

#endif
extern const PduR_RPTablesType PduR_RoutingPathTables;

#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED

#include "PduR_MemMap.h"
/**
 * @ingroup PDUR_H
 *
 * This Macro is used to point to that particular structure which is generated in PduR_PBConfigType structure
 */
#define PDUR_BASE(s)          (PduR_Base->s)
/**
 * @ingroup PDUR_H
 *
 *
 * This Macro is used to point to that any Lower Transport protocol structure which is generated in PduR_PBConfigType structure
 */
#define PDUR_BASE_LOTP(n,s)       (PduR_Base->LoTpConf[n].s)
/**
 * @ingroup PDUR_H
 *
 * This Macro is used to point to that any Lower Communication Interface which is direct as data provision
 *  structure which is generated in PduR_PBConfigType structure
 */
#define PDUR_BASE_LOIFD(n,s)      (PduR_Base->LoIfDConf[n].s)
/**
 * @ingroup PDUR_H
 *
 * This Macro is used to point to that any Lower Communication Interface which is trigger transmit as data provision
 *  structure which is generated in PduR_PBConfigType structure
 */
#define PDUR_BASE_LOIFTT(n,s)     (PduR_Base->LoIfTTConf[n].s)
/**
 * @ingroup PDUR_H
 *
 * PduR_DcmTransmit: Access to dcmToLo configuration  \n
 */
#define PDUR_BASE_UPTP(n,s)        (PduR_Base->UpTpConf[n].s)       /* PduR_DcmTransmit */

/**
 * @ingroup PDUR_H
 *
 * PduR_<UpIfModule>Transmit: Access to <UpIf>ToLo configuration  \n
 */
#define PDUR_BASE_UPIF(n,s)        (PduR_Base->UpIfConf[n].s)       /* PduR_<UpIfModule>Transmit */

/**
 * @ingroup PDUR_H
 *
 * PduR_<UpTp>CancelReceive: Access to <UpTp>ToLoRx configuration  \n
 */
#define PDUR_BASE_UPTPCANCELRX(n,s)        (PduR_Base->PduR_UpTpToLoTpRxCfg[n].s)       /* PduR_<UpTp>CancelReceive */

/**
 * @ingroup PDUR_H
 *
 * PduR_GF_IFTx: Access to gwToLoMc configuration  \n
 */
#define PDUR_GW_MC_TX_BASE      PDUR_BASE(gwToLoMc)      /* Gateway multicast (If) */

/**
 * @ingroup PDUR_H
 *
 * Gateway multicast If: Access to Gateway multicast If(loIfTxToUpMc) configuration  \n
 */
#define PDUR_LOIF_TX_MC_BASE    PDUR_BASE(loIfTxToUpMc)  /* Gateway multicast If */
/**
 * @ingroup PDUR_H
 *
 * PduR_<Lo>TpCopyTxData, PduR_<Lo>TpTxConfirmation: multicast table \n
 */
#define PDUR_LOTP_TX_MC_BASE    PDUR_BASE(loTpTxToUpMc)  /* PduR_<Lo>TpCopyTxData, PduR_<Lo>TpTxConfirmation: multicast table */
/**
 * @ingroup PDUR_H
 *
 * PduR_<Lo>TpCopyTxData, PduR_<Lo>TpTxConfirmation: multicast state\n
 */
#define PDUR_LOTP_TX_STATE_BASE PDUR_BASE(loTpTxToUpMs)  /* PduR_<Lo>TpCopyTxData, PduR_<Lo>TpTxConfirmation: multicast state */
/**
 * @ingroup PDUR_H
 *
 * PduR_GF_If_RxIndication: Access to Gateway Interface Rx Indication configuration  \n
 */
#define PDUR_GW_IF_TX_BASE      PDUR_BASE(gwIfTx)        /* PduR_GF_If_RxIndication */
/**
 * @ingroup PDUR_H
 *
 * IF Gateway transmission support\n
 */
#define PDUR_GW_IF_BASE         PDUR_BASE(gwIf)          /* IF Gateway transmission support */
/**
 * @ingroup PDUR_H
 *
 * TP Gateway transmission support\n
 */
#define PDUR_GW_TP_BASE         PDUR_BASE(gwTp)          /* TP Gateway transmission support */

/**
 * @ingroup PDUR_H
 *
 * RX TP Reception support\n
 */
#define PDUR_RX_TP_RPG_BASE         PDUR_BASE(rpgRxTp)          /* Rx TP Reception support */

#if defined(PDUR_MODE_DEPENDENT_ROUTING) && (PDUR_MODE_DEPENDENT_ROUTING != 0)
/**
 * @ingroup PDUR_H
 *
 * Routing Path Group support\n
 */
#define PDUR_RPG_CONFIG_INFO    PDUR_BASE(rpg_ConfigInfo)       /* RPG Support */

/**
 * @ingroup PDUR_H
 *
 * Enable\Disable Routing Path depending on RPG config\n
 */
#define PDUR_RPG_ENROUTING_INFO(index)  PduR_Base->pduR_Rpg_Status[index]    /* RPG Support */

#define PDUR_RPGID_MAP_TAB(index)  PduR_Base->rpgId_MappingTab[index]

#endif /*#if defined(PDUR_MODE_DEPENDENT_ROUTING) && (PDUR_MODE_DEPENDENT_ROUTING != 0)*/

#define PDUR_INVALID_RPG_ID                       0xFFFFu
/**
 * @ingroup PDUR_H
 *
 * Valid No. Of Entries\n
 */
#define PDUR_NR_VALID_IDS(rt)   PDUR_BASE(rt##_NrEntries)

#if (PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)
/**
 * @ingroup PDUR_H
 *
 * Access macro to get max RPG Id when post build variant is selected
 */
#define PDUR_GET_RPGID_MAX     (PduR_Base->rpg_NrEntries)
#else
/**
 * @ingroup PDUR_H
 *
 * Access macro to get max RPG Id when pre-compile variant is selected
 */
#if defined(PDUR_MODE_DEPENDENT_ROUTING) && (PDUR_MODE_DEPENDENT_ROUTING != 0)
#define PDUR_GET_RPGID_MAX     (PDUR_RPGID_MAX)
#endif
#endif

/**
 * @ingroup PDUR_PRV_H
 *
 * FIFO routines
 */
#define PDUR_FIFO_WRITE_INC_WRAP(fifo, val)                         \
   do {                                                           \
      ((val)->writePtr) = ((val)->writePtr) + (fifo)->eltSize;       \
      (val)->writeIndex += 1u;                                              \
      if ((val)->writePtr >= ((fifo)->begin + ((fifo)->eltSize * (fifo)->nrElts))) {  \
         (val)->writePtr = (fifo)->begin;                                  \
          (val)->writeIndex = 0u;                                \
      }                                                           \
   } while (0)


#define PDUR_FIFO_READ_INC_WRAP(fifo, val)                         \
   do {                                                           \
      ((val)->readPtr) = ((val)->readPtr) + (fifo)->eltSize;        \
      (val)->readIndex += 1u;                                              \
      if ((val)->readPtr >= ((fifo)->begin + ((fifo)->eltSize * (fifo)->nrElts))) {  \
         (val)->readPtr = (fifo)->begin;                                    \
          (val)->readIndex = 0u;                                \
      }                                                           \
   } while (0)

/* Section code start
*/
#define PDUR_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "PduR_MemMap.h"
/* ------------------------------------------------------------------------ */
/**
 * @ingroup PDUR_H
 *
 * State information \n
 */
extern PduR_StateType PduR_State;

/* ------------------------------------------------------------------------ */
/* End the set of variable definitions */

 /**
  * @ingroup PDUR_H
  *
  * Section code stop
  */
#define PDUR_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "PduR_MemMap.h"

 /* ------------------------------------------------------------------------ */
 /* Error detection functions */


 #if defined(PDUR_DEV_ERROR_DETECT) && (PDUR_DEV_ERROR_DETECT != STD_OFF)    
  /**
   * @ingroup PDUR_H
   *
   * If any developemnt error is reported, and DevErrorDetect switch is set to off, the below macro points to NULL_PTR
   */
  #define PDUR_DET_API(name)                      (NULL_PTR)
 #else
  /**
   * @ingroup PDUR_H
   *
   *  If any developemnt error is reported, and DevErrorDetect switch is set to off, the below macro points to actual API
   */
  #define PDUR_DET_API(name)                      (name)
 #endif /* PDUR_DEV_ERROR_DETECT */

 /**
 * @ingroup PDUR_PRV_H
 *
 *  This function is to Initialize a multicast state variable to default value. \n
 *
 *  @param  In:      state - This parameter is used to identify the state of multicast state variable. \n
 *
 *  @return None \n
 */
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"
 extern void PduR_iInit_multicast(PduR_MS_LoTpTxToUp * state);
#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
 /*Extern declarations of internal RPG functions */
#ifdef PDUR_RPG_FOR_RXTP_SUPPORT
 /**
  * @ingroup PDUR_RPG_H
  *  **************************************************************************************************
  * PduR_RGP_Tp_StartOfReception - This function will be called by the transport protocol module at the start of receiving an I-PDU.
  *                                                The I-PDU might be fragmented into multiple N-PDUs (FF with one or more following CFs) or might consist of a single N-PDU (SF). The service shall provide the currently available maximum buffer size when invoked with TpSdulength equal to 0.
  *
  *
  * \param(in)       id                - Identification of the I-PDU.
  *                    info              - Pointer to a PduInfoType structure containing the payload data
  *                                           (without protocol information) and payload length of the first
  *                                           frame or single frame of a transport protocol I-PDU reception.
  *                                           Depending on the global parameter MetaDataLength, additional bytes
  *                                           containing MetaData (e.g. the CAN ID) are appended after the payload data.
  *                    TpSduLength          - Total length of the PDU to be received.
  *
  * \param(out)        bufferSizePtr      - Available receive buffer in the receiving module.
  *                                        This parameter will be used to compute the Block Size (BS)
  *                                        in the transport protocol module.
  *
  * \retval          BUFREQ_OK           -  Connection has been accepted. bufferSizePtr indicates the available receive buffer;
  *                                         reception is continued. If no buffer of the requested size is available,
  *                                         a receive buffer size of 0 shall be indicated by bufferSizePtr.
  *                    BUFREQ_E_NOT_OK   -  Connection has been rejected; reception is aborted. bufferSizePtr remains unchanged.
  *                    BUFREQ_E_OVFL     -  No buffer of the required length can be provided; reception is aborted.
  *                                         bufferSizePtr remains unchanged.
  *
  * \seealso
  * \usedresources
  **************************************************************************************************
  */
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"
 extern BufReq_ReturnType PduR_RPG_Tp_StartOfReception_Func ( PduIdType rpgId,
                                                                         const PduInfoType * info,
                                                                         PduLengthType TpSduLength,
                                                                         PduLengthType * bufferSizePtr);

 /**
  * @ingroup PDUR_RPG_H
  *  **************************************************************************************************
  * PduR_RPG_Tp_CopyRxData - This function is called when transport protocol module have data to copy to
  *                           the receiving module. Several calls may be made during one transportation of an I-PDU.
  *
  * \param(in)       PduIdType    gwId             - Identification of the received I-PDU.
  *                  pduInfoPtr   Info             - Pointer to the buffer (SduDataPtr), metadata (MetaDataPtr) and its
  *                                                      length (SduLength) containing the data
  *                                                      to be copied by PDU Router module in case of gateway or upper
  *                                                      layer module in case of reception.
  * \param(out)      PduLengthType* bufferSizePtr  - Available receive buffer after data has been copied.
  *
  * \retval          BUFREQ_OK: Buffer request accomplished successful.
  *                    BUFREQ_E_NOT_OK: Buffer request not successful. Buffer cannot be accessed.
  *                    BUFREQ_E_OVFL: No Buffer of the required length can be provided.
  * \seealso
  * \usedresources
  **************************************************************************************************
  */

 extern BufReq_ReturnType PduR_RPG_Tp_CopyRxData_Func ( PduIdType rpgId,
                                                                 const PduInfoType * info,
                                                                 PduLengthType * bufferSizePtr);

 /**
  * @ingroup PDUR_RPG_H
  **************************************************************************************************
  * PduR_RPG_Tp_RxIndication - Called by the transport protocol module after an I-PDU has been received successfully
  *                              or when an error occurred. It is also used to confirm cancellation of an I-PDU.
  *
  * \param(in)       PduIdType    Id                 - Identification of the received I-PDU.
  *                  Std_ReturnType result        - Result of the reception.
  *
  * \param(out)      None
  *
  * \retval            None
  * \seealso
  * \usedresources
  **************************************************************************************************

  */

 extern void PduR_RPG_Tp_RxIndication_Func ( PduIdType rpgId,
                                                        Std_ReturnType result);
#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#endif /*PDUR_RPG_FOR_RXTP_SUPPORT*/

 /* ------------------------------------------------------------------------ */
 /* PduR_Init */

  /**
   * @ingroup PDUR_H
   * This API initializes the PDU Router.\n
   *
   * @param in         ConfigPtr :    Pointer to post build configuration.\n
   *
   * @return None \n
   */
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"
 extern void PduR_iInit( const PduR_ConfigType * ConfigPtr );
#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

 /**
  * @ingroup PDUR_H
  * This API initializes the PDU Router.\n
  *
  * @param in       ConfigPtr :   Pointer to post build configuration.\n
  *
  * @return None \n
  */
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"
 extern void PduR_dInit( const PduR_ConfigType * ConfigPtr );

 /* ------------------------------------------------------------------------ */
 /**
  * @ingroup PDUR_H
  *
  * PduR_GetConfigurationId
  * */
 extern PduR_PBConfigIdType PduR_dGetConfigurationId( void );
#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"
 extern PduR_PBConfigIdType PduR_iGetConfigurationId( void );

 /* ------------------------------------------------------------------------ */
 /**
  * @ingroup PDUR_H
  *      Null function to handle unwanted PduR_xxxIfTriggerTransmit
  *
  * @param in           Id      : ID of N-PDU that shall be received
  * @param in           data    : Data pointer for transmit buffer
  *
  * @return None \n
  */
 extern Std_ReturnType PduR_NF_TriggerTransmit_Func( PduIdType id, PduInfoType * data);

 /**
  * @ingroup PDUR_H
  *    This functions to be invoked when an invalid ID is encountered.\n
  *
  *
  * @param in        id      :   ID of L-PDU that is requested to be transmitted.\n
  *                  data    :   Pointer to place inside the transmit buffer of the L-PDU where data shall be copied to.\n
  */
 extern Std_ReturnType PduR_invId_IfTriggerTransmit( PduIdType id,  \
                                                              PduInfoType * data );
 /**
  * @ingroup PDUR_H
  *     This functions to be invoked when an invalid ID is encountered.\n
  *
  * @param in           id: ID of DCM I-PDU to be cancelled the transmission.\n
  */
 extern Std_ReturnType PduR_invId_UpCancelTransmit( PduIdType id);

 /**
 * @ingroup PDUR_H
 *     This functions to be invoked when an invalid ID is encountered.\n
 *
 * @param in           id: ID of UpTp I-PDU to be cancelled the reception.\n
 */
 extern Std_ReturnType PduR_invId_UpCancelReceive( PduIdType id);
#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
 /**
  * @ingroup PDUR_H
  *   The following extern declarations irrespective of whether the tables exist in the PduR_Cfg.c file.
  */

 /* ------------------------------------------------------------------------ */
 /* Begin section for extern constant */

#define PDUR_START_SEC_CONFIG_DATA_UNSPECIFIED

#include "PduR_MemMap.h"

 /**
  * @ingroup PDUR_H
  * extern declaration of PduR_upIfRxIndicationTable[]
  * */
 extern const PduR_upIfRxIndicationFuncType PduR_upIfRxIndicationTable[];
 /**
  * @ingroup PDUR_H
  * extern declaration of PduR_upIfTxConfirmationTable[]
  * */
 extern const PduR_upIfTxConfirmationFuncType PduR_upIfTxConfirmationTable[];
 /**
  * @ingroup PDUR_H
  * extern declaration of PduR_upTpCopyRxDataTable[]
  * */
 extern const PduR_upTpCopyRxDataFuncType PduR_upTpCopyRxDataTable[];
 /**
  * @ingroup PDUR_H
  * extern declaration of PduR_upTpStartOfReceptionTable[]
  * */
 extern const PduR_upTpStartOfReceptionFuncType PduR_upTpStartOfReceptionTable[];
 /**
  * @ingroup PDUR_H
  * extern declaration of PduR_upTpRxIndicationTable[]
  * */
 extern const PduR_upTpRxIndicationFuncType PduR_upTpRxIndicationTable[];
 /**
  * @ingroup PDUR_H
  * extern declaration of PduR_upTpCopyTxDataTable[]
  * */
 extern const PduR_upTpCopyTxDataFuncType PduR_upTpCopyTxDataTable[];
 /**
  * @ingroup PDUR_H
  * extern declaration of PduR_upTpTxConfirmationTable[]
  * */
 extern const PduR_upTpTxConfirmationFuncType PduR_upTpTxConfirmationTable[];

 /**
  * @ingroup PDUR_H
  * extern declaration of PduR_upIfTriggerTxTable[]
  * */
 extern const PduR_upIfTriggerTxFuncType PduR_upIfTriggerTxTable[];

 /**
  * @ingroup PDUR_H
  * extern declaration of PduR_loTransmitTable[]
  * */
 extern const PduR_loTransmitFuncType PduR_loTransmitTable[];

  /**
  * @ingroup PDUR_H
  * extern declaration of PduR_loCancelTransmitTable[]
  * */
 extern const PduR_loCancelTransmitFuncType PduR_loCancelTransmitTable[];
 /**
 * @ingroup PDUR_H
 * extern declaration of PduR_loCancelRxTable[]
 * */
 extern const PduR_loCancelReceiveFuncType PduR_loCancelRxTable[];

 /* ------------------------------------------------------------------------ */
 /* End section for extern constant */

#define PDUR_STOP_SEC_CONFIG_DATA_UNSPECIFIED

#include "PduR_MemMap.h"

#if (PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)
/* Internal APIs & Interfaces required for COM module */
#define PDUR_START_SEC_CODE

#include "PduR_MemMap.h"
extern void PduRAppl_IncompatibleGenerator(void);
#define PDUR_STOP_SEC_CODE

#include "PduR_MemMap.h"
#endif

/* Flags */

/* These flags share a byte with the count of the number of lower layers which we are gatewaying
 * to, therefore they should have large values. */
/**
 * @ingroup PDUR_PRV_H
 *
 * This flag share a byte with the count of the number of lower layers which we are gatewaying
 * to, therefore they should have large values. This is used for the need of Tx \n
 */
#define PDUR_GW_TP_NEED_TX    ((uint8)0x40)
/**
 * @ingroup PDUR_PRV_H
 *
 * This flag share a byte with the count of the number of lower layers which we are gatewaying
 * to, therefore they should have large values. This is used when transmit is called \n
 */
#define PDUR_GW_TP_TX_CALLED  ((uint8)0x20)
/**
 * @ingroup PDUR_PRV_H
 *
 * This flag share a byte with the count of the number of lower layers which we are gatewaying
 * to, therefore they should have large values. This is used when transmit is failed \n
 */
#define PDUR_GW_TP_TX_FAILED  ((uint8)0x80)

#endif /* PDUR_PRIV_H */

