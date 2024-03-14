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
#ifndef NVM_PRV_PROCESSMULTIBLOCK_H
#define NVM_PRV_PROCESSMULTIBLOCK_H

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
typedef boolean (*NvM_Prv_Multi_Cancel_tpfct)(void);

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern void NvM_Prv_Multi_Initialize(void);
extern boolean NvM_Prv_Multi_IsInProgress(void);
extern void NvM_Prv_Multi_SetResult(NvM_RequestResultType Result_uo);
extern NvM_RequestResultType NvM_Prv_Multi_GetResult(void);
extern void NvM_Prv_Multi_Cancel(NvM_Prv_Multi_Cancel_tpfct Cancel_pfct);
extern NvM_Prv_idJob_ten NvM_Prv_Multi_Process(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                               NvM_Prv_QueueEntry_tst* QueueEntry_pst,
                                               NvM_BlockIdType* idBlockToProcess_puo);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

#endif /* NVM_PRV_PROCESSMULTIBLOCK_H */
