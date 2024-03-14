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

#include "rba_MemLib.h"
#include "rba_MemLib_Special.h"

#include "NvM_Prv.h"
#include "NvM_Prv_Crc.h"
#include "NvM_Prv_Csm.h"
#include "NvM_Prv_Crypto.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_ErrorDetection.h"

/* Include the actual definitions of the provided inline functions */
#include "NvM_Prv_Crc_Inl.h"

#if (defined(TESTCD_NVM_ENABLED) && (TESTCD_NVM_ENABLED == STD_ON))
# include "TestCd_NvM.h"
#endif

/*
 **********************************************************************************************************************
 * Inline declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"
#if (NVM_CRYPTO_USED == STD_ON)

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_CryptoWrite_PrepareEncryption(NvM_Prv_JobResult_tst* JobResult_pst,
                                                                   NvM_Prv_JobData_tst const* JobData_pcst,
                                                                   NvM_Prv_Crypto_ServiceData_tst* JobCryptoData_pst);

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_CryptoWrite_PrepareAuthentication(NvM_Prv_JobResult_tst* JobResult_pst,
                                                                       NvM_Prv_JobData_tst const* JobData_pcst,
                                                                       NvM_Prv_Crypto_ServiceData_tst* JobCryptoData_pst);

#endif  // (NVM_CRYPTO_USED == STD_ON)
#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

#if (NVM_CRYPTO_USED == STD_ON)

RBA_MEMLIB_STATIC boolean NvM_Prv_CryptoWrite_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_CryptoWrite_GetStateFct(NvM_Prv_stJob_ten stJob_en);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_GetKeyInitVector(NvM_Prv_stJob_ten* stJob_pen,
                                                                                     NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                     NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_DoKeySetting(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_StartEncryption(NvM_Prv_stJob_ten* stJob_pen,
                                                                                    NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                    NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_PollEncryption(NvM_Prv_stJob_ten* stJob_pen,
                                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                   NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_StartSignatureGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                             NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                             NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_PollSignatureGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                            NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                            NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_StartMacGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                       NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                       NvM_Prv_JobData_tst const* JobData_pcst);

RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_PollMacGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                      NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                      NvM_Prv_JobData_tst const* JobData_pcst);

#endif  // (NVM_CRYPTO_USED == STD_ON)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

#if (NVM_CRYPTO_USED == STD_ON)

RBA_MEMLIB_STATIC_CONST NvM_Prv_Job_StateMachine_Data_tst NvM_Prv_CryptoWrite_Data_cst =
{
        &NvM_Prv_CryptoWrite_GetStateFct,
        &NvM_Prv_CryptoWrite_IsEnabled,
        NvM_Prv_stJob_Write_Crypto_GetKeyInitVector_e,
        NvM_Prv_stJob_Write_DoCrypto_e,
        NvM_Prv_JobResource_Cluster_Crypto_e

};

#endif  // (NVM_CRYPTO_USED == STD_ON)

#define NVM_STOP_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
# define NVM_START_SEC_CODE
# include "NvM_MemMap.h"

NvM_Prv_Job_StateMachine_Data_tst const* NvM_Prv_CryptoWrite_GetData(void)
{
#if (NVM_CRYPTO_USED == STD_ON)
    return &NvM_Prv_CryptoWrite_Data_cst;
#else
    return NULL_PTR;
#endif
}

#if (NVM_CRYPTO_USED == STD_ON)

RBA_MEMLIB_STATIC boolean NvM_Prv_CryptoWrite_IsEnabled(NvM_Prv_JobData_tst const* JobData_pcst)
{
    // Cryptographic write job is enabled only for blocks with configured cryptographic features
    return NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_CRYPTO);
}

/**
 * \brief
 * This private function returns the pointer to the job step for the current state of the cryptographic write job.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 *
 * \param[IN] stJob_en
 * Current state of the cryptographic write job
 *
 * \return
 * Pointer to the job step
 */
RBA_MEMLIB_STATIC NvM_Prv_Job_State_tpfct NvM_Prv_CryptoWrite_GetStateFct(NvM_Prv_stJob_ten stJob_en)
{
    NvM_Prv_Job_State_tpfct JobStateCryptoWrite_pfct = NULL_PTR;

    switch (stJob_en)
    {
        case NvM_Prv_stJob_Write_Crypto_GetKeyInitVector_e:
            JobStateCryptoWrite_pfct = &NvM_Prv_CryptoWrite_GetKeyInitVector;
        break;

        case NvM_Prv_stJob_Write_Crypto_DoKeySetting_e:
            JobStateCryptoWrite_pfct = &NvM_Prv_CryptoWrite_DoKeySetting;
        break;

        case NvM_Prv_stJob_Write_Crypto_StartEncryption_e:
            JobStateCryptoWrite_pfct = &NvM_Prv_CryptoWrite_StartEncryption;
        break;

        case NvM_Prv_stJob_Write_Crypto_PollEncryption_e:
            JobStateCryptoWrite_pfct = &NvM_Prv_CryptoWrite_PollEncryption;
        break;

        case NvM_Prv_stJob_Write_Crypto_StartGenerationSignature_e:
            JobStateCryptoWrite_pfct = &NvM_Prv_CryptoWrite_StartSignatureGeneration;
        break;

        case NvM_Prv_stJob_Write_Crypto_PollGenerationSignature_e:
            JobStateCryptoWrite_pfct = &NvM_Prv_CryptoWrite_PollSignatureGeneration;
        break;

        case NvM_Prv_stJob_Write_Crypto_StartGenerationMac_e:
            JobStateCryptoWrite_pfct = &NvM_Prv_CryptoWrite_StartMacGeneration;
        break;

        case NvM_Prv_stJob_Write_Crypto_PollGenerationMac_e:
            JobStateCryptoWrite_pfct = &NvM_Prv_CryptoWrite_PollMacGeneration;
        break;

        default:
            JobStateCryptoWrite_pfct = NULL_PTR;
        break;
    }

    return JobStateCryptoWrite_pfct;
}

/**
 * \brief
 * This local private function is a cryptographic job step function and prepares the initialization vector for the key
 * used for the encryption / authentication.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 * This function checks if initialization vector usage is configured for the current block and initialization vector
 * is available (a random number has been generated before). If so then this function updates initialization vector and
 * copies it into the internal buffer for later usage.
 *
 * \param [inout] stJob_pen
 * State of the cryptographic job
 * \param [inout] JobResult_pst
 * Pointer to the result of the cryptographic job, possible values:
 * - NvM_Prv_JobResult_Succeeded_e = initialization vector prepared successfully or
 *                                   no initialization vector is configured for current block
 * - NvM_Prv_JobResult_Failed_e    = initialization vector not available, i.e. no random number has been generated before
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_GetKeyInitVector(NvM_Prv_stJob_ten* stJob_pen,
                                                                                     NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                     NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoWrite_e;

#if (NVM_CRYPTO_RANDOM_USED == STD_ON)
    uint16 LengthInitVector_u16 = NvM_Prv_BlkDesc_GetLengthJobCsm(JobData_pcst->idBlock_uo,
                                                                  NvM_Prv_Crypto_idService_InitVector_e);

    if (0u != LengthInitVector_u16)
    {
        uint16 PositionInitVector_u16;

        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3366] initialization vector check:
        //                                          - if available (random number generated successfully before)
        //                                            -> set init vector and encrypt user data before writing
        //                                          - if not available (random number generation failed or not done)
        //                                            -> report DET error, encryption shall be still possible
        NvM_Prv_ErrorDetection_ReportInitVectorNotAvailable(JobData_pcst->idService_uo, JobData_pcst->idBlock_uo);

        PositionInitVector_u16 = NvM_Prv_BlkDesc_GetPositionJobCsmInitVector(JobData_pcst->idBlock_uo);

        // Update IV of the key configured for current block
        NvM_Prv_IncrementInitVector();
        // copy initialization vector into the internal buffer
        rba_MemLib_MemCopy(NvM_Prv_GetInitVectorCryptoKey(),
                           &((JobData_pcst->IntBuffer_st.Buffer_pu8)[PositionInitVector_u16]),
                           LengthInitVector_u16);

        JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
        *stJob_pen = NvM_Prv_stJob_Write_Crypto_DoKeySetting_e;
        idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoKeySetting_e;
    }
    else
#endif
    {
        (void)JobData_pcst;
        JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
        *stJob_pen = NvM_Prv_stJob_Write_Crypto_StartEncryption_e;
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a cryptographic job step function and executes a sub-state machine to set key elements
 * required for user data encryption / authentication.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 *
 * \param [inout] stJob_pen
 * State of the cryptographic job
 * \param [inout] JobResult_pst
 * Pointer to the result of the cryptographic job, possible values:
 * - NvM_Prv_JobResult_Succeeded_e = setting of key elements succeeded or
 *                                   no key elements configured for current block
 * - NvM_Prv_JobResult_Pending_e   = setting of key elements still pending -> continue in next cycle
 * - NvM_Prv_JobResult_Failed_e    = setting of key elements failed
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
/* MR12 RULE 8.13 VIOLATION: This function does not update the result structure but all Job-State-functions shall
 * have the same signature and especially the result-structure has to be changeable so that the result can be modified
 * if necessary. */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_DoKeySetting(NvM_Prv_stJob_ten* stJob_pen,
                                                                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                 NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoWrite_e;

    (void)JobData_pcst;

    if (NvM_Prv_JobResult_Succeeded_e == JobResult_pst->Result_en)
    {
        // cryptographic services for key setting have succeeded or are not configured
        // -> start encryption
        *stJob_pen = NvM_Prv_stJob_Write_Crypto_StartEncryption_e;
    }
    else
    {
        // cryptographic services for key setting have failed
        // -> finish cryptographic services for write job
        *stJob_pen = NvM_Prv_stJob_Write_DoCrypto_e;
    }

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a cryptographic job step function and starts the cryptographic service
 * for data encryption / authentication.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 * This function only prepares all required data. The actual start is done by the function NvM_Prv_Csm_StartJob.
 *
 * \param [inout] stJob_pen
 * State of the cryptographic job
 * \param [inout] JobResult_pst
 * Pointer to the result of the cryptographic job, possible values:
 * - NvM_Prv_JobResult_Pending_e   = data encryption cannot be started due to busy Csm -> retry in next cycle
 * - NvM_Prv_JobResult_Succeeded_e = start of data encryption has succeeded or no data encryption configured
 *                                   -> start signature generation
 * - NvM_Prv_JobResult_Failed_e    = start of data encryption has failed
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_StartEncryption(NvM_Prv_stJob_ten* stJob_pen,
                                                                                    NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                    NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoWrite_e;

    NvM_Prv_Crypto_dataServiceStart_tst dataJobStart_st;
    NvM_Prv_Crypto_ServiceData_tst jobCryptoData_st;
    uint16 LengthEncryptedData_u16 = NvM_Prv_BlkDesc_GetLengthJobCsm(JobData_pcst->idBlock_uo,
                                                                     NvM_Prv_Crypto_idService_Encrypt_e);
    uint16 LengthTag_u16 = NvM_Prv_BlkDesc_GetLengthJobCsmTag(JobData_pcst->idBlock_uo);

    jobCryptoData_st.inputPrimary_pcu8          = NULL_PTR;
    jobCryptoData_st.inputPrimaryLength_u32     = 0u;
    jobCryptoData_st.inputSecondary_pcu8        = NULL_PTR;
    jobCryptoData_st.inputSecondaryLength_u32   = 0u;
    jobCryptoData_st.inputTertiary_pcu8         = NULL_PTR;
    jobCryptoData_st.inputTertiaryLength_u32    = 0u;
    jobCryptoData_st.outputPrimary_pu8          = NULL_PTR;
    jobCryptoData_st.outputPrimaryLength_pu32   = NULL_PTR;
    jobCryptoData_st.outputSecondary_pu8        = NULL_PTR;
    jobCryptoData_st.outputSecondaryLength_pu32 = NULL_PTR;
    jobCryptoData_st.ResultVerification_puo     = NULL_PTR;

    // Prefill job results structure for data encryption
    JobResult_pst->Crypto_st.idServiceCrypto_en = NvM_Prv_Crypto_idService_Encrypt_e;
    JobResult_pst->Crypto_st.idJobStarted_uo = NvM_Prv_BlkDesc_GetIdJobCsm(JobData_pcst->idBlock_uo,
                                                                           NvM_Prv_Crypto_idService_Encrypt_e);
    // member idJobCallback_uo will be set in the callback if Csm fom has completed
    JobResult_pst->Crypto_st.outputPrimaryLength_u32 = LengthEncryptedData_u16;
    JobResult_pst->Crypto_st.outputPrimaryLengthConfigured_u32 = LengthEncryptedData_u16;
    JobResult_pst->Crypto_st.outputSecondaryLength_u32 = LengthTag_u16;
    JobResult_pst->Crypto_st.outputSecondaryLengthConfigured_u32 = LengthTag_u16;
    // member Result_uo will be set in the callback if Csm job has completed
    JobResult_pst->Crypto_st.ResultVerification_uo = (Std_ReturnType)CRYPTO_E_VER_OK;   // not used for data encryption
    JobResult_pst->Crypto_st.isJobCanceled_b = FALSE;

    // Prefill data structure for a cryptographic job dependent on encryption / authentication
    if (0u != LengthEncryptedData_u16)
    {
        // encryption is configured
        // preset used size in bytes within internal buffer
        *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = LengthEncryptedData_u16;
        NvM_Prv_CryptoWrite_PrepareEncryption(JobResult_pst, JobData_pcst, &jobCryptoData_st);
    }
    else
    {
        // no encryption is required
        // -> check authentication
        if (0u != LengthTag_u16)
        {
            // authentication is configured
            NvM_Prv_CryptoWrite_PrepareAuthentication(JobResult_pst, JobData_pcst, &jobCryptoData_st);
        }
        // if no encryption and no authentication is configured then current job step wil be skipped
    }

    dataJobStart_st.stJobNextSuccess_en = NvM_Prv_stJob_Write_Crypto_PollEncryption_e;
    dataJobStart_st.stJobNextSkipped_en = NvM_Prv_stJob_Write_Crypto_StartGenerationSignature_e;
    dataJobStart_st.isCryptoServiceConfigured_b = ((0u != LengthEncryptedData_u16) || (0u != LengthTag_u16));
    dataJobStart_st.idJobToStart_uo = JobResult_pst->Crypto_st.idJobStarted_uo;

    if (0u != LengthTag_u16)
    {
        // tag configured -> use AEAD data encryption
        dataJobStart_st.StartJobCsm_pfct = &NvM_Prv_Csm_StartEncryptionAead;
    }
    else
    {
        // no tag configured -> use standard data encryption
        dataJobStart_st.StartJobCsm_pfct = &NvM_Prv_Csm_StartEncryption;
    }

    NvM_Prv_Csm_StartJob(stJob_pen, JobResult_pst, JobData_pcst, &jobCryptoData_st, &dataJobStart_st);

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a cryptographic job step function and polls results of cryptographic services
 * for data encryption / authentication.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 * As soon as results of data encryption / authentication are available and plausible (see NvM_Prv_Csm_CheckJobResult)
 * this function updates the result of the job step.
 *
 * \param [inout] stJob_pen
 * State of the cryptographic job
 * \param [inout] JobResult_pst
 * Pointer to the result of the cryptographic job, possible values:
 * - NvM_Prv_JobResult_Pending_e            = data encryption / authentication still pending
 * - NvM_Prv_JobResult_Succeeded_e          = data encryption / authentication has succeeded
 * - NvM_Prv_JobResult_Failed_e             = data encryption / authentication has failed
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_PollEncryption(NvM_Prv_stJob_ten* stJob_pen,
                                                                                   NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                   NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoWrite_e;

    // check whether the Csm job has completed and result is available
    if (NvM_Prv_Csm_CheckJobResult(JobResult_pst,
                                   JobData_pcst,
                                   NVM_E_BLOCK_CIPHER_LENGTH_MISSMATCH,
                                   NVM_E_LENGTH_AEAD_TAG))
    {
        if (E_OK == JobResult_pst->Crypto_st.Result_uo)
        {
            *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = (uint16)(   *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16
                                                                        + JobResult_pst->Crypto_st.outputSecondaryLength_u32);
            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
            *stJob_pen = NvM_Prv_stJob_Write_Crypto_StartGenerationSignature_e;
        }
        else
        {
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
            *stJob_pen = NvM_Prv_stJob_Write_DoCrypto_e;
        }
    }
    // as long as Csm job has not completed and no result is available NvM remains pending in current state

    return idJobStateMachineNext_en;
}

/**
 * This local private function is a cryptographic job step function and starts the cryptographic service
 * for the signature generation.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 * This function only prepares all required data. The actual start is done by the function NvM_Prv_Csm_StartJob.
 *
 * \param [inout] stJob_pen
 * State of the cryptographic job
 * \param [inout] JobResult_pst
 * Pointer to the result of the cryptographic job, possible values:
 * - NvM_Prv_JobResult_Pending_e   = signature generation cannot be started due to busy Csm -> retry in next cycle
 * - NvM_Prv_JobResult_Succeeded_e = start of signature generation has succeeded or no signature generation configured
 *                                   -> start MAC generation
 * - NvM_Prv_JobResult_Failed_e    = start of signature generation has failed
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_StartSignatureGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                             NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                             NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoWrite_e;

    NvM_Prv_Crypto_dataServiceStart_tst dataJobStart_st;
    NvM_Prv_Crypto_ServiceData_tst jobCryptoData_st;
    uint32 lengthSignature_u32 = NvM_Prv_BlkDesc_GetLengthJobCsm(JobData_pcst->idBlock_uo,
                                                                 NvM_Prv_Crypto_idService_GenerateSignature_e);


    // Prefill job results structure for signature generation
    JobResult_pst->Crypto_st.idServiceCrypto_en = NvM_Prv_Crypto_idService_GenerateSignature_e;
    JobResult_pst->Crypto_st.idJobStarted_uo = NvM_Prv_BlkDesc_GetIdJobCsm(JobData_pcst->idBlock_uo,
                                                                           NvM_Prv_Crypto_idService_GenerateSignature_e);
    // member idJobCallback_uo will be set in the callback if Csm fom has completed
    JobResult_pst->Crypto_st.outputPrimaryLength_u32 = lengthSignature_u32;
    JobResult_pst->Crypto_st.outputPrimaryLengthConfigured_u32 = lengthSignature_u32;
    JobResult_pst->Crypto_st.outputSecondaryLength_u32 = 0u;              // not used for signature generation
    JobResult_pst->Crypto_st.outputSecondaryLengthConfigured_u32 = 0u;    // not used for signature generation
    // member Result_uo will be set in the callback if Csm job has completed
    JobResult_pst->Crypto_st.ResultVerification_uo = CRYPTO_E_VER_OK;     // not used for signature generation
    JobResult_pst->Crypto_st.isJobCanceled_b = FALSE;

    // Prefill data structure for a cryptographic job
    jobCryptoData_st.inputPrimary_pcu8 = JobData_pcst->IntBuffer_st.Buffer_pu8;
    jobCryptoData_st.inputPrimaryLength_u32 = *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16;
    jobCryptoData_st.outputPrimary_pu8 = &(JobData_pcst->IntBuffer_st.Buffer_pu8[jobCryptoData_st.inputPrimaryLength_u32]);
    jobCryptoData_st.outputPrimaryLength_pu32 = &JobResult_pst->Crypto_st.outputPrimaryLength_u32;
    // all other members of jobCsmData_st are not required to start signature generation

    dataJobStart_st.stJobNextSuccess_en = NvM_Prv_stJob_Write_Crypto_PollGenerationSignature_e;
    dataJobStart_st.stJobNextSkipped_en = NvM_Prv_stJob_Write_Crypto_StartGenerationMac_e;
    dataJobStart_st.isCryptoServiceConfigured_b = (0u != lengthSignature_u32);
    dataJobStart_st.idJobToStart_uo = JobResult_pst->Crypto_st.idJobStarted_uo;
    dataJobStart_st.StartJobCsm_pfct = &NvM_Prv_Csm_StartSignatureGeneration;

    NvM_Prv_Csm_StartJob(stJob_pen, JobResult_pst, JobData_pcst, &jobCryptoData_st, &dataJobStart_st);

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a cryptographic job step function and polls results of cryptographic services
 * for signature generation.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 * As soon as results of signature generation are available and plausible (see NvM_Prv_Csm_CheckJobResult)
 * this function updates the result of the job step.
 *
 * \param [inout] stJob_pen
 * State of the cryptographic job
 * \param [inout] JobResult_pst
 * Pointer to the result of the cryptographic job, possible values:
 * - NvM_Prv_JobResult_Pending_e            = signature generation still pending
 * - NvM_Prv_JobResult_Succeeded_e          = signature generation has succeeded
 * - NvM_Prv_JobResult_Failed_e             = signature generation has failed
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_PollSignatureGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                            NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                            NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoWrite_e;

    // check whether the Csm job has completed and result is available
    if (NvM_Prv_Csm_CheckJobResult(JobResult_pst,
                                   JobData_pcst,
                                   NVM_E_LENGTH_SIGNATURE,
                                   NVM_PRV_NO_DET_ERROR))
    {
        if (E_OK == JobResult_pst->Crypto_st.Result_uo)
        {
            *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = (uint16)(  *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16
                                                                        + JobResult_pst->Crypto_st.outputPrimaryLength_u32);
            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
            *stJob_pen = NvM_Prv_stJob_Write_Crypto_StartGenerationMac_e;
        }
        else
        {
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
            *stJob_pen = NvM_Prv_stJob_Write_DoCrypto_e;
        }
    }
    // as long as Csm job has not completed and no result is available NvM remains pending in current state

    return idJobStateMachineNext_en;
}

/**
 * This local private function is a cryptographic job step function and starts the cryptographic service
 * for the MAC (message authentication code) generation.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 * This function only prepares all required data. The actual start is done by the function NvM_Prv_Csm_StartJob.
 *
 * \param [inout] stJob_pen
 * State of the cryptographic job
 * \param [inout] JobResult_pst
 * Pointer to the result of the cryptographic job, possible values:
 * - NvM_Prv_JobResult_Pending_e   = MAC generation cannot be started due to busy Csm -> retry in next cycle
 * - NvM_Prv_JobResult_Succeeded_e = start of MAC generation has succeeded or no MAC generation configured
 *                                   -> finish cryptographic job
 * - NvM_Prv_JobResult_Failed_e    = start of MAC generation has failed
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_StartMacGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                       NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                       NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoWrite_e;

    NvM_Prv_Crypto_dataServiceStart_tst dataJobStart_st;
    NvM_Prv_Crypto_ServiceData_tst jobCryptoData_st;
    uint32 lengthMac_u32 = NvM_Prv_BlkDesc_GetLengthJobCsm(JobData_pcst->idBlock_uo,
                                                           NvM_Prv_Crypto_idService_GenerateMac_e);


    // Prefill job results structure for MAC generation
    JobResult_pst->Crypto_st.idServiceCrypto_en = NvM_Prv_Crypto_idService_GenerateMac_e;
    JobResult_pst->Crypto_st.idJobStarted_uo = NvM_Prv_BlkDesc_GetIdJobCsm(JobData_pcst->idBlock_uo,
                                                                           NvM_Prv_Crypto_idService_GenerateMac_e);
    // member idJobCallback_uo will be set in the callback if Csm fom has completed
    JobResult_pst->Crypto_st.outputPrimaryLength_u32 = lengthMac_u32;
    JobResult_pst->Crypto_st.outputPrimaryLengthConfigured_u32 = lengthMac_u32;
    JobResult_pst->Crypto_st.outputSecondaryLength_u32 = 0u;              // not used for MAC generation
    JobResult_pst->Crypto_st.outputSecondaryLengthConfigured_u32 = 0u;    // not used for MAC generation
    // member Result_uo will be set in the callback if Csm job has completed
    JobResult_pst->Crypto_st.ResultVerification_uo = CRYPTO_E_VER_OK;     // not used for MAC generation
    JobResult_pst->Crypto_st.isJobCanceled_b = FALSE;

    // Prefill data structure for a cryptographic job
    jobCryptoData_st.inputPrimary_pcu8 = JobData_pcst->IntBuffer_st.Buffer_pu8;
    jobCryptoData_st.inputPrimaryLength_u32 = *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16;
    jobCryptoData_st.outputPrimary_pu8 = &(JobData_pcst->IntBuffer_st.Buffer_pu8[jobCryptoData_st.inputPrimaryLength_u32]);
    jobCryptoData_st.outputPrimaryLength_pu32 = &JobResult_pst->Crypto_st.outputPrimaryLength_u32;
    // all other members of jobCsmData_st are not required to start signature generation

    dataJobStart_st.stJobNextSuccess_en = NvM_Prv_stJob_Write_Crypto_PollGenerationMac_e;
    dataJobStart_st.stJobNextSkipped_en = NvM_Prv_stJob_Write_DoCrypto_e;
    dataJobStart_st.isCryptoServiceConfigured_b = (0u != lengthMac_u32);
    dataJobStart_st.idJobToStart_uo = JobResult_pst->Crypto_st.idJobStarted_uo;
    dataJobStart_st.StartJobCsm_pfct = &NvM_Prv_Csm_StartMacGeneration;

    NvM_Prv_Csm_StartJob(stJob_pen, JobResult_pst, JobData_pcst, &jobCryptoData_st, &dataJobStart_st);

    return idJobStateMachineNext_en;
}

/**
 * \brief
 * This local private function is a cryptographic job step function and polls results of cryptographic services
 * for MAC (message authentication code) generation.
 * \details
 * This function is only available if usage of cryptographic services is enabled by NvM configuration.
 * As soon as results of signature generation are available and plausible (see NvM_Prv_Csm_CheckJobResult)
 * this function updates the result of the job step.
 *
 * \param [inout] stJob_pen
 * State of the cryptographic job
 * \param [inout] JobResult_pst
 * Pointer to the result of the cryptographic job, possible values:
 * - NvM_Prv_JobResult_Pending_e            = MAC generation still pending
 * - NvM_Prv_JobResult_Succeeded_e          = MAC generation has succeeded
 * - NvM_Prv_JobResult_Failed_e             = MAC generation has failed
 * \param [in] JobData_pcst
 * Pointer to the data of the current NvM job
 */
RBA_MEMLIB_STATIC NvM_Prv_idJobStateMachine_ten NvM_Prv_CryptoWrite_PollMacGeneration(NvM_Prv_stJob_ten* stJob_pen,
                                                                                      NvM_Prv_JobResult_tst* JobResult_pst,
                                                                                      NvM_Prv_JobData_tst const* JobData_pcst)
{
    NvM_Prv_idJobStateMachine_ten idJobStateMachineNext_en = NvM_Prv_idJobStateMachine_DoCryptoWrite_e;

    // check whether the Csm job has completed and result is available
    if (NvM_Prv_Csm_CheckJobResult(JobResult_pst,
                                   JobData_pcst,
                                   NVM_E_LENGTH_MAC,
                                   NVM_PRV_NO_DET_ERROR))
    {
        if (E_OK == JobResult_pst->Crypto_st.Result_uo)
        {
            *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 = (uint16)(  *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16
                                                                        + JobResult_pst->Crypto_st.outputPrimaryLength_u32);
            JobResult_pst->Result_en = NvM_Prv_JobResult_Succeeded_e;
            *stJob_pen = NvM_Prv_stJob_Write_DoCrypto_e;
        }
        else
        {
            JobResult_pst->Result_en = NvM_Prv_JobResult_Failed_e;
            *stJob_pen = NvM_Prv_stJob_Write_DoCrypto_e;
        }
    }
    // as long as Csm job has not completed and no result is available NvM remains pending in current state

    return idJobStateMachineNext_en;
}


/*
 **********************************************************************************************************************
 * Inline code
 **********************************************************************************************************************
 */

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_CryptoWrite_PrepareEncryption(NvM_Prv_JobResult_tst* JobResult_pst,
                                                                   NvM_Prv_JobData_tst const* JobData_pcst,
                                                                   NvM_Prv_Crypto_ServiceData_tst* JobCryptoData_pst)
{
    uint16 LengthInitVector_u16 = NvM_Prv_BlkDesc_GetLengthJobCsm(JobData_pcst->idBlock_uo,
                                                                  NvM_Prv_Crypto_idService_InitVector_e);

    // -> check RAM CRC
    if (NvM_Prv_BlkDesc_IsBlockSelected(JobData_pcst->idBlock_uo, NVM_PRV_BLOCK_FLAG_USE_RAM_CRC))
    {
        // RAM CRC is used for current block
        // -> append recalculated RAM CRC into the internal buffer directly after encrypted data
        NvM_Prv_Crc_AppendCrc(JobData_pcst->idBlock_uo,
                              JobData_pcst->IntBuffer_st.Buffer_pu8,
                              JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16,
                              &JobResult_pst->CrcData_st.CrcRamBlk_un);
    }

    *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 += LengthInitVector_u16;

    JobCryptoData_pst->inputPrimary_pcu8 = &JobData_pcst->IntBuffer_st.Buffer_pu8[JobData_pcst->IntBuffer_st.OffsetPlainData_u16];
    JobCryptoData_pst->inputPrimaryLength_u32 = NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo);
    JobCryptoData_pst->outputPrimary_pu8 = JobData_pcst->IntBuffer_st.Buffer_pu8;
    JobCryptoData_pst->outputPrimaryLength_pu32 = &JobResult_pst->Crypto_st.outputPrimaryLength_u32;

    JobCryptoData_pst->inputSecondary_pcu8 = NvM_Prv_BlkDesc_GetDataJobCsmAssociated(JobData_pcst->idBlock_uo);
    JobCryptoData_pst->inputSecondaryLength_u32 = NvM_Prv_BlkDesc_GetLengthJobCsmAssociated(JobData_pcst->idBlock_uo);
    JobCryptoData_pst->outputSecondary_pu8 =
            &JobData_pcst->IntBuffer_st.Buffer_pu8[*JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16];
    JobCryptoData_pst->outputSecondaryLength_pu32 = &JobResult_pst->Crypto_st.outputSecondaryLength_u32;
    // all other members of jobCryptoData_st are not required to start data encryption
}

RBA_MEMLIB_LOCAL_INLINE void NvM_Prv_CryptoWrite_PrepareAuthentication(NvM_Prv_JobResult_tst* JobResult_pst,
                                                                       NvM_Prv_JobData_tst const* JobData_pcst,
                                                                       NvM_Prv_Crypto_ServiceData_tst* JobCryptoData_pst)
{
    uint16 PositionTag_u16;
    uint16 LengthInitVector_u16 = NvM_Prv_BlkDesc_GetLengthJobCsm(JobData_pcst->idBlock_uo,
                                                                  NvM_Prv_Crypto_idService_InitVector_e);
    uint16 PositionPlainUserData_uo = NvM_Prv_BlkDesc_GetBlockSizeStored(JobData_pcst->idBlock_uo) - NVM_RB_BLOCK_HEADER_LENGTH;
    uint16 PositionAssociatedData_uo = PositionPlainUserData_uo + NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo);

    // -> copy plain user data and associated data directly after data to be stored
    rba_MemLib_MemCopy(JobData_pcst->IntBuffer_st.Buffer_pu8,
                       &(JobData_pcst->IntBuffer_st.Buffer_pu8[PositionPlainUserData_uo]),
                       NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo));

    rba_MemLib_MemCopy(NvM_Prv_BlkDesc_GetDataJobCsmAssociated(JobData_pcst->idBlock_uo),
                       &(JobData_pcst->IntBuffer_st.Buffer_pu8[PositionAssociatedData_uo]),
                       NvM_Prv_BlkDesc_GetLengthJobCsmAssociated(JobData_pcst->idBlock_uo));

    *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16 += LengthInitVector_u16;
    PositionTag_u16 = *JobData_pcst->IntBuffer_st.UsedSizeInBytes_pu16;

    JobCryptoData_pst->inputPrimary_pcu8 = NULL_PTR;
    JobCryptoData_pst->inputPrimaryLength_u32 = 0u;
    JobCryptoData_pst->outputPrimary_pu8 = NULL_PTR;
    JobCryptoData_pst->outputPrimaryLength_pu32 = NULL_PTR;

    JobCryptoData_pst->inputSecondary_pcu8 =
            &JobData_pcst->IntBuffer_st.Buffer_pu8[PositionPlainUserData_uo];
    JobCryptoData_pst->inputSecondaryLength_u32 =   NvM_Prv_BlkDesc_GetSize(JobData_pcst->idBlock_uo)
                                                  + NvM_Prv_BlkDesc_GetLengthJobCsmAssociated(JobData_pcst->idBlock_uo);
    JobCryptoData_pst->outputSecondary_pu8 = &JobData_pcst->IntBuffer_st.Buffer_pu8[PositionTag_u16];
    JobCryptoData_pst->outputSecondaryLength_pu32 = &JobResult_pst->Crypto_st.outputSecondaryLength_u32;
    // all other members of jobCryptoData_st are not required to start data encryption
}

#endif  // (NVM_CRYPTO_USED == STD_ON)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

