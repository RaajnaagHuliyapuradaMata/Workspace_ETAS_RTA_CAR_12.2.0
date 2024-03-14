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

#include "NvM_Prv_ErrorDetection.h"

/*
 **********************************************************************************************************************
 * Code
 **********************************************************************************************************************
*/
#if (NVM_VERSION_INFO_API == STD_ON)

# define NVM_START_SEC_CODE
# include "NvM_MemMap.h"

// TRACE[NVM286] The function NvM_GetVersionInfo shall be pre compile time configurable by the configuration parameter
// NvMVersionInfoApi.
// TRACE[NVM650] The function NvM_GetVersionInfo shall be configurable On/Off by the configuration parameter
// NvMVersionInfoApi.

void NvM_GetVersionInfo(Std_VersionInfoType* VersionInfoPtr)
{
    // TRACE[NVM452] Service to get the version information of the NvM module

    // TRACE[BSW_SWCS_AR_NVRAMManager_Ext-3114] Set block ID to 0 if NvM service does not use any block ID
    // TRACE[NVM613] Report the DET error NVM_E_PARAM_POINTER when a NULL pointer is passed via the parameter VersionInfo
    NvM_Prv_ErrorDetection_Ptr_tun ptr_un;
    ptr_un.ptrVersionInfo_pst = VersionInfoPtr;
    if (NvM_Prv_ErrorDetection_IsPtrValid(NVM_SERVICE_ID_GET_VERSION_INFO, 0, NVM_E_PARAM_POINTER, &ptr_un))
    {
        // TRACE[NVM285] The function NvM_GetVersionInfo shall return the version information of this module.
        // The version information includes:
        //      - Module Id
        //      - Vendor Id
        //      - Software version
        VersionInfoPtr->vendorID            = NVM_VENDOR_ID;
        VersionInfoPtr->moduleID            = NVM_MODULE_ID;
        VersionInfoPtr->sw_major_version    = NVM_SW_MAJOR_VERSION;
        VersionInfoPtr->sw_minor_version    = NVM_SW_MINOR_VERSION;
        VersionInfoPtr->sw_patch_version    = NVM_SW_PATCH_VERSION;
    }
}

# define NVM_STOP_SEC_CODE
# include "NvM_MemMap.h"

#endif
