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

/**
\defgroup COMM_BUSSM_H    COMM - AUTOSAR interfaces Upper Layer
*/
#ifndef COMM_BUSSM_H
#define COMM_BUSSM_H
/*
 ***************************************************************************************************
 * Include
 ***************************************************************************************************
 */
#include "ComStack_Types.h"
#include "ComM_Cfg.h"
/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */
/*
 ***************************************************************************************************
 * Extern declarations
 ***************************************************************************************************
 */
/**
 * @ingroup COMM_BUSSM_H
 *
 *  This function is used to Indicate the actual bus mode by the corresponding Bus State Manager.   \n
 *
 *  @param  In:      Channel- Handle to identify a communication channel                            \n
 *  @param  In:      ComMode - communication mode                                                   \n
 *
 *  @return None \n
 */

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"

extern void ComM_BusSM_ModeIndication(NetworkHandleType Channel, ComM_ModeType ComMode);

extern void ComM_BusSM_BusSleepMode(NetworkHandleType Channel);

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#endif /*COMM_BUSSM_H*/

