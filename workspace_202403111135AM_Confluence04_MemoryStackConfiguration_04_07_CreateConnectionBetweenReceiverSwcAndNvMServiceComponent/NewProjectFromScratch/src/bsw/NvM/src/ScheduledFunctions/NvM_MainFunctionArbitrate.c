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

#include "NvM_Prv.h"
#include "NvM_Prv_Crc.h"
#include "NvM_Prv_Job.h"
#include "NvM_Prv_Service.h"
#include "NvM_Prv_JobQueue.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_RequestQueue.h"
#include "NvM_Prv_ErrorDetection.h"
#include "NvM_Prv_ProcessMultiBlock.h"
#include "NvM_Prv_JobResource.h"

/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
 */
/* Note: caller has to ensure that idReqQueue_uo/Request_pst are valid */
RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_SetJobForRequest(NvM_Prv_idQueue_tuo idReqQueue_uo,
                                                      NvM_Prv_idJobResource_tuo idJobResource_uo,
                                                      NvM_Prv_Queue_idx_tst const* idxRequest_pcst,
                                                      NvM_Prv_QueueEntry_tst* Request_pst,
                                                      NvM_Prv_MainStates_tst* MainStates_pst);

RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_IsRequestDelayRequired(NvM_Prv_idQueue_tuo idReqQueueNext_uo,
                                                               NvM_Prv_idQueue_tuo idReqQueueCurrent_uo,
                                                               NvM_Prv_Queue_idx_tst const* idxRequest_pcst);

RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idJob_ten NvM_Prv_GetNextJob(NvM_Prv_idQueue_tuo idReqQueue_uo,
                                                             NvM_Prv_idJobResource_tuo idJobResource_uo,
                                                             NvM_Prv_QueueEntry_tst* QueueEntry_pst,
                                                             NvM_BlockIdType* idBlockToProcess_puo);
/*
 **********************************************************************************************************************
 * NvM-private code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * This function determines the next request to be executed by checking all queues for user requests to be started.
 *
 * \param[inout] MainStates_pst
 * Pointer to the structure with all state information which is not block-specific.
 *
 * \return
 * - true = NvM has to process a user request or an internal NvM job
 * - false = NvM has nothing to do and is completely idle
 */
boolean NvM_Prv_MainFunctionArbitrate(NvM_Prv_MainStates_tst* MainStates_pst)
{
    NvM_Prv_idJobResource_tuo idJobResource_uo  = 0u;
    NvM_Prv_QueueEntry_tst* Request_pst         = NULL_PTR;
    boolean isJobAvailable_b                    = FALSE;
    NvM_Prv_Queue_idx_tst idxRequest_st         = {
                                                    NVM_PRV_QUEUE_IDX_INVALID,  // idx_uo
                                                    NVM_PRV_QUEUE_IDX_INVALID   // idxPrev_uo
                                                  };

    while (!NvM_Prv_JobQueue_IsFull() && (idJobResource_uo < NvM_Prv_idJobResource_NrJobResources_e))
    {
        if (!NvM_Prv_JobResource_IsClaimed(idJobResource_uo))
        {
            NvM_Prv_idQueue_tuo idQueue_uo = NvM_Prv_RequestQueue_GetEntryToProcess(idJobResource_uo,
                                                                                    &Request_pst,
                                                                                    &idxRequest_st);
            /* Note:
             * returned idQueue_uo and Request_pst are always corresponding after this function call.
             * If Request_pst== NULL_PTR then and only then idQueue_uo is also invalid and shall not be used.
             */

            if (NULL_PTR != Request_pst)
            {
                isJobAvailable_b = TRUE;
                MainStates_pst->Activity_rAMwM_en = NVM_PRV_ACTIVITY_BUSY;

                /*Note: idQueue_uo is in valid range at this point */
                NvM_Prv_SetJobForRequest(idQueue_uo, idJobResource_uo, &idxRequest_st, Request_pst, MainStates_pst);
            }
        }
        else
        {
            isJobAvailable_b = TRUE;
        }

        ++idJobResource_uo;
    }

    if (NvM_Prv_JobQueue_IsEmpty() && !isJobAvailable_b)
    {
        // No request pending -> NvM is completely idle
        NvM_BlockIdType idBlock_uo;

        MainStates_pst->idServiceActive_uo = NVM_SERVICE_ID_INIT;
        MainStates_pst->idQueueActive_uo = NvM_Prv_idQueue_nrQueues_e;

        // check whether RAM block CRC recalculation is required
        if (NvM_Prv_Crc_IsRamBlockCrcRecalcRequired(&idBlock_uo))
        {
            NvM_Prv_QueueEntry_tst Request_st;

            Request_st.idService_uo                 = NVM_SERVICE_ID_INIT;
            Request_st.idBlock_uo                   = idBlock_uo;
            Request_st.ServiceBit_uo                = NvM_Prv_ServiceBit_Unspecified_e;
            Request_st.BlockData_un.ptrRamBlock_pv  = NvM_Prv_BlkDesc_GetPRamBlockAddress(idBlock_uo);

            NvM_Prv_Job_Start(NvM_Prv_idJob_RecalcRamBlkCrc_e, NvM_Prv_idQueue_nrQueues_e, &Request_st);

            MainStates_pst->Activity_rAMwM_en = NVM_PRV_ACTIVITY_RAM_BLOCK_CRC;
        }
        else
        {
            MainStates_pst->Activity_rAMwM_en = NVM_PRV_ACTIVITY_IDLE;
        }
    }

    return (!NvM_Prv_JobQueue_IsEmpty());
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Inline code
 **********************************************************************************************************************
 */
/**
 * \brief
 * This local private function sets up the job for a user request.
 *
 * \details
 * To set up a job this function checks whether a job for given user request can be processed,
 * s. NvM_Prv_IsRequestDelayRequired.
 * If so then this function updates main states and prepares all job data to enqueue it into the job queue,
 * s. NvM_Prv_Job_Setup.
 *
 * \attention
 * This function dequeues a multi-block user request only after its completion.
 *
 * \attention
 * This function uses scheduler lock to dequeue a user request. The duration of the scheduler lock corresponds
 * to 9 copy-operations:
 * - NvM_Prv_RequestQueue_Dequeue = 9 (see description of NvM_Prv_RequestQueue_Dequeue)
 *
 * \param[in] idReqQueue_uo
 * Queue ID where current user requests is enqueued (valid value assumed here)
 * \param[in] idJobResource_uo
 * Job resource ID required by the current user request
 * \param[in] idxRequest_pcst
 * Index in the queue buffer for the current user request
 * \param[inout] Request_pst
 * Pointer to the current user request (valid value assumed here)
 * \param[inout] MainStates_pst
 * Pointer to the structure with all state information which is not block-specific.
 */
RBA_MEMLIB_LOCAL_INLINE  void NvM_Prv_SetJobForRequest(NvM_Prv_idQueue_tuo idReqQueue_uo,
                                                       NvM_Prv_idJobResource_tuo idJobResource_uo,
                                                       NvM_Prv_Queue_idx_tst const* idxRequest_pcst,
                                                       NvM_Prv_QueueEntry_tst* Request_pst,
                                                       NvM_Prv_MainStates_tst* MainStates_pst)
{
    /*Note: idReqQueue_uo is in valid range at this point */

    if (!NvM_Prv_IsRequestDelayRequired(idReqQueue_uo, MainStates_pst->idQueueActive_uo, idxRequest_pcst))
    {
        NvM_Prv_idJob_ten idJob_en;
        NvM_Prv_QueueEntry_tst RequestToProcess_st;

        if (NVM_SERVICE_ID_INIT == MainStates_pst->idServiceActive_uo)
        {
            MainStates_pst->idServiceActive_uo = Request_pst->idService_uo;
            MainStates_pst->idQueueActive_uo = idReqQueue_uo;
        }

        idJob_en = NvM_Prv_GetNextJob(idReqQueue_uo,
                                      idJobResource_uo,
                                      Request_pst,
                                      &RequestToProcess_st.idBlock_uo);

        if (NvM_Prv_idJob_Suspend_e != idJob_en)
        {
            if ((NvM_Prv_idJob_Idle_e != idJob_en) && (NvM_Prv_idJob_Invalid_e != idJob_en))
            {
                RequestToProcess_st.idService_uo        = Request_pst->idService_uo;
                RequestToProcess_st.idQueueOriginal_uo  = Request_pst->idQueueOriginal_uo;
                RequestToProcess_st.ServiceBit_uo       = Request_pst->ServiceBit_uo;
                RequestToProcess_st.BlockData_un        = Request_pst->BlockData_un;

                NvM_Prv_JobResource_Claim(idJobResource_uo, RequestToProcess_st.idBlock_uo);
                NvM_Prv_Job_Start(idJob_en, RequestToProcess_st.idQueueOriginal_uo, &RequestToProcess_st);

                NvM_Prv_Block_SetState(RequestToProcess_st.idBlock_uo,
                                       NVM_BLOCK_STATE_JOB_ONGOING,
                                       NVM_BLOCK_STATE_JOB_ONGOING);
            }
            else
            {
                MainStates_pst->idServiceActive_uo = NVM_SERVICE_ID_INIT;
                MainStates_pst->idQueueActive_uo = NvM_Prv_idQueue_nrQueues_e;
            }
        }

        if ((NvM_Prv_idQueue_Multi_e != idReqQueue_uo) || (NvM_Prv_idJob_Idle_e == idJob_en))
        {
            // Set scheduling lock
            SchM_Enter_NvM_Main();
            /* Note: idReqQueue_uo is valid here */
            NvM_Prv_RequestQueue_Dequeue(idReqQueue_uo, idxRequest_pcst);
            // Reset scheduling lock
            SchM_Exit_NvM_Main();
        }
    }
}

/**
 * \brief
 * This local private function checks whether a user request can be processed or must be delayed before processing.
 *
 * \details
 * Currently the NvM has to delay a user request for following reasons:
 * - A user request has been enqueued into the request queue but user data has not been inserted,
 *   s.a. NvM_Prv_RequestQueue_IsInserted.
 *   Currently types of user requests change in following cases:
 *   - an immediate request can interrupt a multi-block request only if all jobs for multi-block request
 *     in the job queue has been finished
 *   - a multi-block request can start only if all jobs for single-block requests in the job queue has been finished
 *
 * \param[in] idReqQueueNext_uo
 * ID of the queue containing user request to be started
 * \param[in] idReqQueueCurrent_uo
 * ID of the queue containing user request currently processed
 * \param[in] stMain_en
 * Current NvM state
 *
 * \return
 * Information whether a user request can be processed
 * - true: User request shall be delayed (cannot be processed)
 * - false: User request shall not be delayed (can be processed)
 */
RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_IsRequestDelayRequired(NvM_Prv_idQueue_tuo idReqQueueNext_uo,
                                                               NvM_Prv_idQueue_tuo idReqQueueCurrent_uo,
                                                               NvM_Prv_Queue_idx_tst const* idxRequest_pcst)
{
    boolean isChangeFromOrToMultiRequired_b = (   (idReqQueueNext_uo != idReqQueueCurrent_uo)
                                               && (   (NvM_Prv_idQueue_Multi_e == idReqQueueNext_uo)
                                                   || (NvM_Prv_idQueue_Multi_e == idReqQueueCurrent_uo)
                                                   )
                                               );

    // enqueued request shall be processed only if its initiation has been completed i.e. insertion
    // of request data is done and all user data required to proceed a user request are available
    boolean isRequestInserted_b = NvM_Prv_RequestQueue_IsInserted(idReqQueueNext_uo, idxRequest_pcst->idx_uo);

    return ((!NvM_Prv_JobQueue_IsEmpty() && isChangeFromOrToMultiRequired_b) || !isRequestInserted_b);
}

/**
 * \brief
 * This function determines which job the user request requires.
 * \details
 * If user requests a multi-block user request then this function processes it.
 * The processing of a multi-block user request determines which job is required for which block.
 * If initiation of the requested multi-block user request is still ongoing then this function suspends it.
 *
 * If user requests a single-block user request then this function determines which job is required for the requested
 * block. Additionally this function checks whether the single-block service has requested a valid service by checking
 * the service bit.
 *
 * \param[in] idReqQueue_uo
 * ID of the queue with user request to be processed
 * \param[in] idJobResource_uo
 * Job resource ID required for the user request
 * \param[in] idxRequest_uo
 * Index of the user request in the queue buffer
 * \param[inout] QueueEntry_pst
 * Pointer to the data of the user request to be processed
 * \param[inout] idBlockToProcess_puo
 * Pointer to the ID of the block to be processed
 *
 * \return
 * ID of the next job to be processed
 */
RBA_MEMLIB_LOCAL_INLINE NvM_Prv_idJob_ten NvM_Prv_GetNextJob(NvM_Prv_idQueue_tuo idReqQueue_uo,
                                                             NvM_Prv_idJobResource_tuo idJobResource_uo,
                                                             NvM_Prv_QueueEntry_tst* QueueEntry_pst,
                                                             NvM_BlockIdType* idBlockToProcess_puo)
{
    NvM_Prv_idJob_ten idJobNext_en = NvM_Prv_idJob_Idle_e;

    if (NvM_Prv_idQueue_Multi_e == idReqQueue_uo)
    {
        // process multi-block service
        idJobNext_en = NvM_Prv_Multi_Process(idJobResource_uo, QueueEntry_pst, idBlockToProcess_puo);
    }
    else
    {
        idJobNext_en = NvM_Prv_idJob_Invalid_e;

        // Check whether the service identified by the service bit is valid
        if (NvM_Prv_ErrorDetection_IsServiceBitValid(NVM_SERVICE_ID_MAIN_FUNCTION,
                                                     QueueEntry_pst->idBlock_uo,
                                                     QueueEntry_pst->ServiceBit_uo))
        {
            // process single block service
            idJobNext_en = NvM_Prv_GetJobId(QueueEntry_pst->ServiceBit_uo);
            *idBlockToProcess_puo = QueueEntry_pst->idBlock_uo;
        }
    }

    return idJobNext_en;
}

