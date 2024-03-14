/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2020, all rights reserved
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

#include "MemIf.h"
// TRACE[NVM089] Check version compatibility of included header files
#if (!defined(MEMIF_AR_RELEASE_MAJOR_VERSION) || (MEMIF_AR_RELEASE_MAJOR_VERSION != NVM_AR_RELEASE_MAJOR_VERSION))
# error "AUTOSAR major version undefined or mismatched"
#endif

#include "rba_MemLib_Special.h"

#include "NvM_Prv.h"
#include "NvM_Prv_MemIf.h"
#include "NvM_Prv_Crypto.h"
#include "NvM_Prv_Job_Types.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_BlockSplit.h"

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
# include "TestCd_NvM.h"
#endif

/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
# define NVM_START_SEC_CODE
# include "NvM_MemMap.h"

RBA_MEMLIB_STATIC boolean NvM_Prv_MemIf_Read_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);
RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_MemIf_Read_GetStateFct(NvM_Prv_stJob_ten stJob_en);
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_MemIf_InitiateRead(NvM_Prv_stJob_ten* stJob_pen,
                                                                           NvM_Prv_JobResult_tst* JobResult_pst,
                                                                           NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC boolean NvM_Prv_MemIf_Write_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);
RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_MemIf_Write_GetStateFct(NvM_Prv_stJob_ten stJob_en);
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_MemIf_InitiateWrite(NvM_Prv_stJob_ten* stJob_pen,
                                                                            NvM_Prv_JobResult_tst* JobResult_pst,
                                                                            NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC boolean NvM_Prv_MemIf_Invalidate_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);
RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_MemIf_Invalidate_GetStateFct(NvM_Prv_stJob_ten stJob_en);
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_MemIf_InitiateInvalidate(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC boolean NvM_Prv_MemIf_Maintain_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);
RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_MemIf_Maintain_GetStateFct(NvM_Prv_stJob_ten stJob_en);
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_MemIf_InitiateMaintain(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_MemIf_Poll(NvM_Prv_stJob_ten* stJob_pen,
                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                   NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_JobResult_ten NvM_Prv_MemIf_FetchJobResult(NvM_BlockIdType idBlock_uo);

RBA_MEMLIB_STATIC NvM_Rb_ErrorInfoDetail_ten NvM_Prv_MemIf_FetchErrorInfoDetail(NvM_BlockIdType idBlock_uo);

# define NVM_STOP_SEC_CODE
# include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC NvM_Prv_Job_StateMachine_Data_tst const NvM_Prv_JobMemIf_Read_Data_cst =
{
        &NvM_Prv_MemIf_Read_GetStateFct,
        &NvM_Prv_MemIf_Read_IsEnabled,
        NvM_Prv_stJob_MemIf_Read_Initiate_e,
        NvM_Prv_stJob_MemIf_Completed_e,
        NvM_Prv_JobResource_Cluster_MemIf_e
};

RBA_MEMLIB_STATIC NvM_Prv_Job_StateMachine_Data_tst const NvM_Prv_JobMemIf_Write_Data_cst =
{
        &NvM_Prv_MemIf_Write_GetStateFct,
        &NvM_Prv_MemIf_Write_IsEnabled,
        NvM_Prv_stJob_MemIf_Write_Initiate_e,
        NvM_Prv_stJob_MemIf_Completed_e,
        NvM_Prv_JobResource_Cluster_MemIf_e
};

RBA_MEMLIB_STATIC NvM_Prv_Job_StateMachine_Data_tst const NvM_Prv_JobMemIf_Invalidate_Data_cst =
{
        &NvM_Prv_MemIf_Invalidate_GetStateFct,
        &NvM_Prv_MemIf_Invalidate_IsEnabled,
        NvM_Prv_stJob_MemIf_Invalidate_Initiate_e,
        NvM_Prv_stJob_MemIf_Completed_e,
        NvM_Prv_JobResource_Cluster_MemIf_e
};

RBA_MEMLIB_STATIC NvM_Prv_Job_StateMachine_Data_tst const NvM_Prv_JobMemIf_Maintain_Data_cst =
{
        &NvM_Prv_MemIf_Maintain_GetStateFct,
        &NvM_Prv_MemIf_Maintain_IsEnabled,
        NvM_Prv_stJob_MemIf_Maintain_Initiate_e,
        NvM_Prv_stJob_MemIf_Completed_e,
        NvM_Prv_JobResource_Cluster_MemIf_e
};

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

void NvM_Prv_MemIf_MainFunctionAndDependency (void)
{
    MemIf_Rb_MainFunctionAndDependency();
}

/**
 * \brief
 * This private function enters high-speed mode for all configured memory devices.
 * \details
 * First this function enters high-speed-mode for all configured memory devices.
 * If entering high-speed-mode has failed for at least one device then this function exits high-speed-mode
 * for all devices where entering high-speed-mode has succeeded before.
 *
 * \attention
 * If entering high-speed-mode has failed then no memory devices will remain in high-speed-mode.
 *
 * \return
 * Information about status of high-speed-mode for all configured memory devices
 * - E_OK       = high-speed-mode has been entered successfully for all configured memory devices
 * - E_NOT_OK   = entering high-speed-mode has failed for at least one configured memory device
 *                -> high-speed-mode is not active for NvM
 */
Std_ReturnType NvM_Prv_MemIf_EnterHighSpeedMode(void)
{
    Std_ReturnType stEnterHighSpeedMode_uo = E_NOT_OK;

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
    uint8 idxDevice_uo = 0u;
    NvM_Prv_IdMemoryDevice_tuo const* Devices_pcuo = NvM_Prv_BlkDesc_GetIdDeviceHighSpeedMode();

    if (NULL_PTR != Devices_pcuo)
    {
        /* MR12 RULE 13.5 VIOLATION: The side effect that right-side function is not called if idxDevice_uo is out-of-range is intended and necessary */
        while (   (idxDevice_uo < NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE)
               && (E_NOT_OK != MemIf_Rb_EnterHighSpeedMode(Devices_pcuo[idxDevice_uo])))
        {
            ++idxDevice_uo;
        }

        while ((idxDevice_uo > 0u) && (idxDevice_uo < NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE))
        {
            --idxDevice_uo;
            // return value of MemIf_Rb_ExitHighSpeedMode does not matter here so NvM does not evaluate it.
            (void)MemIf_Rb_ExitHighSpeedMode(Devices_pcuo[idxDevice_uo]);
        }

        // If idxDevice_uo is equal to the configured number of devices for high-speed-mode,
        // then entering high-speed-mode has succeeded for all configured devices
        if (idxDevice_uo == NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE)
        {
            stEnterHighSpeedMode_uo = E_OK;
        }
    }
#endif

    return stEnterHighSpeedMode_uo;
}

/**
 * \brief
 * This private function exits high-speed mode for all configured memory devices.
 *
 * \attention
 * The caller of this function shall ensure that high-speed-mode has been successfully entered before.
 *
 * \return
 * - E_OK = exit high-speed-mode has succeeded
 */
Std_ReturnType NvM_Prv_MemIf_ExitHighSpeedMode(void)
{
    // exit high-speed-mode can only succeed -> return value is always E_OK
    Std_ReturnType stExitHighSpeedMode_uo = E_OK;

#if ((NVM_JOB_PRIORITIZATION == STD_ON) && (NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE > 0u))
    NvM_Prv_IdMemoryDevice_tuo const* Devices_pcuo = NvM_Prv_BlkDesc_GetIdDeviceHighSpeedMode();

    if (NULL_PTR != Devices_pcuo)
    {
        uint8 idxDevice_uo = 0u;

        while (idxDevice_uo < NVM_CFG_NR_DEVICES_HIGH_SPEED_MODE)
        {
            // return value of MemIf_Rb_ExitHighSpeedMode does not matter here so NvM does not evaluate it.
            (void)MemIf_Rb_ExitHighSpeedMode(Devices_pcuo[idxDevice_uo]);
            ++idxDevice_uo;
        }
    }
#endif

    return stExitHighSpeedMode_uo;
}

/* MR12 RULE 8.13 VIOLATION: This function does not change the job-structure but all job-resource-cancel-functions shall
 * have the same signature and especially the job-structure has to be changeable so that the job-result can be modified
 * if necessary. */
boolean NvM_Prv_MemIf_Cancel(NvM_Prv_Job_tst* Job_pst)
{
    boolean isMemIfJobCanceled_b = FALSE;

    MemIf_Cancel(NvM_Prv_BlkDesc_GetIdDevice(Job_pst->JobData_st.idBlock_uo));

    if (MEMIF_JOB_CANCELED == MemIf_GetJobResult(NvM_Prv_BlkDesc_GetIdDevice(Job_pst->JobData_st.idBlock_uo)))
    {
        isMemIfJobCanceled_b = TRUE;
    }

    return isMemIfJobCanceled_b;
}

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_MemIf_Read_GetData(void)
{
    return &NvM_Prv_JobMemIf_Read_Data_cst;
}

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_MemIf_Write_GetData(void)
{
    return &NvM_Prv_JobMemIf_Write_Data_cst;
}
NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_MemIf_Invalidate_GetData(void)
{
    return &NvM_Prv_JobMemIf_Invalidate_Data_cst;
}

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_MemIf_Maintain_GetData(void)
{
    return &NvM_Prv_JobMemIf_Maintain_Data_cst;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_MemIf_Read_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // MemIf read is always enabled for all blocks
    return TRUE;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_MemIf_Read_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobStateRead_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_MemIf_Read_Initiate_e:
            JobStateRead_pfct = &NvM_Prv_MemIf_InitiateRead;
        break;

        case NvM_Prv_stJob_MemIf_Poll_Read_e:
            JobStateRead_pfct = &NvM_Prv_MemIf_Poll;
        break;

        default:
            JobStateRead_pfct = NULL_PTR;
        break;
    }
    return JobStateRead_pfct;
}

/**
 * \brief
 * This local private function is a read job step function and initiates a MemIf read job.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - permanent RAM block is used for current read job and data in the permanent RAM block is invalid OR
 *   temporary RAM block is used
 *
 * This function gets the number of bytes to be read from the block configuration
 * (see NvM_Prv_BlkDesc_GetBlockSizeStored).
 *
 * \param [inout] stJob_pen
 * State of the read job
 * \param [inout] JobResult_pst
 * Pointer to the job results, possible values:
 * - NvM_Prv_JobResult_Pending_e = MemIf job initiation has succeeded -> wait 1 cycle before polling the MemIf
 * - NvM_Prv_JobResult_Failed_e  = MemIf job initiation has failed -> start implicit recovery if required
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_MemIf_InitiateRead(NvM_Prv_stJob_ten* stJob_pen,
                                                                           NvM_Prv_JobResult_tst* JobResult_pst,
                                                                           NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Read_e;
    uint16 OffsetToSubBlock_u16;

    // MemIf job initiation has succeeded -> Poll MemIf job
    JobResult_pst->Result_en = NvM_Prv_JobResult_Pending_e;
    *stJob_pen = NvM_Prv_stJob_MemIf_Poll_Read_e;

    if (NULL_PTR != NvM_Prv_BlkDesc_GetCfgBlockSplit(JobData_pcst->idBlock_uo))
    {
        *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = NvM_Prv_BlockSplit_GetSizeSubBlock(JobData_pcst->idBlock_uo,
                                                                                              JobData_pcst->idxDataset_u8);
        OffsetToSubBlock_u16 = NvM_Prv_BlockSplit_GetOffsetToSubBlock(JobData_pcst->idBlock_uo,
                                                                      JobData_pcst->idxDataset_u8);
    }
    else
    {
        *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = NvM_Prv_BlkDesc_GetBlockSizeStored(JobData_pcst->idBlock_uo);
        OffsetToSubBlock_u16 = 0u;
    }

    // Start a MemIf_Read job
    if (NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_SELECT_FOR_MIGRATION))
    {
        // If variable length read is not available then function MemIf_Rb_VarLenRead will be replaced by E_NOT_OK
        // (see MemIf configuration MemIf_Cfg.h) and NvM stores the result of MemIf_Rb_VarLenRead in local variable
        // to avoid a constant expression in 'if' statement and resulting MISRA warning
#if (NVM_RB_BLOCK_HEADER > 0u)
        Std_ReturnType VarLenReadInitiated_uo =
                MemIf_Rb_VarLenRead(NvM_Prv_BlkDesc_GetIdDevice(JobData_pcst->idBlock_uo),
                                    NvM_Prv_BlkDesc_GetIdMemIf(JobData_pcst->idBlock_uo) + JobData_pcst->idxDataset_u8,
                                    0,
                                    JobData_pcst->IntBuffer_st.BlockHeader_pu8,
                                    *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16);
#else
        Std_ReturnType VarLenReadInitiated_uo =
                MemIf_Rb_VarLenRead(NvM_Prv_BlkDesc_GetIdDevice(JobData_pcst->idBlock_uo),
                                    NvM_Prv_BlkDesc_GetIdMemIf(JobData_pcst->idBlock_uo) + JobData_pcst->idxDataset_u8,
                                    0,
                                    &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetToSubBlock_u16],
                                    *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16);
#endif

        // Reads all blocks where Migration or variable block length feature is active
        // Migration feature can be active without variable block length feature
        // Variable block length feature neeads activation of Migration feature
        if (E_OK != VarLenReadInitiated_uo)
        {
            // MemIf job initiation has failed
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
            *stJob_pen = NvM_Prv_stJob_MemIf_Completed_e;
        }
    }
    else
    {
#if (NVM_RB_BLOCK_HEADER > 0u)
        (void)OffsetToSubBlock_u16;
        // Reads a normal block without Migration or variable block length feature active
        if (E_OK != MemIf_Read(NvM_Prv_BlkDesc_GetIdDevice(JobData_pcst->idBlock_uo),
                               NvM_Prv_BlkDesc_GetIdMemIf(JobData_pcst->idBlock_uo) + JobData_pcst->idxDataset_u8,
                               0,
                               JobData_pcst->IntBuffer_st.BlockHeader_pu8,
                               *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16))
#else
        // Reads a normal block without Migration or variable block length feature active
        if (E_OK != MemIf_Read(NvM_Prv_BlkDesc_GetIdDevice(JobData_pcst->idBlock_uo),
                               NvM_Prv_BlkDesc_GetIdMemIf(JobData_pcst->idBlock_uo) + JobData_pcst->idxDataset_u8,
                               0,
                               &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetToSubBlock_u16],
                               *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16))
#endif
        {
            // MemIf job initiation has failed
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
            *stJob_pen = NvM_Prv_stJob_MemIf_Completed_e;
        }
    }

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_MemIf_Write_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // MemIf write is always enabled for all blocks
    return TRUE;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_MemIf_Write_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobStateWrite_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_MemIf_Write_Initiate_e:
            JobStateWrite_pfct = &NvM_Prv_MemIf_InitiateWrite;
        break;

        case NvM_Prv_stJob_MemIf_Poll_Write_e:
            JobStateWrite_pfct = &NvM_Prv_MemIf_Poll;
        break;

        default:
            JobStateWrite_pfct = NULL_PTR;
        break;
    }
    return JobStateWrite_pfct;
}

/**
 * \brief
 * This local private function is a write job step function and initiates a MemIf write job.
 * \details
 * The NvM reaches this job step only if following preconditions are true:
 * - CRC recalculation has succeeded
 * - write is required
 * - cryptographic services has succeeded
 * - job buffer contains data to be written
 *
 * \param [inout] stJob_pen
 * State of the write job
 * \param [inout] JobResult_pst
 * Pointer to the job results,  possible values:
 * - NvM_Prv_JobResult_Pending_e = MemIf job initiation has succeeded -> wait 1 cycle before polling the MemIf
 * - NvM_Prv_JobResult_Failed_e  = MemIf job initiation has failed -> finish job
 * \param [in] JobData_pcst
 * Constant pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_MemIf_InitiateWrite(NvM_Prv_stJob_ten* stJob_pen,
                                                                            NvM_Prv_JobResult_tst* JobResult_pst,
                                                                            NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Write_e;
    uint16 OffsetToSubBlock_u16;

    // MemIf job initiation has succeeded -> poll MemIf job
    JobResult_pst->Result_en = NvM_Prv_JobResult_Pending_e;
    *stJob_pen = NvM_Prv_stJob_MemIf_Poll_Write_e;

#if (NVM_RB_REQ_MONITOR == NVM_RB_REQ_MON_WRITE)
    if (!NvM_Prv_BlockSplit_IsIdxSubBlock(JobData_pcst->idBlock_uo, JobData_pcst->idxDataset_u8))
    {
        // Write needed -> increment write counters and continue with job
        NvM_Prv_WriteMonCntrHandling(JobData_pcst->idBlock_uo,
                                     NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo),
                                     FALSE);
    }
#endif

    OffsetToSubBlock_u16 = NvM_Prv_BlockSplit_GetOffsetToSubBlock(JobData_pcst->idBlock_uo,
                                                                  JobData_pcst->idxDataset_u8);
    // Start a MemIf_Write job
    if (NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_VARIABLE_BLOCK_LENGTH))
    {
        // If variable length write is not available then function MemIf_Rb_VarLenWrite will be replaced by E_NOT_OK
        // (see MemIf configuration MemIf_Cfg.h) and NvM stores the result of MemIf_Rb_VarLenWrite in local variable
        // to avoid a constant expression in 'if' statement and resulting MISRA warning
#if (NVM_RB_BLOCK_HEADER > 0u)
        Std_ReturnType VarLenWriteInitiated_uo =
                MemIf_Rb_VarLenWrite(NvM_Prv_BlkDesc_GetIdDevice(JobData_pcst->idBlock_uo),
                                     NvM_Prv_BlkDesc_GetIdMemIf(JobData_pcst->idBlock_uo) + JobData_pcst->idxDataset_u8,
                                     JobData_pcst->IntBuffer_st.BlockHeader_pu8,
                                     NvM_Prv_BlkDesc_GetBlockSizeStored(JobData_pcst->idBlock_uo));
#else
        Std_ReturnType VarLenWriteInitiated_uo =
                MemIf_Rb_VarLenWrite(NvM_Prv_BlkDesc_GetIdDevice(JobData_pcst->idBlock_uo),
                                     NvM_Prv_BlkDesc_GetIdMemIf(JobData_pcst->idBlock_uo) + JobData_pcst->idxDataset_u8,
                                     &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetToSubBlock_u16],
                                     NvM_Prv_BlkDesc_GetBlockSizeStored(JobData_pcst->idBlock_uo));
#endif

        // Write blocks where the block length is variable (defined in NvMRbNvBlockLengthString)
        // Migration feature must be active here
        if (E_OK != VarLenWriteInitiated_uo)
        {
            // Initiating write job has failed
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
            *stJob_pen = NvM_Prv_stJob_MemIf_Completed_e;
        }
    }
    else
    {
        // Write blocks where the length is defined as numeric literal in NvMNvBlockLength
        // Migration feature could be active here
#if (NVM_RB_BLOCK_HEADER > 0u)
        (void)OffsetToSubBlock_u16;
        if (E_OK != MemIf_Write(NvM_Prv_BlkDesc_GetIdDevice(JobData_pcst->idBlock_uo),
                                NvM_Prv_BlkDesc_GetIdMemIf(JobData_pcst->idBlock_uo) + JobData_pcst->idxDataset_u8,
                                JobData_pcst->IntBuffer_st.BlockHeader_pu8))
#else
        if (E_OK != MemIf_Write(NvM_Prv_BlkDesc_GetIdDevice(JobData_pcst->idBlock_uo),
                                NvM_Prv_BlkDesc_GetIdMemIf(JobData_pcst->idBlock_uo) + JobData_pcst->idxDataset_u8,
                                &JobData_pcst->IntBuffer_st.Buffer_pu8[OffsetToSubBlock_u16]))
#endif
        {
            // Initiating write job has failed
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
            *stJob_pen = NvM_Prv_stJob_MemIf_Completed_e;
        }
    }

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_MemIf_Invalidate_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // MemIf invalidate is always enabled for all blocks
    return TRUE;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_MemIf_Invalidate_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobStateInvalidate_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_MemIf_Invalidate_Initiate_e:
            JobStateInvalidate_pfct = &NvM_Prv_MemIf_InitiateInvalidate;
        break;

        case NvM_Prv_stJob_MemIf_Poll_Invalidate_e:
            JobStateInvalidate_pfct = &NvM_Prv_MemIf_Poll;
        break;

        default:
            JobStateInvalidate_pfct = NULL_PTR;
        break;
    }
    return JobStateInvalidate_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_MemIf_InitiateInvalidate(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Invalidate_e;

    // MemIf job initiation has succeeded -> Poll MemIf job
    JobResult_pst->Result_en = NvM_Prv_JobResult_Pending_e;
    *stJob_pen = NvM_Prv_stJob_MemIf_Poll_Invalidate_e;

    // Start a MemIf_InvalidateBlock job
    if (E_OK != MemIf_InvalidateBlock(NvM_Prv_BlkDesc_GetIdDevice(JobData_pcst->idBlock_uo),
                                      NvM_Prv_BlkDesc_GetIdMemIf(JobData_pcst->idBlock_uo) + JobData_pcst->idxDataset_u8))
    {
        // Initiating invalidate job has failed
        JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
        *stJob_pen = NvM_Prv_stJob_MemIf_Completed_e;
    }

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC boolean NvM_Prv_MemIf_Maintain_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // MemIf maintenance is always enabled for all blocks
    return TRUE;
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_MemIf_Maintain_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobStateMaintain_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_MemIf_Maintain_Initiate_e:
            JobStateMaintain_pfct = &NvM_Prv_MemIf_InitiateMaintain;
        break;

        case NvM_Prv_stJob_MemIf_Poll_Maintain_e:
            JobStateMaintain_pfct = &NvM_Prv_MemIf_Poll;
        break;

        default:
            JobStateMaintain_pfct = NULL_PTR;
        break;
    }
    return JobStateMaintain_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_MemIf_InitiateMaintain(NvM_Prv_stJob_ten* stJob_pen,
                                                                               NvM_Prv_JobResult_tst* JobResult_pst,
                                                                               NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Maintain_e;

    Std_ReturnType BlockMaintenanceInitiated_uo;

    // MemIf job initiation has succeeded -> poll MemIf job
    JobResult_pst->Result_en = NvM_Prv_JobResult_Pending_e;
    *stJob_pen = NvM_Prv_stJob_MemIf_Poll_Maintain_e;

    // If block maintenance is not available then function MemIf_Rb_BlockMaintenance will be replaced by E_NOT_OK
    // (see MemIf configuration MemIf_Cfg.h) and NvM stores the result of MemIf_Rb_BlockMaintenance in local variable
    // to avoid a constant expression in 'if' statement and resulting MISRA warning
    BlockMaintenanceInitiated_uo =
            MemIf_Rb_BlockMaintenance(NvM_Prv_BlkDesc_GetIdDevice(JobData_pcst->idBlock_uo),
                                      NvM_Prv_BlkDesc_GetIdMemIf(JobData_pcst->idBlock_uo) + JobData_pcst->idxDataset_u8);

    // Start a MemIf_Rb_BlockMaintenance job
    if (E_OK != BlockMaintenanceInitiated_uo)
    {
        // Initiating maintain job has failed
        JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
        *stJob_pen = NvM_Prv_stJob_MemIf_Completed_e;
    }

    // If maintenance feature is disabled then function MemIf_Rb_BlockMaintenance will be replaced by E_NOT_OK and
    // compiler generates a warning about "unused parameter".
    // Following line was added to avoid this warning
    (void)JobData_pcst;

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_MemIf_Poll(NvM_Prv_stJob_ten* stJob_pen,
                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                   NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en;

    switch (*stJob_pen)
    {
        case NvM_Prv_stJob_MemIf_Poll_Read_e:
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Read_e;
        break;

        case NvM_Prv_stJob_MemIf_Poll_Write_e:
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Write_e;
        break;

        case NvM_Prv_stJob_MemIf_Poll_Maintain_e:
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Maintain_e;
        break;

        case NvM_Prv_stJob_MemIf_Poll_Invalidate_e:
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoMemIf_Invalidate_e;
        break;

        default:
            idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_Invalid_e;
        break;
    }

    JobResult_pst->Result_en = NvM_Prv_MemIf_FetchJobResult(JobData_pcst->idBlock_uo);

    if (NvM_Prv_JobResult_Pending_e != JobResult_pst->Result_en)
    {
        if (NvM_Prv_JobResult_BlockInconsistent_e == JobResult_pst->Result_en)
        {
            // MemIf only provides senseful error info details if the job finished with Inconsistent
            JobResult_pst->errorInfoDetail_en = NvM_Prv_MemIf_FetchErrorInfoDetail(JobData_pcst->idBlock_uo);
        }

        *stJob_pen = NvM_Prv_stJob_MemIf_Completed_e;
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function fetches the job result of the MemIf component and translates it into NvM types
 *
 * \param [in] BlockId
 * ID of the NvM block for which the job result shall be fetched
 * \return
 * MemIf job result translated into NvM types
 */
RBA_MEMLIB_STATIC NvM_Prv_JobResult_ten NvM_Prv_MemIf_FetchJobResult(NvM_BlockIdType idBlock_uo)
{
    // TRACE[NVM346] Check the status of the currently active MemIf job
    MemIf_JobResultType ResultMemIf_en = MemIf_GetJobResult(NvM_Prv_BlkDesc_GetIdDevice(idBlock_uo));
    NvM_Prv_JobResult_ten result_en;

    // If only Fee or only Ea is used then function MemIf_GetJobResult will not use idBlock_uo and
    // compiler generates a warning "unused parameter".
    // Following line was added to avoid this warning
    (void)idBlock_uo;

    switch (ResultMemIf_en)
    {
        case MEMIF_JOB_PENDING:
        {
            result_en = NvM_Prv_JobResult_Pending_e;
        }
        break;
        case MEMIF_JOB_OK:
        {
            result_en = NvM_Prv_JobResult_Succeeded_e;
        }
        break;
        case MEMIF_BLOCK_INCONSISTENT:
        {
            result_en = NvM_Prv_JobResult_BlockInconsistent_e;
        }
        break;
        case MEMIF_BLOCK_INVALID:
        {
            result_en = NvM_Prv_JobResult_BlockInvalidated_e;
        }
        break;
        default:
        {
            result_en = NvM_Prv_JobResult_Failed_e;
        }
        break;
    }

    return(result_en);
}

/**
 * \brief
 * This local private function fetches detailed error information from the MemIf component
 * \attention
 * Currently MemIf delivers only senseful detailed error results if the previous MemIf_Read
 *  finished with MEMIF_BLOCK_INCONSISTENT.
 *
 * \param [in] BlockId
 * ID of the NvM block for which error details are required
 * \return
 * Detailed error information from MemIf level
 */
RBA_MEMLIB_STATIC NvM_Rb_ErrorInfoDetail_ten NvM_Prv_MemIf_FetchErrorInfoDetail(NvM_BlockIdType idBlock_uo)
{
#if (NVM_RB_ERROR_INFO_DETAIL == STD_ON)

    MemIf_Rb_DetailedBlkInfo_ten detail_en = MemIf_Rb_GetDetailedBlkInfo(NvM_Prv_BlkDesc_GetIdDevice(idBlock_uo),
                                                                         NvM_Prv_BlkDesc_GetIdMemIf(idBlock_uo));
    NvM_Rb_ErrorInfoDetail_ten result_en;

    // If no detailed error info is available on lower level (Fee/Ea) then function MemIf_Rb_GetDetailedBlkInfo
    // will be replaced by MEMIF_RB_DETAILED_BLK_INFO_NOT_AVAILABLE_E (see MemIf configuration MemIf_Cfg.h) and
    // compiler generates a warning "unused parameter" about idBlock_uo.
    // Following line was added to avoid this warning
    (void)idBlock_uo;

    /* Translate MemIf detail error information to NvM detail error information */
    switch(detail_en)
    {
        case MEMIF_RB_DETAILED_BLK_INFO_NOT_AVAILABLE_E:
        {
            result_en = NvM_Rb_ErrorInfoDetail_NotAvailable_e;
        }
        break;
        case MEMIF_RB_DETAILED_BLK_INFO_INIT_E:
        {
            result_en = NvM_Rb_ErrorInfoDetail_NotAvailable_e;
        }
        break;
        case MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_MISSING_E:
        {
            result_en = NvM_Rb_ErrorInfoDetail_BlockNotFoundOnMedium_e;
        }
        break;
        case MEMIF_RB_DETAILED_BLK_INFO_INCONSISTENT_DATA_E:
        {
            result_en = NvM_Rb_ErrorInfoDetail_InconsistentData_e;
        }
        break;
        default:
        {
            /* Should never be reached */
            result_en = NvM_Rb_ErrorInfoDetail_NotAvailable_e;
        }
        break;
    }

    return(result_en);
#else
    (void)idBlock_uo;
    return NvM_Rb_ErrorInfoDetail_NotAvailable_e;
#endif
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

