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
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "Ea.h"

#if(defined(EA_RB_CFG_SELECTED_FS) && (1 == EA_RB_CFG_SELECTED_FS))

#include "Ea_Rb_EaFs1_EepDepndcyAdaptr.h"

/*
 ***************************************************************************************************
 * NOTE:
 *  If Ea module is integrated with Eep driver then editing of this file is not necessary.
 *    This file is provided to support integration of Ea module with 3rd party Eep Driver.
 *    This file needs to be edited only in case of Ea module is integrated with 3rd party Eep Driver.
 *
 ***************************************************************************************************
 */


/*
 ***************************************************************************************************
 * Function definition
 ***************************************************************************************************
 */

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

void Ea_Rb_EaFs1_EepScheduleDependency(void)
{
#if (EEP_VENDOR_ID == EA_VENDOR_ID)
    Spi_MainFunction_Handling();    /* Integrator Hint: If External Eeprom is used Spi_MainFunction_Handling should be kept       */
    /* If Internal Eeprom is used Spi_MainFunction_Handling function calling should be commented  */
#endif
}

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif
