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
#ifndef NVM_PRV_JOB_H
#define NVM_PRV_JOB_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Types.h"

#include "NvM_Prv_Job_Types.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern void NvM_Prv_Job_PrepareData(NvM_Prv_idJob_ten idJob_en,
                                    NvM_Prv_idQueue_tuo idReqQueue_uo,
                                    NvM_Prv_JobData_tst* JobData_pst,
                                    NvM_Prv_QueueEntry_tst const* Request_pcst);

extern void NvM_Prv_Job_Start(NvM_Prv_idJob_ten idJob_en,
                              NvM_Prv_idQueue_tuo idReqQueue_uo,
                              NvM_Prv_QueueEntry_tst const* Request_pcst);

extern void NvM_Prv_Job_Do(NvM_Prv_Job_tst* Job_pst);

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
extern NvM_Prv_Job_tst* NvM_Prv_Job_Get(NvM_BlockIdType idBlock_uo,
                                        NvM_Prv_idService_tuo idService_uo);
#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"


/* NVM_PRV_JOB_H */
#endif

