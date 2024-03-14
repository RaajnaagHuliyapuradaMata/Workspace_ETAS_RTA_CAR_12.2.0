/** @file     Rte_NvM_Type.h
  *
  * @brief    Application types header file
  *
  * @note     AUTOMATICALLY GENERATED FILE! DO NOT EDIT!
  *
  * @note     Generated by ETAS RTA-RTE for Windows
  * @note     Version 12.2.0
  */

#ifndef RTE_NvM_TYPE_H
#define RTE_NvM_TYPE_H

#include "Rte_Type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* BEGIN: SWC specific types */
/* END: SWC specific types */

/* BEGIN: SWC specific definitions */
#if !defined(RTE_CORE)

/* Box: /AUTOSAR_NvM/ImplementationDataTypes/NvM_BlockIdType begin */
/* Box: "NvMConf_NvMBlockDescriptor_NvM_MultiBlock" begin */
#ifndef NvMConf_NvMBlockDescriptor_NvM_MultiBlock
#define NvMConf_NvMBlockDescriptor_NvM_MultiBlock (0U)
#endif /* !NvMConf_NvMBlockDescriptor_NvM_MultiBlock */
/* Box: "NvMConf_NvMBlockDescriptor_NvM_MultiBlock" end */
/* Box: "NvMConf_NvMBlockDescriptor_NvM_ConfigId" begin */
#ifndef NvMConf_NvMBlockDescriptor_NvM_ConfigId
#define NvMConf_NvMBlockDescriptor_NvM_ConfigId (1U)
#endif /* !NvMConf_NvMBlockDescriptor_NvM_ConfigId */
/* Box: "NvMConf_NvMBlockDescriptor_NvM_ConfigId" end */
/* Box: "NvMConf_NvMBlockDescriptor_ECUM_CFG_NVM_BLOCK" begin */
#ifndef NvMConf_NvMBlockDescriptor_ECUM_CFG_NVM_BLOCK
#define NvMConf_NvMBlockDescriptor_ECUM_CFG_NVM_BLOCK (2U)
#endif /* !NvMConf_NvMBlockDescriptor_ECUM_CFG_NVM_BLOCK */
/* Box: "NvMConf_NvMBlockDescriptor_ECUM_CFG_NVM_BLOCK" end */
/* Box: "NvMConf_NvMBlockDescriptor_NvM_NativeBlock_2" begin */
#ifndef NvMConf_NvMBlockDescriptor_NvM_NativeBlock_2
#define NvMConf_NvMBlockDescriptor_NvM_NativeBlock_2 (3U)
#endif /* !NvMConf_NvMBlockDescriptor_NvM_NativeBlock_2 */
/* Box: "NvMConf_NvMBlockDescriptor_NvM_NativeBlock_2" end */
/* Box: "NvMConf_NvMBlockDescriptor_NvM_NativeBlock_3" begin */
#ifndef NvMConf_NvMBlockDescriptor_NvM_NativeBlock_3
#define NvMConf_NvMBlockDescriptor_NvM_NativeBlock_3 (4U)
#endif /* !NvMConf_NvMBlockDescriptor_NvM_NativeBlock_3 */
/* Box: "NvMConf_NvMBlockDescriptor_NvM_NativeBlock_3" end */
/* Box: /AUTOSAR_NvM/ImplementationDataTypes/NvM_BlockIdType end */

/* Box: /AUTOSAR_NvM/ImplementationDataTypes/NvM_RequestResultType begin */
/* Box: "NVM_REQ_OK" begin */
#ifndef NVM_REQ_OK
#define NVM_REQ_OK (0U)
#endif /* !NVM_REQ_OK */
/* Box: "NVM_REQ_OK" end */
/* Box: "NVM_REQ_NOT_OK" begin */
#ifndef NVM_REQ_NOT_OK
#define NVM_REQ_NOT_OK (1U)
#endif /* !NVM_REQ_NOT_OK */
/* Box: "NVM_REQ_NOT_OK" end */
/* Box: "NVM_REQ_PENDING" begin */
#ifndef NVM_REQ_PENDING
#define NVM_REQ_PENDING (2U)
#endif /* !NVM_REQ_PENDING */
/* Box: "NVM_REQ_PENDING" end */
/* Box: "NVM_REQ_INTEGRITY_FAILED" begin */
#ifndef NVM_REQ_INTEGRITY_FAILED
#define NVM_REQ_INTEGRITY_FAILED (3U)
#endif /* !NVM_REQ_INTEGRITY_FAILED */
/* Box: "NVM_REQ_INTEGRITY_FAILED" end */
/* Box: "NVM_REQ_BLOCK_SKIPPED" begin */
#ifndef NVM_REQ_BLOCK_SKIPPED
#define NVM_REQ_BLOCK_SKIPPED (4U)
#endif /* !NVM_REQ_BLOCK_SKIPPED */
/* Box: "NVM_REQ_BLOCK_SKIPPED" end */
/* Box: "NVM_REQ_NV_INVALIDATED" begin */
#ifndef NVM_REQ_NV_INVALIDATED
#define NVM_REQ_NV_INVALIDATED (5U)
#endif /* !NVM_REQ_NV_INVALIDATED */
/* Box: "NVM_REQ_NV_INVALIDATED" end */
/* Box: "NVM_REQ_CANCELED" begin */
#ifndef NVM_REQ_CANCELED
#define NVM_REQ_CANCELED (6U)
#endif /* !NVM_REQ_CANCELED */
/* Box: "NVM_REQ_CANCELED" end */
/* Box: "NVM_REQ_REDUNDANCY_FAILED" begin */
#ifndef NVM_REQ_REDUNDANCY_FAILED
#define NVM_REQ_REDUNDANCY_FAILED (7U)
#endif /* !NVM_REQ_REDUNDANCY_FAILED */
/* Box: "NVM_REQ_REDUNDANCY_FAILED" end */
/* Box: "NVM_REQ_RESTORED_FROM_ROM" begin */
#ifndef NVM_REQ_RESTORED_FROM_ROM
#define NVM_REQ_RESTORED_FROM_ROM (8U)
#endif /* !NVM_REQ_RESTORED_FROM_ROM */
/* Box: "NVM_REQ_RESTORED_FROM_ROM" end */
/* Box: /AUTOSAR_NvM/ImplementationDataTypes/NvM_RequestResultType end */

/* Box: /AUTOSAR_Platform/ImplementationDataTypes/boolean begin */
/* Box: "FALSE" begin */
#ifndef FALSE
#define FALSE (0U)
#endif /* !FALSE */
/* Box: "FALSE" end */
/* Box: "TRUE" begin */
#ifndef TRUE
#define TRUE (1U)
#endif /* !TRUE */
/* Box: "TRUE" end */
/* Box: /AUTOSAR_Platform/ImplementationDataTypes/boolean end */
#endif /* !defined(RTE_CORE) */
/* END: SWC specific definitions */

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif /* !RTE_NvM_TYPE_H */