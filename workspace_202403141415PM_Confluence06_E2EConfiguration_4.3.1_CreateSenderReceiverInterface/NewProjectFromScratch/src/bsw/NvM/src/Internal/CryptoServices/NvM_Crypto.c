/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
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

#include "NvM_Prv.h"
#include "NvM_Prv_Crypto.h"
#include "NvM_Prv_JobResource.h"
#include "NvM_Prv_ErrorDetection.h"
#include "NvM_Prv_Crc.h"
#include "NvM_Prv_Csm.h"
#include "NvM_Prv_Crypto_ServiceTypes.h"
#include "NvM_Prv_Crypto_InternalTypes.h"

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
# include "TestCd_NvM.h"
#endif

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)
/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
# define NVM_START_SEC_CODE
# include "NvM_MemMap.h"

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_CryptoGenerateRandomNr_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC boolean NvM_Prv_CryptoGenerateRandomNr_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Crypto_StartRandomNrGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                       NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                       NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Crypto_PollRandomNrGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                      NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                      NvM_Prv_JobData_tst const* JobData_pcst);

# define NVM_STOP_SEC_CODE
# include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_CryptoGenerateRandomNr_Data_cst =
{
        &NvM_Prv_CryptoGenerateRandomNr_GetStateFct,
        &NvM_Prv_CryptoGenerateRandomNr_IsEnabled,
        NvM_Prv_stJob_GenerateRandomNr_Crypto_Start_e,
        NvM_Prv_stJob_GenerateRandomNr_Crypto_Completed_e,
        NvM_Prv_JobResource_Cluster_Crypto_e

};

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

#endif  // (NVM_CRYPTO_RANDOM_USED == STD_ON)

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"
/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_CryptoGenerateRandomNr_GetData(void)
{
#if (NVM_CRYPTO_RANDOM_USED == STD_ON)
    return &NvM_Prv_CryptoGenerateRandomNr_Data_cst;
#else
    return NULL_PTR;
#endif
}

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)

RBA_MEMLIB_STATIC boolean NvM_Prv_CryptoGenerateRandomNr_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    (void)JobData_pcst;
    // Cryptographic read job is enabled only if cryptographic features and random number generation are configured
#if ((NVM_CRYPTO_USED == STD_ON) && (NVM_CRYPTO_RANDOM_USED == STD_ON))
    return TRUE;
#else
    return FALSE;
#endif
}

/*
 * \brief
 * This local private function returns the pointer to the job step for the current state
 * of the cryptographic random number generation job.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 *
 * \param[IN] stJob_en
 * Current state of the cryptographic random number generation job
 *
 * \return
 * Pointer to the job step
 */
RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_CryptoGenerateRandomNr_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobStateGenerateRandom_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_GenerateRandomNr_Crypto_Start_e:
            JobStateGenerateRandom_pfct = &NvM_Prv_Crypto_StartRandomNrGeneration;
        break;

        case NvM_Prv_stJob_GenerateRandomNr_Crypto_Poll_e:
            JobStateGenerateRandom_pfct = &NvM_Prv_Crypto_PollRandomNrGeneration;
        break;

        default:
            JobStateGenerateRandom_pfct = NULL_PTR;
        break;
    }

    return JobStateGenerateRandom_pfct;
}

/**
 * \brief
 * This local private function is a cryptographic job step function and starts the cryptographic service
 * for the random number generation.
 * \details
 * This function is only available if usage of cryptographic services and random number generation are enabled
 * by NvM configuration.
 * This function only prepares all required data. The actual start is done by the function NvM_Prv_Csm_StartJob.
 *
 * \param [inout] stJob_pen
 * State of the cryptographic job
 * \param [inout] JobResult_pst
 * Pointer to the result of the cryptographic job, possible values:
 * - NvM_Prv_JobResult_Succeeded_e = start of random number generation has succeeded
 * - NvM_Prv_JobResult_Pending_e   = random number generation cannot be started due to busy Csm -> retry in next cycle
 * - NvM_Prv_JobResult_Failed_e    = start of random number generation has failed
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Crypto_StartRandomNrGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                       NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                       NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoGenerateRandomNr_e;
#if (NVM_CRYPTO_RANDOM_USED == STD_ON)

    NvM_Prv_Crypto_dataServiceStart_tst dataJobStart_st;
    NvM_Prv_Crypto_ServiceData_tst jobCryptoData_st;

    (void)JobData_pcst;

    // Prefill job results structure for random number generation
    JobResult_pst->Crypto_st.idServiceCrypto_en = NvM_Prv_Crypto_idService_Count_e;
    JobResult_pst->Crypto_st.idJobStarted_uo = NVM_PRV_ID_CSM_JOB_RANDOM;
    // member idJobCallback_uo will be set in the callback if Csm fom has completed
    JobResult_pst->Crypto_st.outputPrimaryLength_u32 = NVM_PRV_LENGTH_RANDOM_NR;
    JobResult_pst->Crypto_st.outputPrimaryLengthConfigured_u32 = JobResult_pst->Crypto_st.outputPrimaryLength_u32;
    JobResult_pst->Crypto_st.outputSecondaryLength_u32 = 0u;              // not used for random number generation
    JobResult_pst->Crypto_st.outputSecondaryLengthConfigured_u32 = 0u;    // not used for random number generation
    // member Result_uo will be set in the callback if Csm job has completed
    JobResult_pst->Crypto_st.ResultVerification_uo = (Crypto_VerifyResultType)CRYPTO_E_VER_OK;  // not used for random number generation
    JobResult_pst->Crypto_st.isJobCanceled_b = FALSE;

    // Prefill data structure for a cryptographic job
    jobCryptoData_st.outputPrimary_pu8 = NvM_Prv_GetInitVectorCryptoKey();
    jobCryptoData_st.outputPrimaryLength_pu32 = &JobResult_pst->Crypto_st.outputPrimaryLength_u32;
    // all other members of jobCryptoData_st are not required to start random number generation

    dataJobStart_st.stJobNextSuccess_en = NvM_Prv_stJob_GenerateRandomNr_Crypto_Poll_e;
    dataJobStart_st.stJobNextSkipped_en = NvM_Prv_stJob_GenerateRandomNr_Crypto_Completed_e;
    dataJobStart_st.isCryptoServiceConfigured_b = (0u != NVM_PRV_LENGTH_RANDOM_NR);
    dataJobStart_st.idJobToStart_uo = JobResult_pst->Crypto_st.idJobStarted_uo;
    dataJobStart_st.StartJobCsm_pfct = &NvM_Prv_Csm_StartRandomGeneration;

    NvM_Prv_Csm_StartJob(stJob_pen, JobResult_pst, JobData_pcst, &jobCryptoData_st, &dataJobStart_st);
#else
    // Set stJob_pen to already set value to avoid warnings that stJob_pen can be a pointer to const
    *stJob_pen = NvM_Prv_stJob_GenerateRandomNr_Crypto_Completed_e;
    // If function NvM_Prv_Crypto_StartRandomGeneration has been called with disabled random number generation
    // let the job fail
    JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
    (void)JobData_pcst;
#endif

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a cryptographic job step function and polls the cryptographic service
 * for the random number generation.
 * \details
 * This function is only available if usage of cryptographic services and random number generation are enabled
 * by NvM configuration.
 * This function checks whether the cryptographic service for the random number generation has completed and
 * generated random number is valid and available for later usage.
 * If length of the generated random number does not match the user configured length then the NvM reports
 * the DET error NVM_E_LENGTH_RANDOM_NUMBER (see NvM_Prv_Csm_CheckJobResult).
 *
 * \param [inout] stJob_pen
 * State of the cryptographic job
 * \param [inout] JobResult_pst
 * Pointer to the result of the cryptographic job, possible values:
 * - NvM_Prv_JobResult_Succeeded_e = random number generation has completed and generated random number is valid
 * - NvM_Prv_JobResult_Pending_e   = random number generation is still pending
 * - NvM_Prv_JobResult_Failed_e    = random number generation has failed or generated random number is invalid
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_Crypto_PollRandomNrGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                      NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                      NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoGenerateRandomNr_e;

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)

# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    if (TestCd_NvM_st.Arguments.CsmJobRandomSucceed_b)
    {
        uint8* RandomNr_puo = NvM_Prv_GetInitVectorCryptoKey();
        RandomNr_puo[0] = 0xFFu;
        RandomNr_puo[1] = 0xFFu;
        RandomNr_puo[2] = 0xFDu;
    }
# endif

    // check whether the Csm job has completed and result is available
    if (NvM_Prv_Csm_CheckJobResult(JobResult_pst,
                                   JobData_pcst,
                                   NVM_E_LENGTH_RANDOM_NUMBER,
                                   NVM_PRV_NO_DET_ERROR))
    {
        if (E_OK == JobResult_pst->Crypto_st.Result_uo)
        {
            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
            NvM_Prv_SetInitVectorStatus(TRUE);
        }
        else
        {
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
            NvM_Prv_SetInitVectorStatus(FALSE);
        }

        *stJob_pen = NvM_Prv_stJob_GenerateRandomNr_Crypto_Completed_e;
    }
    // as long as Csm job has not completed and no result is available NvM remains pending in current state
#else
    // Set stJob_pen to already set value to avoid warnings that stJob_pen can be a pointer to const
    *stJob_pen = NvM_Prv_stJob_GenerateRandomNr_Crypto_Completed_e;
    // If function NvM_Prv_Crypto_StartRandomNrGeneration has been called with disabled random number generation
    // let the job fail
    JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
    (void)JobData_pcst;
#endif

    return idJobStateMachineNext_en;
}

#endif  // (NVM_CRYPTO_RANDOM_USED == STD_ON)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

