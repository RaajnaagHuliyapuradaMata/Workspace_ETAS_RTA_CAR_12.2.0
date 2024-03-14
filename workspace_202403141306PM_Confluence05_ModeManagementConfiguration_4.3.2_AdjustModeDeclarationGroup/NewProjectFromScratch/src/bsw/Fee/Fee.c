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
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"
#include "MemIf_Types.h"
#include "Fee_Rb_Types.h"   
#include "Fee_Rb_Idx.h"
#include "Fee_Cfg.h"
#include "Fee.h"

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */
 
#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"
 

/* MR12 RULE 8.13 VIOLATION: AR4.2 defines this function prototype with a non-constant pointer */
void Fee_Init(Fee_ConfigType const * CfgPtr_pst)
{
    Fee_Rb_Idx_Init(Fee_Rb_DeviceName,CfgPtr_pst);
    return;
}

void Fee_MainFunction(void)
{
    Fee_Rb_Idx_MainFunction(Fee_Rb_DeviceName);
    return;
}

void Fee_Rb_MainFunctionAndDependency(void)
{
    Fee_Rb_Idx_MainFunctionAndDependency(Fee_Rb_DeviceName);
    return;
}

Std_ReturnType Fee_Read(uint16 nrBlks_u16, uint16 nrBlkOffs_u16, uint8 * DataBuf_pu8, uint16 nrLen_u16)
{
    return(Fee_Rb_Idx_Read(Fee_Rb_DeviceName, nrBlks_u16, nrBlkOffs_u16, DataBuf_pu8, nrLen_u16));
}

Std_ReturnType Fee_Write(uint16 nrBlks_u16, uint8 * DataBuf_pu8)
{
    return(Fee_Rb_Idx_Write(Fee_Rb_DeviceName, nrBlks_u16, DataBuf_pu8));
}

Std_ReturnType Fee_InvalidateBlock(uint16 nrBlks_u16)
{
    return(Fee_Rb_Idx_InvalidateBlock(Fee_Rb_DeviceName, nrBlks_u16));
}

MemIf_StatusType Fee_GetStatus(void)
{
    return(Fee_Rb_Idx_GetStatus(Fee_Rb_DeviceName));
}

MemIf_JobResultType Fee_GetJobResult(void)
{
    return(Fee_Rb_Idx_GetJobResult(Fee_Rb_DeviceName));
}

Std_ReturnType Fee_EraseImmediateBlock(uint16 nrBlks_u16)
{
    return(Fee_Rb_Idx_EraseImmediateBlock(Fee_Rb_DeviceName, nrBlks_u16));
}

void Fee_Cancel(void)
{
/* ### FeeRbCancel disabled - Cancellation of ongoing job not available ### */
    return;
}

/* ### FeeRbEnterAndExitHighSpeedModeApis disabled - Fee_Rb_EnterHighSpeedMode() and Fee_Rb_ExitHighSpeedMode() not available ### */

/* ### FeeSetModeSupported disabled - Fee_SetMode, Fee_Rb_GetMode API not available ### */

/* ### FeeVersionInfoApi disabled - Fee_GetVersionInfo API not available ### */

void Fee_Rb_EndInit(void)
{
    Fee_Rb_Idx_EndInit(Fee_Rb_DeviceName);
    return;
}

MemIf_JobResultType Fee_Rb_GetAdapterJobResult(void)
{
    return(Fee_Rb_Idx_GetAdapterJobResult(Fee_Rb_DeviceName));
}

MemIf_JobResultType Fee_Rb_GetInternalJobResult(void)
{
    return(Fee_Rb_Idx_GetInternalJobResult(Fee_Rb_DeviceName));
}

/* ### System jobs e.g. FeeRbTriggerReorgApi or FeeRbEnterStopModeApi are disabled - Fee_GetSystemJobResult, Fee_SystemJob API not available ### */

uint32 Fee_Rb_GetSectChngCnt(void)
{
    return(Fee_Rb_Idx_GetSectChngCnt(Fee_Rb_DeviceName));
}

void Fee_Rb_DisableBackgroundOperation(void)
{
    Fee_Rb_Idx_DisableBackgroundOperation(Fee_Rb_DeviceName);
    return;
}

void Fee_Rb_EnableBackgroundOperation(void)
{
    Fee_Rb_Idx_EnableBackgroundOperation(Fee_Rb_DeviceName);
    return;
}

boolean Fee_Rb_IsBlockDoubleStorageActiveByBlockNr(uint16 blkNr_u16)
{
    return(Fee_Rb_Idx_IsBlockDoubleStorageActiveByBlockNr(Fee_Rb_DeviceName, blkNr_u16));
}

Fee_Rb_WorkingStateType_ten Fee_Rb_GetWorkingState(void)
{
    return(Fee_Rb_Idx_GetWorkingState(Fee_Rb_DeviceName));
}

/* ### FeeRbMaintainRedundantBlocks disabled - Fee_Rb_BlockMaintenance API not available ### */

/* ### FeeRbVarBlockLength and FeeRbFirstReadDataMigration disabled for all blocks - Fee_Rb_VarLenRead API not available ### */

/* ### FeeRbVarBlockLength disabled for all blocks - Fee_Rb_VarLenWrite API not available ### */

/* ### FeeRbFirstReadDataMigration disabled for all blocks - Fee_Rb_GetMigrationResult API not available ### */

/* ### FeeRbDetailedBlkInfoApi disabled - Fee_Rb_GetDetailedBlkInfo API not available ### */

/* ### FeeRbTriggerReorgApi disabled - Fee_Rb_TriggerReorg, Fee_Rb_TriggerReorgFAT, Fee_Rb_TriggerReorgDAT API not available ### */

/* ### No Fee-internal persistent data - Fee_Rb_WritePersistentData() API not available ### */

/* ### FeeRbGetNrSectorErases disabled - Fee_Rb_GetSectEraseCntr and Fee_Rb_GetNrOfSuprtdErases APIs not available ### */

/* ### FeeRbStressMeasmt disabled - Fee_Rb_RestartStressMeasmt and Fee_Rb_GetStressMeasmtResult APIs not available ### */

/* ### FeeRbEnterStopModeApi disabled - Fee_Rb_EnterStopMode API not available ### */

/* ### FeeRbGetFreeSpaceApi disabled - Fee_Rb_GetNrFreeBytes API not available ### */

/* ### FeeRbGetNrFreeBytesAndFatEntriesApi disabled - Fee_Rb_GetNrFreeBytesAndFatEntries API not available ### */

/* ### FeeRbSetAndGetJobModeApi disabled - Fee_Rb_SetJobMode() and Fee_Rb_GetJobMode() APIs available ### */

/* ### FeeRbGetSectStateApi disabled - Fee_Rb_GetSectState API not available ### */

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"
