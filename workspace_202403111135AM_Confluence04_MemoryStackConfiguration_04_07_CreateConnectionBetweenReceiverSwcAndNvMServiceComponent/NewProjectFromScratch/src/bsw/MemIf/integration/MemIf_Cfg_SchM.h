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

#ifndef MEMIF_CFG_SCHM_H
#define MEMIF_CFG_SCHM_H



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
 * Optimization possibility if MemIf_Rb_MainFunction protection needs not protected against multi-calls at the
 * same time. This requires that above protection functions are used.
 * MEMIF_RB_USE_PROTECTION = TRUE:    Protection enabled (more code)     (DEFAULT)
 * MEMIF_RB_USE_PROTECTION = FALSE:   no protection - only useful in single-core/task configurations
 *
 * Detection of reentrant Main Function calls will be enabled irrespective of this feature,
 * only prevention is controlled by this compiler switch
 *
 * The Following are consequences of enabling this feature if the reentrant scenario of MemIf_Rb_MainFunction
 * is not handled completely,
 *
 * Additional consumption of runtime if the feature is enabled
 *
 * Priority reversal (i.e., If an interrupted event waits for completion of certain operation upon calling
 * MemIf_Rb_MainFunction and the interrupt occured in the middle of MemIf_Rb_Mainfunction execution)
 *
 * Jumping out in the middle of MemIf_Rb_MainFunction to shutdown is clearly forbidden
 * (i.e., If MemIf_Rb_MainFunction is expected to perform some activity in the shutdown sequence,
 * it will never work anymore, since the interrupted function will have the lock forever)
 */
#define MEMIF_RB_USE_PROTECTION   (TRUE)

/*
 **********************************************************************************************************************
 * User Includes
 **********************************************************************************************************************
*/
/* This is only needed if MEMIF_RB_USE_PROTECTION is TRUE and below functions are implemented. */
//proposal: #include "rba_BswSrv.h"

/*
 **********************************************************************************************************************
 * Inline Functions
 **********************************************************************************************************************
*/

/* To be defined by the integrator
 * *******************************
 * Exclusive area "MemIf_Main":
 * - This exclusive area protects all accesses to shared ressources within the MemIf module,
 *   in particular between MemIf's public APIs and the MemIf_Rb_MainFunction.
 * - On multi-core machines, a lock functionality is required which works across all cores which could invoke MemIf's
 *   public APIs or schedules the MemIf_Rb_MainFunction.
 * - On single core machines, a global interrupt lock is sufficient.
 *
 * Hint:
 * As NvM_MainFunction and MemIf_Rb_MainFunction have to be scheduled in the same task, the configuration of
 * NvM can be overtaken also for MemIf
 */
LOCAL_INLINE void SchM_Enter_MemIf_Main(void)
{
//proposal:    rba_BswSrv_GetLockCommon();
}
LOCAL_INLINE void SchM_Exit_MemIf_Main(void)
{
//proposal:    rba_BswSrv_ReleaseLockCommon();
}


/* MEMIF_CFG_SCHM_H */
#endif
