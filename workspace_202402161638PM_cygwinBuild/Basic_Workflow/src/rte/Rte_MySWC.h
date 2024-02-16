/** @file     Rte_MySWC.h
  *
  * @brief    Application header file
  *
  * @note     AUTOMATICALLY GENERATED FILE! DO NOT EDIT!
  *
  * @note     Generated by ETAS RTA-RTE for Windows
  * @note     Version 12.2.0
  *
  * @date     Thu Feb 15 15:55:09 2024
  */

#ifndef RTE_MYSWC_H
#define RTE_MYSWC_H

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
#include "Rte_MySWC_Type.h"

#include "Rte_DataHandleType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(RTE_RUNNABLEAPI_RE_MySWC)
   /* Do nothing for this and all elif clauses below (rte_sws_2751) */
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
struct Rte_PDS_MySWC_My_SR_Interface_P {
   Rte_WriteFP_MySWC_My_SR_Interface_DE_MyDataElement Write_DE_MyDataElement;
};
typedef struct Rte_PDS_MySWC_My_SR_Interface_P Rte_PDS_MySWC_My_SR_Interface_P;
typedef struct Rte_PDS_MySWC_My_SR_Interface_P Rte_PDS_MySWC_My_SR_Interface_PA[1];
struct Rte_PDS_MySWC_My_SR_Interface_R {
   uint8 Rte_Dummy;
};
typedef struct Rte_PDS_MySWC_My_SR_Interface_R Rte_PDS_MySWC_My_SR_Interface_R;
typedef struct Rte_PDS_MySWC_My_SR_Interface_R Rte_PDS_MySWC_My_SR_Interface_RA[1];
struct Rte_CDS_MySWC {
   /* Data handles */
   P2VAR(Rte_DE_uint8                    , AUTOMATIC, RTE_DATA) RE_MySWC_RPortPrototype_0_DE_MyDataElement;
   /* Port Data Structure Arrays */
   Rte_PDS_MySWC_My_SR_Interface_PA PPortPrototype_0;
   Rte_PDS_MySWC_My_SR_Interface_RA RPortPrototype_0;
};
typedef struct Rte_CDS_MySWC Rte_CDS_MySWC;
/* END: SWC types (core visible) */

/*******************************************************
 ***
 *** Private Types
 ***
 *******************************************************/

/* BEGIN: SWC types (private) */
#ifndef RTE_CORE
typedef P2CONST(struct Rte_PDS_MySWC_My_SR_Interface_P, AUTOMATIC, RTE_CONST) Rte_PortHandle_My_SR_Interface_P;
typedef P2CONST(struct Rte_PDS_MySWC_My_SR_Interface_R, AUTOMATIC, RTE_CONST) Rte_PortHandle_My_SR_Interface_R;
#endif /* RTE_CORE */

/* END: SWC types (private) */

/*******************************************************
 ***
 *** Instance Declarations
 ***
 *******************************************************/

#define RTE_START_SEC_CONST_UNSPECIFIED
#include "Rte_MemMap.h"
extern CONST(struct Rte_CDS_MySWC, RTE_CONST) Rte_Instance_CPT_MySWC_ApplicationECU;
#define RTE_STOP_SEC_CONST_UNSPECIFIED
#include "Rte_MemMap.h"


#ifndef RTE_CORE
typedef CONSTP2CONST(struct Rte_CDS_MySWC, AUTOMATIC, RTE_CONST) Rte_Instance;
#endif /* RTE_CORE */

/*******************************************************
 ***
 *** API Mapping macros
 ***
 *******************************************************/

/* API Mapping Macros */
#ifndef RTE_CORE

#define RTE_MYSWC_START_SEC_CODE
#include "Rte_MemMap.h"
FUNC(Std_ReturnType, RTE_CODE) Rte_Write_CPT_MySWC_ApplicationECU_PPortPrototype_0_DE_MyDataElement(VAR(uint8, AUTOMATIC) data);
#define RTE_MYSWC_STOP_SEC_CODE
#include "Rte_MemMap.h"
#if defined(RTE_PRV_ALL_API) || defined(RTE_RUNNABLEAPI_RE_MySWC)
/* Inline read optimization; Rte_IReadRef_RE_MySWC_RPortPrototype_0_DE_MyDataElement to implicit access macro */
#define Rte_IReadRef_RE_MySWC_RPortPrototype_0_DE_MyDataElement( self )  ( (P2CONST(uint8, AUTOMATIC, RTE_DATA)) &((self->RE_MySWC_RPortPrototype_0_DE_MyDataElement)->value) )
#endif
#if defined(RTE_PRV_ALL_API) || defined(RTE_RUNNABLEAPI_RE_MySWC)
/* Inline read optimization; Rte_IRead_RE_MySWC_RPortPrototype_0_DE_MyDataElement to implicit access macro */
#define Rte_IRead_RE_MySWC_RPortPrototype_0_DE_MyDataElement( self )  ( (CONST(uint8, RTE_DATA)) ((self->RE_MySWC_RPortPrototype_0_DE_MyDataElement)->value) )
#endif
#if defined(RTE_PRV_ALL_API) || defined(RTE_RUNNABLEAPI_RE_MySWC)
#define Rte_Write_PPortPrototype_0_DE_MyDataElement( self, data ) (Rte_Write_CPT_MySWC_ApplicationECU_PPortPrototype_0_DE_MyDataElement(data))
#endif

/* Rte_Port API */
#define Rte_Port_PPortPrototype_0(self) ((Rte_PortHandle_My_SR_Interface_P)&((self)->PPortPrototype_0[0]))
#define Rte_Port_RPortPrototype_0(self) ((Rte_PortHandle_My_SR_Interface_R)&((self)->RPortPrototype_0[0]))

/* Rte_NPorts API */
#define Rte_NPorts_My_SR_Interface_P(self) ((uint8)1)
#define Rte_NPorts_My_SR_Interface_R(self) ((uint8)1)

/* Rte_Ports API */
#define Rte_Ports_My_SR_Interface_P(self) ((Rte_PortHandle_My_SR_Interface_P)&((self)->PPortPrototype_0[0]))
#define Rte_Ports_My_SR_Interface_R(self) ((Rte_PortHandle_My_SR_Interface_R)&((self)->RPortPrototype_0[0]))

#endif /* RTE_CORE */

/*******************************************************
 ***
 *** RE Prototypes
 ***
 *******************************************************/

#define MySWC_START_SEC_CODE
#include "MySWC_MemMap.h"
FUNC(void, MySWC_CODE) RE_MySWC_func(CONST(Rte_SelfType_MySWC, AUTOMATIC) self);
#define MySWC_STOP_SEC_CODE
#include "MySWC_MemMap.h"

/* Initial values for data element prototypes */

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif /* !RTE_MYSWC_H */
