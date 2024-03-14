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
#ifndef NVM_PRV_JOB_TYPES_H
#define NVM_PRV_JOB_TYPES_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_JobStack_Types.h"

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/**
 * This macro generates lists where each entry corresponds to a job provided by NvM.
 *
 * Each entry of the macro shall look like follows:
 *
 * ENTRY(Name, InitialStateMachine, ResultEvaluation)
 *
 * with:
 * Name:                name of the job for which the entry is generated
 * InitialStateMachine: initial state machine of the job for which the entry is generated
 * ResultEvaluation:    structure with pointers to all functions required to evaluate job results
 */
#define LIST_JOB(ENTRY)                                                                                     \
        ENTRY(Idle,                            Invalid,                    NoEvaluation)                    \
        ENTRY(ReadRecovery,                    DoReadRecovery,             ReadRecovery)                    \
        ENTRY(Write,                           DoWrite,                    Write)                           \
        ENTRY(Erase,                           DoInvalidate,               Invalidate)                      \
        ENTRY(Restore,                         DoRestore,                  Restore)                         \
        ENTRY(Maintain,                        DoMemIf_Maintain,           NoEvaluation)                    \
        ENTRY(Validate,                        DoValidate,                 Validate)                        \
        ENTRY(Invalidate,                      DoInvalidate,               Invalidate)                      \
        ENTRY(ReadIdConfigForReadAll,          DoRead,                     ReadIdConfigForReadAll)          \
        ENTRY(InvalidateForFirstInitAll,       DoInvalidateAll,            Invalidate)                      \
        ENTRY(InvalidateForRemoveNonResistant, DoInvalidateAll,            InvalidateForRemoveNonResistant) \
        ENTRY(RecalcRamBlkCrc,                 DoRecalcRamBklCrc,          RecalcRamBlkCrc)                 \
        ENTRY(WriteAll,                        DoWrite,                    WriteAll)                        \
        ENTRY(ReadAllExtPreparation,           DoRestore,                  ReadAllExtPreparation)           \
        ENTRY(GenerateRandomNr,                DoCryptoGenerateRandomNr,   NoEvaluation)                    \
        ENTRY(RestoreWrite,                    DoRestoreWrite,             RestoreWrite)                    \
        ENTRY(Suspend,                         Invalid,                    NoEvaluation)                    \
        ENTRY(Invalid,                         Invalid,                    NoEvaluation)

#define LIST_JOB_ENTRY_ID(Name, InitialStateMachine, ResultEvaluation)  NvM_Prv_idJob_##Name##_e,

// Enumeration of NvM-internal job IDs
// TRACE[NVM507] Private type added to public NvM_Types.h to allow AUTOSAR debugging
typedef enum
{
    /* MR12 RULE 20.7 VIOLATION: To create the list the argument shall be not parenthesized */
    LIST_JOB(LIST_JOB_ENTRY_ID)

    //-------------------------------------------------------------------------
    // This is actually no job ID,
    // in fact this enum value defines count of jobs available in NvM and
    // can be used for array definition
    //-------------------------------------------------------------------------
    NvM_Prv_idJob_Count_e

} NvM_Prv_idJob_ten;

/**
 * Definition of the structure containing all data required for a NvM job.
 */
typedef struct
{
    /// ID of the job to be executed by the NvM (NVM_PRV_JOB_ID_IDLE if no job shall be executed)
    NvM_Prv_idJob_ten idJob_en;

    /// All results of the NvM job
    NvM_Prv_JobResult_tst JobResult_st;

    /// All data required to process the NvM job
    NvM_Prv_JobData_tst JobData_st;

    NvM_Prv_JobStack_tst JobStack_st;

} NvM_Prv_Job_tst;

/// Type definition for function to cancel a job working on a job resource.
typedef boolean (*NvM_Prv_Job_Cancel_tpfct)(NvM_Prv_Job_tst* Job_pst);

/* NVM_PRV_JOB_TYPES_H */
#endif

