/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2023, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/


#ifndef RBA_BSWSRV_CFG_H
#define RBA_BSWSRV_CFG_H

/*
 **********************************************************************************************************************
 * This file is for configuration of component rba_BswSrv
 **********************************************************************************************************************
 */
 

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

/* Compatibility configuration not configured. */

/* 64BIT Operations like memset_64, memcpy_64 and memcompare_64 can enable or disable*/

#define RBA_BSWSRV_CFG_MACHINE_SUPPORT_64_BIT              (STD_OFF) 

/* Activate the following #define ONLY for Compiler Library Optimisation. */
#define RBA_BSWSRV_CFG_COMPILER_LIBRARY_SUPPORT            (STD_OFF) 


/* Activate the following #define RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT ONLY for BootCtrl, or possibly CB projects. This Removes all interrupt suspend and spinlock interfaces. */
/* Lock support Disabled */
#define RBA_BSWSRV_CFG_DISABLE_LOCK_SUPPORT 









/* RBA_BSWSRV_CFG_H */

#endif


