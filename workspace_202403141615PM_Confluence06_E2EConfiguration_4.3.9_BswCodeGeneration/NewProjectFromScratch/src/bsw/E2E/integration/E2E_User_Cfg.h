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

#ifndef E2E_USERCFG_H
#define E2E_USERCFG_H


#endif /* E2E_USERCFG_H */

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

//If the Callouts are enabled E2E will call E2E_Prv_PXXCalcCRCXXXX_Callout(…) instead of E2E_Prv_PXXCalcCRC32XXXX(…)
//The user is supposed to implement the _Callout functions in E2E_Callout.c

#define E2E_USERCFG_ENABLE    1
#define E2E_USERCFG_DISABLE   0

#define E2E_P01USER_CALLOUT E2E_USERCFG_DISABLE
#define E2E_P02USER_CALLOUT E2E_USERCFG_DISABLE
#define E2E_P04USER_CALLOUT E2E_USERCFG_DISABLE
#define E2E_P05USER_CALLOUT E2E_USERCFG_DISABLE
#define E2E_P06USER_CALLOUT E2E_USERCFG_DISABLE
#define E2E_P07USER_CALLOUT E2E_USERCFG_DISABLE
#define E2E_P11USER_CALLOUT E2E_USERCFG_DISABLE
#define E2E_P22USER_CALLOUT E2E_USERCFG_DISABLE
#define E2E_P44USER_CALLOUT E2E_USERCFG_DISABLE

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#if E2E_P01USER_CALLOUT || E2E_P02USER_CALLOUT || E2E_P04USER_CALLOUT || E2E_P05USER_CALLOUT || E2E_P06USER_CALLOUT || E2E_P07USER_CALLOUT || E2E_P11USER_CALLOUT || E2E_P22USER_CALLOUT || E2E_P44USER_CALLOUT
#include "E2E_CallOut.h"
#endif
