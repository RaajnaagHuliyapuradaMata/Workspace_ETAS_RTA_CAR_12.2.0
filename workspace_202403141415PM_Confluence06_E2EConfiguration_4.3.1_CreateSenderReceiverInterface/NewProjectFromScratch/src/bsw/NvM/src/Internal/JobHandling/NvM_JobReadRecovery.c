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

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobReadRecovery_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC boolean NvM_Prv_JobReadRecovery_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Job_ReadRecovery_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Job_ReadRecovery_DoRead(NvM_Prv_stJob_ten* stJob_pen,
                                                                                NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Job_ReadRecovery_DoRecovery(NvM_Prv_stJob_ten* stJob_pen,
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

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_JobReadRecovery_Data_cst =
{
        &NvM_Prv_JobReadRecovery_GetStateFct,
        &NvM_Prv_JobReadRecovery_IsEnabled,
        NvM_Prv_stJob_ReadRecovery_Start_e,
        NvM_Prv_stJob_ReadRecovery_Completed_e,
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

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobReadRecovery_GetData(void)
{
    return &NvM_Prv_JobReadRecovery_Data_cst;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_JobReadRecovery_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // Read with implicit recovery is always enabled for all blocks
    return TRUE;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobReadRecovery_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobReadRecovery_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_ReadRecovery_Start_e:
            JobReadRecovery_State_pfct = &NvM_Prv_Job_ReadRecovery_Start;
        break;

        case NvM_Prv_stJob_ReadRecovery_DoRead_e:
            JobReadRecovery_State_pfct = &NvM_Prv_Job_ReadRecovery_DoRead;
        break;

        case NvM_Prv_stJob_ReadRecovery_DoRecovery_e:
            JobReadRecovery_State_pfct = &NvM_Prv_Job_ReadRecovery_DoRecovery;
        break;

        default:
            JobReadRecovery_State_pfct = NULL_PTR;
        break;
    }

    return JobReadRecovery_State_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Job_ReadRecovery_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachine_en = NvM_Prv_idJobStateMachine_DoReadRecovery_e;
    (void)JobData_pcst;

    *stJob_pen = NvM_Prv_stJob_ReadRecovery_DoRead_e;
    idJobStateMachine_en = NvM_Prv_idJobStateMachine_DoRead_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachine_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Job_ReadRecovery_DoRead(NvM_Prv_stJob_ten* stJob_pen,
                                                                                NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachine_en = NvM_Prv_idJobStateMachine_DoReadRecovery_e;

    switch (JobResult_pst->Result_en)
    {
        case NvM_Prv_JobResult_Succeeded_e:
        case NvM_Prv_JobResult_Succeeded_MemIfSkipped_e:
        case NvM_Prv_JobResult_BlockInvalidated_e:
        case NvM_Prv_JobResult_Skipped_e:
        {
            // Reading data has succeeded or is not required ->
            // finish job, current result will be used for result evaluation
            *stJob_pen = NvM_Prv_stJob_ReadRecovery_Completed_e;
        }
        break;
        case NvM_Prv_JobResult_FailedNoRecovery_e:
        {
            // Reading data has failed due to failed explicit synchronization ->
            // finish job, update final job result
            *stJob_pen = NvM_Prv_stJob_ReadRecovery_Completed_e;
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
        }
        break;
        default:
        {
            // Reading data has failed ->

            // Implicit recovery will be started only if MemIf read job did not finish successfully
            // -> start implicit recovery only for blocks with available default data
            // Note: As opposed to NVM389, implicit recovery is (currently?) not supported for DATASET blocks
            // TRACE[SWS_NvM_00951] Read job for NvM_ReadBlock or NvM_ReadPRAMBlock shall provide implicit recovery
            //                      only for blocks of type native and redundant
            // TRACE[SWS_NvM_00789] Read job for NvM_ReadPRAMBlock shall load default values if lower layer fails
            // TRACE[SWS_NvM_00882] Read job for NvM_ReadPRAMBlock shall load default values if lower layer fails
            if ((NvM_Prv_BlkDesc_IsDefaultDataAvailable(JobData_pcst->idBlock_uo)) &&
                (NVM_BLOCK_DATASET != NvM_Prv_BlkDesc_GetType(JobData_pcst->idBlock_uo)))
            {
                // do implicit recovery by loading default values
                // Important: availability of default data will be checked within the state machine for restore
                *stJob_pen = NvM_Prv_stJob_ReadRecovery_DoRecovery_e;
                idJobStateMachine_en = NvM_Prv_idJobStateMachine_DoRestore_e;
                JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
            }
            else
            {
                // finish job,
                // do not update final job result so that result of the read state machine remains for evaluation
                *stJob_pen = NvM_Prv_stJob_ReadRecovery_Completed_e;
            }
        }
        break;
    }

    return idJobStateMachine_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Job_ReadRecovery_DoRecovery(NvM_Prv_stJob_ten* stJob_pen,
                                                                                    NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                    NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachine_en = NvM_Prv_idJobStateMachine_DoReadRecovery_e;
    (void)JobData_pcst;

    // finish job, update final job result
    *stJob_pen = NvM_Prv_stJob_ReadRecovery_Completed_e;

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        JobResult_pst->Result_en = NvM_Prv_JobResult_BlockDefault_e;
    }
    else
    {
        JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
    }

    return idJobStateMachine_en;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

