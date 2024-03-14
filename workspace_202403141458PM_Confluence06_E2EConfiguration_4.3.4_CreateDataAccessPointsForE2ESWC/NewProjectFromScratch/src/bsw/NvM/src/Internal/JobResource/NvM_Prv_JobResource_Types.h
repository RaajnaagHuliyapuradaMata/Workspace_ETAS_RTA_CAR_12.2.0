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
#ifndef NVM_PRV_JOBRESOURCE_TYPES_H
#define NVM_PRV_JOBRESOURCE_TYPES_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Types.h"

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */
/// Type definition for function to determine whether a block requires a job resource.
typedef boolean (*NvM_Prv_JobResource_IsRequired_tpfct)(NvM_BlockIdType idBlock_uo);

/**
 * \brief
 * Definition of enumeration for different clusters of job resources.
 *
 * \details
 * NvM puts together several job resources to a cluster, if all these job resources are using the same state machine
 * to process a job.
 * I.e. different job resources from the same cluster use the same state machine to process a job (or a part of it).
 * E.g. different memory devices are own job resources (internal flash, external flash, eeprom, etc.) but to process
 * a job on such job resource NvM uses the same state machine (NvM_Prv_MemIf_DoStateMachine).
 */
typedef enum
{
    /// Job resource cluster to process a job on a memory device (NvM_Prv_MemIf_DoStateMachine)
    NvM_Prv_JobResource_Cluster_MemIf_e,

#if (NVM_CRYPTO_USED == STD_ON)
    /// Job resource cluster to process a job on a crypto device (NvM_Prv_Crypto_DoStateMachine)
    NvM_Prv_JobResource_Cluster_Crypto_e,
#endif

    //-----------------------------------------------------------------------------------
    // This is actually no job resource cluster,
    // in fact this enum value defines count of job resource clusters available in NvM
    // and can be used for array definitions
    //-----------------------------------------------------------------------------------
    NvM_Prv_JobResource_nrCluster_e

} NvM_Prv_JobResource_Cluster_ten;


/* NVM_PRV_JOBRESOURCE_TYPES_H */
#endif

