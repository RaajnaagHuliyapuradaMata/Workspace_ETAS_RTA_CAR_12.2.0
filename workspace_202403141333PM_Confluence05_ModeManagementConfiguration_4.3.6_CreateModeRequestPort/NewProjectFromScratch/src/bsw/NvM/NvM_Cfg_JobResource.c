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

#include "NvM_Prv_JobResource.h"
#include "NvM_Prv_Job_Types.h"
#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_MemIf.h"

/*
 **********************************************************************************************************************
 * Local declarations
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

static boolean NvM_Prv_JobResource_IsRequiredDevice_0(NvM_BlockIdType idBlock_uo);

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"
/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
 */
#define NVM_START_SEC_CONST_UNSPECIFIED
#include "NvM_MemMap.h"

static NvM_Prv_JobResource_IsRequired_tpfct const NvM_Prv_JobResource_IsRequired_acpfct[NvM_Prv_idJobResource_NrJobResources_e] =
{
    NvM_Prv_JobResource_IsRequiredDevice_0,
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

boolean NvM_Prv_JobResource_IsRequired(NvM_Prv_idJobResource_tuo idJobResource_uo,
                                       NvM_BlockIdType idBlock_uo)
{
    return (NvM_Prv_JobResource_IsRequired_acpfct[idJobResource_uo](idBlock_uo));
}


static boolean NvM_Prv_JobResource_IsRequiredDevice_0(NvM_BlockIdType idBlock_uo)
{
    return (0u == NvM_Prv_BlkDesc_GetIdDevice(idBlock_uo));
}


#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"


