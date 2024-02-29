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
 * COPYRIGHT (c) ETAS GmbH 2016, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/



#ifndef CANSM_CFG_SCHM_H
#define CANSM_CFG_SCHM_H



/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

//#include "SchM.h"

/* If the RTE code generation is Involved, SchM_CanSM.h is included via CanSM_Cfg.h */
#include "CanSM_Cfg.h"

/* The integrator shall implement the particular services SchM_Enter and SchM_Exit */

/* Since there is currently only one resource used, there is no need to consider the resource name */
#define CANSM_START_SEC_CODE
#include "CanSM_MemMap.h"
LOCAL_INLINE void SchM_Enter_CanSM_BOR_Nw_ModesNoNest(void);
LOCAL_INLINE void SchM_Exit_CanSM_BOR_Nw_ModesNoNest(void);

LOCAL_INLINE void SchM_Enter_CanSM_BaudModesNoNest(void);
LOCAL_INLINE void SchM_Exit_CanSM_BaudModesNoNest(void);

LOCAL_INLINE void SchM_Enter_CanSM_GetMutexNoNest(void);
LOCAL_INLINE void SchM_Exit_CanSM_GetMutexNoNest(void);

#define CANSM_STOP_SEC_CODE
#include "CanSM_MemMap.h"


#define CANSM_START_SEC_CODE
#include "CanSM_MemMap.h"

LOCAL_INLINE void SchM_Enter_CanSM_BOR_Nw_ModesNoNest(void)
{
    /* The integrator shall place here the code which would lock the interrupts */
}

LOCAL_INLINE void SchM_Exit_CanSM_BOR_Nw_ModesNoNest(void)
{
    /* The integrator shall place here the code which would unlock the interrupts */
}


LOCAL_INLINE void SchM_Enter_CanSM_BaudModesNoNest(void)
{
    /* The integrator shall place here the code which would lock the interrupts */
}

LOCAL_INLINE void SchM_Exit_CanSM_BaudModesNoNest(void)
{
    /* The integrator shall place here the code which would unlock the interrupts */
}


LOCAL_INLINE void SchM_Enter_CanSM_GetMutexNoNest(void)
{
    /* The integrator shall place here the code which would lock the interrupts */
}

LOCAL_INLINE void SchM_Exit_CanSM_GetMutexNoNest(void)
{
    /* The integrator shall place here the code which would unlock the interrupts */
}

#define CANSM_STOP_SEC_CODE
#include "CanSM_MemMap.h"

#endif /* CANSM_CFG_SCHM_H */



