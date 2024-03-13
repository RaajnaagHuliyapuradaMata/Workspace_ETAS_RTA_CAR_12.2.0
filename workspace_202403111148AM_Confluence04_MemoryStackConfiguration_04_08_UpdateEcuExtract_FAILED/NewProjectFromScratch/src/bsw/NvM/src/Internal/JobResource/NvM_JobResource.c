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

#include "rba_MemLib_Special.h"

#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_JobResource.h"
/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define NVM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"

/// Definition of the array for all claimed job resources.
RBA_MEMLIB_STATIC NvM_BlockIdType NvM_Prv_JobResource_Claimed_auo[NvM_Prv_idJobResource_NrJobResources_e];

/// Definition of the array for all internal jobs processed on job resources in parallel.
RBA_MEMLIB_STATIC NvM_Prv_JobData_tst const* NvM_Prv_JobResources_apcst[NvM_Prv_idJobResource_NrJobResources_e];

#define NVM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "NvM_MemMap.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

void NvM_Prv_JobResource_Init(void)
{
    NvM_Prv_idJobResource_tuo idJobResource_uo;
    for (idJobResource_uo = 0u; idJobResource_uo < NvM_Prv_idJobResource_NrJobResources_e; ++idJobResource_uo)
    {
        NvM_Prv_JobResource_Claimed_auo[idJobResource_uo] = 0u;
        NvM_Prv_JobResources_apcst[idJobResource_uo] = NULL_PTR;
    }
}

void NvM_Prv_JobResource_Claim(NvM_Prv_idJobResource_tuo idJobResource_uo,
                               NvM_BlockIdType idBlock_uo)
{
    NvM_Prv_JobResource_Claimed_auo[idJobResource_uo] = idBlock_uo;
}

void NvM_Prv_JobResource_Unclaim(NvM_BlockIdType idBlock_uo)
{
    NvM_Prv_idJobResource_tuo idJobResource_uo;
    for (idJobResource_uo = 0u; idJobResource_uo < NvM_Prv_idJobResource_NrJobResources_e; ++idJobResource_uo)
    {
        if (idBlock_uo == NvM_Prv_JobResource_Claimed_auo[idJobResource_uo])
        {
            NvM_Prv_JobResource_Claimed_auo[idJobResource_uo] = 0u;
        }
    }
}

boolean NvM_Prv_JobResource_IsClaimed(NvM_Prv_idJobResource_tuo idJobResource_uo)
{
    return (NvM_Prv_JobResource_Claimed_auo[idJobResource_uo] > 0u);
}

boolean NvM_Prv_JobResource_Lock(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                 NvM_Prv_JobData_tst const* JobData_pcst)
{
    boolean isLocked_b;
    if (NULL_PTR == NvM_Prv_JobResources_apcst[idJobResource_uo])
    {
        NvM_Prv_JobResources_apcst[idJobResource_uo] = JobData_pcst;
        if (JobData_pcst->idBlock_uo == NvM_Prv_JobResource_Claimed_auo[idJobResource_uo])
        {
            NvM_Prv_JobResource_Claimed_auo[idJobResource_uo] = 0u;
        }
        isLocked_b = TRUE;
    }
    else if (JobData_pcst == NvM_Prv_JobResources_apcst[idJobResource_uo])
    {
        isLocked_b = TRUE;
    }
    else
    {
        if (   NvM_Prv_JobResource_IsRequired(idJobResource_uo, JobData_pcst->idBlock_uo)
            && (0u == NvM_Prv_JobResource_Claimed_auo[idJobResource_uo]))
        {
            NvM_Prv_JobResource_Claimed_auo[idJobResource_uo] = JobData_pcst->idBlock_uo;
        }
        isLocked_b = FALSE;
    }
    return isLocked_b;
}

void NvM_Prv_JobResource_Release(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                 NvM_Prv_JobData_tst const* JobData_pcst)
{
    if (JobData_pcst == NvM_Prv_JobResources_apcst[idJobResource_uo])
    {
        NvM_Prv_JobResources_apcst[idJobResource_uo] = NULL_PTR;
    }
}

boolean NvM_Prv_JobResource_IsLocked(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                     NvM_Prv_JobData_tst const* JobData_pcst)
{
    return (JobData_pcst == NvM_Prv_JobResources_apcst[idJobResource_uo]);
}

NvM_Prv_JobResource_Cluster_ten NvM_Prv_JobResource_GetLockedByJob(NvM_Prv_JobData_tst const* JobData_pcst)
{
    boolean isJobResourceLocked_b = FALSE;
    NvM_Prv_JobResource_Cluster_ten Cluster_en = NvM_Prv_JobResource_Cluster_MemIf_e;

    while (!isJobResourceLocked_b && (Cluster_en < NvM_Prv_JobResource_nrCluster_e))
    {
        NvM_Prv_idJobResource_tuo idJobResource_uo = NvM_Prv_BlkDesc_GetIdJobResource(JobData_pcst->idBlock_uo,
                                                                                      Cluster_en);

        if (   (idJobResource_uo < NvM_Prv_idJobResource_NrJobResources_e)
            && (JobData_pcst == NvM_Prv_JobResources_apcst[idJobResource_uo])
            )
        {
            isJobResourceLocked_b = TRUE;
        }
        else
        {
            /* MR12 RULE 10.1 VIOLATION: This function ensures that enum type is used only in valid range */
            ++Cluster_en;
        }
    }

    return Cluster_en;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

