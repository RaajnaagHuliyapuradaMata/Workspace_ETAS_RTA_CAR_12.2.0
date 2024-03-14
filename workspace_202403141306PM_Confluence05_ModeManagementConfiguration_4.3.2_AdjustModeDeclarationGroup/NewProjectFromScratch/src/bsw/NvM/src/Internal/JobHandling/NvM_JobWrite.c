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
#include "NvM_Prv_Crc.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_BlockSplit.h"
#include "NvM_Prv_InternalBuffer.h"
#include "NvM_Prv_JobStateMachine.h"
#include "NvM_Prv_ExplicitSynchronization.h"
/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_JobWrite_IsWriteRequired(NvM_Prv_Crc_tun const* Crc_pcun,
                                                                 NvM_Prv_JobData_tst const* JobData_pcst);

/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobWrite_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC boolean NvM_Prv_JobWrite_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_GetUserData(NvM_Prv_stJob_ten* stJob_pen,
                                                                             NvM_Prv_JobResult_tst* JobResult_pst,
                                                                             NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_RecalcUserDataCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                   NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_DoBlockSplit(NvM_Prv_stJob_ten* stJob_pen,
                                                                              NvM_Prv_JobResult_tst* JobResult_pst,
                                                                              NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_DoCrypto(NvM_Prv_stJob_ten* stJob_pen,
                                                                          NvM_Prv_JobResult_tst* JobResult_pst,
                                                                          NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_AppendAdminData(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_CalcNvCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                           NvM_Prv_JobResult_tst* JobResult_pst,
                                                                           NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_AppendBlockHeader(NvM_Prv_stJob_ten* stJob_pen,
                                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                   NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_DoMemIf(NvM_Prv_stJob_ten* stJob_pen,
                                                                         NvM_Prv_JobResult_tst* JobResult_pst,
                                                                         NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_DoBlockSplitInvalidate(NvM_Prv_stJob_ten* stJob_pen,
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

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_JobWrite_Data_cst =
{
        &NvM_Prv_JobWrite_GetStateFct,
        &NvM_Prv_JobWrite_IsEnabled,
        NvM_Prv_stJob_Write_GetUserData_e,
        NvM_Prv_stJob_Write_Completed_e,
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

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobWrite_GetData(void)
{
    return &NvM_Prv_JobWrite_Data_cst;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_JobWrite_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // Write is always enabled for all blocks
    return TRUE;
}

/**
 * \brief
 * This private function returns the pointer to the job step for the current state of the write job.
 * \details
 * This function returns a NULL pointer if passed write job state is invalid. In this case the write job will fail.
 *
 * \param [inout] stJob_en
 * Pointer to the current state of the write job
 *
 * \return
 * Pointer to the job step function or NULL pointer for invalid write job state
 */
RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobWrite_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobWrite_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_Write_GetUserData_e:
            JobWrite_State_pfct = &NvM_Prv_JobWrite_GetUserData;
        break;

        case NvM_Prv_stJob_Write_RecalcUserDataCrc_e:
            JobWrite_State_pfct = &NvM_Prv_JobWrite_RecalcUserDataCrc;
        break;

        case NvM_Prv_stJob_Write_DoBlockSplit_e:
            JobWrite_State_pfct = &NvM_Prv_JobWrite_DoBlockSplit;
        break;

        case NvM_Prv_stJob_Write_DoCrypto_e:
            JobWrite_State_pfct = &NvM_Prv_JobWrite_DoCrypto;
        break;

        case NvM_Prv_stJob_Write_AppendAdminData_e:
            JobWrite_State_pfct = &NvM_Prv_JobWrite_AppendAdminData;
        break;

        case NvM_Prv_stJob_Write_AppendBlockHeader_e:
            JobWrite_State_pfct = &NvM_Prv_JobWrite_AppendBlockHeader;
        break;

        case NvM_Prv_stJob_Write_CalcNvBlkCrc_e:
            JobWrite_State_pfct = &NvM_Prv_JobWrite_CalcNvCrc;
        break;

        case NvM_Prv_stJob_Write_DoMemIf_e:
            JobWrite_State_pfct = &NvM_Prv_JobWrite_DoMemIf;
        break;

        case NvM_Prv_stJob_Write_DoBlockSplitInvalidate_e:
            JobWrite_State_pfct = &NvM_Prv_JobWrite_DoBlockSplitInvalidate;
        break;

        default:
            JobWrite_State_pfct = NULL_PTR;
        break;
    }

    return JobWrite_State_pfct;
}

/**
 * \brief
 * This local private function is a write job step function and copies user data into the internal buffer if required.
 * \details
 * After successful copying the NvM internal buffer contains user data.
 *
 * \param [inout] stJob_pen
 * Pointer to the current job state
 * \param [inout] JobResult_pst
 * Pointer to the job results,  possible values:
 * - NvM_Prv_JobResult_Pending_e   = Copying of user data is still pending -> wait 1 cycle
 * - NvM_Prv_JobResult_Failed_e    = Copying of user data has failed -> finish write job
 * - NvM_Prv_JobResult_Succeeded_e = Copying of user data has succeeded -> recalculate CRC over user data
 *                                                                         (CRC will be used to check if write can be omitted)
 * \param [in] JobData_pcst
 * Constant pointer to the job data
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_GetUserData(NvM_Prv_stJob_ten* stJob_pen,
                                                                             NvM_Prv_JobResult_tst* JobResult_pst,
                                                                             NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoWrite_e;

    uint16 OffsetPlainData_u16 = JobData_pcst->IntBuffer_st.OffsetPlainData_u16;
    uint8* IntBuffer_pu8 = &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetPlainData_u16];

    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

#if (NVM_RB_REQ_MONITOR == NVM_RB_REQ_MON_WRITE)
    // In case write-monitoring is active and the block to be written is the WriteMon-block itself the counters
    // have to be increased before the CRC is calculated. This can be safely done here as the WriteMon-block is
    // always written.
    // CAUTION: WriteMonitorCounters shall be updated before copying block data into the internal buffer.
    //          Otherwise NvM will copy outdated values into the internal buffer.
    //          This function updates counter only for the WriteMon-block, for all other blocks this function
    //          does nothing.
    NvM_Prv_WriteMonCntrHandling(JobData_pcst->idBlock_uo,
                                 NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo),
                                 TRUE);
#endif

    *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo);

    if (   !NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo, JobData_pcst->idxDataset_u8)
        && JobData_pcst->isIntBufferToUse_b)
    {
        // Check whether explicit synchronization is enabled and permanent RAM block is used
        // Attention: if temporary RAM block is used for blocks with explicit synchronization
        //            then user data will be copied into the internal buffer via NvM_Prv_InternalBuffer_CopyData
        if (   NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_SYNC_MECHANISM)
            && JobData_pcst->isPRamBlockUsed_b)
        {
            JobResult_pst->Result_en = NvM_Prv_ExplicitSync_CopyData(NvM_Prv_BlkDesc_GetCopyFctForWrite(JobData_pcst->idBlock_uo,
                                                                                                        IntBuffer_pu8),
                                                                     JobData_pcst->idBlock_uo,
                                                                     &JobResult_pst->cntrExpSyncOperations_u8,
                                                                     IntBuffer_pu8);
        }
        else
        {
            JobResult_pst->Result_en = NvM_Prv_InternalBuffer_CopyData(JobData_pcst->idBlock_uo,
                                                                       JobData_pcst->UserData_un,
                                                                       IntBuffer_pu8,
                                                                       TRUE);
        }
    }

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // Copying data into the internal buffer has succeeded or was not required

        // -> continue with recalculation of RAM CRC over user data
        // TRACE[SWS_NvM_00212] Write job for NvM_WriteBlock shall recalculate CRC over the RAM block
        //                      before the copy process if configured
        // TRACE[SWS_NvM_00801] Write job for NvM_WritePRAMBlock shall recalculate CRC over the RAM block
        //                      before the copy process if configured
        *stJob_pen = NvM_Prv_stJob_Write_RecalcUserDataCrc_e;
        idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRecalcUserDataCrc_e;

    }
    else if (NvM_Prv_JobResult_Failed_e == JobResult_pst->Result_en)
    {
        // Copying data for explicit synchronization has failed ->
        // finish write job
        *stJob_pen = NvM_Prv_stJob_Write_Completed_e;
    }
    else
    {
        // Copying data into the internal buffer is still pending ->
        // remain in this state, job result is pending
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a write job step function and recalculates the CRC over user data if configured.
 * \details
 * The NvM reaches this job step only if job buffer contains user data.
 *
 * This function determines further job steps dependent on the RAM CRC configuration
 * - RAM CRC configured    = check if user has changed data compared to last successful read/write/restore ->
 *                           - user has changed data     -> continue with cryptographic services
 *                           - user has not changed data -> finish job
 * - no RAM CRC configured = continue with cryptographic services
 *
 * \param [inout] stJob_pen
 * Pointer to the current job state
 * \param [inout] JobResult_pst
 * Pointer to the job results,  possible values:
 * - NvM_Prv_JobResult_Pending_e                = CRC recalculation is still pending -> wait 1 cycle
 * - NvM_Prv_JobResult_Succeeded_e              = CRC recalculation has succeeded or not configured, user data changed ->
 *                                                continue with cryptographic services
 * - NvM_Prv_JobResult_Succeeded_MemIfSkipped_e = CRC recalculation has succeeded, user data not changed ->
 *                                                finish job
 * \param [in] JobData_pcst
 * Constant pointer to the job data
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_RecalcUserDataCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                   NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoWrite_e;

    // CRC recalculation over the user data has succeeded (CRC recalculation cannot fail) ->
    // check whether the write is required

    // Write is required in following cases:
    // - User has changed data in PRAM block and notified NvM about it via NvM_SetRamBlockStatus,
    //   NvM has recalculated the CRC over new user data in background (see NvM_JobRecalcRamBlkCrc.c) and
    //   has determined that data has been changed -> write is required without additional CRC checks
    // - CRC compare mechanism is enabled for current block->
    //   check if user wants to write data which has been changed comparing to last successful read or write
    // - sub-block has been changed
    if (NvM_Prv_JobWrite_IsWriteRequired(&JobResult_pst->CrcData_st.Calculation_st.Crc_un, JobData_pcst))
    {
        // write is required -> continue with split blocks
        *stJob_pen = NvM_Prv_stJob_Write_DoBlockSplit_e;
        idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoBlockSplit_Write_e;

        if (!NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo, JobData_pcst->idxDataset_u8))
        {
            // Copy CRC over the user data for later usage
            JobResult_pst->CrcData_st.CrcRamBlk_un.Crc32_u32 =
                    JobResult_pst->CrcData_st.Calculation_st.Crc_un.Crc32_u32;
        }
    }
    else
    {
        // recalculated CRC is equal to RAM block CRC ->
        // NvM can finish write job successfully without writing into the NV memory
        // TRACE[SWS_NvM_00852]
        // TRACE[SWS_NvM_00854] Skip writing because the data has not been changed since last successful
        //                      read or write job
        JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_MemIfSkipped_e;
        *stJob_pen = NvM_Prv_stJob_Write_Completed_e;
    }

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_DoBlockSplit(NvM_Prv_stJob_ten* stJob_pen,
                                                                              NvM_Prv_JobResult_tst* JobResult_pst,
                                                                              NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoWrite_e;

    (void)JobData_pcst;

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // Handling split blocks for write job have succeeded or are not configured
        // check if a sub-block is in use
        if (NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo, JobData_pcst->idxDataset_u8))
        {
            // -> write sub-block into the non-volatile memory
            *stJob_pen = NvM_Prv_stJob_Write_DoMemIf_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Write_e;
        }
        else
        {
            // -> continue with cryptographic services
            *stJob_pen = NvM_Prv_stJob_Write_DoCrypto_e;
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoWrite_e;
        }
        //JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
    }
    else
    {
        // Handling split blocks for write job have failed ->
        // finish write job, update final job result
        JobResult_pst->errorInfoDetail_en = NvM_Rb_ErrorInfoDetail_BlockSplitFailed_e;
        *stJob_pen = NvM_Prv_stJob_Write_Completed_e;
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a write job step function and handles cryptographic services if configured.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - CRC recalculation has succeeded
 * - write is required
 *
 * This function executes the state machine for cryptographic write job
 * (see NvM_Prv_Crypto_DoStateMachine and NvM_Prv_Crypto_GetJobStepWrite).
 *
 * \param [inout] stJob_pen
 * Pointer to the current job state
 * \param [inout] JobResult_pst
 * Pointer to the job results,  possible values:
 * - NvM_Prv_JobResult_Pending_e   = Cryptographic services are still pending -> wait 1 cycle
 * - NvM_Prv_JobResult_Failed_e    = Cryptographic services has failed -> finish job and
 *                                     set error info detail to crypto failed
 * - NvM_Prv_JobResult_Succeeded_e = Cryptographic services has succeeded -> append administrative data
 * \param [in] JobData_pcst
 * Constant pointer to the job data
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_DoCrypto(NvM_Prv_stJob_ten* stJob_pen,
                                                                          NvM_Prv_JobResult_tst* JobResult_pst,
                                                                          NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoWrite_e;

    (void)JobData_pcst;

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // Cryptographic services for write job have succeeded or are not configured
        // -> append administrative data to the internal buffer which will be written into the non-volatile memory
        *stJob_pen = NvM_Prv_stJob_Write_AppendAdminData_e;
        //JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
    }
    else
    {
        // Cryptographic services for write job have failed ->
        // finish write job, update final job result
        JobResult_pst->errorInfoDetail_en = NvM_Rb_ErrorInfoDetail_CryptoFailed_e;
        *stJob_pen = NvM_Prv_stJob_Write_Completed_e;
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a write job step function and appends administrative data to the data to be written
 * into the non-volatile memory.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - write is required
 * - cryptographic services have succeeded
 *
 * This function always continues with CRC calculation over the data to be written into the non-volatile memory (NV CRC).
 *
 * \param [inout] stJob_pen
 * Pointer to the current job state
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Succeeded_e = administrative data appended -> calculate NV CRC
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_AppendAdminData(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoWrite_e;

    // Increase write counter
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3287] NvM shall increase the counter before initiating write operation
    //                                         on lower layer (MemIf) if enabled
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3288] NvM shall store write counter persistently together with user data
    //                                         if enabled
    // If NvM has reached this state during a write job then a write operation on lower level will be initiated
    // definitely
    uint16 BlockVersionSize_u16 = 0u;
    uint16 OffsetToSubBlock_u16 = NvM_Prv_BlockSplit_GetOffsetToSubBlock(JobData_pcst->idBlock_uo,
                                                                         JobData_pcst->idxDataset_u8);
    uint16 UsedSize_u16 = *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 + OffsetToSubBlock_u16;
    uint16 CntrSize_u16 = NvM_Prv_Block_AppendWriteCounter(JobData_pcst->idBlock_uo,
                                                           &JobData_pcst->IntBuffer_st.Buffer_pu8[UsedSize_u16]);

    UsedSize_u16 += CntrSize_u16;
    *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = UsedSize_u16;

    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3349] NvM shall store the block version persistently together with user data
    //                                          if the block version is configured
    BlockVersionSize_u16 = NvM_Prv_BlkDesc_AppendBlockVersion(JobData_pcst->idBlock_uo,
                                                              &JobData_pcst->IntBuffer_st.Buffer_pu8[UsedSize_u16]);
    *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 += BlockVersionSize_u16;

    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    // -> continue with appending block header
    *stJob_pen = NvM_Prv_stJob_Write_AppendBlockHeader_e;

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a write job step function and appends block header to the data to be written
 * into the non-volatile memory.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - write is required
 * - cryptographic services have succeeded
 *
 * This function always continues with CRC calculation over the data to be written into the non-volatile memory (NV CRC).
 *
 * \param [inout] stJob_pen
 * Pointer to the current job state
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Succeeded_e = block header appended -> calculate NV CRC
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_AppendBlockHeader(NvM_Prv_stJob_ten* stJob_pen,
                                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                   NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoWrite_e;

    boolean isIdxSubBlock_b = NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo,
                                                               JobData_pcst->idxDataset_u8);
    boolean isBlockSplit_b = (NULL_PTR != NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo));
    uint16 OffsetToSubBlock_u16 = NvM_Prv_BlockSplit_GetOffsetToSubBlock(JobData_pcst->idBlock_uo,
                                                                         JobData_pcst->idxDataset_u8);

    NvM_Prv_BlkDesc_AppendBlockHeader(JobData_pcst->idBlock_uo, JobData_pcst->IntBuffer_st.BlockHeader_pu8);

    // -> continue with calculation of NV CRC
    *stJob_pen = NvM_Prv_stJob_Write_CalcNvBlkCrc_e;
    idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCalcNvCrc_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
    // Update number of used bytes in internal buffer for master-block of a split-block so that it is correct for NV CRC
    if (isBlockSplit_b && !isIdxSubBlock_b)
    {
        *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 -= OffsetToSubBlock_u16;
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a write job step function and calculates the CRC over data to be written
 * into the non-volatile memory if configured.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - write is required
 * - cryptographic services have succeeded
 * - job buffer contains all data beside the block header to be written
 *
 * This function always continues with appending block header.
 *
 * \param [inout] stJob_pen
 * Pointer to the current job state
 * \param [inout] JobResult_pst
 * Pointer to the job results,  possible values:
 * - NvM_Prv_JobResult_Pending_e   = CRC calculation is still pending -> wait 1 cycle
 * - NvM_Prv_JobResult_Succeeded_e = CRC calculation has succeeded or is not configured -> continue with MemIf job
 * \param [in] JobData_pcst
 * Constant pointer to the job data
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_CalcNvCrc(NvM_Prv_stJob_ten* stJob_pen,
                                                                           NvM_Prv_JobResult_tst* JobResult_pst,
                                                                           NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoWrite_e;

    // Append the Crc at the end of the buffer
    if (NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_NV_CRC))
    {
        uint16 OffsetToSubBlock_u16 = NvM_Prv_BlockSplit_GetOffsetToSubBlock(JobData_pcst->idBlock_uo,
                                                                             JobData_pcst->idxDataset_u8);

        NvM_Prv_Crc_AppendCrc(JobData_pcst->idBlock_uo,
                              &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetToSubBlock_u16],
                              JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16,
                              &JobResult_pst->CrcData_st.Calculation_st.Crc_un);
    }

    // CRC calculation over data to be written into the non-volatile memory has succeeded
    // -> write block into the non-volatile memory
    *stJob_pen = NvM_Prv_stJob_Write_DoMemIf_e;
    idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Write_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a write job step function and polls the MemIf to complete the write job.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - CRC recalculation has succeeded
 * - write is required
 * - cryptographic services has succeeded
 * - job buffer contains data to be written
 *
 * \param [inout] stJob_pen
 * State of the read job
 * \param [inout] JobResult_pst
 * Pointer to the job results,  possible values:
 * - NvM_Prv_JobResult_Pending_e           = MemIf job is still pending -> wait 1 cycle
 * - NvM_Prv_JobResult_Succeeded_e         = MemIf job has written data to the non-volatile memory -> finish job
 * - NvM_Prv_JobResult_Failed_e            = MemIf write job has failed -> finish job
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_DoMemIf(NvM_Prv_stJob_ten* stJob_pen,
                                                                         NvM_Prv_JobResult_tst* JobResult_pst,
                                                                         NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoWrite_e;

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // MemIf job has succeeded -> finish write job
        *stJob_pen = NvM_Prv_stJob_Write_Completed_e;

        // NvM has successfully written user data into the non-volatile memory
        // check if a sub-block is in use
        if (!NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo, JobData_pcst->idxDataset_u8))
        {
            // -> update Ram block CRC so that it matches user data in non-volatile memory
            if (NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_RAM_CRC))
            {
                NvM_Prv_Crc_SetRamBlockCrc(JobData_pcst->idBlock_uo, &JobResult_pst->CrcData_st.CrcRamBlk_un);
            }

            // -> split block has been written successfully -> invalidate changed sub-blocks and update master-block
            if (NULL_PTR != NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo))
            {
                *stJob_pen = NvM_Prv_stJob_Write_DoBlockSplitInvalidate_e;
                idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoBlockSplit_Invalidate_e;
            }
        }
    }
    else
    {
        // MemIf job has failed -> finish write job, update final job result
        JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
        *stJob_pen = NvM_Prv_stJob_Write_Completed_e;
    }

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobWrite_DoBlockSplitInvalidate(NvM_Prv_stJob_ten* stJob_pen,
                                                                                        NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                        NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoWrite_e;

    NvM_Prv_BlockSplit_UpdateMasterBlock(JobData_pcst->idBlock_uo, JobData_pcst->IntBuffer_st.Buffer_pu8);

    // -> finish write job successfully
    *stJob_pen = NvM_Prv_stJob_Write_Completed_e;
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

/*
 **********************************************************************************************************************
 * Inline code
 **********************************************************************************************************************
 */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_JobWrite_IsWriteRequired(NvM_Prv_Crc_tun const* Crc_pcun,
                                                                 NvM_Prv_JobData_tst const* JobData_pcst)
{
    boolean isWriteRequired_b = TRUE;
    boolean isWriteReqDueToBackgroundCrcRecalc_b = NvM_Prv_Block_IsWriteRequired(JobData_pcst->idBlock_uo);

#if (NVM_BLOCK_COMPRESSION_USED == STD_ON)
    if (NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo, JobData_pcst->idxDataset_u8))
    {
        // The following union is necessary to remove an otherwise unavoidable compiler warning
        // due to a discarded const qualifier
        union
        {
            NvM_Prv_JobData_tst const* JobData_pcst;
            NvM_Prv_JobData_tst* JobData_pst;
        } JobData_un;
        JobData_un.JobData_pcst = JobData_pcst;

        isWriteRequired_b = NvM_Prv_BlockSplit_UpdateSubBlock(JobData_pcst->idBlock_uo,
                                                              &JobData_un.JobData_pst->idxDataset_u8,
                                                              JobData_pcst->IntBuffer_st.Buffer_pu8,
                                                              Crc_pcun);
    }
    else
#endif
    if (   NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_CRC_COMP_MECHANISM)
        && !isWriteReqDueToBackgroundCrcRecalc_b)
    {
        isWriteRequired_b = !NvM_Prv_Crc_CheckRamBlockCrc(JobData_pcst->idBlock_uo,
                                                          Crc_pcun);
    }
    else
    {
        /* no action needed */
    }

    return isWriteRequired_b;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

