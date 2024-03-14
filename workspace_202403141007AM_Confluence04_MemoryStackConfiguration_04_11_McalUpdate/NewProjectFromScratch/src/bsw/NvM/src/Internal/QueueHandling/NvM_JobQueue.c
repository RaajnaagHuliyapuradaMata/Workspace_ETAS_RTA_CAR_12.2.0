/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2020, all rights reserved
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

#include "NvM_Prv_JobQueue.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_JobResource.h"
#include "NvM_Prv_RequestQueue.h"
#include "NvM_Prv_InternalBuffer.h"

#if (!defined(NVM_PRV_NR_PARALLEL_JOB) || (NVM_PRV_NR_PARALLEL_JOB == 1u))
# include "NvM_Prv_Queue.h"
#else
# include "NvM_Prv_QueueLinked.h"
#endif

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
# include "TestCd_NvM.h"
#endif

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/**
 * \brief
 * Definition of a union to get different types of pointer to job structure without casting.
 * \details
 * The NvM uses such unions to avoid MISRA warnings.
 */
typedef union
{
    /// Byte pointer to job structure.
    uint8*                  Job_pu8;
    /// Byte pointer to constant job structure.
    uint8 const*            Job_pcu8;
    /// Pointer to job structure using original data type.
    NvM_Prv_Job_tst*        Job_pst;
    /// Pointer to constant job structure using original data type.
    NvM_Prv_Job_tst const*  Job_pcst;

} NvM_Prv_Job_Data_tun;

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define NVM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"

/// Definition of the array for the internal job queue.
/// All jobs in this queue can be processed in parallel if they require different job resources.
static NvM_Prv_Job_tst NvM_Prv_JobQueue_Elements_ast[NVM_PRV_NR_PARALLEL_JOB];

/// Array with insertion flags for each element in the queue.
/// NvM can use insertion flags to minimize scheduler lock time while enqueuing/dequeuing elements.
static boolean NvM_Prv_JobQueue_isInserted_ab[NVM_PRV_NR_PARALLEL_JOB];

#if (defined(NVM_PRV_NR_PARALLEL_JOB) && (NVM_PRV_NR_PARALLEL_JOB > 1u))

/// Array with indexes to next element in the queue.
/// If there is no next element then index to the next value will have invalid value NVM_PRV_QUEUE_IDX_INVALID.
/// This array is only required if linked queue is ised for handling internal jobs.
static NvM_Prv_Queue_idx_tuo NvM_Prv_JobQueue_idxNext_auo[NVM_PRV_NR_PARALLEL_JOB];
/// Array with indexes to next free element in the queue.
/// If there is no next free element then index to the next value will have invalid value NVM_PRV_QUEUE_IDX_INVALID.
/// This array is only required if linked queue is ised for handling internal jobs.
static NvM_Prv_Queue_idx_tuo NvM_Prv_JobQueue_idxNextFree_auo[NVM_PRV_NR_PARALLEL_JOB];

#endif

#define NVM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"

#define NVM_START_SEC_VAR_INIT_UNSPECIFIED
#include "NvM_MemMap.h"

/// Definition of administrative data for the internal job queue.
/// If there is more than 1 entry then NvM processes all available entries in parallel if possible.
static NvM_Prv_JobQueue_DataAdmin_tst NvM_Prv_JobQueue_DataAdmin_st =
{
#if (!defined(NVM_PRV_NR_PARALLEL_JOB) || (NVM_PRV_NR_PARALLEL_JOB == 1u))

    // administrative data for the internal job queue implemented as FIFO queue
    NVM_PRV_NR_PARALLEL_JOB,        // size_cu16
    NVM_PRV_NR_PARALLEL_JOB,        // cntrFreeEntries_u16
    0,                              // idxHead_u16
    NvM_Prv_JobQueue_isInserted_ab  // isInserted_cpb

#else

    // administrative data for the internal job queue implemented as linked queue
    NVM_PRV_NR_PARALLEL_JOB,            // size_cuo
    NVM_PRV_NR_PARALLEL_JOB,            // cntrFreeEntries_uo
    0u,                                 // idxHead_uo
    NvM_Prv_JobQueue_isInserted_ab,     // isInserted_cpb
    0u,                                 // idxTail_uo
    0u,                                 // idxHeadFree_uo
    NvM_Prv_JobQueue_idxNext_auo,       // idxNext_cpuo
    NvM_Prv_JobQueue_idxNextFree_auo    // idxNextFree_cpuo

#endif
};

#define NVM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

/// Definition of the structure for internal job provided by the NvM.
static NvM_Prv_JobQueue_tst const NvM_Prv_JobQueue_cst =
{
    &NvM_Prv_JobQueue_DataAdmin_st,     // Queue_cpst
    NvM_Prv_JobQueue_Elements_ast,      // data_cpo
};

/// Definition of the structure with functions required to handle internal job queue.
/// Parallel processing of internal jobs is only possible if linked queue is used.
static NvM_Prv_JobQueue_Fcts_tst const NvM_Prv_JobQueue_Fcts_st =
{
#if (!defined(NVM_PRV_NR_PARALLEL_JOB) || (NVM_PRV_NR_PARALLEL_JOB == 1u))

    NvM_Prv_Queue_Initialize,               // Initialize_pfct
    NvM_Prv_Queue_IsEmpty,                  // IsEmpty_pfct
    NvM_Prv_Queue_IsFull,                   // IsFull_pfct
    NvM_Prv_Queue_GetNrEntries,             // GetNrEntries_pfct
    NvM_Prv_Queue_Enqueue,                  // Enqueue_pfct
    NvM_Prv_Queue_Insert,                   // Insert_pfct
    NvM_Prv_Queue_Dequeue,                  // Dequeue_pfct
    NvM_Prv_Queue_GetIdxElement,            // GetIdxElement

#else

    NvM_Prv_QueueLinked_Initialize,         // Initialize_pfct
    NvM_Prv_QueueLinked_IsEmpty,            // IsEmpty_pfct
    NvM_Prv_QueueLinked_IsFull,             // IsFull_pfct
    NvM_Prv_QueueLinked_GetNrEntries,       // GetNrEntries_pfct
    NvM_Prv_QueueLinked_Enqueue,            // Enqueue_pfct
    NvM_Prv_QueueLinked_Insert,             // Insert_pfct
    NvM_Prv_QueueLinked_Dequeue,            // Dequeue_pfct
    NvM_Prv_QueueLinked_GetIdxElement       // GetIdxElement

#endif
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

/**
 * \brief
 * This private function initializes the queue for internal jobs.
 */
void NvM_Prv_JobQueue_Initialize(void)
{
    NvM_Prv_JobQueue_Fcts_st.Initialize_pfct(NvM_Prv_JobQueue_cst.Queue_cpst);
}

/**
 * \brief
 * This private function enqueues an internal job.
 * \details
 * This function always enqueues a new job at the end of the queue.
 *
 * \return
 * Index in the job queue where job shall be inserted
 */
NvM_Prv_Queue_idx_tuo NvM_Prv_JobQueue_Enqueue(void)
{
    return NvM_Prv_JobQueue_Fcts_st.Enqueue_pfct(NvM_Prv_JobQueue_cst.Queue_cpst);
}

/**
 * \brief
 * This private function inserts a job into the job queue at given position.
 * \details
 * \attention
 * This function inserts a user request at given position.
 * Insertion of a user request can be done without scheduler lock.
 *
 * \param Job_pcst
 * Pointer to the job to be inserted
 * \param idxPosition_uo
 * Index in the job buffer to insert given job
 */
void NvM_Prv_JobQueue_Insert(NvM_Prv_Job_tst const* Job_pcst, NvM_Prv_Queue_idx_tuo idxPosition_uo)
{
    NvM_Prv_Job_Data_tun JobData_un;
    NvM_Prv_Job_Data_tun QueueBuffer_un;

    JobData_un.Job_pcst = Job_pcst;
    QueueBuffer_un.Job_pst = NvM_Prv_JobQueue_Elements_ast;

    NvM_Prv_JobQueue_Fcts_st.Insert_pfct(NvM_Prv_JobQueue_cst.Queue_cpst,
                                         idxPosition_uo,
                                         QueueBuffer_un.Job_pu8,
                                         JobData_un.Job_pcu8,
                                         sizeof(NvM_Prv_Job_tst));

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    TestCd_NvM_JobMonitor_Start(&NvM_Prv_JobQueue_Elements_ast[idxPosition_uo].JobData_st);
#endif

}

/**
 * \brief
 * This private function dequeues an internal job at given position.
 *
 * \param idxJob_pcst
 * Structure with indexes defining position of the element to dequeue
 */
void NvM_Prv_JobQueue_Dequeue(NvM_Prv_Queue_idx_tst const* idxJob_pcst)
{
    NvM_Prv_JobQueue_Fcts_st.Dequeue_pfct(NvM_Prv_JobQueue_cst.Queue_cpst, idxJob_pcst);
}

/**
 * \brief
 * This private function returns whether the job queue is full.
 *
 * \return
 * - true = job queue is full
 * - false = job queue is not full, i.e. at least one element can be enqueued
 */
boolean NvM_Prv_JobQueue_IsFull(void)
{
    return NvM_Prv_JobQueue_Fcts_st.IsFull_pfct(NvM_Prv_JobQueue_cst.Queue_cpst);
}

/**
 * \brief
 * This private function returns whether the job queue is empty.
 *
 * \return
 * - true = job queue is empty
 * - false = job queue is not empty, i.e. at least one element is currently enqueued
 */
boolean NvM_Prv_JobQueue_IsEmpty(void)
{
    return NvM_Prv_JobQueue_Fcts_st.IsEmpty_pfct(NvM_Prv_JobQueue_cst.Queue_cpst);
}

/**
 * \brief
 * This private function returns number of jobs currently enqueud.
 *
 * \return
 * Number of jobs currently enqueued
 */
NvM_Prv_Queue_idx_tuo NvM_Prv_JobQueue_GetNrEntries(void)
{
    return NvM_Prv_JobQueue_Fcts_st.GetNrEntries_pfct(NvM_Prv_JobQueue_cst.Queue_cpst);
}

/**
 * \brief
 * This private function returns the job index in the array implementing the job queue identified by the passed number.
 * \details
 * \attention
 * Do not confuse the job number and the job index.
 *
 * The job number means enqueuing order in the job queue. First (oldest) enqueued job has number 0, second enqueud job
 * has number 1, third enqueued job has number 2, etc.
 * The job index means the index in the array which implements the job queue.
 *
 * \param[in] nrJob_uo
 * Number of the job in the queue for which the index is required
 *
 * \return
 * Index in the array which implements the job queue
 */
NvM_Prv_Queue_idx_tuo NvM_Prv_JobQueue_GetIdxJob(uint8 nrJob_uo)
{
    return NvM_Prv_JobQueue_Fcts_st.GetIdxElement(NvM_Prv_JobQueue_cst.Queue_cpst, nrJob_uo);
}

/**
 * \brief
 * This private function returns the pointer to the entry in the job queue identified by the passed number.
 * \details
 * For number 0 this function returns the first (oldest) entry in the job queue.
 * For number 1 the next one, for number 2 the next one etc.
 * Via this function the NvM can iterate over all jobs in the queue.
 *
 * \attention
 * The caller has to ensure that the job queue contains enough jobs to return the required number.
 *
 * \attention
 * Do not confuse the job number and the job index.
 *
 * \param[in] nrJob_uo
 * Number of the job in the queue to be returned
 *
 * \return
 * Pointer to the job in the queue identified by the passed number
 */
NvM_Prv_Job_tst* NvM_Prv_JobQueue_GetJob(uint8 nrJob_uo)
{
    NvM_Prv_Job_tst* Job_pst = &(NvM_Prv_JobQueue_cst.data_cpo[NvM_Prv_JobQueue_GetIdxJob(nrJob_uo)]);

    return Job_pst;
}

#if (NVM_JOB_PRIORITIZATION == STD_ON)

boolean NvM_Prv_JobQueue_SuspendJob(NvM_Prv_Queue_idx_tst const* idxJob_pcst)
{
    boolean isSuspended_b = FALSE;
    // move request causing current job into the queue for suspended requests so that NvM can process this request later

    NvM_Prv_Job_tst const* Job_pcst = &(NvM_Prv_JobQueue_cst.data_cpo[idxJob_pcst->idx_uo]);

    // move suspended request into the queue for suspended requests only if it is not a multi-block request
    if (NvM_Prv_idQueue_Multi_e != Job_pcst->JobData_st.idQueue_uo)
    {
        uint8 idxRequestQueue_uo = 0u;

        // Prepare entry for suspended queue based on suspended job
        NvM_Prv_QueueEntry_tst QueueEntry_st;
        QueueEntry_st.idService_uo                  = Job_pcst->JobData_st.idService_uo;
        QueueEntry_st.idQueueOriginal_uo            = Job_pcst->JobData_st.idQueue_uo;
        QueueEntry_st.idBlock_uo                    = Job_pcst->JobData_st.idBlock_uo;
        QueueEntry_st.ServiceBit_uo                 = Job_pcst->JobData_st.ServiceBit_uo;
        QueueEntry_st.BlockData_un.ptrRamBlock_pv   = Job_pcst->JobData_st.UserData_un.ptrRamBlock_pv;

        if (NvM_Prv_RequestQueue_Enqueue(NvM_Prv_idQueue_Suspended_e, &idxRequestQueue_uo))
        {
            isSuspended_b = TRUE;
            NvM_Prv_RequestQueue_Insert(NvM_Prv_idQueue_Suspended_e, &QueueEntry_st, idxRequestQueue_uo);
        }
    }

    if ((NvM_Prv_idQueue_Multi_e == Job_pcst->JobData_st.idQueue_uo) || isSuspended_b)
    {
        isSuspended_b = TRUE;
        // Free all resources claimed by suspended block
        NvM_Prv_JobResource_Unclaim(Job_pcst->JobData_st.idBlock_uo);
        NvM_Prv_JobQueue_Dequeue(idxJob_pcst);
        // After dequeuing the job is not ongoing anymore
        NvM_Prv_Block_SetState(Job_pcst->JobData_st.idBlock_uo, NVM_BLOCK_STATE_JOB_ONGOING, 0u);
    }

    return isSuspended_b;
}

#endif

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

uint8* NvM_Prv_JobQueue_GetUsedIntBfr(NvM_BlockIdType idBlock_uo, NvM_Prv_idService_tuo idService_uo)
{
    NvM_Prv_Job_tst const* Job_pcst = NULL_PTR;
    uint16 idxJob_u16 = 0u;
    uint8* IntBfr_pu8 = NULL_PTR;

    do
    {
        // search in all available job entries for job with required block and service ID
        Job_pcst = &NvM_Prv_JobQueue_Elements_ast[idxJob_u16];
        if ((Job_pcst->JobData_st.idBlock_uo != idBlock_uo) || (Job_pcst->JobData_st.idService_uo != idService_uo))
        {
            ++idxJob_u16;
        }
        else
        {
            IntBfr_pu8 = Job_pcst->JobData_st.IntBuffer_st.Buffer_pu8;
        }

    } while ((NULL_PTR == IntBfr_pu8) && (idxJob_u16 < NVM_PRV_NR_PARALLEL_JOB));

    return IntBfr_pu8;
}

NvM_Prv_Job_tst* NvM_Prv_JobQueue_GetJobData(NvM_BlockIdType idBlock_uo, NvM_Prv_idService_tuo idService_uo)
{
    NvM_Prv_Job_tst* Job_pst = NULL_PTR;
    uint16 idxJob_u16 = 0u;

    do
    {
        // search in all available job entries for job with required block and service ID
        if (   (NvM_Prv_JobQueue_Elements_ast[idxJob_u16].JobData_st.idBlock_uo != idBlock_uo)
            || (NvM_Prv_JobQueue_Elements_ast[idxJob_u16].JobData_st.idService_uo != idService_uo))
        {
            ++idxJob_u16;
        }
        else
        {
            Job_pst = &NvM_Prv_JobQueue_Elements_ast[idxJob_u16];
        }

    } while ((NULL_PTR == Job_pst) && (idxJob_u16 < NVM_PRV_NR_PARALLEL_JOB));

    return Job_pst;
}

#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

