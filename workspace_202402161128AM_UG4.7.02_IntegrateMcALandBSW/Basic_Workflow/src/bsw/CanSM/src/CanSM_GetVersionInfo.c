/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2017, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "CanSM_Prv.h"

/***************************************************************************************************
** Function Name        : CanSM_GetVersionInfo
**
** Service ID           : 0x01
**
** Description          : This service puts out the version information of this module (module ID, vendor ID, vendor
**                        specific version numbers related to BSW00407)
**
** Sync/Async           : Synchronous
**
** Re-entrancy          : Reentrant
**
** Input Parameters     : None
**
** InOut parameter      : None
**
** Output Parameters    : VersionInfo - Pointer to where to store the version information of this module.
**
** Return parameter     : None

***************************************************************************************************/

#if (CANSM_VERSION_INFO_API == STD_ON)
#define CANSM_START_SEC_CODE
#include "CanSM_MemMap.h"
void CanSM_GetVersionInfo( Std_VersionInfoType * VersionInfo )
{
    CANSM_REPORT_ERROR((VersionInfo==NULL_PTR),
                       (uint8)CANSM_GETVERSIONINFO_SID,
                       (uint8)CANSM_E_PARAM_POINTER)

    VersionInfo->vendorID = CANSM_VENDOR_ID;
    VersionInfo->moduleID = CANSM_MODULE_ID;
    VersionInfo->sw_major_version = CANSM_SW_MAJOR_VERSION;
    VersionInfo->sw_minor_version = CANSM_SW_MINOR_VERSION;
    VersionInfo->sw_patch_version = CANSM_SW_PATCH_VERSION;
}
#define CANSM_STOP_SEC_CODE
#include "CanSM_MemMap.h"
#endif

