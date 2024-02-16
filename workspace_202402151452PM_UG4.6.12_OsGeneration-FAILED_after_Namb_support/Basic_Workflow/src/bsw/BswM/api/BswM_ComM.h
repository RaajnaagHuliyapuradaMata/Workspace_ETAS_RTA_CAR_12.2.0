/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2016, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef BSWM_COMM_H
#define BSWM_COMM_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "BswM_Cfg_General.h"

#if (defined(BSWM_COMM_ENABLED) && (BSWM_COMM_ENABLED == TRUE))

#include "ComM.h"
//#if (!defined(COMM_AR_RELEASE_MAJOR_VERSION) || (COMM_AR_RELEASE_MAJOR_VERSION != BSWM_AR_RELEASE_MAJOR_VERSION))
//#error "AUTOSAR major version undefined or mismatched"
//#endif
//#if (!defined(COMM_AR_RELEASE_MINOR_VERSION) || (COMM_AR_RELEASE_MINOR_VERSION != BSWM_AR_RELEASE_MINOR_VERSION))
//#error "AUTOSAR minor version undefined or mismatched"
//#endif

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

extern void BswM_ComM_InitiateReset(void);
extern void BswM_ComM_CurrentMode (NetworkHandleType Network, ComM_ModeType RequestedMode);
extern void BswM_ComM_CurrentPNCMode(PNCHandleType PNC, ComM_PncModeType CurrentPncMode);

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif /* BSWM_COMM_ENABLED */

#endif
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
