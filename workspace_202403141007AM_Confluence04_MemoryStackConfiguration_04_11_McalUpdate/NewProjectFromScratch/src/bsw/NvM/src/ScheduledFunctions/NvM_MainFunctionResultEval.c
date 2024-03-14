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
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM.h"
#include "NvM_Cfg_SchM.h"

#include "rba_MemLib_Special.h"

#include "NvM_Prv.h"
#include "NvM_Prv_Crc.h"
#include "NvM_Prv_JobStack.h"
#include "NvM_Prv_JobQueue.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_JobResource.h"
#include "NvM_Prv_ErrorDetection.h"
#include "NvM_Prv_ProcessMultiBlock.h"
/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
typedef NvM_RequestResultType (*NvM_Prv_Job_EvalReqResult_tpfct)(NvM_Prv_Job_tst const* Job_pcst);
typedef void (*NvM_Prv_Job_EvalProductionErrors_tpfct)(NvM_Prv_Job_tst* Job_pst);
typedef void (*NvM_Prv_Job_UpdateBlockStatus_tpfct)(NvM_Prv_Job_tst const* Job_pcst);

typedef struct
{
    NvM_Prv_Job_EvalReqResult_tpfct         EvaluateRequestResult_pfct;
    NvM_Prv_Job_EvalProductionErrors_tpfct  EvaluateProductionErrors_pfct;
    NvM_Prv_Job_UpdateBlockStatus_tpfct     UpdateBlockStatus_pfct;

} NvM_Prv_Job_ResultEvaluation_tst;

/*
 **********************************************************************************************************************
 * Local declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC NvM_RequestResultType NvM_Prv_EvalReqResult(NvM_Prv_Job_tst const* Job_pcst);
RBA_MEMLIB_STATIC NvM_RequestResultType NvM_Prv_EvalReqResult_Read(NvM_Prv_Job_tst const* Job_pcst);
RBA_MEMLIB_STATIC NvM_RequestResultType NvM_Prv_EvalReqResult_InvalidateForRemoveNonResistant(NvM_Prv_Job_tst const* Job_pcst);
RBA_MEMLIB_STATIC NvM_RequestResultType NvM_Prv_EvalReqResult_ReadAllExtPreparation(NvM_Prv_Job_tst const* Job_pcst);
RBA_MEMLIB_STATIC NvM_RequestResultType NvM_Prv_EvalReqResult_ReadIdConfigForReadAll(NvM_Prv_Job_tst const* Job_pcst);

RBA_MEMLIB_STATIC void NvM_Prv_EvalProductionErrors(NvM_Prv_Job_tst* Job_pst);
RBA_MEMLIB_STATIC void NvM_Prv_EvalProductionErrors_Read(NvM_Prv_Job_tst* Job_pst);
RBA_MEMLIB_STATIC void NvM_Prv_EvalProductionErrors_ReadIdConfigForReadAll(NvM_Prv_Job_tst* Job_pst);

RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatus_Restore(NvM_Prv_Job_tst const* Job_pcst);
RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatus_Read(NvM_Prv_Job_tst const* Job_pcst);
RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatus_Write(NvM_Prv_Job_tst const* Job_pcst);
RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatus_WriteAll(NvM_Prv_Job_tst const* Job_pcst);
RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatusJobValidate(NvM_Prv_Job_tst const* Job_pcst);
RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatusIdConfigForReadAll(NvM_Prv_Job_tst const* Job_pcst);
RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatus_RecalcRamBlkCrc(NvM_Prv_Job_tst const* Job_pcst);

RBA_MEMLIB_STATIC NvM_Prv_Job_EvalReqResult_tpfct NvM_Prv_GetFctJobEvalReqResult(NvM_Prv_idJob_ten idJob_en);
RBA_MEMLIB_STATIC NvM_Prv_Job_EvalProductionErrors_tpfct NvM_Prv_GetFctJobEvalProductionErrors(NvM_Prv_idJob_ten idJob_en);
RBA_MEMLIB_STATIC NvM_Prv_Job_UpdateBlockStatus_tpfct NvM_Prv_GetFctJobUpdateBlockStatus(NvM_Prv_idJob_ten idJob_en);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
 */

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_MainFunctionResultEval_EvaluateJob(NvM_Prv_Job_tst* Job_pst,
                                                                        NvM_Prv_MainStates_tst* MainStates_pst,
                                                                        NvM_Prv_Queue_idx_tst const* idxJob_pcst);
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_MainFunctionResultEval_EvalRequestResults(NvM_Prv_Job_tst* Job_pst,
                                                                                  NvM_RequestResultType* stReqResult_puo,
                                                                                  boolean isJobResultPlausible_b);
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_MainFunctionResultEval_UpdateBlkStatus(NvM_Prv_Job_tst const* Job_pcst,
                                                                            boolean isJobResultPlausible_b);
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_MainFunctionResultEval_FinalBlockCallbacks(NvM_RequestResultType stRequestResult_uo,
                                                                                NvM_BlockIdType idActiveBlock_uo,
                                                                                NvM_Prv_idService_tuo idActiveService_uo,
                                                                                boolean IsAuxServiceActive_b);
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_MainFunctionResultEval_ResetRequests(NvM_BlockIdType idBlock_uo,
                                                                          NvM_Prv_idService_tuo idService_uo,
                                                                          NvM_Prv_ServiceBit_tuo ServiceBit_uo,
                                                                          NvM_RequestResultType stRequestResult_uo);
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_MainFunctionResultEval_SuspendJobUsingCanceledCsmJob(NvM_Prv_Job_tst const* Job_pcst,
                                                                                             NvM_Prv_Queue_idx_tst const* idxJob_pcst);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_NoEvaluation_cst =
{
        NULL_PTR,                           // EvaluateRequestResult_pfct
        NULL_PTR,                           // EvaluateProductionErrors_pfct
        NULL_PTR                            // UpdateBlockStatus_pfct
};

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_ReadRecovery_cst =
{
        &NvM_Prv_EvalReqResult_Read,        // EvaluateRequestResult_pfct
        &NvM_Prv_EvalProductionErrors_Read, // EvaluateProductionErrors_pfct
        &NvM_Prv_UpdateBlockStatus_Read     // UpdateBlockStatus_pfct
};

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_Write_cst =
{
        &NvM_Prv_EvalReqResult,             // EvaluateRequestResult_pfct
        &NvM_Prv_EvalProductionErrors,      // EvaluateProductionErrors_pfct
        &NvM_Prv_UpdateBlockStatus_Write    // UpdateBlockStatus_pfct
};

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_Restore_cst =
{
        &NvM_Prv_EvalReqResult,             // EvaluateRequestResult_pfct
        &NvM_Prv_EvalProductionErrors,      // EvaluateProductionErrors_pfct
        &NvM_Prv_UpdateBlockStatus_Restore  // UpdateBlockStatus_pfct
};

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_Validate_cst =
{
        &NvM_Prv_EvalReqResult,                 // EvaluateRequestResult_pfct
        &NvM_Prv_EvalProductionErrors,          // EvaluateProductionErrors_pfct
        &NvM_Prv_UpdateBlockStatusJobValidate   // UpdateBlockStatus_pfct
};

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_Invalidate_cst =
{
        &NvM_Prv_EvalReqResult,                 // EvaluateRequestResult_pfct
        &NvM_Prv_EvalProductionErrors,          // EvaluateProductionErrors_pfct
        NULL_PTR                                // UpdateBlockStatus_pfct
};

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_ReadIdConfigForReadAll_cst =
{
        &NvM_Prv_EvalReqResult_ReadIdConfigForReadAll,          // EvaluateRequestResult_pfct
        &NvM_Prv_EvalProductionErrors_ReadIdConfigForReadAll,   // EvaluateProductionErrors_pfct
        &NvM_Prv_UpdateBlockStatusIdConfigForReadAll            // UpdateBlockStatus_pfct
};

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_InvalidateForRemoveNonResistant_cst =
{
        &NvM_Prv_EvalReqResult_InvalidateForRemoveNonResistant, // EvaluateRequestResult_pfct
        &NvM_Prv_EvalProductionErrors,                          // EvaluateProductionErrors_pfct
        NULL_PTR                                                // UpdateBlockStatus_pfct
};

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_RecalcRamBlkCrc_cst =
{
        NULL_PTR,                                   // EvaluateRequestResult_pfct
        NULL_PTR,                                   // EvaluateProductionErrors_pfct
        &NvM_Prv_UpdateBlockStatus_RecalcRamBlkCrc  // UpdateBlockStatus_pfct
};

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_WriteAll_cst =
{
        &NvM_Prv_EvalReqResult,                 // EvaluateRequestResult_pfct
        &NvM_Prv_EvalProductionErrors,          // EvaluateProductionErrors_pfct
        &NvM_Prv_UpdateBlockStatus_WriteAll     // UpdateBlockStatus_pfct
};

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_ReadAllExtPreparation_cst =
{
        &NvM_Prv_EvalReqResult_ReadAllExtPreparation,   // EvaluateRequestResult_pfct
        &NvM_Prv_EvalProductionErrors_Read,             // EvaluateProductionErrors_pfct
        &NvM_Prv_UpdateBlockStatus_Restore              // UpdateBlockStatus_pfct
};

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const NvM_Prv_Job_RestoreWrite_cst =
{
        &NvM_Prv_EvalReqResult,             // EvaluateRequestResult_pfct
        &NvM_Prv_EvalProductionErrors,      // EvaluateProductionErrors_pfct
        &NvM_Prv_UpdateBlockStatus_Write    // UpdateBlockStatus_pfct
};

/**
 * Function-like macro: LIST_JOB_ENTRY_RESULT_EVALUATION
 *
 * This macro generates an entry for a structure with all functions required to evaluate results in a list
 * of all jobs provided by NvM.
 * S.a. macro LIST_JOB to generate a list for all jobs provided by NvM
 *
 * Paramater:
 * Name:                unused
 * InitialStateMachine: unused
 * ResultEvaluation:    structure with pointers to all functions required to evaluate job results
 */
#define LIST_JOB_ENTRY_RESULT_EVALUATION(Name, InitialStateMachine, ResultEvaluation)   &NvM_Prv_Job_##ResultEvaluation##_cst,

RBA_MEMLIB_STATIC NvM_Prv_Job_ResultEvaluation_tst const* const NvM_Prv_Job_ResultEvaluation_capcst[NvM_Prv_idJob_Count_e] =
{
    /* MR12 RULE 20.7 VIOLATION: To create the list the argument shall be not parenthesized */
    LIST_JOB(LIST_JOB_ENTRY_RESULT_EVALUATION)
};


#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * NvM-private code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC NvM_RequestResultType NvM_Prv_EvalReqResult(NvM_Prv_Job_tst const* Job_pcst)
{
    NvM_RequestResultType stRequestResult_uo;

    // TRACE[SWS_NvM_00809] Write job for NvM_WritePRAMBlock shall set request result NVM_REQ_OK
    //                      after successful writing of redundant block
    // TRACE[SWS_NvM_00820] Read job for NvM_RestorePRAMBlockDefaults  shall set request result NVM_REQ_OK
    //                      after successful restoring
    if (    (NvM_Prv_JobResult_Succeeded_e == Job_pcst->JobResult_st.Result_en)
         || (NvM_Prv_JobResult_Succeeded_MemIfSkipped_e == Job_pcst->JobResult_st.Result_en) )
    {
        stRequestResult_uo = NVM_REQ_OK;
    }
    else if (NvM_Prv_JobResult_Skipped_e == Job_pcst->JobResult_st.Result_en)
    {
        stRequestResult_uo = NVM_REQ_BLOCK_SKIPPED;
    }
    else
    {
        stRequestResult_uo = NVM_REQ_NOT_OK;
    }
    return stRequestResult_uo;
}

RBA_MEMLIB_STATIC NvM_RequestResultType NvM_Prv_EvalReqResult_Read(NvM_Prv_Job_tst const* Job_pcst)
{
    NvM_RequestResultType stRequestResult_uo;
    // TRACE[SWS_NvM_00775] Read job for NvM_ReadPRAMBlock shall set request result NVM_REQ_OK
    //                      after successful reading
    // TRACE[SWS_NvM_00782] Read job for NvM_ReadPRAMBlock shall set request result NVM_REQ_OK
    //                      after successful reading of redundant block
    if (    (NvM_Prv_JobResult_Succeeded_e == Job_pcst->JobResult_st.Result_en)
         || (NvM_Prv_JobResult_Succeeded_MemIfSkipped_e == Job_pcst->JobResult_st.Result_en) )
    {
        stRequestResult_uo = NVM_REQ_OK;
    }
    // TRACE[SWS_NvM_00770] Read job for NvM_ReadPRAMBlock shall not copy NV block to the RAM block
    //                      if dataset block is invalidated
    // TRACE[SWS_NvM_00776] Read job for NvM_ReadPRAMBlock shall set request result NVM_REQ_NV_INVALIDATED
    //                      if read block is invalidated
    // TRACE[SWS_NvM_00785] Read job for NvM_ReadPRAMBlock shall set request result NVM_REQ_NV_INVALIDATED
    //                      if both NV blocks of redundant block are invalidated
    else if (NvM_Prv_JobResult_BlockInvalidated_e == Job_pcst->JobResult_st.Result_en)
    {
        stRequestResult_uo = NVM_REQ_NV_INVALIDATED;
    }
    // TRACE[SWS_NvM_00882] Read job for NvM_ReadPRAMBlock shall set request result NVM_REQ_RESTORED_FROM_ROM
    //                      if loading default values has succeeded
    else if (NvM_Prv_JobResult_BlockDefault_e == Job_pcst->JobResult_st.Result_en)
    {
        stRequestResult_uo = NVM_REQ_RESTORED_FROM_ROM;
    }
    // TRACE[SWS_NvM_00771] Read job for NvM_ReadPRAMBlock shall not copy NV block to the RAM block
    //                      if dataset block is inconsistent
    // TRACE[SWS_NvM_00778] Read job for NvM_ReadPRAMBlock shall set request result NVM_REQ_INTEGRITY_FAILED
    //                      if read block is inconsistent
    else if (NvM_Prv_JobResult_BlockInconsistent_e == Job_pcst->JobResult_st.Result_en)
    {
        stRequestResult_uo = NVM_REQ_INTEGRITY_FAILED;
    }
    else if (NvM_Prv_JobResult_Skipped_e == Job_pcst->JobResult_st.Result_en)
    {
        stRequestResult_uo = NVM_REQ_BLOCK_SKIPPED;
    }
    // TRACE[SWS_NvM_00780] Read job for NvM_ReadPRAMBlock shall set request result NVM_REQ_NOT_OK
    //                      if reading has failed
    else
    {
        stRequestResult_uo = NVM_REQ_NOT_OK;
    }
    return stRequestResult_uo;
}

RBA_MEMLIB_STATIC NvM_RequestResultType NvM_Prv_EvalReqResult_InvalidateForRemoveNonResistant(NvM_Prv_Job_tst const* Job_pcst)
{
    NvM_RequestResultType stRequestResult_uo;
    if (NvM_Prv_JobResult_Succeeded_e == Job_pcst->JobResult_st.Result_en)
    {
        stRequestResult_uo = NVM_REQ_NV_INVALIDATED;
    }
    else
    {
        stRequestResult_uo = NVM_REQ_NOT_OK;
    }
    return stRequestResult_uo;
}

RBA_MEMLIB_STATIC NvM_RequestResultType NvM_Prv_EvalReqResult_ReadAllExtPreparation(NvM_Prv_Job_tst const* Job_pcst)
{
    NvM_RequestResultType stRequestResult_uo;
    if (NvM_Prv_JobResult_Succeeded_e == Job_pcst->JobResult_st.Result_en)
    {
        // TRACE[SWS_NvM_00308] Set result NVM_REQ_OK if restoring default data has succeeded
        stRequestResult_uo = NVM_REQ_OK;
    }
    else
    {
        // TRACE[SWS_NvM_00968] Set result NVM_REQ_INTEGRITY_FAILED if there is no default data
        //                      (no ROM block and (no initialization callback or initialization callback has failed))
        // This requirement is defined in AR4.6, in older AR versions there is no explicit request result
        // if user has not configured default data
        stRequestResult_uo = NVM_REQ_INTEGRITY_FAILED;
    }

    return stRequestResult_uo;
}

/**
 * \brief
 * This function evaluates the request result if the configuration ID has been read for ReadAll service.
 *
 * \details
 * The request result depends on the read job result and the read value of the configuration ID:
 * - reading configuration ID has succeeded AND configuration ID has not changed -> NVM_REQ_OK
 * - reading configuration ID has succeeded AND configuration ID has changed -> NVM_REQ_NOT_OK
 * - invalidated configuration ID has been read (someone has intentionally invalidated it) -> NVM_REQ_NV_INVALIDATED
 * - inconsistent configuration ID has been read
 *   (configuration ID has got corrupted in NV memory or was never written into the NV memory) -> NVM_REQ_NV_INVALIDATED
 * - reading configuration ID has failed -> NVM_REQ_INTEGRITY_FAILED
 * - all other job results -> NVM_REQ_NOT_OK
 *
 * If the configuration ID was not read successfully then this function sets it to trigger special actions.
 * Following scenarios are possible:
 * - set configuration ID to value different from compiled value (s. NVM_COMPILED_CONFIG_ID)
 *   so that NvM will overwrite configuration ID in non-volatile memory, this is required in following cases:
 *   - inconsistent configuration ID has been read
 *   - reading configuration ID has failed
 * - set configuration ID to compiled value (s. NVM_COMPILED_CONFIG_ID) so that NvM will not overwrite configuration ID
 *   in non-volatile memory, this is required in following cases:
 *   - invalidated configuration ID has been read
 *   - all other job results
 *
 * If configured then this function initiates the internal multi-block service for block initialization
 * after a ReadAll service.
 *
 * \param JobData_pcst
 * Pointer to the structure with job data.
 * The caller has to make sure that this pointer is valid and points to plausible data.
 */
RBA_MEMLIB_STATIC NvM_RequestResultType NvM_Prv_EvalReqResult_ReadIdConfigForReadAll(NvM_Prv_Job_tst const* Job_pcst)
{
    NvM_RequestResultType stRequestResult_uo;

    if (   (NvM_Prv_JobResult_Succeeded_e == Job_pcst->JobResult_st.Result_en)
        || (NvM_Prv_JobResult_Succeeded_MemIfSkipped_e == Job_pcst->JobResult_st.Result_en) )
    {
        if (NvM_Prv_Block_HasIdConfigChanged())
        {
            stRequestResult_uo = NVM_REQ_NOT_OK;
        }
        else
        {
            stRequestResult_uo = NVM_REQ_OK;
        }
    }
    else if (NvM_Prv_JobResult_BlockInvalidated_e == Job_pcst->JobResult_st.Result_en)
    {
        stRequestResult_uo = NVM_REQ_NV_INVALIDATED;
        // set ConfigID to compiled value to signal that ConfigId has not changed and to avoid writing it
        NvM_Prv_Block_SetIdConfig(NVM_COMPILED_CONFIG_ID);
    }
    else if (NvM_Prv_JobResult_BlockInconsistent_e == Job_pcst->JobResult_st.Result_en)
    {
        stRequestResult_uo = NVM_REQ_NV_INVALIDATED;
        // set ConfigID to signal that ConfigId has changed and to trigger writing it
        NvM_Prv_Block_SetIdConfig((uint16)(~((uint16)NVM_COMPILED_CONFIG_ID)));
    }
    else if (NvM_Prv_JobResult_Failed_e == Job_pcst->JobResult_st.Result_en)
    {
        stRequestResult_uo = NVM_REQ_INTEGRITY_FAILED;
        // set ConfigID to signal that ConfigId has changed and to trigger writing it
        NvM_Prv_Block_SetIdConfig((uint16)(~((uint16)NVM_COMPILED_CONFIG_ID)));
    }
    else
    {
        stRequestResult_uo = NVM_REQ_NOT_OK;
        // set ConfigID to compiled value to signal that ConfigId has not changed and to avoid writing it
        NvM_Prv_Block_SetIdConfig(NVM_COMPILED_CONFIG_ID);
    }

#if ((NVM_DYNAMIC_CONFIGURATION == STD_ON) || (NVM_RB_INIT_AT_LAYOUT_CHANGE == STD_ON))
    NvM_Prv_InitAfterReadAll(Job_pcst->JobResult_st.Result_en);
#endif

    return stRequestResult_uo;
}

RBA_MEMLIB_STATIC void NvM_Prv_EvalProductionErrors(NvM_Prv_Job_tst* Job_pst)
{
    if (   (NvM_Prv_JobResult_Succeeded_e != Job_pst->JobResult_st.Result_en)
        && (NvM_Prv_JobResult_Succeeded_MemIfSkipped_e != Job_pst->JobResult_st.Result_en) )
    {
        Job_pst->JobResult_st.ProductionError_u8 |= NVM_E_RB_REQ_FAILED;
    }
}

RBA_MEMLIB_STATIC void NvM_Prv_EvalProductionErrors_Read(NvM_Prv_Job_tst* Job_pst)
{
    if (NvM_Prv_JobResult_BlockInconsistent_e == Job_pst->JobResult_st.Result_en)
    {
        Job_pst->JobResult_st.ProductionError_u8 |= NVM_E_RB_INTEGRITY_FAILED;
    }
    else if (NvM_Prv_JobResult_Failed_e == Job_pst->JobResult_st.Result_en)
    {
        Job_pst->JobResult_st.ProductionError_u8 |= NVM_E_RB_REQ_FAILED;
    }
    else
    {
        // do nothing
    }
}

RBA_MEMLIB_STATIC void NvM_Prv_EvalProductionErrors_ReadIdConfigForReadAll(NvM_Prv_Job_tst* Job_pst)
{
    if (NvM_Prv_JobResult_BlockInconsistent_e == Job_pst->JobResult_st.Result_en)
    {
        // Production error NVM_E_RB_INTEGRITY_FAILED which could be entered by the read job
        // is reset here due to the requirement SWS_NvM_00306 to avoid its reporting.
        Job_pst->JobResult_st.ProductionError_u8 &= (~NVM_E_RB_INTEGRITY_FAILED);
    }
    else if (NvM_Prv_JobResult_Failed_e == Job_pst->JobResult_st.Result_en)
    {
        Job_pst->JobResult_st.ProductionError_u8 |= NVM_E_RB_REQ_FAILED;
    }
    else
    {
        // do nothing
    }
}

RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatus_Restore(NvM_Prv_Job_tst const* Job_pcst)
{
    // TRACE[SWS_NvM_00820] Restore job for NvM_RestorePRAMBlockDefaults  shall set the RAM block to valid and changed
    //                      after successful restoring default data
    if (NvM_Prv_JobResult_Succeeded_e == Job_pcst->JobResult_st.Result_en)
    {
        if (Job_pcst->JobData_st.isPRamBlockUsed_b)
        {
            uint8 maskBitsToChange_u8 =   NVM_BLOCK_STATE_RAM_VALID
                                        | NVM_BLOCK_STATE_RAM_CHANGED
                                        | NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ
                                        | NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ;

            uint8 maskBitsNewValue_u8 =   NVM_BLOCK_STATE_RAM_VALID
                                        | NVM_BLOCK_STATE_RAM_CHANGED
                                        | NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ;

            NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, maskBitsToChange_u8, maskBitsNewValue_u8);
        }
    }
}

RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatus_Read(NvM_Prv_Job_tst const* Job_pcst)
{
    if (NvM_Prv_JobResult_Succeeded_e == Job_pcst->JobResult_st.Result_en)
    {
        if (Job_pcst->JobData_st.isPRamBlockUsed_b)
        {

            uint8 maskBitsToChange_u8 =   NVM_BLOCK_STATE_RAM_VALID
                                        | NVM_BLOCK_STATE_RAM_CHANGED
                                        | NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ
                                        | NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ;

            uint8 maskBitsNewValue_u8 =   NVM_BLOCK_STATE_RAM_VALID;

            NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, maskBitsToChange_u8, maskBitsNewValue_u8);
        }

        if (NvM_Prv_BlkDesc_IsBlockSelected(Job_pcst->JobData_st.idBlock_uo, NVM_PRV_BLOCK_FLAG_WRITE_ONCE))
        {
            NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, NVM_BLOCK_STATE_WRITE_PROT, NVM_BLOCK_STATE_WRITE_PROT);
        }
    }
    else if (NvM_Prv_JobResult_Succeeded_MemIfSkipped_e == Job_pcst->JobResult_st.Result_en)
    {
        // After a soft-reset if data has survived for a block with RAM CRC the read job skips reading from
        // media if CRC is consistent. In this case the Changed-flag and WriteReq-flag shall not be reset in
        // order to allow the writing of the block later.
        if (Job_pcst->JobData_st.isPRamBlockUsed_b)
        {

            uint8 maskBitsToChange_u8 =   NVM_BLOCK_STATE_RAM_VALID
                                        | NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ;

            uint8 maskBitsNewValue_u8 =   NVM_BLOCK_STATE_RAM_VALID;

            NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, maskBitsToChange_u8, maskBitsNewValue_u8);
        }

        // In case that no access to medium has been done write protection must not be activated for
        // NVM_PRV_BLOCK_FLAG_WRITE_ONCE blocks
    }
    else if (NvM_Prv_JobResult_BlockDefault_e == Job_pcst->JobResult_st.Result_en)
    {
        // TRACE[SWS_NvM_00774] Restore job for NvM_ReadPRAMBlock shall set the RAM block to valid and changed
        //                      after successful restoring default data
        if (Job_pcst->JobData_st.isPRamBlockUsed_b)
        {
            uint8 maskBitsToChange_u8 =   NVM_BLOCK_STATE_RAM_VALID
                                        | NVM_BLOCK_STATE_RAM_CHANGED
                                        | NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ
                                        | NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ;

            uint8 maskBitsNewValue_u8 =   NVM_BLOCK_STATE_RAM_VALID
                                        | NVM_BLOCK_STATE_RAM_CHANGED
                                        | NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ;

            NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, maskBitsToChange_u8, maskBitsNewValue_u8);
        }
    }
    else
    {
        // nothing to do
    }
    NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, NVM_BLOCK_STATE_READ_DONE, NVM_BLOCK_STATE_READ_DONE);
}

RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatus_Write(NvM_Prv_Job_tst const* Job_pcst)
{
    if (NvM_Prv_JobResult_Succeeded_e == Job_pcst->JobResult_st.Result_en)
    {
        if (Job_pcst->JobData_st.isPRamBlockUsed_b)
        {

            uint8 maskBitsToChange_u8 =   NVM_BLOCK_STATE_RAM_VALID
                                        | NVM_BLOCK_STATE_RAM_CHANGED
                                        | NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ
                                        | NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ;

            uint8 maskBitsNewValue_u8 =   NVM_BLOCK_STATE_RAM_VALID;

            NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, maskBitsToChange_u8, maskBitsNewValue_u8);
        }

        if (NvM_Prv_BlkDesc_IsBlockSelected(Job_pcst->JobData_st.idBlock_uo, NVM_PRV_BLOCK_FLAG_WRITE_ONCE))
        {
            NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, NVM_BLOCK_STATE_WRITE_PROT, NVM_BLOCK_STATE_WRITE_PROT);
        }
    }
    else if (NvM_Prv_JobResult_Succeeded_MemIfSkipped_e == Job_pcst->JobResult_st.Result_en)
    {
        // Write Job has skipped writing to media because of CRC compare mechanism
        if (Job_pcst->JobData_st.isPRamBlockUsed_b)
        {

            uint8 maskBitsToChange_u8 =   NVM_BLOCK_STATE_RAM_VALID
                                        | NVM_BLOCK_STATE_RAM_CHANGED
                                        | NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ
                                        | NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ;

            uint8 maskBitsNewValue_u8 =   NVM_BLOCK_STATE_RAM_VALID;

            NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, maskBitsToChange_u8, maskBitsNewValue_u8);
        }
    }
    else
    {
        // nothing to do
    }

}

RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatus_WriteAll(NvM_Prv_Job_tst const* Job_pcst)
{
    NvM_Prv_UpdateBlockStatus_Write(Job_pcst);

#if (NVM_RB_SET_WRITEALL_TRIGGER_API == STD_ON)
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2980]
    // Set the flag back to the init state after finishing the WriteAll job for the current block
    NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, NVM_RB_BLOCK_STATE_MASK_TRG_WRITEALL, 0u);
#endif
}

RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatusJobValidate(NvM_Prv_Job_tst const* Job_pcst)
{
    if (NvM_Prv_JobResult_Succeeded_e == Job_pcst->JobResult_st.Result_en)
    {
        uint8 maskBitsToChange_u8;
        uint8 maskBitsNewValue_u8;

        if (NvM_Prv_BlkDesc_IsBlockSelected(Job_pcst->JobData_st.idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_RAM_CRC))
        {
            maskBitsToChange_u8 =   NVM_BLOCK_STATE_RAM_CHANGED
                                  | NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ;

            maskBitsNewValue_u8 =   NVM_BLOCK_STATE_RAM_CHANGED
                                  | NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ;
        }
        else
        {
            maskBitsToChange_u8 =   NVM_BLOCK_STATE_RAM_CHANGED;

            maskBitsNewValue_u8 =   NVM_BLOCK_STATE_RAM_CHANGED;
        }

        NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, maskBitsToChange_u8, maskBitsNewValue_u8);
    }
}

RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatusIdConfigForReadAll(NvM_Prv_Job_tst const* Job_pcst)
{
    if (NvM_Prv_Block_InitIdConfigDuringWriteAll())
    {
        // TRACE[NVM310] Mark the config ID block as VALID/CHANGED to enable its writing during WriteAll
        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2901] Signal the writing of the config ID block in WriteAll
        //                                               when dynamic conf is active and extended dynamic conf is inactive
        NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo,
                               NVM_BLOCK_STATE_RAM_MASK,
                               NVM_BLOCK_STATE_RAM_VALID_CHANGED);
    }
    NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, NVM_BLOCK_STATE_READ_DONE, NVM_BLOCK_STATE_READ_DONE);
}

/**
 * \brief
 * This local private function updates status of the block used in jobs to recalculate the CRC over the data
 * in the permanent RAM block.
 * \details
 *
 * \attention
 * NvM invokes this function with enabled scheduler lock. In worst case the duration of the scheduler lock corresponds
 * to 16 copy-operations:
 * - 1 for function call
 * - 1 for internal copy operations
 * - NvM_Prv_Block_IsRamCrcRecalcOngoing = 1 (see description of NvM_Prv_Block_IsRamCrcRecalcOngoing)
 * - NvM_Prv_Crc_CheckRamBlockCrc = 6 (see description of NvM_Prv_Crc_CheckRamBlockCrc)
 * - NvM_Prv_Block_SetState = 1 (see description of NvM_Prv_Block_SetState)
 * - NvM_Prv_Crc_SetRamBlockCrc = 6 (see description of NvM_Prv_Crc_SetRamBlockCrc)
 *
 * \param[in] Job_pcst
 * Pointer to the data of the job to update block status
 * \param[in] isResultPlausible_b
 * Information whether current job result is plausible
 */
RBA_MEMLIB_STATIC void NvM_Prv_UpdateBlockStatus_RecalcRamBlkCrc(NvM_Prv_Job_tst const* Job_pcst)
{
    uint8 maskBitsToChange_u8;
    uint8 maskBitsNewValue_u8;

    // This check is to make sure that the application has not changed the data and called SetRamBlockStatus
    // in the mean time again. -> If still ongoing then SetRamBlockStatus() has not been called since start of
    // CRC recalculation.
    if (NvM_Prv_Block_IsRamCrcRecalcOngoing(Job_pcst->JobData_st.idBlock_uo))
    {
        // RAM block CRC recalculation was not interrupted by "another" NvM_SetRamBlockStatus() call
        if (NvM_Prv_JobResult_Succeeded_e == Job_pcst->JobResult_st.Result_en)
        {
            if (NvM_Prv_Crc_CheckRamBlockCrc(Job_pcst->JobData_st.idBlock_uo,
                                             &Job_pcst->JobResult_st.CrcData_st.CrcRamBlk_un))
            {
                // Data has not been changed (CRC identical) -> just end CRC recalc

                maskBitsToChange_u8 =   NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ
                                      | NVM_BLOCK_STATE_RAM_CRC_RECALC_ONGOING;

                maskBitsNewValue_u8 = 0u;
            }
            else
            {
                // Data has changed -> must be written during next write operation
                maskBitsToChange_u8 =   NVM_BLOCK_STATE_RAM_CRC_RECALC_ONGOING
                                      | NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ
                                      | NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ;

                maskBitsNewValue_u8 =   NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ;
            }

            NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, maskBitsToChange_u8, maskBitsNewValue_u8);
            // Saving of RAM Block CRC has to be done after CRC comparison (NvM_Prv_Crc_CheckRamBlockCrc())
            // => therefore it is beeing done here instead of doing it in NvM_JobRecalcRamBlkCrc.c.
            NvM_Prv_Crc_SetRamBlockCrc(Job_pcst->JobData_st.idBlock_uo,
                                       &Job_pcst->JobResult_st.CrcData_st.Calculation_st.Crc_un);
        }
        else if (NvM_Prv_JobResult_Failed_e == Job_pcst->JobResult_st.Result_en)
        {
            // Job Result failed -> Explicit Sync callback has failed (CRC recalculation calculation cannot fail)
            // If Explicit Sync callback fails, then status flags shall be set in the same way as if CRC calculation
            // finished and a data change has been detected. -> NvM shall shall write data during next NvM_WriteAll
            maskBitsToChange_u8 =   NVM_BLOCK_STATE_RAM_CRC_RECALC_ONGOING
                                  | NVM_BLOCK_STATE_RAM_CRC_RECALC_REQ
                                  | NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ;

            maskBitsNewValue_u8 =   NVM_BLOCK_STATE_RAM_CRC_WRITE_REQ;

            NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, maskBitsToChange_u8, maskBitsNewValue_u8);
            // Ram Block CRC is not set, as it was not possible to retrieve the data and calculate it
        }
        else
        {
            // is never reached
        }
    }

    return;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_EvalReqResult_tpfct NvM_Prv_GetFctJobEvalReqResult(NvM_Prv_idJob_ten idJob_en)
{
    if (idJob_en >= NvM_Prv_idJob_Count_e)
    {
        idJob_en = NvM_Prv_idJob_Invalid_e;
    }
    return NvM_Prv_Job_ResultEvaluation_capcst[idJob_en]->EvaluateRequestResult_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_EvalProductionErrors_tpfct NvM_Prv_GetFctJobEvalProductionErrors(NvM_Prv_idJob_ten idJob_en)
{
    if (idJob_en >= NvM_Prv_idJob_Count_e)
    {
        idJob_en = NvM_Prv_idJob_Invalid_e;
    }
    return NvM_Prv_Job_ResultEvaluation_capcst[idJob_en]->EvaluateProductionErrors_pfct;
}

/**
 * \brief
 * This private local function returns the function which shall be used to update the status of the block
 * used in the given job.
 *
 * \detailed
 * In cases this function is called with enabled scheduler lock the run-time of 2 can be used:
 * - 1 for function call
 * - 1 for internal copy-operations
 *
 * \param[in] idJob_en
 * ID of the job for which the function to update block status is required
 *
 * \return
 * Pointer to the function to update block status
 */
RBA_MEMLIB_STATIC NvM_Prv_Job_UpdateBlockStatus_tpfct NvM_Prv_GetFctJobUpdateBlockStatus(NvM_Prv_idJob_ten idJob_en)
{
    if (idJob_en >= NvM_Prv_idJob_Count_e)
    {
        idJob_en = NvM_Prv_idJob_Invalid_e;
    }
    return NvM_Prv_Job_ResultEvaluation_capcst[idJob_en]->UpdateBlockStatus_pfct;
}

/**
 * \brief
 * This private function evaluates results for all finished jobs in the NvM internal job queue.
 *
 * \param[inout] MainStates_pst
 * Pointer to the structure with all state information which is not block-specific.
 */
void NvM_Prv_MainFunctionResultEval(NvM_Prv_MainStates_tst* MainStates_pst)
{
    NvM_Prv_Queue_idx_tuo cntrJob_uo = 0u;
    NvM_Prv_Queue_idx_tuo nrJobs_uo = NvM_Prv_JobQueue_GetNrEntries();
    NvM_Prv_Queue_idx_tst idxJob_st;

    idxJob_st.idxPrev_uo = NVM_PRV_QUEUE_IDX_INVALID;

    while (cntrJob_uo < nrJobs_uo)
    {
        NvM_Prv_Job_tst* Job_pst = NvM_Prv_JobQueue_GetJob(cntrJob_uo);
        idxJob_st.idx_uo = NvM_Prv_JobQueue_GetIdxJob(cntrJob_uo);

        if (NvM_Prv_JobStack_IsEmpty(&Job_pst->JobStack_st))
        {
            if (!NvM_Prv_MainFunctionResultEval_SuspendJobUsingCanceledCsmJob(Job_pst, &idxJob_st))
            {
                NvM_Prv_MainFunctionResultEval_EvaluateJob(Job_pst, MainStates_pst, &idxJob_st);
            }
            nrJobs_uo = NvM_Prv_JobQueue_GetNrEntries();
        }
        else
        {
            ++cntrJob_uo;
            idxJob_st.idxPrev_uo = idxJob_st.idx_uo;
        }
    };
}


/*
 **********************************************************************************************************************
 * Inline code
 **********************************************************************************************************************
 */
/**
 * \brief
 * This local private function evaluates all results for a job.
 * \details
 * \attention
 * This function uses scheduler lock to dequeue a user request. In worst case the duration of the scheduler lock
 * corresponds to 26 copy-operations:
 * - NvM_Prv_MainFunctionResultEval_UpdateBlkStatus = 19 (see description of the function)
 * - NvM_Prv_Block_SetRequestResult = 1 (see description of the function)
 * - NvM_Prv_Block_SetErrorInfoDetail = 1 (see description of the function)
 * - NvM_Prv_ErrorDetection_SetProductionError = 3 (see description of the function)
 * - NvM_Prv_MainFunctionResultEval_ResetRequests = 2 (see description of the function)
 *
 * \param[inout] Job_pst
 * Pointer to the data of the job to be evaluated
 */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_MainFunctionResultEval_EvaluateJob(NvM_Prv_Job_tst* Job_pst,
                                                                        NvM_Prv_MainStates_tst* MainStates_pst,
                                                                        NvM_Prv_Queue_idx_tst const* idxJob_pcst)
{
    boolean isRequestResultEvaluated_b = FALSE;
    boolean isJobResultPlausible_b = FALSE;
    NvM_RequestResultType stRequestResult_uo = NVM_REQ_NOT_OK;

    NvM_Prv_idService_tuo idActiveService_uo = Job_pst->JobData_st.idService_uo;
    boolean isMultiServiceActive_b = Job_pst->JobData_st.isMultiServiceActive_b;
    NvM_BlockIdType idBlockForRequestResult_uo = Job_pst->JobData_st.idBlock_uo;
#if (NVM_RB_AUX_INTERFACE == STD_ON)
    if (Job_pst->JobData_st.isAuxServiceActive_b)
    {
        idBlockForRequestResult_uo = NVM_PRV_AUX_ADMIN_BLOCK;
    }

    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3027] Set migration result for auxiliary read
    NvM_Prv_SetAuxMigrationResult(idActiveService_uo, Job_pst->JobData_st.idBlock_uo);
#endif

    // We rely heavily on the fact that we know really all possible job results
    // -> check this assumption and report DET error if development error detection is enabled
    isJobResultPlausible_b = NvM_Prv_ErrorDetection_IsJobResultMemIfPlausible(NVM_SERVICE_ID_MAIN_FUNCTION,
                                                                              Job_pst->JobData_st.idBlock_uo,
                                                                              Job_pst->JobResult_st.Result_en);

    isRequestResultEvaluated_b = NvM_Prv_MainFunctionResultEval_EvalRequestResults(Job_pst,
                                                                                   &stRequestResult_uo,
                                                                                   isJobResultPlausible_b);

    SchM_Enter_NvM_Main();

    // Part to be done after result evaluation WITHIN scheduling lock
    NvM_Prv_MainFunctionResultEval_UpdateBlkStatus(Job_pst, isJobResultPlausible_b);

    if (isRequestResultEvaluated_b)
    {
        // Set global variables from local copies to update results/states for the user
        // TRACE[NVM175] Indicate the request result to the user
        NvM_Prv_Block_SetRequestResult(idBlockForRequestResult_uo, stRequestResult_uo);
        NvM_Prv_Block_SetErrorInfoDetail(idBlockForRequestResult_uo, Job_pst->JobResult_st.errorInfoDetail_en);
        NvM_Prv_ErrorDetection_SetProductionError(Job_pst->JobData_st.idBlock_uo,
                                                  Job_pst->JobResult_st.ProductionError_u8);
    }

    NvM_Prv_MainFunctionResultEval_ResetRequests(idBlockForRequestResult_uo,
                                                 idActiveService_uo,
                                                 Job_pst->JobData_st.ServiceBit_uo,
                                                 stRequestResult_uo);

    SchM_Exit_NvM_Main();

    // Part to be done after result evaluation WITHOUT scheduling lock
    if (isRequestResultEvaluated_b && (NvM_Prv_idJob_GenerateRandomNr_e != Job_pst->idJob_en))
    {
        // Invoke observer and singleblock callbacks if applicable
        NvM_Prv_MainFunctionResultEval_FinalBlockCallbacks(stRequestResult_uo,
                                                           Job_pst->JobData_st.idBlock_uo,
                                                           idActiveService_uo,
                                                           Job_pst->JobData_st.isAuxServiceActive_b);
    }

    // Free all resources claimed by current block
    NvM_Prv_JobResource_Unclaim(Job_pst->JobData_st.idBlock_uo);
    NvM_Prv_JobQueue_Dequeue(idxJob_pcst);

    if (NvM_Prv_JobQueue_IsEmpty())
    {
        if (!isMultiServiceActive_b)
        {
            MainStates_pst->idServiceActive_uo = NVM_SERVICE_ID_INIT;
            MainStates_pst->idQueueActive_uo = NvM_Prv_idQueue_nrQueues_e;
        }
    }
    else
    {
        Job_pst = NvM_Prv_JobQueue_GetJob(0u);
        MainStates_pst->idServiceActive_uo = Job_pst->JobData_st.idService_uo;
        MainStates_pst->idQueueActive_uo = Job_pst->JobData_st.idQueue_uo;
    }
}

/**
 * \brief
 * This local private function evaluates user request result dependent on the job results.
 * \details
 * This fuction evaluates user request result only if result of the current job is plausible.
 * Additionally this function evaluates internal production errors occured during the current job.
 *
 * \param[inout] Job_pst
 * Pointer to the data of the current job
 * \param[inout] stReqResult_puo
 * Pointer to the evaluated user request result
 * \param[in] isJobResultPlausible_b
 * Information whether the current job result is plausible
 *
 * \return
 * - true = User request result evaluated successfully and returned via parameter stReqResult_puo
 * - false = no user request result evaluated, this is possible if job result is not plausible or
 *           given job data belongs to invalid job
 */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_MainFunctionResultEval_EvalRequestResults(NvM_Prv_Job_tst* Job_pst,
                                                                                  NvM_RequestResultType* stReqResult_puo,
                                                                                  boolean isJobResultPlausible_b)
{
    boolean isReqResultEvaluated_b = FALSE;

    if (isJobResultPlausible_b)
    {
        // Pointer to the function to evaluate request results of the currently active job
        NvM_Prv_Job_EvalReqResult_tpfct JobEvalReqResult_pfct =
                NvM_Prv_GetFctJobEvalReqResult(Job_pst->idJob_en);
        // Pointer to the function to evaluate production errors occured during the currently active job
        NvM_Prv_Job_EvalProductionErrors_tpfct JobEvalProductionErrors_pfct =
                NvM_Prv_GetFctJobEvalProductionErrors(Job_pst->idJob_en);

        if (NULL_PTR != JobEvalReqResult_pfct)
        {
            *stReqResult_puo = JobEvalReqResult_pfct(Job_pst);
            isReqResultEvaluated_b = TRUE;

            // TRACE[NVM395] Maintain overall result of the multiblock operation
            if (Job_pst->JobData_st.isMultiServiceActive_b)
            {
                NvM_Prv_Multi_SetResult(*stReqResult_puo);
            }
        }
        if (NULL_PTR != JobEvalProductionErrors_pfct)
        {
            JobEvalProductionErrors_pfct(Job_pst);
        }
    }

    return isReqResultEvaluated_b;
}

/**
 * \brief
 * This local private function updates status of the block used in current job dependent on its results.
 * \details
 *
 * \attention
 * NvM invokes this function with enabled scheduler lock. In worst case the duration of the scheduler lock corresponds
 * to 19 copy-operations:
 * - NvM_Prv_GetFctJobUpdateBlockStatus = 2 (see description of NvM_Prv_GetFctJobUpdateBlockStatus)
 * - Duration of the function JobUpdateBlockStatus_pfct depends on the current job.
 *   In worst case the function NvM_Prv_UpdateBlockStatus_RecalcRamBlkCr is used.
 *   NvM_Prv_UpdateBlockStatus_RecalcRamBlkCr = 16 (see description of NvM_Prv_UpdateBlockStatus_RecalcRamBlkCrc)
 * - NvM_Prv_Block_SetState = 1 (see description of NvM_Prv_Block_SetState)
 *
 * \param[in] Job_pcst
 * Pointer to the data of the job to update block status
 * \param[in] isJobResultPlausible_b
 * Information whether current job result is plausible
 */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_MainFunctionResultEval_UpdateBlkStatus(NvM_Prv_Job_tst const* Job_pcst,
                                                                            boolean isJobResultPlausible_b)
{
    if (isJobResultPlausible_b)
    {
        // Pointer to the function to update the status of the block handled during the currently active job
        NvM_Prv_Job_UpdateBlockStatus_tpfct JobUpdateBlockStatus_pfct =
                NvM_Prv_GetFctJobUpdateBlockStatus(Job_pcst->idJob_en);

        if (NULL_PTR != JobUpdateBlockStatus_pfct)
        {
            JobUpdateBlockStatus_pfct(Job_pcst);
        }
    }

    NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, NVM_BLOCK_STATE_JOB_ONGOING, 0u);
}

/**
 * \brief
 * This local private function invokes all callback functions required at the end of an internal job.
 * \details
 * This function invokes callback function for a single-block request only if it was not triggered by the auxiliary user.
 *
 * \param[in] stRequestResult_uo
 * Evaluated user request result to be passed to the user via callback functions
 * \param[id] idActiveBlock_uo
 * ID of the block used in finished job
 * \param[in] idActiveService_uo
 * ID of the service which has initiated finished job
 * \param[in] IsAuxServiceActive_b
 * Information whether finished job has been processed for an auxiliary request (service)
 */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_MainFunctionResultEval_FinalBlockCallbacks(NvM_RequestResultType stRequestResult_uo,
                                                                                NvM_BlockIdType idActiveBlock_uo,
                                                                                NvM_Prv_idService_tuo idActiveService_uo,
                                                                                boolean IsAuxServiceActive_b)
{
    // Invoke observer and singleblock callbacks if applicable
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2842] Invoke the observer callback if this callback routine is configured
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2844] Invoke the observer callback before the singleblock callback
    NvM_Prv_BlkDesc_InvokeObserverCallback(idActiveBlock_uo, idActiveService_uo, stRequestResult_uo);

    // TRACE[NVM347] Invoke singleblock callback if callback routine is configured
    // TRACE[NVM281] Processing of this block finished -> invoke callback function (applies to NvM_ReadAll)
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2839] Same as NVM281, but applied to first init
    // TRACE[NVM740] The callback is invoked upon termination of the block operation
    // TRACE[NVM742] The caller is only actively notified if a callback has been configured
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2930]
    // Don't invoke the singleblock callback if the current service is triggered by an auxiliary user
    if (!IsAuxServiceActive_b)
    {
        // TRACE[NVM741] Supply the ID of the just finished block operation to the callback owner
        NvM_Prv_BlkDesc_InvokeSingleBlockCallback(idActiveBlock_uo, idActiveService_uo, stRequestResult_uo);
    }
}

// ====================================================================================================================

/**
 * \brief
 * This local private function resets pending user requests for the given block dependent on current service and result.
 * \details
 * \attention
 * NvM invokes this function with enabled scheduler lock. In worst case the duration of the scheduler lock corresponds
 * to 2 copy-operations:
 * - NvM_Prv_Block_IsRequestPending = 1 (see description of NvM_Prv_Block_IsRequestPending)
 * - NvM_Prv_Block_ClearRequests = 1 (see description of NvM_Prv_Block_ClearRequests)
 *
 * \param[in] idBlock_uo
 * ID of the block to reset user requests
 * \param[in] idService_uo
 * ID of the current user request (service)
 * \param[in] ServiceBit_uo
 * Service Bit of the current user request
 * \param[in] stRequestResult_uo
 * Result of the current user request (service)
 */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_MainFunctionResultEval_ResetRequests(NvM_BlockIdType idBlock_uo,
                                                                          NvM_Prv_idService_tuo idService_uo,
                                                                          NvM_Prv_ServiceBit_tuo ServiceBit_uo,
                                                                          NvM_RequestResultType stRequestResult_uo)
{
    uint16 ServiceBitMask_uo;

    if (idBlock_uo < NVM_PRV_NR_BLOCKS_ADMIN)
    {
        if (NVM_SERVICE_ID_RB_INIT_AFTER_READ_ALL == idService_uo)
        {
            ServiceBitMask_uo = (1u << NvM_Prv_ServiceBit_InitAtLayoutChange_e) |
                                (1u << NvM_Prv_ServiceBit_RemoveNonResistant_e);
        }
        else if (   (NVM_SERVICE_ID_READ_ALL == idService_uo)
                 && (NVM_RB_CONFIG_ID_BLOCK != idBlock_uo))
        {
            ServiceBitMask_uo =   (1u << NvM_Prv_ServiceBit_ReadAll_e)
                                | (1u << NvM_Prv_ServiceBit_ReadAllExtPreparation_e);

            if (   NvM_Prv_Block_IsRequestPending(idBlock_uo, (1u << NvM_Prv_ServiceBit_InitAtLayoutChange_e))
                && (NVM_REQ_RESTORED_FROM_ROM != stRequestResult_uo))
            {
                ServiceBitMask_uo |= (1u << NvM_Prv_ServiceBit_InitAtLayoutChange_e);
            }
        }
        else
        {
            ServiceBitMask_uo = (1u << ServiceBit_uo);
        }

        NvM_Prv_Block_ClearRequests(idBlock_uo, ServiceBitMask_uo);
    }
}

RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_MainFunctionResultEval_SuspendJobUsingCanceledCsmJob(NvM_Prv_Job_tst const* Job_pcst,
                                                                                             NvM_Prv_Queue_idx_tst const* idxJob_pcst)
{
    boolean isCanceledJobSuspended_b = FALSE;
#if (NVM_CRYPTO_USED == STD_ON)
    if (Job_pcst->JobResult_st.Crypto_st.isJobCanceled_b)
    {
        isCanceledJobSuspended_b = NvM_Prv_JobQueue_SuspendJob(idxJob_pcst);
    }
#else
    (void)Job_pcst;
    (void)idxJob_pcst;
#endif

    return isCanceledJobSuspended_b;
}



#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"
