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
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "NvM.h"

#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_ErrorDetection.h"
/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

Std_ReturnType NvM_Rb_GetNvBlockLength(NvM_BlockIdType BlockId, uint16 *NvBlockLengthPtr)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2791] Service for getting the configured NV block length in bytes
    //                                               of an NvM block
    Std_ReturnType stReturn_uo = E_NOT_OK;

    if (NvM_Prv_ErrorDetection_IsBlockIdValid(NVM_SERVICE_ID_RB_GET_NV_BLOCK_LENGTH, BlockId, FALSE))
    {
        NvM_Prv_ErrorDetection_Ptr_tun ptr_un;
        ptr_un.ptrBlockLength_pu16 = NvBlockLengthPtr;
        if (NvM_Prv_ErrorDetection_IsPtrValid(NVM_SERVICE_ID_RB_GET_NV_BLOCK_LENGTH,
                                              BlockId,
                                              NVM_E_PARAM_DATA,
                                              &ptr_un))
        {
            *NvBlockLengthPtr = NvM_Prv_BlkDesc_GetSize(BlockId);
            stReturn_uo = E_OK;
        }
    }

    return stReturn_uo;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

