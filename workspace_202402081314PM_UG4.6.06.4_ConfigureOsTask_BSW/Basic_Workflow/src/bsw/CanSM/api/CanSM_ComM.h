/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2014, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/
#ifndef  CANSM_COMM_H
#define  CANSM_COMM_H

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "CanSM_Cfg.h"
#include "ComM.h"

/*******************************************************************************
**                      Function Prototypes                                   **
*******************************************************************************/
#define CANSM_START_SEC_CODE
#include "CanSM_MemMap.h"

extern Std_ReturnType CanSM_RequestComMode( NetworkHandleType network, ComM_ModeType ComM_Mode );

extern Std_ReturnType CanSM_GetCurrentComMode( NetworkHandleType network,ComM_ModeType * ComM_ModePtr );


#define CANSM_STOP_SEC_CODE
#include "CanSM_MemMap.h"

#endif /* CANSM_COMM_H*/

