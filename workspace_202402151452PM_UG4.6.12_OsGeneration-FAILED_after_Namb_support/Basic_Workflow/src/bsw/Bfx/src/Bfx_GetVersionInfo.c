/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2012, all rights reserved
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
 
 #define SRVLIBS
 
#include "Bfx.h"

/**
 **********************************************************************************************************************
 * Bfx_GetVersionInfo
 *
 * Macro to get version information including Vendor Id, Module Id and Version Info
 *
 * \param    versionInfo - pointer to structure Std_VersionInfoType
 * \retval   void
 **********************************************************************************************************************
*/
#if (BFX_VERSIONINFOAPI == STD_ON)

    #define BFX_START_SEC_CODE
    #include "Bfx_MemMap.h"
    void Bfx_GetVersionInfo(Std_VersionInfoType* versionInfo)
    {
        versionInfo->vendorID = BFX_VENDOR_ID;
        versionInfo->moduleID = BFX_MODULE_ID;
        versionInfo->sw_major_version = BFX_SW_MAJOR_VERSION;
        versionInfo->sw_minor_version = BFX_SW_MINOR_VERSION;
        versionInfo->sw_patch_version = BFX_SW_PATCH_VERSION;
    }
    #define BFX_STOP_SEC_CODE
    #include "Bfx_MemMap.h"

#endif

/*********************************************************************************************************************/


