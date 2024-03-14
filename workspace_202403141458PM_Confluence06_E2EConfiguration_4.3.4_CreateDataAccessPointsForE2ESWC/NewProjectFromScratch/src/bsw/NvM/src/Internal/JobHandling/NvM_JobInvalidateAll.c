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

#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_JobStateMachine.h"
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobInvalidateAll_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC boolean NvM_Prv_JobInvalidateAll_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobInvalidateAll_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobInvalidateAll_DoInvalidate(NvM_Prv_stJob_ten* stJob_pen,
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

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_JobInvalidateAll_Data_cst =
{
        &NvM_Prv_JobInvalidateAll_GetStateFct,
        &NvM_Prv_JobInvalidateAll_IsEnabled,
        NvM_Prv_stJob_InvalidateAll_Start_e,
        NvM_Prv_stJob_InvalidateAll_Completed_e,
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

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobInvalidateAll_GetData(void)
{
    return &NvM_Prv_JobInvalidateAll_Data_cst;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_JobInvalidateAll_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // Invalidation of all non-volatile blocks is always enabled for all blocks
    return TRUE;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobInvalidateAll_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobInvalidateAll_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_InvalidateAll_Start_e:
            JobInvalidateAll_State_pfct = &NvM_Prv_JobInvalidateAll_Start;
        break;

        case NvM_Prv_stJob_InvalidateAll_DoInvalidate_e:
            JobInvalidateAll_State_pfct = &NvM_Prv_JobInvalidateAll_DoInvalidate;
        break;

        default:
            JobInvalidateAll_State_pfct = NULL_PTR;
        break;

    }

    return JobInvalidateAll_State_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobInvalidateAll_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoInvalidateAll_e;
    (void)JobData_pcst;

    *stJob_pen = NvM_Prv_stJob_InvalidateAll_DoInvalidate_e;
    idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoInvalidate_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobInvalidateAll_DoInvalidate(NvM_Prv_stJob_ten* stJob_pen,
                                                                                      NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                      NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoInvalidateAll_e;

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {

        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2850] Invalidate all the NV blocks of an NvM block
        //                                          of type DATASET which is selected for FirstInitAll
        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2852] Remove all the NV blocks of a non-resistant NvM block
        //                                          of type DATASET during the removal phase
        if ((JobData_pcst->idxDataset_u8 < (NvM_Prv_BlkDesc_GetNrNonVolatileBlocks(JobData_pcst->idBlock_uo) - 1u)))
        {
            // The following union is necessary to remove an otherwise unavoidable compiler warning
            // due to a discarded const qualifier
            union
            {
                NvM_Prv_JobData_tst const* JobData_pcst;
                NvM_Prv_JobData_tst* JobData_pst;
            } JobData_un;
            JobData_un.JobData_pcst = JobData_pcst;

            JobData_un.JobData_pst->idxDataset_u8 +=1u;

            *stJob_pen = NvM_Prv_stJob_InvalidateAll_DoInvalidate_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoInvalidate_e;
            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
        }
        else
        {
            // invalidations of all non-volatile blocks has succeeded -> finish job
            *stJob_pen = NvM_Prv_stJob_InvalidateAll_Completed_e;
        }
    }
    else
    {
        // MemIf job has failed -> finish write job, update final job result
        JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
        *stJob_pen = NvM_Prv_stJob_InvalidateAll_Completed_e;
    }

    return idJobStateMachineNext_en;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

