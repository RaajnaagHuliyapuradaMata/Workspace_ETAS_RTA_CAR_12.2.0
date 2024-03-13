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

#include "rba_MemLib.h"
#include "rba_MemLib_Special.h"

#include "NvM_Prv.h"
#include "NvM_Prv_Crc.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_JobResource.h"
#include "NvM_Prv_InternalBuffer.h"
#include "NvM_Prv_JobStateMachine.h"
#include "NvM_Prv_ExplicitSynchronization.h"
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobRestore_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC boolean NvM_Prv_JobRestore_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestore_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                         NvM_Prv_JobResult_tst* JobResult_pst,
                                                                         NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestore_CopyFromRom(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestore_ExplicitSync(NvM_Prv_stJob_ten* stJob_pen,
                                                                                NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestore_InitCallback(NvM_Prv_stJob_ten* stJob_pen,
                                                                                NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestore_Crc(NvM_Prv_stJob_ten* stJob_pen,
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

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_JobRestore_Data_cst =
{
        &NvM_Prv_JobRestore_GetStateFct,
        &NvM_Prv_JobRestore_IsEnabled,
        NvM_Prv_stJob_Restore_Start_e,
        NvM_Prv_stJob_Restore_Completed_e,
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

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_JobRestore_GetData(void)
{
    return &NvM_Prv_JobRestore_Data_cst;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_JobRestore_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // Restore is always enabled for all blocks
    return TRUE;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_JobRestore_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobRestore_State_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_Restore_Start_e:
            JobRestore_State_pfct = &NvM_Prv_JobRestore_Start;
        break;

        case NvM_Prv_stJob_Restore_CopyFromRom_e:
            JobRestore_State_pfct = &NvM_Prv_JobRestore_CopyFromRom;
        break;

        case NvM_Prv_stJob_Restore_ExplicitSync_e:
            JobRestore_State_pfct = &NvM_Prv_JobRestore_ExplicitSync;
        break;

        case NvM_Prv_stJob_Restore_InitCallback_e:
            JobRestore_State_pfct = &NvM_Prv_JobRestore_InitCallback;
        break;

        case NvM_Prv_stJob_Restore_Crc_e:
            JobRestore_State_pfct = &NvM_Prv_JobRestore_Crc;
        break;

        default:
            JobRestore_State_pfct = NULL_PTR;
        break;
    }
    return JobRestore_State_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestore_Start(NvM_Prv_stJob_ten* stJob_pen,
                                                                         NvM_Prv_JobResult_tst* JobResult_pst,
                                                                         NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRestore_e;

    NvM_Prv_JobResult_ten Result_en = NvM_Prv_JobResult_Succeeded_e;

    // restore job can be done only for blocks with available default data
    if (NvM_Prv_BlkDesc_IsDefaultDataAvailable(JobData_pcst->idBlock_uo))
    {
        *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo);

        // TRACE[NVM392] Restore job is done without any MemIf interaction
        // TRACE[NVM392] Leave the NV block untouched
        // Determine the source of the block defaults
        if (NULL_PTR != NvM_Prv_BlkDesc_GetRomBlockAddress(JobData_pcst->idBlock_uo))
        {
            // A ROM block is configured for this block
            // TRACE[NVM267] Fill the block data buffer with ROM block data if configured
            // TRACE[SWS_NvM_00816] Fill the block data buffer with ROM block data if configured
            *stJob_pen = NvM_Prv_stJob_Restore_CopyFromRom_e;
        }
        else
        {
            // An initialization callback is configured for this block
            *stJob_pen = NvM_Prv_stJob_Restore_InitCallback_e;
        }
    }
    else
    {
        // If no default data is available then restore job shall fail
        *stJob_pen = NvM_Prv_stJob_Restore_Completed_e;
        Result_en = NvM_Prv_JobResult_Failed_e;
    }

    JobResult_pst->Result_en = Result_en;

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestore_CopyFromRom(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRestore_e;

    // A ROM block is configured for this block
    // TRACE[NVM267] Fill the block data buffer with ROM block data
    // TRACE[SWS_NvM_00816] Fill the block data buffer with ROM block data
    uint8* adrDestination_pu8;
    uint32 BlockSize_u32 = NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo);
    uint8 nrNonVolatileBlocks_u8 = NvM_Prv_BlkDesc_GetNrNonVolatileBlocks(JobData_pcst->idBlock_uo);
    uint8 const* adrSource_pcu8;
    NvM_Prv_ptrRomBlock_tun ptrRomBlock_un;
    ptrRomBlock_un.ptrRomBlock_pcv = NvM_Prv_BlkDesc_GetRomBlockAddress(JobData_pcst->idBlock_uo);
    adrSource_pcu8 = ptrRomBlock_un.ptrRomBlock_pcu8;

    if (NVM_BLOCK_DATASET == NvM_Prv_BlkDesc_GetType(JobData_pcst->idBlock_uo))
    {
        // This DATASET block has its dataset-index pointing to a ROM block
        // (otherwise it wouldn't have been queued by NvM_RestoreBlockDefaults or by NvM_ReadBlock)
        // -> Move the source pointer to the start address of the indexed ROM block
        adrSource_pcu8 += BlockSize_u32 * (JobData_pcst->idxDataset_u8 - nrNonVolatileBlocks_u8);
    }

    if (   NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_SYNC_MECHANISM)
        && JobData_pcst->isPRamBlockUsed_b)
    {
        adrDestination_pu8 = JobData_pcst->IntBuffer_st.Buffer_pu8;
        // After copying data from ROM data NvM starts explicit synchronization if configured
        *stJob_pen = NvM_Prv_stJob_Restore_ExplicitSync_e;
    }
    else
    {
        adrDestination_pu8 = JobData_pcst->UserData_un.ptrRamBlock_pu8;
        // After copying data from ROM data NvM calls initialization callback if configured
        *stJob_pen = NvM_Prv_stJob_Restore_InitCallback_e;
    }

    // Perform the restore job by copying data from the ROM-block to the RAM-block
    rba_MemLib_MemCopy(adrSource_pcu8, adrDestination_pu8, BlockSize_u32);

    // Copying data from ROM is always done immediately and cannot fail
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestore_ExplicitSync(NvM_Prv_stJob_ten* stJob_pen,
                                                                                NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRestore_e;

    // trigger the read callback for explicit synchronization
    JobResult_pst->Result_en = NvM_Prv_ExplicitSync_CopyData(NvM_Prv_BlkDesc_GetCopyFctForRead(JobData_pcst->idBlock_uo),
                                                             JobData_pcst->idBlock_uo,
                                                             &JobResult_pst->cntrExpSyncOperations_u8,
                                                             JobData_pcst->IntBuffer_st.Buffer_pu8);

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // Copying data for explicit synchronization has succeeded ->
        // call initialization callback before starting CRC recalculation, do not update final job result
        // An initialization callback is configured for this block
        *stJob_pen = NvM_Prv_stJob_Restore_InitCallback_e;
    }
    else if (NvM_Prv_JobResult_Failed_e == JobResult_pst->Result_en)
    {
        // Copying data for explicit synchronization has failed ->
        // finish restore job, update final job result
        *stJob_pen = NvM_Prv_stJob_Restore_Completed_e;
    }
    else
    {
        // Copying data for explicit synchronization is still pending ->
        // remain in this state, do not update final job result
    }

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestore_InitCallback(NvM_Prv_stJob_ten* stJob_pen,
                                                                                NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRestore_e;

    // InitBlockCallback shall be called only if available and only for non-auxiliary services
    if (!JobData_pcst->isAuxServiceActive_b)
    {
        // Invoke the init block callback
        Std_ReturnType stInitCallback_uo = NvM_Prv_BlkDesc_InvokeInitBlockCallback(JobData_pcst->idBlock_uo, JobData_pcst->idService_uo);
        if (E_OK != stInitCallback_uo)
        {
            // If init callback has failed finish restore job
            *stJob_pen = NvM_Prv_stJob_Restore_Completed_e;
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
        }
    }
    // Copying data via initialization callback is always done immediately

    // Next step is CRC recalculation,
    // only if initialization callback has failed NvM finishs restore job
    if (   NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_RAM_CRC)
        && (NvM_Prv_JobResult_Failed_e != JobResult_pst->Result_en))
    {
        // TRACE[SWS_NvM_00821] Restore job for NvM_RestorePRAMBlockDefaults shall recalculate CRC over the RAM block
        //                      after the copy process if configured
        *stJob_pen = NvM_Prv_stJob_Restore_Crc_e;
        idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCrc_e;
        JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

        // Prepare CRC recalculation over the user data
        JobResult_pst->CrcData_st.Calculation_st.isFirstCall_b = TRUE;
        JobResult_pst->CrcData_st.Calculation_st.Length_u16 = NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo);
        JobResult_pst->CrcData_st.Calculation_st.Crc_un.Crc32_u32 = 0u;
        // Buffer with user data depends on block configuration
        if (   NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_SYNC_MECHANISM)
            && JobData_pcst->isPRamBlockUsed_b)
        {
            JobResult_pst->CrcData_st.Calculation_st.Buffer_pu8 = JobData_pcst->IntBuffer_st.Buffer_pu8;
        }
        else
        {
            JobResult_pst->CrcData_st.Calculation_st.Buffer_pu8 = JobData_pcst->UserData_un.ptrRamBlock_pu8;
        }
    }
    else
    {
        // No CRC calculation needed -> finish read job, final result is already set
        *stJob_pen = NvM_Prv_stJob_Restore_Completed_e;
    }

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_JobRestore_Crc(NvM_Prv_stJob_ten* stJob_pen,
                                                                       NvM_Prv_JobResult_tst* JobResult_pst,
                                                                       NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoRestore_e;

    /* CRC calculation is always finished successfully */
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    // Copy CRC over the user data for later usage
    JobResult_pst->CrcData_st.CrcRamBlk_un.Crc32_u32 = JobResult_pst->CrcData_st.Calculation_st.Crc_un.Crc32_u32;

    if (JobData_pcst->isPRamBlockUsed_b)
    {
        // Set calculated CRC if permanent RAM block is used
        NvM_Prv_Crc_SetRamBlockCrc(JobData_pcst->idBlock_uo, &JobResult_pst->CrcData_st.Calculation_st.Crc_un);
    }

    // CRC calculation finished -> finish read job, final result is already set
    *stJob_pen = NvM_Prv_stJob_Restore_Completed_e;

    return idJobStateMachineNext_en;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"
