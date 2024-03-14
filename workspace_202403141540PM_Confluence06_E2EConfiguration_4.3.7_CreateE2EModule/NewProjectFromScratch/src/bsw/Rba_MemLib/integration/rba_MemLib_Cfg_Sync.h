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

#ifndef RBA_MEMLIB_CFG_SYNC_H
#define RBA_MEMLIB_CFG_SYNC_H

/**
 **********************************************************************************************************************
 * \file   rba_MemLib_Cfg_Sync.h
 * \brief  Integration code for access to memory-pipeline synchronization (details in Module Docu)
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
// proposal:    #include "rba_BswSrv.h"


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
 */

/* To be defined by the integrator
 * *******************************
 * CPU and compiler dependent write memory barrier used to ensure that previous writes
 * are performed before this instruction.
 * Typically this is an instruction which is provided by intrinsic functions of the compiler
 * or some special service functions.
 * Note if no multi-core is used, then this macro can be defined "empty".
 */
/* proposal:
#if defined(RBA_BSWSRV_MSYNC)
#define RBA_MEMLIB_MSYNC()     RBA_BSWSRV_MSYNC()
#elif defined(RBA_BSWSRV_IFX_DSYNC)
#define RBA_MEMLIB_MSYNC()     RBA_BSWSRV_IFX_DSYNC()
#elif defined(RBA_BSWSRV_IFX_ARM_MSYNC)
#define RBA_MEMLIB_MSYNC()     RBA_BSWSRV_IFX_ARM_MSYNC()
#elif defined(RBA_BSWSRV_JDP_MSYNC)
#define RBA_MEMLIB_MSYNC()     RBA_BSWSRV_JDP_MSYNC()
#else
#define RBA_MEMLIB_MSYNC()
#endif
*/

/*---- Removal start ----*/
#warning The content of this file is a template which provides empty stubs. Remove this after completed implementation.

#define RBA_MEMLIB_MSYNC()
/*---- Removal end ----*/


#endif
