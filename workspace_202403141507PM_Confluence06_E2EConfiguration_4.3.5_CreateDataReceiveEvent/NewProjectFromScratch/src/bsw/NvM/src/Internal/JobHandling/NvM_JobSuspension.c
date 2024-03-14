/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2023, all rights reserved
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

#include "rba_MemLib_Special.h"

#include "NvM_Prv_JobQueue.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_JobResource.h"
#include "NvM_Prv_RequestQueue.h"
#include "NvM_Prv_JobSuspension.h"
/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

#if (NVM_JOB_PRIORITIZATION == STD_ON)

RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_JobSuspension_Suspend(NvM_Prv_Queue_idx_tst const* idxJob_pcst,
                                                              NvM_Prv_Job_tst* Job_pst);

RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_JobSuspension_IsSuspendRequired(NvM_Prv_Job_tst const* Job_pcst);

#endif

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
void NvM_Prv_JobSuspension_Handle(void)
{
#if (NVM_JOB_PRIORITIZATION == STD_ON)

    if (!NvM_Prv_RequestQueue_IsEmpty(NvM_Prv_idQueue_Immediate_e))
    {
        // search in job queue for jobs using resources as any user request from the immediate request queue
        NvM_Prv_Queue_idx_tuo cntrJob_uo = 0u;
        NvM_Prv_Queue_idx_tuo nrJobs_uo = NvM_Prv_JobQueue_GetNrEntries();
        NvM_Prv_Queue_idx_tst idxJob_st;

        idxJob_st.idxPrev_uo = NVM_PRV_QUEUE_IDX_INVALID;

        while (cntrJob_uo < nrJobs_uo)
        {
            NvM_Prv_Job_tst* Job_pst = NvM_Prv_JobQueue_GetJob(cntrJob_uo);
            idxJob_st.idx_uo = NvM_Prv_JobQueue_GetIdxJob(cntrJob_uo);

            /* MR12 RULE 13.5 VIOLATION: the function is called only if queue ID has required value, no unexpected side-effects */
            if (   (Job_pst->JobData_st.idQueue_uo != NvM_Prv_idQueue_Immediate_e)
                && (NvM_Prv_JobSuspension_Suspend(&idxJob_st, Job_pst)))
            {
                // -> job suspension required (job comes NOT from immediate queue) AND job suspended successfully
                // -> update number of jobs in job queue for the loop searching jobs to suspend
                nrJobs_uo = NvM_Prv_JobQueue_GetNrEntries();
            }
            else
            {
                // ->    no job suspension required (job cames from immediate queue)
                //    OR job resource is locked and cancel of job, which locks required resource, has failed
                //    OR queue for suspended requests is full (shall not happen, is done for robustness)
                // -> look for next job
                ++cntrJob_uo;
                idxJob_st.idxPrev_uo = idxJob_st.idx_uo;
            }
        }
    }

#endif
}

void NvM_Prv_JobSuspension_PrepareHighSpeedMode(boolean isHighSpeedModeRequired_b)
{
#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))

    if (isHighSpeedModeRequired_b)
    {
        // search in job queue for jobs using resources as any user request from the immediate request queue

        NvM_Prv_Queue_idx_tuo cntrJob_uo = 0u;
        NvM_Prv_Queue_idx_tuo nrJobs_uo = NvM_Prv_JobQueue_GetNrEntries();
        NvM_Prv_Queue_idx_tst idxJob_st;

        idxJob_st.idxPrev_uo = NVM_PRV_QUEUE_IDX_INVALID;

        while (cntrJob_uo < nrJobs_uo)
        {
            NvM_Prv_Job_tst* Job_pst = NvM_Prv_JobQueue_GetJob(cntrJob_uo);
            idxJob_st.idx_uo = NvM_Prv_JobQueue_GetIdxJob(cntrJob_uo);

            if (NvM_Prv_JobSuspension_Suspend(&idxJob_st, Job_pst))
            {
                // -> job suspended successfully
                // -> update number of jobs in job queue for the loop searching jobs to suspend
                nrJobs_uo = NvM_Prv_JobQueue_GetNrEntries();
            }
            else
            {
                // ->    job resource is locked and cancel of job, which locks required resource, has failed
                //    OR queue for suspended requests is full (shall not happen, is done for robustness)
                // -> look for next job
                ++cntrJob_uo;
                idxJob_st.idxPrev_uo = idxJob_st.idx_uo;
            }
        }

        NvM_Prv_SetHighSpeedMode(TRUE);
    }
#else
    (void)isHighSpeedModeRequired_b;
#endif
}


#if (NVM_JOB_PRIORITIZATION == STD_ON)

RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_JobSuspension_Suspend(NvM_Prv_Queue_idx_tst const* idxJob_pcst,
                                                              NvM_Prv_Job_tst* Job_pst)
{
    boolean isSuspended_b = FALSE;

    if (NvM_Prv_JobSuspension_IsSuspendRequired(Job_pst))
    {
        boolean isJobCanceled_b = FALSE;
        NvM_Prv_JobResource_Cluster_ten ClusterLocked_en = NvM_Prv_JobResource_GetLockedByJob(&Job_pst->JobData_st);

        if (ClusterLocked_en < NvM_Prv_JobResource_nrCluster_e)
        {
            if (NvM_Prv_JobResource_Cancel(ClusterLocked_en, Job_pst))
            {
                NvM_Prv_JobResource_Release(NvM_Prv_BlkDesc_GetIdJobResource(Job_pst->JobData_st.idBlock_uo,
                                                                             ClusterLocked_en),
                                            &Job_pst->JobData_st);
                isJobCanceled_b = TRUE;
            }
        }

        if ((ClusterLocked_en >= NvM_Prv_JobResource_nrCluster_e) || isJobCanceled_b)
        {
            isSuspended_b = NvM_Prv_JobQueue_SuspendJob(idxJob_pcst);
        }
    }

    return isSuspended_b;
}

RBA_MEMLIB_LOCAL_INLINE boolean NvM_Prv_JobSuspension_IsSuspendRequired(NvM_Prv_Job_tst const* Job_pcst)
{
    boolean isSuspendRequired_b = FALSE;
    NvM_Prv_JobResource_Cluster_ten Cluster_en = NvM_Prv_JobResource_Cluster_MemIf_e;

    while (!isSuspendRequired_b && (Cluster_en < NvM_Prv_JobResource_nrCluster_e))
    {
        NvM_Prv_idJobResource_tuo idJobResourceUsed_uo;
        idJobResourceUsed_uo = NvM_Prv_BlkDesc_GetIdJobResource(Job_pcst->JobData_st.idBlock_uo, Cluster_en);

        // is cluster used by given job
        if (NvM_Prv_idJobResource_NrJobResources_e != idJobResourceUsed_uo)
        {
            NvM_Prv_Queue_idx_tst idxRequest_st = {
                                                    NVM_PRV_QUEUE_IDX_INVALID,  // idx_uo
                                                    NVM_PRV_QUEUE_IDX_INVALID   // idxPrev_uo
                                                  };

            NvM_Prv_RequestQueue_GetRequest(idJobResourceUsed_uo, NvM_Prv_idQueue_Immediate_e, &idxRequest_st);

            if (NVM_PRV_QUEUE_IDX_INVALID != idxRequest_st.idx_uo)
            {
                isSuspendRequired_b = TRUE;
            }
        }

        /* MR12 RULE 10.1 VIOLATION: This function ensures that enum type is used only in valid range */
        ++Cluster_en;
    }

    return isSuspendRequired_b;
}

#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

