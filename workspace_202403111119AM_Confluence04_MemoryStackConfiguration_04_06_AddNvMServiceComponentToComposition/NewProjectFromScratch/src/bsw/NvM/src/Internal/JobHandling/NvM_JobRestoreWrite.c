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

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobRestoreWrite_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC boolean NvM_Prv_JobRestoreWrite_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestoreWrite_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                              NvM_Prv_JobResult_tst* JobResult_pst,
                                                                              NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestoreWrite_DoRestore(NvM_Prv_stJob_ten* stJob_pen,
                                                                                  NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                  NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestoreWrite_DoWrite(NvM_Prv_stJob_ten* stJob_pen,
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

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_JobRestoreWrite_Data_cst =
{
        &NvM_Prv_JobRestoreWrite_GetStateFct,
        &NvM_Prv_JobRestoreWrite_IsEnabled,
        NvM_Prv_stJob_RestoreWrite_Start_e,
        NvM_Prv_stJob_RestoreWrite_Completed_e,
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

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobRestoreWrite_GetData(void)
{
    return &NvM_Prv_JobRestoreWrite_Data_cst;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_JobRestoreWrite_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // Restoring of default values and writing them is always enabled for all blocks
    return TRUE;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobRestoreWrite_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobRestoreWrite_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_RestoreWrite_Start_e:
            JobRestoreWrite_State_pfct = &NvM_Prv_JobRestoreWrite_Start;
        break;

        case NvM_Prv_stJob_RestoreWrite_DoRestore_e:
            JobRestoreWrite_State_pfct = &NvM_Prv_JobRestoreWrite_DoRestore;
        break;

        case NvM_Prv_stJob_RestoreWrite_DoWrite_e:
            JobRestoreWrite_State_pfct = &NvM_Prv_JobRestoreWrite_DoWrite;
        break;

        default:
            JobRestoreWrite_State_pfct = NULL_PTR;
        break;

    }

    return JobRestoreWrite_State_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestoreWrite_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                              NvM_Prv_JobResult_tst* JobResult_pst,
                                                                              NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRestoreWrite_e;
    (void)JobData_pcst;

    *stJob_pen = NvM_Prv_stJob_RestoreWrite_DoRestore_e;
    idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRestore_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestoreWrite_DoRestore(NvM_Prv_stJob_ten* stJob_pen,
                                                                                  NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                  NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRestoreWrite_e;

    (void)JobData_pcst;

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2834] This restore job was part of a first initialization,
        //                                          trigger a write job
        *stJob_pen = NvM_Prv_stJob_RestoreWrite_DoWrite_e;
        idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoWrite_e;
        JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
    }
    else
    {
        // finish job,
        // do not update final job result so that result of the read state machine remains for evaluation
        *stJob_pen = NvM_Prv_stJob_RestoreWrite_Completed_e;
    }

    return idJobStateMachineNext_en;
}

/* MR12 RULE 8.13 VIOLATION: This function does not update the result structure but all Job-State-functions shall
 * have the same signature and especially the result-structure has to be changeable so that the result can be modified
 * if necessary. */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestoreWrite_DoWrite(NvM_Prv_stJob_ten* stJob_pen,
                                                                                NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRestoreWrite_e;

    (void)JobData_pcst;
    // do not update final job result so that result of the write state machine remains for evaluation
    (void)JobResult_pst;

    // finish job, update final job result
    *stJob_pen = NvM_Prv_stJob_RestoreWrite_Completed_e;

    return idJobStateMachineNext_en;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

