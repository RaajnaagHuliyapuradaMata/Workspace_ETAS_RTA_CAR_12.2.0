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
#include "NvM_Cfg_SchM.h"

#include "NvM_Cbk.h"

#include "rba_MemLib.h"

#include "NvM_Prv_Csm.h"
#include "NvM_Prv_Csm_Types.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_ErrorDetection.h"

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
# include "TestCd_NvM.h"
#endif

#if (NVM_CRYPTO_USED == STD_ON)
/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
 */
LOCAL_INLINE boolean NvM_Prv_Csm_GetJobResult(NvM_Prv_Crypto_ServiceResult_tst* JobResult_pst);

LOCAL_INLINE NvM_Prv_JobResult_ten NvM_Prv_Csm_GetJobStatus(uint8* cntrCsmJobRetry_puo,
                                                            Std_ReturnType ResultJobCsm_uo);

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define NVM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"

/**
 * \brief
 * Csm job result.
 * \details
 * This variable allows asynchronous evaluation of the Csm job result stored during the corresponding callback,
 * see NvM_Csm_Callback and NvM_Prv_Csm_SetJobResult.
 * Read access to this variable shall be done only with enabled scheduler lock, see NvM_Prv_Csm_GetJobResult
 */
static NvM_Prv_Csm_JobResult_tst NvM_Prv_Csm_JobResult_st;

#define NVM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

/**
 * \brief
 * This public function defines the callback for Csm jobs and sets corresponding results.
 * \details
 * This function stores the Csm job result and the Csm job ID for later evaluation in a static variable
 * (s. NvM_Prv_Csm_JobResult_st).
 * Typically a Csm job invokes a callback function if it finishes and this function enables the Csm job callback
 * to set the result and execute all required actions.
 * To ensure the results consistency this function writes Csm job results with active scheduler lock.
 *
 * \param[IN] jobId
 * Csm job ID for which the Csm has invoked this callback function.
 * \param[IN] result
 * Csm job result, possible values depend on the Csm job type.
 */
void NvM_Csm_Callback(uint32 jobId, Std_ReturnType result)
{
    SchM_Enter_NvM_Main();

    NvM_Prv_Csm_JobResult_st.idJobCallback_uo = jobId;
    NvM_Prv_Csm_JobResult_st.Result_uo = result;
    NvM_Prv_Csm_JobResult_st.isResultValid_b = TRUE;

    SchM_Exit_NvM_Main();
}


boolean NvM_Prv_Csm_CancelJob(NvM_Prv_Job_tst* Job_pst)
{
    boolean isJobCanceled_b = FALSE;
    Std_ReturnType stCancelJob_uo;
    // NvM calls this function only if there is a job ongoing
    stCancelJob_uo = Csm_CancelJob(Job_pst->JobResult_st.Crypto_st.idJobStarted_uo, CRYPTO_OPERATIONMODE_SINGLECALL);

    isJobCanceled_b = (E_OK == stCancelJob_uo);

    if (CRYPTO_E_JOB_CANCELED == stCancelJob_uo)
    {
        Job_pst->JobResult_st.Crypto_st.isJobCanceled_b = TRUE;
    }

    return isJobCanceled_b;
}

/**
 * \brief
 * This private function checks whether the result of the last Csm job is available and plausible.
 * \details
 * If result of the last Csm job is not plausible then this function reports given DET errors
 * (see NvM_Prv_ErrorDetection_IsCsmJobResultPlausible) and sets internal error NVM_E_RB_CRYPTO_SERVICE_FAILED.
 *
 * \param [INOUT] JobResult_pst
 * Pointer to the result of the cryptographic job
 * \param [IN] JobData_pcst
 * Pointer to the data of the current NvM job
 * \param [IN] idErrorPrimaryOutput_uo
 * DET error to  be set if primary output is not plausible
 * \param [IN] idErrorSecondaryOutput_uo
 * DET error to  be set if secondary output is not plausible
 */
boolean NvM_Prv_Csm_CheckJobResult(NvM_Prv_JobResult_tst* JobResult_pst,
                                   NvM_Prv_JobData_tst const* JobData_pcst,
                                   NvM_Prv_idDetError_tuo idErrorPrimaryOutput_uo,
                                   NvM_Prv_idDetError_tuo idErrorSecondaryOutput_uo)
{
    boolean isResultJobCsmAvailable_b = FALSE;
    if (NvM_Prv_Csm_GetJobResult(&JobResult_pst->Crypto_st))
    {
        isResultJobCsmAvailable_b = TRUE;

        if (   !NvM_Prv_ErrorDetection_IsCsmJobResultPlausible(JobData_pcst->idService_uo,
                                                               JobData_pcst->idBlock_uo,
                                                               idErrorPrimaryOutput_uo,
                                                               idErrorSecondaryOutput_uo,
                                                               &JobResult_pst->Crypto_st)
            || (E_OK != JobResult_pst->Crypto_st.Result_uo))
        {
            // A cryptographic service has failed -> report production error
            JobResult_pst->ProductionError_u8 |= NVM_E_RB_CRYPTO_SERVICE_FAILED;
        }
    }

    return isResultJobCsmAvailable_b;
}


/**
 * \brief
 * This private function starts given Csm job for a cryptographic job step function.
 * \details
 * This function starts given Csm job only if it is configured for the current block.
 * After starting the given Csm job this function checks the retry counter for Csm job starts.
 * If retry counter exceeds user configured number NVM_CSM_RETRY_COUNTER then this functions returns
 * that the Csm job start has failed.
 *
 * \param [INOUT] stJob_pen
 * State of the cryptographic job
 * \param [INOUT] JobResult_pst
 * Pointer to the result of the cryptographic job, possible values:
 * - NvM_Prv_JobResult_Pending_e   = start of cryptographic service has succeeded -> go to polling state
 * - NvM_Prv_JobResult_Succeeded_e = requested cryptographic service not configured-> go to next state
 * - NvM_Prv_JobResult_Failed_e    = start of cryptographic service has failed
 * \param [IN] JobData_pcst
 * Pointer to the data of the current NvM job
 * \param [IN] ServiceData_pcst
 * Pointer to the data for cryptographic service to be started
 * \param [IN] dataServiceStart_pcst
 * Pointer to the data required to start a cryptographic service
 */
void NvM_Prv_Csm_StartJob(NvM_Prv_stJob_ten* stJob_pen,
                          NvM_Prv_JobResult_tst* JobResult_pst,
                          NvM_Prv_JobData_tst const* JobData_pcst,
                          NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst,
                          NvM_Prv_Crypto_dataServiceStart_tst const* dataServiceStart_pcst)
{
    JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;

    if (dataServiceStart_pcst->isCryptoServiceConfigured_b)
    {
        JobResult_pst->Result_en = dataServiceStart_pcst->StartJobCsm_pfct(dataServiceStart_pcst->idJobToStart_uo,
                                                                           &JobResult_pst->cntrCsmJobRetry_uo,
                                                                           ServiceData_pcst);

        if (   NvM_Prv_ErrorDetection_IsCsmJobRetryLimitExceeded(JobData_pcst->idService_uo,
                                                                 JobData_pcst->idBlock_uo,
                                                                 &JobResult_pst->cntrCsmJobRetry_uo)
            || (NvM_Prv_JobResult_Failed_e == JobResult_pst->Result_en))
        {
            // TRACE[SWS_NvM_00977]
            // TRACE[SWS_NvM_00981]
            // Csm has rejected the start of the requested job for configured number of start attempts
            // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3243]
            // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3244]
            // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3246]
            // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3273]
            // Start of the requested Csm job has failed
            // -> cryptographic service has failed, report NvM internal error
            JobResult_pst->ProductionError_u8 |= NVM_E_RB_CRYPTO_SERVICE_FAILED;
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
        }
        else if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
        {
            JobResult_pst->Result_en = NvM_Prv_JobResult_Pending_e;
            *stJob_pen = dataServiceStart_pcst->stJobNextSuccess_en;
        }
        //
        else
        {
            JobResult_pst->Result_en = NvM_Prv_JobResult_Pending_e;
            // job state remains the same so that NvM can retry to start current cryptographic service in next cycle
        }
    }
    else
    {
        *stJob_pen = dataServiceStart_pcst->stJobNextSkipped_en;
    }
}

/**
 * \brief
 * This private function starts random number generation on the Csm.
 *
 * \param[IN] idJobCsm_uo
 * ID of the Csm job to be started.
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[INOUT] ServiceData_pcst
 * Pointer to the data structure for cryptographic services.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm job start succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm job start is pending due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm job start failed
 */
NvM_Prv_JobResult_ten NvM_Prv_Csm_StartRandomGeneration(uint32 idJobCsm_uo,
                                                        uint8* cntrCsmJobRetry_puo,
                                                        NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst)
{
# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

    if (TestCd_NvM_st.Arguments.CsmJobRandomSucceed_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_OK);
    }
    if (TestCd_NvM_st.Arguments.CsmJobStartFail_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_NOT_OK);
    }
    if (TestCd_NvM_st.Arguments.CsmJobBusy_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, CRYPTO_E_BUSY);
    }

# endif

# if (NVM_CRYPTO_RANDOM_USED == STD_ON)
    return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo,
                                    Csm_RandomGenerate(idJobCsm_uo,
                                                       ServiceData_pcst->outputPrimary_pu8,
                                                       ServiceData_pcst->outputPrimaryLength_pu32));
# else
    (void)idJobCsm_uo;
    (void)ServiceData_pcst;
    // Reset counter for Csm job retries for the next Csm job
    // This also avoids MISRA warning that cntrCsmJobRetry_puo can be a pointer to const
    *cntrCsmJobRetry_puo = 0u;
    return NvM_Prv_JobResult_Failed_e;
# endif
}

/**
 * \brief
 * This private function starts data encryption on the Csm.
 *
 * \param[IN] idJobCsm_uo
 * ID of the Csm job to be started.
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[INOUT] ServiceData_pcst
 * Pointer to the data structure for cryptographic services.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm job start succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm job start is pending due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm job start failed
 */
NvM_Prv_JobResult_ten NvM_Prv_Csm_StartEncryption(uint32 idJobCsm_uo,
                                                  uint8* cntrCsmJobRetry_puo,
                                                  NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst)
{
# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

    if (TestCd_NvM_st.Arguments.CsmJobStartFail_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_NOT_OK);
    }
    if (TestCd_NvM_st.Arguments.CsmJobBusy_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, CRYPTO_E_BUSY);
    }

# endif

# if (NVM_CRYPTO_ENCRYPTION_USED == STD_ON)
    return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo,
                                    Csm_Encrypt(idJobCsm_uo,
                                                CRYPTO_OPERATIONMODE_SINGLECALL,
                                                ServiceData_pcst->inputPrimary_pcu8,
                                                ServiceData_pcst->inputPrimaryLength_u32,
                                                ServiceData_pcst->outputPrimary_pu8,
                                                ServiceData_pcst->outputPrimaryLength_pu32));
# else
    (void)idJobCsm_uo;
    (void)ServiceData_pcst;
    // Reset counter for Csm job retries for the next Csm job
    // This also avoids MISRA warning that cntrCsmJobRetry_puo can be a pointer to const
    *cntrCsmJobRetry_puo = 0u;
    return NvM_Prv_JobResult_Failed_e;
# endif
}

/**
 * \brief
 * This private function starts data decryption on the Csm.
 *
 * \param[IN] idJobCsm_uo
 * ID of the Csm job to be started.
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[INOUT] ServiceData_pcst
 * Pointer to the data structure for cryptographic services.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm job start succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm job start is pending due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm job start failed
 */
NvM_Prv_JobResult_ten NvM_Prv_Csm_StartDecryption(uint32 idJobCsm_uo,
                                                  uint8* cntrCsmJobRetry_puo,
                                                  NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst)
{
# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

    if (TestCd_NvM_st.Arguments.CsmJobStartFail_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_NOT_OK);
    }
    if (TestCd_NvM_st.Arguments.CsmJobBusy_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, CRYPTO_E_BUSY);
    }

# endif

# if (NVM_CRYPTO_ENCRYPTION_USED == STD_ON)
    return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo,
                                    Csm_Decrypt(idJobCsm_uo,
                                                CRYPTO_OPERATIONMODE_SINGLECALL,
                                                ServiceData_pcst->inputPrimary_pcu8,
                                                ServiceData_pcst->inputPrimaryLength_u32,
                                                ServiceData_pcst->outputPrimary_pu8,
                                                ServiceData_pcst->outputPrimaryLength_pu32));
# else
    (void)idJobCsm_uo;
    (void)ServiceData_pcst;
    // Reset counter for Csm job retries for the next Csm job
    // This also avoids MISRA warning that cntrCsmJobRetry_puo can be a pointer to const
    *cntrCsmJobRetry_puo = 0u;
    return NvM_Prv_JobResult_Failed_e;
# endif
}

/**
 * \brief
 * This private function starts AEAD data encryption on the Csm.
 *
 * \param[IN] idJobCsm_uo
 * ID of the Csm job to be started.
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[INOUT] ServiceData_pcst
 * Pointer to the data structure for cryptographic services.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm job start succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm job start is pending due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm job start failed
 */
NvM_Prv_JobResult_ten NvM_Prv_Csm_StartEncryptionAead(uint32 idJobCsm_uo,
                                                      uint8* cntrCsmJobRetry_puo,
                                                      NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst)
{
# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

    if (TestCd_NvM_st.Arguments.CsmJobStartFail_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_NOT_OK);
    }
    if (TestCd_NvM_st.Arguments.CsmJobBusy_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, CRYPTO_E_BUSY);
    }
    if (   TestCd_NvM_st.Arguments.CsmJobAuthenticateSucceed_b
        && ((NULL_PTR == ServiceData_pcst->inputPrimary_pcu8) && (NULL_PTR == ServiceData_pcst->outputPrimary_pu8)))
    {
        // Prepare tag
        rba_MemLib_MemSet(ServiceData_pcst->outputSecondary_pu8, 0xAAu, *ServiceData_pcst->outputSecondaryLength_pu32);
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_OK);
    }

# endif

# if (NVM_CRYPTO_AEAD_ENCRYPTION_USED == STD_ON)
    return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo,
                                    Csm_AEADEncrypt(idJobCsm_uo,
                                                    CRYPTO_OPERATIONMODE_SINGLECALL,
                                                    ServiceData_pcst->inputPrimary_pcu8,
                                                    ServiceData_pcst->inputPrimaryLength_u32,
                                                    ServiceData_pcst->inputSecondary_pcu8,
                                                    ServiceData_pcst->inputSecondaryLength_u32,
                                                    ServiceData_pcst->outputPrimary_pu8,
                                                    ServiceData_pcst->outputPrimaryLength_pu32,
                                                    ServiceData_pcst->outputSecondary_pu8,
                                                    ServiceData_pcst->outputSecondaryLength_pu32));
# else
    (void)idJobCsm_uo;
    (void)ServiceData_pcst;
    // Reset counter for Csm job retries for the next Csm job
    // This also avoids MISRA warning that cntrCsmJobRetry_puo can be a pointer to const
    *cntrCsmJobRetry_puo = 0u;
    return NvM_Prv_JobResult_Failed_e;
# endif
}

/**
 * \brief
 * This private function starts AEAD data decryption on the Csm.
 *
 * \param[IN] idJobCsm_uo
 * ID of the Csm job to be started.
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[INOUT] ServiceData_pcst
 * Pointer to the data structure for cryptographic services.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm job start succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm job start is pending due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm job start failed
 */
NvM_Prv_JobResult_ten NvM_Prv_Csm_StartDecryptionAead(uint32 idJobCsm_uo,
                                                      uint8* cntrCsmJobRetry_puo,
                                                      NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst)
{
# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

    if (TestCd_NvM_st.Arguments.CsmJobStartFail_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_NOT_OK);
    }
    if (TestCd_NvM_st.Arguments.CsmJobBusy_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, CRYPTO_E_BUSY);
    }
    if (   TestCd_NvM_st.Arguments.CsmJobAuthenticateSucceed_b
        && ((NULL_PTR == ServiceData_pcst->inputPrimary_pcu8) && (NULL_PTR == ServiceData_pcst->outputPrimary_pu8)))
    {
        // Check tag
        if (rba_MemLib_MemCompareValue(ServiceData_pcst->inputTertiary_pcu8,
            0xAAu,
            ServiceData_pcst->inputTertiaryLength_u32))
        {
            *ServiceData_pcst->ResultVerification_puo = CRYPTO_E_VER_OK;
        }
        else
        {
            *ServiceData_pcst->ResultVerification_puo = CRYPTO_E_VER_NOT_OK;
        }
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_OK);
    }

# endif

# if (NVM_CRYPTO_AEAD_ENCRYPTION_USED == STD_ON)
    return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo,
                                    Csm_AEADDecrypt(idJobCsm_uo,
                                                    CRYPTO_OPERATIONMODE_SINGLECALL,
                                                    ServiceData_pcst->inputPrimary_pcu8,
                                                    ServiceData_pcst->inputPrimaryLength_u32,
                                                    ServiceData_pcst->inputSecondary_pcu8,
                                                    ServiceData_pcst->inputSecondaryLength_u32,
                                                    ServiceData_pcst->inputTertiary_pcu8,
                                                    ServiceData_pcst->inputTertiaryLength_u32,
                                                    ServiceData_pcst->outputPrimary_pu8,
                                                    ServiceData_pcst->outputPrimaryLength_pu32,
                                                    ServiceData_pcst->ResultVerification_puo));
# else
    (void)idJobCsm_uo;
    (void)ServiceData_pcst;
    // Reset counter for Csm job retries for the next Csm job
    // This also avoids MISRA warning that cntrCsmJobRetry_puo can be a pointer to const
    *cntrCsmJobRetry_puo = 0u;
    return NvM_Prv_JobResult_Failed_e;
# endif
}

/**
 * \brief
 * This private function starts signature generation on the Csm.
 *
 * \param[IN] idJobCsm_uo
 * ID of the Csm job to be started.
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[INOUT] ServiceData_pcst
 * Pointer to the data structure for cryptographic services.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm job start succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm job start is pending due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm job start failed
 */
NvM_Prv_JobResult_ten NvM_Prv_Csm_StartSignatureGeneration(uint32 idJobCsm_uo,
                                                           uint8* cntrCsmJobRetry_puo,
                                                           NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst)
{
# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

    if (TestCd_NvM_st.Arguments.CsmJobStartFail_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_NOT_OK);
    }
    if (TestCd_NvM_st.Arguments.CsmJobBusy_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, CRYPTO_E_BUSY);
    }

# endif

# if (NVM_CRYPTO_SIGNATURE_USED == STD_ON)
    return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo,
                                    Csm_SignatureGenerate(idJobCsm_uo,
                                                          CRYPTO_OPERATIONMODE_SINGLECALL,
                                                          ServiceData_pcst->inputPrimary_pcu8,
                                                          ServiceData_pcst->inputPrimaryLength_u32,
                                                          ServiceData_pcst->outputPrimary_pu8,
                                                          ServiceData_pcst->outputPrimaryLength_pu32));
# else
    (void)idJobCsm_uo;
    (void)ServiceData_pcst;
    // Reset counter for Csm job retries for the next Csm job
    // This also avoids MISRA warning that cntrCsmJobRetry_puo can be a pointer to const
    *cntrCsmJobRetry_puo = 0u;
    return NvM_Prv_JobResult_Failed_e;
# endif
}

/**
 * \brief
 * This private function starts signature verification on the Csm.
 *
 * \param[IN] idJobCsm_uo
 * ID of the Csm job to be started.
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[INOUT] ServiceData_pcst
 * Pointer to the data structure for cryptographic services.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm job start succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm job start is pending due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm job start failed
 */
NvM_Prv_JobResult_ten NvM_Prv_Csm_StartSignatureVerification(uint32 idJobCsm_uo,
                                                             uint8* cntrCsmJobRetry_puo,
                                                             NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst)
{
# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

    if (TestCd_NvM_st.Arguments.CsmJobStartFail_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_NOT_OK);
    }
    if (TestCd_NvM_st.Arguments.CsmJobBusy_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, CRYPTO_E_BUSY);
    }

# endif

# if (NVM_CRYPTO_SIGNATURE_USED == STD_ON)
    return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo,
                                    Csm_SignatureVerify(idJobCsm_uo,
                                                        CRYPTO_OPERATIONMODE_SINGLECALL,
                                                        ServiceData_pcst->inputPrimary_pcu8,
                                                        ServiceData_pcst->inputPrimaryLength_u32,
                                                        ServiceData_pcst->inputSecondary_pcu8,
                                                        ServiceData_pcst->inputSecondaryLength_u32,
                                                        ServiceData_pcst->ResultVerification_puo));
# else
    (void)idJobCsm_uo;
    (void)ServiceData_pcst;
    // Reset counter for Csm job retries for the next Csm job
    // This also avoids MISRA warning that cntrCsmJobRetry_puo can be a pointer to const
    *cntrCsmJobRetry_puo = 0u;
    return NvM_Prv_JobResult_Failed_e;
# endif
}

/**
 * \brief
 * This private function starts MAC (message authentication code) generation on the Csm.
 *
 * \param[IN] idJobCsm_uo
 * ID of the Csm job to be started.
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[INOUT] ServiceData_pcst
 * Pointer to the data structure for cryptographic services.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm job start succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm job start is pending due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm job start failed
 */
NvM_Prv_JobResult_ten NvM_Prv_Csm_StartMacGeneration(uint32 idJobCsm_uo,
                                                     uint8* cntrCsmJobRetry_puo,
                                                     NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst)
{
# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

    if (TestCd_NvM_st.Arguments.CsmJobStartFail_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_NOT_OK);
    }
    if (TestCd_NvM_st.Arguments.CsmJobBusy_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, CRYPTO_E_BUSY);
    }

# endif

# if (NVM_CRYPTO_MAC_USED == STD_ON)
    return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, Csm_MacGenerate(idJobCsm_uo,
                                                                         CRYPTO_OPERATIONMODE_SINGLECALL,
                                                                         ServiceData_pcst->inputPrimary_pcu8,
                                                                         ServiceData_pcst->inputPrimaryLength_u32,
                                                                         ServiceData_pcst->outputPrimary_pu8,
                                                                         ServiceData_pcst->outputPrimaryLength_pu32));
# else
    (void)idJobCsm_uo;
    (void)ServiceData_pcst;
    // Reset counter for Csm job retries for the next Csm job
    // This also avoids MISRA warning that cntrCsmJobRetry_puo can be a pointer to const
    *cntrCsmJobRetry_puo = 0u;
    return NvM_Prv_JobResult_Failed_e;
# endif
}

/**
 * \brief
 * This private function starts MAC (message authentication code) verification on the Csm.
 *
 * \param[IN] idJobCsm_uo
 * ID of the Csm job to be started.
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[INOUT] ServiceData_pcst
 * Pointer to the data structure for cryptographic services.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm job start succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm job start is pending due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm job start failed
 */
NvM_Prv_JobResult_ten NvM_Prv_Csm_StartMacVerification(uint32 idJobCsm_uo,
                                                       uint8* cntrCsmJobRetry_puo,
                                                       NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst)
{
# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))

    if (TestCd_NvM_st.Arguments.CsmJobStartFail_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, E_NOT_OK);
    }
    if (TestCd_NvM_st.Arguments.CsmJobBusy_b)
    {
        return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo, CRYPTO_E_BUSY);
    }

# endif

# if (NVM_CRYPTO_MAC_USED == STD_ON)
    return NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo,
                                    Csm_MacVerify(idJobCsm_uo,
                                                  CRYPTO_OPERATIONMODE_SINGLECALL,
                                                  ServiceData_pcst->inputPrimary_pcu8,
                                                  ServiceData_pcst->inputPrimaryLength_u32,
                                                  ServiceData_pcst->inputSecondary_pcu8,
                                                  ServiceData_pcst->inputSecondaryLength_u32,
                                                  ServiceData_pcst->ResultVerification_puo));
# else
    (void)idJobCsm_uo;
    (void)ServiceData_pcst;
    // Reset counter for Csm job retries for the next Csm job
    // This also avoids MISRA warning that cntrCsmJobRetry_puo can be a pointer to const
    *cntrCsmJobRetry_puo = 0u;
    return NvM_Prv_JobResult_Failed_e;
# endif
}

/**
 * \brief
 * This private function sets initialization vector for the given key.
 *
 * \param[IN] idKeyCsm_uo
 * ID of the Csm key to set initialization vector.
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[INOUT] ServiceData_pcst
 * Pointer to the data structure for cryptographic services.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm key element setting succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm key element setting is pending due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm key element setting failed
 */
NvM_Prv_JobResult_ten NvM_Prv_Csm_SetKeyInitVector(uint32 idKeyCsm_uo,
                                                   uint8* cntrCsmJobRetry_puo,
                                                   NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst)
{
# if (NVM_CRYPTO_KEY_SETTING_USED == STD_ON)
    NvM_Prv_JobResult_ten stSetKeyInitVector_en;

    stSetKeyInitVector_en = NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo,
                                                     Csm_KeyElementSet(idKeyCsm_uo,
                                                                       CRYPTO_KE_CIPHER_IV,
                                                                       ServiceData_pcst->inputPrimary_pcu8,
                                                                       ServiceData_pcst->inputPrimaryLength_u32));

    if (NvM_Prv_JobResult_Succeeded_e == stSetKeyInitVector_en)
    {
        NvM_Prv_Csm_JobResult_st.idJobCallback_uo = idKeyCsm_uo;
        NvM_Prv_Csm_JobResult_st.Result_uo = E_OK;
        NvM_Prv_Csm_JobResult_st.isResultValid_b = TRUE;
    }

    return stSetKeyInitVector_en;
# else
    (void)idKeyCsm_uo;
    (void)ServiceData_pcst;
    // Reset counter for Csm job retries for the next Csm job
    // This also avoids MISRA warning that cntrCsmJobRetry_puo can be a pointer to const
    *cntrCsmJobRetry_puo = 0u;
    return NvM_Prv_JobResult_Failed_e;
# endif
}

/**
 * \brief
 * This private function sets the given key valid.
 *
 * \param[IN] idKeyCsm_uo
 * ID of the Csm key to set valid.
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[INOUT] ServiceData_pcst
 * Pointer to the data structure for cryptographic services.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm key validation succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm key validation is pending due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm job start failed
 */
NvM_Prv_JobResult_ten NvM_Prv_Csm_SetKeyValid(uint32 idKeyCsm_uo,
                                              uint8* cntrCsmJobRetry_puo,
                                              NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst)
{
# if (NVM_CRYPTO_KEY_SETTING_USED == STD_ON)
    NvM_Prv_JobResult_ten stSetKeyValid_en;

    (void)ServiceData_pcst;
    stSetKeyValid_en = NvM_Prv_Csm_GetJobStatus(cntrCsmJobRetry_puo,
                                                Csm_KeySetValid(idKeyCsm_uo));

    if (NvM_Prv_JobResult_Succeeded_e == stSetKeyValid_en)
    {
        NvM_Prv_Csm_JobResult_st.idJobCallback_uo = idKeyCsm_uo;
        NvM_Prv_Csm_JobResult_st.Result_uo = E_OK;
        NvM_Prv_Csm_JobResult_st.isResultValid_b = TRUE;
    }

    return stSetKeyValid_en;
# else
    (void)ServiceData_pcst;
    (void)idKeyCsm_uo;
    // Reset counter for Csm job retries for the next Csm job
    // This also avoids MISRA warning that cntrCsmJobRetry_puo can be a pointer to const
    *cntrCsmJobRetry_puo = 0u;
    return NvM_Prv_JobResult_Failed_e;
# endif
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Inline code
 **********************************************************************************************************************
 */
/**
 * \brief
 * This local private function provides access to the result of the last Csm job for its evaluation.
 * \details
 * This function copies the result of the last Csm job into the result structure for cryptographic services
 * provided by the caller. The copying is always done with active scheduler lock.
 * This function copies the result only if it is valid, i.e. the Csm has set the result via corresponding callback.
 * Afterwards this functions invalidates the result.
 *
 * \param[OUT] JobResult_pst
 * Pointer to the result structure for cryptographic services.
 *
 * \return
 * Information whether result of the last Csm job has been copied into the passed result structure
 * - true = valid result is available and has been copied
 * - false = no valid result is available
 */
LOCAL_INLINE boolean NvM_Prv_Csm_GetJobResult(NvM_Prv_Crypto_ServiceResult_tst* JobResult_pst)
{
    boolean isResultJobCsmAvailable_b = FALSE;

# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    if (   TestCd_NvM_st.Arguments.CsmJobRandomSucceed_b
        && (NVM_PRV_ID_CSM_JOB_RANDOM == JobResult_pst->idJobStarted_uo))
    {
        NvM_Prv_Csm_JobResult_st.idJobCallback_uo = NVM_PRV_ID_CSM_JOB_RANDOM;
        NvM_Prv_Csm_JobResult_st.Result_uo = E_OK;
        NvM_Prv_Csm_JobResult_st.isResultValid_b = TRUE;
    }
    if (   TestCd_NvM_st.Arguments.CsmJobAuthenticateSucceed_b
        && (   (CsmConf_CsmJob_NvM_AEAD_Decrypt == JobResult_pst->idJobStarted_uo)
            || (CsmConf_CsmJob_NvM_AEAD_Encrypt == JobResult_pst->idJobStarted_uo))
       )
    {
        NvM_Prv_Csm_JobResult_st.idJobCallback_uo = JobResult_pst->idJobStarted_uo;
        NvM_Prv_Csm_JobResult_st.Result_uo = E_OK;
        NvM_Prv_Csm_JobResult_st.isResultValid_b = TRUE;
    }
#endif

    SchM_Enter_NvM_Main();

    if (NvM_Prv_Csm_JobResult_st.isResultValid_b)
    {
        // Csm has invoked the configured callback
        isResultJobCsmAvailable_b = TRUE;

        JobResult_pst->idJobCallback_uo = NvM_Prv_Csm_JobResult_st.idJobCallback_uo;
        JobResult_pst->Result_uo = NvM_Prv_Csm_JobResult_st.Result_uo;
        NvM_Prv_Csm_JobResult_st.isResultValid_b = FALSE;
    }

    SchM_Exit_NvM_Main();

# if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
    if (isResultJobCsmAvailable_b)
    {
        if (TestCd_NvM_st.Arguments.CsmJobFail_b)
        {
            JobResult_pst->Result_uo = E_NOT_OK;
        }
        if (TestCd_NvM_st.Arguments.CsmJobPrimaryWrongLength_b)
        {
            JobResult_pst->outputPrimaryLength_u32 = JobResult_pst->outputPrimaryLengthConfigured_u32 + 1u;
            JobResult_pst->Result_uo = E_NOT_OK;
        }
        if (TestCd_NvM_st.Arguments.CsmJobVerificationFail_b)
        {
            JobResult_pst->ResultVerification_uo = CRYPTO_E_VER_NOT_OK;
        }
    }
# endif

    return isResultJobCsmAvailable_b;
}

/**
 * \brief
 * This local private function evaluates the Csm job status.
 * \details
 * If Csm job cannot be started due to Csm internal state then this function increases provided retry counter
 * for Csm job starts.
 *
 * \param[INOUT] cntrCsmJobRetry_puo
 * Retry counter for Csm job starts.
 * \param[IN] ResultJobCsm_uo
 * Result of the start attempt of a Csm job.
 *
 * \return
 * Csm job status:
 * - NvM_Prv_JobResult_Succeeded_e = Csm job start succeeded
 * - NvM_Prv_JobResult_Pending_e = Csm job cannot be started due to Csm internal state
 * - NvM_Prv_JobResult_Failed_e = Csm job start failed
 */
LOCAL_INLINE NvM_Prv_JobResult_ten NvM_Prv_Csm_GetJobStatus(uint8* cntrCsmJobRetry_puo,
                                                            Std_ReturnType ResultJobCsm_uo)
{
    NvM_Prv_JobResult_ten stJobCsm_en;

    if (E_OK == ResultJobCsm_uo)
    {
        stJobCsm_en = NvM_Prv_JobResult_Succeeded_e;
        // Reset counter for Csm job retries for the next Csm job
        *cntrCsmJobRetry_puo = 0u;
    }
    else if (CRYPTO_E_BUSY == ResultJobCsm_uo)
    {
        ++(*cntrCsmJobRetry_puo);
        stJobCsm_en = NvM_Prv_JobResult_Pending_e;
    }
    else
    {
        stJobCsm_en = NvM_Prv_JobResult_Failed_e;
    }

    return stJobCsm_en;
}

#endif  // (NVM_CRYPTO_USED == STD_ON)

