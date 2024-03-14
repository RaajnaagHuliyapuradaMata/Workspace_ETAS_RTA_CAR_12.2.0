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
#ifndef NVM_PRV_JOBQUEUE_H
#define NVM_PRV_JOBQUEUE_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_JobQueue_Types.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern void NvM_Prv_JobQueue_Initialize(void);

extern NvM_Prv_Queue_idx_tuo NvM_Prv_JobQueue_Enqueue(void);

extern void NvM_Prv_JobQueue_Insert(NvM_Prv_Job_tst const* Job_pcst, NvM_Prv_Queue_idx_tuo idxPosition_uo);

extern void NvM_Prv_JobQueue_Dequeue(NvM_Prv_Queue_idx_tst const* idxJob_pcst);

extern boolean NvM_Prv_JobQueue_IsFull(void);

extern boolean NvM_Prv_JobQueue_IsEmpty(void);

extern NvM_Prv_Queue_idx_tuo NvM_Prv_JobQueue_GetNrEntries(void);

extern NvM_Prv_Queue_idx_tuo NvM_Prv_JobQueue_GetIdxJob(uint8 nrJob_uo);

extern NvM_Prv_Job_tst* NvM_Prv_JobQueue_GetJob(uint8 nrJob_uo);

#if (NVM_JOB_PRIORITIZATION == STD_ON)
boolean NvM_Prv_JobQueue_SuspendJob(NvM_Prv_Queue_idx_tst const* idxJob_pcst);
#endif

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
extern uint8* NvM_Prv_JobQueue_GetUsedIntBfr(NvM_BlockIdType idBlock_uo, NvM_Prv_idService_tuo idService_uo);
extern NvM_Prv_Job_tst* NvM_Prv_JobQueue_GetJobData(NvM_BlockIdType idBlock_uo,
                                                    NvM_Prv_idService_tuo idService_uo);
#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_JOBQUEUE_H */
#endif

