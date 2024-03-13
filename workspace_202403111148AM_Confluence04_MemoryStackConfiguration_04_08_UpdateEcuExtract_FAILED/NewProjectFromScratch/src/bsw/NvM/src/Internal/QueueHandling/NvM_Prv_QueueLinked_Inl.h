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
#ifndef NVM_PRV_QUEUELINKED_INL_H
#define NVM_PRV_QUEUELINKED_INL_H

/*
**********************************************************************************************************************
* Inline functions declarations
**********************************************************************************************************************
*/
LOCAL_INLINE boolean NvM_Prv_QueueLinked_IsEmpty(NvM_Prv_QueueLinked_tst const* Queue_pcst);
LOCAL_INLINE boolean NvM_Prv_QueueLinked_IsFull(NvM_Prv_QueueLinked_tst const* Queue_pcst);
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_QueueLinked_GetNrFreeEntries(NvM_Prv_QueueLinked_tst const* Queue_pcst);
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_QueueLinked_GetNrEntries(NvM_Prv_QueueLinked_tst const* Queue_pcst);
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_QueueLinked_Enqueue(NvM_Prv_QueueLinked_tst* Queue_pst);
LOCAL_INLINE boolean NvM_Prv_QueueLinked_IsInserted(NvM_Prv_QueueLinked_tst const* Queue_pcst,
                                                    NvM_Prv_Queue_idx_tuo idxPosition_uo);
LOCAL_INLINE void NvM_Prv_QueueLinked_Dequeue(NvM_Prv_QueueLinked_tst* Queue_pst,
                                              NvM_Prv_Queue_idx_tst const* idx_pcst);

/*
**********************************************************************************************************************
* Inline functions
**********************************************************************************************************************
*/
/**
 * \brief
 * This function returns whether the given queue is empty.
 *
 * \param[in] Queue_pcst
 * Pointer to the queue to be checked
 *
 * \return
 * - true = given queue is empty
 * - false = given queue has at least one element enqueued
 */
LOCAL_INLINE boolean NvM_Prv_QueueLinked_IsEmpty(NvM_Prv_QueueLinked_tst const* Queue_pcst)
{
    return (Queue_pcst->size_cuo == Queue_pcst->cntrFreeEntries_uo);
}

/**
 * \brief
 * This function returns whether the given queue is full.
 *
 * \details
 * In cases this function is called with enabled scheduler lock the run-time of 1 can be used:
 * - 1 for internal copy-operation
 *
 * \param[in] Queue_pcst
 * Pointer to the queue to be checked
 *
 * \return
 * - true = given queue is full
 * - false = given queue is not full, i.e. at least one element can be equeued
 */
LOCAL_INLINE boolean NvM_Prv_QueueLinked_IsFull(NvM_Prv_QueueLinked_tst const* Queue_pcst)
{
    return (0u == Queue_pcst->cntrFreeEntries_uo);
}

/**
 * \brief
 * This function returns number of free entries in the given queue.
 *
 * \param[in] Queue_pcst
 * Pointer to the queue for which number of free entries will be returned
 *
 * \return
 * Number of free entries
 */
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_QueueLinked_GetNrFreeEntries(NvM_Prv_QueueLinked_tst const* Queue_pcst)
{
    return Queue_pcst->cntrFreeEntries_uo;
}

/**
 * \brief
 * This function returns number of entries in the given queue.
 *
 * \param[in] Queue_pcst
 * Pointer to the queue for which number of entries will be returned
 *
 * \return
 * Number of entries
 */
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_QueueLinked_GetNrEntries(NvM_Prv_QueueLinked_tst const* Queue_pcst)
{
    return (Queue_pcst->size_cuo - Queue_pcst->cntrFreeEntries_uo);
}

/**
 * \brief
 * This function enqueues an element into the given queue.
 *
 * \details
 * In cases this function is called with enabled scheduler lock the run-time of 6 can be used:
 * - 6 for internal copy-operations
 *
 * \attention
 * This function reserves only an entry in the given queue for an element and returns the index of reserved position.
 * The user is responsible to insert the element at reserved position. The NvM splits the actual enqueuing in 2 steps,
 * reservation and insertion, to minimize the time for the scheduler lock. Only reservation must be done with enabled
 * scheduler lock. The insertion can be done afterwards without a lock.
 *
 * \param[inout] Queue_pst
 * Pointer to the queue to enqueue an element
 *
 * \return
 * Index in the queue buffer where the user can insert an element
 */
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_QueueLinked_Enqueue(NvM_Prv_QueueLinked_tst* Queue_pst)
{
    Queue_pst->idxTail_uo = Queue_pst->idxHeadFree_uo;
    Queue_pst->idxHeadFree_uo = Queue_pst->idxNextFree_cpuo[Queue_pst->idxTail_uo];
    Queue_pst->idxNextFree_cpuo[Queue_pst->idxTail_uo] = NVM_PRV_QUEUE_IDX_INVALID;
    Queue_pst->idxNext_cpuo[Queue_pst->idxTail_uo] = Queue_pst->idxHeadFree_uo;

    --Queue_pst->cntrFreeEntries_uo;

    return Queue_pst->idxTail_uo;
}

/**
 * \brief
 * This function checks whether the element at the given position has been inserted.
 * \details
 * \attention
 * Before using an element from the queue the user has to ensure that the element has been inserted.
 *
 * \param[in] Queue_pcst
 * Pointer to the queue to check an element for insertion
 * \param[in] idxPosition_uo
 * Index which defines the position in the queue to be checked
 *
 * \return
 * - TRUE = element has been inserted at the given position and is available for further processing
 * - FALSE = element has not been inserted yet and is NOT available for further processing
 */
LOCAL_INLINE boolean NvM_Prv_QueueLinked_IsInserted(NvM_Prv_QueueLinked_tst const* Queue_pcst,
                                                    NvM_Prv_Queue_idx_tuo idxPosition_uo)
{
    return Queue_pcst->isInserted_cpvb[idxPosition_uo];
}

/**
 * \brief
 * This function dequeues an element at the given position from the given queue.
 * \details
 * This function only updates queue administrative data. The enqueued element remains in the queue and
 * will be automatically overwritten at further enqueuing actions.
 *
 * In cases this function is called with enabled scheduler lock the run-time of 8 can be used:
 * - 8 for internal copy-operations in worst case if element is dequeued in the middle of the queue
 *
 * \param[inout] Queue_pst
 * Pointer to the queue to dequeue an element
 * \param[in] idx_pcst
 * Structure with indexes defining position of the element to dequeue
 */
LOCAL_INLINE void NvM_Prv_QueueLinked_Dequeue(NvM_Prv_QueueLinked_tst* Queue_pst,
                                              NvM_Prv_Queue_idx_tst const* idx_pcst)
{
    if (idx_pcst->idx_uo == Queue_pst->idxTail_uo)
    {
        // element to dequeue is the last element in the queue

        // update head-free + corresponding Next and NextFree elements
        Queue_pst->idxNext_cpuo[idx_pcst->idx_uo] = NVM_PRV_QUEUE_IDX_INVALID;
        Queue_pst->idxNextFree_cpuo[idx_pcst->idx_uo] = Queue_pst->idxHeadFree_uo;
        // update first free element
        Queue_pst->idxHeadFree_uo = idx_pcst->idx_uo;
        // update tail only if element to be dequeued has a preceding element,
        // i.e. the queue has more than 1 element enqueued
        if (idx_pcst->idxPrev_uo < NVM_PRV_QUEUE_IDX_INVALID)
        {
            Queue_pst->idxTail_uo = idx_pcst->idxPrev_uo;
        }
    }
    else
    {
        if (idx_pcst->idx_uo == Queue_pst->idxHead_uo)
        {
            // element to dequeue is the first element in the queue

            // update head + corresponding Next and NextFree element
            Queue_pst->idxHead_uo = Queue_pst->idxNext_cpuo[idx_pcst->idx_uo];
            Queue_pst->idxNext_cpuo[idx_pcst->idx_uo] = NVM_PRV_QUEUE_IDX_INVALID;
            Queue_pst->idxNextFree_cpuo[idx_pcst->idx_uo] = Queue_pst->idxHead_uo;
        }
        else
        {
            // this part will be only executed if queue has enqueued at least 3 elements,
            // otherwise element to be dequeued is either first or last element in the queue,
            // that means also that element to be dequeued has a preceding element

            // update next element of the preceding element
            Queue_pst->idxNext_cpuo[idx_pcst->idxPrev_uo] = Queue_pst->idxNext_cpuo[idx_pcst->idx_uo];
            // update Next and NextFree element for the element to be dequeued
            Queue_pst->idxNext_cpuo[idx_pcst->idx_uo] = NVM_PRV_QUEUE_IDX_INVALID;
            Queue_pst->idxNextFree_cpuo[idx_pcst->idx_uo] = Queue_pst->idxHeadFree_uo;
            // update next element of the the last element
            Queue_pst->idxNext_cpuo[Queue_pst->idxTail_uo] = idx_pcst->idx_uo;
            // update first free element
            Queue_pst->idxHeadFree_uo = idx_pcst->idx_uo;
        }
    }

    Queue_pst->isInserted_cpvb[idx_pcst->idx_uo] = FALSE;
    ++Queue_pst->cntrFreeEntries_uo;
}

/* NVM_PRV_QUEUELINKED_INL_H */
#endif
