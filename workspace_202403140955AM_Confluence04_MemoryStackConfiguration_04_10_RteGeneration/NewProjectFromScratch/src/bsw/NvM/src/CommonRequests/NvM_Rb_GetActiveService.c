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

#include "NvM_Prv.h"
#include "NvM_Prv_ErrorDetection.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#define NVM_START_SEC_CODE
#include "NvM_MemMap.h"
#if (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_OFF)
Std_ReturnType NvM_Rb_GetActiveService(uint8 *ServiceIdPtr)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2827] Service to read the ID of the currently active service
    //                                               of the NVRAM manager
    Std_ReturnType stReturn_u8 = E_NOT_OK;

    NvM_Prv_ErrorDetection_Ptr_tun ptr_un;
    ptr_un.ptrIdService_puo = ServiceIdPtr;
    if (NvM_Prv_ErrorDetection_IsPtrValid(NVM_SERVICE_ID_RB_GET_ACTIVE_SERVICE, 0, NVM_E_PARAM_DATA, &ptr_un))
    {
        NvM_Prv_idService_tuo idActiveService_uo = NvM_Prv_GetActiveService();

# if (NVM_PRV_MAINTAIN_REDUNDANT_BLOCKS)
        // The maintenance phase shall not be visible for an NvM user.
        // CAUTION: The following assignment has to be changed in the future if it is decided
        //          to make the maintenance operation visible for an NvM user.
        // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2865] If the maintenance operation is currently being executed
        //                                               then NvM_Rb_GetActiveService indicates WRITE_ALL as active service
        if (idActiveService_uo == NVM_PRV_SERVICE_ID_MAINTAIN_BLOCK)
        {
            *ServiceIdPtr = NVM_SERVICE_ID_WRITE_ALL;
        }
        else
# endif // (NVM_PRV_MAINTAIN_REDUNDANT_BLOCKS)
        {
            *ServiceIdPtr = idActiveService_uo ;
        }

        stReturn_u8 = E_OK;
    }

    return stReturn_u8;
}
#endif // (NVM_RB_CALLBACK_SIGNATURE_AR45 == STD_OFF)

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

