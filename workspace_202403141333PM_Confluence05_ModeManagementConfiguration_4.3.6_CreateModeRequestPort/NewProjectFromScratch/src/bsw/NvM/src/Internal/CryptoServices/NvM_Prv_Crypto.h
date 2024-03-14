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
#ifndef NVM_PRV_CRYPTO_H
#define NVM_PRV_CRYPTO_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_JobStateMachine_Types.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_CryptoGenerateRandomNr_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_CryptoKeySetting_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_CryptoWrite_GetData(void);
extern NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_CryptoRead_GetData(void);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_CRYPTO_H */
#endif

