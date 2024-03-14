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
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM.h"

#include "rba_MemLib_Special.h"

#include "NvM_Prv_BlockSplit.h"
#include "NvM_Prv_JobStateMachine.h"
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobInvalidate_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC boolean NvM_Prv_JobInvalidate_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobInvalidate_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                            NvM_Prv_JobResult_tst* JobResult_pst,
                                                                            NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobInvalidate_DoSubBlocks(NvM_Prv_stJob_ten* stJob_pen,
                                                                                  NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                  NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobInvalidate_DoInvalidate(NvM_Prv_stJob_ten* stJob_pen,
                                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                   NvM_Prv_JobData_tst const* JobData_pcst);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_JobInvalidate_Data_cst =
{
        &NvM_Prv_JobInvalidate_GetStateFct,
        &NvM_Prv_JobInvalidate_IsEnabled,
        NvM_Prv_stJob_Invalidate_Start_e,
        NvM_Prv_stJob_Invalidate_Completed_e,
        NvM_Prv_JobResource_nrCluster_e

};

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobInvalidate_GetData(void)
{
    return &NvM_Prv_JobInvalidate_Data_cst;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_JobInvalidate_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // Invalidation of a block is always enabled for all blocks
    return TRUE;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobInvalidate_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobInvalidate_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_Invalidate_Start_e:
            JobInvalidate_State_pfct = &NvM_Prv_JobInvalidate_Start;
        break;

        case NvM_Prv_stJob_Invalidate_DoSubBlocks_e:
            JobInvalidate_State_pfct = &NvM_Prv_JobInvalidate_DoSubBlocks;
        break;

        case NvM_Prv_stJob_Invalidate_DoInvalidate_e:
            JobInvalidate_State_pfct = &NvM_Prv_JobInvalidate_DoInvalidate;
        break;

        default:
            JobInvalidate_State_pfct = NULL_PTR;
        break;

    }

    return JobInvalidate_State_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobInvalidate_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                            NvM_Prv_JobResult_tst* JobResult_pst,
                                                                            NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoInvalidate_e;

    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    // update indexes for all sub-blocks to make sure that all sub-blocks will be invalidated
    NvM_Prv_BlockSplit_SetChangedAllSubBlocks(JobData_pcst->idBlock_uo, JobData_pcst->IntBuffer_st.Buffer_pu8);

    *stJob_pen = NvM_Prv_stJob_Invalidate_DoSubBlocks_e;
    idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoBlockSplit_Invalidate_e;

    return idJobStateMachineNext_en;

}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobInvalidate_DoSubBlocks(NvM_Prv_stJob_ten* stJob_pen,
                                                                                  NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                  NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoInvalidate_e;
    (void)JobData_pcst;

    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
    *stJob_pen = NvM_Prv_stJob_Invalidate_DoInvalidate_e;
    idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Invalidate_e;

    return idJobStateMachineNext_en;
}

/* MR12 RULE 8.13 VIOLATION: This function does not update the result structure but all Job-State-functions shall
 * have the same signature and especially the result-structure has to be changeable so that the result can be modified
 * if necessary. */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobInvalidate_DoInvalidate(NvM_Prv_stJob_ten* stJob_pen,
                                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                   NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoInvalidate_e;
    (void)JobResult_pst;

    // initialize master-block to make sure that if invalidated split-block will be written again
    // then everything will be written properly
    NvM_Prv_BlockSplit_Initialize(JobData_pcst->idBlock_uo);
    // -> finish invalidation job, result is set in sub-state-machine and remains for evaluation of the job
    *stJob_pen = NvM_Prv_stJob_Invalidate_Completed_e;

    return idJobStateMachineNext_en;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

