/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2015, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef EA_H
#define EA_H

/**
 **********************************************************************************************************************
 * \moduledescription
 *           AUTOSAR EA
 *           According to Document Version 4.0.2
 * \scope    INTERN
 **********************************************************************************************************************
 */


/*
 ****************************************************************************************************
 * Includes
 ****************************************************************************************************
 */
#include "Std_Types.h"
#include "Ea_Cfg.h"

/* For Ea test purposes only */
#define EA_PRV_DEBUGGING FALSE
/* EA_PRV_DEBUGGING related to rba_EaFs1 is removed from Ea_HLD from 16.1.0 release onwards Setting it to TRUE leads to build Error for rba_EaFs1 variant */

#if (EA_PRESENT == 1)

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */
/* EA013: Check whether AR versions are defined correctly */
#if (!defined(STD_TYPES_AR_RELEASE_MAJOR_VERSION) || \
        (STD_TYPES_AR_RELEASE_MAJOR_VERSION != EA_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatch"
#endif


/*
 ***************************************************************************************************
 * Type definition and structures
 ***************************************************************************************************
 */

/* Error codes for API used for Ea module */
/* [EA049]: Development error values are of type uint8 */
#define EA_E_NOT_INITIALIZED            0x01        /* Error code for an uninitialized module. This macro is kept to avoid build error in EaFs1 module, modification in EaFs1 is not allowed*/
#define EA_E_UNINIT                     0x01        /* Error code for an uninitialized module                       */
#define EA_E_INVALID_BLOCK_NO           0x02        /* Error code for invalid block number                          */
#define EA_E_INVALID_BLOCK_OFS          0x03        /* Error code for invalid block offset                          */
#define EA_E_PARAM_POINTER              0x04        /* Error code for null pointer                                  */
#define EA_E_INVALID_BLOCK_LEN          0x05        /* Error code for invalid block length                          */
#define EA_E_BUSY                       0x06        /* Error code for api called when BUSY with previous request    */
#define EA_E_INVALID_CANCEL             0x08        /* Error code for Ea_Cancel called while no job was pending     */
#define EA_E_INIT_FAILED                0x09        /* Error code for Ea_Init failed                                */
#define EA_E_RB_BLOCK_LENGTH_MISMATCH   0xA0        /* Error code for block length mismatch                         */
#define EA_E_RB_MIGRATION_FAILED        0xA1        /* Error code for unsuccessful migration                        */
#define EA_E_RB_MIGRATION_NOT_POSSIBLE  0xA2        /* Error code for migration not possible: not enough free space */
#define EA_E_RB_SETMODE_MIG_CONFLICT    0xA3        /* Error code for Set Mode calling while Migration is Ongoing     */

/* API Service IDs used for DET module */
#define EA_SID_INIT                 0x00        /* Service ID for the EA Init function */
#define EA_SID_SETMODE              0x01        /* Service ID for the EA set mode function */
#define EA_SID_READ                 0x02        /* Service ID for the EA read function */
#define EA_SID_WRITE                0x03        /* Service ID for the EA write function */
#define EA_SID_CANCEL               0x04        /* Service ID for the EA job cancel function */
#define EA_SID_GETSTATUS            0x05        /* Service ID for the EA get status function */
#define EA_SID_GETJOBRESULT         0x06        /* Service ID for the EA get job result function */
#define EA_SID_INVALIDATEBLOCK      0x07        /* Service ID for the EA invalidate block function */
#define EA_SID_GETVERSIONINFO       0x08        /* Service ID for the EA get version info function */
#define EA_SID_ERASE                0x09        /* Service ID for the EA erase immediate block function */
#define EA_SID_RB_MAINTENANCE       0xA0        /* Service ID for the EA repair redundant block function */
#define EA_SID_MAINFUNCTION         0xA1        /* Service ID for the EA main function */
#define EA_SID_RB_MIGRATIONINIT     0xA2        /* Service ID for the EA migration init */
#define EA_SID_RB_MIGRATION         0xA3        /* Service ID for the EA migration */
#define EA_SID_RB_FORCEDMIGRATION   0xA4        /* Service ID for the EA Forced Migration request function */
#define EA_SID_RB_GETADAPTERJOBRESULT  0xA5     /* Service ID for the EA get adapter job result function */
#define EA_SID_RB_ENTERSTOPMODE        0xA6     /* Service ID for the EA entered stop mode function */
#define EA_SID_RB_GETDETAILEDBLKINFO    0xA7        /* Service ID for getting detailed block infor */

/* Ea Migration possible states, Migration is not applicable for Boot Mode */
#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG != TRUE)
#define EA_RB_MIGRATION_NOTNEEDED       0x00            /* Migration Marker is Not changed Migration is Not Needed */
#define EA_RB_MIGRATION_RUNNING         0x01            /* Migration is Currently Running */
#define EA_RB_MIGRATION_FINISHED        0x02            /* Migration is completed sucessfully*/
#define EA_RB_MIGRATION_ERROR           0x03            /* Migration Exited with Error */
#define EA_RB_MIGRATION_FORCEDRUNNING   0x04            /* Forced Migration triggered via rba_FeeAddOnAS is running */
#endif

/*
 ***************************************************************************************************
 * Type definition and enums
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Externals
 ***************************************************************************************************
 */
#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"
extern MemIf_JobResultType Ea_JobResult;                /* Job Result of latest job */
#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

#define EA_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"
extern MemIf_ModeType Ea_Mode;
#define EA_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Ea_MemMap.h"

#define EA_START_SEC_VAR_INIT_UNSPECIFIED
#include "Ea_MemMap.h"
extern MemIf_StatusType Ea_GlobModuleState_st;          /* Main state of the EA driver */
#define EA_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Ea_MemMap.h"

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

#ifndef EA_RB_HIDE_RTEAPI
/**
 * Declaration of the service for performing the processing of the Ea jobs.
 * This declaration is used only if RTE generator is NOT used, for details please see coding guidelines
 * Rule BSW_ServiceRTE_002.
 */
extern void Ea_MainFunction(void);
#endif /* EA_RB_HIDE_RTEAPI */

extern void Ea_Init(void);
extern void Ea_Rb_EndInit(void);
extern Std_ReturnType Ea_Read(uint16 BlockNumber, uint16 BlockOffset, uint8* DataBufferPtr, uint16 Length);
extern Std_ReturnType Ea_Write(uint16 BlockNumber, uint8* DataBufferPtr);
extern Std_ReturnType Ea_Rb_BlockMaintenance(uint16 BlockNumber);
extern void Ea_Cancel(void);
extern MemIf_JobResultType Ea_GetJobResult(void);
extern MemIf_StatusType Ea_GetStatus(void);
extern void Ea_SetMode(MemIf_ModeType Mode);
extern void Ea_GetVersionInfo(Std_VersionInfoType* VersioninfoPtr);
extern Std_ReturnType  Ea_EraseImmediateBlock(uint16 BlockNumber);
extern Std_ReturnType  Ea_InvalidateBlock(uint16 BlockNumber);
extern MemIf_Rb_DetailedBlkInfo_ten Ea_Rb_GetDetailedBlkInfo(uint16 blockNumber_u16);
extern void Ea_Rb_MainFunctionAndDependency(void);
#if(EA_RB_ACT_UNKNOWN_BLOCK_MIG != TRUE)
extern uint8 Ea_Rb_MigrationStatus(void);
#if(defined(EA_RB_CFG_SELECTED_FS) && (2 == EA_RB_CFG_SELECTED_FS))
#if (EA_RB_ENTER_STOP_MODE != FALSE)
extern Std_ReturnType Ea_Rb_ForcedMigration(void);
#endif /* EA_RB_ENTER_STOP_MODE */
#endif /* EA_RB_CFG_SELECTED_FS */
#endif /* EA_RB_ACT_UNKNOWN_BLOCK_MIG */
#if (EA_RB_ENTER_STOP_MODE != FALSE)
extern MemIf_JobResultType Ea_Rb_GetAdapterJobResult(void);
extern Std_ReturnType Ea_Rb_EnterStopMode(void);
#endif /* EA_RB_ENTER_STOP_MODE */
#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"
/* End of #if (EA_PRESENT == 1) */
#endif
/* EA_H */
#endif

