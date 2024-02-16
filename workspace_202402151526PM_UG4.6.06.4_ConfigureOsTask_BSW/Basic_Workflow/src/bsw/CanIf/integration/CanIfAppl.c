/*
* *********************************************************************************************************************
* This is a template file.
* It defines integration functions which are necessary to be implemented.
* The integrator must complete the templates before deploying software containing functions defined in this file.
* Once templates have been completed, the integrator should delete the "removal part"
* Note: The integrator is responsible for updates made to this file.
*
* To suppress following error message, define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT
* during compiler invocation (e.g. -DNOT_READY_FOR_TESTING_OR_DEPLOYMENT)
* The suppression of the error only allows the user to proceed with the building phase.
* *********************************************************************************************************************
*/

/*---- Removal start ----*/
/* Hint for the integrator: remove everything between Removal start and Removal end after proper implementation */

#ifndef NOT_READY_FOR_TESTING_OR_DEPLOYMENT
# error The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#else
# warning The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#endif /* NOT_READY_FOR_TESTING_OR_DEPLOYMENT */
/*---- Removal end ----*/

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

/*
 * This file includes application functions that are called by CanIf
 * A dummy implementation is provided here. User has to implement the actual functionality
 * */

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "CanIf_Prv.h"

/***************************************************************************************************
 Function name    : CanIfAppl_IncompatibleGenerator

 Description      : This is CanIfAppl function called when the Incompatible version of the generator
                    is used in comparison to what was originally used.
                    This error is detected during CanIf_Init() function call
                    If there is an error, then CanIf module will remain in NM_STATE_UNINIT.
                    It will not be possible to use the module functionality, in case of this failure
                    NOTE : If DET is enabled, then most of APIs check if the module is initialised. If DET is disabled
                    most of the APIs will not check for uninitialised state. If the module APIs are called when this
                    error happens , s/w might go to trap.
                    Hence user has to ensure that when this error is detected, module APIs are not called. Ex : Don't
                    start OS
                    This function is called only when post-build loadable feature is used.
 Parameter        : void
 Return value     : void
 ***************************************************************************************************
 */
#if(CANIF_CONFIGURATION_VARIANT > 1)

#define CANIF_START_SEC_CODE
#include "CanIf_MemMap.h"

void CanIfAppl_IncompatibleGenerator(void)
{
    // User can report DET error / DEM error etc here

}


#define CANIF_STOP_SEC_CODE
#include "CanIf_MemMap.h"

#endif
