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

#ifndef RBA_MEMLIB_CFG_SCHM_H
#define RBA_MEMLIB_CFG_SCHM_H

/**
 **********************************************************************************************************************
 * \file   rba_MemLib_Cfg_SchM.h
 * \brief  Schedule manager interface used by the rba_MemLib component
 **********************************************************************************************************************
 * NOTE: All definitions within this headers have to be adjusted according to the project needs
 * Within the provided template only one example is provided.
 * Please consult the documentation of rba_MemLib for details of the functionality required.
 */



/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
/* Include if available */
// proposal: #include "rba_BswSrv.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */

/* To be defined by the integrator
 * *******************************
 * Exclusive area "rba_MemLib":
 * - This exclusive area protects all accesses to shared ressources within the component,
 * - On multi-core machines, a lock functionality is required which works across all cores which could invoke the component
 * - On single core machines, a global interrupt lock is sufficient.
 *
 * - Note: the run-time within the critical section is very low (refer to docu for details).
 */
/* Here for timer handling */
LOCAL_INLINE void SchM_Enter_rba_MemLib_TimerUs(void)
{
    /* The integrator shall place his/her code here to enter the critical code section */
    // proposal:     rba_BswSrv_GetLockCommon();
}
LOCAL_INLINE void SchM_Exit_rba_MemLib_TimerUs(void)
{
    /* The integrator shall place his/her code here to exit the critical code section */
    // proposal:     rba_BswSrv_ReleaseLockCommon();
}

#endif
