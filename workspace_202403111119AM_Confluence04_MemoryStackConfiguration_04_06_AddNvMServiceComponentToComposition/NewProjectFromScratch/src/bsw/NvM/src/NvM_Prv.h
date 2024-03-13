/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2017, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_H
#define NVM_PRV_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_Job_Types.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
// Service IDs
// -----------
// Redundant block maintenance service ID
#define NVM_PRV_SERVICE_ID_MAINTAIN_BLOCK (249u)

// Miscellaneous
// -------------
// Magic value used with NvM_Prv_SavedZoneMarker_u32 to detect saved zone resets if NvMRbRamInitCheck is set to quick mode
#define NVM_PRV_SAVED_ZONE_MAGIC_VALUE      (0xABCDEFABuL)
// Index for the default savedzone marker
#define NVM_RB_DEFAULT_SAVEDZONE_IDX        (0u)

/**
 * Maximum value for the number of written bytes, i.e. number of written bytes cannot exceed this value.
 */
#define NVM_PRV_NR_BYTES_WRITTEN_MAX_VALUE  (0xFFFFFFFFu)
/**
 * Maximum value for the accumulated number of all write requests,
 * i.e. number of written bytes cannot exceed this value.
 */
#define NVM_PRV_CNTR_WRITES_ABS_MAX_VALUE   (0xFFFFFFFFu)
/**
 * Maximum value for the number write requests since last power-on,
 * i.e. number of write requests cannot exceed this value.
 */
#define NVM_PRV_CNTR_WRITES_MAX_VALUE       (0xFFFFu)

/**
 * Number of internal jobs processed in parallel.
 */
#if (NVM_CRYPTO_USED == STD_ON)
# define NVM_PRV_NR_PARALLEL_JOB  (NVM_CFG_NR_MEMIF_DEVICES + 1u)
#else
# define NVM_PRV_NR_PARALLEL_JOB  (NVM_CFG_NR_MEMIF_DEVICES)
#endif

/**
 * Number of queue entries for the queue with suspended user requests.
 */
#define NVM_PRV_SIZE_SUSPENDED_JOB_QUEUE    NVM_PRV_NR_PARALLEL_JOB

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */

#define NVM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"

// Common (i.e. not block-specific) states
// TRACE[NVM506] Hint: All of the following variables are global and hence accessible via debugger
// TRACE[NVM508] The size of all the following variables can be determined via the standard C sizeof operator
extern NvM_Prv_MainStates_tst NvM_Prv_Main_st;

#define NVM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
// Private code (defined in various NvM_*.c files)
// -----------------------------------------------
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

#if (NVM_RB_AUX_INTERFACE == STD_ON)
extern void NvM_Prv_InitializeAuxMigrationResult(void);
extern void NvM_Prv_SetAuxMigrationResult(NvM_Prv_idService_tuo idService_uo, NvM_BlockIdType idBlock_uo);
#endif

// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2999] the potentially incompatible blocks after layout changes should be
//                                               sanitized by ReadAll at first
extern boolean NvM_Prv_IsSanitizedByReadAll(NvM_BlockIdType idBlock_uo);

#if ((NVM_DYNAMIC_CONFIGURATION == STD_ON) && (NVM_RB_REMOVE_NON_RESISTANT_BLOCKS == STD_ON))
extern void NvM_Prv_InitializeStateReadAll(void);
#endif

#if ((NVM_DYNAMIC_CONFIGURATION == STD_ON) || (NVM_RB_INIT_AT_LAYOUT_CHANGE == STD_ON))
extern void NvM_Prv_InitAfterReadAll(NvM_Prv_JobResult_ten JobResult_en);
#endif

// Main function sub-tasks (defined in NvM_MainFunction+.c)
extern void NvM_Prv_InitializeMainStates(void);
extern void NvM_Prv_UninitializeMainStates(void);
extern boolean NvM_Prv_IsNvmInitialized(void);

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
extern void NvM_Prv_RequireHighSpeedMode(boolean isHighSpeedModeRequired_b);
extern void NvM_Prv_SetHighSpeedMode(boolean stHighSpeedMode_b);
extern boolean NvM_Prv_IsHighSpeedModeActive(void);
extern boolean NvM_Prv_IsHighSpeedModeRequired(void);
#endif

extern boolean NvM_Prv_IsNvmIdle(void);
extern boolean NvM_Prv_IsCrcRecalcActive(void);
extern boolean NvM_Prv_IsMultiActive(void);
extern NvM_Prv_idService_tuo NvM_Prv_GetActiveService(void);
extern NvM_Prv_idQueue_tuo NvM_Prv_GetActiveQueue(void);
extern void NvM_Prv_UpdateActiveServiceData(NvM_Prv_idService_tuo idService_uo,
                                            NvM_Prv_idQueue_tuo idReqQueue_uo);
extern NvM_Prv_idJob_ten NvM_Prv_GetJobId(NvM_Prv_ServiceBit_tuo ServiceBit_uo);
extern boolean NvM_Prv_IsSingleBlockProcessingIndependentOnReadAll(void);
extern void NvM_Prv_SetReadAllDone(NvM_Prv_idService_tuo idService_uo);

extern boolean NvM_Prv_MainFunctionArbitrate(NvM_Prv_MainStates_tst* MainStates_pst);
extern void NvM_Prv_MainFunctionJobHandling(void);
extern void NvM_Prv_MainFunctionResultEval(NvM_Prv_MainStates_tst* MainStates_pst);

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)
extern boolean NvM_Prv_IsInitVectorAvailable(void);
extern void NvM_Prv_SetInitVectorStatus(boolean isInitVectorAvalable_b);
extern uint8* NvM_Prv_GetInitVectorCryptoKey(void);
extern void NvM_Prv_IncrementInitVector(void);
#endif

#if (NVM_RB_REQ_MONITOR == NVM_RB_REQ_MON_WRITE)
extern void NvM_Prv_InitializeWriteMonCntrs(boolean isSavedZoneDataLost_b);
extern void NvM_Prv_WriteMonCntrHandling(NvM_BlockIdType idBlock_uo,
                                         uint16 BlockLength_u16,
                                         boolean isCallBeforeCrcCalc_b);
#endif

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
extern NvM_Prv_Activities_ten NvM_Prv_Main_GetCurrentState(void);
extern void NvM_Prv_SetSemaphoreFlag (boolean flg_b);
extern void NvM_Prv_ReadAll_SetState(void);
#endif

#if (NVM_RB_RAM_INIT_CHECK == NVM_RB_RAM_INIT_CHECK_QUICK)
extern void NvM_Prv_SetMarkerSavedZoneId(uint8 idxSavedZone_u8, uint32 MarkerSavedZone_u32);
extern uint32 NvM_Prv_GetMarkerSavedZoneId(uint8 idxSavedZone_u8);
#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_H */
#endif
