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
#ifndef NVM_PRV_QUEUELINKED_TYPES_H
#define NVM_PRV_QUEUELINKED_TYPES_H
/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/**
 * \brief
 * Definition of the structure which defines a linked queue implemented by a circular buffer.
 * \details
 * This structure contains only administrative data required to handle a linked queue.
 * The user of a queue has to provide a buffer to store the elements.
 */
typedef struct
{
    /// Maximum number of entries the queue can maintain.
    NvM_Prv_Queue_idx_tuo const     size_cuo;
    /// Counter of free entries currently available.
    NvM_Prv_Queue_idx_tuo           cntrFreeEntries_uo;
    /// Index of the first element (head) in the queue.
    /// The head of the queue is the oldest element so a search for an element to be processed starts always with it.
    NvM_Prv_Queue_idx_tuo           idxHead_uo;
    /// Constant pointer to the array with volatile flags for each index in the queue showing
    /// whether an element has been inserted at this index.
    /// \attention
    /// It is essential that insertion flags are volatile. These flags belongs to critical data and will be set/reset
    /// without a scheduler lock to reduce locking time. Making these flags volatile makes sure tat compiler does not
    /// optimize access to them.
    boolean volatile* const         isInserted_cpvb;
    /// Index to the last element (tail) in the queue.
    /// The tail of the queue is the last (newest) element so next element is the first free element in the queue.
    NvM_Prv_Queue_idx_tuo           idxTail_uo;
    /// Index to the last element (end) in the queue.
    /// The end of the queue is the first free element so a new element will be enqueued at this position.
    NvM_Prv_Queue_idx_tuo           idxHeadFree_uo;
    /// Constant pointer to the array with indexes of next element for each index.
    /// If for an element identified by an index there is no next element in the queue
    /// then the index of next element is set to invalid value NVM_PRV_QUEUE_IDX_INVALID.
    NvM_Prv_Queue_idx_tuo* const    idxNext_cpuo;
    /// Constant pointer to the array with indexes of next free element for each index.
    /// If for an element identified by an index there is no next free element in the queue
    /// then the index of previous element is set to invalid value NVM_PRV_QUEUE_IDX_INVALID.
    NvM_Prv_Queue_idx_tuo* const    idxNextFree_cpuo;

} NvM_Prv_QueueLinked_tst;

/* NVM_PRV_QUEUELINKED_TYPES_H */
#endif

