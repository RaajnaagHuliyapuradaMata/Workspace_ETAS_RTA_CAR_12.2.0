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
#ifndef NVM_PRV_REQUESTQUEUE_TYPES_H
#define NVM_PRV_REQUESTQUEUE_TYPES_H
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
/// Type definition for administrative data required by the user request queue.
/// If no user request processing in parallel is required then the NvM uses a simple FIFO queue as job queue.
/// Otherwise the NvM uses a linked queue as user request queue.
#if (!defined(NVM_PRV_NR_PARALLEL_JOB) || (NVM_PRV_NR_PARALLEL_JOB == 1u))
typedef NvM_Prv_Queue_tst           NvM_Prv_RequestQueue_DataAdmin_tst;
#else
typedef NvM_Prv_QueueLinked_tst     NvM_Prv_RequestQueue_DataAdmin_tst;
#endif

/**
 * \brief
 * Definition of the structure which defines a queue for user requests.
 * \details
 * This definition extends the circular buffer queue provided by the unit NvM_Queue (see NvM_Prv_Queue_tst)
 * by adding following elements:
 * - array to store enqueued user requests
 * - pointer to a special variable to monitor minimum number of free entries in the queue if required
 */
typedef struct
{
    /// Constant pointer to the queue handling user requests
    NvM_Prv_RequestQueue_DataAdmin_tst* const   Queue_cpst;
    /// Constant pointer to the array to store user request data.
    NvM_Prv_QueueEntry_tst* const               data_cpo;
    /// Constant pointer to the component global variable which holds the minimum number of free entries in the queue.
    uint8* const                                minNrFreeEntries_cpu8;

} NvM_Prv_RequestQueue_tst;

/**
 * \brief
 * Definition of the structure which defines functions used for user request handling.
 * \details
 * If no user request processing in parallel is required then the NvM uses functions for a simple FIFO queue.
 * Otherwise the NvM uses functions for a linked queue.
 */
typedef struct
{
    void (*Initialize_pfct)(NvM_Prv_RequestQueue_DataAdmin_tst* QueueDataAdmin_pst);
    boolean (*IsEmpty_pfct)(NvM_Prv_RequestQueue_DataAdmin_tst const* QueueDataAdmin_pcst);
    boolean (*IsFull_pfct)(NvM_Prv_RequestQueue_DataAdmin_tst const* QueueDataAdmin_pcst);
    NvM_Prv_Queue_idx_tuo (*GetNrFreeEntries_pfct)(NvM_Prv_RequestQueue_DataAdmin_tst const* QueueDataAdmin_pcst);
    NvM_Prv_Queue_idx_tuo (*GetNrEntries_pfct)(NvM_Prv_RequestQueue_DataAdmin_tst const* QueueDataAdmin_pcst);
    NvM_Prv_Queue_idx_tuo (*Enqueue_pfct)(NvM_Prv_RequestQueue_DataAdmin_tst* QueueDataAdmin_pst);
    void (*Insert_pfct)(NvM_Prv_RequestQueue_DataAdmin_tst const* QueueDataAdmin_pcst,
                        NvM_Prv_Queue_idx_tuo idx_uo,
                        uint8* QueueBuffer_pu8,
                        uint8 const* ElementToEnqueue_pcu8,
                        uint32 ElementSize_u32);
    boolean (*IsInserted_pfct)(NvM_Prv_RequestQueue_DataAdmin_tst const* QueueDataAdmin_pcst,
                               NvM_Prv_Queue_idx_tuo idxRequest_uo);
    void (*Dequeue_pfct)(NvM_Prv_RequestQueue_DataAdmin_tst* QueueDataAdmin_pst, NvM_Prv_Queue_idx_tst const* idx_pcst);
    void (*GetNext_pfct)(NvM_Prv_RequestQueue_DataAdmin_tst const* QueueDataAdmin_pcst,
                         NvM_Prv_Queue_idx_tuo* idxRequest_puo);

} NvM_Prv_RequestQueue_Fcts_tst;

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
/**
 * \brief
 * Size definition of the queue for multi-block requests.
 * \details
 * Queue for multi-block requests has always only one entry.
 */
#define NVM_PRV_SIZE_MULTI_QUEUE    (1u)

/**
 * \brief
 * Size definition of the queue for auxiliary requests.
 * \details
 * If enabled by the parameter NvMRbAuxInterface the queue for auxiliary requests has only one entry,
 * otherwise the queue for auxiliary requests is not available -> size = 0.
 */
// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2933] Queue for auxiliary requests has only one single entry
#if (NVM_RB_AUX_INTERFACE == STD_ON)
# define NVM_PRV_SIZE_AUX_QUEUE     (1u)
#else
# define NVM_PRV_SIZE_AUX_QUEUE     (0u)
#endif

/**
 * \brief
 * Size definition of the queue for single-block requests with immediate priority.
 * \details
 * If enabled by the parameter NvMJobPrioritization the queue size is defined by the parameter NvMSizeImmediateJobQueue,
 * otherwise the queue is not available -> size = 0.
 * \note
 * If the immediate queue is disabled then NVM_SIZE_IMMEDIATE_JOB_QUEUE = 1 due to the implementation
 * in corrresponding BCT-action. For this reason queue management is using own constant to get queue size 0.
 */
#if (NVM_JOB_PRIORITIZATION == STD_ON)
# define NVM_PRV_SIZE_IMMEDIATE_QUEUE   (NVM_SIZE_IMMEDIATE_JOB_QUEUE)
#else
# define NVM_PRV_SIZE_IMMEDIATE_QUEUE   (0u)
#endif

/**
 * \brief
 * Size definition of the queue for all requests provided by the NvM.
 */
#define NVM_PRV_SIZE_OVERALL_QUEUE (NVM_PRV_SIZE_MULTI_QUEUE            +   \
                                    NVM_PRV_SIZE_AUX_QUEUE              +   \
                                    NVM_SIZE_STANDARD_JOB_QUEUE         +   \
                                    NVM_PRV_SIZE_SUSPENDED_JOB_QUEUE    +   \
                                    NVM_PRV_SIZE_IMMEDIATE_QUEUE)


/* NVM_PRV_REQUESTQUEUE_TYPES_H */
#endif

