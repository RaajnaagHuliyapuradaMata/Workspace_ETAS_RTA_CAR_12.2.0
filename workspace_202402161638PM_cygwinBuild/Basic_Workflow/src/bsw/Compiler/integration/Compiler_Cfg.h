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

#define NOT_READY_FOR_TESTING_OR_DEPLOYMENT
#ifndef NOT_READY_FOR_TESTING_OR_DEPLOYMENT
# error The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#else
# warning The content of this file is a template which provides empty stubs. The content of this file must be completed by the integrator accordingly to project specific requirements
#endif /* NOT_READY_FOR_TESTING_OR_DEPLOYMENT */
/*---- Removal end ----*/

#ifndef COMPILER_CFG_H
#define COMPILER_CFG_H

/*
 **********************************************************************************************************************
 * Based on AUTOSAR_SWS_CompilerAbstraction.pdf; AR4.2; Rev 2 and BSW_CodingGuidelines_Cur_v.1.10.pdf
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Include
 **********************************************************************************************************************
 */
#include "Os_Compiler_Cfg.h"


/*
 **********************************************************************************************************************
 * Types and Defines
 **********************************************************************************************************************
 */

/* TRACE[Rule BSW_VersionInfo_002]: Module Vendor Identifier. */
/* No special module ID is defined for Compiler_Cfg header. COMPILER_CFG is chosen. */
#define COMPILER_CFG_VENDOR_ID 6

/* TRACE[Rule BSW_VersionInfo_004]: Software Version Number and AUTOSAR Specification Version Number. */
#define COMPILER_CFG_SW_MAJOR_VERSION 1
#define COMPILER_CFG_SW_MINOR_VERSION 0
#define COMPILER_CFG_SW_PATCH_VERSION 0
#define COMPILER_CFG_AR_RELEASE_MAJOR_VERSION 4
#define COMPILER_CFG_AR_RELEASE_MINOR_VERSION 2
#define COMPILER_CFG_AR_RELEASE_REVISION_VERSION 2

/*******************************************************************************
**                      RB specific Macros                                    **
*******************************************************************************/
/* COMPILER_RB_PARAMUNUSED to flag unused parameters */
#ifndef COMPILER_RB_PARAMUNUSED
#define COMPILER_RB_PARAMUNUSED(x) ((void)(x))
#endif


/* COMPILER_RB_GCC_VERSION is defined (other than 0) if GNU GCC compiler is used */
#ifdef __GNUC__
#  define COMPILER_RB_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#else
#  define COMPILER_RB_GCC_VERSION (0)
#endif

/* COMPILER_RB_NORETURN to flag non-returning functions  */
/*RTA-BSW SW should independent of Compiler dependencies, hence the removed the GCC version
 * dependency, Currently there is no usage COMPILER_RB_NORETURN within BSW */

/*#ifndef COMPILER_RB_NORETURN
#  if (COMPILER_RB_GCC_VERSION >= 20500)
#    define COMPILER_RB_NORETURN __attribute__ ((__noreturn__))
#  else
#    define COMPILER_RB_NORETURN
#  endif
#endif
*/

#define COMPILER_RB_NORETURN

/* COMPILER_RB_STATICASSERTGLOBAL for simulating static assertions in global scope    */
/* \param cond shall be a statically evaluable conditional-expression                 */
/* \param msg  shall be an identifier specifying the kind of static condition check   */
#ifndef COMPILER_RB_ASSERT_GLOBAL
#define COMPILER_RB_ASSERT_GLOBAL(cond, msg)  extern const uint8 Compiler_Rb_AssertGlobal_##msg[(cond)?1:-1];
#endif

/* COMPILER_RB_STATICASSERTLOCAL for simulating static assertions in local scope      */
/* \param cond shall be a statically evaluable conditional-expression                 */
/* \param msg  shall be an identifier specifying the kind of static condition check   */
/* MISRA RULE 19.4 VIOLATION: This statement block is used to simulate static assertions. */
#ifndef COMPILER_RB_ASSERT_LOCAL
#define COMPILER_RB_ASSERT_LOCAL(cond, msg)                        \
{                                                                  \
    static const uint8 Compiler_Rb_AssertLocal_##msg[(cond)?1:-1]; \
    (void)Compiler_Rb_AssertLocal_##msg[0];                        \
}
#endif

/*
 **********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************
 */
/*TRACE[SWS_COMPILER_00040] */
/*<PREFIX> is composed according <snp>[_<vi>_<ai>] for basic software modules*/
/*
 * <snp> is the Section Name Prefix which shall be the BswModuleDescription’s shortName converted in upper case letters
 * if no SectionNamePrefix is defined for the MemorySection in the Basic Software Module Description or
 * Software Component Description.
 */
/*
 * <snp> shall be the symbol of the Section NamePrefix associated to the MemorySection
 * if a SectionNamePrefix is defined for the MemorySection.
 */
/*<vi> is the vendorId of the BSW module*/
/*<ai> is the vendorApiInfix of the BSW module*/
/*
 * The sub part in squared brackets [_<vi>_<ai>] is omitted if no vendorApiInfix is defined for the
 * Basic Software Module which indicates that it does not use multiple instantiation.
 */

/*<PREFIX> is the shortName of the software component type for software components (case sensitive)*/

/* To be used for code */
/*
 * PERIOD is the typical period time value and unit of the ExecutableEntitys in this MemorySection.
 * The name part [_<PERIOD>] is optional.units are:
 * US microseconds
 * MS milli second
 * S second
 * For example: 100US, 400US, 1MS, 5MS, 10MS, 20MS, 100MS, 1S
 * Please note that deviations from this typical period time are possible due to
 * integration decisions (e.g. RTEEvent To Task Mapping). Further, in special modes of the ECU the
 * code may be scheduled with a higher or lower period.
 */
//#define <PREFIX>_CODE[_<PERIOD>]

/* To be used for callout code.*/
/*<CN> is the callback name (including module reference) written in uppercase letters.*/
//#define <PREFIX>_<CN>_CODE

/*
 * To be used for code that shall go into fast code memory segments.
 * The FAST sections should be used when the execution does not happen in a welldefined period time but with
 * the knowledge of high frequent access and /or high execution time,
 * for example, a callback for a frequent notification.
 */
//#define <PREFIX>_CODE_FAST

/*
 * To be used for code that shall go into slow code memory segments.
 * The SLOW sections should be used when the execution does not happen in a welldefined period time but
 * with the knowledge of low frequent access, for example, a callback in case of seldom error.
 */
//#define <PREFIX>_CODE_SLOW

/* To be used for global or static constants */
//#define <PREFIX>_CONST

/* To be used for calibration constants. */
//#define <PREFIX>_CALIB

/* To be used for module configuration constants. */
//#define <PREFIX>_CONFIG_DATA


/* To be used for references on application data (expected to be in RAM or ROM) passed via API */
//#define <PREFIX>_APPL_DATA

/*
 * To be used for references on application constants (expected to be certainly in ROM, for instance pointer of
 * Initfunction) passed via API
 */
//#define <PREFIX>_APPL_CONST

/* To be used for pointers to registers (e.g. static volatile CONSTP2VAR(uint16, PWM_CONST, REGSPACE)). */
//#define REGSPACE

/*
 * <INIT_POLICY> is the initialization policy of variables. Possible values are:
 * NO_INIT: Used for variables that are never cleared and never initialized.
 * CLEARED: Used for variables that are cleared to zero after every reset.
 * POWER_ON_CLEARED: Used for variables that are cleared to zero only after power on reset.
 * INIT: Used for variables that are initialized with values after every reset.
 * POWER_ON_INIT: Used for variables that are initialized with values only after power on reset.
 */

/* To be used for all global or static variables. */
//#define <PREFIX>_VAR_<INIT_POLICY>

/*
 * To be used for all global or static variables that have at least accessed bitwise or frequently used or
 * high number of accesses in source code
 * Some platforms allow the use of bit instructions for variables located in this specific RAM area as well as shorter
 * addressing instructions. This saves code and runtime.
 */
//#define <PREFIX>_VAR_FAST_<INIT_POLICY>

/* To be used for all infrequently accessed global or static variables. */
//#define <PREFIX>_VAR_SLOW_<INIT_POLICY>

/* To be used for global or static variables which are accessible from a calibration tool. */
//#define <PREFIX>_INTERNAL_VAR_<INIT_POLICY>

#define AUTOSAR_COMSTACKDATA
#define CANDRV_CODE
#define CANDRV_CONST
#define CANDRV_APPL_DATA
#define CANDRV_APPL_CONST
#define CANDRV_APPL_CODE
#define CANDRV_CALLOUT_CODE
#define CANDRV_VAR_NOINIT
#define CANDRV_VAR_POWER_ON_INIT
#define CANDRV_VAR_FAST
#define CANDRV_VAR

#define CANDRV_CFG_CONST

#define CANTRCV_CODE
#define CANTRCV_CONST
#define CANTRCV_APPL_DATA
#define CANTRCV_APPL_CONST
#define CANTRCV_APPL_CODE
#define CANTRCV_CALLOUT_CODE
#define CANTRCV_VAR_NOINIT
#define CANTRCV_VAR_POWER_ON_INIT
#define CANTRCV_VAR_FAST
#define CANTRCV_VAR

#define CANIF_CODE
#define CANIF_CONST
#define CANIF_APPL_DATA
#define CANIF_APPL_CONST
#define CANIF_APPL_CODE
#define CANIF_CALLOUT_CODE
#define CANIF_VAR_NOINIT
#define CANIF_VAR_POWER_ON_INIT
#define CANIF_VAR_FAST
#define CANIF_VAR

#define CANNM_CODE
#define CANNM_CONST
#define CANNM_APPL_DATA
#define CANNM_APPL_CONST
#define CANNM_APPL_CODE
#define CANNM_CALLOUT_CODE
#define CANNM_VAR_NOINIT
#define CANNM_VAR_POWER_ON_INIT
#define CANNM_VAR_FAST
#define CANNM_VAR

#define CANSM_CODE
#define CANSM_CONST
#define CANSM_APPL_DATA
#define CANSM_APPL_CONST
#define CANSM_APPL_CODE
#define CANSM_CALLOUT_CODE
#define CANSM_VAR_NOINIT
#define CANSM_VAR_POWER_ON_INIT
#define CANSM_VAR_FAST
#define CANSM_VAR

#define CANTP_CODE
#define CANTP_CONST
#define CANTP_APPL_DATA
#define CANTP_APPL_CONST
#define CANTP_APPL_CODE
#define CANTP_CALLOUT_CODE
#define CANTP_VAR_NOINIT
#define CANTP_VAR_POWER_ON_INIT
#define CANTP_VAR_FAST
#define CANTP_VAR
#define CanTp_APPL_DATA


#define FR_CODE
#define FR_CONST
#define FR_APPL_DATA
#define FR_APPL_CONST
#define FR_APPL_CODE
#define FR_CALLOUT_CODE
#define FR_VAR_NOINIT
#define FR_VAR_POWER_ON_INIT
#define FR_VAR_FAST
#define FR_VAR

#define FRTRCV_CODE
#define FRTRCV_CONST
#define FRTRCV_APPL_DATA
#define FRTRCV_APPL_CONST
#define FRTRCV_APPL_CODE
#define FRTRCV_CALLOUT_CODE
#define FRTRCV_VAR_NOINIT
#define FRTRCV_VAR_POWER_ON_INIT
#define FRTRCV_VAR_FAST
#define FRTRCV_VAR

#define FRIF_CODE
#define FRIF_CONST
#define FRIF_APPL_DATA
#define FRIF_APPL_CONST
#define FRIF_APPL_CODE
#define FRIF_CALLOUT_CODE
#define FRIF_VAR_NOINIT
#define FRIF_VAR_POWER_ON_INIT
#define FRIF_VAR_FAST
#define FRIF_VAR

#define FRNM_CODE
#define FRNM_CONST
#define FRNM_APPL_DATA
#define FRNM_APPL_CONST
#define FRNM_APPL_CODE
#define FRNM_CALLOUT_CODE
#define FRNM_VAR_NOINIT
#define FRNM_VAR_POWER_ON_INIT
#define FRNM_VAR_FAST
#define FRNM_VAR


#define FRSM_CODE
#define FRSM_CONST
#define FRSM_APPL_DATA
#define FRSM_APPL_CONST
#define FRSM_APPL_CODE
#define FRSM_CALLOUT_CODE
#define FRSM_VAR_NOINIT
#define FRSM_VAR_POWER_ON_INIT
#define FRSM_VAR_FAST
#define FRSM_VAR


#define FRTP_CODE
#define FRTP_CONST
#define FRTP_APPL_DATA
#define FRTP_APPL_CONST
#define FRTP_APPL_CODE
#define FRTP_CALLOUT_CODE
#define FRTP_VAR_NOINIT
#define FRTP_VAR_POWER_ON_INIT
#define FRTP_VAR_FAST
#define FRTP_VAR
#define FRTP_VAR_INIT
#define FRTP_VAR_NO_INIT


#define IPDUM_CODE
#define IPDUM_CONST
#define IPDUM_APPL_DATA
#define IPDUM_APPL_CONST
#define IPDUM_APPL_CODE
#define IPDUM_APPL_VAR
#define IPDUM_CALLOUT_CODE
#define IPDUM_VAR_NOINIT
#define IPDUM_VAR_POWER_ON_INIT
#define IPDUM_VAR_FAST
#define IPDUM_VAR

#define LIN_CODE
#define LIN_CONST
#define LIN_APPL_DATA
#define LIN_APPL_CONST
#define LIN_APPL_CODE
#define LIN_CALLOUT_CODE
#define LIN_VAR_NOINIT
#define LIN_VAR_POWER_ON_INIT
#define LIN_VAR_FAST
#define LIN_VAR

#define LINIF_CODE
#define LINIF_VAR_FAST
#define LINIF_VAR
#define LINIF_CONST
#define LINIF_APPL_DATA
#define LINIF_APPL_CONST

#define LINSM_CODE
#define LINSM_CONST
#define LINSM_APPL_DATA
#define LINSM_APPL_CONST
#define LINSM_APPL_CODE
#define LINSM_CALLOUT_CODE
#define LINSM_VAR_NOINIT
#define LINSM_VAR_POWER_ON_INIT
#define LINSM_VAR_FAST
#define LINSM_VAR

#define LINTRCV_CODE
#define LINTRCV_CONST
#define LINTRCV_APPL_DATA
#define LINTRCV_APPL_CONST
#define LINTRCV_APPL_CODE
#define LINTRCV_CALLOUT_CODE
#define LINTRCV_VAR_NOINIT
#define LINTRCV_VAR_POWER_ON_INIT
#define LINTRCV_VAR_FAST
#define LINTRCV_VAR


#define COM_CODE
#define COM_CONST
#define COM_APPL_DATA
#define COM_APPL_CONST
#define COM_APPL_CODE
#define COM_CALLOUT_CODE
#define COM_VAR_NOINIT
#define COM_VAR_POWER_ON_INIT
#define COM_VAR_FAST
#define COM_VAR

#define BSWM_CODE
#define BSWM_VAR
#define BSWM_CONST
#define BSWM_APPL_DATA
#define BSWM_APPL_CONST


#define COMM_CODE
#define COMM_CONST
#define COMM_APPL_DATA
#define COMM_APPL_CONST
#define COMM_APPL_CODE
#define COMM_CALLOUT_CODE
#define COMM_VAR_NOINIT
#define COMM_VAR_POWER_ON_INIT
#define COMM_VAR_FAST
#define COMM_VAR

#define NM_CODE
#define NM_CONST
#define NM_APPL_DATA
#define NM_APPL_CONST
#define NM_APPL_CODE
#define NM_CALLOUT_CODE
#define NM_VAR_NOINIT
#define NM_VAR_POWER_ON_INIT
#define NM_VAR_FAST
#define NM_VAR

#define PDUR_CODE
#define PDUR_CONST
#define PDUR_APPL_DATA
#define PDUR_APPL_CONST
#define PDUR_APPL_CODE
#define PDUR_CALLOUT_CODE
#define PDUR_VAR_NOINIT
#define PDUR_VAR_POWER_ON_INIT
#define PDUR_VAR_FAST
#define PDUR_VAR

#define DCM_CODE
#define DCM_CONST
#define DCM_APPL_DATA
#define DCM_APPL_CONST
#define DCM_APPL_CODE
#define DCMAPPL_CODE
#define DCM_CALLOUT_CODE
#define DCM_VAR_NOINIT
#define DCM_VAR_POWER_ON_INIT
#define DCM_VAR_FAST
#define DCM_VAR
#define DCM_INTERN_DATA
#define DCM_INTERN_CONST

#define DET_CODE
#define DET_LIBCODE
#define DET_CONST
#define DET_APPL_DATA
#define DET_DATA
#define DET_APPL_CONST
#define DET_APPL_CODE
#define DET_CALLOUT_CODE
#define DET_VAR_NOINIT
#define DET_VAR_POWER_ON_INIT
#define DET_VAR_FAST
#define DET_VAR

#define FIM_CODE
#define FIM_LIBCODE
#define FIM_CONST
#define FIM_APPL_DATA
#define FIM_DATA
#define FIM_APPL_CONST
#define FIM_APPL_CODE
#define FIM_CALLOUT_CODE
#define FIM_VAR_NOINIT
#define FIM_VAR_POWER_ON_INIT
#define FIM_VAR_FAST
#define FIM_VAR

#define STBM_CODE
#define STBM_LIBCODE
#define STBM_CONST
#define STBM_APPL_DATA
#define STBM_DATA
#define STBM_APPL_CONST
#define STBM_APPL_CODE
#define STBM_CALLOUT_CODE
#define STBM_VAR_NOINIT
#define STBM_VAR_POWER_ON_INIT
#define STBM_VAR_FAST
#define STBM_VAR

#define RTE_CODE
#define RTE_LIBCODE
#define RTE_CONST
#define RTE_APPL_DATA
#define RTE_DATA
#define RTE_APPL_CONST
#define RTE_APPL_CODE
#define RTE_CALLOUT_CODE
#define RTE_VAR_NOINIT
#define RTE_VAR_POWER_ON_INIT
#define RTE_VAR_FAST
#define RTE_VAR


#define Swc1_CODE
#define Swc2_CODE
#define Swc3_CODE
#define Swc_NvMTest_CODE

#define NvM_CODE
#define NvM_CONST
#define NvM_APPL_DATA
#define NvM_APPL_CONST
#define NvM_APPL_CODE
#define NvM_CALLOUT_CODE
#define NvM_VAR_NOINIT
#define NvM_VAR_POWER_ON_INIT
#define NvM_VAR_FAST
#define NvM_VAR

#define MEMIF_CODE

#define FEE_CODE
#define FEE_CONST
#define FEE_APPL_DATA
#define FEE_APPL_CONST
#define FEE_APPL_CODE
#define FEE_CALLOUT_CODE
#define FEE_VAR_NOINIT
#define FEE_VAR_POWER_ON_INIT
#define FEE_VAR_FAST
#define FEE_VAR

#define FLS_CODE
#define FLS_CONST
#define FLS_APPL_DATA
#define FLS_APPL_CONST
#define FLS_APPL_CODE
#define FLS_CALLOUT_CODE
#define FLS_VAR_NOINIT
#define FLS_VAR_POWER_ON_INIT
#define FLS_VAR_FAST
#define FLS_VAR

#define EcuM_CODE
#define EcuM_CONST
#define EcuM_APPL_DATA
#define EcuM_APPL_CONST
#define EcuM_APPL_CODE
#define EcuM_CALLOUT_CODE
#define EcuM_VAR_NOINIT
#define EcuM_VAR_POWER_ON_INIT
#define EcuM_VAR_FAST
#define EcuM_VAR

#define ECUM_CODE
#define ECUM_CONST
#define ECUM_APPL_DATA
#define ECUM_APPL_CONST
#define ECUM_APPL_CODE
#define ECUM_CALLOUT_CODE
#define ECUM_VAR_NOINIT
#define ECUM_VAR_POWER_ON_INIT
#define ECUM_VAR_FAST
#define ECUM_VAR

#define EA_CODE
#define EA_CONST
#define EA_APPL_DATA
#define EA_APPL_CONST
#define EA_APPL_CODE
#define EA_CALLOUT_CODE
#define EA_VAR_NOINIT
#define EA_VAR_POWER_ON_INIT
#define EA_VAR_FAST
#define EA_VAR

#define EEP_CODE
#define EEP_CONST
#define EEP_APPL_DATA
#define EEP_APPL_CONST
#define EEP_APPL_CODE
#define EEP_CALLOUT_CODE
#define EEP_VAR_NOINIT
#define EEP_VAR_POWER_ON_INIT
#define EEP_VAR_FAST
#define EEP_VAR

#define RBA_TRCV_CODE
#define RBA_TRCV_CONST
#define RBA_TRCV_APPL_DATA
#define RBA_TRCV_APPL_CONST
#define RBA_TRCV_APPL_CODE
#define RBA_TRCV_CALLOUT_CODE
#define RBA_TRCV_VAR_NOINIT
#define RBA_TRCV_VAR_POWER_ON_INIT
#define RBA_TRCV_VAR_FAST
#define RBA_TRCV_VAR
#define RBA_TRCV_PBCFG
#define RBA_TRCV_CFG

#define CSM_CODE
#define CRYPTOBCL_CODE
#define CRYPTOHSM_CODE


/* SBTLwiRte: Memory mapping sections */
#define rba_SBTLwiRte_User_Client_CODE
#define rba_SBTLwiRte_CmdTrigger_CODE
#define rba_SBTLwiRte_TestEngine_CODE
#define rba_SBTLwiRte_NvM_Client_CODE
#define rba_SBTLwiRte_BswM_Client_CODE
#define rba_SBTLwiRte_ComM_Client_CODE
#define rba_SBTLwiRte_Dem_Client_CODE
#define rba_SBTLwiRte_Dcm_Client_CODE
#define Dem_Client_SWCD_CODE
#define NvM_Client_SWCD_CODE
#define NvM_CODE

/* -------------------------------------------------------------------------- */
/*                                    WDGM                                    */
/* -------------------------------------------------------------------------- */

#define WDGM_CODE               /* module functions         */
#define WDGM_VAR_NOINIT         /* uninitialized variables */
#define WDGM_VAR_FAST           /* variables requiring bitwise or frequent access */
#define WDGM_VAR                /* global/static variables initialized after reset */
#define WDGM_CONST              /* module constants            */
#define WDGM_APPL_DATA          /* pointers to application data passed via API */
#define WDGM_APPL_CONST         /* application constants passed via API */
#define WDGM_APPL_CODE          /* pointers to application functions - callbacks */
#define WDGM_FAR                /* Data provided by another layer. COM never modifies the
                                   data, but the data may be in either ROM or RAM */

#endif /* COMPILER_CFG_H */
