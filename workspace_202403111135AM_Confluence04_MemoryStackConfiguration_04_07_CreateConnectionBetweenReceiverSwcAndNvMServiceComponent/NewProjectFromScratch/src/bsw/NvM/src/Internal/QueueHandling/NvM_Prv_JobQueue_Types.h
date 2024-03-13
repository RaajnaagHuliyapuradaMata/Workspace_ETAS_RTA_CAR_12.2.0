/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_JOBQUEUE_TYPES_H
#define NVM_PRV_JOBQUEUE_TYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv.h"
#include "NvM_Types.h"
#if (!defined(NVM_PRV_NR_PARALLEL_JOB) || (NVM_PRV_NR_PARALLEL_JOB == 1))
# include "NvM_Prv_Queue_Types.h"
#else
# include "NvM_Prv_QueueLinked_Types.h"
#endif

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

/// Type definition for administrative data required by the internal job queue.
/// If no job processing in parallel is required then the NvM uses a simple FIFO queue as job queue.
/// Otherwise the NvM uses a linked queue as job queue.
#if (!defined(NVM_PRV_NR_PARALLEL_JOB) || (NVM_PRV_NR_PARALLEL_JOB == 1u))
typedef NvM_Prv_Queue_tst           NvM_Prv_JobQueue_DataAdmin_tst;
#else
typedef NvM_Prv_QueueLinked_tst     NvM_Prv_JobQueue_DataAdmin_tst;
#endif

/**
 * \brief
 * Definition of the structure which defines a queue for all pending internal jobs.
 * \details
 * This structure consists of 2 elements required for internal job queue:
 * - pointer to the administrative data
 * - pointer to the array to store job data
 */
typedef struct
{
    /// Constant pointer to the structure with administarative data required to handle the queue.
    NvM_Prv_JobQueue_DataAdmin_tst* const   Queue_cpst;
    /// Constant pointer to the array to store job data.
    NvM_Prv_Job_tst* const                  data_cpo;

} NvM_Prv_JobQueue_tst;

/**
 * \brief
 * Definition of the structure which defines functions used for internal job handling.
 * \details
 * If no job processing in parallel is required then the NvM uses functions for a simple FIFO queue.
 * Otherwise the NvM uses functions for a linked queue.
 */
typedef struct
{
    void (*Initialize_pfct)(NvM_Prv_JobQueue_DataAdmin_tst* QueueDataAdmin_pst);
    boolean (*IsEmpty_pfct)(NvM_Prv_JobQueue_DataAdmin_tst const* QueueDataAdmin_pcst);
    boolean (*IsFull_pfct)(NvM_Prv_JobQueue_DataAdmin_tst const* QueueDataAdmin_pcst);
    NvM_Prv_Queue_idx_tuo (*GetNrEntries_pfct)(NvM_Prv_JobQueue_DataAdmin_tst const* QueueDataAdmin_pcst);
    NvM_Prv_Queue_idx_tuo (*Enqueue_pfct)(NvM_Prv_JobQueue_DataAdmin_tst* QueueDataAdmin_pst);
    void (*Insert_pfct)(NvM_Prv_JobQueue_DataAdmin_tst const* QueueDataAdmin_pcst,
                        NvM_Prv_Queue_idx_tuo idx_uo,
                        uint8* QueueBuffer_pu8,
                        uint8 const* ElementToEnqueue_pcu8,
                        uint32 ElementSize_u32);
    void (*Dequeue_pfct)(NvM_Prv_JobQueue_DataAdmin_tst* QueueDataAdmin_pst, NvM_Prv_Queue_idx_tst const* idx_pcst);
    NvM_Prv_Queue_idx_tuo (*GetIdxElement)(NvM_Prv_JobQueue_DataAdmin_tst const* QueueDataAdmin_pcst, uint8 nrElement_uo);

} NvM_Prv_JobQueue_Fcts_tst;

/* NVM_PRV_JOBQUEUE_TYPES_H */
#endif

