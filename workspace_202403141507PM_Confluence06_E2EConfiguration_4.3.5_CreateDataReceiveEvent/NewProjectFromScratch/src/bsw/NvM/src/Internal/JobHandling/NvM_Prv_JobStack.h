/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2022, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_JOBSTACK_H
#define NVM_PRV_JOBSTACK_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_Job_Types.h"
#include "NvM_Prv_JobStack_Types.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern void NvM_Prv_JobStack_Initialize(NvM_Prv_idJob_ten idJob_en,
                                        NvM_Prv_JobStack_tst* JobStack_pst,
                                        NvM_Prv_JobResult_tst* JobResult_pst,
                                        NvM_Prv_JobData_tst const* JobData_pcst);

extern boolean NvM_Prv_JobStack_IsEmpty(NvM_Prv_JobStack_tst const* JobStack_pcst);

extern void NvM_Prv_JobStack_Do(NvM_Prv_JobStack_tst* JobStack_pst,
                                NvM_Prv_JobResult_tst* JobResult_pst,
                                NvM_Prv_JobData_tst const* JobData_pcst);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_JOB_H */
#endif

