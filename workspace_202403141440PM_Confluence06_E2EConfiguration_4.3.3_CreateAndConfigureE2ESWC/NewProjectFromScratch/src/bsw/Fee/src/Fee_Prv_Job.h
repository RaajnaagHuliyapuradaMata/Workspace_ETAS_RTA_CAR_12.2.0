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
 * The Job unit stores the received orders in internal job slots.
 * The main function will poll jobs from the Job unit and inform the Job unit if a job is finished.
 */

#ifndef FEE_PRV_JOB_H
#define FEE_PRV_JOB_H

#include "Std_Types.h"
#include "Fee_Prv_Cfg.h"

/* Disable the Fee common part when not needed */
# if(defined(FEE_PRV_CFG_COMMON_ENABLED) && (TRUE == FEE_PRV_CFG_COMMON_ENABLED))

#include "MemIf_Types.h"
#include "Fee_Cfg.h"
#include "Fee_Prv_Config.h"
#include "Fee_PrvTypes.h"
#include "Fee_Prv_JobTypes.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"

extern void                     Fee_Prv_JobCheckAndSetNextJob(
                                               Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
extern void                     Fee_Prv_JobEnd(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst,
                                               Fee_Rb_JobResultType_ten              result_en);
extern void                     Fee_Prv_JobChunkEnd(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
extern void                     Fee_Prv_JobUpdateChunkStatus(
                                                    Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst,
                                                    Fee_Rb_JobResultType_ten              result_en,
                                                    uint32                                nrBytProc_u32);

extern Fee_Rb_User_ten          Fee_Prv_JobGetNxtUserJob(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
extern Std_ReturnType           Fee_Prv_JobEnterHighSpeedMode(
                                                    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                                    Fee_Rb_User_ten                      user_en);
extern Std_ReturnType           Fee_Prv_JobExitHighSpeedMode(
                                                    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                                    Fee_Rb_User_ten                      user_en);

extern Std_ReturnType           Fee_Prv_JobPut(Fee_Rb_DeviceName_ten        deviceName_en,
                                               uint8                        apiId_u8,
                                               Fee_Prv_JobDesc_tst const *  jobDesc_pcst);
extern boolean                  Fee_Prv_JobCancelInQue(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                                       Fee_Rb_User_ten                      user_en);
extern void                     Fee_Prv_JobCancelActv(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst,
                                                      Fee_Rb_User_ten                       user_en);
extern Fee_Rb_JobResultType_ten Fee_Prv_JobGetResult(Fee_Rb_DeviceName_ten deviceName_en, Fee_Rb_User_ten user_en);

#  if((TRUE == FEE_PRV_CFG_RB_SET_AND_GET_JOB_MODE) || (STD_ON == FEE_PRV_CFG_SET_MODE_SUPPORTED)  || (TRUE == FEE_PRV_CFG_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API))
extern Fee_Rb_JobMode_ten       Fee_Prv_JobGetJobMode(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                                      Fee_Rb_JobType_ten                   jobType_en);
extern void                     Fee_Prv_JobSetJobMode(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                                      Fee_Rb_JobType_ten                   jobType_en,
                                                      Fee_Rb_JobMode_ten                   jobMode_en);
#  endif

extern void                     Fee_Prv_JobSetModesBasedOnDriverMode(
                                                    Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                                    MemIf_ModeType                       mode);


/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
*/
LOCAL_INLINE void               Fee_Prv_JobInit(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE boolean            Fee_Prv_JobIsBlkMaintainAllwd(
                                                   Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE boolean            Fee_Prv_JobIsInQue(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst,
                                                   Fee_Rb_User_ten                       user_en);
LOCAL_INLINE boolean            Fee_Prv_JobIsThereActvUser(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst);
LOCAL_INLINE boolean            Fee_Prv_JobIsReadOngoing(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE boolean            Fee_Prv_JobIsWriteOngoing(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Rb_JobType_ten Fee_Prv_JobGetActvJobType(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Prv_JobDesc_tst const * Fee_Prv_JobGetActvDesc(
                                                         Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE Fee_Rb_User_ten    Fee_Prv_JobGetActvUser(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE uint32             Fee_Prv_JobGetChunkNrBytProc(
                                                         Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE boolean            Fee_Prv_JobIsChunkActv(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE boolean            Fee_Prv_JobIsChunkCancldByOtherUser(
                                                       Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE boolean            Fee_Prv_JobIsPsblToCancel(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst,
                                                          Fee_Rb_User_ten                       user_en);
LOCAL_INLINE boolean            Fee_Prv_JobIsWaitingForNextChunk(
                                                        Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst);
LOCAL_INLINE boolean            Fee_Prv_JobIsFirstPart(Fee_Prv_JobDesc_tst const * xJob_pcst);
LOCAL_INLINE boolean            Fee_Prv_JobIsLastPart(Fee_Prv_JobDesc_tst const * xJob_pcst);

#  if!((TRUE == FEE_PRV_CFG_RB_SET_AND_GET_JOB_MODE) || (STD_ON == FEE_PRV_CFG_SET_MODE_SUPPORTED)  || (TRUE == FEE_PRV_CFG_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API))
LOCAL_INLINE Fee_Rb_JobMode_ten    Fee_Prv_JobGetJobMode(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst, Fee_Rb_JobType_ten jobType_en);
#  endif


/*
 **********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************
*/
/**
 * \brief   Function called during Fee_Init(). Initializes all the variables of this unit.
 * To save code size + delay due to calling of function with parameter, the function is made inline.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 */
LOCAL_INLINE void Fee_Prv_JobInit(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    Fee_Prv_JobData_tst *    jobData_pst = &deviceConfigTable_pcst->feeData_pst->jobData_st;
    uint8_least              idxLoop_qu8;

    /* Init the job queue */
    for(idxLoop_qu8 = 0u; idxLoop_qu8 < (uint8_least)FEE_RB_USER_MAX_E; idxLoop_qu8++)
    {
        jobData_pst->jobs_ast[idxLoop_qu8].type_en = FEE_RB_JOBTYPE_MAX_E;
        jobData_pst->results_aen[idxLoop_qu8]      = FEE_RB_JOB_OK;
    }
    jobData_pst->allwdUser_en = FEE_RB_USER_MAX_E;
    jobData_pst->actvJobDesc_st.type_en = FEE_RB_JOBTYPE_MAX_E;
    jobData_pst->actvUser_en = FEE_RB_USER_MAX_E;
    jobData_pst->isChunkJobCncldByOthUser_b = FALSE;

    /* set the mode of different jobs to default mode */
    for(idxLoop_qu8 = 0; idxLoop_qu8 < (uint8_least)FEE_RB_JOBTYPE_MAX_E; idxLoop_qu8++)
    {
        jobData_pst->jobMode_en[idxLoop_qu8] = FEE_RB_ALLJOBS_ALLSTEPS_E;
    }
}

/**
 * \brief   Function checks if block maintain jobs are allowed to be processed
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table for which check has to be performed
 *
 * \retval  TRUE  : Maintain jobs can be accepted and executed
 * \retval  FALSE : Maintain jobs are disabled, requests should be rejected
 */
LOCAL_INLINE boolean Fee_Prv_JobIsBlkMaintainAllwd(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    return(FEE_RB_BLOCKMAINTENANCE_DISABLE_E != deviceConfigTable_pcst->feeData_pst->jobData_st.jobMode_en[FEE_RB_JOBTYPE_BLOCKMAINTENANCE_E]);
}

/**
 * \brief   Function checks if there is any job in the queue for the external user
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table
 * \param   user_en                  User for which the job has to be checked
 *
 * \retval  TRUE  : There is a job in queue for the passed external user
 * \retval  FALSE : There is no job in queue for the passed external user
 */
LOCAL_INLINE boolean Fee_Prv_JobIsInQue(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst,
                                        Fee_Rb_User_ten                       user_en)
{
    return(FEE_RB_JOBTYPE_MAX_E != deviceConfigTable_pcst->feeData_pst->jobData_st.jobs_ast[user_en].type_en);
}

/**
 * \brief   Function checks if there is any external user job being processed
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 *
 * \retval  TRUE  : Some external user job is being processed
 * \retval  FALSE : Currently there is no external user job being processed
 */
LOCAL_INLINE boolean Fee_Prv_JobIsThereActvUser(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    return(FEE_RB_USER_MAX_E != deviceConfigTable_pcst->feeData_pst->jobData_st.actvUser_en);
}

/**
 * \brief   Function that returns TRUE if the current job being processed is a read job.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 *
 * \retval  TRUE  : Currently active job is a read job
 * \retval  FALSE : Currently active job is not a read job
 *
 * \attention Function should be called only when there is an already ongoing active job being processed.
 *  Example usage is when waiting for next chunk and accepting next chunk only for the ongoing job type.
 */
LOCAL_INLINE boolean Fee_Prv_JobIsReadOngoing(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    /* NULL_PTR != xJob_st.bfr_pu8 indicates that a read job is ongoing */
    return(NULL_PTR != deviceConfigTable_pcst->feeData_pst->jobData_st.actvJobDesc_st.bfr_pu8);
}

/**
 * \brief   Function that returns TRUE if the current job being processed is a write job.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 *
 * \retval  TRUE  : Currently active job is a write job
 * \retval  FALSE : Currently active job is not a write job
 *
 * \attention Function should be called only when there is an already ongoing active job being processed.
 *  Example usage is when waiting for next chunk and accepting next chunk only for the ongoing job type.
 */
LOCAL_INLINE boolean Fee_Prv_JobIsWriteOngoing(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    /* NULL_PTR != bfr_pcu8.bfr_pu8 indicates that a write job is ongoing */
    return(NULL_PTR != deviceConfigTable_pcst->feeData_pst->jobData_st.actvJobDesc_st.bfr_pcu8);
}

/**
 * \brief   Function returns the currently active job type
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 *
 * \return  Fee_Rb_JobType_ten       Currently active user job type
 */
LOCAL_INLINE Fee_Rb_JobType_ten Fee_Prv_JobGetActvJobType(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    return(deviceConfigTable_pcst->feeData_pst->jobData_st.actvJobDesc_st.type_en);
}

/**
 * \brief   Returns the details of job which is currently active
 *
 * \param   deviceConfigTable_pcst Pointer to the config table
 *
 * \return  Pointer (Read only) to job which is currently active
 */
LOCAL_INLINE Fee_Prv_JobDesc_tst const * Fee_Prv_JobGetActvDesc(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    return(&deviceConfigTable_pcst->feeData_pst->jobData_st.actvJobDesc_st);
}

/**
 * \brief   Returns the currently active user which is being processed.
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table.
 *
 * \return  User which is currently active
 */
LOCAL_INLINE Fee_Rb_User_ten Fee_Prv_JobGetActvUser(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    return (deviceConfigTable_pcst->feeData_pst->jobData_st.actvUser_en);
}

/**
 * \brief   Returns the number of bytes that have been processed in the current/last chunk including chunk header
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table.
 *
 * \return  Number of bytes that have been processed in the current/last chunk
*/
LOCAL_INLINE uint32 Fee_Prv_JobGetChunkNrBytProc(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    /* Return number of bytes that have been processed in the current/last chunk */
    return(deviceConfigTable_pcst->feeData_pst->jobData_st.jobs_ast[FEE_RB_USER_CHUNK_E].bytDone_un.nrBytProc_u32);
}

/**
 * \brief   Function returns TRUE if the active ongoing job is a chunk job
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 *
 * \retval  TRUE  : Currently active job is from "Chunk" requestor
 * \retval  FALSE : Currently active job is not from "Chunk" requestor
 *
 * \attention Function should be called only when there is an already ongoing active job being processed.
 */
LOCAL_INLINE boolean Fee_Prv_JobIsChunkActv(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    return(deviceConfigTable_pcst->feeData_pst->jobData_st.actvJobDesc_st.isChunkJob_b);
}

/**
 * \brief   Function that returns TRUE if the chunk job was canceled by another user.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 *
 * \retval  TRUE  : Currently active chunk job was canceled by another user
 * \retval  FALSE : Currently active chunk job was canceled by chunk user
 */
LOCAL_INLINE boolean Fee_Prv_JobIsChunkCancldByOtherUser(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    return(deviceConfigTable_pcst->feeData_pst->jobData_st.isChunkJobCncldByOthUser_b);
}

/**
 * \brief   Function that returns TRUE if the job for the passed user can be canceled or not.
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 *
 * \retval  TRUE  : User job can be canceled
 * \retval  FALSE : User job can not be canceled
 */
LOCAL_INLINE boolean Fee_Prv_JobIsPsblToCancel(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst,
                                               Fee_Rb_User_ten                       user_en)
{
    boolean isCancelPsbl_b;

    switch(user_en)
    {
        /* Currently cancel feature is supported only for NvM user */
        case FEE_RB_USER_NVM_E  : isCancelPsbl_b = TRUE;  break;
        /* When cancel feature for crash data use case is supported, it would be possible to also cancel the chunk job
         * at any time. However, Fee2 and Fee3 can currently only cancel jobs when waiting for next chunk. So the check
         * of waiting for next chunk is implemented below. This check can be removed when cancel feature for Fee2 and
         * Fee3 is supported. */
        case FEE_RB_USER_CHUNK_E: isCancelPsbl_b = Fee_Prv_JobIsWaitingForNextChunk(deviceConfigTable_pcst);  break;
        /* Cancel feature not supported for other users. This can be removed once cancel feature for NvM is working. */
        default                 : isCancelPsbl_b = FALSE; break;
    }

    return(isCancelPsbl_b);
}

/**
 * \brief   Function that returns TRUE if the current job is waiting for the next chunk
 *
 * \param   deviceConfigTable_pcst   Pointer to the config table which has to be initialized.
 */
LOCAL_INLINE boolean Fee_Prv_JobIsWaitingForNextChunk(Fee_Prv_ConfigDeviceTable_tst const * deviceConfigTable_pcst)
{
    return(FEE_RB_JOBTYPE_WAIT_NEXT_CHUNK_E == deviceConfigTable_pcst->feeData_pst->jobData_st.actvJobDesc_st.type_en);
}

/**
 * \brief   For a given Fee job order, check whether we are processing the first part, or a subsequent chunk.
 *
 * \param   xJob_pcst   Pointer to job order
 *
 * \return  Function result
 * \retval  TRUE  if the processed is the first part (i.e. first chunk of a chunk-wise job, or a normal job)
 * \retval  FALSE if the processed is not the first part (i.e. a sbsequent chunk)
 *
 * \attention  For this function to work as expected for the second chunk, it assumes that some block data has been
 *             processed in the first chunk. In other words, the first of successive chunks must always read/write
 *             some block data.
*/
LOCAL_INLINE boolean Fee_Prv_JobIsFirstPart(Fee_Prv_JobDesc_tst const * xJob_pcst)
{
    return( 0u == (xJob_pcst->bytDone_un.cntrPayloadBytDone_u32) );
}

/**
 * \brief   For a given Fee job order, check whether we are processing the last part, or expecting subsequent chunks.
 *
 * \param   xJob_pcst   Pointer to job order
 *
 * \return  Function result
 * \retval  TRUE  if the processed is the last part (i.e. last chunk of a chunk-wise job, or a normal job)
 * \retval  FALSE if the processed is not the last part (i.e. subsequent chunk(s) will follow)
*/
LOCAL_INLINE boolean Fee_Prv_JobIsLastPart(Fee_Prv_JobDesc_tst const * xJob_pcst)
{
    boolean    isLastChunk_b;

    if(xJob_pcst->isChunkJob_b)
    {
        isLastChunk_b =
                (xJob_pcst->bytDone_un.cntrPayloadBytDone_u32 + xJob_pcst->length_u16) >= xJob_pcst->nrBytTot_u16;
    }
    else
    {
        isLastChunk_b = TRUE;
    }

    return(isLastChunk_b);
}

/**
 * \brief This function should always be present, even when both the SetMode and JobMode features are disabled.
 * As this function is called in all job orders to get the mode in which the job has to be performed.
 * In such situtation to save code its better to make this inline function and always return default job mode.
 *
 * \param   deviceConfigTable_pcst  Pointer to the config table which has to be initialized.
 * \param   jobType_en              Job type for which mode has to be set
 *
 * \return  FEE_RB_ALLJOBS_ALLSTEPS_E for all jobs
 */
#  if!((TRUE == FEE_PRV_CFG_RB_SET_AND_GET_JOB_MODE) || (STD_ON == FEE_PRV_CFG_SET_MODE_SUPPORTED)  || (TRUE == FEE_PRV_CFG_RB_ENTER_AND_EXIT_HIGH_SPEED_MODE_API))
LOCAL_INLINE Fee_Rb_JobMode_ten Fee_Prv_JobGetJobMode(Fee_Prv_ConfigDeviceTable_tst const* deviceConfigTable_pcst,
                                                      Fee_Rb_JobType_ten                   jobType_en)
{
    (void)deviceConfigTable_pcst;
    (void)jobType_en;

    return(FEE_RB_ALLJOBS_ALLSTEPS_E);
}
#  endif


#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

/* FEE_PRV_CFG_COMMON_ENABLED */
# endif

/* FEE_PRV_JOB_H */
#endif
