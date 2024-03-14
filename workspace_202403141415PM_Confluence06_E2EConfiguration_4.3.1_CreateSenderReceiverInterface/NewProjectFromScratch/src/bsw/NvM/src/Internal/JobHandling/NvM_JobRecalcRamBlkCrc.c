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

#include "NvM_Prv_Crc.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_JobStateMachine.h"
#include "NvM_Prv_ExplicitSynchronization.h"
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#if (NVM_CALC_RAM_BLOCK_CRC == STD_ON)

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobRecalcRamBlkCrc_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC boolean NvM_Prv_JobRecalcRamBlkCrc_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRecalcRamBlkCrc_ExplicitSyncWriteToNvM(NvM_Prv_stJob_ten* stJob_pen,
                                                                                                  NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                                  NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRecalcRamBlkCrc_DoCrc(NvM_Prv_stJob_ten* stJob_pen,
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

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_JobRecalcRamBlkCrc_Data_cst =
{
        &NvM_Prv_JobRecalcRamBlkCrc_GetStateFct,
        &NvM_Prv_JobRecalcRamBlkCrc_IsEnabled,
        NvM_Prv_stJob_RecalcRamBlkCrc_ExplicitSyncWriteToNvM_e,
        NvM_Prv_stJob_RecalcRamBlkCrc_Completed_e,
        NvM_Prv_JobResource_nrCluster_e

};

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

#endif  // (NVM_CALC_RAM_BLOCK_CRC == STD_ON)

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobRecalcRamBlkCrc_GetData(void)
{
#if (NVM_CALC_RAM_BLOCK_CRC == STD_ON)
    return &NvM_Prv_JobRecalcRamBlkCrc_Data_cst;
#else
    return NULL_PTR;
#endif
}

#if (NVM_CALC_RAM_BLOCK_CRC == STD_ON)

RBA_MEMLIB_STATIC boolean NvM_Prv_JobRecalcRamBlkCrc_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    // RAM block CRC recalculation only possible for blocks with configured CRC recalculation
    return NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_RAM_CRC);
}

/**
 * \brief
 * This private function returns the pointer to the job step for the current state of the job for the recalculation
 * of the CRC over the data in the permanent RAM block .
 * \details
 * This function returns a NULL pointer if passed job state is invalid. In this case the job will fail.
 *
 * \param[INOUT] stJob_pen
 * Pointer to the current state of the read job
 *
 * \return
 * Pointer to the job step function or NULL pointer for invalid job state
 */
RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobRecalcRamBlkCrc_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobRecalcRamBlkCrc_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_RecalcRamBlkCrc_ExplicitSyncWriteToNvM_e:
            JobRecalcRamBlkCrc_State_pfct = &NvM_Prv_JobRecalcRamBlkCrc_ExplicitSyncWriteToNvM;
        break;

        case NvM_Prv_stJob_RecalcRamBlkCrc_Do_e:
            JobRecalcRamBlkCrc_State_pfct = &NvM_Prv_JobRecalcRamBlkCrc_DoCrc;
        break;

        default:
            JobRecalcRamBlkCrc_State_pfct = NULL_PTR;
        break;
    }

    return JobRecalcRamBlkCrc_State_pfct;
}


/**
 * \brief
 * This local private function is a job step function and gets user data for a CRC recalculation if required.
 * \details
 * Before starting the CRC recalculation this function ensures that the job buffer contains user data.
 *
 * The NvM has to copy user data into the job buffer only if current block is configured for explicit configuration.
 *
 * \param [inout] stJob_pen
 * State of the job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Succeeded_e  = Copying of user data has succeeded or is not required -> recalculate CRC over the user data
 * - NvM_Prv_JobResult_Pending_e    = Copying data into the job buffer is still in progress -> wait 1 cycle
 * - NvM_Prv_JobResult_Failed_e     = Copying of user data has failed -> finish job
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRecalcRamBlkCrc_ExplicitSyncWriteToNvM(NvM_Prv_stJob_ten* stJob_pen,
                                                                                                  NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                                  NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRecalcRamBklCrc_e;

    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    if (NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_SYNC_MECHANISM))
    {
        // TRACE[SWS_NvM_00362] Before NvM can check the RAM block CRC for blocks with configured explicit
        //                      synchronization the RAM block data must be copied into the NvM RAM mirror
        //                      via write callback,
        JobResult_pst->Result_en = NvM_Prv_ExplicitSync_CopyData(NvM_Prv_BlkDesc_GetCopyFctForWrite(JobData_pcst->idBlock_uo,
                                                                                                    JobData_pcst->IntBuffer_st.Buffer_pu8),
                                                                 JobData_pcst->idBlock_uo,
                                                                 &JobResult_pst->cntrExpSyncOperations_u8,
                                                                 JobData_pcst->IntBuffer_st.Buffer_pu8);
    }

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // TRACE[SWS_NvM_00362] Copying data for explicit synchronization has succeeded or
        //                      current block is not configured for explicit synchronization ->
        //                      Initiate RAM block CRC recalculation
        *stJob_pen = NvM_Prv_stJob_RecalcRamBlkCrc_Do_e;
        idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCrc_e;
        JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

        // Prepare CRC recalculation over the user data
        JobResult_pst->CrcData_st.Calculation_st.isFirstCall_b = TRUE;
        JobResult_pst->CrcData_st.Calculation_st.Length_u16 = NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo);
        JobResult_pst->CrcData_st.Calculation_st.Crc_un.Crc32_u32 = 0u;
        JobResult_pst->CrcData_st.Calculation_st.Buffer_pu8 = JobData_pcst->IntBuffer_st.Buffer_pu8;
    }
    else if (NvM_Prv_JobResult_Failed_e == JobResult_pst->Result_en)
    {
        // If copying data for explicit synchronization has failed then no CRC check can be done
        // -> Signal that it failed so that in NvM_Prv_UpdateBlockStatus_RecalcRamBlkCrc()
        // the further processing can be done the same way as if data has been changed.
        *stJob_pen = NvM_Prv_stJob_RecalcRamBlkCrc_Completed_e;
    }
    else
    {
        // If copying data for explicit synchronization is still pending do nothing
    }

    return idJobStateMachineNext_en;
}


/**
 * \brief
 * This local private function is a job step function and recalculates the CRC over the user data in permanent RAM block.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - job buffer contains user data
 *
 * \param [inout] stJob_pen
 * State of the job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Pending_e   = CRC recalculation is still in progress -> wait 1 cycle
 * - NvM_Prv_JobResult_Succeeded_e = CRC recalculation completed -> finish job
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRecalcRamBlkCrc_DoCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRecalcRamBklCrc_e;

    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo);
    *stJob_pen = NvM_Prv_stJob_RecalcRamBlkCrc_Completed_e;

    // Copy CRC over the user data for later usage
    JobResult_pst->CrcData_st.CrcRamBlk_un.Crc32_u32 = JobResult_pst->CrcData_st.Calculation_st.Crc_un.Crc32_u32;

    return idJobStateMachineNext_en;
}

#endif  // (NVM_CALC_RAM_BLOCK_CRC == STD_ON)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

