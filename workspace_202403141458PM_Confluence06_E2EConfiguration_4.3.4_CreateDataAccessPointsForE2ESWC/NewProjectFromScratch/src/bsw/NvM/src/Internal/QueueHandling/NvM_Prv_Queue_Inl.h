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
#ifndef NVM_PRV_QUEUE_INL_H
#define NVM_PRV_QUEUE_INL_H

/*
**********************************************************************************************************************
* Inline functions declarations
**********************************************************************************************************************
*/
LOCAL_INLINE boolean NvM_Prv_Queue_IsEmpty(NvM_Prv_Queue_tst const* Queue_pcst);
LOCAL_INLINE boolean NvM_Prv_Queue_IsFull(NvM_Prv_Queue_tst const* Queue_pcst);
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_Queue_GetNrFreeEntries(NvM_Prv_Queue_tst const* Queue_pcst);
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_Queue_GetNrEntries(NvM_Prv_Queue_tst const* Queue_pcst);
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_Queue_Enqueue(NvM_Prv_Queue_tst* Queue_pst);
LOCAL_INLINE void NvM_Prv_Queue_Dequeue(NvM_Prv_Queue_tst* Queue_pst, NvM_Prv_Queue_idx_tst const* idx_pcst);

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
LOCAL_INLINE boolean NvM_Prv_Queue_IsEmpty(NvM_Prv_Queue_tst const* Queue_pcst)
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
LOCAL_INLINE boolean NvM_Prv_Queue_IsFull(NvM_Prv_Queue_tst const* Queue_pcst)
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
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_Queue_GetNrFreeEntries(NvM_Prv_Queue_tst const* Queue_pcst)
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
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_Queue_GetNrEntries(NvM_Prv_Queue_tst const* Queue_pcst)
{
    return (Queue_pcst->size_cuo - Queue_pcst->cntrFreeEntries_uo);
}

/**
 * \brief
 * This function enqueues an element into the given queue.
 *
 * \details
 * In cases this function is called with enabled scheduler lock the run-time of 4 can be used:
 * - 4 for internal copy-operations
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
 * Index in the queue buffer(array) where the user can insert an element
 */
LOCAL_INLINE NvM_Prv_Queue_idx_tuo NvM_Prv_Queue_Enqueue(NvM_Prv_Queue_tst* Queue_pst)
{
    NvM_Prv_Queue_idx_tuo cntrEntries_uo = Queue_pst->size_cuo - Queue_pst->cntrFreeEntries_uo;
    NvM_Prv_Queue_idx_tuo idxEnd_uo = (NvM_Prv_Queue_idx_tuo)
                                      (((uint32)Queue_pst->idxHead_uo + cntrEntries_uo) % Queue_pst->size_cuo);

    --Queue_pst->cntrFreeEntries_uo;

    return idxEnd_uo;
}

/**
 * \brief
 * This function checks whether the element at the given position has been inserted.
 *
 * \param[in] Queue_pst
 * Pointer to the queue to dequeue an element
 * \param[in] idxPosition_uo
 * Index which defines the position in the queue to be checked
 *
 * \return
 * - TRUE = element has been inserted at the given position and can be dequeued safely
 * - FALSE = element has not been inserted yet and cannot be dequeued
 */
LOCAL_INLINE boolean NvM_Prv_Queue_IsInserted(NvM_Prv_Queue_tst const* Queue_pcst,
                                              NvM_Prv_Queue_idx_tuo idxPosition_uo)
{
    return Queue_pcst->isInserted_cpvb[idxPosition_uo];
}

/**
 * \brief
 * This function dequeues first element (head) from the given queue.
 * \details
 * This function only updates queue administrative data. The enqueued element remains in the queue and
 * will be automatically overwritten at further enqueuing actions.
 * Since this function always dequeues the first element the second parameter idx_pcst has no meaning is not used.
 *
 * In cases this function is called with enabled scheduler lock the run-time of 4 can be used:
 * - 4 for internal copy-operations
 *
 * \param[inout] Queue_pst
 * Pointer to the queue to dequeue an element
 * \param[in] idx_pcst
 * Structure with indexes defining position of the element to dequeue (not used)
 */
LOCAL_INLINE void NvM_Prv_Queue_Dequeue(NvM_Prv_Queue_tst* Queue_pst,
                                        NvM_Prv_Queue_idx_tst const* idx_pcst)
{
    (void)idx_pcst;
    Queue_pst->isInserted_cpvb[Queue_pst->idxHead_uo] = FALSE;
    Queue_pst->idxHead_uo = (Queue_pst->idxHead_uo + 1u) % Queue_pst->size_cuo;
    ++Queue_pst->cntrFreeEntries_uo;
}

/* NVM_PRV_QUEUE_INL_H */
#endif
