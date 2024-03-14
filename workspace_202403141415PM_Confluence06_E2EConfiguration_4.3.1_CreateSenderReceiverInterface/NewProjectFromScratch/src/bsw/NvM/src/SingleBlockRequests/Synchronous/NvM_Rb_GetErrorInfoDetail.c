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

#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_ErrorDetection.h"
/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#if (NVM_RB_ERROR_INFO_DETAIL == STD_ON)

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

Std_ReturnType NvM_Rb_GetErrorInfoDetail(NvM_BlockIdType BlockId, NvM_Rb_ErrorInfoDetail_ten *result_pen)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3401] Service to read out error information details after a failing read
    Std_ReturnType stReturn_uo = E_NOT_OK;

    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3388] Report the DET error when the NVM is not yet initialized
    if (NvM_Prv_ErrorDetection_IsNvmInitialized(NVM_SERVICE_ID_RB_GET_ERROR_INFO_DETAIL, BlockId))
    {
        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3389] Report the DET error when the passed BlockID is out of range
        if (NvM_Prv_ErrorDetection_IsBlockIdValid(NVM_SERVICE_ID_RB_GET_ERROR_INFO_DETAIL, BlockId, FALSE))
        {
            // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3390] Report the DET error when a NULL pointer is passed
            //             via the parameter result_pen
            NvM_Prv_ErrorDetection_Ptr_tun ptr_un;
            ptr_un.ptrErrorInfoDetail_pen = result_pen;
            if (NvM_Prv_ErrorDetection_IsPtrValid(NVM_SERVICE_ID_RB_GET_ERROR_INFO_DETAIL,
                                                  BlockId,
                                                  NVM_E_PARAM_DATA,
                                                  &ptr_un))
            {
                // Parameters are valid
                // No SchM lock can and should be done here to allow calling this function from NvM callbacks
                // without deadlocks
                *result_pen = NvM_Prv_Block_GetErrorInfoDetail(BlockId);
                stReturn_uo = E_OK;
            }
        }
    }

    return(stReturn_uo);
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

#endif
