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
#ifndef NVM_PRV_SERVICE_H
#define NVM_PRV_SERVICE_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_Service_Types.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern void NvM_Prv_Service_InitiateMulti(NvM_Prv_idQueue_tuo idQueue_uo,
                                          NvM_Prv_QueueEntry_tst const* QueueEntry_pcst,
                                          NvM_Prv_Service_ConfigurationMulti_tst const* ConfigurationMulti_pcst);

extern Std_ReturnType NvM_Prv_Service_InitiateSingle(NvM_Prv_idQueue_tuo idQueue_uo,
                                                     NvM_Prv_BlockData_tst* BlockData_pst,
                                                     NvM_Prv_Service_Configuration_tst const* ServiceConfiguration_pcst);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_SERVICE_H */
#endif
