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
#include "NvM_Cfg_SchM.h"

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

# ifdef NVM_CFG_API_RB_AUX_SET_DATA_INDEX

// TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2917] Declaration of the overloading function shall be done here,
//                                               not in a header file
/* MR12 RULE 8.5 VIOLATION: Not using a header file for this declaration is a customer requirement */
extern Std_ReturnType NVM_CFG_API_RB_AUX_SET_DATA_INDEX(uint8 DataIndex);

Std_ReturnType NvM_Rb_AuxSetDataIndex(uint8 DataIndex)
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2920] Call the overloading function
    return NVM_CFG_API_RB_AUX_SET_DATA_INDEX(DataIndex);
}

# endif


# ifdef NVM_CFG_API_RB_AUX_SET_DATA_INDEX
Std_ReturnType NvM_Rb_AuxSetDataIndexBase(uint8 DataIndex)
# else
Std_ReturnType NvM_Rb_AuxSetDataIndex(uint8 DataIndex)
# endif
{
    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-2941] Service used from an auxiliary user for setting the DataIndex
    //                                               of a dataset NVRAM block
    Std_ReturnType stReturn_uo = E_NOT_OK;

    // TRACE[NVM027] Report the DET error NVM_E_NOT_INITIALIZED when the NVM is not yet initialized
    if (NvM_Prv_ErrorDetection_IsNvmInitialized(NVM_SERVICE_ID_RB_AUX_SET_DATA_INDEX, 0u))
    {
        // VariabNvM_Prv_Service_InitiationErrors_tuoles used to report DET errors
        NvM_Prv_Service_InitiationErrors_tuo errors_uo = 0u;

        SchM_Enter_NvM_Main();

        // TRACE[NVM704] Prohibit administrative data block modifications
        //               while a single-block operation is pending or in progress
        if (!NvM_Prv_Block_IsPending(NVM_PRV_AUX_ADMIN_BLOCK, &errors_uo))
        {
            NvM_Prv_Block_SetIdxData(NVM_PRV_AUX_ADMIN_BLOCK, DataIndex);
            stReturn_uo = E_OK;
        }

        SchM_Exit_NvM_Main();

        // Calling foreign DET function is done out of the scheduler lock
        NvM_Prv_ErrorDetection_ReportServiceInitiationErrors(NVM_SERVICE_ID_RB_AUX_SET_DATA_INDEX,
                                                             0u,
                                                             errors_uo);
    }

    // Return whether this operation was successful
    return stReturn_uo;
}

#define NVM_STOP_SEC_CODE
#include "NvM_MemMap.h"

#endif
