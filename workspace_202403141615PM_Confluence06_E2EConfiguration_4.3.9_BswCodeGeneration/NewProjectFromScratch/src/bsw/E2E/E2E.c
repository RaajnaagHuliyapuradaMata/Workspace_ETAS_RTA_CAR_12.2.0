

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "E2E.h"

/*
 **********************************************************************************************************************
 Auxiliary functions
 **********************************************************************************************************************
*/
/**
*******************************************************************************************************************
* E2E_GetVersionInfo
*
* \brief private routine to spend information about the version of the library
*
* Description:
*
* Restrictions:
*   -
*
* Dependencies:
*   -
*
* Resources:
*   -
*
* \param   Std_VersionInfoType *VersionInfo  Pointer to the version structure
* \return  void
*
*******************************************************************************************************************
*/
#define E2E_START_SEC_CODE
#include "E2E_MemMap.h"

/* TRACE[SWS_E2E_00032] , TRACE[SWS_E2E_00033] */
void E2E_GetVersionInfo(Std_VersionInfoType * VersionInfo)
{
    if(VersionInfo != NULL_PTR)
    {
        VersionInfo->vendorID = E2E_VENDOR_ID;
        VersionInfo->moduleID = E2E_MODULE_ID;
        VersionInfo->sw_major_version = E2E_SW_MAJOR_VERSION;
        VersionInfo->sw_minor_version = E2E_SW_MINOR_VERSION;
        VersionInfo->sw_patch_version=  E2E_SW_PATCH_VERSION;
    }
}

#define E2E_STOP_SEC_CODE
#include "E2E_MemMap.h"
