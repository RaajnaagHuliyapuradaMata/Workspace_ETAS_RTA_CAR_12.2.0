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
 * COPYRIGHT (c) ETAS GmbH 2012, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef DET_CFG_SCHM_H
#define DET_CFG_SCHM_H

#include "SchM_Default.h"

/* This lock is always required, as the component using the DET are usually runing in different tasks as DET and
 * therefore may interrupt the processing of DET at any time.
 */


LOCAL_INLINE void SchM_Enter_Det_Monitoring(void)
{
    /*The integrator shall place his code here which would disable/lock the interrupt*/
    /* It is recommended that the projects use Common lock here as it uses LESS runtime. NoNest locks if used will only reduce cross core runtime influence */
    SCHM_ENTER_DEFAULT();
}

LOCAL_INLINE void SchM_Exit_Det_Monitoring(void)
{
    /*The integrator shall place his code here which would disable/lock the interrupt*/
    /* It is recommended that the projects use Common lock here as it uses LESS runtime. NoNest locks if used will only reduce cross core runtime influence */
   SCHM_EXIT_DEFAULT();
}

#endif /* include protection */
