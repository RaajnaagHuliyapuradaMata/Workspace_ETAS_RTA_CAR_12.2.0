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
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM.h"

#include "NvM_Prv.h"
#include "NvM_Prv_Job.h"
#include "NvM_Prv_JobQueue.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * This private function handles all jobs in the NvM internal job queue.
 */
void NvM_Prv_MainFunctionJobHandling(void)
{
    NvM_Prv_Queue_idx_tuo cntrJob_uo;

    for (cntrJob_uo = 0u; cntrJob_uo < NvM_Prv_JobQueue_GetNrEntries(); ++cntrJob_uo)
    {
        NvM_Prv_Job_tst* Job_pst = NvM_Prv_JobQueue_GetJob(cntrJob_uo);

        NvM_Prv_Job_Do(Job_pst);
    }
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

