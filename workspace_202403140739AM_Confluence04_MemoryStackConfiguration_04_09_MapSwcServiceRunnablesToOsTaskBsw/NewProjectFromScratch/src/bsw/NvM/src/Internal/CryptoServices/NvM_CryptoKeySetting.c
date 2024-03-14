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
#include "NvM.h"

#include "rba_MemLib_Special.h"

#include "NvM_Prv_ErrorDetection.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_Crypto.h"
#include "NvM_Prv_Csm.h"

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
# include "TestCd_NvM.h"
#endif

/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)

RBA_MEMLIB_STATIC boolean NvM_Prv_CryptoKeySetting_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_CryptoKeySetting_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoKeySetting_SetInitVector(NvM_Prv_stJob_ten* stJob_pen,
                                                                                       NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                       NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoKeySetting_SetValid(NvM_Prv_stJob_ten* stJob_pen,
                                                                                  NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                  NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoKeySetting_PollJob(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst);

#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"


/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_CryptoKeySetting_Data_cst =
{
        &NvM_Prv_CryptoKeySetting_GetStateFct,
        &NvM_Prv_CryptoKeySetting_IsEnabled,
        NvM_Prv_stJob_CryptoKeySetting_SetInitVector_e,
        NvM_Prv_stJob_CryptoKeySetting_Completed_e,
        NvM_Prv_JobResource_Cluster_Crypto_e

};

#endif  // (NVM_CRYPTO_RANDOM_USED == STD_ON)

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_CryptoKeySetting_GetData(void)
{
    // Cryptographic key setting job is enabled only for blocks with configured cryptographic features and
    // random number generation
#if (NVM_CRYPTO_RANDOM_USED == STD_ON)
    return &NvM_Prv_CryptoKeySetting_Data_cst;
#else
    return NULL_PTR;
#endif
}

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)

RBA_MEMLIB_STATIC boolean NvM_Prv_CryptoKeySetting_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    return NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_CRYPTO);
}

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_CryptoKeySetting_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobStateCryptoKeySetting_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_CryptoKeySetting_SetInitVector_e:
            JobStateCryptoKeySetting_pfct = &NvM_Prv_CryptoKeySetting_SetInitVector;
        break;

        case NvM_Prv_stJob_CryptoKeySetting_SetValid_e:
            JobStateCryptoKeySetting_pfct = &NvM_Prv_CryptoKeySetting_SetValid;
        break;

        case NvM_Prv_stJob_CryptoKeySetting_PollSetInitVector_e:
        case NvM_Prv_stJob_CryptoKeySetting_PollSetValid_e:
            JobStateCryptoKeySetting_pfct = &NvM_Prv_CryptoKeySetting_PollJob;
        break;

        default:
            JobStateCryptoKeySetting_pfct = NULL_PTR;
        break;
    }

    return JobStateCryptoKeySetting_pfct;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoKeySetting_SetInitVector(NvM_Prv_stJob_ten* stJob_pen,
                                                                                       NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                       NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoKeySetting_e;

    NvM_Prv_Crypto_dataServiceStart_tst dataJobStart_st;
    NvM_Prv_Crypto_ServiceData_tst jobCryptoData_st;
    uint16 LengthInitVector_u16 = NvM_Prv_BlkDesc_GetLengthJobCsm(JobData_pcst->idBlock_uo,
                                                                  NvM_Prv_Crypto_idService_InitVector_e);
    uint16 PositionInitVector_u16 = NvM_Prv_BlkDesc_GetPositionJobCsmInitVector(JobData_pcst->idBlock_uo);

    // Prefill job results structure for key element setting
    JobResult_pst->Crypto_st.idServiceCrypto_en = NvM_Prv_Crypto_idService_InitVector_e;
    JobResult_pst->Crypto_st.idJobStarted_uo = NvM_Prv_BlkDesc_GetIdJobCsm(JobData_pcst->idBlock_uo,
                                                                           NvM_Prv_Crypto_idService_InitVector_e);
    // member idJobCallback_uo will be set in NvM_Prv_Csm_SetKeyElementInitVector
    JobResult_pst->Crypto_st.outputPrimaryLength_u32 = 0u;              // not used for key element setting
    JobResult_pst->Crypto_st.outputPrimaryLengthConfigured_u32 = 0u;    // not used for key element setting
    JobResult_pst->Crypto_st.outputSecondaryLength_u32 = 0u;            // not used for key element setting
    JobResult_pst->Crypto_st.outputSecondaryLengthConfigured_u32 = 0u;  // not used for key element setting
    // member Result_uo will be set in NvM_Prv_Csm_SetKeyElementInitVector
    JobResult_pst->Crypto_st.ResultVerification_uo = CRYPTO_E_VER_OK;   // not used for key element setting
    JobResult_pst->Crypto_st.isJobCanceled_b = FALSE;

    // Prefill data structure for a cryptographic job
    jobCryptoData_st.inputPrimary_pcu8 = &((JobData_pcst->IntBuffer_st.Buffer_pu8)[PositionInitVector_u16]);
    jobCryptoData_st.inputPrimaryLength_u32 = LengthInitVector_u16;
    // all other members of jobCryptoData_st are not required to set key element

    dataJobStart_st.stJobNextSuccess_en = NvM_Prv_stJob_CryptoKeySetting_PollSetInitVector_e;
    dataJobStart_st.stJobNextSkipped_en = NvM_Prv_stJob_CryptoKeySetting_Completed_e;
    dataJobStart_st.isCryptoServiceConfigured_b = (0u != LengthInitVector_u16);
    dataJobStart_st.idJobToStart_uo = JobResult_pst->Crypto_st.idJobStarted_uo;
    dataJobStart_st.StartJobCsm_pfct = &NvM_Prv_Csm_SetKeyInitVector;

    NvM_Prv_Csm_StartJob(stJob_pen, JobResult_pst, JobData_pcst, &jobCryptoData_st, &dataJobStart_st);

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoKeySetting_SetValid(NvM_Prv_stJob_ten* stJob_pen,
                                                                                  NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                  NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoKeySetting_e;

    NvM_Prv_Crypto_dataServiceStart_tst dataJobStart_st;
    uint16 LengthInitVector_u16 = NvM_Prv_BlkDesc_GetLengthJobCsm(JobData_pcst->idBlock_uo,
                                                                  NvM_Prv_Crypto_idService_InitVector_e);

    // Prefill job results structure to set key valid
    JobResult_pst->Crypto_st.idServiceCrypto_en = NvM_Prv_Crypto_idService_InitVector_e;
    JobResult_pst->Crypto_st.idJobStarted_uo = NvM_Prv_BlkDesc_GetIdJobCsm(JobData_pcst->idBlock_uo,
                                                                           NvM_Prv_Crypto_idService_InitVector_e);
    // member idJobCallback_uo will be set in NvM_Prv_Csm_SetKeyElementInitVector
    JobResult_pst->Crypto_st.outputPrimaryLength_u32 = 0u;              // not used to set key valid
    JobResult_pst->Crypto_st.outputPrimaryLengthConfigured_u32 = 0u;    // not used not used to set key valid
    JobResult_pst->Crypto_st.outputSecondaryLength_u32 = 0u;            // not used not used to set key valid
    JobResult_pst->Crypto_st.outputSecondaryLengthConfigured_u32 = 0u;  // not used not used to set key valid
    // member Result_uo will be set in NvM_Prv_Csm_SetKeyElementInitVector
    JobResult_pst->Crypto_st.ResultVerification_uo = CRYPTO_E_VER_OK;   // not used not used to set key valid
    JobResult_pst->Crypto_st.isJobCanceled_b = FALSE;

    // Prefill data structure for a cryptographic job
    // all members of jobCryptoData_st are not required to set key valid

    dataJobStart_st.stJobNextSuccess_en = NvM_Prv_stJob_CryptoKeySetting_PollSetValid_e;
    dataJobStart_st.stJobNextSkipped_en = NvM_Prv_stJob_CryptoKeySetting_Completed_e;
    dataJobStart_st.isCryptoServiceConfigured_b = (0u != LengthInitVector_u16);
    dataJobStart_st.idJobToStart_uo = JobResult_pst->Crypto_st.idJobStarted_uo;
    dataJobStart_st.StartJobCsm_pfct = &NvM_Prv_Csm_SetKeyValid;

    NvM_Prv_Csm_StartJob(stJob_pen, JobResult_pst, JobData_pcst, NULL_PTR, &dataJobStart_st);

    return idJobStateMachineNext_en;
}

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoKeySetting_PollJob(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoKeySetting_e;

    // check if requested Csm job has completed
    // check whether the Csm job has completed and result is available
    if (NvM_Prv_Csm_CheckJobResult(JobResult_pst,
                                   JobData_pcst,
                                   NVM_PRV_NO_DET_ERROR,
                                   NVM_PRV_NO_DET_ERROR))
    {
        if (E_OK == JobResult_pst->Crypto_st.Result_uo)
        {
            if (NvM_Prv_stJob_CryptoKeySetting_PollSetInitVector_e == *stJob_pen)
            {
                *stJob_pen = NvM_Prv_stJob_CryptoKeySetting_SetValid_e;
            }
            else
            {
                *stJob_pen = NvM_Prv_stJob_CryptoKeySetting_Completed_e;
            }

            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
        }
        else
        {
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
            *stJob_pen = NvM_Prv_stJob_CryptoKeySetting_Completed_e;
        }
    }
    // as long as Csm job has not completed and no result is available NvM remains pending in current state

    return idJobStateMachineNext_en;
}

#endif  // (NVM_CRYPTO_RANDOM_USED == STD_ON)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

