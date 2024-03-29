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
#ifndef BSWM_GENERIC_H
#define BSWM_GENERIC_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Std_Types.h"
#include "BswM_Cfg_General.h"

#include "BswM_Types.h"

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

/* Function called by a generic mode requester */
extern void BswM_RequestMode(BswM_UserType requesting_user, BswM_ModeType requested_mode);

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"

#endif
/**********************************************************************************************************************
 * End of header file
 **********************************************************************************************************************/
