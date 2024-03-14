/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_CBK_H
#define NVM_CBK_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM.h"

#if (NVM_CRYPTO_USED == STD_ON)
/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern void NvM_Csm_Callback(uint32 jobId, Std_ReturnType result);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

#endif  // (NVM_CRYPTO_USED == STD_ON)

/* NVM_CBK_H */
#endif

