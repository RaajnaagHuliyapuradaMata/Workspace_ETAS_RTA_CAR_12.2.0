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
#ifndef FEE_H
#define FEE_H

#include "Std_Types.h"
#include "MemIf_Types.h"
#include "Fee_Rb_Types.h"   // All static macros + type defs included via this header file
#include "Fee_Cfg.h"        // To get the definitions of Fls hook function

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/
#define FEE_VENDOR_ID                                        (6u)                 /* Supplier of RTA-BSW */
#define FEE_MODULE_ID                                        (21u)                /* Module ID of Fee */
#define FEE_INSTANCE_ID                                      (0u)                 /* Instance ID */

/* Fee Software Version */
#define FEE_SW_MAJOR_VERSION                                 (22u)
#define FEE_SW_MINOR_VERSION                                 (0u)
#define FEE_SW_PATCH_VERSION                                 (1u)

/* Fee compatible Autosar Version */
#define FEE_AR_RELEASE_MAJOR_VERSION                         (4u)
#define FEE_AR_RELEASE_MINOR_VERSION                         (5u)
#define FEE_AR_RELEASE_REVISION_VERSION                      (0u)

/* Pre-processor switch to enable or disable development error detection */
#define FEE_DEV_ERROR_DETECT                                 (STD_OFF)

/* Compiler switch to enable/disable the 'SetMode' functionality of the FEE module */
#define FEE_SET_MODE_SUPPORTED                               (STD_OFF)

/* Pre-processor switch to enable or disable the API to read out the modules version information */
#define FEE_VERSION_INFO_API                                 (STD_OFF)

/* Pre-processor switch to enable or disable redunant block maintenance API */
#define FEE_RB_MAINTAIN                                      (FALSE)

/* Pre-processor switch to enable or disable of variable length APIs */
#define FEE_RB_VAR_LEN_READ_WRITE_API                        (FALSE)

/* Pre-processor switch to enable or disable of migration result APIs */
#define FEE_RB_FIRST_READ_DATA_MIGRATION_API                 (FALSE)

/* Pre-processor switch to enable or disable forced sector reorganisation (for one sector) */
#define FEE_RB_TRIGGER_REORG                                 (FALSE)

/* Pre-processor switch to enable or disable stop mode */
#define FEE_RB_ENTER_STOP_MODE                               (FALSE)

/* Pre-processor switch to enable or disable free space API */
#define FEE_RB_GET_NR_FREE_BYTES                             (FALSE)

/* Pre-processor switch to enable or disable sector erase counter APIs */
#define FEE_RB_GET_NR_SECTOR_ERASES                          (FALSE)

/* Pre-processor switch to enable or disable stress measurement and driving cycle prediction */
#define FEE_RB_STRESS_MEASMT                                 (FALSE)

/* Pre-processor switch to enable or disable number free bytes and FAT entries API */
#define FEE_RB_GET_NR_FREE_BYTES_AND_FAT_ENTRIES             (FALSE)

/* Pre-processor switch to  indicate if the block properties table is in ROM or RAM */
#define FEE_RB_USE_ROM_BLOCKTABLE                            (FALSE)

/* Pre-processor switch to enable or disable Set and Get Job Mode APIs */
#define FEE_RB_SET_AND_GET_JOB_MODE                          (FALSE)

/* Pre-processor switch to enable or disable Detailed Block Info API */
#define FEE_RB_DETAILED_BLK_INFO_API                         (FALSE)

/* Pre-processor switch to enable or disable get sector state API */
#define FEE_RB_GET_SECT_STATE_API                            (FALSE)

/* Pre-processor switch to enter or exit high speed mode API */
#define FEE_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API            (FALSE)

/* Pre-processor switch for NvM user to cancel ongoing jobs */
#define FEE_CANCEL                                           (FALSE)

/* Hook into synchronous Fls_MainFunction loop */
#define FEE_RB_SYNC_LOOP_FLS_MAIN_HOOK()                     (void)0

/* ******************************************************************************************************************
   ******************************************** FeePublishedInformation *********************************************
   ****************************************************************************************************************** */

/* FEE116_Conf: FeeVirtualPageSize {FEE_BLOCK_OVERHEAD}. Management overhead per logical block in bytes. */
#define FEE_VIRTUAL_PAGE_SIZE                                (8u)

/* FEE117_Conf: FeeBlockOverhead {FEE_BLOCK_OVERHEAD}. Management overhead per logical block in bytes. */
#define FEE_BLOCK_OVERHEAD                                   (14u)

/* FEE118_Conf: FeePageOverhead {FEE_PAGE_OVERHEAD}. Management overhead per page in bytes. */
#define FEE_PAGE_OVERHEAD                                    (0u)

/* ******************************************************************************************************************
   ******************************************* Generate FLASH configuration *****************************************
   ****************************************************************************************************************** */

/* If Relative Addressing Feature is activated, the below macros will have relative address values*/
#define FEE_CFG_EMULATION_START                              (0x00000000u)
#define FEE_CFG_EMULATION_END                                (0x000007FFu)
#define FEE_CFG_EMULATION_SIZE                               (0x00000800u)

/* Number of logical sectors */
#define FEE_NUM_FLASH_BANKS_AVAILABLE                        (2u)

/* ******************************************************************************************************************
   ******************************************* Defines for accessing blocks *****************************************
   ****************************************************************************************************************** */
# ifndef FEE_CFG_BLOCKDEFINES_H
#define FeeConf_FeeBlockConfiguration_NvM_NativeBlock_2                                              (0u)
#define FeeConf_FeeBlockConfiguration_NvM_NativeBlock_3                                              (1u)
#define FeeConf_FeeBlockConfiguration_ECUM_CFG_NVM_BLOCK                                             (2u)
# endif /* FEE_CFG_BLOCKDEFINES_H */

/* ******************************************************************************************************************
   ********************************************** Fs specific defines ***********************************************
   ****************************************************************************************************************** */

#define FEE_CFG_FEE_COMMON_ENABLED                           (TRUE)
#define FEE_CFG_MULTIINSTANCE_ENABLED                        (FALSE)
#define FEE_CFG_FEE1X_ENABLED                                (TRUE)
/* Threshold when reached soft reorganisation will start */
#define FEE_REQUIRED_FREE_SPACE_BEFORE_SOFT_SR               (928uL)
/* Threshold when reached hard reorganisation will start */
#define FEE_REQUIRED_FREE_SPACE_BEFORE_HARD_SR               (928uL)

/* ******************************************************************************************************************
   ******************* External declarations of functions that are supported (enabled) for this instance ************
   ****************************************************************************************************************** */

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

// mandatory AUTOSAR APIs that are always supported
extern void                 Fee_Init(Fee_ConfigType const * CfgPtr_pst);
extern Std_ReturnType       Fee_Read(uint16 nrBlks_u16, uint16 nrBlkOffs_u16, uint8 * DataBuf_pu8, uint16 nrLen_u16);
extern Std_ReturnType       Fee_Write(uint16 nrBlks_u16, uint8 * DataBuf_pu8);
extern Std_ReturnType       Fee_InvalidateBlock(uint16 nrBlks_u16);
extern Std_ReturnType       Fee_EraseImmediateBlock(uint16 nrBlks_u16);
#ifndef FEE_RB_HIDE_MAINFUNC_RTEAPI
/* This declaration is used only if RTE generator is NOT used */
extern void                 Fee_MainFunction(void);
#endif /* FEE_RB_HIDE_MAINFUNC_RTEAPI */
extern MemIf_StatusType     Fee_GetStatus(void);
extern MemIf_JobResultType  Fee_GetJobResult(void);
extern void                 Fee_Cancel(void);

// optional AUTOSAR APIs that are supported when the feature is enabled
/* ### FeeSetModeSupported disabled - Fee_SetMode API not available ### */
/* ### FeeVersionInfoApi disabled - Fee_GetVersionInfo API not available ### */

// non autosar APIs that are always supported
#ifndef FEE_RB_HIDE_ENDINIT_RTEAPI
/* This declaration is used only if RTE generator is NOT used */
extern void                         Fee_Rb_EndInit(void);
#endif /* FEE_RB_HIDE_ENDINIT_RTEAPI */
extern MemIf_JobResultType          Fee_Rb_GetAdapterJobResult(void);
extern MemIf_JobResultType          Fee_Rb_GetInternalJobResult(void);
extern Fee_Rb_JobResultType_ten     Fee_Rb_GetSystemJobResult(void);
extern uint32                       Fee_Rb_GetSectChngCnt(void);
extern void                         Fee_Rb_DisableBackgroundOperation(void);
extern void                         Fee_Rb_EnableBackgroundOperation(void);
extern boolean                      Fee_Rb_IsBlockDoubleStorageActiveByBlockNr(uint16 blkNr_u16);
extern Fee_Rb_WorkingStateType_ten  Fee_Rb_GetWorkingState(void);
extern void                         Fee_Rb_MainFunctionAndDependency(void);

// non autosar APIs that are supported when the feature is enabled
/* ### FeeRbMaintainRedundantBlocks disabled - Fee_Rb_BlockMaintenance API not available ### */
/* ### FeeRbVarBlockLength and FeeRbFirstReadDataMigration disabled for all blocks - Fee_Rb_VarLenRead API not available ### */
/* ### FeeRbVarBlockLength disabled for all blocks - Fee_Rb_VarLenWrite API not available ### */
/* ### FeeRbFirstReadDataMigration disabled for all blocks - Fee_Rb_GetMigrationResult API not available ### */
/* ### FeeRbDetailedBlkInfoApi disabled - Fee_Rb_GetDetailedBlkInfo API not available ### */
/* ### FeeRbTriggerReorgApi disabled - Fee_Rb_TriggerReorg, Fee_Rb_TriggerReorgFAT, Fee_Rb_TriggerReorgDAT API not available ### */
/* ### FeeSetModeSupported disabled - Fee_Rb_GetMode API not available ### */
/* ### No Fee-internal persistent data - Fee_Rb_WritePersistentData() API not available ### */
/* ### FeeRbGetNrSectorErases disabled - Fee_Rb_GetSectEraseCntr and Fee_Rb_GetNrOfSuprtdErases APIs not available ### */
/* ### FeeRbStressMeasmt disabled - Fee_Rb_RestartStressMeasmt and Fee_Rb_GetStressMeasmtResult APIs not available ### */
/* ### FeeRbEnterStopModeApi disabled - Fee_Rb_EnterStopMode API not available ### */
/* ### FeeRbGetFreeSpaceApi disabled - Fee_Rb_GetNrFreeBytes API not available ### */
/* ### FeeRbGetNrFreeBytesAndFatEntriesApi disabled - Fee_Rb_GetNrFreeBytesAndFatEntries API not available ### */
/* ### FeeRbSetAndGetJobModeApi disabled - Fee_Rb_SetJobMode() and Fee_Rb_GetJobMode() APIs not available ### */
/* ### FeeRbGetSectStateApi disabled - Fee_Rb_GetSectState API not available ### */
/* ### FeeRbEnterAndExitHighSpeedModeApis disabled - Fee_Rb_EnterHighSpeedMode() and Fee_Rb_ExitHighSpeedMode() not available ### */

/* End of Fee section */
#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

/* ******************************************************************************************************************
   *********************** Macro for external declarations of hook functions provided to Fee ************************
   ****************************************************************************************************************** */

#endif /* FEE_H */
