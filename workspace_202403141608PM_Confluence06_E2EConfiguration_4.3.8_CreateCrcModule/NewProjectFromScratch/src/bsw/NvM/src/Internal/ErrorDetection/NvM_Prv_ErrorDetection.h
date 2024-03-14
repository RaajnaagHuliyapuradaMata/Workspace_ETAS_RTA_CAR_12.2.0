/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2018, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_ERRORDETECTION_H
#define NVM_PRV_ERRORDETECTION_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Types.h"
#include "NvM_Prv_Job_Types.h"
#include "NvM_Prv_Queue_Types.h"
#include "NvM_Prv_Service_Types.h"
#if (NVM_CRYPTO_USED == STD_ON)
# include "NvM_Prv_Crypto_ServiceTypes.h"
#endif

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */
// Macro used to initialize the local DET error ID variable used in APIs
#define NVM_PRV_NO_DET_ERROR (0u)

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
typedef union
{
    void*                       ptrToCheck_pv;
    uint8*                      ptrDataIdx_pu8;
    NvM_RequestResultType*      ptrRequestResult_puo;
    NvM_Rb_MigrationResult_ten* ptrMigrationResult_pen;
    NvM_Rb_ErrorInfoDetail_ten* ptrErrorInfoDetail_pen;
    NvM_Rb_StatusType*          ptrStatusType_pen;
    uint16*                     ptrBlockLength_pu16;
    NvM_Prv_idService_tuo*      ptrIdService_puo;
    NvM_BlockIdType*            ptrIdBlock_puo;
    NvM_Rb_BlockErrors_tuo*     ptrBlockErr_puo;
    Std_VersionInfoType*        ptrVersionInfo_pst;
    uint32*                     ptrCntrWriteBlock_puo;

} NvM_Prv_ErrorDetection_Ptr_tun;

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define NVM_START_SEC_VAR_SAVED_ZONE_UNSPECIFIED
#include "NvM_MemMap.h"

// ID of the most previous error reported via Det_ReportError within NvM (initialized to NVM_PRV_NO_DET_ERROR)
extern NvM_Prv_idDetError_tuo NvM_Prv_idLastDetError_uo;
// ID of the service which triggered the last DET error (initialized to NVM_SERVICE_ID_INIT)
extern NvM_Prv_idService_tuo NvM_Prv_idServiceLastDetError_uo;
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3113] Private variable to store block ID used in NvM service
//                                               which has caused the last DET error
extern NvM_BlockIdType NvM_Prv_idBlockLastDetError_uo;

#define NVM_STOP_SEC_VAR_SAVED_ZONE_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern NvM_Rb_BlockErrors_tuo NvM_Prv_ErrorDetection_GetBlockErrors(NvM_BlockIdType idBlock_uo);
extern void NvM_Prv_ErrorDetection_InitializeDetError(boolean isSavedZoneDataLost_b);
extern void NvM_Prv_ErrorDetection_InitializeProductionErrors(void);
extern void NvM_Prv_ErrorDetection_SetProductionError(NvM_BlockIdType idBlock_uo, uint8 MaskErrorBit_u8);
extern boolean NvM_Prv_ErrorDetection_IsServiceBitValid(NvM_Prv_idService_tuo idService_uo,
                                                        NvM_BlockIdType idBlock_uo,
                                                        NvM_Prv_ServiceBit_tuo ServiceBit_uo);
extern boolean NvM_Prv_ErrorDetection_IsJobIdValid(NvM_Prv_idService_tuo idService_uo,
                                                   NvM_BlockIdType idBlock_uo,
                                                   NvM_Prv_idJob_ten idJob_en);
extern boolean NvM_Prv_ErrorDetection_IsJobResultMemIfPlausible(NvM_Prv_idService_tuo idService_uo,
                                                                NvM_BlockIdType idBlock_uo,
                                                                NvM_Prv_JobResult_ten JobResult_en);
extern boolean NvM_Prv_ErrorDetection_IsBlockSizeValidForExplicitSync(NvM_Prv_idService_tuo idService_uo,
                                                                      NvM_BlockIdType idBlock_uo,
                                                                      uint32 SizeRamMirror_u32);
extern boolean NvM_Prv_ErrorDetection_IsPtrValid(NvM_Prv_idService_tuo idService_uo,
                                                 NvM_BlockIdType idBlock_uo,
                                                 NvM_Prv_idDetError_tuo idDetError_uo,
                                                 NvM_Prv_ErrorDetection_Ptr_tun const* ptr_pcun);
extern boolean NvM_Prv_ErrorDetection_IsNvmInitialized(NvM_Prv_idService_tuo idService_uo,
                                                       NvM_BlockIdType idBlock_uo);
extern boolean NvM_Prv_ErrorDetection_IsBlockIdValid(NvM_Prv_idService_tuo idService_uo,
                                                     NvM_BlockIdType idBlock_uo,
                                                     boolean isMultiBlockAllowed_b);
extern boolean NvM_Prv_ErrorDetection_IsDefaultDataAvailable(NvM_Prv_idService_tuo idService_uo,
                                                             NvM_BlockIdType idBlock_uo);
extern boolean NvM_Prv_ErrorDetection_IsRamBlockAddressValid(NvM_Prv_idService_tuo idService_uo,
                                                             NvM_BlockIdType idBlock_uo,
                                                             void const* RamBlockAddress_pv);
extern boolean NvM_Prv_ErrorDetection_IsBlockTypeDataset(NvM_Prv_idService_tuo idService_uo,
                                                         NvM_BlockIdType idBlock_uo);
extern boolean NvM_Prv_ErrorDetection_IsBlockIdxValid(NvM_Prv_idService_tuo idService_uo,
                                                      NvM_BlockIdType idBlock_uo,
                                                      uint8 idxData_u8);
extern boolean NvM_Prv_ErrorDetection_IsBlockPriorityImmediate(NvM_Prv_idService_tuo idService_uo,
                                                               NvM_BlockIdType idBlock_uo);
extern boolean NvM_Prv_ErrorDetection_IsBlockWriteProtectionChangeable(NvM_Prv_idService_tuo idService_uo,
                                                                       NvM_BlockIdType idBlock_uo);
extern void NvM_Prv_ErrorDetection_ReportReentrantError(NvM_Prv_idService_tuo idService_uo);

extern void NvM_Prv_ErrorDetection_ReportServiceInitiationErrors(NvM_Prv_idService_tuo idService_uo,
                                                                 NvM_BlockIdType idBlock_uo,
                                                                 NvM_Prv_Service_InitiationErrors_tuo Errors_uo);
#if (NVM_CRYPTO_RANDOM_USED == STD_ON)

extern void NvM_Prv_ErrorDetection_ReportInitVectorNotAvailable(NvM_Prv_idService_tuo idService_uo,
                                                                NvM_BlockIdType idBlock_uo);

#endif

#if (NVM_CRYPTO_USED == STD_ON)

extern boolean NvM_Prv_ErrorDetection_IsCsmJobRetryLimitExceeded(NvM_Prv_idService_tuo idService_uo,
                                                                 NvM_BlockIdType idBlock_uo,
                                                                 uint8 const* cntrCsmJobRetry_pcuo);

extern boolean NvM_Prv_ErrorDetection_IsCsmJobResultPlausible(NvM_Prv_idService_tuo idService_uo,
                                                              NvM_BlockIdType idBlock_uo,
                                                              NvM_Prv_idDetError_tuo idErrorPrimaryOutput_uo,
                                                              NvM_Prv_idDetError_tuo idErrorSecondaryOutput_uo,
                                                              NvM_Prv_Crypto_ServiceResult_tst const* JobResult_pcst);

#endif  // (NVM_CRYPTO_USED == STD_ON)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_ERRORDETECTION_H */
#endif
