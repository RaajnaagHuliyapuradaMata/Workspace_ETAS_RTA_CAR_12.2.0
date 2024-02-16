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
#ifndef BSWM_NVM_H
#define BSWM_NVM_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Std_Types.h"
#include "BswM_Cfg_General.h"

#ifdef BSWM_NVM_ENABLED
#if ( BSWM_NVM_ENABLED == TRUE )

#include "NvM.h"
//#if (!defined(NVM_AR_RELEASE_MAJOR_VERSION) || (NVM_AR_RELEASE_MAJOR_VERSION != BSWM_AR_RELEASE_MAJOR_VERSION))
//#error "AUTOSAR major version undefined or mismatched"
//#endif
//#if (!defined(NVM_AR_RELEASE_MINOR_VERSION) || (NVM_AR_RELEASE_MINOR_VERSION != BSWM_AR_RELEASE_MINOR_VERSION))
//#error "AUTOSAR minor version undefined or mismatched"
//#endif

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/
extern void BswM_NvM_CurrentBlockMode(NvM_BlockIdType Block, NvM_RequestResultType CurrentBlockMode);
extern void BswM_NvM_CurrentJobMode(uint8 ServiceId, NvM_RequestResultType CurrentJobMode);

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif /* ( BSWM_NVM_ENABLED == TRUE ) */
#endif /* BSWM_NVM_ENABLED */

#endif /* BSWM_NVM_H */
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
