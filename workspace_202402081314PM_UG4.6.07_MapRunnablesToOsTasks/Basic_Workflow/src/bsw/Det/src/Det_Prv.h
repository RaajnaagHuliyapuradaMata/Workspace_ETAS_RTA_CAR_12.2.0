/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2012, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef DET_PRV_H
#define DET_PRV_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Det_Types.h"
#include "Det_Cfg.h"
#include "rba_DiagLib_MemUtils.h"

/*
 **********************************************************************************************************************
 * Macros
 **********************************************************************************************************************
 */

/* Abstraction to the MemSet function */
#define DET_MEMSET(xDest_pv, xPattern_u32, numBytes_u32)  RBA_DIAGLIB_MEMSET((xDest_pv),(xPattern_u32),(numBytes_u32))

/* Abstraction to void cast */
#define DET_UNUSED_PARAM(P)   ((void)(P))

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */
#define DET_START_SEC_VAR_INIT_UNSPECIFIED
#include "Det_MemMap.h"

extern boolean Det_Initialized_b;
extern const Det_ConfigType* Det_ActiveConfigPtr;

#define DET_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Det_MemMap.h"

#define DET_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Det_MemMap.h"

#if (DET_CFG_ERRORBUFFER_ENABLED)
extern Det_ErrorEntryBufferType Det_ErrorEntryBuffer[DET_CFG_ERRORBUFFERSIZE];
#endif /* #if (DET_CFG_ERRORBUFFER_ENABLED) */

#define DET_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Det_MemMap.h"

#endif /* #ifndef DET_PRV_H */
