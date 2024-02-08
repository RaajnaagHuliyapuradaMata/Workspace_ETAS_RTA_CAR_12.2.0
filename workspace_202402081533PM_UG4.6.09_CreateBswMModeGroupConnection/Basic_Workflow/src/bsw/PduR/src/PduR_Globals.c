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
/* Provide definition of PduR variables that are independent of PDUR_DEV_ERROR_DETECT and
 * which always need to be linked into any application using the Pdu Router
 */

#include "PduR_Prv.h"
/* ------------------------------------------------------------------------ */
/* Begin a set of variable definitions */

#define PDUR_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "PduR_MemMap.h"

const PduR_PBConfigType * PduR_Base;

PduR_StateType PduR_State;


#if (PDUR_DSM_REINT_SUPPORT != STD_OFF)

PduR_DsmReInitType PduR_DsmReInitStruct;

#endif
#define PDUR_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "PduR_MemMap.h"
