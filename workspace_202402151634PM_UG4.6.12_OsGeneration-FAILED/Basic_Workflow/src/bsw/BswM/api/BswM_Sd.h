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
#ifndef BSWM_SD_H
#define BSWM_SD_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Std_Types.h"
#include "BswM_Cfg_General.h"

#ifdef BSWM_SD_ENABLED
#if (BSWM_SD_ENABLED == TRUE)

#include "Sd.h"
//#if (!defined(SD_AR_RELEASE_MAJOR_VERSION) || (SD_AR_RELEASE_MAJOR_VERSION != BSWM_AR_RELEASE_MAJOR_VERSION))
//#error "AUTOSAR major version undefined or mismatched"
//#endif
//#if (!defined(SD_AR_RELEASE_MINOR_VERSION) || (SD_AR_RELEASE_MINOR_VERSION != BSWM_AR_RELEASE_MINOR_VERSION))
//#error "AUTOSAR minor version undefined or mismatched"
//#endif

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

extern void BswM_Sd_EventHandlerCurrentState(uint16 SdEventHandlerHandleId, Sd_EventHandlerCurrentStateType EventHandlerStatus);
extern void BswM_Sd_ClientServiceCurrentState(uint16 SdClientServiceHandleId, Sd_ClientServiceCurrentStateType CurrentClientState);
extern void BswM_Sd_ConsumedEventGroupCurrentState(uint16 SdConsumedEventGroupHandleId, Sd_ConsumedEventGroupCurrentStateType ConsumedEventGroupState);

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif //BSWM_SD_ENABLED == TRUE
#endif //BSWM_SD_ENABLED


#endif
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
