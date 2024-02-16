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

#include "rba_DiagLib.h"

#define RBA_DIAGLIB_START_SEC_CODE
#include "rba_DiagLib_MemMap.h"
//this function is only necessary until another function is implemented
//it is only temporarely necessary for the coverage and misra tests
boolean rba_DiagLib_ByteOrderUtils_Dummy(void){
    return TRUE;
}
#define RBA_DIAGLIB_STOP_SEC_CODE
#include "rba_DiagLib_MemMap.h"
