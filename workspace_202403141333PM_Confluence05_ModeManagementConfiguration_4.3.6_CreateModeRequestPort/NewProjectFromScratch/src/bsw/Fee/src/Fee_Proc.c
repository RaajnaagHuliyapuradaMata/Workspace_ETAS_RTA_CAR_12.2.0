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
# if (defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

#include "Fee_Cfg_SchM.h"
#include "Fee_Rb_Types.h"
#include "Fee_Cfg.h"
#include "Fee_Rb_Idx.h"
#include "Fee_Prv_Chunk.h"
#include "Fee_Prv_Dbg.h"
#include "Fee_Prv_FsIf.h"
#include "Fee_Prv_Job.h"
#include "Fee_Prv_Lib.h"
#include "Fee_Prv_Order.h"
#include "Fee_Prv_Config.h"
#include "Fee_Prv_Proc.h"
#include "Fee_Prv_Medium.h"
#include "MemIf_Types.h"

# if (STD_ON == FEE_PRV_CFG_DEV_ERROR_DETECT)
#include "Det.h"
# endif

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/* Protection selectable via Fee_Cfg_SchM.h to save code */
# ifndef FEE_RB_USE_PROTECTION
#define FEE_RB_USE_PROTECTION   (TRUE)
# endif

# if(!defined(FEE_PRV_CFG_RB_CHUNK_JOBS) || (FALSE == FEE_PRV_CFG_RB_CHUNK_JOBS))
#define Fee_Prv_ProcChunkReadDo(deviceConfigTable_pcst, proc_pst, jobType_en)         Fee_Prv_FsIfReadDo(deviceConfigTable_pcst)
#define Fee_Prv_ProcChunkWriteDo(deviceConfigTable_pcst, proc_pst, jobType_en)        Fee_Prv_FsIfWriteDo(deviceConfigTable_pcst)
# else
#define Fee_Prv_ProcChunkReadDo(deviceConfigTable_pcst, proc_pst, jobType_en)         Fee_Prv_ProcChunkJobDo(deviceConfigTable_pcst, proc_pst, jobType_en)
#define Fee_Prv_ProcChunkWriteDo(deviceConfigTable_pcst, proc_pst, jobType_en)        Fee_Prv_ProcChunkJobDo(deviceConfigTable_pcst, proc_pst, jobType_en)
# endif

/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

LOCAL_INLINE void Fee_Prv_ProcMainFunctionIntern(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void Fee_Prv_ProcStartExtJob(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void Fee_Prv_ProcProcessIdle(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE void Fee_Prv_ProcProcessBackground(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
# if(defined(FEE_PRV_CFG_RB_CHUNK_JOBS) && (TRUE == FEE_PRV_CFG_RB_CHUNK_JOBS))
LOCAL_INLINE Fee_Rb_JobResultType_ten Fee_Prv_ProcChunkJobDo(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Prv_Proc_tst * proc_pst, Fee_Rb_JobType_ten jobType_en);
LOCAL_INLINE boolean Fee_Prv_ProcIsJobDone(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst, Fee_Rb_JobResultType_ten result_en);
# endif
LOCAL_INLINE void Fee_Prv_ProcProcessExtJob(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Rb_WorkingStateType_ten Fee_Prv_ProcCalcWorkingState(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE boolean Fee_Prv_ProcTryToGetLock(Fee_Rb_DeviceName_ten deviceName_en, Fee_Prv_ProcLock_tst * procLock_pst);
LOCAL_INLINE void Fee_Prv_ProcReleaseLock(Fee_Prv_ProcLock_tst * procLock_pst);
LOCAL_INLINE Fee_Rb_JobResultType_ten Fee_Prv_ProcEnterStopModeDo(Fee_Prv_Proc_tst * proc_pst);

/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
 */

/*
 * \brief   Disables execution of background operations.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which the background has to be disabled.
 */
void Fee_Prv_ProcDisableBackground(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_Proc_tst* proc_pst = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;

    proc_pst->backgroundState_en = FEE_PRV_PROC_BACKGROUNDSTATE_DISABLED;

    return;
}

/*
 * \brief   Enables execution of background operations.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which the background has to be enabled.
 */
void Fee_Prv_ProcEnableBackground(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_Proc_tst* proc_pst = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;

    proc_pst->backgroundState_en = FEE_PRV_PROC_BACKGROUNDSTATE_ENABLED_ACTIVE;

    return;
}

/*
 * \brief   Set the state of the driver.
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table for which the background has to be enabled.
 * \param   driverState_en          The state which shall be entered
 */
void Fee_Prv_ProcSetDriverState(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst, Fee_Prv_ProcDriverState_ten driverState_en)
{
    deviceConfigTable_pcst->feeData_pst->procData_st.proc_st.driverState_ven = driverState_en;

    return;
}

/*
 * \brief   Return the current state of the driver.
 *
 * \param   deviceName_en   Device instance for which the driver state has to be returned
 *
 * \return  Current state of the driver
 */
Fee_Prv_ProcDriverState_ten Fee_Prv_ProcGetDriverState(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Get the pointer to config data model */
    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    Fee_Prv_Proc_tst*                    proc_pst               = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;

    return(proc_pst->driverState_ven);
}

/* \brief   Return the current driver execution mode
 *
 * \param   deviceName_en   Device instance for which the driver mode has to be returned
 *
 * \return  MEMIF_MODE_SLOW: Driver executes in slow mode
 * \return  MEMIF_MODE_FAST: Driver executes in fast mode
 */
MemIf_ModeType Fee_Prv_ProcGetDriverMode(Fee_Rb_DeviceName_ten deviceName_en)
{
    MemIf_ModeType retVal_en;
    /* Get the pointer to config data model */
    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
    Fee_Prv_Proc_tst*                    proc_pst               = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;

    /* According to the prerequisites that MemStack has to run on >= 32 bit controllers and data are always aligned,
     * atomic access is guaranteed by the hardware for 32 bit memory access
     */
    retVal_en = proc_pst->driverModeActv_en;

   return(retVal_en);
}

# if(STD_ON == FEE_PRV_CFG_SET_MODE_SUPPORTED)
/**
 * \brief   Set the mode of the Fee and the underlying flash driver
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table for which the driver mode has to be set
 * \param   mode                    requested mode to set
 */
void Fee_Prv_ProcSetDriverMode(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst, MemIf_ModeType mode)
{
    Fee_Prv_Proc_tst* proc_pst = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;

    /* According to the prerequisites that MemStack has to run on >= 32 bit controllers and data are always aligned,
     * atomic access is guaranteed by the hardware for 32 bit memory access
     */
    proc_pst->driverModeActv_en = mode;

    /* Set the modes for each of the job types */
    Fee_Prv_JobSetModesBasedOnDriverMode(deviceConfigTable_pcst, mode);

    return;
}

#  if (FEE_PRV_CFG_AR_RELEASE_MAJOR_VERSION == 4) && (FEE_PRV_CFG_AR_RELEASE_MINOR_VERSION >= 4)
/* \brief   Set the driver execution mode request
 *
 * \param deviceConfigTable_pcst   Pointer to the config table for which the driver mode has to be set
 * \param driverModeReq_en        MEMIF_MODE_SLOW: Driver executes in slow mode
 *                                MEMIF_MODE_FAST: Driver executes in fast mode
 */
void Fee_Prv_ProcSetDriverModeReq(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst, MemIf_ModeType driverModeReq_en)
{
    /* According to the prerequisites that MemStack has to run on >= 32 bit controllers and data are always aligned,
     * atomic access is guaranteed by the hardware for 32 bit memory access
     */
    Fee_Prv_Proc_tst* proc_pst = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;

    proc_pst->driverModeReqd_en = driverModeReq_en;

    return;
}
#  endif

# endif

/*
 * \brief   Returns the current Fee working state.
 *
 * \param deviceConfigTable_pcst   Pointer to the config table for which the working set has to be returned
 *
 * \return  Current working state of the driver
 */
Fee_Rb_WorkingStateType_ten Fee_Prv_ProcGetWorkingState(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst)
{
    Fee_Prv_ProcData_tst * procData_pst = &deviceConfigTable_pcst->feeData_pst->procData_st;

    return(procData_pst->procWorkingState_en);
}

/*
 * \brief   Activates the stop mode.
 *
 * \param   proc_pst        Pointer to the common variables of the proc unit
 * \retval  FEE_RB_JOB_OK   Entering stop mode was successful
 */
LOCAL_INLINE Fee_Rb_JobResultType_ten Fee_Prv_ProcEnterStopModeDo(Fee_Prv_Proc_tst * proc_pst)
{
    proc_pst->stopModeActive_b = TRUE;

    return(FEE_RB_JOB_OK);
}

/*
 * \brief   Check if the stop mode is activated for given Fee device instance.
 *
 * \param   deviceName_en    Device instance for which check has to be performed
 * \return  Returns true if stop mode is activated
 */
boolean Fee_Prv_ProcIsStopModeActive(Fee_Rb_DeviceName_ten deviceName_en)
{
    return Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en)->feeData_pst->procData_st.proc_st.stopModeActive_b;
}

/*
 * \brief   Starts the job requested from the job unit by triggering the filesystem specific handler function.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table, for which the job has to be started
 */
LOCAL_INLINE void Fee_Prv_ProcStartExtJob(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Rb_JobType_ten jobType_en = Fee_Prv_JobGetActvJobType(deviceConfigTable_pcst);

    /* Start timing measurement for new job */
    Fee_Prv_DbgWatchStart(deviceConfigTable_pcst->deviceName_en, Fee_Prv_DbgWatchGetTmrForJob(jobType_en));

    /* Check pending order type */
    switch(jobType_en)
    {
        case FEE_RB_JOBTYPE_READ_E:             Fee_Prv_FsIfRead(deviceConfigTable_pcst);                    break;
        case FEE_RB_JOBTYPE_WRITE_E:            Fee_Prv_FsIfWrite(deviceConfigTable_pcst);                   break;
        case FEE_RB_JOBTYPE_INVALIDATE_E:       Fee_Prv_FsIfInvalidateBlock(deviceConfigTable_pcst);         break;
        case FEE_RB_JOBTYPE_BLOCKMAINTENANCE_E: Fee_Prv_FsIfRbMaintainBlock(deviceConfigTable_pcst);         break;
        case FEE_RB_SYSTEM_JOBTYPE_STOP_MODE_E: /* Do nothing */                                             break;
        default:                                Fee_Prv_FsIfRbSystemJob(deviceConfigTable_pcst, jobType_en); break;
    }

    return;
}

# if(defined(FEE_PRV_CFG_RB_CHUNK_JOBS) && (TRUE == FEE_PRV_CFG_RB_CHUNK_JOBS))
/*
 * \brief   Processes an ongoing (chunk-wise) read/write job.
 *          If the chunk-wise job has been waiting for the next chunk, the job is restarted to process it afterwards.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table, for which the job has to be processed
 * \param   proc_pst                 Pointer to the common variables of the proc unit
 * \param   jobType_en               Job to be processed
 *
 * \return  Job result
 *
 * \attention  This function is only called for the job types FEE_RB_JOBTYPE_READ_E or FEE_RB_JOBTYPE_WRITE_E.
 *             For other job types FEE_RB_JOB_FAILED is returned.
 */
LOCAL_INLINE Fee_Rb_JobResultType_ten Fee_Prv_ProcChunkJobDo(
        Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst,
        Fee_Prv_Proc_tst *                    proc_pst,
        Fee_Rb_JobType_ten                    jobType_en)
{
    /* Return variable */
    Fee_Rb_JobResultType_ten    result_en;

    /* Waiting for next chunk? */
    if(proc_pst->isWaitForNextChunk_b)
    {
        /* Re-start timing measurement for next chunk */
        Fee_Prv_DbgWatchStart(deviceConfigTable_pcst->deviceName_en, Fee_Prv_DbgWatchGetTmrForJob(jobType_en));

        /* Next chunk or termination request has been received */
        proc_pst->isWaitForNextChunk_b = FALSE;
        result_en = FEE_RB_JOB_PENDING;

        /* Restart chunk-wise job */
        switch(jobType_en)
        {
            case FEE_RB_JOBTYPE_READ_E:  Fee_Prv_FsIfRead(deviceConfigTable_pcst);                break;
            case FEE_RB_JOBTYPE_WRITE_E: Fee_Prv_FsIfWrite(deviceConfigTable_pcst);               break;
            default:                     result_en = FEE_RB_JOB_FAILED;                           break;
        }
    }
    else
    {
        /* Continue ongoing chunk-wise job */
        switch(jobType_en)
        {
            case FEE_RB_JOBTYPE_READ_E:  result_en = Fee_Prv_FsIfReadDo(deviceConfigTable_pcst);  break;
            case FEE_RB_JOBTYPE_WRITE_E: result_en = Fee_Prv_FsIfWriteDo(deviceConfigTable_pcst); break;
            default:                     result_en = FEE_RB_JOB_FAILED;                           break;
        }
    }

    return(result_en);
}

/*
 * \brief   Checks if a (chunk-wise) job is done
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table, for which the job has to be checked
 * \param   result_en                Job result
 *
 * \return  Job done status
 * \retval  TRUE   if job is done (in case of a chunk-wise job, this means all chunks are done)
 * \retval  FALSE  otherwise
 */
LOCAL_INLINE boolean Fee_Prv_ProcIsJobDone(
        Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst,
        Fee_Rb_JobResultType_ten result_en)
{
    /* Return variable */
    boolean    isDone_b;

    /* Local variable */
    Fee_Prv_Proc_tst *    proc_pst = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;

    /* Chunk-wise job? */
    if(Fee_Prv_JobIsChunkActv(deviceConfigTable_pcst))
    {
        /* Callback into chunk-unit */
        Fee_Prv_ChunkDoneCbk(deviceConfigTable_pcst, result_en);

        /* Current chunk is done? */
        if(FEE_RB_BLOCK_PART_OK == Fee_Prv_JobGetResult(deviceConfigTable_pcst->deviceName_en, FEE_RB_USER_CHUNK_E))
        {
            /* Update status of chunk-wise job, and wait for next chunk */
            Fee_Prv_JobChunkEnd(deviceConfigTable_pcst);
            proc_pst->isWaitForNextChunk_b = TRUE;
            isDone_b = FALSE;
        }
        else
        {
            /* Chunk-wise job is done */
            proc_pst->isWaitForNextChunk_b = FALSE;
            isDone_b = TRUE;
        }
    }
    else
    {
        /* Normal job is done */
        isDone_b = TRUE;
    }

    return(isDone_b);
}
# endif

/*
 * \brief   Processes the current external job based on the selected job descriptor.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table, for which the job has to be processed
 */
LOCAL_INLINE void Fee_Prv_ProcProcessExtJob(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Rb_JobResultType_ten    result_en;
    Fee_Prv_Proc_tst *          proc_pst     = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;
    Fee_Rb_JobType_ten          jobType_en   = Fee_Prv_JobGetActvJobType(deviceConfigTable_pcst);

    switch(jobType_en)
    {
        case FEE_RB_JOBTYPE_READ_E:             result_en = Fee_Prv_ProcChunkReadDo(deviceConfigTable_pcst, proc_pst, jobType_en);      break;
        case FEE_RB_JOBTYPE_WRITE_E:            result_en = Fee_Prv_ProcChunkWriteDo(deviceConfigTable_pcst, proc_pst, jobType_en);     break;
        case FEE_RB_JOBTYPE_INVALIDATE_E:       result_en = Fee_Prv_FsIfInvalidateBlockDo(deviceConfigTable_pcst);                      break;
        case FEE_RB_JOBTYPE_BLOCKMAINTENANCE_E: result_en = Fee_Prv_FsIfRbMaintainBlockDo(deviceConfigTable_pcst);                      break;
        case FEE_RB_SYSTEM_JOBTYPE_STOP_MODE_E: result_en = Fee_Prv_ProcEnterStopModeDo(proc_pst);                                      break;
        case FEE_RB_JOBTYPE_WAIT_NEXT_CHUNK_E:  result_en = FEE_RB_JOB_PENDING ;                                                        break;
        default:                                result_en = Fee_Prv_FsIfRbSystemJobDo(deviceConfigTable_pcst, jobType_en);              break;
    }

    /* Job done? */
    if(FEE_RB_JOB_PENDING != result_en)
    {
# if(defined(FEE_PRV_CFG_RB_CHUNK_JOBS) && (TRUE == FEE_PRV_CFG_RB_CHUNK_JOBS))
        /* A chunk-wise job is only done after all chunks have been processed; is this the case here? */
        if(Fee_Prv_ProcIsJobDone(deviceConfigTable_pcst, result_en))
# endif
        {
            /* Reset the environment after the job has been finished */
            proc_pst->driverState_ven = FEE_PRV_PROC_DRIVERSTATE_IDLE_E;
            Fee_Prv_JobEnd(deviceConfigTable_pcst, result_en);

            /* Background enabled but sleeping? */
            if(FEE_PRV_PROC_BACKGROUNDSTATE_ENABLED_SLEEPING == proc_pst->backgroundState_en)
            {
                /* Activate background */
                proc_pst->backgroundState_en = FEE_PRV_PROC_BACKGROUNDSTATE_ENABLED_ACTIVE;
            }
        }

        /* Stop timing measurement for job or current chunk */
        Fee_Prv_DbgWatchStop(deviceConfigTable_pcst->deviceName_en, Fee_Prv_DbgWatchGetTmrForJob(jobType_en), FALSE);
    }

    return;
}

/*
 * \brief   Covers all actions to be done when the driver state is idle.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which main function has to called
 */
LOCAL_INLINE void Fee_Prv_ProcProcessIdle(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Rb_JobResultType_ten result_en = FEE_RB_JOB_PENDING;
    Fee_Prv_Proc_tst *       proc_pst  = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;

# if (FEE_PRV_CFG_AR_RELEASE_MAJOR_VERSION == 4) && (FEE_PRV_CFG_AR_RELEASE_MINOR_VERSION >= 4) && (STD_ON == FEE_PRV_CFG_SET_MODE_SUPPORTED)
    /* From Autosar 4.4 and newer
     * Check if the user has requested a driver mode change
     * According to the prerequisites that MemStack has to run on >= 32 bit controllers and data are always aligned,
     * atomic access is guaranteed by the hardware for 32 bit memory access
     */
    MemIf_ModeType      driverModeReqdLocal_en = proc_pst->driverModeReqd_en;

    if(proc_pst->driverModeActv_en != driverModeReqdLocal_en)
    {
        /* Change driver mode */
        Fee_Prv_ProcSetDriverMode(deviceConfigTable_pcst, driverModeReqdLocal_en);
    }
# endif
    /* Is a new user job available? */
    if(Fee_Prv_JobIsThereActvUser(deviceConfigTable_pcst))
    {
        /* Yes -> Start the job processing in the Fs and set the appropriate driver state */
        Fee_Prv_ProcStartExtJob(deviceConfigTable_pcst);
        proc_pst->driverState_ven = FEE_PRV_PROC_DRIVERSTATE_EXT_JOB_E;
    }
    else
    {
        /* No -> Check whether any maintenance functions can be executed in the background */
        if(FEE_PRV_PROC_BACKGROUNDSTATE_ENABLED_ACTIVE == proc_pst->backgroundState_en)
        {
            /* Call Fs-specific background function */
            result_en = Fee_Prv_FsIfBackground(deviceConfigTable_pcst);

            /* Fs-specific background function has already completed successfully? */
            if(FEE_RB_JOB_OK == result_en)
            {
                /* Put background operations back to sleep */
                proc_pst->backgroundState_en = FEE_PRV_PROC_BACKGROUNDSTATE_ENABLED_SLEEPING;
            }
            /* Fs-specific background function not yet complete? */
            else if(FEE_RB_JOB_PENDING == result_en)
            {
                /* Continue the Fs-specific background processing */
                proc_pst->driverState_ven = FEE_PRV_PROC_DRIVERSTATE_BACKGROUND_E;
            }
            /* Fs-specific background function has failed? */
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* FEE_PRV_BACKGROUNDSTATE_DISABLED / FEE_PRV_BACKGROUNDSTATE_ENABLED_SLEEPING : Do nothing */
        }
    }

    return;
}

/*
 * \brief   Covers all background actions.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which main function has to called
 */
LOCAL_INLINE void Fee_Prv_ProcProcessBackground(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    /* Return variable */
    Fee_Rb_JobResultType_ten result_en;
    Fee_Prv_Proc_tst *       proc_pst = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;

    /* Is a new job available or background operations just disabled and background cancel not yet started? */
    if(((Fee_Prv_JobIsThereActvUser(deviceConfigTable_pcst)) || (FEE_PRV_PROC_BACKGROUNDSTATE_DISABLED == proc_pst->backgroundState_en)) &&
       (!proc_pst->backgroundCancelOngoing_b))
    {
        /* Cancel background activity */
        Fee_Prv_FsIfRbCancelBackground(deviceConfigTable_pcst);
        proc_pst->backgroundCancelOngoing_b = TRUE;
    }

    /* Call Fs-specific background function */
    result_en = Fee_Prv_FsIfBackground(deviceConfigTable_pcst);

    if(FEE_RB_JOB_PENDING != result_en)
    {
        proc_pst->backgroundCancelOngoing_b = FALSE;
        /* Background job is finished -> switch the driver state to idle */
        proc_pst->driverState_ven = FEE_PRV_PROC_DRIVERSTATE_IDLE_E;
    }

    return;
}

/*
 * \brief   Calculates the current Fee working state.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which main function has to called
 *
 * \return  Returns the ongoing Fee activity.
 */
LOCAL_INLINE Fee_Rb_WorkingStateType_ten Fee_Prv_ProcCalcWorkingState(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Rb_WorkingStateType_ten stRetVal_en;
    Fee_Prv_ProcDriverState_ten driverState_en     = deviceConfigTable_pcst->feeData_pst->procData_st.proc_st.driverState_ven;
    boolean                     stopModeActive_b   = deviceConfigTable_pcst->feeData_pst->procData_st.proc_st.stopModeActive_b;
    Fee_Rb_JobType_ten          jobType_en         = Fee_Prv_JobGetActvJobType(deviceConfigTable_pcst);

    switch(driverState_en)
    {
        case FEE_PRV_PROC_DRIVERSTATE_UNINIT_E:
        case FEE_PRV_PROC_DRIVERSTATE_INIT_E:
        {
            stRetVal_en = FEE_RB_IDLE_E;

        }
        break;

        case FEE_PRV_PROC_DRIVERSTATE_IDLE_E:
        {
            if(stopModeActive_b)
            {
                stRetVal_en = FEE_RB_STOPMODE_E;
            }
            else
            {
                stRetVal_en = FEE_RB_IDLE_E;
            }
        }
        break;

        case FEE_PRV_PROC_DRIVERSTATE_BACKGROUND_E:
        {
            /* Check if background erase operation is ongoing */
            if(Fee_Prv_FsIfRbIsSectorEraseOngoing(deviceConfigTable_pcst))
            {
                stRetVal_en = FEE_RB_SECTOR_ERASE_E;
            }
            /* Check if background soft reorganisation operation is ongoing */
            else if (Fee_Prv_FsIfRbIsSoftReorgOngoing(deviceConfigTable_pcst))
            {
                stRetVal_en = FEE_RB_SOFT_SECTOR_REORG_MODE_E;
            }
            else
            {
                stRetVal_en = FEE_RB_IDLE_E;
            }
        }
        break;

        case FEE_PRV_PROC_DRIVERSTATE_EXT_JOB_E:
        {
            switch(jobType_en)
            {
                case FEE_RB_JOBTYPE_READ_E:
                {
                    stRetVal_en = FEE_RB_READ_MODE_E;
                }
                break;

                case FEE_RB_SYSTEM_JOBTYPE_STOP_MODE_E:
                {
                    /* Stop mode is completed only when the main function is called once and the state machine is put
                     * to Stop mode. Till then, return that Fee is IDLE (similar to other system job operations below). */
                    if(stopModeActive_b)
                    {
                        stRetVal_en = FEE_RB_STOPMODE_E;
                    }
                    else
                    {
                        stRetVal_en = FEE_RB_IDLE_E;
                    }
                }
                break;

                case FEE_RB_JOBTYPE_WRITE_E:
                case FEE_RB_JOBTYPE_INVALIDATE_E:
                case FEE_RB_JOBTYPE_BLOCKMAINTENANCE_E:
                case FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORG_E:
                case FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORGFAT_E:
                case FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORGDAT_E:
                default:
                {
                    /* Check for hard reorg, because this can happen during a write, invalidate or maintain job */
                    if(Fee_Prv_FsIfRbIsHardReorgOngoing(deviceConfigTable_pcst))
                    {
                        stRetVal_en = FEE_RB_HARD_SECTOR_REORG_MODE_E;
                    }
                    else if(Fee_Prv_FsIfRbIsSectorEraseOngoing(deviceConfigTable_pcst))
                    {
                        stRetVal_en = FEE_RB_SECTOR_ERASE_E;
                    }
                    else
                    {
                        switch(jobType_en)
                        {
                            case FEE_RB_JOBTYPE_WRITE_E:
                            {
                                /* All FSx */
                                stRetVal_en = FEE_RB_WRITE_MODE_E;
                            }
                            break;

                            case FEE_RB_JOBTYPE_INVALIDATE_E:
                            {
                                /* All FSx */
                                stRetVal_en = FEE_RB_INVALIDATE_MODE_E;
                            }
                            break;

                            case FEE_RB_JOBTYPE_BLOCKMAINTENANCE_E:
                            {
                                /* FS1x + FS3 only */
                                stRetVal_en = FEE_RB_MAINTAIN_MODE_E;
                            }
                            break;

                            /* FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORG_E    - Fs1x only */
                            /* FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORGFAT_E - Fs2  only */
                            /* FEE_RB_SYSTEM_JOBTYPE_TRIGGERREORGDAT_E - Fs2  only */
                            default:
                            {
                                stRetVal_en = FEE_RB_IDLE_E;
                            }
                            break;
                        }
                    }
                }
                break;
            }
        }
        break;

        default:
        {
            stRetVal_en = FEE_RB_IDLE_E;
        }
        break;
    }

    return(stRetVal_en);
}

/*
 * \brief   The internal main function does the actual work of the Fee.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which main function has to called
 */
LOCAL_INLINE void Fee_Prv_ProcMainFunctionIntern(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    /*
     * Measure main function execution time.
     * Attention: The result is not the actual time the main function really needed since it can be interrupted.
     *            Nevertheless it is a good indicator for debugging purposes.
     */
    Fee_Prv_ProcData_tst * procData_pst = &deviceConfigTable_pcst->feeData_pst->procData_st;
    Fee_Prv_Proc_tst *     proc_pst     = &procData_pst->proc_st;

    /* avoid warning when debug feature is deactivated */
    Fee_Prv_DbgWatchStart(deviceConfigTable_pcst->deviceName_en, FEE_PRV_DBG_TIMER_PROCMAIN_E);

    /* Reset the effort limitation */
    Fee_Prv_LibEffortReset(deviceConfigTable_pcst);

    /* Is an external job request already accepted? (condition can only apply in idle or background state) */
    if(!Fee_Prv_JobIsThereActvUser(deviceConfigTable_pcst))
    {
        /* No -> Check for a new request */
        Fee_Prv_JobCheckAndSetNextJob(deviceConfigTable_pcst);
    }

    /* Based on the driver state let the Fs process external or internal jobs */
    switch(proc_pst->driverState_ven)
    {
        case FEE_PRV_PROC_DRIVERSTATE_IDLE_E:       Fee_Prv_ProcProcessIdle(deviceConfigTable_pcst);       break;
        case FEE_PRV_PROC_DRIVERSTATE_BACKGROUND_E: Fee_Prv_ProcProcessBackground(deviceConfigTable_pcst); break;
        case FEE_PRV_PROC_DRIVERSTATE_EXT_JOB_E:    Fee_Prv_ProcProcessExtJob(deviceConfigTable_pcst);     break;
        default:                                    /* Do nothing */                                       break;
    }

    /* Background enabled? */
    if(FEE_PRV_PROC_BACKGROUNDSTATE_DISABLED != proc_pst->backgroundState_en)
    {
        /* Execute cyclic debugging actions */
        Fee_Prv_DbgMainFunction(deviceConfigTable_pcst);
    }

    /* Call working state calculation and set private copy */
    procData_pst->procWorkingState_en = Fee_Prv_ProcCalcWorkingState(deviceConfigTable_pcst);

    /* Stop timing measurement of main function */
    Fee_Prv_DbgWatchStop(deviceConfigTable_pcst->deviceName_en, FEE_PRV_DBG_TIMER_PROCMAIN_E, FALSE);

    return;
}

/*
 * \brief   Checks global flag for re-entrant invocation of main function, and reports such error to DET if enabled.
 *
 * \param   deviceName_en    Device instance for which lock has to be acquired
 * \param   procLock_pst     Pointer to the variables related to the reentrancy detection
 *
 * \return  Availability of lock
 * \return  TRUE                if lock is available, i.e. no re-entrant invocation of main function.
 * \return  FALSE               if lock is not available, i.e. re-entrant invocation of main function.
 */
LOCAL_INLINE boolean Fee_Prv_ProcTryToGetLock(Fee_Rb_DeviceName_ten deviceName_en, Fee_Prv_ProcLock_tst * procLock_pst)
{
    /* Return variable */
    boolean isLockAvailable_b = FALSE;

# if(FEE_RB_USE_PROTECTION != FALSE)
    /* Disable interrupts */
    SchM_Enter_Fee_Order();
# endif

    /* Is the main function not invoked yet? */
    if(!procLock_pst->flgUsedSema_vb)
    {
        /* Reserve it and allow its execution */
        procLock_pst->flgUsedSema_vb = TRUE;
        isLockAvailable_b = TRUE;
    }

# if(FEE_RB_USE_PROTECTION != FALSE)
    /* Enable interrupts */
    SchM_Exit_Fee_Order();
# endif

    /* Re-entrant invocation of main function? */
    if(!isLockAvailable_b)
    {
        /* Set the debug flag for the detection of reentrant main function */
        procLock_pst->dbgReentrantMainFunction_vb = TRUE;

        Fee_Prv_LibDetReport(deviceName_en, FEE_SID_MAINFUNCTION, FEE_E_BUSY);
    }

# if(FEE_RB_USE_PROTECTION == FALSE)
    /* Protection disabled, only detection by default */
    isLockAvailable_b = TRUE;
# endif

    return(isLockAvailable_b);
}

/**
 * \brief   Resets the flag having re-entrant information.
 *
 * \param   procLock_pst   Pointer to the variables related to the reentrancy detection
 */
LOCAL_INLINE void Fee_Prv_ProcReleaseLock(Fee_Prv_ProcLock_tst * procLock_pst)
{
    /* Reset the access flag */
    procLock_pst->flgUsedSema_vb = FALSE;

    return;
}

/*
 * \brief   The main function does the actual work of the Fee. The more often it is scheduled the faster the Fee is.
 *
 * \param   deviceName_en       Device instance for which job has to be performed
 */
/* HIS METRIC LEVEL VIOLATION IN Fee_Rb_Idx_MainFunction: This warning shows up only in multi instance combination of FS1 + FS2. FS1 is obsolete and adapting the code would increase complexity+code size and reduce maintainability. */
void Fee_Rb_Idx_MainFunction(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Check parameter */
    if(E_OK == Fee_Prv_OrderDetCheckDeviceName(deviceName_en, FEE_SID_MAINFUNCTION))
    {
        /* Below code is needed only for multi instance with Fee1.0.
         * To be removed when either Fee1.0 is removed or multi instance support with Fee1.0 is removed. */
#  if (defined( RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
        if (Fee_Rb_DeviceName == deviceName_en)
        {
            /* This request is for device index = 0, this means route this request to Fee1.0 */
            Fee_MainFunction();
        }
        else
#  endif
        {
            /* Local flag to store the lock accuring status */
            boolean flgUsed_b = FALSE;
            /* Get the pointer to config data model */
            Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
            Fee_Prv_ProcData_tst *               procData_pst           = &deviceConfigTable_pcst->feeData_pst->procData_st;
            Fee_Prv_Proc_tst *                   proc_pst               = &procData_pst->proc_st;
            Fee_Prv_ProcLock_tst *               procLock_pst           = &procData_pst->procLock_st;
#  if (defined(FEE_PRV_CFG_RB_CANCEL) && (TRUE == FEE_PRV_CFG_RB_CANCEL))
            Fls_Rb_FlsDevice_ten                 flsDevId_en;
            uint32                               flsAdr_u32;
#  endif
            /* Check for re-entrant invocation of main function */
            flgUsed_b = Fee_Prv_ProcTryToGetLock(deviceName_en, procLock_pst);

            /* Lock obtained, i.e. no re-entrant invocation of main function? */
            if(flgUsed_b)
            {
                if(!proc_pst->stopModeActive_b)
                {
#  if (defined(FEE_PRV_CFG_RB_CANCEL) && (TRUE == FEE_PRV_CFG_RB_CANCEL))
                    /* Check if the ongoing job has to be canceled */
                    if(proc_pst->isCnclAndRstrtFsm_b)
                    {
                        /* Reset the state machine only if there was some ongoing job */
                        if((FEE_PRV_PROC_DRIVERSTATE_BACKGROUND_E == proc_pst->driverState_ven) ||
                           (FEE_PRV_PROC_DRIVERSTATE_EXT_JOB_E == proc_pst->driverState_ven))
                        {
                            /* Cancel the job by resetting the state machines. The order of cancel does not matter. But
                             * kept in same order as done during Init */
                            Fee_Prv_LibCancel(deviceConfigTable_pcst);
                            Fee_Prv_MediumCancel(deviceConfigTable_pcst);
                            Fee_Prv_JobCancelActv(deviceConfigTable_pcst, proc_pst->user_en);
                            /* Reset the state machine to be ready to start with new job */
                            proc_pst->isWaitForNextChunk_b = FALSE;
                            proc_pst->driverState_ven = FEE_PRV_PROC_DRIVERSTATE_IDLE_E;
                            proc_pst->backgroundCancelOngoing_b = FALSE;

                            /* It is better to cancel the FeeFsx before cancelling the Fls layer. Doing it other way
                             * could lead to Fls returning MEMIF_JOB_CANCELED and then the FeeFsx needs to be updated
                             * to check and handle for this return value. */
                            Fee_Prv_FsIfCancelJob(deviceConfigTable_pcst);

                            /* Call Fls cancel only if Fls job has been placed, otherwise it might lead to unnecessary
                             * behaviour like DET error reporting. */
                            /* Address is needed when multi instance Cancel API is used, so that the user can be
                             * identified by Fls. Any address within the flash instance that is currently being canceled
                             * is ok and need not match with the current write address or read address. */
                            flsDevId_en = Fls_Rb_GetDeviceIdFromDrvIndex(deviceConfigTable_pcst->deviceIdx_u8);
                            flsAdr_u32 = Fee_Prv_FsIfGetEmulationStartAdr(deviceName_en);
                            if(MEMIF_JOB_PENDING == Fee_Prv_Lib_Fls_GetJobResult(flsDevId_en, flsAdr_u32))
                            {
                                Fee_Prv_Lib_Fls_Cancel(flsDevId_en, flsAdr_u32);
                            }
                        }

                        proc_pst->user_en = FEE_RB_USER_MAX_E;
                        proc_pst->isCnclAndRstrtFsm_b = FALSE;
                    }
#  endif

                    /* Main function can be invoked */
                    Fee_Prv_ProcMainFunctionIntern(deviceConfigTable_pcst);
                }

                /* Release the lock */
                Fee_Prv_ProcReleaseLock(procLock_pst);
            }
        }
    }
    return;
}

/*
 * \brief   This function invokes the Fee Mainfunction and its lower layers.
 *
 * \param   deviceName_en       Device instance for which job has to be performed
 */
void Fee_Rb_Idx_MainFunctionAndDependency(Fee_Rb_DeviceName_ten deviceName_en)
{
    /* Check parameter */
    if(E_OK == Fee_Prv_OrderDetCheckDeviceName(deviceName_en, FEE_SID_MAINFUNCTION))
    {
        /* Below code is needed only for multi instance with Fee1.0.
         * To be removed when either Fee1.0 is removed or multi instance support with Fee1.0 is removed. */
# if (defined( RBA_FEEFS1_PRV_CFG_ENABLED) && (TRUE ==  RBA_FEEFS1_PRV_CFG_ENABLED))
        if (Fee_Rb_DeviceName == deviceName_en)
        {
            /* This request is for device index = 0, this means route this request to Fls MainFunction */
            Fee_MainFunction();
            Fls_Rb_MainFunctionAndDependency();
        }
        else
# endif
        {
            /* Get the pointer to config data model */
            Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst = Fee_Prv_ConfigGetAdrOfConfigTableFromDeviceName(deviceName_en);
            Fee_Prv_Medium_tst const *            medium_pst             = &deviceConfigTable_pcst->feeData_pst->mediumData_st.medium_st;

            Fee_Rb_Idx_MainFunction(deviceName_en);
            Fee_Prv_Lib_Fls_MainFunction(medium_pst->flsDevId_en);
        }
    }

    return;
}

# if (defined(FEE_PRV_CFG_RB_CANCEL) && (TRUE == FEE_PRV_CFG_RB_CANCEL))
/**
 * \brief   Cancel ongoing active job, to be ready to process new request.
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table for which the job has to be finished.
 * \param   user_en                 User which has requested for the ongoing job to be canceled
 *
 * \return  none
 */
void Fee_Prv_ProcEnterCancel(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst, Fee_Rb_User_ten user_en)
{
    Fee_Prv_Proc_tst * proc_pst = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;

    SchM_Enter_Fee_Order();
    /* Set the flag to cancel the state machine and be ready to start the new job */
    proc_pst->isCnclAndRstrtFsm_b = TRUE;
    proc_pst->user_en = user_en;
    SchM_Exit_Fee_Order();
}

/**
 * \brief   Undo cancel of ongoing job
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table for which the job has to be finished.
 *
 * \return  none
 */
void Fee_Prv_ProcExitCancel(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst)
{
    Fee_Prv_Proc_tst * proc_pst = &deviceConfigTable_pcst->feeData_pst->procData_st.proc_st;

    SchM_Enter_Fee_Order();
    /* Reset the flag to cancel the state machine */
    proc_pst->user_en = FEE_RB_USER_MAX_E;
    proc_pst->isCnclAndRstrtFsm_b = FALSE;
    SchM_Exit_Fee_Order();
}
# endif

#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif
