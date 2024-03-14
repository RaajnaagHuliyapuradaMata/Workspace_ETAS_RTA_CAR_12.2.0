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

#ifndef EA_CFG_SCHM_H
#define EA_CFG_SCHM_H

/**
 **********************************************************************************************************************
 * \file   Ea_Cfg_SchM.h
 * \brief  Schedule manager interface used by the Ea components
 **********************************************************************************************************************
 * NOTE: All definitions within this headers have to be adjusted according to the project needs
 * Within the provided template only one example is provided.
 * Please consult the documentation of Ea for details of the functionality required.
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
 * Optimization possibility if Ea_MainFunction protection needs not protected against multi-calls at the
 * same time. This requires that above protection functions are used.
 * EA_RB_USE_PROTECTION = TRUE:    Protection enabled (more code)     (DEFAULT)
 * EA_RB_USE_PROTECTION = FALSE:   no protection - only useful in single-core/task configurations
 */
#define EA_RB_USE_PROTECTION   (TRUE)

/*
 **********************************************************************************************************************
 * User Includes
 **********************************************************************************************************************
 */
/* include the needed spinlock/interrupt drivers */
//proposal: #include "rba_BswSrv.h"

/*
 ***************************************************************************************************
 * Inline Functions
 ***************************************************************************************************
 */
/* To be defined by the integrator */
LOCAL_INLINE void SchM_Enter_Ea_Order(void)
{
    //proposal:     rba_BswSrv_GetLockCommon();
}

/* To be defined by the integrator */
LOCAL_INLINE void SchM_Exit_Ea_Order(void)
{
    //proposal:     rba_BswSrv_ReleaseLockCommon();
}

/* EA_CFG_SCHM_H */
#endif
