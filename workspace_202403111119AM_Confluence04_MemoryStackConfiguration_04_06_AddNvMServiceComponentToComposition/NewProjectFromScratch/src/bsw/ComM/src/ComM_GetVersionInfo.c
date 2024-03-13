/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2014, all rights reserved
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

#include "ComM_Priv.h"

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */
/*
 ***************************************************************************************************
 * Type definitions
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Variables
 ***************************************************************************************************
 */

#if (COMM_VERSION_INFO_API == STD_ON)

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"

/**************************************************************************************************/
/* Global functions (declared in header files )                                                   */
/**************************************************************************************************/

/***************************************************************************************************
 Function name    : ComM_GetVersionInfo
 Syntax           : void ComM_GetVersionInfo(Std_VersionInfoType *versioninfo)
 Description      : Service for published information
 Parameter        : versioninfo -> pointer to application buffer where the version information will be copied.
 Return value     : void
 ***************************************************************************************************/

void ComM_GetVersionInfo(Std_VersionInfoType * versioninfo)
{
    /* Local Variables Declaration */
    /* Local varaiable initialization */
    /************************************DET CHECK STARTS HERE*********************************/
    /* DET check if the pointe is NULL */
    if (versioninfo == NULL_PTR)
    {
        /* set DET error with NULL parameter */
        COMM_DET_REPORT_ERROR(COMM_GETVERINFO_APIID, COMM_E_WRONG_PARAMETERS);
        return;
    }

    /************************************DET CHECK ENDS HERE**********************************/

    /* Assign the version information to the pointer */
    versioninfo->vendorID = COMM_VENDOR_ID;
    versioninfo->moduleID = COMM_MODULE_ID;
    versioninfo->sw_major_version = COMM_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = COMM_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = COMM_SW_PATCH_VERSION;
    return;
}

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#endif/*  #if (COMM_VERSION_INFO_API != STD_OFF)  */

