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
 ***********************************************************************************************************************
 *
 ***********************************************************************************************************************
*/

#ifndef FEE_RB_PRJ_H
#define FEE_RB_PRJ_H

/**
 **********************************************************************************************************************
 * \file   Fee_Rb_Prj.h
 * \brief  Extension definition provided by the project and used by the FeeHL/FeeFsX
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "Fls.h"        // Required for Fls_AddressType and Fls_LengthType
#include "Std_Types.h"  // Required for Std_ReturnType and STD_ON

/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */

#define FLS_BLANK_CHECK_API         (STD_ON)
#define FLS_BLANK_CHECK_API_AVAIL   (STD_ON)

/*
 **********************************************************************************************************************
 * Typedefs
 **********************************************************************************************************************
 */

typedef Fls_AddressType Fls_Rb_AddressType;
typedef Fls_LengthType  Fls_Rb_LengthType;

/*
 **********************************************************************************************************************
 * Extern Declaration
 **********************************************************************************************************************
 */

#if (defined(FLS_AR_RELEASE_MINOR_VERSION) && (0 == FLS_AR_RELEASE_MINOR_VERSION))

#define FEE_START_SEC_CODE
#include "Fee_MemMap.h"
extern Std_ReturnType Fls_BlankCheck(Fls_AddressType TargetAddress, Fls_LengthType Length);
#define FEE_STOP_SEC_CODE
#include "Fee_MemMap.h"

#endif

#endif
