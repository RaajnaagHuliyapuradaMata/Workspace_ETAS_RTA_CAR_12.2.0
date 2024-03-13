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
#ifndef ECUM_COMPILERSPECIFIC_H
#define ECUM_COMPILERSPECIFIC_H

/*The below macro 'NORETURN' defines the compiler attribute to declare a
function which would not return its control back to the caller.*/
/* The integrator could define this macro based on their compiler specific attribute
or may undefine this macro if not applicable.*/

#ifndef NORETURN
#define NORETURN /* The integrator shall add macro expansion (compiler specific keyword) here eg: __attribute__ ((__noreturn__)) for GCC compiler */
#endif

#endif /* ECUM_COMPILERSPECIFIC_H*/


