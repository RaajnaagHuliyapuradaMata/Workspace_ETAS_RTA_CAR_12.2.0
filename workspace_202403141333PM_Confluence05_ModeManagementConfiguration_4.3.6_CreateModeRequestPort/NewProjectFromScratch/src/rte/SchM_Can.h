/** @file     SchM_Can.h
  *
  * @brief    Basic Software Scheduler Module Interlink header file
  *
  * @note     AUTOMATICALLY GENERATED FILE! DO NOT EDIT!
  *
  * @note     Generated by ETAS RTA-RTE for Windows
  * @note     Version 12.2.0
  */

#ifndef SchM_Can
#define SchM_Can

#include "SchM_Can_Type.h"
#include "Rte_Intl.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_BusOff)
   /* Do nothing for this and all elif clauses below (rte_sws_2751) */
#elif defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Mode)
#elif defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Read)
#elif defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Write)
#else
   /* None of above defined so define value including all APIs (rte_sws_2751) */
   #define RTE_PRV_ALL_API
#endif

/* API Mapping Macros */
#ifndef RTE_CORE
#if defined(RTE_PRV_ALL_API) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_BusOff) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Mode) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Read) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Write)
/* Inline function call optimization; SchM_Enter_Can_CAN_EXCLUSIVE_AREA_00 to empty definition */
#define SchM_Enter_Can_CAN_EXCLUSIVE_AREA_00() /* Null Body. Either the EA Strategy is 'task blocking', mappings of REs to tasks mean that no lock is required, or RteExclusiveAreaImplMechanism is NONE. Body is required to support indirect API. */
#endif
#if defined(RTE_PRV_ALL_API) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_BusOff) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Mode) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Read) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Write)
/* Inline function call optimization; SchM_Enter_Can_CAN_EXCLUSIVE_AREA_01 to empty definition */
#define SchM_Enter_Can_CAN_EXCLUSIVE_AREA_01() /* Null Body. Either the EA Strategy is 'task blocking', mappings of REs to tasks mean that no lock is required, or RteExclusiveAreaImplMechanism is NONE. Body is required to support indirect API. */
#endif
#if defined(RTE_PRV_ALL_API) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_BusOff) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Mode) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Read) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Write)
/* Inline function call optimization; SchM_Enter_Can_CAN_EXCLUSIVE_AREA_02 to empty definition */
#define SchM_Enter_Can_CAN_EXCLUSIVE_AREA_02() /* Null Body. Either the EA Strategy is 'task blocking', mappings of REs to tasks mean that no lock is required, or RteExclusiveAreaImplMechanism is NONE. Body is required to support indirect API. */
#endif
#if defined(RTE_PRV_ALL_API) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_BusOff) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Mode) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Read) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Write)
/* Inline function call optimization; SchM_Exit_Can_CAN_EXCLUSIVE_AREA_00 to empty definition */
#define SchM_Exit_Can_CAN_EXCLUSIVE_AREA_00() /* Null Body. Either the EA Strategy is 'task blocking', mappings of REs to tasks mean that no lock is required, or RteExclusiveAreaImplMechanism is NONE. Body is required to support indirect API. */
#endif
#if defined(RTE_PRV_ALL_API) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_BusOff) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Mode) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Read) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Write)
/* Inline function call optimization; SchM_Exit_Can_CAN_EXCLUSIVE_AREA_01 to empty definition */
#define SchM_Exit_Can_CAN_EXCLUSIVE_AREA_01() /* Null Body. Either the EA Strategy is 'task blocking', mappings of REs to tasks mean that no lock is required, or RteExclusiveAreaImplMechanism is NONE. Body is required to support indirect API. */
#endif
#if defined(RTE_PRV_ALL_API) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_BusOff) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Mode) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Read) || defined(RTE_RUNNABLEAPI_BSWSE_MainFunction_Write)
/* Inline function call optimization; SchM_Exit_Can_CAN_EXCLUSIVE_AREA_02 to empty definition */
#define SchM_Exit_Can_CAN_EXCLUSIVE_AREA_02() /* Null Body. Either the EA Strategy is 'task blocking', mappings of REs to tasks mean that no lock is required, or RteExclusiveAreaImplMechanism is NONE. Body is required to support indirect API. */
#endif

#endif /* RTE_CORE */

/*******************************************************
 ***
 *** Schedulable Entity Prototypes
 ***
 *******************************************************/

#define CAN_START_SEC_CODE
#include "Can_MemMap.h"
FUNC(void, CAN_CODE) Can_MainFunction_BusOff(void);
#define CAN_STOP_SEC_CODE
#include "Can_MemMap.h"
#define CAN_START_SEC_CODE
#include "Can_MemMap.h"
FUNC(void, CAN_CODE) Can_MainFunction_Mode(void);
#define CAN_STOP_SEC_CODE
#include "Can_MemMap.h"
#define CAN_START_SEC_CODE
#include "Can_MemMap.h"
FUNC(void, CAN_CODE) Can_MainFunction_Read(void);
#define CAN_STOP_SEC_CODE
#include "Can_MemMap.h"
#define CAN_START_SEC_CODE
#include "Can_MemMap.h"
FUNC(void, CAN_CODE) Can_MainFunction_Write(void);
#define CAN_STOP_SEC_CODE
#include "Can_MemMap.h"

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif /* !SchM_Can */
