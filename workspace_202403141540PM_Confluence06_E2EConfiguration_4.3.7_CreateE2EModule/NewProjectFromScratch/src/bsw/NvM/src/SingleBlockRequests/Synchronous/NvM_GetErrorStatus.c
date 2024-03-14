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
#include "NvM_Prv_HideRteApi.h"

#include "NvM_Prv_BlockData.h"
#include "NvM_Prv_ErrorDetection.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

#ifdef NVM_CFG_API_GET_ERROR_STATUS

// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here,
//                                               not in a header file
/* MR12 RULE 8.5 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_GET_ERROR_STATUS(NvM_BlockIdType BlockId, NvM_RequestResultType *RequestResultPtr);

Std_ReturnType NvM_GetErrorStatus(NvM_BlockIdType BlockId, NvM_RequestResultType *RequestResultPtr)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_GET_ERROR_STATUS(BlockId, RequestResultPtr);
}

#endif


#ifdef NVM_CFG_API_GET_ERROR_STATUS
Std_ReturnType NvM_Rb_GetErrorStatusBase(NvM_BlockIdType BlockId, NvM_RequestResultType *RequestResultPtr)
#else
Std_ReturnType NvM_GetErrorStatus(NvM_BlockIdType BlockId, NvM_RequestResultType *RequestResultPtr)
#endif
{
    // TRACE[NVM451] Service to read the block dependent error/status information
    Std_ReturnType stReturn_u8 = E_NOT_OK;

    // TRACE[NVM610] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    if (NvM_Prv_ErrorDetection_IsNvmInitialized(NVM_SERVICE_ID_GET_ERROR_STATUS, BlockId))
    {
        // TRACE[NVM611] Report the DET error NVM_E_PARAM_BLOCK_ID when the passed BlockID is out of range
        // TRACE[NVM394] NvM_Prv_stRequestResult_au8[0] contains the status of multiblock operations
        if (NvM_Prv_ErrorDetection_IsBlockIdValid(NVM_SERVICE_ID_GET_ERROR_STATUS, BlockId, TRUE))
        {
            // TRACE[NVM612] Report the DET error NVM_E_PARAM_DATA when a NULL pointer is passed
            //               via the parameter RequestResultPtr
            NvM_Prv_ErrorDetection_Ptr_tun ptr_un;
            ptr_un.ptrRequestResult_puo = RequestResultPtr;
            if (NvM_Prv_ErrorDetection_IsPtrValid(NVM_SERVICE_ID_GET_ERROR_STATUS, BlockId, NVM_E_PARAM_DATA, &ptr_un))
            {
                // Parameters are valid
                // TRACE[NVM015] Read out the selected block's state from the administrative block
                // TRACE[NVM394] NvM_Prv_stRequestResult_au8[0] contains the status of multiblock operations
                // No SchM lock can and should be done here to allow calling this function from NvM callbacks
                // without deadlocks
                *RequestResultPtr = NvM_Prv_Block_GetRequestResult(BlockId);
                stReturn_u8 = E_OK;
            }
        }
    }

    // Return whether this operation was successful
    return stReturn_u8;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

