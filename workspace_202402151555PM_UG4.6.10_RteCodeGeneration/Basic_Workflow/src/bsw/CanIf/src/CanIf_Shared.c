/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2017, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/


/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "CanIf_Prv.h"




/* Flag to check if CANIF is Initialized(TRUE) or Not Initialized (FALSE)*/
#define CANIF_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"

boolean CanIf_Prv_InitStatus_b;
CanIf_ControllerStateType CanIf_Prv_ControllerState_ast[CANIF_TOTAL_CTRLS];

#if (CANIF_XCORE_CFG_ENABLED != STD_OFF)

boolean CanIf_XCore_EndBuf_InvalidFlag_b;
boolean CanIf_XCore_RxClrPipeFlag_b;
#endif

#define CANIF_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"

/*
 ***************************************************************************************************
 * Global Variables/Arrays
 ***************************************************************************************************
 */

#if (CANIF_XCORE_CFG_ENABLED != STD_OFF)
/*RAM array for information about Pipe objects*/
#define CANIF_START_SEC_VAR_FAST_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"
CanIf_XCore_PipeRuntimeInfoType_st CanIf_XCore_PipeRuntimeInfo_ast[CANIF_XCORE_MAX_NUM_PIPES];
#define CANIF_STOP_SEC_VAR_FAST_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"
#endif
