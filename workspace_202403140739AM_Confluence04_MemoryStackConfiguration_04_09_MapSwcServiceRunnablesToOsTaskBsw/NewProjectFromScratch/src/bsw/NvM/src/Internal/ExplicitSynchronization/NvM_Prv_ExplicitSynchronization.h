/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2017, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_EXPLICITSYNCHRONIZATION_H
#define NVM_PRV_EXPLICITSYNCHRONIZATION_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_BlockDescriptor_Types.h"
#include "NvM_Prv_JobStateMachine_Types.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern boolean NvM_Prv_ExplicitSync_CheckBlockSizes(void);
extern NvM_Prv_JobResult_ten NvM_Prv_ExplicitSync_CopyData(NvM_Prv_ExplicitSync_Copy_tpfct CopyFunction_pfct,
                                                           NvM_BlockIdType idBlock_uo,
                                                           uint8* cntrExpSyncOperations_pu8,
                                                           uint8* RamBlock_pu8);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_EXPLICITSYNCHRONIZATION_H */
#endif
