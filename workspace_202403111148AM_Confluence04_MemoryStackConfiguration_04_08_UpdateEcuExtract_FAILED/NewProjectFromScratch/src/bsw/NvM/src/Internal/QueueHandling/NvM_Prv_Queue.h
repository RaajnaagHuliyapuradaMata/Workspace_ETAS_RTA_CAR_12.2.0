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
#ifndef NVM_PRV_QUEUE_H
#define NVM_PRV_QUEUE_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_Queue_Types.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern void NvM_Prv_Queue_Initialize(NvM_Prv_Queue_tst* Queue_pst);

extern void NvM_Prv_Queue_Insert(NvM_Prv_Queue_tst const* Queue_pcst,
                                 NvM_Prv_Queue_idx_tuo idx_uo,
                                 uint8* QueueBuffer_pu8,
                                 uint8 const* ElementToEnqueue_pcu8,
                                 uint32 ElementSize_u32);

extern void NvM_Prv_Queue_GetNextElement(NvM_Prv_Queue_tst const* Queue_pcst,
                                         NvM_Prv_Queue_idx_tuo* idx_puo);

extern NvM_Prv_Queue_idx_tuo NvM_Prv_Queue_GetIdxElement(NvM_Prv_Queue_tst const* Queue_pcst,
                                                         uint8 nrElement_uo);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Inline functions
 **********************************************************************************************************************
 */
/* Include the actual definitions of the provided inline functions */
#include "NvM_Prv_Queue_Inl.h"

/* NVM_PRV_QUEUE_H */
#endif

