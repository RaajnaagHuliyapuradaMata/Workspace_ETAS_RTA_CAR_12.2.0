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
#ifndef BSWM_PRV_PROCESSDEFERREDREQUEST_H
#define BSWM_PRV_PROCESSDEFERREDREQUEST_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/


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
 * Variables
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/
#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

extern void BswM_Prv_ClearAllDeferredRequests (void);
extern void BswM_Prv_StoreDeferredRequest(BswM_Cfg_MRPType_ten dataMRPType_en, uint32 dataChannelID_u32, const uint16 * adrAssociatedRules_pu16, uint16 nrAssociatedRules_u16);
/*Function declaration for processing deferred requests*/
extern void BswM_Prv_ProcessDeferredReqst( void );

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
