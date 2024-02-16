/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2010, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
    CONTAINS CALLBACK/CALLOUT FUNCTION PROTOTYPES
    ALL DECLARATIONS NECESSARY TO INTERACT WITH CALLBACKS AND CALLOUTS OF ECUM MODULE
        - dont put any datatype declaration here if not used by this file. Put in EcuM.h
***********************************************************************************************************************
*/

#ifndef ECUM_CBK_H
#define ECUM_CBK_H

/**************************************************************************************************/
/* Preprocessor includes                                                                          */
/**************************************************************************************************/

#include "Std_Types.h" /*Rule BSW_HeaderInc_004:*/

#include "EcuM_Types.h" /* Required for EcuM exported data types */

#include "EcuM_Generated_Types.h" /* specified in SWS. */

#include "EcuM_Cfg_Version.h" /*Version details for the implementation*/

#include "EcuM_Cfg_CbkStartup.h"

/**************************************************************************************************/
/* ALL DECLARATIONS NECESSARY TO INTERACT WITH CALLBACKS AND CALLOUTS OF ECUM MODULE                                                                          */
/**************************************************************************************************/

#define ECUM_START_SEC_CODE
#include "EcuM_MemMap.h"

/*************************************************************************************************
 CALLBACK FUNCTION PROTOTYPES
*************************************************************************************************/

extern void EcuM_SetWakeupEvent( EcuM_WakeupSourceType sources );

extern void EcuM_ValidateWakeupEvent( EcuM_WakeupSourceType sources );

extern void EcuM_CheckWakeup( EcuM_WakeupSourceType wakeupSource );



#define ECUM_STOP_SEC_CODE
#include "EcuM_MemMap.h"

#endif /*ECUM_CBK_H*/

