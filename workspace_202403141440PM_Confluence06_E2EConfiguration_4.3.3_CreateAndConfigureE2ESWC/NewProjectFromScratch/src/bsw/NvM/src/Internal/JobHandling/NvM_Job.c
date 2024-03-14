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
#include "NvM_Prv_Job.h"
#include "NvM_Prv_JobQueue.h"
#include "NvM_Prv_JobStack.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_InternalBuffer.h"

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
# include "TestCd_NvM.h"
#endif

/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

RBA_MEMLIB_LOCAL_INLINE uint8 NvM_Prv_Job_GetIdxDataset(NvM_Prv_idJob_ten idJob_en,
                                                        NvM_BlockIdType idBlock_uo,
                                                        boolean isAuxServiceActive_b);

RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_Job_GetOffsetPlainData(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Job_UpdateJobBuffer(NvM_Prv_JobData_tst* JobData_pst,
                                                         NvM_Prv_Queue_idx_tuo idxEnqueued_uo);

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Job_UpdateIntBufferData(NvM_Prv_JobData_tst* JobData_pst);

#if (NVM_JOB_PRIORITIZATION == STD_ON)
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idQueue_tuo NvM_Prv_Job_GetIdQueueForSuspended(NvM_Prv_idService_tuo idService_uo);
#endif

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
void NvM_Prv_Job_PrepareData(NvM_Prv_idJob_ten idJob_en,
                             NvM_Prv_idQueue_tuo idReqQueue_uo,
                             NvM_Prv_JobData_tst* JobData_pst,
                             NvM_Prv_QueueEntry_tst const* Request_pcst)
{
    JobData_pst->idQueue_uo = idReqQueue_uo;
#if (NVM_JOB_PRIORITIZATION == STD_ON)
    if (NvM_Prv_idQueue_Suspended_e == idReqQueue_uo)
    {
        JobData_pst->idQueue_uo = NvM_Prv_Job_GetIdQueueForSuspended(Request_pcst->idService_uo);
    }
#endif

    JobData_pst->idService_uo               = Request_pcst->idService_uo;
    JobData_pst->ServiceBit_uo              = Request_pcst->ServiceBit_uo;
    JobData_pst->idBlock_uo                 = Request_pcst->idBlock_uo;
    JobData_pst->UserData_un.ptrRamBlock_pv = Request_pcst->BlockData_un.ptrRamBlock_pv;

    JobData_pst->isMultiServiceActive_b = (NvM_Prv_idQueue_Multi_e == JobData_pst->idQueue_uo);
    JobData_pst->isAuxServiceActive_b = FALSE;
#if (NVM_RB_AUX_INTERFACE == STD_ON)
    JobData_pst->isAuxServiceActive_b = (NvM_Prv_idQueue_Aux_e == JobData_pst->idQueue_uo);
#endif

    // Check for which block the job-data must be prepared
    if (Request_pcst->idBlock_uo < NVM_CFG_NR_BLOCKS)
    {
        // job-data for real block (stored in non-volatile memory) shall be prepared

        // TRACE[NVM338] The job of NvM_WriteBlock writes to the NV block which is selected by the data index
        //               when the block is of type DATASET
        // TRACE[NVM339] Same as NVM338, applied to NvM_WriteAll
        // TRACE[NVM340] The job of NvM_ReadBlock reads only the NV block selected by the data index
        //               for a block of type DATASET
        // TRACE[SWS_NvM_00802] The job of NvM_WritePRAMBlock writes to the NV block which is selected by the data index
        //                      when the block is of type DATASET
        // (Fee/Ea block-ID for an NV-block of an NvM-block of type DATASET) = (Fee/Ea block-ID) + (NV-index)
        JobData_pst->idxDataset_u8 = NvM_Prv_Job_GetIdxDataset(idJob_en,
                                                               Request_pcst->idBlock_uo,
                                                               JobData_pst->isAuxServiceActive_b);

        JobData_pst->isPRamBlockUsed_b =
                (JobData_pst->UserData_un.ptrRamBlock_pv == NvM_Prv_BlkDesc_GetPRamBlockAddress(Request_pcst->idBlock_uo));

        if (NvM_Prv_idJob_RecalcRamBlkCrc_e != idJob_en)
        {
            // A cryptographic service or NV CRC or block write counter or explicit synchronization or block version od block header
            // is used for current block
            // -> NvM will use internal buffer for current job (NvM copies user data into/from the internal buffer)
            // Neither a cryptographic service nor NV CRC nor block write counter nor explicit synchronization
            // nor block version nor block splitting nor block header is used for current job
            // -> NvM uses user data directly for current job (NvM avoids copying user data)
            JobData_pst->isIntBufferToUse_b = (   NvM_Prv_BlkDesc_IsBlockSelected(JobData_pst->idBlock_uo,
                                                                                  NVM_PRV_BLOCK_FLAG_USE_CRYPTO)
                                               || NvM_Prv_BlkDesc_IsBlockSelected(JobData_pst->idBlock_uo,
                                                                                  NVM_PRV_BLOCK_FLAG_USE_NV_CRC)
                                               || (NvM_Prv_BlkDesc_IsBlockSelected(JobData_pst->idBlock_uo,
                                                                                   NVM_PRV_BLOCK_FLAG_CNTR_WRITE))
                                               || (NvM_Prv_BlkDesc_IsBlockSelected(JobData_pst->idBlock_uo,
                                                                                   NVM_PRV_BLOCK_FLAG_BLOCK_VERSION))
                                               || (   NvM_Prv_BlkDesc_IsBlockSelected(JobData_pst->idBlock_uo,
                                                                                      NVM_PRV_BLOCK_FLAG_USE_SYNC_MECHANISM)
                                                   && JobData_pst->isPRamBlockUsed_b
                                                  )
                                               || (NULL_PTR != NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pst->idBlock_uo))
                                               || NvM_Prv_BlkDesc_IsBlockHeaderEnabled()
                                              );
        }
        else
        {
            JobData_pst->isIntBufferToUse_b = NvM_Prv_BlkDesc_IsBlockSelected(JobData_pst->idBlock_uo,
                                                                              NVM_PRV_BLOCK_FLAG_USE_SYNC_MECHANISM);
        }

        JobData_pst->isEncryptionEnabled_b = FALSE;
#if (NVM_CRYPTO_USED == STD_ON)
        JobData_pst->isEncryptionEnabled_b = (0u != NvM_Prv_BlkDesc_GetLengthJobCsm(Request_pcst->idBlock_uo,
                                                                                    NvM_Prv_Crypto_idService_Encrypt_e));
#endif
    }
    else
    {
        // job-data for dummy block (NvM internal block, e.g. for random number generation) shall be prepared
        JobData_pst->idxDataset_u8 = 0u;
        JobData_pst->isPRamBlockUsed_b = FALSE;
        JobData_pst->isIntBufferToUse_b = FALSE;
        JobData_pst->isEncryptionEnabled_b = FALSE;
    }
}

void NvM_Prv_Job_Start(NvM_Prv_idJob_ten idJob_en,
                       NvM_Prv_idQueue_tuo idReqQueue_uo,
                       NvM_Prv_QueueEntry_tst const* Request_pcst)
{
    NvM_Prv_Job_tst Job_st;

    // PreInitialize job-structure to avoid warnings related to partly initialized structure,
    // this pre-initialization does not prevent CERT warnings but for completeness we nevertheless initialize everything.
    rba_MemLib_MemSet((uint8*)&Job_st, 0u, sizeof(Job_st));

    Job_st.idJob_en = idJob_en;
    NvM_Prv_Job_PrepareData(idJob_en, idReqQueue_uo, &Job_st.JobData_st, Request_pcst);

    /* CERTC RULE EXP33-C VIOLATION: The sub-structure JobData_st is initialized completely in NvM_Prv_Job_PrepareData just before using it here */
    NvM_Prv_JobStack_Initialize(idJob_en, &Job_st.JobStack_st, &Job_st.JobResult_st, &Job_st.JobData_st);

    // enqueue job, job insertion will be done after update of job data
    Job_st.JobData_st.idxEntryInJobQueue_uo = NvM_Prv_JobQueue_Enqueue();

    // update job data only for jobs which operates on user data
    if (NvM_Prv_idJob_GenerateRandomNr_e != Job_st.idJob_en)
    {
        NvM_Prv_Job_UpdateIntBufferData(&Job_st.JobData_st);
    }

    /* CERTC RULE EXP33-C VIOLATION: The sub-structure JobData_st is initialized completely in NvM_Prv_Job_PrepareData just before using it here */
    NvM_Prv_JobQueue_Insert(&Job_st, Job_st.JobData_st.idxEntryInJobQueue_uo);

    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3055] Invoke block-start callback function if configured
    // SingleBlockStartCallback shall be called only if available and only for non-auxiliary services
    if (!Job_st.JobData_st.isAuxServiceActive_b && (NvM_Prv_idJob_GenerateRandomNr_e != Job_st.idJob_en))
    {
        NvM_Prv_BlkDesc_InvokeSingleBlockStartCallback(Request_pcst->idBlock_uo, Request_pcst->idService_uo);
    }
}

void NvM_Prv_Job_Do(NvM_Prv_Job_tst* Job_pst)
{
    do
    {
        NvM_Prv_JobStack_Do(&Job_pst->JobStack_st, &Job_pst->JobResult_st, &Job_pst->JobData_st);

    } while (   !NvM_Prv_JobStack_IsEmpty(&Job_pst->JobStack_st)
             && (NvM_Prv_JobResult_Pending_e != Job_pst->JobResult_st.Result_en));
}

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
NvM_Prv_Job_tst* NvM_Prv_Job_Get(NvM_BlockIdType idBlock_uo,
                                 NvM_Prv_idService_tuo idService_uo)
{
    NvM_Prv_Job_tst* Job_pst = NULL_PTR;
    boolean isJobOngoing_b = FALSE;
    uint8 idxJob_uo = 0u;

    // get job data only if job queue is not empty
    if (!NvM_Prv_JobQueue_IsEmpty())
    {
        do
        {
            // search in the job queue for a job with required blok and service ID
            Job_pst = NvM_Prv_JobQueue_GetJob(idxJob_uo);
            if (   (NULL_PTR != Job_pst)
                && (Job_pst->JobData_st.idBlock_uo == idBlock_uo)
                && (Job_pst->JobData_st.idService_uo == idService_uo))
            {
                isJobOngoing_b = TRUE;
            }
            else
            {
                ++idxJob_uo;
            }

        } while (!isJobOngoing_b && (idxJob_uo < NVM_PRV_NR_PARALLEL_JOB));
    }

    // if no job is ongoing for required block ID and service ID -> set returned job structure pointer to NULL_PTR
    if (!isJobOngoing_b)
    {
        Job_pst = NULL_PTR;
    }

    return Job_pst;
}
#endif

/*
 **********************************************************************************************************************
 * Inline code
 **********************************************************************************************************************
 */
/**
 * \brief
 * This local private function returns the dataset index to be used in current job.
 * \details
 * In following cases the dataset index is set to 0:
 * - non-dataset block
 * - job for random number generation is active
 * - job for recalculation of RAM block CRC
 * - job to remove blocks configured as non-resistant to layout change
 * - job to invalidate blocks for first initialization
 *
 * In all other cases the NvM always uses the dataset index set by the user via NvM_SetDataIndex.
 * \attention
 * If current block is not of type dataset then the idex is set to 0 during initialization.
 * \attention
 * If current job is part of an auxiliary service then the NvM uses the dataset index set by the user
 * via NvM_Rb_AuxSetDataIndex, in this case to get the dataset index the NvM must use the block ID
 * NVM_PRV_AUX_ADMIN_BLOCK.
 *
 * \return
 * dataset index to be used in current job
 */
RBA_MEMLIB_LOCAL_INLINE uint8 NvM_Prv_Job_GetIdxDataset(NvM_Prv_idJob_ten idJob_en,
                                                        NvM_BlockIdType idBlock_uo,
                                                        boolean isAuxServiceActive_b)
{
    uint8 idxDataset_u8;

    NvM_Cfg_BlockSplit_tst const* CfgBlockSplit_pcst = NvM_Prv_BlkDesc_GetCfgBlockSplit(idBlock_uo);

    if (NULL_PTR != CfgBlockSplit_pcst)
    {
        // for blocks configured for block-splitting feature index = index of master-block
        idxDataset_u8 = CfgBlockSplit_pcst->BlockSplit_Cfg_pcst->nrSubBlocks_u8 * 2u;
    }
    else if (   (NVM_BLOCK_DATASET != NvM_Prv_BlkDesc_GetType(idBlock_uo))
             || (NvM_Prv_idJob_RecalcRamBlkCrc_e == idJob_en)
             || (NvM_Prv_idJob_GenerateRandomNr_e == idJob_en)
             || (NvM_Prv_idJob_InvalidateForFirstInitAll_e == idJob_en)
             || (NvM_Prv_idJob_InvalidateForRemoveNonResistant_e == idJob_en)
            )
    {
        idxDataset_u8 = 0u;
    }
    else
    {
#if (NVM_RB_AUX_INTERFACE == STD_ON)
        if (isAuxServiceActive_b)
        {
            idBlock_uo = NVM_PRV_AUX_ADMIN_BLOCK;
        }
#else
        (void)isAuxServiceActive_b;
#endif
        idxDataset_u8 = NvM_Prv_Block_GetIdxDataset(idBlock_uo);
    }
    return idxDataset_u8;
}

/**
 * \brief
 * This local private function returns offset to the plain user data in the internal buffer (RAM mirror).
 * \details
 * This function is relevant only for encrypted blocks.
 * For encrypted blocks this function returns the offset in the internal buffer where NvM locates decrypted data.
 * For blocks without encryption this function always returns 0.
 *
 * \param JobData_pcst
 * Pointer to the constant job data
 *
 * \return
 * Offset in bytes to the plain data in the internal buffer
 */
RBA_MEMLIB_LOCAL_INLINE uint16 NvM_Prv_Job_GetOffsetPlainData(NvM_Prv_JobData_tst const* JobData_pcst)
{
    uint16 OffsetPlainData_u16 = 0u;

    if (JobData_pcst->isEncryptionEnabled_b)
    {
        OffsetPlainData_u16 = NvM_Prv_BlkDesc_GetBlockSizeStored(JobData_pcst->idBlock_uo) - NVM_RB_BLOCK_HEADER_LENGTH;
    }

    return OffsetPlainData_u16;
}

/**
 * \brief
 * This local private function updates pointer to the buffer used for the current job and for the block header
 * if required.
 * \details
 * If possible then the NvM uses RAM block directly to process a job.
 * For some configurations it is not possible to use RAM block and in this cases the NvM uses internal buffer
 * to process a job.
 * If used then the part of the internal buffer used for the current job is identified by the index in the array
 * which implements the job queue
 *
 * \attention
 * Do not confuse the job number and the job index.
 *
 * \param JobData_pcst
 * Pointer to job data
 * \param idxEnqueued_u16
 * Index of the current job in the array which implements the job queue
 */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Job_UpdateJobBuffer(NvM_Prv_JobData_tst* JobData_pst,
                                                         NvM_Prv_Queue_idx_tuo idxEnqueued_uo)
{
    if (JobData_pst->isIntBufferToUse_b)
    {
        uint8* IntBuffer_pu8 = NvM_Prv_InternalBuffer_GetPtr();
        uint32 IntBufferSize_u32 = NvM_Prv_InternalBuffer_GetSize();

        JobData_pst->IntBuffer_st.BlockHeader_pu8 = &IntBuffer_pu8[IntBufferSize_u32 * idxEnqueued_uo];
        JobData_pst->IntBuffer_st.Buffer_pu8 = JobData_pst->IntBuffer_st.BlockHeader_pu8 + NVM_RB_BLOCK_HEADER_LENGTH;
    }
    else
    {
        JobData_pst->IntBuffer_st.BlockHeader_pu8 = NULL_PTR;
        JobData_pst->IntBuffer_st.Buffer_pu8 = JobData_pst->UserData_un.ptrRamBlock_pu8;
    }
}

/**
 * \brief
 * This local private function updates internal buffer parameter used for the current job.
 * \details
 * Following parameters are required for internal buffer to process a job:
 * - size of the internal buffer in bytes used for the current job
 * - offset to the plain user data located in the internal buffer
 * - pointer to the buffer used for the current job
 * - pointer to the position within internal buffer for the block header
 *
 * \param JobData_pcst
 * Pointer to job data
 * \param idxEnqueued_u16
 * Index of the current job in the array which implements the job queue
 */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_Job_UpdateIntBufferData(NvM_Prv_JobData_tst* JobData_pst)
{
    NvM_Prv_Queue_idx_tuo idxEnqueued_uo = JobData_pst->idxEntryInJobQueue_uo;
    uint16* UsedSizeInBytes_pu16 = NvM_Prv_InternalBuffer_GetPtrToUsedSize();

    JobData_pst->IntBuffer_st.UsedSizeInBytes_pu16 = &UsedSizeInBytes_pu16[idxEnqueued_uo];
    *JobData_pst->IntBuffer_st.UsedSizeInBytes_pu16 = 0u;
    JobData_pst->IntBuffer_st.OffsetPlainData_u16 = NvM_Prv_Job_GetOffsetPlainData(JobData_pst);
    NvM_Prv_Job_UpdateJobBuffer(JobData_pst, idxEnqueued_uo);

    if (   NvM_Prv_BlkDesc_IsBlockSelected(JobData_pst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_SYNC_MECHANISM)
        && JobData_pst->isPRamBlockUsed_b)
    {
        // Update pointer to user data if explicit synchronization is enabled and permanent RAM block is used
        uint16 OffsetPlainData_u16 = JobData_pst->IntBuffer_st.OffsetPlainData_u16;
        JobData_pst->UserData_un.ptrRamBlock_pu8 = &JobData_pst->IntBuffer_st.Buffer_pu8[OffsetPlainData_u16];
    }
}

#if (NVM_JOB_PRIORITIZATION == STD_ON)

RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idQueue_tuo NvM_Prv_Job_GetIdQueueForSuspended(NvM_Prv_idService_tuo idService_uo)
{
    NvM_Prv_idQueue_tuo idQueue_uo;

    switch (idService_uo)
    {
        case NVM_SERVICE_ID_READ_ALL:
        case NVM_SERVICE_ID_WRITE_ALL:
        case NVM_SERVICE_ID_VALIDATE_ALL:
        case NVM_SERVICE_ID_RB_FIRST_INIT_ALL:
        case NVM_SERVICE_ID_RB_INIT_AFTER_READ_ALL:

            idQueue_uo = NvM_Prv_idQueue_Multi_e;

        break;

#if (NVM_RB_AUX_INTERFACE == STD_ON)
        case NVM_SERVICE_ID_RB_AUX_READ_BLOCK:
        case NVM_SERVICE_ID_RB_AUX_WRITE_BLOCK:
        case NVM_SERVICE_ID_RB_AUX_INVALIDATE_NV_BLOCK:

            idQueue_uo = NvM_Prv_idQueue_Aux_e;

        break;
#endif

        default:

            idQueue_uo = NvM_Prv_idQueue_Standard_e;

        break;
    }

    return idQueue_uo;
}

#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

