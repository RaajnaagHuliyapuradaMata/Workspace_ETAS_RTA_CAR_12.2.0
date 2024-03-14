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

#ifndef EA_RB_EAFS1_EEPDEPNDCYADAPTR_H
#define EA_RB_EAFS1_EEPDEPNDCYADAPTR_H

#if(defined(EA_RB_CFG_SELECTED_FS) && (1 == EA_RB_CFG_SELECTED_FS))


/**
 **********************************************************************************************************************
 * \file   Ea_Rb_EaFs1_EepDepndcyAdaptr.h
 * \brief  This file is provided to support integration of Ea module with 3rd party Eep Driver.
 **********************************************************************************************************************
 *
 ***************************************************************************************************
 * Note:
 *    If Ea module is integrated with Eep driver then editing of this file is not necessary.
 *    This file is provided to support integration of Ea module with 3rd party Eep Driver.
 *    This file needs to be edited only in case of Ea module is integrated with 3rd party Eep Driver.
 *
 ***************************************************************************************************
 */




/*
 ***************************************************************************************************
 * Configuration Macros:
 ***************************************************************************************************
 */

/* Proposal
   //EA_RB_EEP_ERASE_VALUE this holds the Autosar Eep Configuration Parameter "EepEraseValue" that is ECUC_Eep_00180 SWS item
   //EepEraseValue parameter will be present in Eep_Cfg.h file

#define EA_RB_EEP_ERASE_VALUE    0xFFu    // The "EA_RB_EEP_ERASE_VALUE" should be assigned with "EepEraseValue" define present in Eep_Cfg.h

   //EA_RB_EEP_SIZE this holds the Autosar Eep Configuration Parameter "EepSize" that is ECUC_Eep_00175 SWS item
   //EepSize parameter will be present in Eep_Cfg.h file

#define EA_RB_EEP_SIZE           0x20000uL // The "EA_RB_EEP_SIZE" should be assigned with "EepSize" define present in Eep_Cfg.h

   //Eep_Mode variable holds the value of Autosar Eep Configuration Parameter "EepDefaultMode" that is ECUC_Eep_00167 SWS item.This variable is modified
   //by Autosar Eep_SetMode function

#define EA_RB_EEP_MODE           Eep_Mode   // The "Eep_Mode" should be replaced with variable defined in Eep driver to hold Configuration Parameter "EepDefaultMode"

*/

/*---- Removal start ----*/
#warning The content of this file is a template which provides empty stubs. Remove this after completed implementation.

/* To use with the Eep driver, following definitions shall be used*/
#define EA_RB_EEP_ERASE_VALUE    0u
#ifdef EEP_RB_SIZE_DETECTION
#define EA_RB_EEP_SIZE           (uint32)Eep_Rb_RealSize_u32
#else
#define EA_RB_EEP_SIZE           EEP_SIZE
#endif
#define EA_RB_EEP_MODE           Ea_Mode
/*---- Removal end ----*/

/*
 ***************************************************************************************************
 * Function Declaration
 ***************************************************************************************************
 */

#define EA_START_SEC_CODE
#include "Ea_MemMap.h"

extern void Ea_Rb_EaFs1_EepScheduleDependency(void);

#define EA_STOP_SEC_CODE
#include "Ea_MemMap.h"

#endif

#endif    /* EA_RB_EAFS1_EEPDEPNDCYADAPTR_H */
