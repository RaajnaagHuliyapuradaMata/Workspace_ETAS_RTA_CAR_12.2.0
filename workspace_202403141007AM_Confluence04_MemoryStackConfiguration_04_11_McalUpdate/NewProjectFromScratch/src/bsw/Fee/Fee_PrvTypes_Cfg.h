/*
**********************************************************************************************************************
*
* COPYRIGHT (c) ETAS GmbH 2021, all rights reserved
* The reproduction, distribution and utilization of this document as well as the communication of its contents to
* others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
**********************************************************************************************************************
*/

#ifndef FEE_PRVTYPES_CFG_H
#define FEE_PRVTYPES_CFG_H

/* ******************************************************************************************************************
   *************************************************** Includes *****************************************************
   ****************************************************************************************************************** */

#include "Fee_Prv_Config.h"

/* ******************************************************************************************************************
   ********************************************* External declarations **********************************************
   ****************************************************************************************************************** */
 
/* The variables in this section are made extern only for variable measurement purposes.
 * They shall shall not be accessed for read or write in other components. */
 
#define FEE_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

extern Fee_Prv_Data_tst Fee_Prv_Data_st;

#define FEE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Fee_MemMap.h"

#endif /* FEE_PRVTYPES_CFG_H */
