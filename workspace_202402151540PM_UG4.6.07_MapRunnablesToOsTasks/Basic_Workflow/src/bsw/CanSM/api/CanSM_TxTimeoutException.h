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
#ifndef  CANSM_TXTIMEOUTEXCEPTION_H
#define  CANSM_TXTIMEOUTEXCEPTION_H

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
extern void CanSM_TxTimeoutException(NetworkHandleType Channel);
#define CANSM_STOP_SEC_CODE
#include "CanSM_MemMap.h"

#endif /*CANSM_TXTIMEOUTEXCEPTION_H*/
