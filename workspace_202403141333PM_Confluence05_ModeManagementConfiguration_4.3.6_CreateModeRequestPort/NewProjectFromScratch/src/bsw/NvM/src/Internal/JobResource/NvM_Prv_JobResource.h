/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2018, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef NVM_PRV_JOBRESOURCE_H
#define NVM_PRV_JOBRESOURCE_H
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "NvM_Prv_Job_Types.h"
#include "NvM_Prv_JobResource_Types.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

extern void NvM_Prv_JobResource_Init(void);

extern void NvM_Prv_JobResource_Claim(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                      NvM_BlockIdType idBlock_uo);

extern void NvM_Prv_JobResource_Unclaim(NvM_BlockIdType idBlock_uo);

extern boolean NvM_Prv_JobResource_IsClaimed(NvM_Prv_idJobResource_tuo idJobResource_uo);

extern boolean NvM_Prv_JobResource_Lock(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                        NvM_Prv_JobData_tst const* JobData_pcst);

extern void NvM_Prv_JobResource_Release(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                        NvM_Prv_JobData_tst const* JobData_pcst);

extern NvM_Prv_JobResource_Cluster_ten NvM_Prv_JobResource_GetLockedByJob(NvM_Prv_JobData_tst const* JobData_pcst);

extern boolean NvM_Prv_JobResource_IsLocked(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                            NvM_Prv_JobData_tst const* JobData_pcst);

// declaration for functions from NvM_Cfg_JobResource.c

extern boolean NvM_Prv_JobResource_IsRequired(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                              NvM_BlockIdType idBlock_uo);

#if (NVM_JOB_PRIORITIZATION == STD_ON)
extern boolean NvM_Prv_JobResource_Cancel(NvM_Prv_JobResource_Cluster_ten Cluster_en,
                                          NvM_Prv_Job_tst* Job_pst);
#endif

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

/* NVM_PRV_JOBRESOURCE_H */
#endif

