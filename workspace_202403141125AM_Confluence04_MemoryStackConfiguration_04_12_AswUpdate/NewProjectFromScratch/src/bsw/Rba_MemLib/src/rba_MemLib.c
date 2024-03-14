/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#include "rba_MemLib.h"
#include "rba_MemLib_Cfg.h"


/**
 *********************************************************************
 * Code
 *********************************************************************
 */

#define  RBA_MEMLIB_START_SEC_CODE
#include "rba_MemLib_MemMap.h"

#if(defined(RBA_MEMLIB_VERSION_INFO) && (RBA_MEMLIB_VERSION_INFO != STD_OFF))
/**
 *********************************************************************
 * rba_MemLib_GetVersionInfo(): Return version information
 *
 * This function returns general and SW version information
 *
 * \param   VersioninfoPtr: Pointer to return the version
 *                          information of this module.
 * \return  None
 * \retval
 * \seealso
 * \usedresources
 *********************************************************************
 */
void rba_MemLib_GetVersionInfo(Std_VersionInfoType *VersionInfoPtr)
{
    VersionInfoPtr->vendorID = 6u;      /* RB=6 */
    VersionInfoPtr->moduleID = 255u;    /* not standarized=255 */

    VersionInfoPtr->sw_major_version   = RBA_MEMLIB_SW_MAJOR_VERSION;
    VersionInfoPtr->sw_minor_version   = RBA_MEMLIB_SW_MINOR_VERSION;
    VersionInfoPtr->sw_patch_version   = RBA_MEMLIB_SW_PATCH_VERSION;

}
#endif /* MEMLIB_VERSION_INFO_API */

#define  RBA_MEMLIB_STOP_SEC_CODE
#include "rba_MemLib_MemMap.h"
