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

#include "rba_MemLib.h"
#include "rba_MemLib_Special.h"

#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_RequestQueue.h"
#include "NvM_Prv_JobResource.h"
#include "NvM_Prv_ProcessMultiBlock.h"

#if (!defined(NVM_PRV_NR_PARALLEL_JOB) || (NVM_PRV_NR_PARALLEL_JOB == 1u))
# include "NvM_Prv_Queue.h"
#else
# include "NvM_Prv_QueueLinked.h"
#endif

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/**
 * \brief
 * Definition of a union to get different types of pointer to user request data without casting.
 * \details
 * The NvM uses such unions to avoid MISRA warnings.
 */
typedef union
{
    /// Byte pointer to user request data.
    uint8*                          RequestData_pu8;
    /// Byte pointer to constant user request data.
    uint8 const*                    RequestData_pcu8;
    /// Pointer to user request data using original data type.
    NvM_Prv_QueueEntry_tst*         RequestData_pst;
    /// Pointer to constant user request data using original data type.
    NvM_Prv_QueueEntry_tst const*   RequestData_pcst;

} NvM_Prv_Request_Data_tun;

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
// Queue monitoring
#define NVM_START_SEC_VAR_SAVED_ZONE_8
#include "NvM_MemMap.h"
/// Minimum number of free entries in the standard queue reached so far during the runtime of the ECU.
uint8 NvM_Rb_minNrFreeStdQueueEntries_u8;

#if (NVM_JOB_PRIORITIZATION == STD_ON)
/// Minimum number of free entries in the immediate queue reached so far during the runtime of the ECU.
uint8 NvM_Rb_minNrFreeImmQueueEntries_u8;
#endif

#define NVM_STOP_SEC_VAR_SAVED_ZONE_8
#include "NvM_MemMap.h"


#define NVM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"

/// Definition of the array for all user requests.
static NvM_Prv_QueueEntry_tst NvM_Prv_RequestQueue_Entries_ast[NVM_PRV_SIZE_OVERALL_QUEUE];

static volatile boolean NvM_Prv_RequestQueue_isInserted_avb[NVM_PRV_SIZE_OVERALL_QUEUE];

#if (defined(NVM_PRV_NR_PARALLEL_JOB) && (NVM_PRV_NR_PARALLEL_JOB > 1u))
static NvM_Prv_Queue_idx_tuo NvM_Prv_RequestQueue_idxNext_auo[NVM_PRV_SIZE_OVERALL_QUEUE];
static NvM_Prv_Queue_idx_tuo NvM_Prv_RequestQueue_idxPrev_auo[NVM_PRV_SIZE_OVERALL_QUEUE];
#endif

#define NVM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"

#define NVM_START_SEC_VAR_INIT_UNSPECIFIED
#include "NvM_MemMap.h"

/// Definition of the array with administrative data for all queues for user requests.
static NvM_Prv_RequestQueue_DataAdmin_tst NvM_Prv_RequestQueue_DataAdmin_ast[NvM_Prv_idQueue_nrQueues_e] =
{
#if (!defined(NVM_PRV_NR_PARALLEL_JOB) || (NVM_PRV_NR_PARALLEL_JOB == 1u))
    // administrative data for the queue for multi-block requests
    {
        NVM_PRV_SIZE_MULTI_QUEUE,                                           // size_cuo
        NVM_PRV_SIZE_MULTI_QUEUE,                                           // cntrFreeEntries_uo
        0u,                                                                 // idxHead_uo
        &NvM_Prv_RequestQueue_isInserted_avb[0u]                            // isInserted_cpb
    },
    // administrative data for the queue for single-block requests with standard priority
    {
        NVM_SIZE_STANDARD_JOB_QUEUE,                                        // size_cuo
        NVM_SIZE_STANDARD_JOB_QUEUE,                                        // cntrFreeEntries_uo
        0u,                                                                 // idxHead_uo
        &NvM_Prv_RequestQueue_isInserted_avb[NVM_PRV_SIZE_MULTI_QUEUE]      // isInserted_cpb
    }
# if (NVM_RB_AUX_INTERFACE == STD_ON)
    ,
    // administrative data for the queue for auxiliary requests
    {
        NVM_PRV_SIZE_AUX_QUEUE,                                             // size_cuo
        NVM_PRV_SIZE_AUX_QUEUE,                                             // cntrFreeEntries_uo
        0u,                                                                 // idxHead_uo
        &NvM_Prv_RequestQueue_isInserted_avb[NVM_PRV_SIZE_MULTI_QUEUE   +
                                             NVM_SIZE_STANDARD_JOB_QUEUE]   // isInserted_cpb
    }
# endif
# if (NVM_JOB_PRIORITIZATION == STD_ON)
    ,
    // administrative data for the queue for suspended single-block requests
    {
        NVM_PRV_SIZE_SUSPENDED_JOB_QUEUE,                                   // size_cuo
        NVM_PRV_SIZE_SUSPENDED_JOB_QUEUE,                                   // cntrFreeEntries
        0u,                                                                 // idxHead_uo
        &NvM_Prv_RequestQueue_isInserted_avb[NVM_PRV_SIZE_MULTI_QUEUE    +
                                             NVM_SIZE_STANDARD_JOB_QUEUE +
                                             NVM_PRV_SIZE_AUX_QUEUE]        // isInserted_cpb
    }
    ,
    // administrative data for the queue for single-block requests with immediate priority
    {
        NVM_PRV_SIZE_IMMEDIATE_QUEUE,                                           // size_cuo
        NVM_PRV_SIZE_IMMEDIATE_QUEUE,                                           // cntrFreeEntries_uo
        0u,                                                                     // idxHead_uo
        &NvM_Prv_RequestQueue_isInserted_avb[NVM_PRV_SIZE_MULTI_QUEUE          +
                                             NVM_SIZE_STANDARD_JOB_QUEUE       +
                                             NVM_PRV_SIZE_AUX_QUEUE            +
                                             NVM_PRV_SIZE_SUSPENDED_JOB_QUEUE]  // isInserted_cpb

    }
# endif
#else
    // administrative data for the queue for multi-block requests
    {
        NVM_PRV_SIZE_MULTI_QUEUE,                                           // size_cuo
        NVM_PRV_SIZE_MULTI_QUEUE,                                           // cntrFreeEntries_uo
        0u,                                                                 // idxHead_uo
        &NvM_Prv_RequestQueue_isInserted_avb[0u],                           // isInserted_cpb
        0u,                                                                 // idxTail_uo
        0u,                                                                 // idxHeadFree_uo
        &NvM_Prv_RequestQueue_idxNext_auo[0u],                              // idxNext_cpuo
        &NvM_Prv_RequestQueue_idxPrev_auo[0u]                               // idxPrev_cpuo
    },
    // administrative data for the queue for single-block requests with standard priority
    {
        NVM_SIZE_STANDARD_JOB_QUEUE,                                        // size_cuo
        NVM_SIZE_STANDARD_JOB_QUEUE,                                        // cntrFreeEntries_uo
        0u,                                                                 // idxHead_uo
        &NvM_Prv_RequestQueue_isInserted_avb[NVM_PRV_SIZE_MULTI_QUEUE],     // isInserted_cpb
        0u,                                                                 // idxTail_uo
        0u,                                                                 // idxHeadFree_uo
        &NvM_Prv_RequestQueue_idxNext_auo[NVM_PRV_SIZE_MULTI_QUEUE],        // idxNext_cpuo
        &NvM_Prv_RequestQueue_idxPrev_auo[NVM_PRV_SIZE_MULTI_QUEUE]         // idxPrev_cpuo
    }
# if (NVM_RB_AUX_INTERFACE == STD_ON)
    ,
    // administrative data for the queue for auxiliary requests
    {
        NVM_PRV_SIZE_AUX_QUEUE,                                             // size_cuo
        NVM_PRV_SIZE_AUX_QUEUE,                                             // cntrFreeEntries_uo
        0u,                                                                 // idxHead_uo
        &NvM_Prv_RequestQueue_isInserted_avb[NVM_PRV_SIZE_MULTI_QUEUE+
                                          NVM_SIZE_STANDARD_JOB_QUEUE],     // isInserted_cpb
        0u,                                                                 // idxTail_uo
        0u,                                                                 // idxHeadFree_uo
        &NvM_Prv_RequestQueue_idxNext_auo[NVM_PRV_SIZE_MULTI_QUEUE   +
                                          NVM_SIZE_STANDARD_JOB_QUEUE],     // idxNext_cpuo
        &NvM_Prv_RequestQueue_idxPrev_auo[NVM_PRV_SIZE_MULTI_QUEUE   +
                                          NVM_SIZE_STANDARD_JOB_QUEUE]      // idxPrev_cpuo
    }
# endif
# if (NVM_JOB_PRIORITIZATION == STD_ON)
    ,
    // administrative data for the queue for suspended single-block requests
    {
        NVM_PRV_SIZE_SUSPENDED_JOB_QUEUE,                                   // size_cuo
        NVM_PRV_SIZE_SUSPENDED_JOB_QUEUE,                                   // cntrFreeEntries
        0u,                                                                 // idxHead_uo
        &NvM_Prv_RequestQueue_isInserted_avb[NVM_PRV_SIZE_MULTI_QUEUE +
                                             NVM_SIZE_STANDARD_JOB_QUEUE +
                                             NVM_PRV_SIZE_AUX_QUEUE],       // isInserted_cpb
        0u,                                                                 // idxTail_uo
        0u,                                                                 // idxHeadFree_uo
        &NvM_Prv_RequestQueue_idxNext_auo[NVM_PRV_SIZE_MULTI_QUEUE +
                                          NVM_SIZE_STANDARD_JOB_QUEUE +
                                          NVM_PRV_SIZE_AUX_QUEUE],          // idxNext_cpuo
        &NvM_Prv_RequestQueue_idxPrev_auo[NVM_PRV_SIZE_MULTI_QUEUE +
                                          NVM_SIZE_STANDARD_JOB_QUEUE +
                                          NVM_PRV_SIZE_AUX_QUEUE]           // idxPrev_cpuo
    },
    // administrative data for the queue for single-block requests with immediate priority
    {
        NVM_PRV_SIZE_IMMEDIATE_QUEUE,                                       // size_cuo
        NVM_PRV_SIZE_IMMEDIATE_QUEUE,                                       // cntrFreeEntries_uo
        0u,                                                                 // idxHead_uo
        &NvM_Prv_RequestQueue_isInserted_avb[NVM_PRV_SIZE_MULTI_QUEUE           +
                                             NVM_SIZE_STANDARD_JOB_QUEUE        +
                                             NVM_PRV_SIZE_AUX_QUEUE             +
                                             NVM_PRV_SIZE_SUSPENDED_JOB_QUEUE], // isInserted_cpb
        0u,                                                                     // idxTail_uo
        0u,                                                                     // idxHeadFree_uo
        &NvM_Prv_RequestQueue_idxNext_auo[NVM_PRV_SIZE_MULTI_QUEUE          +
                                          NVM_SIZE_STANDARD_JOB_QUEUE       +
                                          NVM_PRV_SIZE_AUX_QUEUE            +
                                          NVM_PRV_SIZE_SUSPENDED_JOB_QUEUE],    // idxNext_cpuo
        &NvM_Prv_RequestQueue_idxPrev_auo[NVM_PRV_SIZE_MULTI_QUEUE          +
                                          NVM_SIZE_STANDARD_JOB_QUEUE       +
                                          NVM_PRV_SIZE_AUX_QUEUE            +
                                          NVM_PRV_SIZE_SUSPENDED_JOB_QUEUE]     // idxPrev_cpuo
    }
# endif
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

/// Definition of the array for all queues for user requests provided by the NvM.
static NvM_Prv_RequestQueue_tst const NvM_Prv_RequestQueues_cast[NvM_Prv_idQueue_nrQueues_e] =
{
    // queue for multi-block requests
    {
        &NvM_Prv_RequestQueue_DataAdmin_ast[NvM_Prv_idQueue_Multi_e],       // Queue_cpst
        &NvM_Prv_RequestQueue_Entries_ast[0],                               // data_cpo
        NULL_PTR                                                            // minNrFreeEntries_cpu8
    },

    // queue for single-block requests with standard priority
    {
        &NvM_Prv_RequestQueue_DataAdmin_ast[NvM_Prv_idQueue_Standard_e],    // Queue_cpst
        &NvM_Prv_RequestQueue_Entries_ast[NVM_PRV_SIZE_MULTI_QUEUE],        // data_cpo
        &NvM_Rb_minNrFreeStdQueueEntries_u8,                                // minNrFreeEntries_cpu8
    }

#if (NVM_RB_AUX_INTERFACE == STD_ON)
    ,
    // queue for auxiliary requests
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2932] Own queue for auxiliary jobs
    {
        &NvM_Prv_RequestQueue_DataAdmin_ast[NvM_Prv_idQueue_Aux_e],         // Queue_cpst
        &NvM_Prv_RequestQueue_Entries_ast[NVM_PRV_SIZE_MULTI_QUEUE   +
                                          NVM_SIZE_STANDARD_JOB_QUEUE],     // data_cpo
        NULL_PTR,                                                           // minNrFreeEntries_cpu8
    }
#endif

// TRACE[NVM564] NvMJobPrioritization enables/disables immediate job processing
#if (NVM_JOB_PRIORITIZATION == STD_ON)
    ,
    // queue for suspended single-block requests
    {
        &NvM_Prv_RequestQueue_DataAdmin_ast[NvM_Prv_idQueue_Suspended_e],   // Queue_cpst
        &NvM_Prv_RequestQueue_Entries_ast[NVM_PRV_SIZE_MULTI_QUEUE    +
                                          NVM_SIZE_STANDARD_JOB_QUEUE +
                                          NVM_PRV_SIZE_AUX_QUEUE],          // data_cpo
        NULL_PTR,                                                           // minNrFreeEntries_cpu8
    },
    // queue for single-block requests with immediate priority
    {
        &NvM_Prv_RequestQueue_DataAdmin_ast[NvM_Prv_idQueue_Immediate_e],   // Queue_cpst
        &NvM_Prv_RequestQueue_Entries_ast[NVM_PRV_SIZE_MULTI_QUEUE    +
                                          NVM_SIZE_STANDARD_JOB_QUEUE +
                                          NVM_PRV_SIZE_AUX_QUEUE      +
                                          NVM_PRV_NR_PARALLEL_JOB],         // data_cpo
        &NvM_Rb_minNrFreeImmQueueEntries_u8,                                // minNrFreeEntries_cpu8
    }
#endif
};

/// Definition of the structure with function pointers to be used for queue handling
/// The NvM uses a FIFO queue if only 1 user request can be processed.
/// If more than 1 user requests can be processed in parallel then the NvM uses a linked queue.
static NvM_Prv_RequestQueue_Fcts_tst const NvM_Prv_RequestQueue_Fcts_st =
{
#if (!defined(NVM_PRV_NR_PARALLEL_JOB) || (NVM_PRV_NR_PARALLEL_JOB == 1u))

    NvM_Prv_Queue_Initialize,               // Initialize_pfct
    NvM_Prv_Queue_IsEmpty,                  // IsEmpty_pfct
    NvM_Prv_Queue_IsFull,                   // IsFull_pfct
    NvM_Prv_Queue_GetNrFreeEntries,         // GetNrFreeEntries_pfct
    NvM_Prv_Queue_GetNrEntries,             // GetNrEntries_pfct
    NvM_Prv_Queue_Enqueue,                  // Enqueue_pfct
    NvM_Prv_Queue_Insert,                   // Insert_pfct
    NvM_Prv_Queue_IsInserted,               // IsInserted_pfct
    NvM_Prv_Queue_Dequeue,                  // Dequeue_pfct
    NvM_Prv_Queue_GetNextElement            // GetNext_pfct

#else

    NvM_Prv_QueueLinked_Initialize,         // Initialize_pfct
    NvM_Prv_QueueLinked_IsEmpty,            // IsEmpty_pfct
    NvM_Prv_QueueLinked_IsFull,             // IsFull_pfct
    NvM_Prv_QueueLinked_GetNrFreeEntries,   // GetNrFreeEntries_pfct
    NvM_Prv_QueueLinked_GetNrEntries,       // GetNrEntries_pfct
    NvM_Prv_QueueLinked_Enqueue,            // Enqueue_pfct
    NvM_Prv_QueueLinked_Insert,             // Insert_pfct
    NvM_Prv_QueueLinked_IsInserted,         // IsInserted_pfct
    NvM_Prv_QueueLinked_Dequeue,            // Dequeue_pfct
    NvM_Prv_QueueLinked_GetNextElement      // GetNext_pfct

#endif
};

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
 */

/*
**********************************************************************************************************************
* Code
**********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * This function initializes all queues for user requests used by the NvM.
 * \details
 * Queue data stored in the saved zone will be initialized only if is lost.
 *
 * \param[in] isSavedZoneDataLost_b
 * Status information of the data in the saved zone RAM:
 * - true = data in the saved zone RAM is lost after the reset
 * - false = data in the saved zone RAM is unchanged after the reset
 */
void NvM_Prv_RequestQueue_Initialize(boolean isSavedZoneDataLost_b)
{
    NvM_Prv_idQueue_tuo idQueue_uo;

    for (idQueue_uo = 0u; idQueue_uo < NvM_Prv_idQueue_nrQueues_e; ++idQueue_uo)
    {
        /* Note: idQueue_uo is valid here */
        NvM_Prv_RequestQueue_Fcts_st.Initialize_pfct(NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst);

        if (isSavedZoneDataLost_b && (NULL_PTR != NvM_Prv_RequestQueues_cast[idQueue_uo].minNrFreeEntries_cpu8))
        {
            *(NvM_Prv_RequestQueues_cast[idQueue_uo].minNrFreeEntries_cpu8) =
                    NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst->size_cuo;
        }
    }
}

/**
 * \brief
 * This function enqueues a user request into the given queue.
 *
 * \detailed
 * In cases this function is called with enabled scheduler lock the run-time of 12 can be used:
 * - 1 for function call
 * - 4 for internal copy-operations
 * - IsFull_pfct = 1 (1 internal copy-operation)
 * - Enqueue_pfct = 6 (worst case is 6 internal copy-operatios if linked queue is used, s. NvM_Prv_QueueLinked_Enqueue)
 *
 * \attention
 * This function reserves only an entry in the given queue for an element and returns the index of reserved position
 * (s. parameter idxPosition_puo).
 * The user is responsible to insert the element at reserved position. The NvM splits the actual enqueuing in 2 steps,
 * reservation and insertion, to minimize the time for the scheduler lock. Only reservation must be done with enabled
 * scheduler lock. The insertion can be done afterwards without a lock.
 *
 * \param[in] idQueue_uo
 * Id of the queue to enqueue passed user request.
 * \param[inout] idxPosition_puo
 * Index in the queue where user request will be inserted
 *
 * \return
 * - true = passed user request enqueued successfully
 * - false = enqueuing of the passed user request has failed
 */
boolean NvM_Prv_RequestQueue_Enqueue(NvM_Prv_idQueue_tuo idQueue_uo,
                                     NvM_Prv_Queue_idx_tuo* idxPosition_puo)
{
    boolean isRequestEnqueued_b = FALSE;

    // Queue is full -> enqueueing another entry is not possible
    if (!NvM_Prv_RequestQueue_Fcts_st.IsFull_pfct(NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst))
    {
        NvM_Prv_Queue_idx_tuo* minNrFreeEntries_puo = NvM_Prv_RequestQueues_cast[idQueue_uo].minNrFreeEntries_cpu8;

        *idxPosition_puo = NvM_Prv_RequestQueue_Fcts_st.Enqueue_pfct(NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst);

        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2808]
        // Update the minimum number of standard queue free entries
        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2809]
        // Update the minimum number of immediate queue free entries
        if ((minNrFreeEntries_puo != NULL_PTR) &&
            (NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst->cntrFreeEntries_uo < *minNrFreeEntries_puo))
        {
            *minNrFreeEntries_puo = (NvM_Prv_Queue_idx_tuo)NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst->cntrFreeEntries_uo;
        }

        isRequestEnqueued_b = TRUE;
    }

    return isRequestEnqueued_b;
}

/**
 * \brief
 * This private function inserts an element into the given queue at given position.
 * \details
 * \attention
 * This function inserts a user request at given position.
 * Insertion of a user request can be done without scheduler lock.
 *
 * \param idQueue_uo
 * Id of the queue to insert passed user request.
 * \param QueueEntry_pcst
 * Pointer to user request to be inserted
 * \param idxPosition_uo
 * Index in the queue buffer to insert user request
 */
void NvM_Prv_RequestQueue_Insert(NvM_Prv_idQueue_tuo idQueue_uo,
                                 NvM_Prv_QueueEntry_tst const* QueueEntry_pcst,
                                 NvM_Prv_Queue_idx_tuo idxPosition_uo)
{
    NvM_Prv_Request_Data_tun QueueBuffer_un;
    NvM_Prv_Request_Data_tun RequestData_un;

    QueueBuffer_un.RequestData_pcst = NvM_Prv_RequestQueues_cast[idQueue_uo].data_cpo;
    RequestData_un.RequestData_pcst = QueueEntry_pcst;

    NvM_Prv_RequestQueue_Fcts_st.Insert_pfct(NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst,
                                             idxPosition_uo,
                                             QueueBuffer_un.RequestData_pu8,
                                             RequestData_un.RequestData_pcu8,
                                             sizeof(NvM_Prv_QueueEntry_tst));
}

/**
 * \brief
 * This function checks whether the given user request has been fully inserted in the given queue.
 *
 * \detailed
 * The NvM splits the actual enqueuing of a user request in 2 steps, reservation and insertion, to minimize the time
 * for the scheduler lock, s.a. NvM_Prv_RequestQueue_Enqueue + NvM_Prv_RequestQueue_Insert.
 * That means that before dequeuing a user request from the request queue the NvM shall check whether this user request
 * has been fully inserted into the queue.
 * Additionally this function makes sure that the evaluation of the insertion flag will be completed before getting next
 * request.
 *
 * \param[in] idQueue_uo
 * Id of the queue to check passed user request (caller has to ensure the value range)
 * \param[in] idxRequest_uo
 * Index of the element in the queue buffer to be checked
 *
 * \return
 * - true = user request identified by passed index has been fully inserted
 * - false = user request identified by passed index has not been inserted yet
 */
boolean NvM_Prv_RequestQueue_IsInserted(NvM_Prv_idQueue_tuo idQueue_uo, NvM_Prv_Queue_idx_tuo idxRequest_uo)
{
    boolean isInserted_b = FALSE;

    if (NvM_Prv_RequestQueue_Fcts_st.IsInserted_pfct(NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst,
                                                     idxRequest_uo))
    {
        // Special memory sync function to make sure that the evaluation of the insertion flag
        // has been completed before getting next request.
        rba_MemLib_MSync();

        isInserted_b = TRUE;
    }

    return isInserted_b;
}

/**
 * \brief
 * This function dequeues a user request from the given queue at the position identified by the index
 * in the queue buffer.
 * \details
 * In cases this function is called with enabled scheduler lock the run-time of 9 can be used:
 * - 1 for function call
 * - Dequeue_pfct = 8 (worst case is 8 internal copy-operatios if linked queue is used, s. NvM_Prv_QueueLinked_Dequeue)
 *
 * \param[in] idQueue_uo
 * Id of the queue to dequeue passed user request (caller has to ensure the value range)
 * \param[in] idxRequest_pcst
 * Index of the element in the queue buffer to dequeue
 */
void NvM_Prv_RequestQueue_Dequeue(NvM_Prv_idQueue_tuo idQueue_uo, NvM_Prv_Queue_idx_tst const* idxRequest_pcst)
{
    NvM_Prv_RequestQueue_Fcts_st.Dequeue_pfct(NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst, idxRequest_pcst);
}

/**
 * \brief
 * This function determines which request will be processed as next by the NvM.
 * \details
 * Normally this function goes through all available queues due to the their priority in descending order.
 * And if one queue has an entry to be processed then the pointer to this entry will be returned.
 * Only user requests which requires the job resource identified by the passed ID can be processed.
 * Additionally this function returns also the user request index in the queue buffer so that NvM can dequeue it
 * at the beginning of the processing.
 *
 * If multi-block request is active then this function checks only if immediate queue has an entry.
 * And if immediate queue has no entries or is not available then this function returns the pointer
 * to the multi-block queue.
 *
 * \param[in] idJobResource_uo
 * Job resource ID which user request shall require to be processed as next one
 * \param[out] QueueEntry_ppst
 * Pointer to the pointer to the queue entry to return the parameter of the next executed request.
 * \parame[inout] idxRequest_pst
 * Pointer to the user request index in the queue buffer
 *
 * \return
 * ID of the queue from which the next request to be processed has been taken.
 *
 * Note: this function is returning an AooB queue index only if *QueueEntry_ppst is NULL_PTR.
 * The caller has to avoid using the returned value if the point is NULL_PTR and vice-versa.
 * It is sufficient to check only one information.
 *
 */
NvM_Prv_idQueue_tuo NvM_Prv_RequestQueue_GetEntryToProcess(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                                           NvM_Prv_QueueEntry_tst** QueueEntry_ppst,
                                                           NvM_Prv_Queue_idx_tst* idxRequest_pst)
{
    NvM_Prv_idQueue_tuo idQueue_uo = NvM_Prv_idQueue_nrQueues_e;
    idxRequest_pst->idx_uo = NVM_PRV_QUEUE_IDX_INVALID;

    do
    {
        --idQueue_uo;

        if (NvM_Prv_Multi_IsInProgress() && (NvM_Prv_idQueue_Standard_e == idQueue_uo))
        {
            idQueue_uo = NvM_Prv_idQueue_Multi_e;
        }

        if (!NvM_Prv_RequestQueue_Fcts_st.IsEmpty_pfct(NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst))
        {
            if (NvM_Prv_idQueue_Multi_e != idQueue_uo)
            {
                NvM_Prv_RequestQueue_GetRequest(idJobResource_uo, idQueue_uo, idxRequest_pst);
            }
            else
            {
                idxRequest_pst->idx_uo = 0u;
                idxRequest_pst->idxPrev_uo = NVM_PRV_QUEUE_IDX_INVALID;
            }
        }

    } while ((NVM_PRV_QUEUE_IDX_INVALID == idxRequest_pst->idx_uo) && (idQueue_uo > 0u));

    /* Was there no queue entry found? */
    if (NVM_PRV_QUEUE_IDX_INVALID == idxRequest_pst->idx_uo)
    {
        /* This is returning a tuple which indicates that no queue entry is to be processed.
         * Both - pointer and ID - are invalid values */
        *QueueEntry_ppst = NULL_PTR;
        idQueue_uo = NvM_Prv_idQueue_nrQueues_e;
    }
    else
    {
        /* idQueue_uo is valid and indicates the found queue index */
        *QueueEntry_ppst = &NvM_Prv_RequestQueues_cast[idQueue_uo].data_cpo[idxRequest_pst->idx_uo];
    }

    return idQueue_uo;
}

/**
 * \brief
 * This local private function searches in the given queue for a user request which requires given job resource and
 * returns the index in the queue buffer to the user request if found.
 * \details
 * If no user request which requires given job resource is available in the queue then this function returns invalid
 * value (s. NVM_PRV_QUEUE_IDX_INVALID).
 *
 * \param[in] idJobResource_uo
 * ID of the job resource which shall be required by the user request to be processed
 * \param[in] idQueue_uo
 * ID of the queue where to search for user request to be processed
 * \param[inout] idxRequest_pst
 * Index in the queue buffer which identifies user request which requires given job resource
 */
void NvM_Prv_RequestQueue_GetRequest(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                     NvM_Prv_idQueue_tuo idQueue_uo,
                                     NvM_Prv_Queue_idx_tst* idxRequest_pst)
{
    NvM_BlockIdType idBlock_uo;
    boolean isRequestProcessable_b;
    boolean isNextRequestAvailable_b = TRUE;
    NvM_Prv_idQueue_tuo idQueueOriginal_uo;

    /* MR12 DIR 4.1, RULE 1.3, 18.1 VIOLATION: The caller of this function makes sure that all pointers are valid */
    idxRequest_pst->idx_uo = NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst->idxHead_uo;
    idxRequest_pst->idxPrev_uo = NVM_PRV_QUEUE_IDX_INVALID;
    /* MR12 DIR 4.1, RULE 1.3, 18.1 VIOLATION: The caller of this function makes sure that all pointers are valid */
    idBlock_uo = NvM_Prv_RequestQueues_cast[idQueue_uo].data_cpo[idxRequest_pst->idx_uo].idBlock_uo;
    idQueueOriginal_uo = NvM_Prv_RequestQueues_cast[idQueue_uo].data_cpo[idxRequest_pst->idx_uo].idQueueOriginal_uo;
    isRequestProcessable_b = NvM_Prv_RequestQueue_IsRequestProcessable(idJobResource_uo,
                                                                       idQueueOriginal_uo,
                                                                       idBlock_uo);

    while (isNextRequestAvailable_b && !isRequestProcessable_b)
    {
        idxRequest_pst->idxPrev_uo = idxRequest_pst->idx_uo;
        /* MR12 DIR 4.1, RULE 1.3, 18.1 VIOLATION: The caller of this function makes sure that all pointers are valid */
        NvM_Prv_RequestQueue_Fcts_st.GetNext_pfct(NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst,
                                                  &idxRequest_pst->idx_uo);

        /* MR12 DIR 4.1, RULE 1.3, 18.1 VIOLATION: The caller of this function makes sure that all pointers are valid */
        if (idxRequest_pst->idx_uo < NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst->size_cuo)
        {
            /* MR12 DIR 4.1, RULE 1.3, 18.1 VIOLATION: The caller of this function makes sure that all pointers are valid */
            idBlock_uo = NvM_Prv_RequestQueues_cast[idQueue_uo].data_cpo[idxRequest_pst->idx_uo].idBlock_uo;
            idQueueOriginal_uo = NvM_Prv_RequestQueues_cast[idQueue_uo].data_cpo[idxRequest_pst->idx_uo].idQueueOriginal_uo;
            isRequestProcessable_b = NvM_Prv_RequestQueue_IsRequestProcessable(idJobResource_uo,
                                                                               idQueueOriginal_uo,
                                                                               idBlock_uo);
        }
        else
        {
            isNextRequestAvailable_b = FALSE;
        }
    };
}

/**
 * \brief
 * This local private function determines whether the request for the given block can be processed.
 * \details
 * To determine when a user request can be processed NvM has to consider following two scenarios:
 * 1. High-speed-mode is NOT available due to configuration OR currently disabled,
 *    a user request can be processed if following condition is fullfilled:
 *    - given block requires passed job resource to process a user request
 *
 * 2. High-speed-mode is available due to configuration AND currently enabled:
 *    a user request can be processed if following conditions are fullfilled:
 *    - given block requires passed job resource to process a user request
 *    -    (given block is processable in high-speed-mode AND user request is from immediate queue)
 *      OR (given block is NOT processable in high-speed-mode)
 *
 * \param[in] idJobResource_uo
 * ID of the job resource which shall be required by the user request to be processed
 * \param[in] idQueueOriginal_uo
 * ID of the queue where the request was enqueued originally (before suspension)
 * \param[in] idBlock_uo
 * ID of the block which would be processed if possible
 */
boolean NvM_Prv_RequestQueue_IsRequestProcessable(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                                  NvM_Prv_idQueue_tuo idQueueOriginal_uo,
                                                  NvM_BlockIdType idBlock_uo)
{
    boolean isRequestProcessable_b = NvM_Prv_JobResource_IsRequired(idJobResource_uo, idBlock_uo);

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
    if (NvM_Prv_IsHighSpeedModeActive())
    {
        if (NvM_Prv_BlkDesc_IsHighSpeedModeProcessable(idBlock_uo))
        {
            isRequestProcessable_b = isRequestProcessable_b && (NvM_Prv_idQueue_Immediate_e == idQueueOriginal_uo);
        }
    }
#else
    (void)idQueueOriginal_uo;
#endif

    return isRequestProcessable_b;
}

/**
 * \brief
 * This private function checks whether a given multi-block request is currently enqueued.
 *
 * \param[in] idService_uo
 * ID of the multi-block request to checked
 *
 * \return
 * - true = given multi-block request is currently enqueued
 * - false = given multi-block request is currently NOT enqueued
 */
boolean NvM_Prv_RequestQueue_IsMultiEnqueued(NvM_Prv_idService_tuo idService_uo)
{
    boolean IsEnqueuedMulti_b = FALSE;
    if (!NvM_Prv_RequestQueue_Fcts_st.IsEmpty_pfct(NvM_Prv_RequestQueues_cast[NvM_Prv_idQueue_Multi_e].Queue_cpst))
    {
        NvM_Prv_Queue_idx_tuo idxHead_uo = NvM_Prv_RequestQueues_cast[NvM_Prv_idQueue_Multi_e].Queue_cpst->idxHead_uo;
        NvM_Prv_idService_tuo idServiceEnqueued_uo =
                NvM_Prv_RequestQueues_cast[NvM_Prv_idQueue_Multi_e].data_cpo[idxHead_uo].idService_uo;
        if (idService_uo == idServiceEnqueued_uo)
        {
            IsEnqueuedMulti_b = TRUE;
        }
    }

    return IsEnqueuedMulti_b;
}

/**
 * \brief
 * This private function returns whether the given request queue is empty.
 *
 * \param[in] idQueue_uo
 * ID of the queue to be checked
 *
 * \return
 * - true = given queue is empty
 * - false = given queue has at least one element enqueued
 */
boolean NvM_Prv_RequestQueue_IsEmpty(NvM_Prv_idQueue_tuo idQueue_uo)
{
    return NvM_Prv_RequestQueue_Fcts_st.IsEmpty_pfct(NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst);
}

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

NvM_Prv_Queue_idx_tuo NvM_Prv_RequestQueue_GetFreeEntries(NvM_Prv_idQueue_tuo idQueue_uo)
{
    return NvM_Prv_RequestQueue_Fcts_st.GetNrFreeEntries_pfct(NvM_Prv_RequestQueues_cast[idQueue_uo].Queue_cpst);
}

#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Inline code
 **********************************************************************************************************************
 */

