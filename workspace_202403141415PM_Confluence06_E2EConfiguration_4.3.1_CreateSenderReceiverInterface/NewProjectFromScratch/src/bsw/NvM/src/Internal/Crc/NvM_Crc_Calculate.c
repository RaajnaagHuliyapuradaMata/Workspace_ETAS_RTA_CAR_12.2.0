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
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_JobStateMachine_Types.h"

/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#if (NVM_CALC_CRC == STD_ON)

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC boolean NvM_Prv_Crc_Calculate_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_Crc_Calculate_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Crc_Calculate_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                            NvM_Prv_JobResult_tst* JobResult_pst,
                                                                            NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Crc_Calculate_Do(NvM_Prv_stJob_ten* stJob_pen,
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

/**
 * Definition of data required for processing of the state machine for CRC calculation.
 */
RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_Crc_Calculate_Data_cst =
{
        &NvM_Prv_Crc_Calculate_GetStateFct,         // StateMachine_pfct
        &NvM_Prv_Crc_Calculate_IsEnabled,           // IsEnabled_pfct
        NvM_Prv_stJob_Crc_Calculate_Start_e,        // stInitial_en
        NvM_Prv_stJob_Crc_Calculate_Completed_e,    // stFinal_en
        NvM_Prv_JobResource_nrCluster_e             // RequiredResourceCluster_en

};

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

#endif  // (NVM_CALC_CRC == STD_ON)

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * This private function returns data required for processing of the state machine for CRC calculation
 * \details
 * If CRC usage is disabled by user configuration then NULL_PTR is returned.
 *
 * \return
 * data required for processing of the state machine for CRC calculation
 */
NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_Crc_Calculate_GetData(void)
{
#if (NVM_CALC_CRC == STD_ON)
    return &NvM_Prv_Crc_Calculate_Data_cst;
#else
    return NULL_PTR;
#endif
}

#if (NVM_CALC_CRC == STD_ON)

/**
 * \brief
 * This local private function checks whether the CRC calculation is enabled for current job.
 * \details
 * CRC calculattion is enabled only if valid CRC type is configured for the block used in current job.
 *
 * \param[in] JobData_pcst
 * Pointer to the job data required for the check
 *
 * \return
 * TRUE = CRC calculation is enabled
 * FALSE = CRC calculation is disabled
 */
RBA_MEMLIB_STATIC boolean NvM_Prv_Crc_Calculate_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_Crc_Type_ten CrcType_en = NvM_Prv_BlkDesc_GetCrcType(JobData_pcst->idBlock_uo);

    return (NvM_Prv_Crc_Type_NoCrc_e != CrcType_en);
}

/**
 * \brief
 * This local private function returns pointer to the job state function dependent on the current job state.
 * \details
 * This function returns a NULL pointer if passed job state is invalid.
 * \note
 * The actual check of returned NULL_PTR will be done centrally in NvM_Prv_JobStack_DoState. And in case of invalid
 * job state the current job state machine will fail immediately.
 *
 * \param[in] stJob_en
 * Current job state
 *
 * \return
 * Pointer to the job state function
 */
RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_Crc_Calculate_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct Crc_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_Crc_Calculate_Start_e:
            Crc_State_pfct = &NvM_Prv_Crc_Calculate_Start;
        break;

        case NvM_Prv_stJob_Crc_Calculate_Do_e:
            Crc_State_pfct = &NvM_Prv_Crc_Calculate_Do;
        break;

        default:
            Crc_State_pfct = NULL_PTR;
        break;
    }

    return Crc_State_pfct;
}

/**
 * \brief
 * This local private job function defines the job state function for the CRC calculation start identified by the job
 * state ID NvM_Prv_stJob_Crc_Calculate_Start_e.
 * \details
 * To start CRC calculation this function intializes the counter for CRC calculation with length of the data
 * for which CRC calculation will be done.
 *
 * Next job state is always the actual CRC calculation processing state NvM_Prv_stJob_Crc_Calculate_Do_e.
 *
 * The CRC calculation start always succeeds.
 *
 * After the CRC calculation start the NvM remains in the same job state machine.
 *
 * \param[in,out] stJob_pen
 * Pointer to the next job state of the CRC calculation
 * \param[in,out] JobResult_pst
 * Pointer to the job results, used elements:
 * - CrcData_st.Calculation_st.cntrCrcCalculation_uo = initialized with data length for CRC calculation
 * - Result_en = NvM_Prv_JobResult_Succeeded_e
 * \param[in] JobData_pcst
 * Pointer to the data of the current NvM job, no job data is used in this job state
 *
 * \return
 * NvM_Prv_idJobStateMachine_DoCrc_e
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Crc_Calculate_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                            NvM_Prv_JobResult_tst* JobResult_pst,
                                                                            NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCrc_e;

    (void)JobData_pcst;

    JobResult_pst->CrcData_st.Calculation_st.cntrCrcCalculation_uo = JobResult_pst->CrcData_st.Calculation_st.Length_u16;

    *stJob_pen = NvM_Prv_stJob_Crc_Calculate_Do_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private job function defines the job state function for the actual CRC calculation identified by the
 * job state ID NvM_Prv_stJob_Crc_Calculate_Do_e.
 * \details
 * This function determines the amount of data in bytes for which the CRC calculation will be done in current cycle,
 * see also NvM_Prv_Crc_Calculate. And after CRC calculation this function decrements the corresponding counter
 * in job results.
 *
 * As long as CRC calculation has not completed (corresponding counter in job results > 0) this function does not update
 * the job state, so that this function will be called again in next cycle.
 * If CRC calculation completes (corresponding counter in job results = 0) this function updates the job state
 * with NvM_Prv_stJob_Crc_Calculate_Completed_e.
 *
 * Current job state machine remains active as long as CRC calculation is in progress.
 * The job-stack changes to the higher-level state machine if CRC calculation completes,
 * see data of current job state machine NvM_Prv_Crc_Calculate_Data_cst.stFinal_en.
 *
 * \param[in,out] stJob_pen
 * Pointer to the next job state of the CRC calculation, possible values:
 * - NvM_Prv_stJob_Crc_Calculate_Do_e = CRC calculation still in progress
 * - NvM_Prv_stJob_Crc_Calculate_Completed_e = CRC calculation completed
 * \param[in,out] JobResult_pst
 * Pointer to the job results, used elements:
 * - CrcData_st.Calculation_st.cntrCrcCalculation_uo = decremented in each cycle until CRC calculation completed
 * - Result_en = NvM_Prv_JobResult_Pending_e (CRC calculation still in progress)
 *             = NvM_Prv_JobResult_Succeeded_e (CRC calculation completed)
 * \param[in] JobData_pcst
 * Pointer to the data of the current NvM job
 *
 * \return
 * NvM_Prv_idJobStateMachine_DoCrc_e
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Crc_Calculate_Do(NvM_Prv_stJob_ten* stJob_pen,
                                                                         NvM_Prv_JobResult_tst* JobResult_pst,
                                                                         NvM_Prv_JobData_tst const* JobData_pcst)
{
    uint16 nrBytesToCalculate_u16;
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCrc_e;

    if (JobResult_pst->CrcData_st.Calculation_st.cntrCrcCalculation_uo <= NVM_CRC_NUM_OF_BYTES)
    {
        nrBytesToCalculate_u16 = JobResult_pst->CrcData_st.Calculation_st.cntrCrcCalculation_uo;
    }
    else
    {
        nrBytesToCalculate_u16 = NVM_CRC_NUM_OF_BYTES;
    }

    NvM_Prv_Crc_Calculate(JobData_pcst->idBlock_uo, nrBytesToCalculate_u16, &JobResult_pst->CrcData_st.Calculation_st);

    JobResult_pst->CrcData_st.Calculation_st.isFirstCall_b = FALSE;
    JobResult_pst->CrcData_st.Calculation_st.cntrCrcCalculation_uo -= nrBytesToCalculate_u16;

    // Check whether CRC calculation has finished (all bytes proceeded)
    if (0u == JobResult_pst->CrcData_st.Calculation_st.cntrCrcCalculation_uo)
    {
        *stJob_pen = NvM_Prv_stJob_Crc_Calculate_Completed_e;
        JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
    }
    else
    {
        JobResult_pst->Result_en = NvM_Prv_JobResult_Pending_e;
    }

    return idJobStateMachineNext_en;
}

#endif  // (NVM_CALC_CRC == STD_ON)
