/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2019, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

#ifndef RBA_MEMLIB_CFG_H
#define RBA_MEMLIB_CFG_H


/* *********************************************************************************************************************
 * WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING
 *
 * ENABLED PROTOTYPE FUNCTIONALITY - THIS CONFIGURATION IS NOT ALLOWED TO BE USED FOR SERIES PRODUCTION!
 *
 * *********************************************************************************************************************
*/

/*********************************************************************************************************************
 * Macros/Defines
 *********************************************************************************************************************
*/

#define RBA_MEMLIB_SW_MAJOR_VERSION                          (22u)
#define RBA_MEMLIB_SW_MINOR_VERSION                          (0u)
#define RBA_MEMLIB_SW_PATCH_VERSION                          (1u)

#define RBA_MEMLIB_AR_RELEASE_MAJOR_VERSION                  (4u)
#define RBA_MEMLIB_AR_RELEASE_MINOR_VERSION                  (5u)
#define RBA_MEMLIB_AR_RELEASE_REVISION_VERSION               (0u)


/* Prototype configuration: ENABLED PROTOTYPE FUNCTIONALITY - do not use for series production */
#define RBA_MEMLIB_CFG_ENABLE_PROTOTYPE

/* Activation of VECU handling */
#if defined(RBA_VECU_ENABLED)
#define  RBA_MEMLIB_VECU_ENABLE                              (STD_ON)
#endif

/*Pre-processor switch to enable or disable the API to read out the modules version information.*/
#define RBA_MEMLIB_VERSION_INFO                              (STD_OFF)

/* Activation of Crc support */
#define RBA_MEMLIB_CRC_ENABLE                                (STD_ON)

/* Activation of bypass for memory operations (to be added via callout-header) */
#define RBA_MEMLIB_BYPASS_MEM_OPERATIONS                     (STD_OFF)

/* Use code-size versus speed optimization */
#define RBA_MEMLIB_CODE_SIZE_OPTIMIZED                       (STD_OFF)

/* Activation of a special modulo oeration */
#define RBA_MEMLIB_USE_SPECIAL_MODULO                        (STD_OFF)

/* Activation of timer usage (to be added via callout-header) */
#define RBA_MEMLIB_USE_TIMER                                 (STD_OFF)

/* Activation of MemAcc build/usage */
#define RBA_MEMLIB_MEMACC_BUILD_ENABLE                       (STD_OFF)
#define RBA_MEMLIB_MEMACC_USAGE_ENABLE                       (STD_OFF)

/* Activation of Measurement support */
#define RBA_MEMLIB_MEAS_ENABLE                               (STD_OFF)

/* Size of the enum variables (in bytes) */
#define RBA_MEMLIB_TARGET_SIZE_ENUM_8                        (4u)
#define RBA_MEMLIB_TARGET_SIZE_ENUM_16                       (4u)

#if defined(RBA_VECU_ENABLED)
#define RBA_MEMLIB_SIZE_ENUM_8                               (4u)
#define RBA_MEMLIB_SIZE_ENUM_16                              (4u)
#else
#define RBA_MEMLIB_SIZE_ENUM_8                               RBA_MEMLIB_TARGET_SIZE_ENUM_8
#define RBA_MEMLIB_SIZE_ENUM_16                              RBA_MEMLIB_TARGET_SIZE_ENUM_16
#endif

#endif
