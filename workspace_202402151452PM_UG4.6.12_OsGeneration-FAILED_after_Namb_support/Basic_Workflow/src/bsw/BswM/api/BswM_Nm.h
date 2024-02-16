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
#ifndef BSWM_NM_H
#define BSWM_NM_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "ComStack_Types.h"
#include "BswM_Cfg_General.h"

#ifdef BSWM_NM_ENABLED
#if ( BSWM_NM_ENABLED == TRUE )

#include "Nm.h"
//#if (!defined(NM_AR_RELEASE_MAJOR_VERSION) || (NM_AR_RELEASE_MAJOR_VERSION != BSWM_AR_RELEASE_MAJOR_VERSION))
//#error "AUTOSAR major version undefined or mismatched"
//#endif
//#if (!defined(NM_AR_RELEASE_MINOR_VERSION) || (NM_AR_RELEASE_MINOR_VERSION != BSWM_AR_RELEASE_MINOR_VERSION))
//#error "AUTOSAR minor version undefined or mismatched"
//#endif

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

/* Function called by Nm to indicate a CarWakeup. */
extern void BswM_Nm_CarWakeUpIndication( NetworkHandleType Network );
/* Function called by Nm to indicate a state change. */
extern void BswM_Nm_StateChangeNotification( NetworkHandleType Network, Nm_StateType currentState);

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif /* ( BSWM_NM_ENABLED == TRUE ) */
#endif /* BSWM_NM_ENABLED */

#endif /* BSWM_NM_H */

/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/

