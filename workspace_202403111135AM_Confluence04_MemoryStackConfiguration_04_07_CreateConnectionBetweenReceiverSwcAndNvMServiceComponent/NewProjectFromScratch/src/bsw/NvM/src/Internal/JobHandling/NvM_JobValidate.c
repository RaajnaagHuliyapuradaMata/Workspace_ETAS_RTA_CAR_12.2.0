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

#include "rba_MemLib_Special.h"

#include "NvM_Prv.h"
#include "NvM_Prv_JobStateMachine.h"

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC boolean NvM_Prv_JobValidate_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobValidate_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobValidate(NvM_Prv_stJob_ten* stJob_pen,
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

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_JobValidate_Data_cst =
{
        &NvM_Prv_JobValidate_GetStateFct,
        &NvM_Prv_JobValidate_IsEnabled,
        NvM_Prv_stJob_Validate_Start_e,
        NvM_Prv_stJob_Validate_Completed_e,
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

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobValidate_GetData(void)
{
    return &NvM_Prv_JobValidate_Data_cst;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_JobValidate_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // AutoValidation is always enabled for all blocks
    return TRUE;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobValidate_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobValidate_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_Validate_Start_e:
            JobValidate_State_pfct = &NvM_Prv_JobValidate;
        break;

        default:
            JobValidate_State_pfct = NULL_PTR;
        break;
    }
    return JobValidate_State_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobValidate(NvM_Prv_stJob_ten* stJob_pen,
                                                                    NvM_Prv_JobResult_tst* JobResult_pst,
                                                                    NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoValidate_e;

    (void)JobData_pcst;
    *stJob_pen = NvM_Prv_stJob_Validate_Completed_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

