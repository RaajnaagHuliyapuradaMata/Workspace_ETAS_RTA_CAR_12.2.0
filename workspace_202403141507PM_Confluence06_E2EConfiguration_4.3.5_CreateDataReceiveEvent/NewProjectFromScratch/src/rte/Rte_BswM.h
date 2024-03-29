/** @file     Rte_BswM.h
  *
  * @brief    Application header file
  *
  * @note     AUTOMATICALLY GENERATED FILE! DO NOT EDIT!
  *
  * @note     Generated by ETAS RTA-RTE for Windows
  * @note     Version 12.2.0
  */

#ifndef RTE_BSWM_H
#define RTE_BSWM_H

#ifndef RTE_CORE
#ifdef RTE_APPLICATION_HEADER_FILE
#error Multiple application header files included.
#endif /* RTE_APPLICATION_HEADER_FILE */
#define RTE_APPLICATION_HEADER_FILE
#endif /* RTE_CORE */

/*******************************************************
 ***
 *** Includes
 ***
 *******************************************************/

#include "Rte.h"
#include "Rte_Intl.h"
#include "Rte_BswM_Type.h"

#include "Rte_DataHandleType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(RTE_RUNNABLEAPI_RE_BswMSwcGenericRequest)
   /* Do nothing for this and all elif clauses below (rte_sws_2751) */
#elif defined(RTE_RUNNABLEAPI_RE_BswM_Cfg_DfrdSwcNotification_BswM_MRP_SwcModeRequest_SHUT_DOWN)
#else
   /* None of above defined so define value including all APIs (rte_sws_2751) */
   #define RTE_PRV_ALL_API
#endif

/*******************************************************
 ***
 *** Constants
 ***
 *******************************************************/


/*******************************************************
 ***
 *** Public Types
 ***
 *******************************************************/

/* BEGIN: SWC types (core visible) */
struct Rte_CDS_BswM {
   uint8 Rte_Dummy;
};
typedef struct Rte_CDS_BswM Rte_CDS_BswM;
/* END: SWC types (core visible) */

/*******************************************************
 ***
 *** Private Types
 ***
 *******************************************************/

/* BEGIN: SWC types (private) */
/* END: SWC types (private) */

/*******************************************************
 ***
 *** Instance Declarations
 ***
 *******************************************************/

#define RTE_START_SEC_CONST_UNSPECIFIED
#include "Rte_MemMap.h"
extern CONST(struct Rte_CDS_BswM, RTE_CONST) Rte_Inst_BswM;
#define RTE_STOP_SEC_CONST_UNSPECIFIED
#include "Rte_MemMap.h"


#ifndef RTE_CORE
typedef CONSTP2CONST(struct Rte_CDS_BswM, AUTOMATIC, RTE_CONST) Rte_Instance;
#endif /* RTE_CORE */

/*******************************************************
 ***
 *** API Mapping macros
 ***
 *******************************************************/

/* API Mapping Macros */
#ifndef RTE_CORE
#if defined(RTE_PRV_ALL_API) || defined(RTE_RUNNABLEAPI_RE_BswM_Cfg_DfrdSwcNotification_BswM_MRP_SwcModeRequest_SHUT_DOWN)
/* Inline read optimization; Rte_Mode_RP_BswM_MRP_SwcModeRequest_MDGP_MSI_ShutDown to direct read */
#define Rte_Mode_RP_BswM_MRP_SwcModeRequest_MDGP_MSI_ShutDown()  ( 1 )
#endif

#endif /* RTE_CORE */

/*******************************************************
 ***
 *** RE Prototypes
 ***
 *******************************************************/

#define BswM_START_SEC_CODE
#include "BswM_MemMap.h"
FUNC(void, BswM_CODE) BswM_Cfg_DfrdSwcNotification_BswM_MRP_SwcModeRequest_SHUT_DOWN(void);
FUNC(void, BswM_CODE) BswM_RequestMode(VAR(BswM_UserType, AUTOMATIC) Requesting_user,
                                       VAR(BswM_ModeType, AUTOMATIC) Requested_mode);
#define BswM_STOP_SEC_CODE
#include "BswM_MemMap.h"

/* Initial values for data element prototypes */

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif /* !RTE_BSWM_H */
