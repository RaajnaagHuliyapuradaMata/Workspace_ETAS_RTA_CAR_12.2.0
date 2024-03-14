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
#if (NVM_RB_AUX_INTERFACE == STD_ON)

#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"

#ifdef NVM_CFG_API_RB_AUX_GET_ERROR_STATUS

// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here,
//                                               not in a header file
/* MR12 RULE 8.5 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_RB_AUX_GET_ERROR_STATUS(NvM_RequestResultType *RequestResultPtr);

Std_ReturnType NvM_Rb_AuxGetErrorStatus(NvM_RequestResultType *RequestResultPtr)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_RB_AUX_GET_ERROR_STATUS(RequestResultPtr);
}

#endif


#ifdef NVM_CFG_API_RB_AUX_GET_ERROR_STATUS
Std_ReturnType NvM_Rb_AuxGetErrorStatusBase(NvM_RequestResultType *RequestResultPtr)
#else
Std_ReturnType NvM_Rb_AuxGetErrorStatus(NvM_RequestResultType *RequestResultPtr)
#endif
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2939] Service used from an auxiliary user to read the block dependent
    //                                               error/status information

    Std_ReturnType stReturn_uo = E_NOT_OK;

    // TRACE[NVM610] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    if (NvM_Prv_ErrorDetection_IsNvmInitialized(NVM_SERVICE_ID_RB_AUX_GET_ERROR_STATUS, 0u))
    {
        // TRACE[NVM612] Report the DET error NVM_E_PARAM_DATA when a NULL pointer is passed
        //               via the parameter RequestResultPtr
        NvM_Prv_ErrorDetection_Ptr_tun ptr_un;
        ptr_un.ptrRequestResult_puo = RequestResultPtr;
        if (NvM_Prv_ErrorDetection_IsPtrValid(NVM_SERVICE_ID_RB_AUX_GET_ERROR_STATUS, 0, NVM_E_PARAM_DATA, &ptr_un))
        {
            // Parameters are valid
            // TRACE[NVM015] Read out the selected block's state from the administrative block
            // No SchM lock can and should be done here to allow calling this function from NvM callbacks without deadlocks
            // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2929] Use special auxiliary admin block for the request result update
            *RequestResultPtr = NvM_Prv_Block_GetRequestResult(NVM_PRV_AUX_ADMIN_BLOCK);
            stReturn_uo = E_OK;
        }
    }

    // Return whether this operation was successful
    return stReturn_uo;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

#endif
