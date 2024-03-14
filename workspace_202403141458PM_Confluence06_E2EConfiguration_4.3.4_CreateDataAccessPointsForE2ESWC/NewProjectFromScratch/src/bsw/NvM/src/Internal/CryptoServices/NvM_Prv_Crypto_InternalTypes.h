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
#ifndef NVM_PRV_CRYPTO_INTERNALTYPES_H
#define NVM_PRV_CRYPTO_INTERNALTYPES_H

/*
 * This header file contains types for internal use in the unit NvM_Crypto.c.
 * The internal types are maintained externally to ease increase unit testability with Cantata.
 * Probably this header files needs to be restructured along with the other header files.
 */

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
#if (NVM_CRYPTO_USED == STD_ON)
/// Type definition of function to start a cryptographic service on the Csm.
typedef NvM_Prv_JobResult_ten (*NvM_Prv_Csm_StartJob_tpfct)(uint32 idJobCsm_uo,
                                                            uint8* cntrCsmJobRetry_puo,
                                                            NvM_Prv_Crypto_ServiceData_tst const* ServiceData_pcst);

/**
 * \brief
 * Definition of the structure for data required to start a cryptographic service.
 */
typedef struct
{
    /// Next state of the current job if the cryptographic service start succeeds.
     NvM_Prv_stJob_ten stJobNextSuccess_en;
    /// Next state of the current job if NvM skips the cryptographic service due to current block configuration.
     NvM_Prv_stJob_ten stJobNextSkipped_en;
    /// Information whether the cryptographic service is configured for current block.
    boolean isCryptoServiceConfigured_b;
    /// Id of the Csm job to start for the cryptographic service.
    uint32 idJobToStart_uo;
    /// Pointer to the function which starts the cryptographic service.
    NvM_Prv_Csm_StartJob_tpfct StartJobCsm_pfct;

} NvM_Prv_Crypto_dataServiceStart_tst;

#endif  // (NVM_CRYPTO_USED == STD_ON)

/* NVM_PRV_CRYPTO_INTERNALTYPES_H */
#endif

