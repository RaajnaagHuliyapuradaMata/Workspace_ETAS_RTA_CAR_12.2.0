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

// ----------------------------------------------------------------------------
//! \file
//! \brief Common Exclusive Area Handling
//!
//! $Source: Config/SchM/SchM_Default.h $
//! $Author$
//! $Revision$
//! $Date$
//!
//! To ease Configuring Exclusive Area Handling the component specific handling
//! is mapped to a common handling. Only in case if a component specific
//! handling is required the component specific file "SchM_<MSN>.h" has to be
//! adapted.
//! Design specification: AUTOSAR_SWS_BSW_Scheduler.pdf Version 3.1 Rev0001
// ----------------------------------------------------------------------------



#ifndef SCHM_DEFAULT_H
#define SCHM_DEFAULT_H

#include "Os.h"
//#include "SchM.h"
#include "Compiler.h"

// ---- Includes --------------------------------------------------------------

#define SCHM_ENTER_DEFAULT()    SuspendAllInterrupts()

#define SCHM_EXIT_DEFAULT()     ResumeAllInterrupts()


#endif /* SCHM_DEFAULT_H */

// -------- END OF FILE -------------------------------------------------------
