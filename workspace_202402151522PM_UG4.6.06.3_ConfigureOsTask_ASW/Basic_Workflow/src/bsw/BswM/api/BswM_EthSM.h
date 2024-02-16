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
#ifndef BSWM_ETHSM_H
#define BSWM_ETHSM_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "BswM_Cfg_General.h"

#if (defined(BSWM_ETHSM_ENABLED) && (BSWM_ETHSM_ENABLED == TRUE))

#include "EthSM.h"
//#if (!defined(ETHSM_AR_RELEASE_MAJOR_VERSION) || (ETHSM_AR_RELEASE_MAJOR_VERSION != BSWM_AR_RELEASE_MAJOR_VERSION))
//#error "AUTOSAR major version undefined or mismatched"
//#endif
//#if (!defined(ETHSM_AR_RELEASE_MINOR_VERSION) || (ETHSM_AR_RELEASE_MINOR_VERSION != BSWM_AR_RELEASE_MINOR_VERSION))
//#error "AUTOSAR minor version undefined or mismatched"
//#endif

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

extern void BswM_EthSM_CurrentState(NetworkHandleType Network, EthSM_NetworkModeStateType CurrentState);

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif

#endif
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
