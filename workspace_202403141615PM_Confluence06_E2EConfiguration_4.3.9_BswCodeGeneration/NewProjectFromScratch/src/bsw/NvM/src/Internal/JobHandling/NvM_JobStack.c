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

#include "NvM_Prv_Crc.h"
#include "NvM_Prv_MemIf.h"
#include "NvM_Prv_Crypto.h"
#include "NvM_Prv_JobStack.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_BlockSplit.h"
#include "NvM_Prv_JobResource.h"
#include "NvM_Prv_ErrorDetection.h"
#include "NvM_Prv_JobStateMachine.h"

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
# include "TestCd_NvM.h"
#endif

/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
 */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idJobStateMachine_ten NvM_Prv_Job_GetInitialStateMachine(NvM_Prv_idJob_ten idJob_en);

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_JobStack_IsStateMachineReady(boolean* stReadiness_b,
                                                                  NvM_Prv_JobData_tst const* JobData_pcst,
                                                                  NvM_Prv_JobResource_Cluster_ten ClusterToLock_en);

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_JobStack_DoStateMachine(NvM_Prv_JobStack_tst* JobStack_pst,
                                                             NvM_Prv_JobResult_tst* JobResult_pst,
                                                             NvM_Prv_JobData_tst const* JobData_pcst,
                                                             NvM_Prv_Job_StateMachine_Data_tst const* dataStateMachine_pcst);

RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idJobStateMachine_ten NvM_Prv_JobStack_DoState(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst,
                                                                               NvM_Prv_Job_StateMachine_Data_tst const* dataStateMachine_pcst);

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_JobStack_ReleaseResource(NvM_Prv_JobData_tst const* JobData_pcst,
                                                              NvM_Prv_idJobStateMachine_ten idJobStateMachineActive_en,
                                                              NvM_Prv_JobResource_Cluster_ten ResourceClusterToRelease_en);

/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_Job_StateMachine_GetData(NvM_Prv_idJobStateMachine_ten idJobStateMachine_en);

RBA_MEMLIB_STATIC void NvM_Prv_JobStack_Push(NvM_Prv_JobStack_tst* JobStack_pst,
                                             NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en,
                                             NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC void NvM_Prv_JobStack_Pop(NvM_Prv_JobStack_tst* JobStack_pst,
                                            NvM_Prv_JobData_tst const* JobData_pcst,
                                            NvM_Prv_JobResource_Cluster_ten ResourceClusterToRelease_en);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

#define LIST_JOB_ENTRY_INITIAL_MACHINE(Name, StateMachine, ResultEvaluation)    NvM_Prv_idJobStateMachine_##StateMachine##_e,

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten const NvM_Prv_Job_InitialStateMachines_caen[NvM_Prv_idJob_Count_e] =
{
        /* MR12 RULE 20.7 VIOLATION: To create the list the argument shall be not parenthesized */
        LIST_JOB(LIST_JOB_ENTRY_INITIAL_MACHINE)
};

#define LIST_ENTRY_JOB_STATE_MACHINE_DATA(Name, Data)   Data,

RBA_MEMLIB_STATIC NvM_Prv_Job_StateMachine_GetData_tpfct const NvM_Prv_Job_StateMachine_GetData_capfct[NvM_Prv_idJobStateMachine_Count_e] =
{
        /* MR12 RULE 20.7 VIOLATION: To create the list the argument shall be not parenthesized */
        LIST_JOB_STATE_MACHINES(LIST_ENTRY_JOB_STATE_MACHINE_DATA)
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

void NvM_Prv_JobStack_Initialize(NvM_Prv_idJob_ten idJob_en,
                                 NvM_Prv_JobStack_tst* JobStack_pst,
                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                 NvM_Prv_JobData_tst const* JobData_pcst)
{
    // initialize index to current entry on job stack
    JobStack_pst->idxJobStack_uo = NVM_CFG_JOB_STACK_SIZE;

    if (NvM_Prv_ErrorDetection_IsJobIdValid(NVM_SERVICE_ID_MAIN_FUNCTION, JobData_pcst->idBlock_uo, idJob_en))
    {
        NvM_Prv_JobStack_Push(JobStack_pst, NvM_Prv_Job_GetInitialStateMachine(idJob_en), JobData_pcst);
    }
    else
    {
        // This function may only be called for a valid job ID
        // (we should actually never get into this branch; it's just there for robustness)
        // if job id is invalid then job stack remains empty so NvM executes no job and evaluates results in next step,
        // see NvM_MainFunctionResultEval.c
        JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
    }
}

boolean NvM_Prv_JobStack_IsEmpty(NvM_Prv_JobStack_tst const* JobStack_pcst)
{
    return (NVM_CFG_JOB_STACK_SIZE <= JobStack_pcst->idxJobStack_uo);
}

void NvM_Prv_JobStack_Do(NvM_Prv_JobStack_tst* JobStack_pst,
                         NvM_Prv_JobResult_tst* JobResult_pst,
                         NvM_Prv_JobData_tst const* JobData_pcst)
{
    uint8 idxJobStack_uo = JobStack_pst->idxJobStack_uo;
    NvM_Prv_Job_StateMachine_Data_tst const* dataStateMachine_pcst;

    dataStateMachine_pcst = NvM_Prv_Job_StateMachine_GetData(JobStack_pst->Entries_ast[idxJobStack_uo].idJobStateMachine_en);

    if (NULL_PTR != dataStateMachine_pcst)
    {
        NvM_Prv_JobStack_IsStateMachineReady(&JobStack_pst->Entries_ast[idxJobStack_uo].isStateMachineReady_b,
                                             JobData_pcst,
                                             dataStateMachine_pcst->RequiredResourceCluster_en);

        if (JobStack_pst->Entries_ast[idxJobStack_uo].isStateMachineReady_b)
        {
            NvM_Prv_JobStack_DoStateMachine(JobStack_pst, JobResult_pst, JobData_pcst, dataStateMachine_pcst);
        }
        else
        {
            JobResult_pst->Result_en = NvM_Prv_JobResult_Pending_e;
        }
    }
    else
    {
        JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
        NvM_Prv_JobStack_Pop(JobStack_pst, JobData_pcst, NvM_Prv_JobResource_nrCluster_e);
    }
}

RBA_MEMLIB_STATIC NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_Job_StateMachine_GetData(NvM_Prv_idJobStateMachine_ten idJobStateMachine_en)
{
    if (idJobStateMachine_en >= NvM_Prv_idJobStateMachine_Count_e)
    {
        idJobStateMachine_en = NvM_Prv_idJobStateMachine_Invalid_e;
    }

    if (NULL_PTR != NvM_Prv_Job_StateMachine_GetData_capfct[idJobStateMachine_en])
    {
        return (NvM_Prv_Job_StateMachine_GetData_capfct[idJobStateMachine_en])();
    }
    else
    {
        return NULL_PTR;
    }
}

RBA_MEMLIB_STATIC void NvM_Prv_JobStack_Push(NvM_Prv_JobStack_tst* JobStack_pst,
                                             NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en,
                                             NvM_Prv_JobData_tst const* JobData_pcst)
{
    if (JobStack_pst->idxJobStack_uo > 0u)
    {
        NvM_Prv_Job_StateMachine_Data_tst const* dataStateMachine_pcst;
        dataStateMachine_pcst = NvM_Prv_Job_StateMachine_GetData(idJobStateMachineNext_en);

        /* MR12 RULE 13.5 VIOLATION: if function pointer is defined, the function is called, no unexpected side-effects */
        if ((NULL_PTR != dataStateMachine_pcst) && (dataStateMachine_pcst->IsEnabled_pfct)(JobData_pcst))
        {
            --JobStack_pst->idxJobStack_uo;
            JobStack_pst->Entries_ast[JobStack_pst->idxJobStack_uo].idJobStateMachine_en = idJobStateMachineNext_en;
            JobStack_pst->Entries_ast[JobStack_pst->idxJobStack_uo].stJob_en = dataStateMachine_pcst->stInitial_en;
            JobStack_pst->Entries_ast[JobStack_pst->idxJobStack_uo].isStateMachineReady_b =
                            (dataStateMachine_pcst->RequiredResourceCluster_en >= NvM_Prv_JobResource_nrCluster_e);
        }
    }
}

RBA_MEMLIB_STATIC void NvM_Prv_JobStack_Pop(NvM_Prv_JobStack_tst* JobStack_pst,
                                            NvM_Prv_JobData_tst const* JobData_pcst,
                                            NvM_Prv_JobResource_Cluster_ten ResourceClusterToRelease_en)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineAfterPop_en = NvM_Prv_idJobStateMachine_Invalid_e;

    if (JobStack_pst->idxJobStack_uo < NVM_CFG_JOB_STACK_SIZE)
    {
        JobStack_pst->Entries_ast[JobStack_pst->idxJobStack_uo].idJobStateMachine_en = NvM_Prv_idJobStateMachine_Invalid_e;
        JobStack_pst->Entries_ast[JobStack_pst->idxJobStack_uo].stJob_en = NvM_Prv_stJob_Idle_e;
        JobStack_pst->Entries_ast[JobStack_pst->idxJobStack_uo].isStateMachineReady_b = FALSE;
        ++JobStack_pst->idxJobStack_uo;

        if (JobStack_pst->idxJobStack_uo < NVM_CFG_JOB_STACK_SIZE)
        {
            // there is still a state machine on the stack to be executed
            idJobStateMachineAfterPop_en = JobStack_pst->Entries_ast[JobStack_pst->idxJobStack_uo].idJobStateMachine_en;
        }
    }

    NvM_Prv_JobStack_ReleaseResource(JobData_pcst, idJobStateMachineAfterPop_en, ResourceClusterToRelease_en);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Inline code
 **********************************************************************************************************************
 */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idJobStateMachine_ten NvM_Prv_Job_GetInitialStateMachine(NvM_Prv_idJob_ten idJob_en)
{
    if (idJob_en >= NvM_Prv_idJob_Count_e)
    {
        idJob_en = NvM_Prv_idJob_Invalid_e;
    }
    return NvM_Prv_Job_InitialStateMachines_caen[idJob_en];
}

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_JobStack_IsStateMachineReady(boolean* stReadiness_b,
                                                                  NvM_Prv_JobData_tst const* JobData_pcst,
                                                                  NvM_Prv_JobResource_Cluster_ten ClusterToLock_en)
{
    if (!(*stReadiness_b))
    {
        // NvM invokes this function only if state machine uses a resource,
        // i.e. RequiredCluster_en < NvM_Prv_JobResource_Cluster_NoResource_e
        NvM_Prv_idJobResource_tuo idJobResourceToLock_uo = NvM_Prv_BlkDesc_GetIdJobResource(JobData_pcst->idBlock_uo,
                                                                                            ClusterToLock_en);

        *stReadiness_b = NvM_Prv_JobResource_Lock(idJobResourceToLock_uo, JobData_pcst);
    }
}

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_JobStack_DoStateMachine(NvM_Prv_JobStack_tst* JobStack_pst,
                                                             NvM_Prv_JobResult_tst* JobResult_pst,
                                                             NvM_Prv_JobData_tst const* JobData_pcst,
                                                             NvM_Prv_Job_StateMachine_Data_tst const* dataStateMachine_pcst)
{
    uint8 idxJobStack_uo = JobStack_pst->idxJobStack_uo;
    NvM_Prv_idJobStateMachine_ten idJobStateMachine_en = JobStack_pst->Entries_ast[idxJobStack_uo].idJobStateMachine_en;
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en;

    idJobStateMachineNext_en = NvM_Prv_JobStack_DoState(&JobStack_pst->Entries_ast[idxJobStack_uo].stJob_en,
                                                        JobResult_pst,
                                                        JobData_pcst,
                                                        dataStateMachine_pcst);

    if (idJobStateMachine_en != idJobStateMachineNext_en)
    {
        NvM_Prv_JobStack_Push(JobStack_pst, idJobStateMachineNext_en, JobData_pcst);
    }
    else
    {
        if (JobStack_pst->Entries_ast[idxJobStack_uo].stJob_en == dataStateMachine_pcst->stFinal_en)
        {
            NvM_Prv_JobStack_Pop(JobStack_pst, JobData_pcst, dataStateMachine_pcst->RequiredResourceCluster_en);
        }
    }
}

RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idJobStateMachine_ten NvM_Prv_JobStack_DoState(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst,
                                                                               NvM_Prv_Job_StateMachine_Data_tst const* dataStateMachine_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_Invalid_e;
    NvM_Prv_Job_State_tpfct State_pfct = dataStateMachine_pcst->StateMachine_pfct(*stJob_pen);

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_JobMonitor_SetState(JobData_pcst, *stJob_pen);
#endif

    if ((NULL_PTR != State_pfct))
    {
        idJobStateMachineNext_en = State_pfct(stJob_pen, JobResult_pst, JobData_pcst);
    }
    else
    {
        JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
    }

    if (NvM_Prv_JobResult_Failed_e == JobResult_pst->Result_en)
    {
        *stJob_pen = dataStateMachine_pcst->stFinal_en;
    }

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_JobMonitor_SetState(JobData_pcst, *stJob_pen);
#endif

    return idJobStateMachineNext_en;
}

/**
 *
 */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_JobStack_ReleaseResource(NvM_Prv_JobData_tst const* JobData_pcst,
                                                              NvM_Prv_idJobStateMachine_ten idJobStateMachineActive_en,
                                                              NvM_Prv_JobResource_Cluster_ten ResourceClusterToRelease_en)
{
    boolean isResourceClusterInUse_b = FALSE;

    NvM_Prv_Job_StateMachine_Data_tst const* dataStateMachine_pcst;
    dataStateMachine_pcst = NvM_Prv_Job_StateMachine_GetData(idJobStateMachineActive_en);

    isResourceClusterInUse_b = (   (NULL_PTR != dataStateMachine_pcst)
                                && (ResourceClusterToRelease_en == dataStateMachine_pcst->RequiredResourceCluster_en));

    if (!isResourceClusterInUse_b && (ResourceClusterToRelease_en < NvM_Prv_JobResource_nrCluster_e))
    {
        NvM_Prv_idJobResource_tuo idJobResourceToRelease_uo;

        idJobResourceToRelease_uo = NvM_Prv_BlkDesc_GetIdJobResource(JobData_pcst->idBlock_uo,
                                                                     ResourceClusterToRelease_en);

        NvM_Prv_JobResource_Release(idJobResourceToRelease_uo, JobData_pcst);
    }
}

