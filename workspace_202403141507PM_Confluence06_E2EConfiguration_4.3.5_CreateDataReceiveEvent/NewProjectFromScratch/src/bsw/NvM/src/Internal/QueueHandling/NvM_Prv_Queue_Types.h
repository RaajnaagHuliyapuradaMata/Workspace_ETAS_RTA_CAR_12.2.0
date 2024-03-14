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
#ifndef NVM_PRV_QUEUE_TYPES_H
#define NVM_PRV_QUEUE_TYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/**
 * \brief
 * Definition of the structure which defines a FIFO queue implemented by a circular buffer.
 * \details
 * This structure contains only administrative data required for a circular buffer.
 * The user of a queue has to provide a buffer to store the elements.
 */
typedef struct
{
    /// Maximum number of entries the queue can maintain.
    NvM_Prv_Queue_idx_tuo const size_cuo;
    /// Counter of free entries currently available.
    NvM_Prv_Queue_idx_tuo       cntrFreeEntries_uo;
    /// Index of the first element (head) in the queue.
    /// The head of the queue is the oldest element so it will be dequeued as first.
    NvM_Prv_Queue_idx_tuo       idxHead_uo;
    /// Constant pointer to the array with volatile flags for each index in the queue showing
    /// whether an element has been inserted at this index.
    /// \attention
    /// It is essential that insertion flags are volatile. These flags belongs to critical data and will be set/reset
    /// without a scheduler lock to reduce locking time. Making these flags volatile makes sure tat compiler does not
    /// optimize access to them.
    boolean volatile* const     isInserted_cpvb;

} NvM_Prv_Queue_tst;

/* NVM_PRV_QUEUE_TYPES_H */
#endif

