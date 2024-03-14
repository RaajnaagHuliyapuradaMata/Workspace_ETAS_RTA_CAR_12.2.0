/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
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

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
#if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

#include "MemIf_Types.h"
#include "Fee_Cfg_SchM.h"
#include "Fee_Prv_Config.h"
#include "Fee_Prv_Lib.h"
#include "Fee_Prv_FsIf.h"
#include "Fee_Prv_Job.h"
#include "Fee_PrvTypes.h"
#include "Fee_Prv_Config.h"

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/**
 * \brief   Try to place an order in the internal job slot.
 *          Based on block configuration the right job slot is chosen.
 *          If the Fee is not ready or the job slot is occupied E_NOT_OK is returned.
 *
 * \param   deviceName_en       Device instance for which job has to be performed
 * \param   apiId_u8            The ID of the API which places the job
 * \param   jobDesc_pcst        The order that shall be placed
 *
 * \retval  E_OK                Order placed successfully
 * \retval  E_NOT_OK            Order could not be placed
 */
Std_ReturnType Fee_Prv_JobPut(Fee_Rb_DeviceName_ten       deviceName_en,
                              uint8                       apiId_u8,
                              Fee_Prv_JobDesc_tst const * jobDesc_pcst)
{
    Std_ReturnType  result_en = E_NOT_OK;
    Fee_Rb_User_ten user_en;

    /* Get the block pointer to the block properties table for the validaiton checks */
    Fee_Rb_BlockPropertiesType_tst const* blockPropertiesTable_pcst = Fee_Prv_FsIfGetBlockPropertiesTable(deviceName_en);

    /* Get the pointer to config data model */
    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    Fee_Prv_JobData_tst *                jobData_pst            = &deviceConfigTable_pcst->feeData_pst->jobData_st;

    /* Select the user based on the type of the job. */
    if((FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORG_E <= jobDesc_pcst->type_en) && (FEE_RB_JOBTYPE_MAX_E > jobDesc_pcst->type_en))
    {
        /* Handle as a system job, not as a block job */
        user_en = FEE_RB_USER_SYSTEM_E;
    }
    else if(jobDesc_pcst->isChunkJob_b)
    {
        /* Chunk-wise job operations also have their own slot/user */
        user_en = FEE_RB_USER_CHUNK_E;
    }
    else
    {
        /* Check block configuration to find out who is the user */
        user_en = Fee_Prv_ConfigGetBlockUserByBlockNr(jobDesc_pcst->blockNumber_u16, blockPropertiesTable_pcst);
    }

    SchM_Enter_Fee_Order();
    /*
     * Make sure the job slot is free.
     * A spin lock is needed since the main function might want to finish a job asynchronous.
     */
    if(    (FEE_RB_JOBTYPE_MAX_E             == jobData_pst->jobs_ast[user_en].type_en)
        || (FEE_RB_JOBTYPE_WAIT_NEXT_CHUNK_E == jobData_pst->jobs_ast[user_en].type_en) )
    {
        /* Set job and result of the current job */
        jobData_pst->jobs_ast   [user_en] = *jobDesc_pcst;
        jobData_pst->results_aen[user_en] = FEE_RB_JOB_PENDING;

        if(FEE_RB_USER_CHUNK_E == jobData_pst->actvUser_en)
        {
            /* For chunk job, take over the new details for the next chunk into the working object */
            jobData_pst->actvJobDesc_st = *jobDesc_pcst;
            /* Reset last chunk(-wise job) results */
            jobData_pst->jobs_ast[user_en].bytDone_un.nrBytProc_u32 = 0u;
            jobData_pst->isChunkJobCncldByOthUser_b = FALSE;
        }
        result_en = E_OK;
    }
    SchM_Exit_Fee_Order();

    /*
     * In case placing the order did not work trigger Det error.
     * Do not do this for internal orders.
     * Do not do this under interrupt lock!
     */
    if(E_NOT_OK == result_en)
    {
        switch(user_en)
        {
            case FEE_RB_USER_NVM_E:     Fee_Prv_LibDetReport(deviceName_en, apiId_u8, FEE_E_BUSY         ); break;
            case FEE_RB_USER_ADAPTER_E:
            case FEE_RB_USER_SYSTEM_E:
            case FEE_RB_USER_CHUNK_E:   Fee_Prv_LibDetReport(deviceName_en, apiId_u8, FEE_E_BUSY_INTERNAL); break;
            default:                            /* Not DET report for internal orders */                    break;
        }
    }

    return(result_en);
}

# if (defined(FEE_PRV_CFG_RB_CANCEL) && (TRUE == FEE_PRV_CFG_RB_CANCEL))
/**
 * \brief   Cancel the job from queue, to be ready to accept new request.
 * Only the passed job will be cancelled. Nothing will be done in case the passed job is not present in the queue.
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table for which the job has to be finished
 * \param   user_en                 user for which the job has to be canceled
 *
 * \retval  TRUE                    Job was successfully cancelled
 * \retval  FALSE                   Job was not in the queue, so nothing was done
 */
boolean Fee_Prv_JobCancelInQue(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst, Fee_Rb_User_ten user_en)
{
    boolean jobCanceled_b;
    /* Get the pointer to config data model */
    Fee_Prv_JobData_tst * jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;

    /* Cancel job under spin lock */
    SchM_Enter_Fee_Order();

    /* Cancel the requested job */
    if(FEE_RB_JOBTYPE_MAX_E != jobData_pst->jobs_ast[user_en].type_en)
    {
        jobData_pst->jobs_ast[user_en].type_en = FEE_RB_JOBTYPE_MAX_E;
        jobData_pst->results_aen[user_en] = FEE_RB_JOB_CANCELED;

        if(FEE_RB_USER_CHUNK_E == user_en)
        {
            /* Clear the number of processed bytes in last chunk */
            jobData_pst->jobs_ast[FEE_RB_USER_CHUNK_E].bytDone_un.nrBytProc_u32 = 0u;
        }

        jobCanceled_b = TRUE;
    }
    else
    {
        /* There is no ongoing job for the requested user -> nothing to cancel. Do nothing, it is important that the
         * status of the job result is untouched (AUTOSAR requirement SWS_Fee_00164) */
        jobCanceled_b = FALSE;
    }

    SchM_Exit_Fee_Order();

    return(jobCanceled_b);
}

/**
 * \brief   Function when called cancels the ongoing active job.
 *
 * Aim is to keep the impact on the other users as minimum as possible (both ongoing and yet to be started). Which means
 * if user A request is ongoing, user B calls cancel, the ongoing job of user A will be canceled internally and
 * restarted later. In the mean time the status for the user A is "job pending" -> from the user perspective, it observes
 * unusually longer time to complete the job. However, this is not possible if a multi chunk job has already started.
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table which has to be initialized.
 * \param   user_en                 user which has requested the ongoing job to be canceled
 */
void Fee_Prv_JobCancelActv(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Rb_User_ten user_en)
{
    Fee_Prv_JobData_tst * jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;

    /* Cancel job under spin lock */
    SchM_Enter_Fee_Order();
    /* Multi chunk jobs can not be restarted internally and should also be canceled if one chunk is already finished.
     * When first chunk is being processed, it is not needed to cancel the request. The request can still be kept in the
     * queue and restarted at later point of time. */
    if((FEE_RB_USER_CHUNK_E == jobData_pst->actvUser_en) &&
       (0u != jobData_pst->actvJobDesc_st.bytDone_un.cntrPayloadBytDone_u32))
    {
        if(FEE_RB_USER_CHUNK_E == user_en)
        {
            jobData_pst->isChunkJobCncldByOthUser_b = FALSE;
        }
        else
        {
            jobData_pst->isChunkJobCncldByOthUser_b = TRUE;
            /* Since the job is canceled by another user, do not clear the number of processed bytes in last chunk ->
             * the job would have been canceled between the time Chunk user checks the status via Fee_Rb_Idx_ChunkGetResult()
             * and the number of processed bytes via Fee_Rb_Idx_ChunkGetNrBytProc(). */
            jobData_pst->jobs_ast[FEE_RB_USER_CHUNK_E].type_en = FEE_RB_JOBTYPE_MAX_E;
            jobData_pst->results_aen[FEE_RB_USER_CHUNK_E] = FEE_RB_JOB_CANCELED;
        }
    }

    jobData_pst->actvUser_en = FEE_RB_USER_MAX_E;
    jobData_pst->actvJobDesc_st.type_en = FEE_RB_JOBTYPE_MAX_E;

    SchM_Exit_Fee_Order();
    return;
}

# endif

# if(defined(FEE_PRV_CFG_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API) && (TRUE == FEE_PRV_CFG_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API))
/**
 * \brief   Enter high speed mode where writes are performed as fast as possible
 *
 * \param   deviceConfigTable_pcst Pointer to the config table for which the job has to be finished.
 * \param   user_en                User for which the jobs have to be allowed
 *
 * \retval  E_OK                   High speed mode was successfully set for the passed user
 * \retval  E_NOT_OK               High speed mode could not be set for the passed user
 */
Std_ReturnType Fee_Prv_JobEnterHighSpeedMode(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                             Fee_Rb_User_ten                      user_en)
{
    Std_ReturnType        result_en = E_NOT_OK;
    /* Get the pointer to config data model */
    Fee_Prv_JobData_tst * jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;

    /* Begin job under interrupt lock since at the same time ExitHighSpeedMode API for same or another user might be
     * called. */
    SchM_Enter_Fee_Order();

    /* High speed mode can be set only for one user. Accept the request only if the mode is not already set for a user
     * or requested for the same user again. */
    if((FEE_RB_USER_MAX_E == jobData_pst->allwdUser_en))
    {
        jobData_pst->allwdUser_en = user_en;

        /* Set the write and invalidate to fastest possible mode + allow write during hard reorg */
        jobData_pst->jobMode_en[FEE_RB_JOBTYPE_WRITE_E] = FEE_RB_WRITEJOB_SKIP_HARDREORG_WRITE_ONLY_E;
        jobData_pst->jobMode_en[FEE_RB_JOBTYPE_INVALIDATE_E] = FEE_RB_INVALIDATEJOB_SKIP_HARDREORG_WRITE_ONLY_E;
        /* Disable block maintenance feature */
        jobData_pst->jobMode_en[FEE_RB_JOBTYPE_BLOCKMAINTENANCE_E] = FEE_RB_BLOCKMAINTENANCE_DISABLE_E;

        result_en = E_OK;
    }

    SchM_Exit_Fee_Order();

    return(result_en);
}

/**
 * \brief   The function exits high speed mode for the user and hence enables execution of all user jobs.
 *          This API is called to do the reverse actions done by the function Fee_Prv_JobEnterHighSpeedModeForPassedUser
 *
 * \param   deviceConfigTable_pcst Pointer to the config table for which the job has to be finished.
 * \param   user_en                User which is requesting to exit high speed mode
 *
 * \retval  E_OK                   High speed mode was successfully exited for the passed user
 * \retval  E_NOT_OK               High speed mode could not be exited for the passed user
 */
Std_ReturnType Fee_Prv_JobExitHighSpeedMode(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                            Fee_Rb_User_ten                      user_en)
{
    Std_ReturnType        result_en = E_NOT_OK;
    /* Get the pointer to config data model */
    Fee_Prv_JobData_tst * jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;

    /* Begin job under interrupt lock since at the same time EnterHighSpeedMode API for same or another user might be
     * called. */
    SchM_Enter_Fee_Order();

    /* High speed mode can be exited only by the user which had set it. Reject the request coming from other user
     * or requested for the same user again. */
    if((FEE_RB_USER_MAX_E != jobData_pst->allwdUser_en) && (user_en == jobData_pst->allwdUser_en))
    {
        /* Allow processing of all jobs */
        jobData_pst->allwdUser_en = FEE_RB_USER_MAX_E;

        /* Reset write, invaldiate and maintain job modes to normal working */
        jobData_pst->jobMode_en[FEE_RB_JOBTYPE_WRITE_E] = FEE_RB_ALLJOBS_ALLSTEPS_E;
        jobData_pst->jobMode_en[FEE_RB_JOBTYPE_INVALIDATE_E] = FEE_RB_ALLJOBS_ALLSTEPS_E;
        jobData_pst->jobMode_en[FEE_RB_JOBTYPE_BLOCKMAINTENANCE_E] = FEE_RB_ALLJOBS_ALLSTEPS_E;

        result_en = E_OK;
    }
    SchM_Exit_Fee_Order();

    return(result_en);
}
# endif

/**
 * \brief   Called by the main function if it wants to begin an order.
 *          Jobs are priotorized in the order in which the users are mentioned in Fee_Rb_User_ten.
 *          In special mode, only a particular user will be processed (ex. in crash, where only NvM jobs are processed).
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which the next job has to searched and returned.
 */
void Fee_Prv_JobCheckAndSetNextJob(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_JobData_tst * jobData_pst;
    Fee_Rb_User_ten       nxtUserJob_en = Fee_Prv_JobGetNxtUserJob(deviceConfigTable_pcst);

    /* If a new job is available, set the job details into working object */
    if(FEE_RB_USER_MAX_E != nxtUserJob_en)
    {
        jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;
        /* Begin job under interrupt lock since at the same time an external user might modify the job. */
        SchM_Enter_Fee_Order();
        jobData_pst->actvJobDesc_st = jobData_pst->jobs_ast[nxtUserJob_en];
        jobData_pst->actvUser_en    = nxtUserJob_en;
        SchM_Exit_Fee_Order();
    }

    return;
}

/**
 * \brief   Called by the main function if it wants to finish a job.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which the job has to be finished.
 * \param   result_en                Result of that job
 */
void Fee_Prv_JobEnd(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Rb_JobResultType_ten result_en)
{
    Fee_Prv_JobData_tst * jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;
    Fee_Rb_User_ten       actvUser_en;

    /* Finish job under spin lock */
    SchM_Enter_Fee_Order();

    actvUser_en = jobData_pst->actvUser_en;

    jobData_pst->actvJobDesc_st.bytDone_un.cntrPayloadBytDone_u32 = (uint32)jobData_pst->actvJobDesc_st.nrBytTot_u16;

    /* Do not update the result for chunk wise job, as we need to also consider if all chunks are completed or there
     * are some more chunks pending. This is already updated in the function Fee_Prv_JobUpdateChunkStatus,
     * Fee_Prv_JobChunkEnd and Fee_Prv_JobCancelJobInQueue */
    if(FEE_RB_USER_CHUNK_E != actvUser_en)
    {
        jobData_pst->results_aen[actvUser_en] = result_en;
    }
    jobData_pst->actvJobDesc_st.type_en = FEE_RB_JOBTYPE_MAX_E;
    jobData_pst->actvUser_en = FEE_RB_USER_MAX_E;
    jobData_pst->jobs_ast[actvUser_en].type_en = FEE_RB_JOBTYPE_MAX_E;

    SchM_Exit_Fee_Order();

    return;
}

# if(defined(FEE_PRV_CFG_RB_CHUNK_JOBS) && (TRUE == FEE_PRV_CFG_RB_CHUNK_JOBS))
/**
 * \brief   Called by the main function if a chunk is done, but the chunk-wise job is incomplete.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which the job has to be finished.
 */
void Fee_Prv_JobChunkEnd(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_JobData_tst * jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;
    Fee_Prv_JobDesc_tst * jobDesc_pst = &jobData_pst->actvJobDesc_st;

    /* Finish chunk under spin lock */
    SchM_Enter_Fee_Order();
    jobDesc_pst->type_en = FEE_RB_JOBTYPE_WAIT_NEXT_CHUNK_E;
    jobData_pst->jobs_ast[FEE_RB_USER_CHUNK_E].type_en = FEE_RB_JOBTYPE_WAIT_NEXT_CHUNK_E;

    jobData_pst->actvJobDesc_st.bytDone_un.cntrPayloadBytDone_u32 += jobDesc_pst->length_u16;

    SchM_Exit_Fee_Order();

    return;
}

/**
 * \brief   Function called after the current chunk is finished, to update the status and number of bytes done into
 * internal variables.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which the job has to be finished
 * \param   result_en                Result of the job
 * \param   nrBytProc_u32            Number of bytes processed in last chunk (including chunk header size)
 */
void Fee_Prv_JobUpdateChunkStatus(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst,
                                  Fee_Rb_JobResultType_ten              result_en,
                                  uint32                                nrBytProc_u32)
{
    Fee_Prv_JobData_tst * jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;

    /* Finish job under spin lock */
    SchM_Enter_Fee_Order();
    jobData_pst->results_aen[FEE_RB_USER_CHUNK_E] = result_en;
    jobData_pst->jobs_ast[FEE_RB_USER_CHUNK_E].bytDone_un.nrBytProc_u32 = nrBytProc_u32;
    SchM_Exit_Fee_Order();

    return;
}
# endif

/**
 * \brief   Returns the highest priority user for which a job has been placed in the queue
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table.
 *
 * \return  user with highest prio of all requested job. Returns FEE_RB_USER_MAX_E when no jobs in the queue.
 */
Fee_Rb_User_ten Fee_Prv_JobGetNxtUserJob(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Rb_User_ten       highestPrioUser_en;
    uint32                i_u32;
    Fee_Prv_JobData_tst * jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;

    highestPrioUser_en = FEE_RB_USER_MAX_E;

    /* Check if all users are allowed to be processed? */
    if(FEE_RB_USER_MAX_E != jobData_pst->allwdUser_en)
    {
        /* Check only for the allowed user if there is any job in the queue? */
        if(FEE_RB_JOBTYPE_MAX_E != jobData_pst->jobs_ast[jobData_pst->allwdUser_en].type_en)
        {
            highestPrioUser_en = jobData_pst->allwdUser_en;
        }
    }
    else
    {
        for(i_u32 = (uint32)FEE_RB_USER_SYSTEM_E;
                (i_u32 < (uint32)FEE_RB_USER_MAX_E) && (FEE_RB_USER_MAX_E == highestPrioUser_en);
                i_u32++)
        {
            if(FEE_RB_JOBTYPE_MAX_E != jobData_pst->jobs_ast[i_u32].type_en)
            {
                /* MR12 RULE 10.5 VIOLATION: Cast is uncritical, because i_u32 will not exceed FEE_RB_USER_MAX_E */
                highestPrioUser_en = (Fee_Rb_User_ten)i_u32;
            }
        }
    }
    return (highestPrioUser_en);
}

/**
 * \brief   Returns the result of the job for the given user.
 *
 * \param   deviceName_en               Device instance for which job result has to be returned
 * \param   user_en                     Select the job slot to be checked (NvM, Adapter, Debug, Chunk)
 *
 * \retval  FEE_RB_JOB_OK                Last job executed successfully
 * \retval  FEE_RB_JOB_FAILED            Last job failed unexpected
 * \retval  FEE_RB_JOB_PENDING           Last job is still running
 * \retval  FEE_RB_BLOCK_INCONSISTENT    Last job was a read and
 *                                       a) Not a single instance of all instances has consistent data
 *                                       b) The block is not present at all
 * \retval  FEE_RB_BLOCK_INVALID         Last job was a read and the block was invalidated intentionally
 * \retval  FEE_RB_BLOCK_PART_OK         Processing of last chunk has fishished successfully, ready for next chunk
 * \retval  FEE_RB_JOB_CANCELED          Processing of the job canceled by the user
 *
 * \attention   Job result can change asynchronously - atomic access because of <=32bit memory access and alignment is ensured.
 */
Fee_Rb_JobResultType_ten Fee_Prv_JobGetResult(Fee_Rb_DeviceName_ten deviceName_en, Fee_Rb_User_ten user_en)
{
    Fee_Rb_JobResultType_ten result_en;

    /* Get the pointer to config data model */
    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    Fee_Prv_JobData_tst *                jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;

    result_en = jobData_pst->results_aen[user_en];

    return(result_en);
}

#  if((TRUE == FEE_PRV_CFG_RB_SET_AND_GET_JOB_MODE) || (STD_ON == FEE_PRV_CFG_SET_MODE_SUPPORTED)  || (TRUE == FEE_PRV_CFG_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API))
/* \brief   Return the current job mode for the passed job
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table for which the driver mode is requested
 * \param   jobType_en              Job type for which mode is requested
 *
 * \return  jobMode_en              Currently active mode for the passed job type
 */
Fee_Rb_JobMode_ten Fee_Prv_JobGetJobMode(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                         Fee_Rb_JobType_ten                   jobType_en)
{
    Fee_Rb_JobMode_ten      jobMode_en;
    Fee_Prv_JobData_tst *   jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;

    jobMode_en = jobData_pst->jobMode_en[jobType_en];

    return(jobMode_en);
}

/* \brief   Store the requested job mode for the passed job
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table for which the driver mode has to be set
 * \param   jobType_en              Job type for which mode has to be set
 * \param   jobMode_en              Mode that is requested to be set for the passed job type
 */
void Fee_Prv_JobSetJobMode(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                           Fee_Rb_JobType_ten                   jobType_en,
                           Fee_Rb_JobMode_ten                   jobMode_en)
{
    Fee_Prv_JobData_tst * jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;

    jobData_pst->jobMode_en[jobType_en] = jobMode_en;

    return;
}
# endif

# if(STD_ON == FEE_PRV_CFG_SET_MODE_SUPPORTED)
/**
 * \brief   Set the modes of all the jobs based on the passed driver mode
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table for which the driver mode has to be set
 * \param   mode                    Driver mode
 */
void Fee_Prv_JobSetModesBasedOnDriverMode(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                          MemIf_ModeType                       mode)
{
    uint8                   i_u8;
    Fee_Prv_JobData_tst *   jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;

    /* Set the modes for each of the job types */
    if (MEMIF_MODE_SLOW == mode)
    {
        /* Set all the jobs to the default mode */
        for(i_u8 = 0; i_u8 < (uint8)FEE_RB_JOBTYPE_MAX_E; i_u8++)
        {
            jobData_pst->jobMode_en[i_u8] = FEE_RB_ALLJOBS_ALLSTEPS_E;
        }
    }
    else
    {
        /* It could be possible that the job modes are different for different file system (due to some modes not yet
         * supported in the file system or customer requirement). So get the mode to be set from the respective file system  */
        for(i_u8 = 0; i_u8 < (uint8)FEE_RB_JOBTYPE_MAX_E; i_u8++)
        {
            /* MR12 RULE 10.5 VIOLATION: Cast is uncritical, because i_u8 will not exceed FEE_RB_JOBTYPE_MAX_E */
            jobData_pst->jobMode_en[i_u8] = Fee_Prv_FsIfGetJobModeForFastDriverMode(deviceConfigTable_pcst, (Fee_Rb_JobType_ten)i_u8);
        }
    }

    return;
}
# endif

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif
