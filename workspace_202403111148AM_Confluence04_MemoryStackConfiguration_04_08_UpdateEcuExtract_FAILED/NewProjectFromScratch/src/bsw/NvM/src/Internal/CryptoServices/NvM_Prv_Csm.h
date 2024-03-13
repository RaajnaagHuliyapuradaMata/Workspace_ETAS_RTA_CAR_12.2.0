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
#ifndef NVM_PRV_CSM_H
#define NVM_PRV_CSM_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_Job_Types.h"
#include "NvM_Prv_Crypto_ServiceTypes.h"
#include "NvM_Prv_Crypto_InternalTypes.h"

#if (NVM_CRYPTO_USED == STD_ON)

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
# define NVM_START_SEC_CODE
# include "NvM_MemMap.h"

extern boolean NvM_Prv_Csm_CancelJob(NvM_Prv_Job_tst* Job_pst);

extern void NvM_Prv_Csm_PollJob(NvM_Prv_stJob_ten* stJob_pen,
                                NvM_Prv_JobResult_tst* JobResult_pst,
                                NvM_Prv_JobData_tst const* JobData_pcst);

extern boolean NvM_Prv_Csm_CheckJobResult(NvM_Prv_JobResult_tst* JobResult_pst,
                                          NvM_Prv_JobData_tst const* JobData_pcst,
                                          NvM_Prv_idDetError_tuo idErrorPrimaryOutput_uo,
                                          NvM_Prv_idDetError_tuo idErrorSecondaryOutput_uo);

extern void NvM_Prv_Csm_StartJob(NvM_Prv_stJob_ten* stJob_pen,
                                 NvM_Prv_JobResult_tst* JobResult_pst,
                                 NvM_Prv_JobData_tst const* JobData_pcst,
                                 NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst,
                                 NvM_Prv_Crypto_dataServiceStart_tst const* dataServiceStart_pcst);

extern NvM_Prv_JobResult_ten NvM_Prv_Csm_StartRandomGeneration(uint32 idJobCsm_uo,
                                                               uint8* cntrCsmJobRetry_puo,
                                                               NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

extern NvM_Prv_JobResult_ten NvM_Prv_Csm_StartEncryption(uint32 idJobCsm_uo,
                                                         uint8* cntrCsmJobRetry_puo,
                                                         NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

extern NvM_Prv_JobResult_ten NvM_Prv_Csm_StartDecryption(uint32 idJobCsm_uo,
                                                         uint8* cntrCsmJobRetry_puo,
                                                         NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

extern NvM_Prv_JobResult_ten NvM_Prv_Csm_StartEncryptionAead(uint32 idJobCsm_uo,
                                                             uint8* cntrCsmJobRetry_puo,
                                                             NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

extern NvM_Prv_JobResult_ten NvM_Prv_Csm_StartDecryptionAead(uint32 idJobCsm_uo,
                                                             uint8* cntrCsmJobRetry_puo,
                                                             NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

extern NvM_Prv_JobResult_ten NvM_Prv_Csm_StartSignatureGeneration(uint32 idJobCsm_uo,
                                                                  uint8* cntrCsmJobRetry_puo,
                                                                  NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

extern NvM_Prv_JobResult_ten NvM_Prv_Csm_StartSignatureVerification(uint32 idJobCsm_uo,
                                                                    uint8* cntrCsmJobRetry_puo,
                                                                    NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

extern NvM_Prv_JobResult_ten NvM_Prv_Csm_StartMacGeneration(uint32 idJobCsm_uo,
                                                            uint8* cntrCsmJobRetry_puo,
                                                            NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

extern NvM_Prv_JobResult_ten NvM_Prv_Csm_StartMacVerification(uint32 idJobCsm_uo,
                                                              uint8* cntrCsmJobRetry_puo,
                                                              NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

extern NvM_Prv_JobResult_ten NvM_Prv_Csm_SetKeyInitVector(uint32 idKeyCsm_uo,
                                                          uint8* cntrCsmJobRetry_puo,
                                                          NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

extern NvM_Prv_JobResult_ten NvM_Prv_Csm_SetKeyValid(uint32 idKeyCsm_uo,
                                                     uint8* cntrCsmJobRetry_puo,
                                                     NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

# define NVM_STOP_SEC_CODE
# include "NvM_MemMap.h"

#endif  // (NVM_CRYPTO_USED == STD_ON)

/* NVM_PRV_CSM_H */
#endif

