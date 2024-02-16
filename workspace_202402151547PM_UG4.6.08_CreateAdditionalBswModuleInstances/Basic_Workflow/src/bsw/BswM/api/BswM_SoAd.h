/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef BSWM_SOAD_H
#define BSWM_SOAD_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "BswM_Cfg_General.h"

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

/* Function declaration will be available only if SoAd is enabled in the project  */
#if (defined(BSWM_RB_SOAD_ENABLED) && (BSWM_RB_SOAD_ENABLED == TRUE))

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

extern void BswM_SoAd_SoConModeChg(SoAd_SoConIdType SoConId, SoAd_SoConModeType State);

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif /* BSWM_RB_SOAD_ENABLED */

#endif /* BSWM_SOAD_H */

/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
