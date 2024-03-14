/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2022, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_JOBSTATEMACHINE_TYPES_H
#define NVM_PRV_JOBSTATEMACHINE_TYPES_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_JobResource_Types.h"
#include "NvM_Prv_Crypto_ServiceTypes.h"
#include "NvM_Prv_InternalBuffer_Types.h"

/*
 **********************************************************************************************************************
 * Macros
 **********************************************************************************************************************
 */

/* This is only a test extension */
#ifndef NVM_FUNC_NULL
#define NVM_FUNC_NULL   NULL_PTR
#endif

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

/**
 * This macro generates lists where each entry corresponds to a job state machine provided by NvM.
 *
 * Each entry of the macro shall look like follows:
 *
 * ENTRY(Name, Data)
 *
 * with:
 * Name: name of the job for which the entry is generated
 * Data: function to get data required for state machine execution
 *
 * \note
 * This macro uses a technique for reliable maintenance of parallel lists of code and/or data,
 * whose corresponding items must be declared or executed in the same order.
 * For details see X_Macro concept
 */
#define LIST_JOB_STATE_MACHINES(ENTRY)  \
    ENTRY(Invalid,                  NVM_FUNC_NULL)                          \
    ENTRY(DoCrc,                    NvM_Prv_Crc_Calculate_GetData)          \
    ENTRY(DoCalcNvCrc,              NvM_Prv_JobCalcNvCrc_GetData)           \
    ENTRY(DoRecalcUserDataCrc,      NvM_Prv_JobRecalcUserDataCrc_GetData)   \
    ENTRY(DoRead,                   NvM_Prv_JobRead_GetData)                \
    ENTRY(DoReadRecovery,           NvM_Prv_JobReadRecovery_GetData)        \
    ENTRY(DoWrite,                  NvM_Prv_JobWrite_GetData)               \
    ENTRY(DoRestore,                NvM_Prv_JobRestore_GetData)             \
    ENTRY(DoRestoreWrite,           NvM_Prv_JobRestoreWrite_GetData)        \
    ENTRY(DoInvalidate,             NvM_Prv_JobInvalidate_GetData)          \
    ENTRY(DoInvalidateAll,          NvM_Prv_JobInvalidateAll_GetData)       \
    ENTRY(DoValidate,               NvM_Prv_JobValidate_GetData)            \
    ENTRY(DoRecalcRamBklCrc,        NvM_Prv_JobRecalcRamBlkCrc_GetData)     \
    ENTRY(DoBlockSplit_Read,        NvM_Prv_BlockSplit_Read_GetData)        \
    ENTRY(DoBlockSplit_Write,       NvM_Prv_BlockSplit_Write_GetData)       \
    ENTRY(DoBlockSplit_Invalidate,  NvM_Prv_BlockSplit_Invalidate_GetData)  \
    ENTRY(DoMemIf_Read,             NvM_Prv_MemIf_Read_GetData)             \
    ENTRY(DoMemIf_Write,            NvM_Prv_MemIf_Write_GetData)            \
    ENTRY(DoMemIf_Invalidate,       NvM_Prv_MemIf_Invalidate_GetData)       \
    ENTRY(DoMemIf_Maintain,         NvM_Prv_MemIf_Maintain_GetData)         \
    ENTRY(DoCryptoRead,             NvM_Prv_CryptoRead_GetData)             \
    ENTRY(DoCryptoWrite,            NvM_Prv_CryptoWrite_GetData)            \
    ENTRY(DoCryptoKeySetting,       NvM_Prv_CryptoKeySetting_GetData)       \
    ENTRY(DoCryptoGenerateRandomNr, NvM_Prv_CryptoGenerateRandomNr_GetData)

/**
 * Function-like macro: LIST_ENTRY_JOB_STATE_MACHINE_ID
 *
 * This macro generates an entry for an enumeration with all job state machine IDs provided by NvM.
 * S.a. macro LIST_JOB_STATE_MACHINES to generate a list for all job state machines provided by NvM
 *
 * Paramater:
 * Name:    name of the state machine to generate an enumeration element
 * Data:    unused
 */
#define LIST_ENTRY_JOB_STATE_MACHINE_ID(Name, Data)  NvM_Prv_idJobStateMachine_##Name##_e,

/**
 * \brief
 * Definition of all job state machine IDs provided by NvM.
 * \details
 * This enumeration is defined via Macros LIST_JOB_STATE_MACHINES and LIST_ENTRY_JOB_STATE_MACHINE_ID
 * to get all IDs in predefined order.
 */
typedef enum
{
    /* MR12 RULE 20.7 VIOLATION: To create the list the argument shall be not parenthesized */
    LIST_JOB_STATE_MACHINES(LIST_ENTRY_JOB_STATE_MACHINE_ID)

    //-------------------------------------------------------------------------
    // This is actually no job state machine,
    // in fact this enum value defines count of job state machines available
    // in NvM and can be used for array definition
    //-------------------------------------------------------------------------
    NvM_Prv_idJobStateMachine_Count_e

} NvM_Prv_idJobStateMachine_ten;

/**
 * \brief
 * Definition of different states for all job state machines provided by NvM.
 * \details
 * The NvM provides following job types:
 * - write jobs
 * - read jobs
 * - restore jobs
 * - invalidate jobs
 * - maintain jobs
 * - validate jobs
 * - jobs to recalculate RAM block CRCs in background
 */
typedef enum
{
    NvM_Prv_stJob_Idle_e,

    NvM_Prv_stJob_Crc_Calculate_Start_e,
    NvM_Prv_stJob_Crc_Calculate_Do_e,
    NvM_Prv_stJob_Crc_Calculate_Completed_e,

    NvM_Prv_stJob_CalcNvCrc_Start_e,
    NvM_Prv_stJob_CalcNvCrc_DoDataCrc_e,
    NvM_Prv_stJob_CalcNvCrc_DoBlkHdrCrc_e,
    NvM_Prv_stJob_CalcNvCrc_Completed_e,

    NvM_Prv_stJob_RecalcUserDataCrc_Start_e,
    NvM_Prv_stJob_RecalcUserDataCrc_DoUserDataCrc_e,
    NvM_Prv_stJob_RecalcUserDataCrc_Completed_e,

    NvM_Prv_stJob_Write_GetUserData_e,
    NvM_Prv_stJob_Write_RecalcUserDataCrc_e,
    NvM_Prv_stJob_Write_DoBlockSplit_e,
    NvM_Prv_stJob_Write_DoCrypto_e,
    NvM_Prv_stJob_Write_AppendAdminData_e,
    NvM_Prv_stJob_Write_AppendBlockHeader_e,
    NvM_Prv_stJob_Write_CalcNvBlkCrc_e,
    NvM_Prv_stJob_Write_DoMemIf_e,
    NvM_Prv_stJob_Write_DoBlockSplitInvalidate_e,
    NvM_Prv_stJob_Write_Completed_e,

    NvM_Prv_stJob_Write_Crypto_GetKeyInitVector_e,
    NvM_Prv_stJob_Write_Crypto_DoKeySetting_e,
    NvM_Prv_stJob_Write_Crypto_StartEncryption_e,
    NvM_Prv_stJob_Write_Crypto_PollEncryption_e,
    NvM_Prv_stJob_Write_Crypto_StartGenerationSignature_e,
    NvM_Prv_stJob_Write_Crypto_PollGenerationSignature_e,
    NvM_Prv_stJob_Write_Crypto_StartGenerationMac_e,
    NvM_Prv_stJob_Write_Crypto_PollGenerationMac_e,

    NvM_Prv_stJob_Read_Start_e,
    NvM_Prv_stJob_Read_GetUserData_e,
    NvM_Prv_stJob_Read_CrcCheck_e,
    NvM_Prv_stJob_Read_DoMemIf_e,
    NvM_Prv_stJob_Read_RecalcUserDataCrc_e,
    NvM_Prv_stJob_Read_CalcNvBlkCrc_e,
    NvM_Prv_stJob_Read_ExtractAdminData_e,
    NvM_Prv_stJob_Read_DoBlockSplit_e,
    NvM_Prv_stJob_Read_DoCrypto_e,
    NvM_Prv_stJob_Read_SetUserData_e,
    NvM_Prv_stJob_Read_Completed_e,

    NvM_Prv_stJob_ReadFromMedium_Start_e,
    NvM_Prv_stJob_ReadFromMedium_DoMemIf_e,
    NvM_Prv_stJob_ReadFromMedium_Completed_e,

    NvM_Prv_stJob_Read_Crypto_StartMacVerification_e,
    NvM_Prv_stJob_Read_Crypto_PollMacVerification_e,
    NvM_Prv_stJob_Read_Crypto_StartSignatureVerification_e,
    NvM_Prv_stJob_Read_Crypto_PollSignatureVerification_e,
    NvM_Prv_stJob_Read_Crypto_DoKeySetting_e,
    NvM_Prv_stJob_Read_Crypto_StartDecryption_e,
    NvM_Prv_stJob_Read_Crypto_PollDecryption_e,

    NvM_Prv_stJob_BlockSplit_Write_DoSubBlocks_e,
    NvM_Prv_stJob_BlockSplit_Write_Completed_e,

    NvM_Prv_stJob_BlockSplit_Read_DoSubBlocks_e,
    NvM_Prv_stJob_BlockSplit_Read_Completed_e,

    NvM_Prv_stJob_BlockSplit_Invalidate_DoSubBlocks_e,
    NvM_Prv_stJob_BlockSplit_Invalidate_Completed_e,

    NvM_Prv_stJob_CryptoKeySetting_SetInitVector_e,
    NvM_Prv_stJob_CryptoKeySetting_PollSetInitVector_e,
    NvM_Prv_stJob_CryptoKeySetting_SetValid_e,
    NvM_Prv_stJob_CryptoKeySetting_PollSetValid_e,
    NvM_Prv_stJob_CryptoKeySetting_Completed_e,

    NvM_Prv_stJob_GenerateRandomNr_Crypto_Start_e,
    NvM_Prv_stJob_GenerateRandomNr_Crypto_Poll_e,
    NvM_Prv_stJob_GenerateRandomNr_Crypto_Completed_e,

    NvM_Prv_stJob_Restore_Start_e,
    NvM_Prv_stJob_Restore_CopyFromRom_e,
    NvM_Prv_stJob_Restore_ExplicitSync_e,
    NvM_Prv_stJob_Restore_InitCallback_e,
    NvM_Prv_stJob_Restore_Crc_e,
    NvM_Prv_stJob_Restore_Completed_e,

    NvM_Prv_stJob_Invalidate_Start_e,
    NvM_Prv_stJob_Invalidate_DoSubBlocks_e,
    NvM_Prv_stJob_Invalidate_DoInvalidate_e,
    NvM_Prv_stJob_Invalidate_Completed_e,

    NvM_Prv_stJob_MemIf_Write_Initiate_e,
    NvM_Prv_stJob_MemIf_Poll_Write_e,
    NvM_Prv_stJob_MemIf_Read_Initiate_e,
    NvM_Prv_stJob_MemIf_Poll_Read_e,
    NvM_Prv_stJob_MemIf_Invalidate_Initiate_e,
    NvM_Prv_stJob_MemIf_Poll_Invalidate_e,
    NvM_Prv_stJob_MemIf_Maintain_Initiate_e,
    NvM_Prv_stJob_MemIf_Poll_Maintain_e,
    NvM_Prv_stJob_MemIf_Completed_e,

    NvM_Prv_stJob_Validate_Start_e,
    NvM_Prv_stJob_Validate_Completed_e,

    NvM_Prv_stJob_RecalcRamBlkCrc_ExplicitSyncWriteToNvM_e,
    NvM_Prv_stJob_RecalcRamBlkCrc_Do_e,
    NvM_Prv_stJob_RecalcRamBlkCrc_Completed_e,

    NvM_Prv_stJob_ReadRecovery_Start_e,
    NvM_Prv_stJob_ReadRecovery_DoRead_e,
    NvM_Prv_stJob_ReadRecovery_DoRecovery_e,
    NvM_Prv_stJob_ReadRecovery_Completed_e,

    NvM_Prv_stJob_InvalidateAll_Start_e,
    NvM_Prv_stJob_InvalidateAll_DoInvalidate_e,
    NvM_Prv_stJob_InvalidateAll_Completed_e,

    NvM_Prv_stJob_RestoreWrite_Start_e,
    NvM_Prv_stJob_RestoreWrite_DoRestore_e,
    NvM_Prv_stJob_RestoreWrite_DoWrite_e,
    NvM_Prv_stJob_RestoreWrite_Completed_e,

    //-------------------------------------------------------------------------
    // This is actually no job state,
    // in fact this enum value defines count of job states available in NvM and
    // can be used for array definition
    //-------------------------------------------------------------------------
    NvM_Prv_stJob_Count_e

} NvM_Prv_stJob_ten;

/**
 * Definition of enumeration for the job result type.
 */
typedef enum
{
    /// Job step has succeeded.
    NvM_Prv_JobResult_Succeeded_e,
    /// Job step is pending and job must be continued in the next cycle.
    NvM_Prv_JobResult_Pending_e,
    /// Job has failed.
    NvM_Prv_JobResult_Failed_e,
    /// Job has failed and implicit recovery not possible (explicit synchronization callback has failed).
    NvM_Prv_JobResult_FailedNoRecovery_e,
    /// Block processed in current job is inconsistent.
    NvM_Prv_JobResult_BlockInconsistent_e,
    /// Block processed in current job is invalidated.
    NvM_Prv_JobResult_BlockInvalidated_e,
    /// Block processed in current job has been recovered with default values
    NvM_Prv_JobResult_BlockDefault_e,
    /// Block skipped in multi-block service
    NvM_Prv_JobResult_Skipped_e,
    /// Job finished without access to medium, e.g. due to RAM Block CRC usage
    NvM_Prv_JobResult_Succeeded_MemIfSkipped_e

} NvM_Prv_JobResult_ten;

/**
 * Definition of the structure for data required to process a NvM job.
 */
typedef struct
{
    /// ID of the queue containing current job
    NvM_Prv_idQueue_tuo idQueue_uo;
    /// ID of the block service for which current job will be done
    NvM_Prv_idService_tuo idService_uo;
    /// Service bit for the block service for which current job will be done
    NvM_Prv_ServiceBit_tuo ServiceBit_uo;
    /// Id of the block used in current job
    NvM_BlockIdType idBlock_uo;
    /// Index of the dataset used in current job
    uint8 idxDataset_u8;

    /// Information whether current job will be done for a multi-block service
    boolean isMultiServiceActive_b;
    /// Information whether current job will be done for an auxiliary service
    boolean isAuxServiceActive_b;
    /// Information whether the permanent RAM block is used in current job.
    boolean isPRamBlockUsed_b;
    /// Information whether user data will be copied into the NvM internal buffer for current job.
    boolean isIntBufferToUse_b;
    /// Information whether encryption of user data is enabled.
    boolean isEncryptionEnabled_b;


    NvM_Prv_Queue_idx_tuo idxEntryInJobQueue_uo;

    /// Pointer to buffer for user data used in current job.
    /// For write jobs this buffer contains user data to be written
    /// (for explicit synchronization the NvM copies user data via callback into the internal buffer
    ///  before writing it).
    /// For read/restore jobs this buffer contains read/restored user data
    /// (for explicit synchronization NvM reads/restores user data into the internal buffer and
    ///  copies it via callback afterwards).
    NvM_Prv_ptrRamBlock_tun UserData_un;

    /// Pointer to the internal buffer structure used in current job if required by block configuration.
    NvM_Prv_InternalBuffer_st IntBuffer_st;

} NvM_Prv_JobData_tst;

/**
 * Definition of the structure for CRC calculation during a NvM job.
 */
typedef struct
{
    /// Counter for bytes remaining during CRC recalculation
    uint16                  cntrCrcCalculation_uo;
    /// Set to TRUE to initiate a new CRC calculation with start value from CRC component.
    /// Set to FALSE when initiating a subsequent CRC calculation with a CRC used as start value.
    boolean                 isFirstCall_b;
    /// Length of the data in bytes for the CRC calculation.
    uint16                  Length_u16;
    /// Pointer to the buffer which contains data for the CRC calculation.
    uint8*                  Buffer_pu8;
    /// CRC calculated in last operation over data in the buffer defined by Buffer_pu8.
    NvM_Prv_Crc_tun         Crc_un;

} NvM_Prv_Job_CrcCalculation_tst;

/**
 * Definition of the structure for data required for CRC calculation during a NvM job.
 */
typedef struct
{
    NvM_Prv_Job_CrcCalculation_tst Calculation_st;
    /// CRC calculated over data in the permanent RAM block.
    /// Permanent RAM block contains only user data.
    // TRACE[SWS_NvM_00544] Allocate memory for largest CRC used by NvM.
    NvM_Prv_Crc_tun CrcRamBlk_un;

} NvM_Prv_Job_CrcData_tst;

/**
 * Definition of the structure for the result of a NvM job.
 */
typedef struct
{
    /// Result of the current job
    NvM_Prv_JobResult_ten Result_en;
    /// Detail error information of the current job
    NvM_Rb_ErrorInfoDetail_ten errorInfoDetail_en;
    /// Production errors detected during the current job
    uint8 ProductionError_u8;

    /// Counter for operations to copy user data into/from the NvM internal buffer during explicit synchronization.
    uint8 cntrExpSyncOperations_u8;

    /// Data for CRC calculation during the current job.
    NvM_Prv_Job_CrcData_tst CrcData_st;

#if (NVM_CRYPTO_USED == STD_ON)
    /// Counter for Csm job retries.
    uint8 cntrCsmJobRetry_uo;

    ///Results of the current job related to cryptographic services.
    NvM_Prv_Crypto_ServiceResult_tst Crypto_st;
#endif

} NvM_Prv_JobResult_tst;

/// Type definition of state functions for different NvM job state machines.
typedef NvM_Prv_idJobStateMachine_ten (*NvM_Prv_Job_State_tpfct)(NvM_Prv_stJob_ten* stJob_pen,
                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                 NvM_Prv_JobData_tst const* JobData_pcst);

/// Type definition of state machine functions for different NvM jobs.
typedef NvM_Prv_Job_State_tpfct (*NvM_Prv_Job_StateMachine_tpfct)(NvM_Prv_stJob_ten stJob_en);

/// Type definition of functions to get activation status of a job state machine.
typedef boolean (*NvM_Prv_Job_StateMachine_IsEnabled_tpfct)(NvM_Prv_JobData_tst const* JobData_pcst);

/**
 * Definition of the structure for the data required for execution of job state machines.
 */
typedef struct
{
    NvM_Prv_Job_StateMachine_tpfct              StateMachine_pfct;
    NvM_Prv_Job_StateMachine_IsEnabled_tpfct    IsEnabled_pfct;
    NvM_Prv_stJob_ten                           stInitial_en;
    NvM_Prv_stJob_ten                           stFinal_en;
    NvM_Prv_JobResource_Cluster_ten             RequiredResourceCluster_en;

} NvM_Prv_Job_StateMachine_Data_tst;

/// Type definition of a function to get all data required for job state machine execution.
typedef NvM_Prv_Job_StateMachine_Data_tst const* (*NvM_Prv_Job_StateMachine_GetData_tpfct)(void);


/* NVM_PRV_JOBSTATEMACHINE_TYPES_H */
#endif

