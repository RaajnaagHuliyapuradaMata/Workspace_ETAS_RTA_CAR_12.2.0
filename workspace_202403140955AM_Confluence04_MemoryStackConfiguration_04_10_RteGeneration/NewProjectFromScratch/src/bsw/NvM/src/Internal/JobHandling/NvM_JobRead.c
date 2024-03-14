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

#include "NvM_Prv_Crc.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_BlockSplit.h"
#include "NvM_Prv_InternalBuffer.h"
#include "NvM_Prv_JobStateMachine.h"
#include "NvM_Prv_ExplicitSynchronization.h"
#include "NvM_Prv_Crc_Inl.h"
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC boolean NvM_Prv_JobRead_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobRead_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                      NvM_Prv_JobResult_tst* JobResult_pst,
                                                                      NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_GetUserData(NvM_Prv_stJob_ten* stJob_pen,
                                                                            NvM_Prv_JobResult_tst* JobResult_pst,
                                                                            NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_CrcCheck(NvM_Prv_stJob_ten* stJob_pen,
                                                                         NvM_Prv_JobResult_tst* JobResult_pst,
                                                                         NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_DoMemIf(NvM_Prv_stJob_ten* stJob_pen,
                                                                        NvM_Prv_JobResult_tst* JobResult_pst,
                                                                        NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_RecalcUserDataCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                                  NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                  NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_CalcNvBlkCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                             NvM_Prv_JobResult_tst* JobResult_pst,
                                                                             NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_ExtractAdminData(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_DoBlockSplit(NvM_Prv_stJob_ten* stJob_pen,
                                                                             NvM_Prv_JobResult_tst* JobResult_pst,
                                                                             NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_DoCrypto(NvM_Prv_stJob_ten* stJob_pen,
                                                                         NvM_Prv_JobResult_tst* JobResult_pst,
                                                                         NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_SetUserData(NvM_Prv_stJob_ten* stJob_pen,
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

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_JobRead_Data_cst =
{
        &NvM_Prv_JobRead_GetStateFct,
        &NvM_Prv_JobRead_IsEnabled,
        NvM_Prv_stJob_Read_Start_e,
        NvM_Prv_stJob_Read_Completed_e,
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

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobRead_GetData(void)
{
    return &NvM_Prv_JobRead_Data_cst;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_JobRead_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // Read is always enabled for all blocks
    return TRUE;
}


/**
 * \brief
 * This private function returns the pointer to the job step for the current state of the read job.
 * \details
 * This function returns a NULL pointer if passed read job state is invalid. In this case the read job will fail.
 *
 * \param[INOUT] stJob_pen
 * Pointer to the current state of the read job
 *
 * \return
 * Pointer to the job step function or NULL pointer for invalid read job state
 */
RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobRead_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobRead_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_Read_Start_e:
            JobRead_State_pfct = &NvM_Prv_JobRead_Start;
        break;

        case NvM_Prv_stJob_Read_GetUserData_e:
            JobRead_State_pfct = &NvM_Prv_JobRead_GetUserData;
        break;

        case NvM_Prv_stJob_Read_CrcCheck_e:
            JobRead_State_pfct = &NvM_Prv_JobRead_CrcCheck;
        break;

        case NvM_Prv_stJob_Read_DoMemIf_e:
            JobRead_State_pfct = &NvM_Prv_JobRead_DoMemIf;
        break;

        case NvM_Prv_stJob_Read_RecalcUserDataCrc_e:
            JobRead_State_pfct = &NvM_Prv_JobRead_RecalcUserDataCrc;
        break;

        case NvM_Prv_stJob_Read_CalcNvBlkCrc_e:
            JobRead_State_pfct = &NvM_Prv_JobRead_CalcNvBlkCrc;
        break;

        case NvM_Prv_stJob_Read_ExtractAdminData_e:
            JobRead_State_pfct = &NvM_Prv_JobRead_ExtractAdminData;
        break;

        case NvM_Prv_stJob_Read_DoBlockSplit_e:
            JobRead_State_pfct = &NvM_Prv_JobRead_DoBlockSplit;
        break;

        case NvM_Prv_stJob_Read_DoCrypto_e:
            JobRead_State_pfct = &NvM_Prv_JobRead_DoCrypto;
        break;

        case NvM_Prv_stJob_Read_SetUserData_e:
            JobRead_State_pfct = &NvM_Prv_JobRead_SetUserData;
        break;

        default:
            JobRead_State_pfct = NULL_PTR;
        break;
    }

    return JobRead_State_pfct;
}

/**
 * \brief
 * This local private function is a read job step function and starts the read job.
 * \details
 * This function determines whether the block read is mandatory or can be skipped due to the configuration.
 * The NvM reads a block from the medium without additional checks in following cases:
 * - temporary RAM block is used for current read job
 * - permanent RAM block is used for current read job but data in the permanent RAM block is invalid
 *
 * The NvM checks whether the block read can be skipped in following cases:
 * - permanent RAM block is used for current read job and data in the permanent RAM block is valid
 *
 * Before skipping the block read the NvM checks whether a CRC check for data in the permanent RAM block is configured.
 * If no CRC check is configured then the NvM finishes the read job immediately.
 * In this case permanent RAM block is valid only because of quick initialization feature.
 * If CRC check is configured then the NvM initiates a CRC check to confirm that data in the permanent RAM block
 * is really valid.
 *
 * \param [inout] stJob_pen
 * State of the read job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Succeeded_e = Read job started successfully, continue the read job (block read or CRC check required)
 * - NvM_Prv_JobResult_Skipped_e  = no read is required because permanent RAM block contains valid data
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                      NvM_Prv_JobResult_tst* JobResult_pst,
                                                                      NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRead_e;

    // TRACE[SWS_NvM_00362] Before reading data from NV memory NvM checks RAM block CRC
    //                      to avoid overwriting of still valid data with outdated data from NV memory
    boolean isPRamBlockValid_e = NvM_Prv_Block_IsPRamBlockValid(JobData_pcst->idBlock_uo);

    if (JobData_pcst->isPRamBlockUsed_b && isPRamBlockValid_e)
    {
        // Permanent RAM block is used for current read request AND RAM block is valid
        // -> check if RAM block CRC is enabled
        if (NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_RAM_CRC))
        {
            // RAM block CRC is enabled
            // -> check if RAM block is consistent

            // TRACE[SWS_NvM_00362] Before NvM can check the RAM block CRC for blocks with configured explicit
            //                      synchronization the RAM block data must be copied into the NvM RAM mirror
            //                      via write callback
            *stJob_pen = NvM_Prv_stJob_Read_GetUserData_e;
            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
            *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo);
        }
        else
        {
            // RAM block CRC is disabled
            // -> RAM block with disabled RAM block CRC can be valid only if quick initialization is active
            // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2802] Request the reading of the block
            //                                          only if the NvM RAM block status bit is not set to VALID.
            *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
            JobResult_pst->Result_en = NvM_Prv_JobResult_Skipped_e;
        }
    }
    else
    {
        // Temporary RAM block is used for current read request OR RAM block is invalid
        // -> read data from NV memory
        *stJob_pen = NvM_Prv_stJob_Read_DoMemIf_e;
        idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Read_e;
        JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a read job step function and gets user data for a CRC check if required.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - permanent RAM block is used for current read job and data in the permanent RAM block is valid
 * - additional CRC check for data in the permanent RAM block is configured by the user to ensure
 *   that data is valid and consistent
 *
 * Before starting the CRC recalculation this function ensures that the job buffer contains user data.
 * Normally the RAM block used in current read job contains user data and no additional actions are required.
 *
 * The NvM has to copy user data into the job buffer only if current block is configured for explicit configuration
 * and permanent RAM block is used for current read job.
 *
 * \param [inout] stJob_pen
 * State of the read job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Succeeded_e  = NvM checks the CRC over the user data
 * - NvM_Prv_JobResult_Pending_e    = copying data into the job buffer is still in progress -> wait 1 cycle
 * - NvM_Prv_JobResult_Failed_e     = NvM continues the read job (CRC check or reading from medium required)
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_GetUserData(NvM_Prv_stJob_ten* stJob_pen,
                                                                            NvM_Prv_JobResult_tst* JobResult_pst,
                                                                            NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRead_e;

    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    if (NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_SYNC_MECHANISM))
    {
        // TRACE[SWS_NvM_00362] Before NvM can check the RAM block CRC for blocks with configured explicit
        //                      synchronization the RAM block data must be copied into the NvM RAM mirror
        //                      via write callback,
        //                      if current block is not configured for explicit synchronization then this step
        //                      always succeeds
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
        *stJob_pen = NvM_Prv_stJob_Read_CrcCheck_e;
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
        // -> read data from NV memory
        *stJob_pen = NvM_Prv_stJob_Read_DoMemIf_e;
        idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Read_e;
        JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
    }
    else
    {
        // If copying data for explicit synchronization is still pending do nothing
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a read job step function and checks the CRC over the user data in permanent RAM block.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - permanent RAM block is used for current read job and data in the permanent RAM block is valid
 * - additional CRC check for data in the permanent RAM block is configured by the user
 * - job buffer contains user data
 *
 * This function recalculates the CRC over the data in the permanent RAM block and checks if it is equal to the CRC
 * calculated during last successful read/write/restore job.
 *
 * \param [inout] stJob_pen
 * State of the read job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Pending_e   = CRC recalculation is still in progress -> wait 1 cycle
 * - NvM_Prv_JobResult_Succeeded_e = NvM continues the read job (reads data from medium) or
 *                                   no read is required (permanent RAM block contains valid data)
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_CrcCheck(NvM_Prv_stJob_ten* stJob_pen,
                                                                         NvM_Prv_JobResult_tst* JobResult_pst,
                                                                         NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRead_e;

    *stJob_pen = NvM_Prv_stJob_Read_CrcCheck_e;

    // Check whether RAM block CRC recalculation has finished
    // Note: RAM block CRC recalculation cannot fail, it is only possible that recalculation takes several cycles.
    //       In this case Result_en will be NvM_Prv_JobStepResult_Pending_e.
    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // CRC calculation finished
        // TRACE[SWS_NvM_00364] Check whether calculated RAM block CRC matches stored RAM block Crc
        if (NvM_Prv_Crc_CheckRamBlockCrc(JobData_pcst->idBlock_uo, &JobResult_pst->CrcData_st.Calculation_st.Crc_un))
        {
            // calculated RAM block CRC matches stored RAM block CRC -> finish read job successfully
            *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_MemIfSkipped_e;
        }
        else
        {
            // Stored RAM CRC is inconsistent -> read data from NV memory
            *stJob_pen = NvM_Prv_stJob_Read_DoMemIf_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Read_e;
            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
        }
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a read job step function and polls the MemIf to complete the read job.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - permanent RAM block is used for current read job and data in the permanent RAM block is invalid OR
 *   temporary RAM block is used
 *
 *  This function determines further job steps dependent on the result of the MemIf job:
 * - MEMIF_JOB_OK             = MemIf read job has succeeded and job buffer contains data read from the medium ->
 *                              - if user data is encrypted and no RAM CRC is configured ->
 *                                calculate the CRC over the data read from the non-volatile memory
 *                              - if user data is not encrypted or RAM CRC is configured ->
 *                                calculate the CRC over the user data read from the non-volatile memory
 * - MEMIF_BLOCK_INCONSISTENT = MemIf has not found valid data on the medium -> start implicit recovery if required
 * - MEMIF_BLOCK_INVALID      = MemIf has read invalidated block -> finish job with corresponding result
 * - MEMIF_JOB_PENDING        = MemIf read job is still in progress -> wait 1 cycle
 * - all other return values  = MemIf read job has failed -> start implicit recovery if required
 *
 * \param [inout] stJob_pen
 * State of the read job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Pending_e           = MemIf job is still pending -> wait 1 cycle
 * - NvM_Prv_JobResult_Succeeded_e         = MemIf job has read data from the medium -> see description above
 * - NvM_Prv_JobResult_BlockInconsistent_e = MemIf job has not found valid data -> start implicit recovery if required
 * - NvM_Prv_JobResult_BlockInvalidated_e  = MemIf job has read invalidated block -> finish job with corresponding result
 * - NvM_Prv_JobResult_Failed_e            = MemIf read job has failed -> start implicit recovery if required
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_DoMemIf(NvM_Prv_stJob_ten* stJob_pen,
                                                                        NvM_Prv_JobResult_tst* JobResult_pst,
                                                                        NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRead_e;

    NvM_BlockIdType idBlock_uo = JobData_pcst->idBlock_uo;

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // MemIf job has succeeded, data read from the non-volatile memory is in the internal buffer

        boolean isIdxSubBlock_b = NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo,
                                                                   JobData_pcst->idxDataset_u8);
        boolean isBlockSplit_b = (NULL_PTR != NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo));

        // Update number of used bytes in internal buffer so that it is correct for further evaluation
        *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 -= NVM_RB_BLOCK_HEADER_LENGTH;

        if (NvM_Prv_BlkDesc_IsBlockSelected(idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_NV_CRC) && !isIdxSubBlock_b)
        {
            // Update number of used bytes in internal buffer so that it is correct for NV CRC calculation
            *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 -= NvM_Prv_Crc_GetSizeCrc(NvM_Prv_BlkDesc_GetCrcType(idBlock_uo));
        }

        if (JobData_pcst->isEncryptionEnabled_b || (isBlockSplit_b && !isIdxSubBlock_b))
        {
            // -> continue with calculation of NV CRC
            *stJob_pen = NvM_Prv_stJob_Read_CalcNvBlkCrc_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCalcNvCrc_e;
        }
        else
        {
            // -> continue with recalculation of RAM CRC over user data
            *stJob_pen = NvM_Prv_stJob_Read_RecalcUserDataCrc_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRecalcUserDataCrc_e;
        }
    }
    // TRACE[SWS_NvM_00882] Read job for NvM_ReadPRAMBlock shall load default values
    //                      if lower layer has read inconsistent value
    else if (NvM_Prv_JobResult_BlockInconsistent_e == JobResult_pst->Result_en)
    {
        // MemIf job has read inconsistent block -> start implicit recovery
        *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
        // The detail job result errorInfoDetail_en reason from MemIf is set right when fetching the MemIf job result
    }
    else if (NvM_Prv_JobResult_BlockInvalidated_e == JobResult_pst->Result_en)
    {
        // MemIf job has returned invalidated block -> finish read job
        *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
    }
    else
    {
        // MemIf job has failed -> start implicit recovery, update final job result
        JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
        *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a read job step function and recalculates the CRC over the user data.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - MemIf read job has succeeded AND (user data is not encrypted and RAM CRC is configured)
 * - User data decryption has succeeded and RAM CRC is configured
 *
 *  This function determines further job steps dependent on user data encryption
 * - user data encrypted     = check if recalculated CRC over user data is equal to the CRC calculated
 *                             during last successful read/write/restore job ->
 *                             - CRC check successful -> copy user data into the RAM block used in current job
 *                             - CRC check not successful ->
 *                                 start implicit recovery if required and
 *                                 set error info detail to crypto failed
 * - user data not encrypted = check if NV CRC is configured ->
 *                             - NV CRC -> complete CRC recalculation over the read data
 *                             - no NV CRC -> extract administrative data from the read data
 *
 * \param [inout] stJob_pen
 * State of the read job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Pending_e   = CRC recalculation is still pending -> wait 1 cycle
 * - NvM_Prv_JobResult_Succeeded_e = CRC recalculation has succeeded -> see description above
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_RecalcUserDataCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                                  NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                  NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRead_e;

    NvM_BlockIdType idBlock_uo = JobData_pcst->idBlock_uo;
    boolean isIdxSubBlock_b = NvM_Prv_BlockSplit_IsIdxSubBlock(idBlock_uo, JobData_pcst->idxDataset_u8);
    boolean isBlockSplit_b = (NULL_PTR != NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo));

    // CRC recalculation cannot fail and can only succeed -> no result evaluation
    // CRC recalculation over the user data has succeeded or is not configured

    if (isIdxSubBlock_b)
    {
        // CRC over read sub-block has been calculated
        // -> check if it is consistent to the CRC in the previously read master-block
        if (NvM_Prv_BlockSplit_IsSubBlockConsistent(idBlock_uo,
                                                    JobData_pcst->idxDataset_u8,
                                                    JobData_pcst->IntBuffer_st.Buffer_pu8,
                                                    &JobResult_pst->CrcData_st.Calculation_st.Crc_un))
        {
            // read sub-block is consistent ->
            // finish read job
            *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
        }
        else
        {
            // read sub-block is inconsistent ->
            // finish read job, update final job result
            *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
            JobResult_pst->Result_en = NvM_Prv_JobResult_BlockInconsistent_e;
            JobResult_pst->errorInfoDetail_en = NvM_Rb_ErrorInfoDetail_BlockSplitFailed_e;
        }
    }
    else
    {
        // Copy CRC over the user data for later usage
        JobResult_pst->CrcData_st.CrcRamBlk_un.Crc32_u32 = JobResult_pst->CrcData_st.Calculation_st.Crc_un.Crc32_u32;

        if (isBlockSplit_b)
        {
            *stJob_pen = NvM_Prv_stJob_Read_SetUserData_e;
        }
        else if (   NvM_Prv_BlkDesc_IsBlockSelected(idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_RAM_CRC)
                 && JobData_pcst->isEncryptionEnabled_b)
        {
            // Read data is encrypted, NvM has arrived in this state after decrypting read data
            // -> check if recalculated CRC over decrypted user data matches the CRC stored with encrypted user data

            // Update number of used bytes in internal buffer so that it is correct for CRC check
            *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = NvM_Prv_BlkDesc_GetSize(idBlock_uo);

            if (NvM_Prv_Crc_CheckCrc(idBlock_uo,
                                     &JobData_pcst->IntBuffer_st.Buffer_pu8[JobData_pcst->IntBuffer_st.OffsetPlainData_u16],
                                     JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16,
                                     &JobResult_pst->CrcData_st.Calculation_st.Crc_un))
            {
                *stJob_pen = NvM_Prv_stJob_Read_SetUserData_e;
            }
            else
            {
                // Crc is not equal
                *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
                JobResult_pst->ProductionError_u8 |= NVM_E_RB_INTEGRITY_FAILED;
                JobResult_pst->errorInfoDetail_en = NvM_Rb_ErrorInfoDetail_CryptoFailed_e;
                JobResult_pst->Result_en = NvM_Prv_JobResult_BlockInconsistent_e;
            }
        }
        else
        {
            // Read data is not encrypted, NvM has arrived in this state after reading data from the non-volatile memory
            // and recalculating CRC over user data
            // -> continue with Nv CRC calculation
            *stJob_pen = NvM_Prv_stJob_Read_CalcNvBlkCrc_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCalcNvCrc_e;
        }
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a read job step function and calculates the CRC over the data read
 * from the non-volatile memory.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - MemIf read job has succeeded AND (user data is encrypted or no RAM CRC is configured),
 *   i.e. this function calculates the CRC over whole data read from the non-volatile memory
 * - User data is not encrypted and CRC over user data has been recalculated,
 *   i.e. this function calculates the CRC over the remaining data read from the non-volatile memory
 *
 *  This function determines further job steps dependent on the consistency of the CRC read from the non-volatile memory
 * - calculated CRC matches the CRC read from the non-volatile memory or no NV CRC is configured ->
 *   extract administrative data from the read data
 * - calculated CRC doesn't match the CRC read from the non-volatile memory ->
 *   start implicit recovery if required and set error info detail to inconsistent data
 *
 * \param [inout] stJob_pen
 * State of the read job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Pending_e   = CRC recalculation is still pending -> wait 1 cycle
 * - NvM_Prv_JobResult_Succeeded_e = CRC recalculation has succeeded -> see description above
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_CalcNvBlkCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                             NvM_Prv_JobResult_tst* JobResult_pst,
                                                                             NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRead_e;

    uint16 OffsetToSubBlock_u16 = NvM_Prv_BlockSplit_GetOffsetToSubBlock(JobData_pcst->idBlock_uo,
                                                                          JobData_pcst->idxDataset_u8);

    // CRC calculation over data read from the non-volatile memory has succeeded
    // (CRC calculation cannot fail -> no result evaluation required)
    // Extract and compare the Nv CRC read from the non-volatile memory against the calculated CRC
    // (Hint if NV CRC is not configured for current block then no check is done)
    /* MR12 RULE 13.5 VIOLATION: if Nv-CRC is configured, then Nv-CRC shall be checked, no unexpected side-effects */
    if (   !NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_NV_CRC)
        || NvM_Prv_Crc_CheckCrc(JobData_pcst->idBlock_uo,
                &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetToSubBlock_u16],
                JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16,
                &JobResult_pst->CrcData_st.Calculation_st.Crc_un))
    {
        // NV-CRC is not configured or calculated CRC matches the CRC read from the non-volatile memory
        // -> read data is consistent, extract administrative data from the read data
        *stJob_pen = NvM_Prv_stJob_Read_ExtractAdminData_e;
    }
    else
    {
        // NV-CRC is configured and calculated CRC does not match the CRC read from the non-volatile memory
        // -> read data is inconsistent, start implicit recovery
        JobResult_pst->ProductionError_u8 |= NVM_E_RB_INTEGRITY_FAILED;
        JobResult_pst->Result_en = NvM_Prv_JobResult_BlockInconsistent_e;
        JobResult_pst->errorInfoDetail_en = NvM_Rb_ErrorInfoDetail_InconsistentData_e;
        *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a read job step function and extracts administrative data out of the data read
 * from the non-volatile memory.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - CRC read from the non-volatile memory (NV CRC) is consistent
 * - no NV CRC is configured
 * This function extracts and checks the block version if configured and extracts the write counter if enabled.
 * - If the block version matches the expected one -> crypto operations will be done next
 * - If the block version does not match the expected one -> implicit recovery will be started and
 *     the error info detail will be set to version mismatch
 *
 * \param [inout] stJob_pen
 * State of the read job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Succeeded_e = administrative data extracted -> handle cryptographic services
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_ExtractAdminData(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRead_e;

    uint16 OffsetToSubBlock_u16;
    boolean isBlockVersionValid_b;

    OffsetToSubBlock_u16 = NvM_Prv_BlockSplit_GetOffsetToSubBlock(JobData_pcst->idBlock_uo,
                                                                  JobData_pcst->idxDataset_u8);

    // If the block version is configured for the given block, extract block version out of the data read from
    // the non-volatile memory and return if the read block version is valid or not.
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3351] NvM shall check the block version on read and compare it with
    //                                          the configured one.
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3352] NvM shall load the read data into RAM block only if the
    //                                          configured block version matches the read block version.
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3352] NvM shall load configured default values into RAM block if the
    //                                          configured block version mismatches the read block version.
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3352] NvM shall let the RAM block unchanged if the configured block version
    //                                          mismatches the read block version and no default values are available.
    isBlockVersionValid_b = NvM_Prv_BlkDesc_ExtractCheckBlockVersion(JobData_pcst->idBlock_uo,
                                                                     &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetToSubBlock_u16],
                                                                     JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16);

    // Extract write counter out of the data read from the non-volatile memory
    NvM_Prv_Block_ExtractWriteCounter(JobData_pcst->idBlock_uo,
                                      &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetToSubBlock_u16],
                                      JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16);

    if (isBlockVersionValid_b)
    {
        *stJob_pen = NvM_Prv_stJob_Read_DoBlockSplit_e;
        idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoBlockSplit_Read_e;
        JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
    }
    else
    {
        JobResult_pst->Result_en = NvM_Prv_JobResult_BlockInconsistent_e;
        JobResult_pst->errorInfoDetail_en = NvM_Rb_ErrorInfoDetail_VersionMismatch_e;
        *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
    }

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_DoBlockSplit(NvM_Prv_stJob_ten* stJob_pen,
                                                                             NvM_Prv_JobResult_tst* JobResult_pst,
                                                                             NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRead_e;

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // Handling split blocks for read job have succeeded or are not configured
        // check if a sub-block is in use
        if (NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo, JobData_pcst->idxDataset_u8))
        {
            // -> read sub-block from the non-volatile memory
            *stJob_pen = NvM_Prv_stJob_Read_DoMemIf_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Read_e;
        }
        else
        {
            *stJob_pen = NvM_Prv_stJob_Read_DoCrypto_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoRead_e;
        }
        //JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
    }
    else
    {
        // Handling split blocks for read job have failed ->
        // finish write job, update final job result
        JobResult_pst->errorInfoDetail_en = NvM_Rb_ErrorInfoDetail_BlockSplitFailed_e;
        *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a read job step function and handles cryptographic services.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - data read from the non-volatile memory is consistent or no NV CRC is configured
 *
 * This function executes the state machine for cryptographic read job
 * (see NvM_Prv_Crypto_DoStateMachine and NvM_Prv_Crypto_GetJobStepRead).
 *
 * After completing cryptographic read job this function determines further job steps dependent on user data encryption:
 * - user data encrypted     -> recalculate CRC over the decrypted user data to check it for consistency
 *                              (see state NvM_Prv_JobRead_RecalcUserDataCrc)
 * - user data not encrypted ->  copy user data into the RAM block
 *
 * \param [inout] stJob_pen
 * State of the read job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Pending_e           = Cryptographic services are still pending -> wait 1 cycle
 * - NvM_Prv_JobResult_BlockInconsistent_e = Cryptographic services has found inconsistent data ->
 *                                           start implicit recovery if required, set error info detail to crypto failed
 * - NvM_Prv_JobResult_Failed_e            = Cryptographic services has failed -> start implicit recovery if required
 *                                            and set error info detail to crypto failed
 * - NvM_Prv_JobResult_Succeeded_e         = Cryptographic services has succeeded -> see description above
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_DoCrypto(NvM_Prv_stJob_ten* stJob_pen,
                                                                         NvM_Prv_JobResult_tst* JobResult_pst,
                                                                         NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRead_e;

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // Cryptographic services for read job have succeeded or are not configured

        boolean isBlockSplit_b = (NULL_PTR != NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo));
        boolean isRamCrcConfigured_b = NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo,
                                                                       NVM_PRV_BLOCK_FLAG_USE_RAM_CRC);

        if ((JobData_pcst->isEncryptionEnabled_b && isRamCrcConfigured_b) || isBlockSplit_b)
        {
            // read data is encrypted or a sub-block has been read
            // -> calculate the CRC over the decrypted user data or sub-block data
            *stJob_pen = NvM_Prv_stJob_Read_RecalcUserDataCrc_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRecalcUserDataCrc_e;
        }
        else
        {
            // read data is not encrypted AND master-block of a split-block is in progress
            // -> copy read user data into the RAM block
            *stJob_pen = NvM_Prv_stJob_Read_SetUserData_e;
        }
    }
    else
    {
        *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
        JobResult_pst->errorInfoDetail_en = NvM_Rb_ErrorInfoDetail_CryptoFailed_e;
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a read job step function and copies read data into the RAM block used for current job.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - data read from the non-volatile memory is decrypted (if required) and consistent
 *
 * \param [inout] stJob_pen
 * State of the read job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Pending_e   = Copying of user data is still pending -> wait 1 cycle
 * - NvM_Prv_JobResult_Failed_e    = Copying of user data has failed -> finish job
 * - NvM_Prv_JobResult_Succeeded_e = Copying of user data has succeeded -> finish job
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRead_SetUserData(NvM_Prv_stJob_ten* stJob_pen,
                                                                            NvM_Prv_JobResult_tst* JobResult_pst,
                                                                            NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRead_e;

    uint16 OffsetIntBuffer_uo = JobData_pcst->IntBuffer_st.OffsetPlainData_u16;

    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    if (   NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_SYNC_MECHANISM)
        && JobData_pcst->isPRamBlockUsed_b)
    {
        JobResult_pst->Result_en = NvM_Prv_ExplicitSync_CopyData(NvM_Prv_BlkDesc_GetCopyFctForRead(JobData_pcst->idBlock_uo),
                                                                 JobData_pcst->idBlock_uo,
                                                                 &JobResult_pst->cntrExpSyncOperations_u8,
                                                                 &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetIntBuffer_uo]);
    }
    else
    {
        if (JobData_pcst->isIntBufferToUse_b)
        {
            JobResult_pst->Result_en = NvM_Prv_InternalBuffer_CopyData(JobData_pcst->idBlock_uo,
                                                                       JobData_pcst->UserData_un,
                                                                       &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetIntBuffer_uo],
                                                                       FALSE);
        }
    }

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // Copying data for explicit synchronization has succeeded or was not required ->
        // Job is finished, update final job result and save CRC

        if (   NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_RAM_CRC)
            && JobData_pcst->isPRamBlockUsed_b)
        {
            // Set calculated CRC if permanent RAM block is used
            NvM_Prv_Crc_SetRamBlockCrc(JobData_pcst->idBlock_uo, &JobResult_pst->CrcData_st.CrcRamBlk_un);
        }

        NvM_Prv_BlockSplit_UpdateMasterBlock(JobData_pcst->idBlock_uo, JobData_pcst->IntBuffer_st.Buffer_pu8);

        *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
    }
    else if (NvM_Prv_JobResult_Failed_e == JobResult_pst->Result_en)
    {
        // Copying data for explicit synchronization has failed ->
        // finish read job, update final job result
        *stJob_pen = NvM_Prv_stJob_Read_Completed_e;
        JobResult_pst->Result_en = NvM_Prv_JobResult_FailedNoRecovery_e;
    }
    else
    {
        // Copying data for explicit synchronization is still pending ->
        // remain in this state, do not update final job result
    }

    return idJobStateMachineNext_en;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

