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

#ifndef FEE_CFG_SCHM_H
#define FEE_CFG_SCHM_H

/**
 **********************************************************************************************************************
 * \file   Fee_Cfg_SchM.h
 * \brief  Schedule manager interface used by the Fee components
 **********************************************************************************************************************
 * NOTE: All definitions within this headers have to be adjusted according to the project needs
 * Within the provided template only one example is provided.
 * Please consult the documentation of Fls for details of the functionality required.
 */



/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "Std_Types.h"

/*
  **********************************************************************************************************************
  * Defines/Macros
  **********************************************************************************************************************
  */

 /*
  * Optimization possibility if Fee_MainFunction protection needs not protected against multi-calls at the
  * same time. This requires that above protection functions are used.
  * FEE_RB_USE_PROTECTION = TRUE:    Protection enabled (more code)     (DEFAULT)
  * FEE_RB_USE_PROTECTION = FALSE:   no protection - only useful in single-core/task configurations
  *
  * Detection of reentrant Main Function calls will be enabled irrespective of this feature,
  * only prevention is controlled by this compiler switch
  *
  * The following consequences may result if the feature is enabled in a project where reentrant
  * invocation of the Fee_Mainfunction is not handled completely,
  *
  * Additional consumption of runtime if the feature is enabled
  *
  * Priority reversal (i.e., If an interrupted event waits for completion of certain operation upon
  * calling Fee_Mainfunction and the interrupt occured in the middle of Fee_Mainfunction execution)
  *
  * Jumping out in the middle of Fee_Mainfunction to shutdown is clearly forbidden (i.e., If Fee_Mainfunction is
  * expected to perform some activity in the shutdown sequence, it will never work anymore, since the
  * interrupted function will have the lock forever)
  *
  */
 #define FEE_RB_USE_PROTECTION   (TRUE)

/*
 **********************************************************************************************************************
 * User Includes
 **********************************************************************************************************************
 */
/* include the needed spinlock/interrupt drivers */
//proposal: #include "rba_BswSrv.h"


/*
 **********************************************************************************************************************
 * Inline Functions
 **********************************************************************************************************************
 */
/*
 * The implementation of this function has to ensure that no other
 * Fee component code is executed while the code between
 * SchM_Enter_Fee_Order() and SchM_Exit_Fee_Order() is executed.
 *
 * Typically a spinlock is used here.
 * Remark: Only if the Fls-requests are issued from the same or lower priority task and the same core
 * as the Fee, it is possible to keep the protection empty.
 *
 * This protection is only used for very short code sequences (<=20 copy-operation equivalent).
 */
LOCAL_INLINE void SchM_Enter_Fee_Order(void)
{
    // proposal:    rba_BswSrv_GetLockCommon();
}
LOCAL_INLINE void SchM_Exit_Fee_Order(void)
{
    // proposal:    rba_BswSrv_ReleaseLockCommon();
}

/* FEE_CFG_SCHM_H */
#endif
