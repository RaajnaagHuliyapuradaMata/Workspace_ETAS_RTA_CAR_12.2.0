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

Std_ReturnType NvM_Rb_AuxGetDataIndex(uint8 *DataIndexPtr)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2940] Service used from an auxiliary user to get the currently
    //                                               set DataIndex of a dataset NVRAM block
    Std_ReturnType stReturn_uo = E_NOT_OK;

    // TRACE[NVM602] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    if (NvM_Prv_ErrorDetection_IsNvmInitialized(NVM_SERVICE_ID_RB_AUX_GET_DATA_INDEX, 0u))
    {
        // TRACE[NVM605] Report the DET error NVM_E_PARAM_DATA when a NULL pointer is passed
        //               via the parameter DataIndexPtr
        NvM_Prv_ErrorDetection_Ptr_tun ptr_un;
        ptr_un.ptrDataIdx_pu8 = DataIndexPtr;
        if (NvM_Prv_ErrorDetection_IsPtrValid(NVM_SERVICE_ID_RB_AUX_GET_DATA_INDEX, 0u, NVM_E_PARAM_DATA, &ptr_un))
        {
            // Parameters are valid
            // TRACE[NVM021] Get the current data index of the selected block if its management type is NVM_BLOCK_DATASET
            // No SchM lock can and should be done here to allow calling this function from NvM callbacks without deadlocks
            *DataIndexPtr = NvM_Prv_Block_GetIdxDataset(NVM_PRV_AUX_ADMIN_BLOCK);
            stReturn_uo = E_OK;
        }
    }

    // Return whether this operation was successful
    return stReturn_uo;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

#endif
