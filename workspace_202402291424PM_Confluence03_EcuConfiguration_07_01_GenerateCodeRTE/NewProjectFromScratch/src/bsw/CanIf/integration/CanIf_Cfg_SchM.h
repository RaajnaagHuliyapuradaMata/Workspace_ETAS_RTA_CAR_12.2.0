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


#ifndef CANIF_CFG_SCHM_H
#define CANIF_CFG_SCHM_H



#include "SchM_CanIf.h"
#include "CanIf_Cfg.h"
/* It is the integrator who needs to implement the functions in this file. This file is just an example.
Please see the comments and enable the corresponding macros and the inclusion of the header files


// ***************************************************************************************************
 //* Includes
// ***************************************************************************************************


#include "SchM.h"
#include "SchM_Default.h"

// If the RTE code generation is Involved please include the below files /

#if (CANIF_XCORE_CFG_ENABLED != STD_OFF)
#include "OS.h" // to get CoreId, Activate Task and Call Interrupt /


LOCAL_INLINE void Schm_CanIf_XCore_Enter(void);
LOCAL_INLINE void Schm_CanIf_XCore_Exit(void);
LOCAL_INLINE void SCHM_CANIF_NONEST_DISABLE_CORE_LOCAL_IRQ(uint32* previous_state);
LOCAL_INLINE void SCHM_CANIF_NONEST_RESTORE_CORE_LOCAL_IRQ(uint32 previous_state);

// Acquire a global lock, including interrupts deactivated on the local core /
LOCAL_INLINE void Schm_CanIf_XCore_Enter(void)
{
    SCHM_ENTER_DEFAULT();
}
// release a global lock/

LOCAL_INLINE void Schm_CanIf_XCore_Exit(void)
{
    SCHM_EXIT_DEFAULT();
}

// This function activates the given Software Interrupt ID /
#define Schm_CanIf_XCore_Activate_SW_Irq(id) /* not implemented /

//** This function immediately activates the task given as parameter/
#define Schm_CanIf_XCore_Activate_Task(id)  Os_ActivateTask((TaskType)id)

//** This function returns the ID of the current core. Expected result is {0 = Core 0, 1 = Core 1} /
#define Schm_CanIf_XCore_GetCurrentCoreId()  GetCoreID()

//** this function shall deactivate ONLY the IRQs for the local core.
    The implementation is required to be very fast, much faster than the global Schm_Enter_Default()! /
LOCAL_INLINE void SCHM_CANIF_NONEST_DISABLE_CORE_LOCAL_IRQ(uint32* previous_state)
{
    SCHM_ENTER_CORELOCAL_DEFAULT(previous_state);
}

LOCAL_INLINE void SCHM_CANIF_NONEST_RESTORE_CORE_LOCAL_IRQ(uint32 previous_state)
{
    SCHM_EXIT_CORELOCAL_DEFAULT(previous_state);
}


#define SCHM_MULTICORE_OPTIMAL STD_ON //* STD_ON: disables SchM Locks for optimal Configuration according to Multicore /
#else
#define SCHM_MULTICORE_OPTIMAL STD_OFF
#endif

// The integrator shall implement the particular services SchM_Enter and SchM_Exit /

// Since there is currently only one resource used, there is no need to consider the resource name /

LOCAL_INLINE void SchM_Enter_CanIf_TxBufAccessNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_TxBufAccessNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_RxBufAccessNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_RxBufAccessNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_ControllerState(void);
LOCAL_INLINE void SchM_Exit_CanIf_ControllerState(void);

LOCAL_INLINE void SchM_Enter_CanIf_NewMsgRxWkpEvtNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_NewMsgRxWkpEvtNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_TxRxNotifNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_TxRxNotifNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_DynIdNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_DynIdNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_PNPduStatusNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_PNPduStatusNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_TxMuxing(void);
LOCAL_INLINE void SchM_Exit_CanIf_TxMuxing(void);



LOCAL_INLINE void SchM_Enter_CanIf_TxBufAccessNoNest(void)
{
    //* The integrator shall place here the code which would lock the interrupts /
    //* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_TxBufAccessNoNest(void)
{
    // The integrator shall place here the code which would unlock the interrupts /

    // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_RxBufAccessNoNest(void)
{
    // The integrator shall place here the code which would lock the interrupts /

    // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_RxBufAccessNoNest(void)
{
    // The integrator shall place here the code which would unlock the interrupts /

    // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_ControllerState(void)
{
    // The integrator shall place here the code which would lock the interrupts /

    // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_ControllerState(void)
{
    // The integrator shall place here the code which would unlock the interrupts /

   // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_NewMsgRxWkpEvtNoNest(void)
{
    // The integrator shall place here the code which would lock the interrupts /

    // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_NewMsgRxWkpEvtNoNest(void)
{
    // The integrator shall place here the code which would unlock the interrupts /

    // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_TxRxNotifNoNest(void)
{
    // The integrator shall place here the code which would lock the interrupts /

   // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_TxRxNotifNoNest(void)
{
    // The integrator shall place here the code which would unlock the interrupts /

    // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_DynIdNoNest(void)
{
    // The integrator shall place here the code which would lock the interrupts /

   // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_DynIdNoNest(void)
{
    // The integrator shall place here the code which would unlock the interrupts /

   // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_PNPduStatusNoNest(void)
{
    // The integrator shall place here the code which would lock the interrupts /

    // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_PNPduStatusNoNest(void)
{
    // The integrator shall place here the code which would unlock the interrupts /

    // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_TxMuxing(void)
{
    // The integrator shall place here the code which would lock the interrupts /
    //NOTE: This lock shall ensure to disable TX-request and TX-confirmation interrupts since this is used in
     // CanIf_Transmit() and CanIf_TxConfirmation(); else it would affect the TX-multiplexing behaviour./

    // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_TxMuxing(void)
{
    // The integrator shall place here the code which would unlock the interrupts /

    // DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core /
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}
*/
/* Below are examplem for ETAS integration use case so that it compilation will be through when the template file is converted to .h file , but the values and the function calls have to modified to work functionally. See the comments above for example*/
#if (CANIF_XCORE_CFG_ENABLED != STD_OFF)

LOCAL_INLINE void Schm_CanIf_XCore_Enter(void);
LOCAL_INLINE void Schm_CanIf_XCore_Exit(void);
LOCAL_INLINE void SCHM_CANIF_NONEST_DISABLE_CORE_LOCAL_IRQ(uint32* previous_state);
LOCAL_INLINE void SCHM_CANIF_NONEST_RESTORE_CORE_LOCAL_IRQ(uint32 previous_state);

/** Acquire a global lock, including interrupts deactivated on the local core */
LOCAL_INLINE void Schm_CanIf_XCore_Enter(void)
{
    /* The integrator shall place his/her code here which would disable interrupt or acquire lock */
}


LOCAL_INLINE void Schm_CanIf_XCore_Exit(void)
{
    /* The integrator shall place his/her code here which would enable interrupt or release lock */
}

/** This function activates the given Software Interrupt ID */
#define Schm_CanIf_XCore_Activate_SW_Irq(id) /* not implemented */

/** This function immediately activates the task given as parameter*/
#define Schm_CanIf_XCore_Activate_Task(id)  Os_ActivateTask((TaskType)id)

/** This function returns the ID of the current core. Expected result is {0 = Core 0, 1 = Core 1} */
#define Schm_CanIf_XCore_GetCurrentCoreId()  GetCoreID()

/** this function shall deactivate ONLY the IRQs for the local core.
    The implementation is required to be very fast, much faster than the global Schm_Enter_Default()! */
LOCAL_INLINE void SCHM_CANIF_NONEST_DISABLE_CORE_LOCAL_IRQ(uint32* previous_state)
{
    /* The integrator shall place his/her code here which would disable interrupt or acquire lock */
}

LOCAL_INLINE void SCHM_CANIF_NONEST_RESTORE_CORE_LOCAL_IRQ(uint32 previous_state)
{
    /* The integrator shall place his/her code here which would enable interrupt or release lock */
}


#define SCHM_MULTICORE_OPTIMAL STD_ON /* STD_ON: disables SchM Locks for optimal Configuration according to Multicore */
#else
#define SCHM_MULTICORE_OPTIMAL STD_OFF
#endif

/* The integrator shall implement the particular services SchM_Enter and SchM_Exit */

/* Since there is currently only one resource used, there is no need to consider the resource name */

LOCAL_INLINE void SchM_Enter_CanIf_TxBufAccessNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_TxBufAccessNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_RxBufAccessNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_RxBufAccessNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_ControllerState(void);
LOCAL_INLINE void SchM_Exit_CanIf_ControllerState(void);

LOCAL_INLINE void SchM_Enter_CanIf_NewMsgRxWkpEvtNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_NewMsgRxWkpEvtNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_TxRxNotifNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_TxRxNotifNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_DynIdNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_DynIdNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_PNPduStatusNoNest(void);
LOCAL_INLINE void SchM_Exit_CanIf_PNPduStatusNoNest(void);

LOCAL_INLINE void SchM_Enter_CanIf_TxMuxing(void);
LOCAL_INLINE void SchM_Exit_CanIf_TxMuxing(void);



LOCAL_INLINE void SchM_Enter_CanIf_TxBufAccessNoNest(void)
{
    /* The integrator shall place his/her code here which would disable interrupt or acquire lock */

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_TxBufAccessNoNest(void)
{
    /* The integrator shall place his/her code here which would enable interrupt or release lock */

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_RxBufAccessNoNest(void)
{
    /* The integrator shall place his/her code here which would disable interrupt or acquire lock */

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_RxBufAccessNoNest(void)
{
    /* The integrator shall place his/her code here which would enable interrupt or release lock */

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_ControllerState(void)
{
    /* The integrator shall place his/her code here which would disable interrupt or acquire lock */

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_ControllerState(void)
{
    /* The integrator shall place his/her code here which would enable interrupt or release lock */

   /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_NewMsgRxWkpEvtNoNest(void)
{
    /* The integrator shall place his/her code here which would disable interrupt or acquire lock */

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_NewMsgRxWkpEvtNoNest(void)
{
    /* The integrator shall place his/her code here which would enable interrupt or release lock */

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_TxRxNotifNoNest(void)
{
    /* The integrator shall place his/her code here which would disable interrupt or acquire lock */

   /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_TxRxNotifNoNest(void)
{
    /* The integrator shall place his/her code here which would enable interrupt or release lock */

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_DynIdNoNest(void)
{
    /* The integrator shall place his/her code here which would disable interrupt or acquire lock */

   /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_DynIdNoNest(void)
{
    /* The integrator shall place his/her code here which would enable interrupt or release lock */

   /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_PNPduStatusNoNest(void)
{
    /* The integrator shall place his/her code here which would disable interrupt or acquire lock */

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_PNPduStatusNoNest(void)
{
    /* The integrator shall place his/her code here which would enable interrupt or release lock */

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}


LOCAL_INLINE void SchM_Enter_CanIf_TxMuxing(void)
{
    /* The integrator shall place his/her code here which would disable interrupt or acquire lock */

    /*NOTE: This lock shall ensure to disable TX-request and TX-confirmation interrupts since this is used in
     * CanIf_Transmit() and CanIf_TxConfirmation(); else it would affect the TX-multiplexing behaviour.*/

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}

LOCAL_INLINE void SchM_Exit_CanIf_TxMuxing(void)
{
    /* The integrator shall place his/her code here which would enable interrupt or release lock */

    /* DD: not needed since the Multicore project uses only Polling configuration and the CAN drv operations are limited to one core */
#if (!defined SCHM_MULTICORE_OPTIMAL) || (SCHM_MULTICORE_OPTIMAL != STD_ON)

#endif
}
#endif /* CANIF_CFG_SCHM_H */



