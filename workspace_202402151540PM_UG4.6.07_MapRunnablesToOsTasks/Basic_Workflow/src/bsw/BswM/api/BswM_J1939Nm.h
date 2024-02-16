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
#ifndef BSWM_J1939NM_H
#define BSWM_J1939NM_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Std_Types.h"
#include "ComStack_Types.h"
#include "BswM_Cfg_General.h"

#ifdef BSWM_J1939NM_ENABLED
#if ( BSWM_J1939NM_ENABLED == TRUE )

#include "J1939Nm.h"
//#if (!defined(J1939NM_AR_RELEASE_MAJOR_VERSION) || (J1939NM_AR_RELEASE_MAJOR_VERSION != BSWM_AR_RELEASE_MAJOR_VERSION))
//#error "AUTOSAR major version undefined or mismatched"
//#endif
//#if (!defined(J1939NM_AR_RELEASE_MINOR_VERSION) || (J1939NM_AR_RELEASE_MINOR_VERSION != BSWM_AR_RELEASE_MINOR_VERSION))
//#error "AUTOSAR minor version undefined or mismatched"
//#endif

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/
#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

extern void BswM_J1939Nm_StateChangeNotification(NetworkHandleType Network, uint8 Node, Nm_StateType NmState);

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif /* ( BSWM_J1939NM_ENABLED == TRUE ) */
#endif /* BSWM_J1939NM_ENABLED */

#endif
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
