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

#include "NvM_Prv_Queue.h"

 /*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * This private function initializes given queue.
 * \details
 * Initialized queue is always empty.
 *
 * \param[in] Queue_pst
 * Pointer to the queue to initialize
 */
void NvM_Prv_Queue_Initialize(NvM_Prv_Queue_tst* Queue_pst)
{
    NvM_Prv_Queue_idx_tuo cntrEntries_uo;

    Queue_pst->idxHead_uo = 0u;
    Queue_pst->cntrFreeEntries_uo = Queue_pst->size_cuo;

    for (cntrEntries_uo = 0u; cntrEntries_uo < Queue_pst->size_cuo; ++cntrEntries_uo)
    {
        Queue_pst->isInserted_cpvb[cntrEntries_uo] = FALSE;
    }
}

/**
 * \brief
 * This private function inserts an element into the queue buffer at given index.
 *
 * \attention
 * The insertion flag, which indicates that the insertion has been completed, will be set via special function
 * to make sure that the copy operation is completely done before insertion flag will be set.
 *
 * \param[in] Queue_pst
 * Constant pointer to the queue to insert an element
 * \param[in] idx_uo
 * Index which defines the position to insert an element
 * \param[inout] QueueBuffer_pu8
 * Pointer to the queue buffer to insert an element
 * \param[in] ElementToEnqueue_pcu8
 * Constant pointer to the element to insert
 * \param[in] ElementSize_u32
 * Size of the element to insert in bytes
 */
void NvM_Prv_Queue_Insert(NvM_Prv_Queue_tst const* Queue_pcst,
                          NvM_Prv_Queue_idx_tuo idx_uo,
                          uint8* QueueBuffer_pu8,
                          uint8 const* ElementToEnqueue_pcu8,
                          uint32 ElementSize_u32)
{
    rba_MemLib_MemCopy(ElementToEnqueue_pcu8,
                       &QueueBuffer_pu8[ElementSize_u32 * idx_uo],
                       ElementSize_u32);

    // Special sync write function to make sure that the copy operation before has been completed
    rba_MemLib_SyncWriteBool(&(Queue_pcst->isInserted_cpvb[idx_uo]), TRUE);
}

/**
 * \brief
 * This private function returns the element next to the given element in the given queue which can be processed.
 * \details
 * For FIFO queues implemented as circular buffer it is not allowed to dequeue an element before the head element
 * has been processed. Therefore this function returns always an invalid index.
 *
 * \param[in] Queue_pcst
 * Constant pointer to the queue to get next element
 * \param[inout] idx_puo
 * Index of the element for which next element is required
 */
void NvM_Prv_Queue_GetNextElement(NvM_Prv_Queue_tst const* Queue_pcst,
                                  NvM_Prv_Queue_idx_tuo* idx_puo)
{
    *idx_puo = NVM_PRV_QUEUE_IDX_INVALID;
    (void)Queue_pcst;
}

/**
 * \brief
 * This private function returns the index of the element enqueued in given order.
 * \details
 * \attention
 * Do not confuse the order number and the index.
 *
 * The order number means enqueuing order in the queue. First (oldest) enqueued element has number 0,
 * second enqueud element has number 1, third enqueued element has number 2, etc.
 * The index means the index in the array where queue elements will be stored.
 *
 * \param[in] Queue_pcst
 * Constant pointer to the queue to get index
 * \param[in] nrElement_uo
 * Enqueuing order number for which the array index is required
 *
 * \return
 * Index in the array where required queue element is stored
 */
NvM_Prv_Queue_idx_tuo NvM_Prv_Queue_GetIdxElement(NvM_Prv_Queue_tst const* Queue_pcst,
                                                  uint8 nrElement_uo)
{
    return (NvM_Prv_Queue_idx_tuo)(((uint32)Queue_pcst->idxHead_uo + nrElement_uo) % Queue_pcst->size_cuo);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

