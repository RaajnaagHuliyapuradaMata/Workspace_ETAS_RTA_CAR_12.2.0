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
\defgroup COMM_DCM_H    COMM - AUTOSAR interfaces Upper Layer
*/

#ifndef COMM_DCM_H
#define COMM_DCM_H
/*
 ***************************************************************************************************
 * Include
 ***************************************************************************************************
 */
#include "ComM_Cfg.h"
#include "ComStack_Types.h"
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

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"

/**
 * @ingroup COMM_DCM_H
 *
 * This function Indicates active diagnostic by the DCM  \n
 *
 *  @param  In:      Channel - This parameter indicates Channel needed for Diagnostic communication \n
 *
 *  @return None \n
 */
extern void ComM_DCM_ActiveDiagnostic(NetworkHandleType Channel);

/**
 * @ingroup COMM_DCM_H
 *
 * This function Indicates Inactive diagnostic by the DCM  \n
 *
 *  @param  In:      Channel - This parameter indicates Channel no longer needed for Diagnostic communication \n
 *
 *  @return None \n
 */
extern void ComM_DCM_InactiveDiagnostic(NetworkHandleType Channel);

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#endif /*COMM_DCM_H*/

