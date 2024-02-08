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

/*****************************************************************************************
 * Header Includes *
 *****************************************************************************************/

#include "Std_Types.h"
#include "BswM.h"
#include "BswM_Prv.h"

/***********************************************************
* Function name: void BswM_Appl_IncompatibleGenerator( void )
* Description: Function called by BswM in case of version incompatibility in the post-build generate.
* This an empty function. User will write the error handler code here.
* Return value: None
* Remarks:
***********************************************************/

#define BSWM_START_SEC_CODE
#include "BswM_MemMap.h"

void BswM_Appl_IncompatibleGenerator( void )
{
    /* Fill the error handling code to handle the version mis-match in the post-build structure */
    return;
}

#define BSWM_STOP_SEC_CODE
#include "BswM_MemMap.h"


/*
 **********************************************************************************************************************
*/
