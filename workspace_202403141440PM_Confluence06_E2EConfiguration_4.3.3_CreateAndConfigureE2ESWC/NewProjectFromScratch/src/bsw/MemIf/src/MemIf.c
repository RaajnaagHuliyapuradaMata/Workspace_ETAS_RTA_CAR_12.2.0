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

#include "MemIf.h"
#include "MemIf_Cfg_SchM.h"

#define MEMIF_START_SEC_CODE
#include "MemIf_MemMap.h"

#if (defined(MEMIF_VERSION_INFO_API) && (MEMIF_VERSION_INFO_API == STD_ON))
/**
 *********************************************************************
 * MemIf_GetVersionInfo(): Return version information
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
void MemIf_GetVersionInfo(Std_VersionInfoType *VersionInfoPtr)
{
    VersionInfoPtr->vendorID = MEMIF_VENDOR_ID;
    VersionInfoPtr->moduleID = MEMIF_MODULE_ID;
    VersionInfoPtr->sw_major_version = MEMIF_SW_MAJOR_VERSION;
    VersionInfoPtr->sw_minor_version = MEMIF_SW_MINOR_VERSION;
    VersionInfoPtr->sw_patch_version = MEMIF_SW_PATCH_VERSION;
}
#endif /* MEMIF_VERSION_INFO_API */

#define MEMIF_STOP_SEC_CODE
#include "MemIf_MemMap.h"
