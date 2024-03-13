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
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM.h"

#include "rba_MemLib.h"

#include "NvM_Prv_QueueLinked.h"

 /*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * This function initializes given queue.
 * \details
 * Initialized queue is always empty.
 *
 * \param[in] Queue_pst
 * Pointer to the queue to initialize
 */
void NvM_Prv_QueueLinked_Initialize(NvM_Prv_QueueLinked_tst* Queue_pst)
{
    NvM_Prv_Queue_idx_tuo cntrElement_uo;
    NvM_Prv_Queue_idx_tuo idxLastElement_uo = 1u;

    Queue_pst->idxHead_uo = 0u;
    Queue_pst->idxTail_uo = 0u;
    Queue_pst->idxHeadFree_uo = 0u;
    Queue_pst->cntrFreeEntries_uo = Queue_pst->size_cuo;

    for (cntrElement_uo = 0u; cntrElement_uo < Queue_pst->size_cuo; ++cntrElement_uo)
    {
        Queue_pst->isInserted_cpvb[cntrElement_uo] = FALSE;
        Queue_pst->idxNext_cpuo[cntrElement_uo] = NVM_PRV_QUEUE_IDX_INVALID;
        Queue_pst->idxNextFree_cpuo[cntrElement_uo] = (idxLastElement_uo + cntrElement_uo) % Queue_pst->size_cuo;
    }
}

/**
 * \brief
 * This function inserts an element into the queue buffer at given position.
 *
 * \attention
 * The insertion flag, which indicates that the insertion has been completed, will be set via special function
 * to make sure that the copy operation is completely done before insertion flag will be set.
 *
 * \param[in] Queue_pst
 * Pointer to the queue to insert an element
 * \param[in] idx_uo
 * Index which defines the position to insert an element
 * \param[inout] QueueBuffer_pu8
 * Pointer to the queue buffer to insert an element
 * \param[in] ElementToEnqueue_pcu8
 * Constant pointer to the element to insert
 * \param[in] ElementSize_u32
 * Size of the element to insert in bytes
 */
void NvM_Prv_QueueLinked_Insert(NvM_Prv_QueueLinked_tst const* Queue_pcst,
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
 * This function returns element next to given element in the given queue which can be processed.
 * \details
 * If next element is required for the last element in the queue then there is no next element in the queue and
 * this function returns invalid index.
 *
 * \param[in] Queue_pcst
 * Pointer to the queue to get next element
 * \param[inout] idx_puo
 * Index of the element for which next element is required
 */
void NvM_Prv_QueueLinked_GetNextElement(NvM_Prv_QueueLinked_tst const* Queue_pcst,
                                        NvM_Prv_Queue_idx_tuo* idx_puo)
{
    if (*idx_puo == Queue_pcst->idxTail_uo)
    {
        *idx_puo = NVM_PRV_QUEUE_IDX_INVALID;
    }
    else
    {
        *idx_puo = Queue_pcst->idxNext_cpuo[*idx_puo];
    }
}

/**
 * \brief
 * This private function returns the index of the element in the queue buffer defined by the order number.
 * \details
 * \attention
 * Do not confuse the order number and the index.
 *
 * The order number means enqueuing order in the queue. First (oldest) enqueued element has number 0,
 * second enqueud element has number 1, third enqueued element has number 2, etc.
 * The index means the index in the queue buffer where queue elements will be stored.
 *
 * \param Queue_pcst
 * Pointer to the queue to get index of an element
 * \param nrElement_uo
 * Order number of the element to get index
 *
 * \return
 * Index of the element in the queue buffer
 */
NvM_Prv_Queue_idx_tuo NvM_Prv_QueueLinked_GetIdxElement(NvM_Prv_QueueLinked_tst const* Queue_pcst,
                                                        uint8 nrElement_uo)
{
    NvM_Prv_Queue_idx_tuo cntrElement_uo;
    NvM_Prv_Queue_idx_tuo idxElement_uo = Queue_pcst->idxHead_uo;
    for (cntrElement_uo = 0u; cntrElement_uo < nrElement_uo; ++cntrElement_uo)
    {
        idxElement_uo = Queue_pcst->idxNext_cpuo[idxElement_uo];
    }

    return idxElement_uo;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

