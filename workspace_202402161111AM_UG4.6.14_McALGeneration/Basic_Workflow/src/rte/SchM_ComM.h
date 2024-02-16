/** @file     SchM_ComM.h
  *
  * @brief    Basic Software Scheduler Module Interlink header file
  *
  * @note     AUTOMATICALLY GENERATED FILE! DO NOT EDIT!
  *
  * @note     Generated by ETAS RTA-RTE for Windows
  * @note     Version 12.2.0
  *
  * @date     Thu Feb 15 15:55:09 2024
  */

#ifndef SchM_ComM
#define SchM_ComM

#include "SchM_ComM_Type.h"
#include "Rte_Intl.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(RTE_RUNNABLEAPI_CE_Init)
   /* Do nothing for this and all elif clauses below (rte_sws_2751) */
#elif defined(RTE_RUNNABLEAPI_SE_MainFunction_Can_Network_Channel_Can_Network)
#else
   /* None of above defined so define value including all APIs (rte_sws_2751) */
   #define RTE_PRV_ALL_API
#endif

/* API Mapping Macros */
#ifndef RTE_CORE

#endif /* RTE_CORE */

/*******************************************************
 ***
 *** Schedulable Entity Prototypes
 ***
 *******************************************************/

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"
FUNC(void, COMM_CODE) ComM_MainFunction_Can_Network_Channel_Can_Network(void);
#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif /* !SchM_ComM */