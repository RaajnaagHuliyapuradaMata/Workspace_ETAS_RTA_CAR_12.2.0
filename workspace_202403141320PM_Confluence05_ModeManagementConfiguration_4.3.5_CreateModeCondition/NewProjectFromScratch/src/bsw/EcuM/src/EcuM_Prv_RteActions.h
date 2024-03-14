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
#ifndef ECUM_PRV_RTEACTIONS_H
#define ECUM_PRV_RTEACTIONS_H

/**************************************************************************************************/
/* Preprocessor includes                                                                          */
/**************************************************************************************************/
#include "EcuM_Types.h"
/*************************************************************************************************
 Variables
*************************************************************************************************
*/
#if (ECUM_CFG_MODE_HANDLING == STD_ON)

#define ECUM_START_SEC_CODE
#include "EcuM_MemMap.h"

extern void EcuM_Prv_RteModeSwitch ( Rte_ModeType_EcuM_Mode Mode);

extern uint8 EcuM_Prv_GetRteMode(void);

#define ECUM_STOP_SEC_CODE
#include "EcuM_MemMap.h"

#endif

#endif /*ECUM_PRV_RTEACTIONS_H*/
