/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef FEE_PRV_CFG_H
#define FEE_PRV_CFG_H

/* ******************************************************************************************************************
   *************************************************** Includes *****************************************************
   ****************************************************************************************************************** */

#include "Std_Types.h"
#include "Fee_Cfg.h"            // To get the definitions of Fls hook function
#include "Fee_Rb_Prj.h"

/* ******************************************************************************************************************
   **************************************** Internal published macros of Fee ****************************************
   ****************************************************************************************************************** */

#define FEE_PRV_CFG_VENDOR_ID                                (6u)                 /* Supplier of RTA-BSW */
#define FEE_PRV_CFG_MODULE_ID                                (21u)                /* Module ID of Fee */

/* Fee Software Version */
#define FEE_PRV_CFG_SW_MAJOR_VERSION                         (22u)
#define FEE_PRV_CFG_SW_MINOR_VERSION                         (0u)
#define FEE_PRV_CFG_SW_PATCH_VERSION                         (1u)

/* Fee compatible Autosar Version */
#define FEE_PRV_CFG_AR_RELEASE_MAJOR_VERSION                 (4u)
#define FEE_PRV_CFG_AR_RELEASE_MINOR_VERSION                 (5u)
#define FEE_PRV_CFG_AR_RELEASE_REVISION_VERSION              (0u)

/* ******************************************************************************************************************
   **************************************** Macros for Fee code optimization ****************************************
   ****************************************************************************************************************** */

/* Pre-processor switch to enable or disable development error detection */
#define FEE_PRV_CFG_DEV_ERROR_DETECT                         (STD_OFF)

/* Pre-processor switch to enable or disable reporting of run time error to DET */
#define FEE_PRV_CFG_ENABLE_RUNTIME_ERROR_REPORTING           (TRUE)

/* Compiler switch to enable/disable the 'SetMode' functionality of the FEE module */
#define FEE_PRV_CFG_SET_MODE_SUPPORTED                       (STD_OFF)

/* Pre-processor switch to enable or disable the API to read out the modules version information */
#define FEE_PRV_CFG_VERSION_INFO_API                         (STD_OFF)

/* Pre-processor switch to enable or disable the API to maintain the redundant blocks */
#define FEE_PRV_CFG_RB_MAINTAIN                              (FALSE)

/* Pre-processor switch to enable or disable of variable length for Fee_Rb_Idx based API */
#define FEE_PRV_CFG_RB_VAR_LEN_READ_WRITE_API                (FALSE)

/* Pre-processor switch to enable or disable of migration result for Fee_Rb_Idx based API */
#define FEE_PRV_CFG_RB_GET_FIRST_READ_DATA_MIGRATION_API     (FALSE)

/* Pre-processor switch to enable or disable forced sector reorganisation (for one sector) */
#define FEE_PRV_CFG_RB_TRIGGER_REORG                         (FALSE)

/* Pre-processor switch to enable or disable stop mode */
#define FEE_PRV_CFG_RB_ENTER_STOP_MODE                       (FALSE)

/* Pre-processor switch to enable or disable free space API */
#define FEE_PRV_CFG_RB_GET_NR_FREE_BYTES                     (FALSE)

/* Pre-processor switch to enable or disable number free bytes and FAT entries API */
#define FEE_PRV_CFG_RB_GET_NR_FREE_BYTES_AND_FAT_ENTRIES     (FALSE)

/* Pre-processor switch to enable or disable chunk-wise job processing APIs */
#define FEE_PRV_CFG_RB_CHUNK_JOBS                            (FALSE)

/* Pre-processor switch to enable or disable system job APIs */
#define FEE_PRV_CFG_RB_SYSTEM_JOBS                           (FALSE)

/* macro with number of available page buffers */
#define FEE_PRV_CFG_RB_NR_PAGE_BUFFER                        (1u)

/* Pre-processor switch to enable or disable Set and Get Job Mode APIs */
#define FEE_PRV_CFG_RB_SET_AND_GET_JOB_MODE                  (FALSE)

/* Pre-processor switch to enable or disable sector erase counter APIs */
#define FEE_PRV_CFG_RB_GET_NR_SECTOR_ERASES                  (FALSE)

/* Pre-processor switch to enable or disable stress measurement and driving cycle prediction */
#define FEE_PRV_CFG_RB_STRESS_MEASMT                         (FALSE)

/* Pre-processor switch to enable or disable Detailed Block Info API */
#define FEE_PRV_CFG_RB_DETAILED_BLK_INFO_API                 (FALSE)

/* Pre-processor switch to enable or disable get sector state API */
#define FEE_PRV_CFG_RB_GET_SECT_STATE_API                    (FALSE)

/* Pre-processor switch to enter or exit high speed mode APIs */
#define FEE_PRV_CFG_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API    (FALSE)

/* Pre-processor switch to support cancel feature  
 * Note: The cancel feature must also be active when the chunk-wise job processing APIs are enabled */
#define FEE_PRV_CFG_RB_CANCEL                                (FALSE)

/* Following macros are needed when multi instance with Fee1.0 is used */

/* Debug feature flags */
#define FEE_PRV_CFG_DBG                                      (FALSE)
#define FEE_PRV_CFG_DBG_TIME                                 (FALSE)
#define FEE_PRV_CFG_DBG_TIME_HISTOGRAM_BASKETS               (1u)
#define FEE_PRV_CFG_DBG_ERASECOUNTERS                        (FALSE)
#define FEE_PRV_CFG_DBG_ERASECOUNTERS_HISTOGRAM_BASKETS      (0u)
#define FEE_PRV_CFG_DBG_DUMP                                 (FALSE)
#define FEE_PRV_CFG_DBG_BLOCK                                (FALSE)
#define FEE_PRV_CFG_DBG_BLOCK_SIZE                           (0u)
#define FEE_PRV_CFG_DBG_GRAY_BOX_TEST                        (FALSE)

/* Persistent data macros */
#define FEE_PRV_CFG_RB_PERSIST_DATA                          (FALSE)

/* ******************************************************************************************************************
   ************************************************* Macros for Fee *************************************************
   ****************************************************************************************************************** */

#define FEE_PRV_CFG_WL_ERROR_HNDLG_ENABLED                   (FALSE)
#define FEE_PRV_CFG_WL_ERROR_HNDLG_WLSIZE                    (0u)

#define FEE_PRV_CFG_MULTIINSTANCE_ENABLED                    (FALSE)
#define FEE_PRV_CFG_COMMON_ENABLED                           (TRUE)
#define RBA_FEEFS1X_PRV_CFG_ENABLED                          (TRUE)

/* ******************************************************************************************************************
   ************************************************ Typedefs for Fee ************************************************
   ****************************************************************************************************************** */

typedef union
{
    uint32  u32[256u];    /* This union member shall NEVER be used - it only forces 32bit alignment */
    uint8   u8[1024u];    /* uint8 indexing of the buffer */
} Fee_Prv_Cfg_MediumBuffer_tun;

typedef union
{
    uint32  u32[1u];    /* This union member shall NEVER be used - it only forces 32bit alignment */
    uint8   u8[4u];    /* uint8 indexing of the buffer */
} Fee_Prv_Cfg_MediumBuffer2_tun;

typedef union
{
    uint64  u64[1u];    /* This union member shall NEVER be used - it only forces 64bit alignment */
    uint8   u8[8u];    /* uint8 indexing of the buffer */
} Fee_Prv_Cfg_PersDataBuffer_tun;

#endif /* FEE_PRV_CFG_H */
